#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>

#define FDS_SIZE 100 // fds数组的大小
#define IP_ADDR "127.0.0.1" // fds数组的大小
#define PORT 6000 // fds数组的大小


// 向fds结构体数组中插入一个文件描述符
void InsertFd(struct pollfd *fds, int fd, int flag) // 此处flag是为了判断是文件描述符c，还是listenfd，来设置events
{
    for (int i = 0; i < FDS_SIZE; ++i)
    {
        if (fds[i].fd == -1)
        {
            fds[i].fd = fd;
            fds[i].events |= POLLIN;
            if (flag)
            {
                fds[i].events |= POLLRDHUP;
            }

            break;
        }
    }
}

// 从fds结构体数组中删除一个文件描述符
void DeleteFd(struct pollfd *fds, int fd)
{
    int i = 0;
    for (; i < FDS_SIZE; ++i)
    {
        if (fds[i].fd == fd)
        {
            fds[i].fd = -1;
            fds[i].events = 0;
            break;
        }
    }
}

// 断开一个用户连接
void UnlinkClient(int fd, struct pollfd *fds)
{
    close(fd);
    DeleteFd(fds, fd);
    printf("one client unlink\n");
}



int main()
{
     struct sockaddr_in ser;

    // 创建一个用于监听的socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(-1 != listenfd);

   
    memset(&ser, 0, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(PORT);
    ser.sin_addr.s_addr = inet_addr(IP_ADDR);

    int res = bind(listenfd, (struct sockaddr *)&ser, sizeof(ser));
    assert(-1 != res);

    listen(listenfd, 5);

    struct pollfd *fds = (struct pollfd *)malloc(sizeof(struct pollfd) * FDS_SIZE);
    // malloc一个fds结构体数组
    assert(NULL != fds);

    // 初始化fds结构体数组
    int i = 0;
    for (; i < FDS_SIZE; ++i)
    {
        fds[i].fd = -1;
        fds[i].events = 0;
        fds[i].revents = 0;
    }

    InsertFd(fds, listenfd, 0);
    // 插入文件描述符listenfd

    while (1)
    {
        int n = poll(fds, FDS_SIZE, -1);
        if (n <= 0)
        {
            printf("poll error\n");
            continue;
        }

        // poll返回后，处理就绪的文件描述符
        int i = 0;
        for (; i < FDS_SIZE; ++i)
        {
            if (fds[i].fd == -1)
            {
                continue;
            }

            int fd = fds[i].fd;
            if (fd == listenfd && fds[i].revents & POLLIN)
            {

                // 获取一个已完成三次握手的连接
                struct sockaddr_in cli;
                socklen_t len = sizeof(cli);
                int c = accept(fd, (struct sockaddr *)&cli, &len);
                assert(c != -1);

                printf("one client link success\n");

                InsertFd(fds, c, 1);
            }
            else if (fds[i].revents & POLLRDHUP)
            {
                UnlinkClient(fd, fds);
                // 断开连接
            }
            else if (fds[i].revents & POLLIN)
            {
                // 处理客户端发送来的数据
                char buff[128] = {0};
                int n = recv(fd, buff, 127, 0);
                if (n <= 0)
                {
                    UnlinkClient(fd, fds);
                    return;
                }

                printf("%s\n", buff);
                send(fd, "ok", 2, 0);
            }
        }

        // 处理就绪的文件描述符
    }

    free(fds);
}