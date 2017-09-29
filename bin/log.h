#ifndef _LOG_H_
#define _LOG_G_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

#define LOG_DIR "../log/"

//日志链表节点
typedef struct msglog
{
    char msg[128];       //日志信息
    int grade;          //分级
    struct msglog *next;//下一节点
    } msglog_t;

typedef struct log_thread
{
    volatile int size;
    msglog_t *head;    //链表头指针
    msglog_t *tail;     //链表尾指针
    pthread_mutex_t lock;
    pthread_cond_t log_not_empty;

} log_thread_t;

int fast_cat(char *s, const char *p, int start);

//初始话日志线程
void init_log_thread(log_thread_t *log_thread);

//日志线程工作入口
void *log_work(void *arg);

//获得时间戳
void get_time(char *ptr);

//将日志信息推送到日志链表
void push_log(int error, const char *str, int grade, log_thread_t *log_thread);

#endif
