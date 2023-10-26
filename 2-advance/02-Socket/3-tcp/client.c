#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_SIZE 512
#define PORT 3332
#define SERVEP_IP "192.168.40.40"

int main()
{
    int client_socket_fd;
    int addrlen = sizeof(struct sockaddr);
    char buf[MAX_SIZE];
    struct sockaddr_in server_socket;
    int ret;

    /*=== 1、创建 socket 网络文件描述符===*/
    client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    /*=== 2、填充 socket ===*/
    bzero(&server_socket, sizeof(struct sockaddr_in));
    server_socket.sin_family = AF_INET;
    server_socket.sin_port = ntohs(PORT);
    server_socket.sin_addr.s_addr = inet_addr(SERVEP_IP);

    /*=== 3、建立与服务器的连接。 ===*/
    ret = connect(client_socket_fd, (struct sockaddr *)&server_socket, addrlen);

    while (1)
    {
        memset(buf, 0, MAX_SIZE);
        printf("please input message:");
        scanf("%s", buf);
        printf("message lenght:%ld\n", strlen(buf));

        /*=== 4、发送数据到服务端 ===*/
        ret = send(client_socket_fd, (void *)buf, strlen(buf), 0);
    }

    // 关闭连接
    close(client_socket_fd);
    return 0;
}
