#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <stdio.h>
#include <stdlib.h>
#define default_port 2017
#define default_que_max 5000
#define default_thread_num 3
#define default_listen_num 20
#define default_evlist_size 2048
#define CONFIG_FILE_NAME "../etc/httpd.conf"

struct http_conf_t
{
	int port;       //端口
	int thread_num; //线程池中线程数
    int listen_num; //最大监听队列大小
    char docroot[50];//资源文件目录
    int evlist_num; //event list 大小
};

//读取配置信息
void init_conf(struct http_conf_t *);

//配置文件读取失败，使用默认信息
void default_conf(struct http_conf_t *);

#endif
