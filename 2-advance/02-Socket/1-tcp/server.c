#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


/*

实现 tcp 通信，客户端发送一条消息到服务端
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
    printf("start ...!\n");
    /*=== 1、创建 socket 网络文件描述符===*/
    /*
     int socket(int domain, int type, int protocol);
    用于创建套接字，即一个用于网络通信的文件描述符。
    它允许您指定套接字的地址家族（通常是 IPv4 地址家族）、类型（面向连接或无连接
    套接字）以及协议（通常为 0，表示自动选择合适的协议）。
    参数
    domain
        参数指定了套接字的地址家族或协议族，通常是 AF_INET
        表示 IPv4 地址家族。
    type
        参数指定了套接字的类型，常用的是 SOCK_STREAM 表示面
        向连接的套接字（如 TCP 套接字）或 SOCK_DGRAM 表示无
        连接的套接字（如 UDP 套接字）。
    protocol
        参数通常设置为 0，表示根据 domain 和 type 的
        值自动选择合适的协议。
    返回值：
        如果函数调用成功，它将返回一个新创建的套接字的文件描述符，
        如果失败，则返回 -1，并设置 errno 来指示错误的原因
    */
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    /*=== 2、填充 socket ===*/
    // 填充服务器端的数据，用于套接字绑定
    bzero(&server_socket, sizeof(struct sockaddr_in));
    server_socket.sin_family = AF_INET;                   // 设置为IPV4
    server_socket.sin_port = htons(PORT);                 // 将端口号主机序转换为网络序
    server_socket.sin_addr.s_addr = inet_addr(SERVEP_IP); // ip设置为192.168.192.128
                                                          // 创建套接字

    /*=== 3、将 socket 绑定到特定的 IP 地址和端口号。 ===*/
    /*
    这行代码用于在服务器套接字 (`server_fd`) 上绑定地址信息 (`address`)。

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

        - `sockfd`：这是服务器套接字的描述符，通常是一个先前通过 `socket` 函数创建的套接字。
        - `addr`：这是一个指向 `struct sockaddr` 结构的指针，包含了要绑定的地址信息。在你的代码中，这是 `&address`，其中 `address` 包含了服务器要使用的 IP 地址和端口号。
        - `addrlen`：这是 `addr` 结构的大小，通常使用 `sizeof(address)` 来指定。

    返回值
        - `bind` 函数返回一个整数，表示绑定操作是否成功。
        如果绑定成功，它会返回0；否则，返回-1，并设置 `errno` 以指示发生的错误。

    在服务器编程中，`bind` 用于将服务器套接字绑定到特定的 IP 地址和端口号。
    这将告诉操作系统服务器要监听哪个地址和端口以接受连接。
    如果 `bind` 失败，通常是因为所选择的地址和端口已经被占用或其他错误。
    */
    ret = bind(server_socket_fd, (struct sockaddr *)&server_socket, addrlen);

    /*=== 4、将 socket 设置为监听状态， 以等待传入的连接请求。 ===*/
    /*
    将服务器套接字 (`server_fd`) 设置为监听状态，
    以等待传入的连接请求。

   int listen(int sockfd, int backlog);

    - `sockfd`：这是服务器套接字的描述符，通常是一个先前通过 `socket` 和 `bind`
    函数创建并绑定了地址信息的套接字。
    - `backlog`：这是等待连接队列的最大长度。
    它表示可以同时等待处理的未连接请求的最大数量。
    在你的代码中，它被设置为3，表示最多可以等待3个未连接的客户端连接请求。

    返回值
    - `listen` 函数返回一个整数，表示操作是否成功。
    如果成功，它返回0；否则，返回-1，并设置 `errno` 以指示错误。

    一旦调用 `listen` 函数，服务器套接字就会进入监听状态，
    开始等待客户端连接请求。未连接的客户端请求将排队，
    最多可以排队到指定的 `backlog` 长度。当客户端尝试连接到服务器时，
    服务器将从队列中接受请求并建立连接。
    */
    ret = listen(server_socket_fd, 3);

    /*=== 5、阻塞等待客户端的连接请求， ===*/
    /*
    这行代码用于接受客户端连接请求，创建一个新的套接字 (`client_fd`) 来处理与客户端的通信。
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

    - `sockfd`：这是服务器套接字的描述符，通常是通过 `socket`、`bind` 和 `listen` 函数创建的套接字。它用于监听客户端连接请求。
    - `addr`：这是一个指向 `struct sockaddr` 结构的指针，用于存储连接到服务器的客户端的地址信息。
    - `addrlen`：这是一个指向整数的指针，指示 `addr` 结构的大小。在 `accept` 调用之前，你需要设置 `addrlen` 以告诉函数 `addr` 结构的大小。在成功的 `accept` 调用后，`addrlen` 将包含接受的地址结构的实际大小。

    返回值
    - `accept` 函数返回一个新的套接字描述符 (`client_fd`)，用于与新连接的客户端通信。如果出现错误，它返回-1，并设置 `errno` 以指示错误。

    在服务器编程中，`accept` 用于等待客户端的连接请求，
    并在连接请求到达时创建一个新的套接字来处理与客户端的通信。
    通常，在这之后，你会在 `client_fd` 上执行读取和写入操作，
    以与客户端进行通信。
    */
    printf("wait for a message from the client !\n");
    client_socket_fd = accept(server_socket_fd, &client_socket, &addrlen);

    /*=== 6、从已建立连接的 socket 中接收数据， ===*/
    /*
    **函数作用：**
    这行代码的作用是从已建立连接的套接字 (`client_socket_fd`) 中接收数据，
    将接收到的数据存储在 `buf` 缓冲区中，并返回接收的字节数。
    通常，它用于接收来自客户端的数据，以便服务器可以处理或回复这些数据。

    ssize_t recv(int sockfd, void *buf, size_t len, int flags);

    - `sockfd`：这是已建立连接的套接字描述符，用于接收数据。它指定了数据应该从哪个套接字接收。
    - `buf`：这是一个指向用于存储接收数据的缓冲区的指针。通常是一个字符数组，但在这里用 `void *` 表示通用性，因为 `recv` 可以用于接收任何类型的数据。
    - `len`：这是要接收的数据的最大字节数，它表示缓冲区 `buf` 的大小。 `recv` 将尝试接收不多于 `len` 字节的数据。
    - `flags`：这是一个整数标志，用于指定接收数据的行为。在你的代码中，使用了 `0`，表示没有特殊的标志。

    **返回值作用：**
    - `recv` 函数返回一个 `ssize_t` 类型的值，表示接收的字节数。具体作用如下：
    - 如果 `recv` 成功接收数据，它将返回接收到的字节数，这个值可能小于或等于 `len`。
    - 如果 `recv` 在尝试接收数据时出现错误，它将返回一个负数，表示接收出错。
    你可以使用 `errno` 变量来获取更多关于错误的信息。
    - 如果连接已经关闭，`recv` 将返回 `0`，表示没有更多数据可接收。

    在你的代码中，`recv` 函数用于接收来自客户端的数据，并将其存储在 `buf` 缓冲区中。
    接收的字节数被赋值给 `recvnum` 变量。你可以使用 `recvnum` 来确定实际接收了多少数据，
    然后根据需要对接收到的数据进行处理。如果 `recvnum` 是负数，
    表示发生了错误，你可以根据错误代码来进行错误处理。如果 `recvnum` 是0，
    表示连接已经关闭，没有更多数据可接收。
    */
    recvnum = recv(client_socket_fd, (void *)buf, MAX_SIZE, 0);

    buf[recvnum] = '\0';
    printf("recv from client: %s\n", buf);
    memset(buf, 0, MAX_SIZE);
    // 关闭连接
    printf("close server connect!!! \n");
    close(server_socket_fd);
    close(client_socket_fd);
    return 0;
}
