#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

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
    /*
    函数作用：`connect` 函数用于建立与远程主机的连接。

    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`

    - `sockfd`：要连接的套接字文件描述符。
    - `addr`：指向目标服务器的 `struct sockaddr` 结构体的指针。通常需要将目标服务器的地址信息转换为 `struct sockaddr` 类型，并将其作为参数传递给 `connect` 函数。在这种情况下，使用了类型转换 `(struct sockaddr *)&server_socket`。
    - `addrlen`：`addr` 结构体的大小，以字节为单位。

    - 如果连接成功建立，返回值为 0。
    - 如果连接失败，返回值为 -1，并通过设置全局变量 `errno` 来指示具体的错误原因。

    */
    ret = connect(client_socket_fd, (struct sockaddr *)&server_socket, addrlen);

 
    memset(buf, 0, MAX_SIZE);
    printf("please input message:");
    scanf("%s", buf);

    /*=== 4、发送数据到服务端 ===*/
    /*
    这行代码使用 `send` 函数将数据从用户空间发送到已建立连接的套接字 (`client_socket_fd`)，
    然后将数据传递到远程连接的另一端。这是在网络编程中发送数据的常见操作。

    ssize_t send(int sockfd, const void *buf, size_t len, int flags);

    - `sockfd`：这是已建立连接的套接字描述符，用于发送数据。它指定了数据应该发送到哪个套接字。
    - `buf`：这是一个指向包含要发送数据的缓冲区的指针。通常是用户空间中的一个缓冲区，但在这里使用了 `const void *` 表示通用性，因为 `send` 可以用于发送任何类型的数据。
    - `len`：这是要发送的数据的字节数，表示要从缓冲区 `buf` 发送多少字节的数据。
    - `flags`：这是一个整数标志，用于指定发送数据的行为。在你的代码中，使用了 `0`，表示没有特殊的标志。

    - `send` 函数返回一个 `ssize_t` 类型的值，表示已发送的字节数。
    - 如果成功发送数据，它将返回已发送的字节数，通常等于 `len`。
    - 如果发送数据过程中发生错误，它将返回一个负数，表示发送出错。

    `send` 函数用于将数据从用户空间缓冲区 (`buf`) 发送到已建立的连接，
    通常是发送给远程客户端。在你的代码中，`ret` 变量将存储已发送的字节数。
    你可以使用 `ret` 来确定实际发送了多少数据，以及进行适当的错误处理，
    如果 `ret` 是负数的话。
    */
    ret = send(client_socket_fd, (void *)buf, MAX_SIZE, 0);
   

    // 关闭连接
    close(client_socket_fd);
    return 0;
}
