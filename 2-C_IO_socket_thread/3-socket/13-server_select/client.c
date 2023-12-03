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

int do_client()
{
    int sockfd;
    if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("connect");

    return sockfd;
}

int main(void)
{

    fd_set rset;
    FD_ZERO(&rset);
    int nready;
    int maxfd;
    int fd_stdin = fileno(stdin);
    int sock = do_client();

    maxfd = fd_stdin > sock ? fd_stdin : sock;
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    int stdineof = 0;

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

            int ret = readline(sock, recvbuf, sizeof(recvbuf));
            if (ret == -1)
                ERR_EXIT("readline");
            else if (ret == 0)
            {
                printf("server close\n");
                break;
            }

            fputs(recvbuf, stdout);
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
            writen(sock, sendbuf, strlen(sendbuf));
            memset(sendbuf, 0, sizeof(sendbuf));
        }
    }

    close(sock);

    return 0;
}