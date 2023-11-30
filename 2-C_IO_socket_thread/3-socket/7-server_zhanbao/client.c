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

struct packet
{
    int len;
    char buf[1024];
};

ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count;
    ssize_t nread;
    char *bufp = (char *)buf;
    while (nleft > 0)
    {
        if ((nread = read(fd, bufp, nleft)) < 0) // 可中断睡眠
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        else if (nread == 0) // 若对方已关闭
        {
            return count - nleft;
        }
        bufp += nread;
        nleft -= nread;
    }
    return count;
}

ssize_t writen(int fd, const void *buf, size_t count)
{

    printf("writen 字节数：%ld\n", count);
    size_t nleft = count;
    ssize_t nwritten;
    const char *bufp = (const char *)buf;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, bufp, nleft)) < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        else if (nwritten == 0)
            continue;
        bufp += nwritten;
        nleft -= nwritten;
    }
    return count;
}

int main()
{
    int sock;
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("connect");

    struct packet sendbuf;
    struct packet recvbuf;
    memset(&sendbuf, 0, sizeof(sendbuf));
    memset(&recvbuf, 0, sizeof(recvbuf));
    int n;

    while (fgets(sendbuf.buf, sizeof(sendbuf.buf), stdin) != NULL)
    {
        n = strlen(sendbuf.buf);
        sendbuf.len = htonl(n);
        printf("1.控制台获取字节数：%d\n", n);
        int writenCount = writen(sock, &sendbuf, 4 + n);
        printf("2.client send 4 + n byte data 字节数：%d\n", writenCount);

        int ret = readn(sock, &recvbuf.len, 4);
        printf("6.client read 4  字节数%d\n", ret);
        if (ret == -1)
            ERR_EXIT("read");
        else if (ret < 4)
        {
            printf("client closed\n");
            break;
        }

        n = ntohl(recvbuf.len);
        ret = readn(sock, recvbuf.buf, n);
        printf("readn buf success 字节数：%d\n", ret);
        if (ret == -1)
            ERR_EXIT("read");
        else if (ret < n)
        {
            printf("client closed\n");
            break;
        }
        printf("6.client read len 字节数%d\n", ret);
        fputs(recvbuf.buf, stdout);

        memset(&sendbuf, 0, sizeof(sendbuf));
        memset(&recvbuf, 0, sizeof(recvbuf));
    }

    close(sock);
    return 0;
}
