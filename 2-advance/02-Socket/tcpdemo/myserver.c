#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

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

#define MAX_CLIENTS 100
#define MAX_MSG_LEN 1024
#define PORT 8888
#define IP_ADDR "127.0.0.1"



struct message
{
    char username[50];
    char msg_content[MAX_MSG_LEN];
}




void *handle_client(void *arg);




int clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;



int main(int argc, char *argv[])
{
    int server_fd, client_fd, valread, ret;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // 清空 client
    memset(clients, 0, MAX_CLIENTS * sizeof(int));

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
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_RET(server_fd, "socket failed");

    /*=== 2、设置 socket ===*/
    /*
    设置套接字选项的代码，
    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

    - `sockfd`：这是套接字描述符，通常是一个已创建的套接字，用于设置选项。
    - `level`：这是选项的级别，通常使用`SOL_SOCKET`来表示套接字级别的选项。它指示要设置的选项的类型。
    - `optname`：这是要设置的选项的名称，它指示要执行的特定操作。在这里，`SO_REUSEADDR`和`SO_REUSEPORT`是两个标志位，通过按位或操作(`|`)合并在一起，表示要启用地址重用和端口重用选项。
    - `optval`：这是一个指向包含选项值的内存的指针。在你的代码中，它指向`opt`，可能是一个整数，包含选项的值。
    - `optlen`：这是`optval`指向的内存块的大小，通常用`sizeof(opt)`来指定。

    返回值含义和作用：
    - `setsockopt`函数的返回值是一个整数，通常表示操作是否成功。如果设置选项成功，它会返回0；否则，返回-1，并设置`errno`以指示发生的错误。
    */
    ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    CHECK_RET(ret, "setsockopt failed");

    /*
    address.sin_family：这是地址家族，通常设置为 AF_INET，表示使用 IPv4 地址。
    address.sin_addr.s_addr：这是 IP 地址的字段。在你的代码中，
        它被设置为 inet_addr(IP_ADDR)，其中 IP_ADDR 是一个宏或变量，
        包含一个 IPv4 地址的字符串形式，inet_addr 函数将其转换为网络字节序的 32 位二进制表示。
    address.sin_port：这是端口号的字段。在你的代码中，它被设置为 htons(PORT)，
        其中 PORT 是一个宏或变量，表示端口号。
        htons 函数用于将端口号从主机字节序转换为网络字节序

    网络字节序，也称为大端字节序，是一种在网络通信中广泛使用的字节序，其中最高有效位（Most Significant Byte，MSB）在前面。
    这是为了确保在不同计算机架构上传输数据时具有一致的字节顺序。
    */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(IP_ADDR);
    address.sin_port = htons(PORT);

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
    ret = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    CHECK_RET(ret, "bind failed");

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
    ret = listen(server_fd, 3);
    CHECK_RET(ret, "listen failed");

    printf("服务器开启端口号: %d\n", PORT);
    printf("等待客户端连接...\n");

    while (1)
    {
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
        client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        CHECK_RET(ret, "accept failed");

        // 将 client_fd 存入 clients
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] == 0)
            {
                clients[i] = client_fd;
                break;
            }
        }

        if (i == MAX_CLIENTS)
        {
            printf("客户端连接失败\n");
            close(client_fd);
            continue;
        }

        // 创建一个线程用于处理与客户端的通信
        pthread_t new_thread;
        if (pthread_create(&new_thread, NULL, handle_client, (void *)(intptr_t)client_fd) != 0)
        {
            printf("线程创建失败\n");
            continue;
        }
    }
}

void *handle_client(void *arg)
{
    int ret;
    int client_fd = (int)(intptr)arg;
    struct message msg;
    /*
    这行代码用于从客户端套接字 (`client_fd`) 接收数据

   ssize_t recv(int sockfd, void *buf, size_t len, int flags);

   - `sockfd`：这是套接字描述符，通常是用于接收数据的客户端套接字 (`client_fd`)。
   - `buf`：这是一个指向接收数据的缓冲区的指针，通常是一个变量或数组。
   - `len`：这是要接收的数据的最大字节数。
   - `flags`：这是一个标志位，用于控制接收操作的行为，通常可以设置为0。

    返回值
   - `recv` 函数返回一个 `ssize_t` 类型的值，表示实际接收的字节数。
   如果返回值是正数，表示成功接收了数据；如果返回值是0，表示对方已经关闭了连接；
   如果返回值是-1，表示发生了错误，可以通过检查 `errno` 来了解错误的具体原因。

    在你的代码中，`recv` 用于从客户端套接字 `client_fd` 接收数据，
    并将其存储在 `msg` 变量中。你可以根据实际应用的需要处理接收到的数据，
    通常是解析它们并采取适当的操作。
    */
    ret = recv(client_fd, &msg, sizeof(msg), 0);
    CHECK_RET(ret, "recv failed");
    printf("%s 加入聊天室\n", msg.username);

    while (1)
    {
        ret = recv(client_fd, &msg, sizeof(msg), 0);

        if (ret <= 0)
        {
            pthread_mutex_lock(&clients_mutex);
            // 连接中断,清除该连接的 fd
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i] == client_fd)
                {
                    printf("【%s】,离开聊天室\n", msg.username);
                    clients[i] = 0;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        }
        else
        {
            // 成功接收信息，服务端打印信息详情，广播到其它用户
            printf("%s:%s\n", msg.username, msg.msg_content);
            for (int 1 = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i] && client_fd != clients[i])
                {
                    /*
                    向服务器上的其他客户端发送消息 `msg`
                    ssize_t send(int sockfd, const void *buf, size_t len, int flags);

                    - `sockfd`：这是套接字描述符，通常是服务器用于与客户端通信的套接字。
                    - `buf`：这是一个指向要发送的数据的缓冲区的指针，通常是消息内容。
                    - `len`：这是要发送的数据的字节数。
                    - `flags`：这是一个标志位，用于控制发送操作的行为，通常可以设置为0。
                    - 返回值
                    - `send` 函数返回一个 `ssize_t` 类型的值，表示实际发送的字节数。
                    如果返回值是正数，表示成功发送了数据；如果返回值是-1，表示发生了错误，
                    可以通过检查 `errno` 来了解错误的具体原因。

                    在你的代码中，`send` 函数被用于遍历服务器上的所有客户端
                    （`clients` 数组）并向它们发送消息 `msg`。
                    这通常用于多客户端聊天服务器或广播消息给所有连接的客户端。
                    循环检查每个客户端套接字，
                    如果不是当前客户端的套接字（`client_fd != clients[i]`），
                    则将消息发送给其他客户端。这确保了消息不会发给发送它的客户端。
                    */
                    send(clients[i], &msg, sizeof(msg), 0);
                }
            }
        }
    }
}

