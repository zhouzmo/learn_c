#include "scr_util.h"
#include <sys/wait.h>
#include <signal.h>
#include <stdint.h>

void sig_handler(int signum)
{
    int status;
    printf("process pid:%d recv (%d)SIGPIPE\n ", getpid(), signum);
    // wait(NULL);
    // 仍然存在信号丢失，但是使用 while

    printf("process pid:%d handle success\n ", getpid());
}

int main(void)
{

    signal(SIGPIPE, sig_handler);

    fd_set rset;
    FD_ZERO(&rset);
    int nready;
    int maxfd;
    int fd_stdin = fileno(stdin);
    intptr_t sock;

    maxfd = fd_stdin > sock ? fd_stdin : sock;
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    int stdineof = 0;
    int len;

    IpAndPort info;
    info.ipaddr = "127.0.0.1";
    info.port = 8888;
    void *handle;
    if (client_init(info, &handle) != 0)
    {
        printf("Failed to initialize client.\n");
        return -1;
    }
    sock = (intptr_t)handle;

    while (1)
    {
        if (stdineof == 0)
            FD_SET(fd_stdin, &rset);
        FD_SET(sock, &rset);

        // 当无事件发送时，循环非阻塞等待
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready == -1)
            ERR_EXIT("select");

        if (nready == 0)
            continue;

        // 发生 事件
        if (FD_ISSET(sock, &rset))
        {
            printf("发生 read 事件\n");

            int ret = pack_recv(sock, recvbuf, &len);
            if (ret == -1)
                ERR_EXIT("readline");
            else if (ret == 0)
            {
                printf("server close\n");
                break;
            }
            fputs(recvbuf, stdout);
            printf("\n");

            memset(recvbuf, 0, sizeof(recvbuf));
        }

        // 命令行输入事件
        if (FD_ISSET(fd_stdin, &rset))
        {
            printf("命令行输入事件\n");

            if (fgets(sendbuf, sizeof(sendbuf), stdin) == NULL)
            {
                stdineof = 1;
                shutdown(sock, SHUT_WR);
                FD_CLR(fd_stdin, &rset);
                continue;
            }

            // 发送数据
            pack_send(sock, sendbuf, strlen(sendbuf));
            memset(sendbuf, 0, sizeof(sendbuf));
        }
    }

    close(sock);

    return 0;
}
