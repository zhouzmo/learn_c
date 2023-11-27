#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAXFD 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define PATH "/home/book/temp/"

char path[100];
int port;

int server_sock_fd;
int epfd;
int id = 0;

struct mess
{
    int id;   // 连接名
    int type; // type = 1 连接请求   type = 2 数据传输请求
    int c;
};

int socket_init()
{

    int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1)
    {
        return -1;
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int res = bind(server_sock_fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (res == -1)
    {
        printf("bind err\n");
        return -1;
    }

    res = listen(server_sock_fd, 5);
    if (res == -1)
    {
        printf("listen err\n");
        return -1;
    }
    printf("log：%s:%d \nIP:%s\n PORT:%d\n socket_init  success\n",
           __FUNCTION__, __LINE__, SERVER_IP, SERVER_PORT);

    return server_sock_fd;
}

void epoll_add(int epfd, int fd)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLONESHOT; // 表示监听读事件

    /*
    EPOLL_CTL_ADD 将文件描述符添加到 epoll 实例
    */
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        printf("epoll add err\n");
    }
}

void epoll_del(int epfd, int fd)
{
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
    {
        printf("epoll del err\n");
    }
}

void epoll_mod(int epfd, int fd)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLONESHOT;

    if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
    {
        printf("epoll mod err\n");
    }
}

char *get_filename(char buff[])
{
    //
    char *ptr = NULL;
    char *s = strtok_r(buff, " ", &ptr); // 按照 “ ”分割字符
    if (s == NULL)
    {
        printf("请求报文错误\n");
        return NULL;
    }
    printf("请求方法:%s\n", s);
    s = strtok_r(NULL, " ", &ptr);
    if (s == NULL)
    {
        printf("请求报文无资源名字\n");
        return NULL;
    }
    if (strcmp(s, "/") == 0)
    {
        return "/index.html";
    }
    return s;
}

int send_httpfile(int c, char *filename)
{
    if (filename == NULL || c < 0)
    {
        send(c, "err", 3, 0);
        return -1;
    }

    char path[128] = {PATH};
    strcat(path, filename);
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        send_404status(c);
        return -1;
    }
    printf("log：%s:%d 待发送文件:%s\n", __FUNCTION__, __LINE__, filename);

    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char head_buff[512] = {"HTTP/1.1 200 OK\r\n"};
    strcat(head_buff, "Server: myhttp\r\n");
    sprintf(head_buff + strlen(head_buff), "Content-Length: %d\r\n", size);
    strcat(head_buff, "\r\n");

    printf("log：%s:%d 发送文件头部：%s\n", __FUNCTION__, __LINE__, head_buff);
    send(c, head_buff, strlen(head_buff), 0);
    printf("send file:\n%s\n", head_buff);

    int num = 0;
    char data[1024] = {0};
    while ((num = read(fd, data, 1024)) > 0)
    {
        send(c, data, num, 0);
    }
    printf("log：%s:%d 发送文件内容完成\n", __FUNCTION__, __LINE__);
    close(fd);

    return 0;
}

int send_404status(int c)
{
    int fd = open("err404.html", O_RDONLY);
    if (fd == -1)
    {
        send(c, "404", 3, 0);
        return 0;
    }

    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char head_buff[512] = {"HTTP/1.1 404 Not Found\r\n"};
    strcat(head_buff, "Server: myhttp\r\n");
    sprintf(head_buff + strlen(head_buff), "Content-Length: %d\r\n", size);
    strcat(head_buff, "\r\n");

    send(c, head_buff, strlen(head_buff), 0);
    printf("send 404:\n%s\n", head_buff);

    int num = 0;
    char data[1024] = {0};
    while ((num = read(fd, data, 1024)) > 0)
    {
        send(c, data, num, 0);
    }
    close(fd);

    return 0;
}

void *work_thread(void *arg)
{
    struct mess *m = (struct mess *)arg;
    int c = m->c;
    int type = m->type;
    free(m);

    if (type == 1)
    {
        printf("log：%s:%d 连接请求\n", __FUNCTION__, __LINE__);
        struct sockaddr_in caddr;
        socklen_t len = sizeof(caddr);
        int client_socket_fd = accept(server_sock_fd, (struct sockaddr *)&caddr, &len);
        if (client_socket_fd < 0)
        {
            printf("连接失败\n");
            return NULL;
        }

        printf("连接成功\n");
        epoll_add(epfd, client_socket_fd);
        return NULL;
    }
    else if (type == 2)
    {
        printf("log：%s:%d 数据传输请求\n", __FUNCTION__, __LINE__);

        char buff[1024] = {0};
        int num = recv(c, buff, sizeof(buff), 0);
        if (num <= 0)
        {
            printf("log：%s:%d 无数据接受\n", __FUNCTION__, __LINE__);
            epoll_del(epfd, c);
            close(c);
            return NULL;
        }

        buff[num] = '\0';
        char *filename = get_filename(buff);
        printf("请求文件名字: %s\n", filename);
        send_httpfile(c, filename);

        epoll_mod(epfd, c);
        return NULL;
        // }

        return NULL;
    }
}

