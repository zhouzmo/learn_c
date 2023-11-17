#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_SIZE 512
#define PORT 3332
// #define SERVEP_IP "192.168.40.40"
#define SERVEP_IP "192.168.111.11"

void *handle_server_msg(void *arg)
{
    unsigned long tid = (unsigned long)pthread_self();
    printf("client create thread tid = %lu\n", tid);
    int client_socket_fd = *(int *)arg;
    int recvnum;
    char recv_server_buf[MAX_SIZE];
    while (1)
    {
        /*=== 6、从已建立连接的 socket 中接收数据，非阻塞！ ===*/
        recvnum = recv(client_socket_fd, (void *)recv_server_buf, MAX_SIZE, 0);
        while (recvnum <= 0)
        {
            continue; // recvnum == 0 时
        }
        recv_server_buf[recvnum] = '\0';
        printf("\ntid【%lu】【%d】server: %s\n", tid, recvnum, recv_server_buf);
        memset(recv_server_buf, 0, MAX_SIZE);
    }
}

int main()
{
    int client_socket_fd;
    int addrlen = sizeof(struct sockaddr);
    char buf[MAX_SIZE];
    struct sockaddr_in server_socket;
    int ret;
    pthread_t pt_recv_server;

    /*=== 1、创建 socket 网络文件描述符===*/
    client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    /*=== 2、填充 socket ===*/
    bzero(&server_socket, sizeof(struct sockaddr_in));
    server_socket.sin_family = AF_INET;
    server_socket.sin_port = ntohs(PORT);
    server_socket.sin_addr.s_addr = inet_addr(SERVEP_IP);

    /*=== 3、建立与服务器的连接。 ===*/
    ret = connect(client_socket_fd, (struct sockaddr *)&server_socket, addrlen);

    // 创建线程，接受数据
    // 创建线程，接受客户端消息
    ret = pthread_create(&pt_recv_server, NULL, handle_server_msg, &client_socket_fd);
    if (ret != 0)
    {
        perror("pthread_create");
        return -1;
    }

    while (1)
    {
        memset(buf, 0, MAX_SIZE);
        printf("client please input message:");
        scanf("%s", buf);
        // printf("message lenght:%ld\n\n", strlen(buf));

        /*=== 4、发送数据到服务端 ===*/
        ret = send(client_socket_fd, (void *)buf, strlen(buf), 0);
        printf("message send ret :%d\n", ret);
        if (ret < 0)
        {
            break;
        }
    }
    printf("发送失败，关闭连接\n");
    // 关闭连接
    close(client_socket_fd);
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