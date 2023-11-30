
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define ERROR_Cache(ptr)     \
    do                       \
    {                        \
        if (ptr < 0)         \
        {                    \
            perror("error"); \
            exit(0);         \
        }                    \
    } while (0)

/*

*/
int main(int argc, char const *argv[])
{

    pid_t pid;

    int ret;
    int sv_sk_fd, ci_sk_fd;
    struct sockaddr_in sv_addr, ci_addr; // tcp/ip 地址结构
    char recv_buf[1024];

    socklen_t addrlen = sizeof(struct sockaddr);

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

    while (1)
    {
        printf("accept 阻塞等待 pid=%d\n", getpid());

        // 返回一个新的连接 已完成连接的 client socket
        ci_sk_fd = accept(sv_sk_fd, (struct sockaddr *)&ci_addr, &addrlen);
        ERROR_Cache(ci_sk_fd);
        unsigned short ci_port;
        char *ci_ipaddr;
        ci_port = ntohs(ci_addr.sin_port);
        ci_ipaddr = inet_ntoa(ci_addr.sin_addr);
        printf("client :%s:%u，连接成功\n", ci_ipaddr, ci_port);

        // 来一个客户端，创建一个子进程
        pid = fork();
        if (pid == 0)
        {
            // 子进程保持客户端连接的同时，
            close(sv_sk_fd); // 子进程不需要监听，关闭节省文件描述符资源
            printf("pid:%d child process ....\n", getpid());
            while (1)
            {
                memset(recv_buf, 0, sizeof(recv_buf));
                ret = read(ci_sk_fd, recv_buf, sizeof(recv_buf)); // 阻塞读，直到有数据
                if (ret == 0)
                {
                    // 如果在读的过程中，对方已经关闭，tcp\ip 协议会返回要给 0 数据包
                    printf("client  :%s:%u，已经关闭\n", ci_ipaddr, ci_port);
                    exit(0);
                }
                ERROR_Cache(ret);
                printf("pid:%d client  :%s:%u 消息：", getpid(), ci_ipaddr, ci_port);
                fputs(recv_buf, stdout);
                // 服务器端收到数据，打印到屏幕
                write(ci_sk_fd, recv_buf, ret); // 服务器端发送数据
            }
        }
        else if (pid > 0)
        {
            printf("pid:%d parent process ....\n", getpid());
            close(ci_sk_fd); // 父进程不需要 ci_sk_fd
        }
        else
        {
            printf("pid:%d parent process ....\n", getpid());
        }
    }
    return 0;
}

/*


book@100ask:~$ netstat -na |grep 8888
tcp        0      0 127.0.0.1:8888          0.0.0.0:*               LISTEN
tcp        0      0 127.0.0.1:46666         127.0.0.1:8888          ESTABLISHED
tcp        0      0 127.0.0.1:8888          127.0.0.1:46666         ESTABLISHED
tcp        0      0 127.0.0.1:8888          127.0.0.1:46664         ESTABLISHED
tcp        0      0 127.0.0.1:46664         127.0.0.1:8888          ESTABLISHED

*/

/*
子进程为何需要  close(sv_sk_fd); 父进程 close(ci_sk_fd);
关闭不需要的套接字描述符有助于释放系统资源，
并且可以避免在多进程环境下可能出现的意外行为。

需要注意的是，关闭套接字描述符并不会影响其他进程或线程对其的访问，
因为每个进程都有自己独立的文件描述符表。
*/