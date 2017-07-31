#ifndef _FILE_BUF_
#define _FILE_BUF_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

typedef struct file
{
    unsigned int hash;
    char buf[10240];
	size_t size;
	struct file *next;
} file_t;

typedef struct file_list
{
    file_t *head;
    file_t *tail;
    int size;
} file_list_t;

void get_filelist(const char *basePath, file_list_t *file_list);

int readFileList(char *basePath, file_list_t *file_list);

unsigned int BKDRHash(char *str);

#endif
