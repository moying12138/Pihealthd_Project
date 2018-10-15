/*************************************************************************
	> File Name: common.h
	> Author: 
	> Mail: 
	> Created Time: 2018年06月10日 星期日 16时06分44秒
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H
#endif
#include<stdio.h>
#include<ctype.h>
#include<dirent.h>
#include<stdlib.h>  
#include<errno.h>  
#include<string.h>  
#include<sys/types.h>  
#include<netinet/in.h>  
#include<sys/socket.h>  
#include<sys/wait.h> 
#include<arpa/inet.h>
#include<fcntl.h>
#include<netdb.h>
#include<stdarg.h>
#include<unistd.h>
#include<signal.h>
#include<sys/file.h>
#include<time.h>
#include <libgen.h>
#define DEBUG
#ifdef DEBUG
#define DBG(format, arg...) {\
    printf(format, ##arg);\
    char __buff[1000];\
    sprintf(__buff,format, ##arg);\
    dbg_save(__buff);\
}
#else
#define DBG(format, arg...) 
#endif
char accept_adress[100];
int __log_sign = 1;
#define mperror(meg) printf("[%s:%d -- %s]  %s \n",__FILE__, __LINE__, __func__, #meg)
void fflush_my() {
        fflush(stdin);
        fflush(stdout);

}
void __mkdir_path(char *);
void __rm_path(char *);
char config_file[100] = "/home/moying12138/linux/project/config";
char p[6][100] = {"cpu","disk","mem","users","system", "evil_process"};
int get_conf_value(char *pathname, char *key_name, char *value){
    FILE *fp;
    if(pathname == NULL || key_name == NULL) {
        perror("NULL ");
        return 0;
    }
    if((fp = fopen(pathname, "r")) == NULL){
        perror("open error");
        return 0;
    }
    
    char buff[100] = {0};
    int len = strlen(key_name);
    while(fscanf(fp, "%s", buff) != EOF) {
        if(strncmp(buff, key_name, len) == 0) {
            if(buff[len] != '=') continue;
            strcpy(value, buff + len + 1);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}
void dbg_save(char *str) {
    char fp_str[100];
    get_conf_value(config_file, "log_path", fp_str);
    char temp_str[100];
    strcpy(temp_str, fp_str);
    __mkdir_path(dirname(temp_str));
    FILE *fp = fopen(fp_str, "a");  //获取配置文件sign
    flock(fp->_fileno, LOCK_EX);
    time_t temp;
    struct tm *tm_t = NULL;
    time(&temp);
    tm_t =localtime(&temp);
    fprintf(fp,"%d.%d.%d %d:%d:%d  %s", (1900+tm_t->tm_year), (1 + tm_t->tm_mon), tm_t->tm_mday, tm_t->tm_hour, tm_t->tm_min, tm_t->tm_sec,str);
    flock(fp->_fileno, LOCK_UN);
    fclose(fp);
}

int socket_creat(int port) {
        
    int sockfd;
    struct sockaddr_in sock_addr;
    if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
	DBG("socket_creat error\n");
        return -1;
    }
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
        DBG("bind error\n");
        return -1;
    }
    if(listen(sockfd,10) < 0) {
        DBG("listen error\n");
        return -1;
    }
    return sockfd;
}
int socket_connect(int port, char *host) {
    int sockfd;
    struct sockaddr_in dest_addr;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        DBG("socket  fail, connect %s \n", host);
        return -1;
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(host);
    if(connect(sockfd, (struct sockaddr *)&dest_addr,sizeof(dest_addr)) < 0) {
	DBG("connect fail : %s \n", host);
        return -1;
    }
    DBG("%s : sucess connect\n", host);
    return sockfd;
}
int socket_accept(int listen_socket) {
    struct sockaddr_in revadrr;
    socklen_t addrlen = sizeof(revadrr);
    int socket = accept(listen_socket, (struct sockaddr *)&revadrr, &addrlen);
    if(socket < 0) {
        DBG("accept error");
        return -1;
    }
    strcpy(accept_adress, inet_ntoa(revadrr.sin_addr));
    DBG("has a connect ip %s\n",inet_ntoa(revadrr.sin_addr));
    return socket;
}

char *anasy_log(char *path, int sign) {
    if(sign < 100 || sign > 105) return NULL;
    char *pathx = (char *)malloc(sizeof(char) * 100);
    strcpy(pathx, path);
    strcat(pathx, "/");
    strcat(pathx, p[sign - 100]);
    strcat(pathx, ".log");
    return pathx;

}

void __mkdir_path(char *path) {
    char str[100] = "mkdir -p ";
    strcat(str, path);
    system(str);
}

void __rm_path(char *path) {
    char str[100] = "rm ";
    strcat(str, path);
    system(str);
}
