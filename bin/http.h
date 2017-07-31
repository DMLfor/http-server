#ifndef _HTTPD_H_
#define _HTTPD_H_
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>

//建立服务器端socket
int startup(uint16_t *server_port, int listen_num);


#endif
