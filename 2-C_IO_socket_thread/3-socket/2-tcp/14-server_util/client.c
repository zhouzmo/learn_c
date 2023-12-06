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

    char sendbuf[MAX_BUFFER_SIZE] = {0};
    char recvbuf[MAX_BUFFER_SIZE] = {0};
    int stdineof = 0;
    int len;
    int ret;
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

    // 注意 初始化后再比较！！！！！！！！
    maxfd = fd_stdin > sock ? fd_stdin : sock;
    LOGGER("fd_stdin=%d,sock=%d,manfd=%d", fd_stdin, sock, maxfd);

    while (1)
    {
        if (stdineof == 0)
            FD_SET(fd_stdin, &rset);
        FD_SET(sock, &rset);

        // 当无事件发送时，循环非阻塞等待
        LOGGER("wait...");

        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready == -1)
            ERR_EXIT("select");

        if (nready == 0)
            continue;
        LOGGER("nready=%d", nready);
        // 发生 事件
        if (FD_ISSET(sock, &rset))
        {
            LOGGER("发生 read 事件");

            ret = pack_recv((void *)sock, recvbuf, &len);
            if (ret < 0)
            {
                printf("客户端对方已经关闭\n");
                break;
            }
            printf("select recv event,recv server msg success (size=%d):%s", len, recvbuf);
            memset(recvbuf, 0, sizeof(recvbuf));
        }

        // 命令行输入事件
        if (FD_ISSET(fd_stdin, &rset))
        {
            LOGGER("命令行输入事件");

            if (fgets(sendbuf, sizeof(sendbuf), stdin) == NULL)
            {
                stdineof = 1;
                shutdown(sock, SHUT_WR);
                FD_CLR(fd_stdin, &rset);
                continue;
            }

            // 发送数据
            pack_send((void *)sock, sendbuf, strlen(sendbuf));
            printf("select cmdline event,send server msg success (size=%zu):%s", strlen(sendbuf), sendbuf);

            memset(sendbuf, 0, sizeof(sendbuf));
        }
    }

    close(sock);

    return 0;
}
