#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define ERR_EXIT(msg)       \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

int main()
{
    int i;
    int client[FD_SETSIZE]; // 支持的最大客户端连接数
    int maxi = -1;          // 最大不空闲位置
    int listenfd = -1;
    int nready;
    int maxfd = -1;
    fd_set rset, allset;

    printf("FD_SETSIZE = %d\n", FD_SETSIZE);

    for (i = 0; i < FD_SETSIZE; ++i)
        client[i] = -1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
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

    maxfd = listenfd;
    FD_ZERO(&allset);
    // ==将 listenfd 放入读监听set，当有连接可读时，会被检测到
    FD_SET(listenfd, &allset);

    while (1)
    {
        printf("wait select...\n");
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready == -1)
        {
            if (errno == EINTR)
                continue;
            ERR_EXIT("select");
        }

        if (nready == 0)
            continue;

        printf("select 监听到的读事件数量：nready = %d\n", nready);
        // ==listenfd 发送读事件，通过 accept 接收连接fd，将 fd 保存到client数组
        if (FD_ISSET(listenfd, &rset))
        {
            struct sockaddr_in peeraddr;
            socklen_t peerlen = sizeof(peeraddr);
            int conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen);
            if (conn == -1)
                ERR_EXIT("accept");

            for (i = 0; i < FD_SETSIZE; ++i)
            {
                if (client[i] == -1)
                {
                    client[i] = conn;
                    if (i > maxi)
                        maxi = i;
                    break;
                }
            }

            if (i == FD_SETSIZE)
            {
                fprintf(stderr, "too many clients\n");
                exit(EXIT_FAILURE);
            }

            printf("client connected: %s:%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
            // ==将 conn_fd 加入读监听set，当有连接可读时，会被检测到
            FD_SET(conn, &allset);
            if (conn > maxfd)
                maxfd = conn;

            if (--nready <= 0)
                continue;
        }
        // 若不是 listen_fd 的读事件，遍历 client 数组，循环检测出对应的 conn_fd ，接收，打印，回传
        printf("select 监听到的读事件中由于 client 发送引起读事件的数量：nready = %d\n", nready);
        for (i = 0; i <= maxi; ++i)
        {
            int conn = client[i];
            if (conn == -1)
                continue;

            if (FD_ISSET(conn, &rset))
            {
                char recvbuf[1024] = {0};
                int ret = recv(conn, recvbuf, sizeof(recvbuf), 0);
                if (ret == -1)
                    ERR_EXIT("recv");
                if (ret == 0)
                {
                    printf("client closed\n");
                    // client socket 已经关闭，从 client 数组，监听set，清除该 fd
                    FD_CLR(conn, &allset);
                    client[i] = -1;
                    close(conn);
                }
                else
                {
                    printf("client msg:");
                    fputs(recvbuf, stdout);
                    send(conn, recvbuf, ret, 0);
                }

                // 当监听到的事件处理完毕时，继续 select 等待事件处理
                if (--nready <= 0)
                    break;
            }
        }
    }

    close(listenfd);

    return 0;
}