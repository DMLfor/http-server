//文件缓存机制，暂未启用
//机制为程序开始时，将配置的资源目录所有文件建立到
//一个链表里面，并且将文件目录hash映射，通过hash来查找
//文件并传输。


#include "filebuf.h"
#include "config.h"


void get_filelist(const char *basePath, file_list_t *file_list)
{
    char filedir[120];
    strcpy(filedir, basePath);
    file_list->head = NULL;
    file_list->tail = NULL;
    file_list->size = 0;
    readFileList(filedir, file_list);
}


int readFileList(char *basePath, file_list_t *file_list)
{
    DIR *dir;
    struct dirent *ptr;
    char base[32], filename[64];
    file_t *file_ptr = NULL;
    int fd;
    size_t size;
    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        exit(EXIT_FAILURE);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)   //file
        {
            file_ptr = (file_t *)malloc(sizeof(file_t));
            sprintf(filename, "%s/%s", basePath, ptr->d_name);
            puts(filename);
            fd = open(filename, O_RDONLY);
            size = read(fd, file_ptr->buf, 10240);
            file_ptr->hash = BKDRHash(filename);
            if(size < 0)
            {
                perror("open file fail");
                continue;
            }
            file_ptr->size = size;
            file_ptr->next = NULL;
            if(file_list->size == 0)
            {
                file_list->head = file_list->tail = file_ptr;
            }
            else
            {
            	file_list->tail->next = file_ptr;
            file_list->tail = file_ptr;
            }
            ++ file_list->size;
        }
        else if(ptr->d_type == 4)    //dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readFileList(base, file_list);
        }
    }
    closedir(dir);
    return 1;
}

unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}
