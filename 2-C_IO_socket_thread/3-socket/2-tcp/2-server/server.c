
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

socket 函数用于创建一个套接字（socket），它是进行网络通信的基础。下面是对 socket 函数的详细解释以及不同的标志（flags）之间的区别：

int socket(int domain, int type, int protocol);
domain 参数指定了套接字的通信域（communication domain），它决定了套接字可以用于哪种类型的网络通信。常见的域包括：

AF_INET：IPv4 互联网域，用于进行基于 IPv4 的网络通信。
AF_INET6：IPv6 互联网域，用于进行基于 IPv6 的网络通信。
AF_LOCAL 或 AF_UNIX：本地域（Unix 域），用于在同一台计算机上的进程之间进行通信。
type 参数指定了套接字的类型，它决定了套接字的特性和支持的通信模式。常见的类型包括：

SOCK_STREAM：流式套接字，提供面向连接的、可靠的、基于字节流的通信。使用 TCP 协议。
SOCK_DGRAM：数据报套接字，提供无连接的、不可靠的、固定大小的数据包通信。使用 UDP 协议。
SOCK_RAW：原始套接字，提供对底层网络协议的直接访问。
protocol 参数指定了套接字使用的协议。通常情况下，可以将其设置为 0，让系统根据 domain 和 type 参数选择适合的协议。如果需要显式指定协议，常见的协议包括：

IPPROTO_TCP：TCP 协议。
IPPROTO_UDP：UDP 协议。




*/
int main(int argc, char const *argv[])
{

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

    // 设置 socket 支持地址复用 SO_REUSEADDR SO_REUSEPORT
    int optval = 1;
    ret = setsockopt(sv_sk_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    ret = setsockopt(sv_sk_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    ERROR_Cache(ret);

    // 一旦调用 listen ,这个套接字将变为被动套接字，只能接受连接，不能主动的发送连接
    // listen 有 2个队列，完成连接队列，和未完成队列
    ret = listen(sv_sk_fd, SOMAXCONN);
    ERROR_Cache(ret);
    printf("server listen %d 成功\n", SOMAXCONN);

    // 返回一个新的连接 已完成连接的 client socket
    ci_sk_fd = accept(sv_sk_fd, (struct sockaddr *)&ci_addr, &addrlen);
    ERROR_Cache(ci_sk_fd);
    unsigned short ci_port;
    char *ci_ipaddr;
    ci_port = ntohs(ci_addr.sin_port);
    ci_ipaddr = inet_ntoa(ci_addr.sin_addr);
    printf("client :%s:%u，连接成功\n", ci_ipaddr, ci_port);

    while (1)
    {
        ret = read(ci_sk_fd, recv_buf, sizeof(recv_buf));
        if (ret == 0)
        {
            // 如果在读的过程中，对方已经关闭，tcp\ip 协议会返回要给 0 数据包
            printf("对方已经关闭\n");
            break;
            ;
        }
        ERROR_Cache(ret);

        printf("client：");
        fputs(recv_buf, stdout);
        // 服务器端收到数据，打印到屏幕
        write(ci_sk_fd, recv_buf, ret); // 服务器端发送数据
        if (!strcmp("server close\n", recv_buf))
        {
            break;
        }

        memset(recv_buf, 0, sizeof(recv_buf));
    }
    close(ci_sk_fd);
    return 0;
}

/*
复现失败  我的是FIN_WAIT2 不是 TIME_WAIT
复现失败  我的是 TIME_WAIT 也不行，？？？？
open server
open client
close server
再次运行，服务端表示端口被占用
需要设置地址复用
允许套接字绑定到一个处于 TIME_WAIT 状态的地址和端口。
这对于服务器在关闭后立即重新启动很有用，
因为不需要等待 TIME_WAIT 状态的套接字释放。
$ netstat -na |grep 8888
tcp        1      0 127.0.0.1:36782         127.0.0.1:8888          CLOSE_WAIT
tcp        0      0 127.0.0.1:8888          127.0.0.1:36782         FIN_WAIT2


*/

/*
问题：不支持并发
open server
open client1
open client2
book@100ask:~$ netstat -na |grep 8888
tcp        1      0 127.0.0.1:8888          0.0.0.0:*               LISTEN
tcp        0      0 127.0.0.1:8888          127.0.0.1:40566         ESTABLISHED
tcp        0      0 127.0.0.1:49464         127.0.0.1:8888          ESTABLISHED
tcp        0      0 127.0.0.1:40566         127.0.0.1:8888          ESTABLISHED
tcp        4      0 127.0.0.1:8888          127.0.0.1:49464         ESTABLISHED


*/