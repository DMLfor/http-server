#include "config.h"
#include <string.h>

void init_conf(struct http_conf_t *conf)
{
    FILE *fp = NULL;
    char line[128];
    char param_name[128];
    char param_value[128];
    if((fp = fopen(CONFIG_FILE_NAME, "r")) == NULL)
    {
        perror("fail to open conf file. now used default_conf.");
        default_conf(conf);
        return ;
    }
    while(fgets(line, 128, fp) != NULL)
    {
        sscanf(line, "%s %s", param_name, param_value);
        if(strcmp(param_name, "PORT") == 0)
            conf->port = atoi(param_value);

        if(strcmp(param_name, "THREAD_NUM") == 0)
            conf->thread_num = atoi(param_value);

        if(strcmp(param_name, "LISTEN_NUM") == 0)
            conf->listen_num = atoi(param_value);

        if(strcmp(param_name, "FILE_DIR") == 0)
            strcpy(conf->docroot, param_value);

        if(strcmp(param_name, "EVENT_LIST") == 0)
            conf->evlist_num = atoi(param_value);
    }
    fclose(fp);
}

void default_conf(struct http_conf_t *conf)
{
    conf->port = default_port;
    conf->thread_num = default_thread_num;
    conf->listen_num = default_listen_num;
    strcpy(conf->docroot, "../docroot");
    conf->evlist_num = default_evlist_size;
}
