#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

/**
实现聊天室功能
1、客户端发送消息
2、客户端接收其他客户的，和服务器消息
3、服务端接收客户端消息，并通知其它客户端
4、服务端发送消息
*/

#define MAX_SIZE 512
#define PORT 3332
// #define SERVEP_IP "192.168.40.40"
#define SERVEP_IP "192.168.111.11"

void *handle_client(void *arg)
{
    unsigned long tid = (unsigned long)pthread_self();
    printf("server create thread tid = %lu\n", tid);
    int client_socket_fd = (int)(intptr_t)arg;
    int recvnum;
    char recv_client_buf[MAX_SIZE];
    while (1)
    {
        /*=== 6、从已建立连接的 socket 中接收数据，非阻塞！ ===*/
        recvnum = recv(client_socket_fd, (void *)recv_client_buf, MAX_SIZE, 0);
        while (recvnum <= 0)
        {
            continue; // recvnum == 0 时
        }

        recv_client_buf[recvnum] = '\0';
        printf("\ntid【%lu】【%d】client: %s\n", tid, recvnum, recv_client_buf);
        memset(recv_client_buf, 0, MAX_SIZE);
    }
}

int main(void)
{
    int ret;
    int server_socket_fd;
    int client_socket_fd;
    int addrlen = sizeof(struct sockaddr);
    struct sockaddr_in server_socket;
    struct sockaddr client_socket;
    char server_send_buf[MAX_SIZE];

    int recvnum;
    pthread_t pt_handle_client;

    /*=== 1、创建 socket 网络文件描述符===*/
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    /*=== 2、填充 socket ===*/
    // 填充服务器端的数据，用于套接字绑定
    bzero(&server_socket, sizeof(struct sockaddr_in));
    server_socket.sin_family = AF_INET;                   // 设置为IPV4
    server_socket.sin_port = htons(PORT);                 // 将端口号主机序转换为网络序
    server_socket.sin_addr.s_addr = inet_addr(SERVEP_IP); // ip设置

    /*=== 3、将 socket 绑定到特定的 IP 地址和端口号。 ===*/
    ret = bind(server_socket_fd, (struct sockaddr *)&server_socket, addrlen);

    /*=== 4、将 socket 设置为监听状态， 以等待传入的连接请求。 ===*/
    ret = listen(server_socket_fd, 3);
    printf("服务器开启端口号: %d\n等待客户端连接...\n", PORT);

    /*=== 5、阻塞等待客户端的连接请求， ===*/
    client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_socket, (socklen_t *)&addrlen);
    // 创建线程，接受客户端消息
    printf("创建线程，接受客户端消息...\n");

    ret = pthread_create(&pt_handle_client, NULL, handle_client, (void *)(intptr_t)client_socket_fd);
    if (ret != 0)
    {
        perror("pthread_create");
        return -1;
    }

    while (1)
    {
        // printf("wait for a message from the client !\n");

        // 主线程可以发送消息
        memset(server_send_buf, 0, MAX_SIZE);
        printf("server please input message:");
        scanf("%s", server_send_buf);

        if (0 == strcmp(server_send_buf, "quit"))
        {
            break;
        }

        send(client_socket_fd, server_send_buf, strlen(server_send_buf), 0);
    }

    printf("结束通信\n");

    // 关闭连接
    close(client_socket_fd);
    close(server_socket_fd);

    return 0;
}

/*
$ gcc -o client.out client.c -pthread
$ gcc -o server.out server.c -pthread

$ ./server.out
服务器开启端口号: 3332
等待客户端连接...
创建线程，接受客户端消息...
server please input message:server create thread tid = 140511389431552
hello
server please input message:
tid【140511389431552】【6】client: hello!
quit
结束通信


*/


/*
$ ./client.out
client please input message:client create thread tid = 140428397987584

tid【140428397987584】【5】server: hello
hello!
message send ret :6
client please input message:1
message send ret :1
client please input message:1



*/