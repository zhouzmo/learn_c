#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
        问题：僵尸进程解决
open server
open client
close client
出现 server child  <defunct>僵尸

解决方案：
1  signal(SIGCHLD, SIG_IGN); // 忽略
2 wait waitpid

*/

struct packet
{
    int len;
    char buf[1024];
};

#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

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
    int retlen;
    while (1)
    {
        memset(&recvbuf, 0, sizeof(recvbuf));

        // 先检查长度
        int ret = readn(conn, &recvbuf.len, 4);
        if (ret == -1)
            ERR_EXIT("read");
        else if (ret < 4)
        {
            printf("client closed\n");
            break;
        }

        // 根据获得的字节长度，读指定的字节
        retlen = ntohl(recvbuf.len);
        printf("3.先读 4 字节，获取客户端发送的recvbuf.len=%d,继续读%d\n", retlen, retlen);

        ret = readn(conn, &recvbuf.buf, retlen);
        printf("4.成功读到了%d字节，内容：%s\n", ret, recvbuf.buf);
        if (ret == -1)
            ERR_EXIT("read");
        else if (ret < retlen)
        {
            printf("client closed\n");
            break;
        }

        // 输出到命令行，回写
        printf("5.将数据重新发回客户端recvbuf.len=%d,recvbuf.buf=%s\n", retlen, recvbuf.buf);

        write(conn, &recvbuf, retlen + 4);
    }

    close(conn);
}

void sig_handler(int signum)
{
    printf("process pid:%d recv SIGCHLD\n ", getpid());
    wait(NULL);
}

int main(void)
{
    // 当客户端异常断开后,服务端子进程会变成僵尸进程,解决
    // signal(SIGCHLD, SIG_IGN); // 忽略

    signal(SIGCHLD, sig_handler);

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
