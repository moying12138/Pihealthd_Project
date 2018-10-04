/************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2018年06月10日 星期日 15时05分16秒
 ************************************************************************/

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
#include"../common.h"
#define BUFF 1024
#define M_BUFF 10240

int get_conf_value(char *pathname, char *key_name, char *value);

int socket_creat(int port);

int socket_accept(int socket_listen);

int socket_connect(int port, char *host);


int rev_msgx(int sockfd) {
    int n;
    recv(sockfd, &n, sizeof(n), 0);
    return n;
}   
void send_int(int sockfd, int text) {
    send(sockfd, &text, sizeof(text), 0);
}

int get_short_sockfd() {
    char short_port[100];
    get_conf_value(config_file, "short_port", short_port);
    int port = atoi(short_port);
    int sockfd_short = socket_creat(port);
    return sockfd_short;
}

int my_fread(int sockfd, char *path, int n, int short_port) {
    char buff[BUFF] = {'\0'};
    FILE *fp;
    if((fp = fopen(path, "r")) == NULL) {
        DBG("%s is null, send code : %d\n", path, n + 300);
        send_int(sockfd, n + 300);
        return 0;
    }
    flock(fp->_fileno, LOCK_EX);
    send_int(sockfd, n + 100);

    DBG("%s has content, send code : %d\n", path, n + 100);
    int data_sockfd = socket_accept(short_port);
    while(fread(buff, 1, 1000, fp)) {
        if(!strlen(buff)) break;
        send(data_sockfd, buff, strlen(buff), 0);
        memset(buff, '\0', sizeof(buff));
    }
    __rm_path(path);
    //添加文件解锁
    flock(fp->_fileno, LOCK_UN);
    fclose(fp);
    DBG("%s.log send complete,  sign code : %d\n", p[n - 100], n);
    close(data_sockfd);
    return 1;
}
char *anasy(int n) {
    char *str = calloc(sizeof(char), 100);
    char path[100];
    get_conf_value(config_file, "send_log_path", path);
    strcpy(str, path);
    switch(n) {
        case 100:
            strcat(str, "/cpu.log");
            break;
        case 101:
            strcat(str, "/disk.log");
            break;
        case 102:
            strcat(str, "/mem.log");
            break;
        case 103:
            strcat(str, "/users.log");
            break;
        case 104:
            strcat(str, "/system.log");
            break;
        case 105:
            strcat(str, "/evil_process.log");
            break;
        default:
            str = NULL;
    }
    printf("%s -- %d\n", str, n);
    return str;
}
int go_connect(int sockfd, int short_port) {
    int cnt = 0;
    for(int i = 0; i < 6; ++i) {   //建立6次短连接
        int sign = rev_msgx(sockfd); // 客户端发来的请求100 101
        DBG("recv code : %d\n", sign);
        char *d_pathx = anasy(sign);
        if(d_pathx == NULL)  {
            DBG("The sign is Unrecognized\n");
            return 0;
        }
        cnt += my_fread(sockfd, d_pathx, sign, short_port);
	    free(d_pathx);
    }
    DBG("send success %d file close long connect\n", cnt);
    return 1;
}

void __strcat(char *file, int sign, int sh_sign) {
    char str[100];
    get_conf_value(config_file, "run_path", str);
    if(sh_sign == 0) {
        strcpy(file, "bash ");
        strcat(file, str);
        strcat(file, "sh/");
        strcat(file, p[sign]);
        strcat(file, ".sh");
    } else {
        strcpy(file, str);
        strcat(file, "log/");
       __mkdir_path(file);
        strcat(file, p[sign]);
        strcat(file, ".log");
    }
}

int *get_sleep_time() {
    int *arr = (int *)malloc(sizeof(int) * 6);
    arr[0] = 5;
    arr[1] = 60;
    arr[2] = 5;
    arr[3] = 60;
    arr[4] = 60;
    arr[5] = 30;
    return arr;
}

void send_str_sign(char *str, int sign, int length) {
    char port_str[100], adress[100];
    if(!sign) {
        get_conf_value(config_file, "warn_port", port_str);
        get_conf_value(config_file, "warn_adress", adress);
    }  
    int port = atoi(port_str);
    int sockfd;
    while(1){
        sockfd = socket_connect(port, adress);
        if(sockfd < 0) sleep(30);
        else break;
    }
    send(sockfd, str, length, 0);
    close(sockfd);
    DBG("send %d %s warning message \n%s\n", port, adress, str);
}

