/*************************************************************************
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
int get_conf_value(char *pathname, char *key_name, char *value);

int socket_creat(int port);
int socket_accept(int socket_listen);

char d_path[100] = "/home/moying12138/linux/project/master/log";
int socket_connect(int port, char *host);

int sen_mesg_int(int sockfd, int text){
    send(sockfd, &text, sizeof(text), 0);
}
int recv_mesg_int(int sockfd) {
    int n;
    recv(sockfd, &n, sizeof(n), 0);
    return n;
}

char **get_ip() {
	char **ip = (char **)calloc(15,sizeof(char *));
	ip[0] = "192.168.1.40";
	ip[1] = "192.168.1.41";
	ip[2] = "192.168.1.42";
	ip[3] = "192.168.1.43";
	ip[4] = "192.168.1.44";
	ip[5] = "192.168.1.45";
	ip[6] = "192.168.1.46";
	ip[7] = "192.168.1.47";
	ip[8] = "192.168.1.48";
	ip[9] = "192.168.1.49";
    ip[10] = "127.0.0.1";
	return ip;
}

void __strcat_m(char *str, char *adress) {
    strcpy(str, d_path);
    strcat(str, "/");
    strcat(str, adress);
}

void recv_file(int sockfd, int sign, char *adress) {
    char dirpath[100];
    __strcat_m(dirpath, adress);
    __mkdir_path(dirpath);
    
    char *path = anasy_log(dirpath, sign);
    if(path == NULL) return;
    FILE *fp = fopen(path, "a");
    if(fp == NULL) {
        DBG("open %s fail, please check\n", path);
	    DBG("close %s connect, recv %s.log fail, recv code : %d\n", adress, p[sign - 100], sign);
	    close(sockfd);
	    return ;
    }
    char buff[BUFF] = {0};
    int size, cnt = 0;
    while((size = recv(sockfd, buff, BUFF, 0)) > 0) {
        cnt += size;
        fwrite(buff, 1, size, fp);
        memset(buff, '\0', sizeof(buff));
    }
    DBG("%s.log recv success, total  %d bytes, recv code: %d\n", p[sign - 100], cnt, sign);
    free(path);
    fclose(fp);
    close(sockfd);
}
void warn_recv(int port, int sockfd) {
    int accept_socket = socket_accept(sockfd);
    int pid = fork();
    if(pid == 0) {
        close(accept_socket);
        return ;
    }
    char warn_str[100];
    get_conf_value(config_file, "warn_path", warn_str);
    FILE *fp = fopen(warn_str, "a");
    char buff[BUFF] = {0};
    strcat(accept_adress, "   \n");
    fwrite(accept_adress, 1, strlen(accept_adress), fp);
    DBG("from %s", accept_adress);
    if(recv(accept_socket, buff, sizeof(buff), 0) > 0) {
        printf("%s\n", buff);
        fwrite(buff, 1, strlen(buff), fp);
    }
    fclose(fp);
    close(accept_socket);
    DBG("close warning connect %d\n", accept_socket);
    exit(0);

}

int get_port(char *port_sign) {
    char port[100];
    get_conf_value(config_file, port_sign, port);
    int temp = atoi(port);
    return temp;
}

int main(int argc) {
    //int pid = fork();
    //if(pid != 0) return 0;
    int port = get_port("long_port");//长连接端口 
    int pid = fork();
    char **ip = get_ip();
    DBG("Systemp startup\n");
    if(pid == 0) {
        int sockfd = socket_creat(4567);
        while(1) {
            warn_recv(4567, sockfd);
        }
    } else {
        int short_port = get_port("short_port");
        int i = 0, pid;
	    for(; i < 10; ++i) {   //0-9 10个
            pid = fork();
            if(!pid) break;
        }
	    while(1){
            int sockfd;
            while(1){
                sockfd = socket_connect(port, ip[i]);
                if(sockfd == -1) sleep(60);
                else break;
            }
            for(int x = 100; x < 106; ++x) {
                    sen_mesg_int(sockfd, x);
                    printf("send %d to %s\n", x, ip[i]);
                    int revcode = recv_mesg_int(sockfd);
                    DBG("recv from %s , code : %d, ", ip[i], revcode);
                    if(revcode == x + 100) {
                        DBG("%s.log  exist, enter in recv\n", p[x - 100]);
                        int data_sockfd = socket_connect(short_port, ip[i]);
                        recv_file(data_sockfd, x, ip[i]);
                        close(data_sockfd);
                    } else if(revcode == x + 300) {
		                DBG("%s.log is no exist ", p[x - 100]);
                    } else {
                        DBG("Unrecognized code  ");
                    }
            }
            DBG("close %s long connect \n", ip[i]);
            close(sockfd);
            sleep(60);
        }
    }
    return 0;
}
