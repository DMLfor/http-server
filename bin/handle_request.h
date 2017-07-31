#ifndef _HANDLE_REQUEST_H
#define _HANDLE_REQUEST_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <errno.h>
#include "epoll.h"

//200 并且非keep-alive报头
#define H200NONKEEP "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n"

//200 并且为keep-alive报头
#define H200KEEP "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\n\
    Keep-Alive: timeout=30\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n"

//405 并且为keep-alive报头
#define H405KEEP "HTTP/1.1 405 Request not allow\r\nConnection: keep-alive\r\n\
    Server: dblank/1.0.0\r\nContent-Type: text/html\r\nContent-Length : 76\r\n\r\n"

//405 并且为close 报头
#define H405NONKEEP "HTTP/1.1 405 Request not allow\r\nConnection: close\r\n\
    Server: dblank/1.0.0\r\nContent-Type: text/html\r\nContent-Length : 76\r\n\r\n"

//任务线程的入口，每个线程回不断的执行epoll_wait并且,根据
//触发的任务调用对应的函数.
void *wait_task(void *arg);

//405处理
void write_405(http_request_t *http_ptr);

//200处理
int write_data(http_request_t *http_ptr,char *filename);

//从http请求抱头中获得 方法, url, 返回值为alive类型
int get_url_method(const char * headet, char *url, char *method, size_t size);

//接受一个http请求
void http_recv(http_request_t *http_ptr, int epfd);

//发送一个http回复
int http_send(http_request_t *http_ptr, int epfd);

//检查是否keep-alive并处理
void check_alive(http_request_t *http_ptr, int epfd);

#endif

