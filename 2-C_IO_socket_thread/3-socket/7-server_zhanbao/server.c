#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/*

定长包

常见的解决粘包问题的方法：

1. **定长包**：
	- 一种简单的方法是在应用层规定每个消息的固定长度，这样接收方就可以根据固定长度来准确地划分消息，避免粘包问题。

2. **包尾加标识符**：
	- 在每个消息的末尾添加特定的标识符，接收方通过检测这个标识符来确定消息的边界。类似于 FTP 协议在每个文件末尾添加 "EOF" 标识符的做法。

3. **包头加上包体长度**：
	- 在消息的开头加上描述消息长度的头部信息，接收方首先读取头部信息以确定消息长度，然后再读取相应长度的数据。这种方法常见于很多网络协议中。

4. **更复杂的应用层协议**：
	- 一些更复杂的应用层协议会在消息格式中包含消息长度字段或者起始/结束标记，以便接收方能够准确地解析消息。
*/


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

// 从套接字中读取指定长度的数据，读不够不返回
ssize_t readn(int fd, void *buf, size_t count)
{
    size_t needNum = count; // 剩余需要读取的字节数
    ssize_t readed;
    char *bufp = (char *)buf;
    while (needNum > 0)
    {
        if ((readed = read(fd, bufp, needNum)) < 0)
        {
            if (errno == EINTR) // 可中断睡眠
                continue;
            return -1;
        }
        else if (readed == 0) // 若对方已关闭
        {
            return count - needNum;
        }

        // 正常情况下，read 成功读取数据，修改 bufp，让下一次读从未读的缓冲区获取
        bufp += readed;    // bufp + readed已经读取的字节数
        needNum -= readed; // 需要读的字节数 - 已经读的字节数
    }
    return count;
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

// 处理客户端请求
void do_service(int conn)
{
    struct packet recvbuf;
    int n;
    while (1)
    {
        memset(&recvbuf, 0, sizeof(recvbuf));

        // 读取包头，4个字节表示数据长度
        int ret = readn(conn, &recvbuf.len, 4);
        if (ret == -1)
            ERR_EXIT("read");
        else if (ret < 4)
        {
            printf("client closed\n");
            break;
        }
        printf("3.server readn 从 buf 中读 4 字节数\n");

        // 根据长度读取数据
        n = ntohl(recvbuf.len);
        printf("字节数%d\n", n);
        ret = readn(conn, recvbuf.buf, n);
        if (ret == -1)
            ERR_EXIT("read");
        else if (ret < n)
        {
            printf("client closed\n");
            break;
        }
        printf("4.server readn 从 buf 中读 len(%d) 字节数\n", ret);

        // 将接收到的数据输出到标准输出
        fputs(recvbuf.buf, stdout);

        // 回写数据给客户端
        writen(conn, &recvbuf, 4 + n);
        printf("5.server send 4 + n(%d) 字节数\n", ret);
    }

    close(conn);
}

int main(void)
{
    int listenfd;
    if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
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
    socklen_t peerlen = sizeof(peeraddr);
    int conn;

    pid_t pid;
    while (1)
    {
        if ((conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen)) < 0)
            ERR_EXIT("accept");

        printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

        pid = fork();
        if (pid == -1)
            ERR_EXIT("fork");
        if (pid == 0)
        {
            close(listenfd);
            do_service(conn);
            exit(EXIT_SUCCESS);
        }
        else
        {
            close(conn);
        }
    }

    return 0;
}

/*
流程，
client send 4+n byte data
server read 4 byte data



*/