#include "http.h"
#include "config.h"
#include "handle_request.h"
#include "epoll.h"
#include "log.h"
#include "filebuf.h"
#include <netinet/tcp.h>
//配置文件结构体
struct http_conf_t conf;

//日志链表结构体
log_thread_t log_thread;


int main()
{
    int client_sock = -1, server_sock = -1;
    int i;
    uint16_t server_port;
    int *epfds = NULL;
    struct sockaddr_in client_addr;
    char client_ip[128];
    socklen_t clilen = sizeof(struct sockaddr_in);
    pthread_t *threads = NULL, log_id;

    //初始化日志链表
    init_log_thread(&log_thread);

    //读取配置文件
    init_conf(&conf);


    //申请线程ID epollfd 资源
    epfds = (int *)malloc(sizeof(int) * conf.thread_num);
    threads = (pthread_t *)malloc(sizeof(pthread_t) * conf.thread_num);
    if(epfds == NULL)
    {
        perror("epfds malloc fail");
        exit(EXIT_FAILURE);
    }
    if(threads == NULL)
    {
        perror("threads malloc fail");
        exit(EXIT_FAILURE);
    }
    //创建并启动服务端监听套接字
    server_port = conf.port;
    server_sock = startup(&server_port, conf.listen_num);

    //创建epoll
    for(i = 0; i < conf.thread_num; i++)
    {
        if((epfds[i] = epoll_create1(0)) < 0 )
        {
            perror("epoll create fail");
            exit(EXIT_FAILURE);
        }
    }
    //创建工作线程
    for(i = 0; i < conf.thread_num; i++)
    {
        if(pthread_create(&threads[i], NULL, wait_task, (void *)(epfds + i)) != 0)
        {
            perror("thread create fail");
            exit(EXIT_FAILURE);
        }
    }
    //创建日志线程
    if(pthread_create(&log_id, NULL, log_work, &log_thread) != 0)
    {
        perror("fail to create log htread.");
        exit(EXIT_FAILURE);
    }
    //主线程功能为从监听套接字中accept到连接套接字,并分下到各线程epoll
    while(1)
    {
        for(int i = 0; i<conf.thread_num; i++)
        {
            if((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &clilen))  < 0)
            {
                push_log(errno, " : accpet fail\n\n", 2, &log_thread);
                continue;
            }
            http_request_t *ptr = (http_request_t *)malloc(sizeof(http_request_t));
            ptr->fd = client_sock;
            ptr->alive = 0;
            epoll_add(epfds[i], ptr, EPOLLIN);

            //添加接入日志
            sprintf(ptr->ip_port, " ip: %s port: %d connect\n\n", inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, 128), ntohs(client_addr.sin_port));
            push_log(errno, ptr->ip_port, 0, &log_thread);
        }
    }
}

//创建和启动服务端套接字
int startup(uint16_t *server_port, int listen_num)
{
    struct sockaddr_in server_addr;
    int on = 1;
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1)
    {
        perror("socket faile");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(*server_port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //设置为端口复用
    if((setsockopt(server_sock, SOL_SOCKET,SO_REUSEADDR, &on, sizeof(on))) < 0)
        perror("setsockopt fail!");
    
    setsockopt(server_sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) <0)
    {
        perror("bind fail");
        exit(EXIT_FAILURE);
    }

    if(listen(server_sock, listen_num) < 0)
    {
        perror("listen fail");
        exit(EXIT_FAILURE);
    }
    return server_sock;
}

