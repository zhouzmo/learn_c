#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

/* 公共部分*/
// 异常判断
#define CHECK_RET(ptr, failed_msg)      \
    do                                  \
    {                                   \
        if ((ptr) == NULL && (ptr) < 0) \
        {                               \
            perror(failed_msg);         \
            exit(EXIT_FAILURE);         \
        }                               \
    } while (0)

#define MAX_MSG_LEN 1024
#define SERVER_PORT 8888
#define BUF_SIZE 4096

struct message
{
    char username[50];
    char msg_content[MAX_MSG_LEN];
}

void *
recv_msg(void *arg);

int main(int argc, char **argv)
{

    int client_fd;
    int ret;
    int send_len;
    pthread_t tid;

    struct sockaddr_in sockaddr_in;
    unsigned char send_buf[1000];

    if (argc != 2)
    {
        printf("please input server_ip\n");
        return -1;
    }
    char *serverIP = argv[1];

    /*=== 1、创建 socket 网络文件描述符===*/
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_RET(client_fd);

    sAddrIn.sin_family = AF_INET;
    sAddrIn.sin_port = htons(SERVER_PORT);
    sAddrIn.sin_addr.s_addr = inet_addr(serverIP);

    /*=== 2、发起与服务器的连接===*/
    /*
    这行代码用于在客户端套接字 (`client_fd`) 上发起与服务器的连接。

   int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

   - `sockfd`：这是客户端套接字的描述符，通常是通过 `socket` 创建的套接字。它将用于与服务器建立连接。
   - `addr`：这是一个指向服务器地址结构 (`sockaddr_in` 或 `sockaddr`，取决于实际情况) 的指针，包含服务器的 IP 地址和端口信息。
   - `addrlen`：这是 `addr` 结构的大小，通常使用 `sizeof(sockaddr_in)` 或 `sizeof(sockaddr)` 来指定。

    返回值
    - `connect` 函数返回一个整数，表示连接操作的结果。
    如果连接成功，它返回0；如果出现错误，返回-1，并设置 `errno` 以指示错误。

    `connect` 函数用于客户端向服务器发起连接请求，以建立网络通信。
    在你的代码中，`connect` 用于将客户端套接字 `client_fd` 连接到指定的服务器地址
    (`sockaddr_in`)，这样客户端就可以与服务器进行数据传输。
    如果连接成功，客户端可以开始通过 `send` 和 `recv` 等函数与服务器通信。
    */
    ret = connect(client_fd, (const struct sockaddr *)&sockaddr_in, sizeof(sockaddr_in));

    char buf[BUF_SIZE];
    pthread_create(&tid, NULL, recv_msg, &client_fd);

    // 获取用户名，并写入 socket
    char name[50] = {};
    printf("please input username:");
    gets(name);
    write(client_fd, name, strlen(name) + 1);

    for (;;)
    {
        // 从控制台获取消息
        printf(">>");
        gets(buf);
        char msg[BUF_SIZE];

        sprintf(msg, "%s:%s", name, buf);

        send_len = write(client_fd, msg, strlen(msg) + 1);
        if (0 >= send_len || 0 == strcmp(buf, "quit"))
        {
            printf("quit chat room !!!");
            return 0;
        }
    }
    return 0;
}

void *recv_msg(void *arg)
{

    int cli_fd = *(int *)arg;
    char buf[BUF_SIZE];
    int recv_size;

    for (;;)
    {
        recv_size = read(cli_fd, buf, BUF_SIZE);
        if (0 >= recv_size)
        {
            printf("disconnect !!!");
        }
        printf("%s\n", buf);
    }
}