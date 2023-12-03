#include "scr_util.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void sig_handler(int signum)
{
    int mypid = 0;
    int saved_errno = errno; // 保存 errno，以便在 waitpid 调用后恢复
    int status;
    printf("process pid:%d recv SIGCHLD signo(%d)\n ", getpid(), signum);
    // wait(NULL);
    // 仍然存在信号丢失，但是使用 while
    while ((mypid = waitpid(-1, NULL, WNOHANG) > 0))
    {
        // 处理已终止的子进程
        if (WIFEXITED(status))
        {
            printf("Child process %d terminated with status: %d\n", mypid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child process %d terminated by signal %d\n", mypid, WTERMSIG(status));
        }
    }
    errno = saved_errno; // 恢复 errno
    printf("process pid:%d handle success\n ", getpid());
}

int main(void)
{
    // 当客户端异常断开后,服务端子进程会变成僵尸进程,解决
    // signal(SIGCHLD, SIG_IGN); // 忽略
    signal(SIGCHLD, sig_handler);

    int listenfd;

    if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt");

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind");

    if (listen(listenfd, SOMAXCONN) < 0)
        ERR_EXIT("listen");

    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    intptr_t conn;

    while (1)
    {
        int ret;
        int flag;
        int wait_seconds = 5;

        if ((conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen)) < 0)
            ERR_EXIT("accept");

        unsigned short prot = ntohs(peeraddr.sin_port);
        char *ipaddr = inet_ntoa(peeraddr.sin_addr);
        printf("accept client ip=%s port=%d\n", ipaddr, prot);

        int pid = fork();
        if (pid == 0)
        {
            printf("client process:%d\n", getpid());
            // 子进程处理客户端连接
            char recvbuf[1024];

            int datalen;
            // 关闭多余的 listenfd，在创建进程时，listenfd 因为引用计数机制会 +1，我们调用 close -1，在下一次调用就可以字节关掉了
            close(listenfd);

            while (1)
            {
                memset(recvbuf, 0, sizeof(recvbuf));

                ret = read_timeout(conn, wait_seconds);

                if (ret == 0) // 指定时间内有读事件发生
                {

                    flag = pack_recv((void *)conn, recvbuf, &datalen);
                    LOGGER("flag=%d", flag);

                    if (flag < 0)
                    {
                        printf("客户端对方已经关闭\n");
                        break;
                    }
                }
                else if (ret == -1 && errno == ETIMEDOUT)
                {
                    printf("读超时..\t");
                    continue;
                }
                else
                {
                    ERR_EXIT("read_timeout error");
                }

                // 在这里可以对接收到的数据进行处理,打印
                // printf("ip=%s port=%d client msg:", ipaddr, prot);
                LOGGER("pack_recv(size %d)  success: %s  \n", datalen, recvbuf);
                printf("recv client msg (size %d)  success: %s  \n", datalen, recvbuf);
                int wtlen;


                ret = write_timeout(conn, wait_seconds);
                if (ret == 0)
                {
                    flag = pack_send((void *)conn, recvbuf, datalen);
                    if (flag < 0)
                    {
                        printf("客户端对方已经关闭\n");
                        break;
                    }
                    else if (flag == 0)
                    {
                        LOGGER("pack_send(size %d)  success: %s  \n", datalen, recvbuf);
                    }
                }
                else if (ret == -1 && errno == ETIMEDOUT)
                {
                    printf("写超时\n");
                    continue;
                }
                else if (ret < 0)
                {
                    ERR_EXIT("write_timeout error");
                }
                LOGGER("pack_send(size %d)  success: %s  \n", datalen, recvbuf);
                printf("send client msg(size %d)  success: %s  \n", datalen, recvbuf);
            }
            close(conn);
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0)
        {
            // 父进程继续监听连接
            close(conn);
            continue;
        }
        else
        {
            ERR_EXIT("fork error");
        }
    }
    return 0;
}