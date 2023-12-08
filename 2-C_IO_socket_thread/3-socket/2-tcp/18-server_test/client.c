#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

ssize_t readline(int sockfd, void *buf, size_t maxline)
{
    int ret;
    int nread;
    char *bufp = buf;
    int nleft = maxline;

    while (1)
    {
        ret = recv(sockfd, bufp, nleft, 0);
        if (ret == -1)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        else if (ret == 0)
        {
            return 0;
        }

        nread = ret;
        int i;
        for (i = 0; i < nread; i++)
        {
            if (bufp[i] == '\n')
            {
                memcpy(bufp, bufp, i + 1);
                return i + 1;
            }
        }

        if (nread > nleft)
        {
            exit(EXIT_FAILURE);
        }

        bufp += nread;
        nleft -= nread;
    }

    return -1;
}

// 向套接字中写入指定长度的数据，写不够不返回
ssize_t writen(int fd, const void *buf, size_t count)
{
    size_t needNum = count;
    ssize_t nwritten;
    const char *bufp = (const char *)buf;
    while (needNum > 0)
    {
        if ((nwritten = write(fd, bufp, needNum)) < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        else if (nwritten == 0)
            continue;

        bufp += nwritten;
        needNum -= nwritten;
    }
    return count;
}

void do_client(int sockfd)
{
    char sendbuf[1024];
    char recvbuf[1024];

    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {

        // 发送数据
        write(sockfd, sendbuf, strlen(sendbuf));

        int ret = readline(sockfd, recvbuf, sizeof(recvbuf));

        /*
        场景实现：
            当 ret == 0，但是 client 不处理，反而写入数据，
            TCP协议会生成一个RST段，并且这可能会导致客户端产生一个 SIGPIPE 信号。
         */
        printf("update  ret(%d)->1\n", ret);
        ret = 1; // 禁止客户端关闭，继续向其中写数据，产生 SIGPIPE

        if (ret == -1)
        {
            ERR_EXIT("readline");
        }
        else if (ret == 0)
        {
            printf("server close\n");
            break;
        }

        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    close(sockfd);
}

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

    int i;
    int sock[1024 * 2];
    for (i = 0; i < 1024 * 2; i++)
    {

        if ((sock[i] = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            ERR_EXIT("socket");

        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(8888);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(sock[i], (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            printf("connect error\n");
            sleep(2);
            ERR_EXIT("connect");
        }
        printf("%d\n", i);

        // struct sockaddr_in localaddr;
        // socklen_t addrlen = sizeof(localaddr);
        // if (getsockname(sock[i], (struct sockaddr *)&localaddr, &addrlen) < 0)
        //     ERR_EXIT("getsockname");
        // printf("ip=%s port=%d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
    }

    do_client(sock[0]);

    return 0;
}