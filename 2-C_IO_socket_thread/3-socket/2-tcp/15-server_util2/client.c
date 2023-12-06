
#include "scr_util.h"

int main()
{
    int ret = 0;
    void *handle = NULL;
    char *data = "123456789";
    int datalen = 10;
    unsigned char out[1024] = {0};
    int outlen = 1024;
    int connfd;
    char buffer[1024] = {0};

    ret = sckClient_init(&handle, 15, 5, 5, 1);
    printf("ret:%d\n", ret);

    sckClient_ConnectConn(handle, "127.0.0.1", 8888, &connfd);
    printf("ret:%d\n", ret);

    fgets(buffer, sizeof(buffer), stdin);

    ret = sck_send(handle, connfd, buffer, strlen(buffer));
    printf("ret:%d\n", ret);

    ret = sck_recv(handle, connfd, out, &outlen);
    printf("ret:%d\n", ret);
    printf("recv data:size(%d)%s\n", outlen, out);

    ret = sck_destory(handle, connfd);
    printf("ret:%d\n", ret);

    return ret;
}