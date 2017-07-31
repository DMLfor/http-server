#include "handle_request.h"
#include "epoll.h"
#include "config.h"
#include "log.h"
#include "filebuf.h"
//主线程中已经读取了的配置数据
extern struct http_conf_t conf;
extern log_thread_t log_thread;
//任务线程的入口，每个线程回不断的执行epoll_wait并且,根据
//触发的任务调用对应的函数.
void* wait_task(void *arg)
{
    int epfd = *(int *)arg;
    struct epoll_event *evlist = NULL;

    evlist = (struct epoll_event *)malloc(sizeof(struct epoll_event) * conf.evlist_num);
    if(evlist == NULL)
    {
        perror("malloc evlist fail");
        exit(EXIT_FAILURE);
    }

    int n, i;
    while(1)
    {
        if((n = epoll_wait(epfd, evlist, conf.evlist_num - 1, -1)) < 0)
        {
            push_log(errno, ": epoll wait fail\n\n", 2, &log_thread);
            continue;
        }
        for(i = 0; i<n; i++)
        {
            http_request_t *ptr = (http_request_t *)evlist[i].data.ptr;
            if(evlist[i].events == EPOLLIN)
            {
                http_recv(ptr, epfd);
            }
            else if(evlist[i].events == EPOLLOUT)
            {
                if(http_send(ptr, epfd) == -1)
                {
                    push_log(errno, ": return 405\n\n", 0, &log_thread);
                }
                else
                {
                    push_log(errno, ": return 200\n\n", 0, &log_thread);
                }
            }
        }
    }
    free(evlist);
}

//接受一个http请求
void http_recv(http_request_t *http_ptr, int epfd)
{
    char header[10240];
    size_t n;
    int alive = 0;
    int sockfd = http_ptr->fd;

    n = recv(sockfd, header, 10240, 0);

    if(n < 0)
    {
        push_log(errno, ": recv header fail\n\n", 2, &log_thread);
        free(http_ptr);
        close(sockfd);
    }
    else if(n == 0)
    {
        push_log(errno, ": connect close\n\n", 0, &log_thread);
        free(http_ptr);
        close(sockfd);
    }
    else
    {
        alive = get_url_method(header, http_ptr->url, http_ptr->method, n);
        http_ptr->alive = alive;
        epoll_mod(epfd, http_ptr, EPOLLOUT);
    }

}

//发送一个http回复,200OK返回0, 405返回-1
int http_send(http_request_t *http_ptr, int epfd)
{
    size_t url_len, i, mark = 0;
    char path[128];

    if(strcasecmp(http_ptr->method, "GET") != 0)
    {
        write_405(http_ptr);
        check_alive(http_ptr, epfd);
        return -1;
    }

    url_len = strlen(http_ptr->url);
    for(i = 1; i<url_len; i++)
    {
        if(http_ptr->url[i] == '.' && http_ptr->url[i-1] == '.')
        {
            write_405(http_ptr);
            check_alive(http_ptr, epfd);
            return -1;
        }
    }

    strcpy(path, conf.docroot);
    strcat(path, http_ptr->url);

    if(path[strlen(path)-1] == '/')
        strcat(path, "index.html");

    mark = write_data(http_ptr, path);
    check_alive(http_ptr, epfd);

    return mark;
}

//检查是否keep-alive并处理
void check_alive(http_request_t *http_ptr, int epfd)
{
    int alive = http_ptr->alive;
    if(alive)
    {
        epoll_mod(epfd, http_ptr, EPOLLIN);
    }
    else
    {
        close(http_ptr->fd);
        free(http_ptr);
    }
}

//从http请求抱头中获得 方法, url, 返回值为alive类型
int get_url_method(const char *header, char *url, char *method, size_t size)
{
    size_t i = 0, j = 0;
    int alive = 0;
    while(header[i] != ' ' && i < 128)
    {
        method[i] = header[i];
        i++;
    }
    method[i] = '\0';

    if(strcasestr(header, "keep-alive") != NULL)
        alive = 1;

    j = i, i = 0;
    while(header[j] == ' ')
        j++;
    while(header[j] != ' ' && i < 128)
    {

        url[i] = header[j];
    j++, i++;
    }
    url[i] = '\0';

    return alive;
}

//处理405
void write_405(http_request_t *http_ptr)
{
    char boby[128] = "<html> <h1>Hahahaha, we only accept GET because the maker is too low!</h1>\r\n";
    char header[256];

	if(http_ptr->alive)
        strcpy(header, H405KEEP);
    else
        strcpy(header, H405NONKEEP);

    if(write(http_ptr->fd, header, strlen(header)) < 0)
        push_log(errno, "： bad request write_header\n\n", 2, &log_thread);

    if(write(http_ptr->fd, boby, strlen(boby)) < 0)
        push_log(errno, ": bad request write boby\n\n", 2, &log_thread);
}

//处理200
int write_data(http_request_t *http_ptr,char *filename)
{
    struct stat st;
    char header[4096];

    if(stat(filename, &st) == -1 || S_ISDIR(st.st_mode))
    {
        write_405(http_ptr);
        return -1;
    }

    if(http_ptr->alive)
        sprintf(header, H200KEEP, st.st_size);
    else
        sprintf(header, H200NONKEEP, st.st_size);

    if(write(http_ptr->fd, header, strlen(header)) < 0)
    {
        push_log(errno, ": write heaer fail\n\n", 2, &log_thread);
        return -1;
    }

    int fd = open(filename, O_RDONLY);
    if(fd == -1)
    {
        push_log(errno, ": fail to open file\n\n", 2, &log_thread);
        return -1;
    }

    if(sendfile(http_ptr->fd, fd, NULL, st.st_size) < 0)
    {
        push_log(errno, ": sendfile fail\n\n", 2, &log_thread);
    }

    close(fd);

    return 0;
}

