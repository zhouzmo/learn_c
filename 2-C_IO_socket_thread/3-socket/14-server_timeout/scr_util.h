#ifndef _SCR_UTIL_H_
#define _SCR_UTIL_H_

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

typedef struct IpAndPort
{
    char *ipaddr;
    unsigned short port;
} IpAndPort, *IpAndPortP;

typedef struct dataPacket
{
    int len;
    char buf[1024];
} DataPacket, *DataPacketP;

void activate_nonblock(int fd);
void deactivate_nonblock(int fd);
int read_timeout(int fd, unsigned int wait_seconds);
int write_timeout(int fd, unsigned int wait_seconds);
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);
ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);
ssize_t recv_peek(int sockfd, void *buf, size_t len);
ssize_t readline(int sockfd, void *buf, size_t maxline);

// 客户端环境初始化
int client_init(IpAndPort info, void **handle);

// 客户端发送报文
int pack_send(void *handle, unsigned char *data, int datalen);

// 客户端接收报文
int pack_recv(void *handle, unsigned char *out, int *outlen);

#endif /* _SCR_UTIL_H_ */