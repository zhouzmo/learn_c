
#include "scr_util.h"

int main()
{
    int ret = 0;
    int listenfd = 0;
    int port = 8888;

    ret = sckServer_init(&listenfd, port);

    while (1)
    {
        int connfd;
        int timeout;
        printf("wait accept\n");
        ret = sckServer_accept(listenfd, &connfd, timeout);
        if (ret == Sck_ErrTimeout)
        {
            LOGGER("time_out ret=%d", ret);
            continue;
        }
        else if (ret < 0)
        {
            LOGGER("sckServer_accept ret=%d", ret);
            // break;
        }

        int pid = fork();
        if (pid == 0)
        {
            // 子进程
            close(listenfd);

            int bufLen = 1024;
            char recvbuf[1024] = {0};
            while (1)
            {
                memset(recvbuf, 0, sizeof(recvbuf));

                ret = sckServer_recv(connfd, recvbuf, &bufLen, 5);
                if (ret != 0)
                {

                    if (ret == Sck_ErrTimeout)
                    {
                        printf(".\n");
                        continue;
                    }
                    LOGGER("recv error ret=%d", ret);
                    break;
                }
                printf("recv len=%d,data=:%s\n", bufLen, recvbuf);

                ret = sckServer_send(connfd, recvbuf, bufLen, 6);
                if (ret != 0)
                {
                    LOGGER("send error ret=%d", ret);
                    break;
                }
                printf("send len=%d,data=:%s\n", bufLen, recvbuf);
            }

            close(connfd);
            exit(ret);
        }
        else
        { // 父进程
            close(connfd);
        }
    }

    return ret;
}