char* anasy_warn(char *temp_str, int sign) { 
    if(sign < 4 && sign > 0) return temp_str;
    int len = 0;
    char buff[1000] = {0};
    while(sscanf(temp_str+len, "%[^\n]", buff)) {
        int count = strlen(buff);
        len += count + 1;
        if(count == 1) break; 
    }
    if(len <= 2) return temp_str + len;
    signal(SIGCHLD,SIG_IGN);
    int pid = fork();
    if(pid == 0) {
        strncpy(buff, temp_str, len - 2);
        buff[len - 2] = '\0';
        send_str_sign(buff, 0, len - 2);
        exit(0);
    }
    else return temp_str + len;
}

void get_backup_path(int sign, char *backup_path) {
    get_conf_value(config_file, "backup_path", backup_path);
    strcat(backup_path, p[sign]);
    __mkdir_path(backup_path);
    strcat(backup_path, "/");
    int time = get_time_count();
    char str[100];
    sprintf(str, "%d", time);
    strcat(backup_path, str);         //添加删除文件
}

void backup_file(char *filepathname, int sign) {
    __mv_path(filepathname, "./");
    char backup_path[100] = {0};
    get_backup_path(sign, backup_path);
    DBG("backup path is %s the sign is %d\n", backup_path, sign);
    __backup_system(basename(filepathname), backup_path);

}
void write_log_sign(int signx) {
    char buff[M_BUFF] = {0};
    int sign = signx;
    char filelogname[100] = {0};
    char fileshname[100] = {0};
    __strcat(filelogname, sign, 1);
    __strcat(fileshname,sign, 0);
    int *time_arr = get_sleep_time();
    DBG("save  log -- %s    sh -- %s\n", filelogname, fileshname);
    int count = 0, time = time_arr[sign];
    free(time_arr);
    while(1) {
        for(int i = 0; i < 10; ++i, ++count) {
            FILE *fp = popen(fileshname, "r");
            if(fp == NULL) {
                DBG("%s run fail\n", fileshname);
                continue;
            }
            char temp_str[1000] = {0};
            fread(temp_str, 1, sizeof(temp_str), fp);
            pclose(fp);
            printf("count=================================%d\n",count);
            char *temp = anasy_warn(temp_str, sign);
            strcat(buff, temp);                 //此处添加分析temp内容,如果warn传送服务端
            sleep(time);
        }
        char temp_dir[100];
        strcpy(temp_dir, filelogname);
        __mkdir_path(dirname(temp_dir));
        FILE *fp = fopen(filelogname, "a");
        flock(fp->_fileno, LOCK_EX);
        fwrite(buff, 1, strlen(buff), fp);
        flock(fp->_fileno, LOCK_UN);
        fclose(fp);
        //if(get_file_size(filelogname) >= 30) backup_file(filelogname, sign);
        DBG("%s write success\n", filelogname);
        memset(buff, 0, sizeof(buff));
    }
}

void write_log() {
    int i = 0, pid;
    for(; i < 5; ++i) {
        signal(SIGCHLD,SIG_IGN);
        pid = fork();
        if(!pid) break;
    }
    write_log_sign(i);
}
int main(int argc, char *argv[]) {
    DBG("System Startup\n");
    signal(SIGCHLD,SIG_IGN);
    int pid1 = fork();
    if(pid1 > 0) return 0;
    signal(SIGCHLD,SIG_IGN);
    int pid = fork();
    if(pid > 0) {
        write_log();
    } else{
        int short_port = get_short_sockfd();
        char long_port[100];
        get_conf_value(config_file, "long_port", long_port);
        int sListen = socket_creat(atoi(long_port)); // 长链接
        int sockfd;
        while(1){
	        printf("wait connect \n");
            sockfd = socket_accept(sListen);
            if(sockfd > 0) {
                signal(SIGCHLD,SIG_IGN);
                int pid = fork();
                if(pid == 0) {
                    close(sListen);
                    go_connect(sockfd, short_port);
                    close(sockfd);
                    exit(0);
                }
                close(sockfd);
            }
        }
    }
    int status = 0, wpid;
    while(wpid = (wait(&status)) > 0) ;
    return 0;
}
