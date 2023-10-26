#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


/**
 实现 tcp 通信，客户端持续发送   服务端可持续接收,可持续接受
*/


#define MAX_SIZE 512
#define PORT 3332
#define SERVEP_IP "192.168.40.40"

int main(void)
{
    int ret;
    int server_socket_fd;
    int client_socket_fd;
    int addrlen = sizeof(struct sockaddr);
    struct sockaddr_in server_socket;
    struct sockaddr client_socket;
    char buf[MAX_SIZE];
    int recvnum;

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
    printf("wait for a message from the client !\n");
    client_socket_fd = accept(server_socket_fd, (struct sockeaddr *)&client_socket, &addrlen);
   
    while (1)
    {
        /*=== 6、从已建立连接的 socket 中接收数据，非阻塞！ ===*/
        recvnum = recv(client_socket_fd, (void *)buf, MAX_SIZE, 0);

        buf[recvnum] = '\0';
        printf("recv %d byte data from client: %s\n", recvnum, buf);
        memset(buf, 0, MAX_SIZE);
    }

    // 关闭连接
    close(server_socket_fd);
    close(client_socket_fd);
    return 0;
}
