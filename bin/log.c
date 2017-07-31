#include "log.h"

pthread_mutex_t lock;

//日志分级信息
//0  为 [info]
//1 为 [debug]
//2 为 [warn]
//3 为 [erro]
char log_grade[][15] = {" [info]: ", " [debug]: ", " [warn]: ", " [erro]: "};

//初始话日志线程
void init_log_thread(log_thread_t *log_thread)
{
    if(pthread_mutex_init(&lock, NULL) != 0)
    {
        perror("fail to init lock");
        exit(EXIT_FAILURE);
    }

    log_thread->size = 0;
    log_thread->head = log_thread->tail = NULL;
}

//将一条日志添加到日志链表里面
void push_log(int error, const char *str, int grade, log_thread_t *log_thread)
{
    msglog_t *msg_i = (msglog_t *)malloc(sizeof(msglog_t));
    msg_i->grade = grade;
    strcpy(msg_i->msg, log_grade[grade]);
    strcat(msg_i->msg, strerror(error));
    strcat(msg_i->msg, str);
    msg_i->next = NULL;

    //多个工作线程，需要加锁
    if(pthread_mutex_lock(&lock) < 0)
    {
        perror("lock fail");
    }

    if(log_thread->size == 0)
        log_thread->tail = log_thread->head = msg_i;
    else
    {
        log_thread->tail->next = msg_i;
        log_thread->tail = msg_i;
    }
    ++ log_thread->size;

    //解锁
    if(pthread_mutex_unlock(&lock) < 0)
    {
        perror("unlock fail");
    }
}

//日志线程入口, 日志线程会在此处不断从日志链表中取出
//start 为日志写缓存区的使用位置
//total 为日志文件使用大小，超过50M后轮替
void *log_work(void *arg)
{
    log_thread_t *log_thread = (log_thread_t *)arg;
    msglog_t *ptr = NULL;
    char filename[128];
    int fd = 0;
    char buf[4096], timebuf[256];
    int start = 0, total = 0;

    get_time(timebuf);
    sprintf(filename, "../log/%s.log", timebuf);
    if((fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666)) == -1)
    {
        perror("open log file fail");
    }
    while(1)
    {
        if(log_thread->size > 0)
        {

			//文件大小轮替
            if(total > 52428800)
            {
                close(fd);
                get_time(timebuf);
                sprintf(filename, "../log/%s.log", timebuf);
                if((fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666)) == -1)
                {
                    perror("open log file fail");
                }
                total = 0;
            }
			//缓存轮替
            if(start > 3968)
            {
                if(write(fd, buf, start) < 0)
                {
                    perror("write log fail");
                }
                total += start;
                start = 0;
            }

            ptr = log_thread->head;
            get_time(timebuf);
            start = fast_cat(buf, timebuf, start);
            start = fast_cat(buf, ptr->msg, start);

            //加锁
            if(pthread_mutex_lock(&lock) < 0)
            {
                perror("fail to lock mutex");
            }

            -- log_thread->size;
            if(log_thread->size == 0)
                log_thread->head = log_thread->tail = NULL;
            else
                log_thread->head = ptr->next;

            free(ptr);
            ptr = NULL;

            //解锁
            if(pthread_mutex_unlock(&lock) < 0)
            {
                perror("fail to lock mutex");
            }
        }
    }
}

//在日志缓存区的后面添加上日志信息，返回值为缓存区的使用位置
int fast_cat(char *s, const char *p, int start)
{
    char *ptr_s= (s + start);
    const char *ptr_p = p;

    do
    {
        *ptr_s = *ptr_p;
        ++ptr_s, ++ptr_p;
        ++ start;
    }while((*ptr_p != '\0'));

    return start;
}

//获得当前时间
void get_time(char *ptr)
{
    time_t timep;
    struct tm *ti_ptr;
    time(&timep);
    ti_ptr = localtime(&timep);
    sprintf(ptr, "%04d_%02d_%02d_%02d_%02d_%02d", 1900+ti_ptr->tm_year,
            1 + ti_ptr->tm_mon, ti_ptr->tm_mday, ti_ptr->tm_hour, ti_ptr->tm_min,
            ti_ptr->tm_sec);
}
