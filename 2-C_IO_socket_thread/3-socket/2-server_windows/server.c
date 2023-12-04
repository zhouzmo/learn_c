#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define ERROR_Cache(ptr) \
    do                   \
    {                    \
        if (ptr < 0)     \
        {                \
            perror("error"); \
            exit(0);     \
        }                \
    } while (0)

int main()
{
    WSADATA wsaData;
    int ret;
    SOCKET sv_sk_fd, ci_sk_fd;
    struct sockaddr_in sv_addr, ci_addr;
    char recv_buf[1024];

    socklen_t addrlen = sizeof(struct sockaddr);

    // 初始化Winsock库
    ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret != 0)
    {
        printf("WSAStartup failed: %d\n", ret);
        return 1;
    }

    sv_sk_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_Cache(sv_sk_fd);

    memset(&sv_addr, 0, sizeof(struct sockaddr_in));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(8888);
    sv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = bind(sv_sk_fd, (struct sockaddr *)&sv_addr, addrlen);
    ERROR_Cache(ret);

    // 一旦调用 listen ，这个套接字将变为被动套接字，只能接受连接，不能主动的发送连接
    // listen 有 2个队列，完成连接队列，和未完成队列
    ret = listen(sv_sk_fd, SOMAXCONN);
    ERROR_Cache(ret);
    printf("server listen %d 成功\n", SOMAXCONN);

    // 返回一个新的连接已完成连接的 client socket
    ci_sk_fd = accept(sv_sk_fd, (struct sockaddr *)&ci_addr, &addrlen);
    ERROR_Cache(ci_sk_fd);
    unsigned short ci_port;
    char *ci_ipaddr;
    ci_port = ntohs(ci_addr.sin_port);
    ci_ipaddr = inet_ntoa(ci_addr.sin_addr);
    printf("client :%s:%u，连接成功\n", ci_ipaddr, ci_port);

    while (1)
    {
        ret = recv(ci_sk_fd, recv_buf, sizeof(recv_buf), 0);
        if (ret == 0)
        {
            // 如果在读的过程中，对方已经关闭，TCP/IP 协议会返回一个 0 数据包
            printf("对方已经关闭\n");
            break;
        }
        ERROR_Cache(ret);

        printf("client：");
        fputs(recv_buf, stdout);
        // 服务器端收到数据，打印到屏幕
        send(ci_sk_fd, recv_buf, ret, 0); // 服务器端发送数据
        if (!strcmp("server close\n", recv_buf))
        {
            break;
        }

        memset(recv_buf, 0, sizeof(recv_buf));
    }
    closesocket(ci_sk_fd);
    closesocket(sv_sk_fd);

    // 清理Winsock库
    WSACleanup();

    return 0;
}



/*
gcc server.c -lws2_32 -o server.exe
gcc client.c -lws2_32 -o client.exe


*/