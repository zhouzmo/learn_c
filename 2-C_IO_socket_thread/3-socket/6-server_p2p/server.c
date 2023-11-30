
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define ERROR_Cache(ptr)           \
    do                             \
    {                              \
        if (ptr < 0)               \
        {                          \
            perror("#zmo# error"); \
            exit(0);               \
        }                          \
    } while (0)

#define BUF_SIZE 1024

void handler(int signo)
{
    if (signo == SIGRTMIN)
    {
        printf("pid:%d recv SIGRTMIN, do exit\n", getpid());
        exit(0);
    }
}

int main(int argc, char const *argv[])
{

    pid_t pid;

    int ret;
    int sv_sk_fd, ci_sk_fd;
    struct sockaddr_in sv_addr, ci_addr; // tcp/ip 地址结构
    char recv_buf[BUF_SIZE];

    signal(SIGRTMIN, handler);

    socklen_t addrlen = sizeof(struct sockaddr);

    // socket bind listen
    {
        sv_sk_fd = socket(AF_INET, SOCK_STREAM, 0);
        ERROR_Cache(sv_sk_fd);

        memset(&sv_addr, 0, sizeof(struct sockaddr_in));
        sv_addr.sin_family = AF_INET;
        sv_addr.sin_port = htons(8888);
        sv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        ret = bind(sv_sk_fd, (struct sockaddr *)&sv_addr, addrlen);
        ERROR_Cache(ret);

        // 设置 socket 支持地址复用
        int optval = 1;
        ret = setsockopt(sv_sk_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        ERROR_Cache(ret);

        // 一旦调用 listen ,这个套接字将变为被动套接字，只能接受连接，不能主动的发送连接
        // listen 有 2个队列，完成连接队列，和未完成队列
        ret = listen(sv_sk_fd, SOMAXCONN);
        ERROR_Cache(ret);
    }

    printf("accept 阻塞等待 pid=%d\n", getpid());

    // 返回一个新的连接 已完成连接的 client socket
    ci_sk_fd = accept(sv_sk_fd, (struct sockaddr *)&ci_addr, &addrlen);
    ERROR_Cache(ci_sk_fd);
    unsigned short ci_port;
    char *ci_ipaddr;
    ci_port = ntohs(ci_addr.sin_port);
    ci_ipaddr = inet_ntoa(ci_addr.sin_addr);
    printf("client :%s:%u，连接成功\n", ci_ipaddr, ci_port);

    // 父进程，接受数据，子进程发送数据
    pid = fork();
    if (pid < 0)
    {
        perror("");
    }
    else if (pid == 0)
    {
        // 子进程发送数据

        printf("pid:%d child process ....\n", getpid());
        char send_buf[1024] = {0};

        while (fgets(send_buf, sizeof(send_buf), stdin))
        {
            // 检查字符串是否为 "quit"，如果是则中断循环
            // if (strncmp(send_buf, "quit", 4) == 0)
            //     break;

            // 写数据
            write(ci_sk_fd, send_buf, strlen(send_buf));
            memset(send_buf, 0, sizeof(send_buf));
        }
        exit(0);
    }

    // 父进程，接受数据
    printf("pid:%d parent process ....\n", getpid());
    while (1)
    {
        memset(recv_buf, 0, sizeof(recv_buf));
        ret = read(ci_sk_fd, recv_buf, sizeof(recv_buf)); // 阻塞读，直到有数据
        if (ret == 0)
        {
            // 如果在读的过程中，对方已经关闭，tcp\ip 协议会返回要给 0 数据包
            printf("parent  :%s:%u，已经关闭\n", ci_ipaddr, ci_port);
            break;
        }
        ERROR_Cache(ret);
        printf("pid:%d parent  :%s:%u 消息：", getpid(), ci_ipaddr, ci_port);

        // 服务器端收到数据，打印到屏幕
        fputs(recv_buf, stdout);
        printf("\n");
    }

    close(ci_sk_fd);
    // 发送信号给子进程，让他关闭
    kill(pid, SIGRTMIN);
    printf("pid:%d parent end....", getpid());

    return 0;
}

/*


 netstat -na | grep 8888
tcp        0      0 127.0.0.1:8888          0.0.0.0:*               LISTEN
tcp        0      0 127.0.0.1:46060         127.0.0.1:8888          ESTABLISHED
tcp        0      0 127.0.0.1:8888          127.0.0.1:46060         ESTABLISHED

ps -ef | grep -E 'out'
book       7866   7257  0 03:59 pts/6    00:00:00 ./server.out
book       7867   6937  0 03:59 pts/4    00:00:00 ./client.out
book       7868   7867  0 03:59 pts/4    00:00:00 ./client.out
book       7869   7866  0 03:59 pts/6    00:00:00 ./server.out

问题：
客户端退出，（客户端主主程序退出）
服务端退出，（服务端主主程序退出）
客户端子进程 fgets 导致也退出
服务端退出，（服务端主主程序退出）
*/