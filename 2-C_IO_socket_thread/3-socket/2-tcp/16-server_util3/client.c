
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

    while (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        if (!strcmp("quit\n", buffer))
        {
            break;
        }

        ret = sckClient_send(handle, connfd, buffer, strlen(buffer));

        printf("sckClient_send succes len=%ld,data = %s\n", strlen(buffer), buffer);

        ret = sckClient_recv(handle, connfd, out, &outlen);
        if (ret != 0)
        {
            break;
        }
        printf("sckClient_recv succes len=%d,data = %s\n", outlen, out);

        memset(buffer, 0, sizeof(buffer));
    }

    ret = sck_close(connfd);
    if (ret == 0)
    {
        printf("sck_close succes ret=%d\n", ret);
    }

    return ret;
}