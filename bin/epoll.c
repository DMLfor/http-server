#include "epoll.h"

//将一个事件添加到epoll之中
void epoll_add(int epfd, http_request_t *ptr, int status)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = ptr;
    ev.events = status;
    set_sock_nonblock(ptr->fd);
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, ptr->fd, &ev) == -1)
    {
        perror("epoll ctl_add fail");
    }

}
//修改epoll事件兴趣
void epoll_mod(int epfd, http_request_t *ptr, int status)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = ptr;
    ev.events = status;
    if(epoll_ctl(epfd, EPOLL_CTL_MOD, ptr->fd, &ev) == -1)
    {
        perror("epoll ctl_add fail");
    }

}
//从epoll中删除有事件
void epoll_del(int epfd, http_request_t *ptr, int status)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = ptr;
    ev.events = status;
    if(epoll_ctl(epfd, EPOLL_CTL_DEL, ptr->fd, &ev) == -1)
    {
        perror("epoll ctl_add fail");
    }

}
//设置套接字为非阻塞
void set_sock_nonblock(int sockfd)
{
    int old_option = fcntl(sockfd, F_GETFL, 0);
    if(old_option == -1)
    {
        perror("fcntl get old fail!");
        return ;
    }
    if(fcntl(sockfd, F_SETFL, old_option | O_NONBLOCK) == -1)
    {
        perror("fcbtl set nonblock fail!");
        return ;
    }
}

