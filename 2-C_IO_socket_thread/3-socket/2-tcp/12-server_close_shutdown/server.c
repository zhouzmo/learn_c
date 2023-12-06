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

close()采用**引用计数技术**，关闭的只是一个fd，
系统会将socket 对应的计数 -1，
当socket 的计数为 0 时，才会真正的关闭。

shutdown可以有选择地终止数据传输的方向或两个方向，并
提供更细粒度的控制。




*/

/*

#include <sys/socket.h>

int shutdown(int sockfd, int how);
该函数接受两个参数：

sockfd：表示要关闭的套接字的文件描述符。

how：表示关闭套接字的方式，可以是以下几个值之一：

SHUT_RD：关闭套接字的读取功能。在调用shutdown函数后，套接字将不能再接收数据。
SHUT_WR：关闭套接字的写入功能。在调用shutdown函数后，套接字将不能再发送数据。
SHUT_RDWR：关闭套接字的读写功能。在调用shutdown函数后，套接字将不能再接收和发送数据。
函数返回值为0表示成功，返回值为-1表示出错。

注意：shutdown函数只是关闭套接字的功能，不会释放套接字的资源。如果需要完全释放套接字的资源，还需要调用close函数。

*/
/*

问题：
为何文件描述符没变，不同进程获取的 8888 信息却变了
回答：



 ip=127.0.0.1 port=39138
ip=127.0.0.1 port=39152
ip=127.0.0.1 port=39168
ip=127.0.0.1 port=39170
ip=127.0.0.1 port=39174
pid:124839,ip=127.0.0.1 port=8888



client
lrwx------ 1 book book 64 Dec  1 13:14 0 -> /dev/pts/4
lrwx------ 1 book book 64 Dec  1 13:14 1 -> /dev/pts/4
lrwx------ 1 book book 64 Dec  1 13:14 2 -> /dev/pts/4
lrwx------ 1 book book 64 Dec  1 13:14 3 -> 'socket:[8327682]'
lrwx------ 1 book book 64 Dec  1 13:14 4 -> 'socket:[8327683]'
lrwx------ 1 book book 64 Dec  1 13:14 5 -> 'socket:[8327684]'
lrwx------ 1 book book 64 Dec  1 13:14 6 -> 'socket:[8327685]'
lrwx------ 1 book book 64 Dec  1 13:14 7 -> 'socket:[8327686]'



server parent process
lrwx------ 1 book book 64 Dec  1 13:15 0 -> /dev/pts/3
lrwx------ 1 book book 64 Dec  1 13:15 1 -> /dev/pts/3
lrwx------ 1 book book 64 Dec  1 13:15 2 -> /dev/pts/3
lrwx------ 1 book book 64 Dec  1 13:15 3 -> 'socket:[8136406]'

server child process
lrwx------ 1 book book 64 Dec  1 13:17 0 -> /dev/pts/3
lrwx------ 1 book book 64 Dec  1 13:17 1 -> /dev/pts/3
lrwx------ 1 book book 64 Dec  1 13:17 2 -> /dev/pts/3
lrwx------ 1 book book 64 Dec  1 13:17 4 -> 'socket:[8329281]'



*/
#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

static char *ipaddr;
static unsigned short port;

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

void printfIPAndPort(int sockfd)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr *)&addr, &addrlen) < 0)
        ERR_EXIT("getsockname");
    printf("pid:%d,fd=%d ip=%s port=%d\n", getpid(), sockfd, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
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

        if (!strcmp("server quit\n", recvbuf))
        {
            /*
           shutdown  关闭 server 发送数据能力，即向 socket 中写入 '\0'
            然后打印 client 最后一次数据 server quit

            client write 数据后，立刻 read ，发现  '\0'，于是close

            server 在 writen 失败后，进程死了？SIGCHLD

            */
            shutdown(conn, SHUT_WR);
            // close(conn);
            // break;
        }

        // 将接收到的数据输出到标准输出
        // printfIPAndPort(conn);
        printf("pid:%d,fd=%d ip=%s port=%d\n", getpid(), conn, ipaddr, port);

        fputs(recvbuf, stdout);

        // 回写数据给客户端
        writen(conn, &recvbuf, ret);
    }

    close(conn);
    exit(EXIT_SUCCESS);
}

/*
success 3
terminated 5
虽然只接受到 3 个信号，但是仍然处理了 5 个子进程，
*/
void sig_handler(int signum)
{
    int mypid = 0;
    int saved_errno = errno; // 保存 errno，以便在 waitpid 调用后恢复
    int status;
    printf("process pid:%d recv SIGCHLD\n ", getpid());
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

        // printf("conn = %d ", conn);
        printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
        ipaddr = inet_ntoa(peeraddr.sin_addr);
        port = ntohs(peeraddr.sin_port);

        pid = fork();
        if (pid == -1)
            ERR_EXIT("fork");
        if (pid == 0)
        {
            close(listenfd);
            do_service(conn);
            // exit(EXIT_SUCCESS);
        }
        else
        {
            close(conn);
        }
    }

    return 0;
}
