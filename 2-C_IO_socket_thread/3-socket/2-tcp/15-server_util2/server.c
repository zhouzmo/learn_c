
#include "scr_util.h"

int main()
{
    int ret = 0;
    void *handle = NULL;

    int datalen = 10;
    char out[1024] = {0};
    int outlen = 1024;

    int clientPort;
    char *clientIPaddr;

    // 客户端环境初始化
    ret = sckServer_init(&handle, 15, 5, 5, 1);

    while (1)
    {
        int connfd;
        printf("wait accept\n");
        ret = sckServer_AcceptConn(handle, clientIPaddr, &clientPort, &connfd);
        if (ret < 0)
        {
            continue;
        }
        printf("ret:%d\n", ret);
        ret = sck_recv(handle, connfd, out, &outlen);
        printf("ret:%d\n", ret);

        printf("收到 len(%d)+4的数据:%s\n", outlen, out);
        ret = sck_send(handle, connfd, out, outlen);
        printf("ret:%d\n", ret);

        // 客户端环境释放
        // ret = sck_destory(handle, connfd);

        printf("success\n");
    }

    return ret;
}