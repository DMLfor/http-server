#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <sys/epoll.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

//每一个连接socket都会创建一个请求结构体,并且epoll_event.data.ptr
//指针会指向这个地址，并且为动态申请内存.
typedef struct http_request
{
    int fd;    //socket描述符
    int alive;  //是否为keep-alive
    char method[32]; //请求方法
    char url[128];   //请求的url
    char ip_port[128]; //请求源ip和端口
} http_request_t;

//将一个事件添加到epoll中
void epoll_add(int epfd, http_request_t *ptr, int status);
//修改一个事件的兴趣
void epoll_mod(int epfd, http_request_t *ptr, int status);
//从epoll中删除一个事件
void epoll_del(int epfd, http_request_t *ptr, int status);
//设置套接字为非阻塞模式
void set_sock_nonblock(int sockfd);
#endif