void *epoll_thread(void *arg)
{
    printf("log：%s:%d 开始创建 epoll 线程(create epoll thread start)...\n", __FUNCTION__, __LINE__);
    struct epoll_event evs[MAXFD];
    while (1)
    {
        int nready = epoll_wait(epfd, evs, MAXFD, -1); // 阻塞等待事件
        if (nready == -1)
        {
            printf("epoll wait err\n");
            return NULL;
        }
        printf("log：%s:%d 有事件发生 (epoll_wait end)\n", __FUNCTION__, __LINE__);
        int i;
        for (i = 0; i < nready; i++)
        {
            // 监听是否有新的连接请求
            int fd = evs[i].data.fd;
            if (evs[i].data.fd == server_sock_fd && evs[i].events & EPOLLIN)
            {
                // server_sock_fd 发生输入事件，需要接受请求，并将 client_socket_fd 加入 epfd 来监听
                pthread_t tid;
                struct mess *m = (struct mess *)malloc(sizeof(struct mess));
                m->type = 1;
                m->c = fd;
                m->id = id++;
                printf("log：%s:%d 创建处理线程 (create work_thread) id=%d\n", __FUNCTION__, __LINE__, id);
                pthread_create(&tid, NULL, work_thread, (void *)m);
            }
            else if (evs[i].events & EPOLLIN)
            {
                // 处理数据
                pthread_t tid;
                struct mess *m = (struct mess *)malloc(sizeof(struct mess));
                m->type = 2;
                m->c = fd;
                m->id = id++;
                printf("log：%s:%d 创建处理线程 (create work_thread) id=%d\n", __FUNCTION__, __LINE__, id);
                pthread_create(&tid, NULL, work_thread, (void *)m);
            }

            // if (evs[i].events & EPOLLIN)
            // {
            //     pthread_t tid;
            //     struct mess *m = (struct mess *)malloc(sizeof(struct mess));
            //     m->type = 1;
            //     m->c = fd;
            //     m->id = id++;
            //     printf("log：%s:%d 创建处理线程 (create work_thread) id=%d\n", __FUNCTION__, __LINE__, id);
            //     pthread_create(&tid, NULL, work_thread, (void *)m);
            // }
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    if (getcwd(path, sizeof(path)) != NULL)
    {
        printf("当前文件路径: %s\n", path);
    }

    server_sock_fd = socket_init();
    if (server_sock_fd == -1)
    {
        printf("socket init err\n");
        return -1;
    }

    epfd = epoll_create(MAXFD);
    if (epfd == -1)
    {
        printf("epoll create err\n");
        return -1;
    }

    epoll_add(epfd, server_sock_fd);

    pthread_t tid;
    pthread_create(&tid, NULL, epoll_thread, NULL);

    while (1)
    {
        printf("log：%s:%d 每秒创建一个接收\n", __FUNCTION__, __LINE__);
        struct mess *m = (struct mess *)malloc(sizeof(struct mess));
        m->type = 1;
        pthread_t tid;
        pthread_create(&tid, NULL, work_thread, (void *)m);
        sleep(1);
    }

    close(server_sock_fd);
    close(epfd);

    return 0;
}

/*
以下是 epoll 常用函数的简要文档说明：

1. `int epoll_create(int size)`
   - 功能：创建一个 epoll 实例。
   - 参数：
     - `size`：epoll 实例内核事件表的大小，通常设置为大于 0 的整数（在较新的内核版本中，该参数被忽略）。
   - 返回值：
     - 成功：返回一个新的 epoll 实例的文件描述符。
     - 失败：返回 -1，并设置 errno。

2. `int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)`
   - 功能：控制 epoll 实例上的文件描述符事件。
   - 参数：
     - `epfd`：epoll 实例的文件描述符。
     - `op`：操作类型，可以是 `EPOLL_CTL_ADD`、`EPOLL_CTL_MOD` 或 `EPOLL_CTL_DEL`，分别表示添加、修改或删除事件。
     - `fd`：要操作的文件描述符。
     - `event`：指向 `struct epoll_event` 结构体的指针，描述要进行的操作和相关数据。
   - 返回值：
     - 成功：返回 0。
     - 失败：返回 -1，并设置 errno。

3. `int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)`
   - 功能：等待事件的发生。
   - 参数：
     - `epfd`：epoll 实例的文件描述符。
     - `events`：指向 `struct epoll_event` 数组的指针，用于存储发生的事件。
     - `maxevents`：`events` 数组的大小，即最多可以存放多少个事件。
     - `timeout`：等待事件的超时时间，以毫秒为单位。可以传递 -1 表示无限等待，0 表示立即返回，正整数表示等待的毫秒数。
   - 返回值：
     - 成功：返回发生的事件数量。
     - 失败：返回 -1，并设置 errno。

`struct epoll_event` 结构体用于描述事件和相关数据：
```c
struct epoll_event {
    __uint32_t events;  // 表示事件类型的位掩码，可以是 EPOLLIN、EPOLLOUT、EPOLLERR 等。
    epoll_data_t data;  // 与事件相关的数据，可以是文件描述符、指针等。
};

typedef union epoll_data {
    void *ptr;         // 指针类型的数据
    int fd;            // 文件描述符类型的数据
    __uint32_t u32;    // 32 位无符号整数类型的数据
    __uint64_t u64;    // 64 位无符号整数类型的数据
} epoll_data_t;


以下是 strtok_r 函数的基本用法：
char *strtok_r(char *str, const char *delim, char **saveptr);
str：要分割的字符串。第一次调用时传入非空指针，后续调用传入 NULL。
delim：分割字符串的分隔符。
saveptr：用于保存分割位置的指针，必须是一个指向 char* 类型的指针的指针。
strtok_r 函数会返回分割后的子字符串，并将 saveptr 指向下一个分割位置。当没有更多的子字符串可分割时，函数返回 NULL。

在给定的代码中，s 是一个指向字符数组的指针，用于保存分割后的子字符串。ptr 是一个 char** 类型的指针，用于保存分割位置的信息。
*/