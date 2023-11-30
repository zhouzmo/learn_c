#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*


ssize_t recv(int s, void *buf, size_t len, int flags);

与 `read` 函数相比，`recv` 函数只能用于套接字文件描述符，并且多了一个 `flags` 参数。下面是关于 `MSG_OOB` 和 `MSG_PEEK` 标志位的描述：

- `MSG_OOB` 标志请求接收带外数据（Out-Of-Band data），这些数据不会通过正常的数据流接收。一些协议将紧急数据放在正常数据队列的开头，因此在这种协议中不能使用 `MSG_OOB` 标志。

- `MSG_PEEK` 标志导致接收操作返回接收队列的开头数据，但不将数据从队列中移除。这意味着后续的接收调用仍将返回相同的数据。利用此特点可以方便地实现按行读取数据等操作。相比一个一个字符地读取，多次调用系统调用 `read` 方法，使用 `MSG_PEEK` 标志可以更高效地实现按行读取数据的功能。

`recv` 函数会将套接字缓冲区中的内容读出，但不清空。与 `read` 函数的区别在于 `recv` 函数有一个 `flags` 标志位，可以设置为 `MSG_PEEK`。

另外，`send` 函数用于将缓冲区中的内容写入到套接字，也不清空，与 `write` 函数的区别在于 `send` 函数用于套接字文件描述符，并且可以实现特定于套接字的发送操作。





*/

/*

包尾加标识符

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

ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    while (1)
    {
        int ret = recv(sockfd, buf, len, MSG_PEEK);
        if (ret == -1 && errno == EINTR)
            continue;
        return ret;
    }
}

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

ssize_t readline(int sockfd, void *buf, size_t maxline)
{
    int ret;
    int nread;
    char *bufp = buf;
    int nleft = maxline;

    while (1)
    {
        // 看一下缓冲区有没有数据，并不移除内核缓冲区数据
        ret = recv_peek(sockfd, bufp, nleft);
        if (ret < 0)
        { // 失败
            return ret;
        }
        else if (ret == 0)
        { // 对方已关闭
            return ret;
        }

        nread = ret;
        int i;
        for (i = 0; i < nread; i++)
        {
            if (bufp[i] == '\n')
            {                                     // 若缓冲区有换行符
                ret = readn(sockfd, bufp, i + 1); // 读走数据
                if (ret != i + 1)
                {
                    exit(EXIT_FAILURE);
                }
                return ret; // 有换行符就返回，并返回读走的数据
            }
        }

        if (nread > nleft)
        { // 如果读到的数据大于一行最大数，异常处理
            exit(EXIT_FAILURE);
        }

        nleft -= nread; // 若缓冲区没有换行符，把剩余的数据读走
        ret = readn(sockfd, bufp, nread);
        if (ret != nread)
        {
            exit(EXIT_FAILURE);
        }
    }
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
    char recvbuf[1024];
    while (1)
    {
        memset(&recvbuf, 0, sizeof(recvbuf));

        int ret = readline(conn, recvbuf, 1024);
        if (ret == -1)
        {
            ERR_EXIT("readline");
        }
        else if (ret == 0)
        {
            printf("client close\n");
            break;
        }

        // 将接收到的数据输出到标准输出
        fputs(recvbuf, stdout);

        // 回写数据给客户端
        writen(conn, &recvbuf, ret);
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
