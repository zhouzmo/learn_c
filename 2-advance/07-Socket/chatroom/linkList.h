#ifndef __LINKLIST_H__
#define __LINKLIST_H__

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ERRLOG(msg)                                         \
    do {                                                    \
        printf("%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        perror(msg);                                        \
        exit(-1);                                           \
    } while (0)

#define N 128

#define LEN 128
#define NUM_USR 64

#define datatype int

//自定义结构体，用来保存所有连接的客户的IP地址、端口号以及acceptfd文件描述符这三个参数
typedef struct info {
    struct sockaddr_in clientaddr;
    int acceptfd;
    char name[16];
    char named;
    struct info* next;
    char sayHi;
} usr_info_t;

extern usr_info_t* info_head;

usr_info_t* LinkListNodeCreate(void);
int LinkListInsertHead(usr_info_t* head, usr_info_t* node);
usr_info_t* LinkListSearchUsrByAcceptfd(usr_info_t* h, int acceptfd);


#endif
