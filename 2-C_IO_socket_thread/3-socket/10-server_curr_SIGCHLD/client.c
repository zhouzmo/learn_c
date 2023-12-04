#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

void do_client(int sockfd)
{
    char sendbuf[1024];
    char recvbuf[1024];

    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        write(sockfd, sendbuf, strlen(sendbuf));

        int ret = readline(sockfd, recvbuf, sizeof(recvbuf));
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

int main(void)
{
    int i;
    int connArr[100];
    for (i = 0; i < 5; i++)
    {

        int sockfd;
        if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            ERR_EXIT("socket");

        connArr[i] = sockfd;

        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(8888);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
            ERR_EXIT("connect");

        struct sockaddr_in localaddr;
        socklen_t addrlen = sizeof(localaddr);
        if (getsockname(sockfd, (struct sockaddr *)&localaddr, &addrlen) < 0)
            ERR_EXIT("getsockname");
        printf("ip=%s port=%d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
    }

    do_client(connArr[0]);

    return 0;
}