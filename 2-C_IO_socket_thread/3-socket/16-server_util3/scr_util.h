#ifndef _SCR_UTIL_H_
#define _SCR_UTIL_H_

#include <unistd.h>
#include <sys/types.h>
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

#define MAX_BUFFER_SIZE 1024

// 开启 debug 日志
// #define LOG_OPEN

#ifdef LOG_OPEN
#define LOGGER(...)                                            \
    do                                                         \
    {                                                          \
        printf("%s:%s:%d ", __FILE__, __FUNCTION__, __LINE__); \
        printf(__VA_ARGS__);                                   \
    } while (0)
#else
#define LOGGER(...)
#endif
//  使用方法：LOGGER("The value of x is %d", x);

/*      错误码      */
#define Sck_Ok 0
#define Sck_BaseErr 3000
#define Sck_ErrParam (Sck_BaseErr + 1)
#define Sck_ErrTimeout (Sck_BaseErr + 2)
#define Sck_ErrPeerClosed (Sck_BaseErr + 3)
#define Sck_ErrMalloc (Sck_BaseErr + 4)

// 客户端
int sckClient_init(void **handle, int conntime, int sendtime, int recvtime, int nConnNum);
int sckClient_ConnectConn(void *handle, char *ip, int port, int *connfd);
int sckClient_send(void *handle, int connfd, unsigned char *data, int datalen);
int sckClient_recv(void *handle, int connfd, unsigned char *out, int *outlen);
// 服务端
int sckServer_init(int *listenfd, int port);
int sckServer_accept(int listenfd, int *connfd, int timeout);
int sckServer_send(int connfd, unsigned char *data, int datalen, int timeout);
int sckServer_recv(int connfd, unsigned char *out, int *outlen, int timeout);

int sck_close(int connfd);
int sck_destory(void *handle, int connfd);

#endif /* _SCR_UTIL_H_ */