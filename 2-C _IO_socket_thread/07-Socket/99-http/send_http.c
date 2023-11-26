#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

// Linux下的头文件
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096
#define HTTP_VERSION "HTTP/1.1"
#define CONNETION_TYPE "Connection: close\r\n"

// /*该函数用于将主机名（hostname）转换为 IP 地址。它首先调用 gethostbyname 函数
// 获取主机名对应的地址信息结构体，然后从中提取出 IP 地址信息，并返回一个指向字符串类型的 IP 地址。*/
char *host_to_ip(const char *hostname)
{
    /*gethostbyname用于通过主机名获取主机的IP地址信息，
    并返回一个hostent结构体类型的指针，该结构体包含了主机名、别名和IP地址等信息。*/
    struct hostent *host_entry = gethostbyname(hostname);

    /*inet_ntoa()函数是一个在网络编程中常用的函数，它可以将一个32位整数IP地址转换成标准的点分十进制形式
    struct in_addr是一个C语言中的数据结构，用于表示32位IPv4地址。*/
    if (host_entry)
    {
        // h_addr_list 是指向主机IP地址列表的指针，其类型为 char **
        return inet_ntoa(*(struct in_addr *)*host_entry->h_addr_list);
    }
    return NULL;
}

/*该函数创建一个 TCP 套接字，并连接到指定的 IP 地址和端口号上。它首先调用 socket 函数创建套接字，
然后调用 connect 函数连接到目标地址。如果成功则返回套接字文件描述符，否则返回 -1。*/
int http_create_socket(char *ip)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // sockaddr_in 用于表示 IPv4 地址和端口号
    struct sockaddr_in sin = {0};        //
    sin.sin_family = AF_INET;            // 指定 IP 地址的协议族
    sin.sin_port = htons(80);            // 表示端口号
    sin.sin_addr.s_addr = inet_addr(ip); // 表示 IP 地址，inet_addr作用与inte_ntoa相反

    /*在网络编程中，由于不同协议族（如IPv4、IPv6等）使用不同的地址结构体，为了保证代码的通用性和可移植性，
    在函数调用时需要将特定协议族所对应的地址结构体指针转换为通用的 sockaddr 结构体指针。
    因此，当我们想要将一个特定协议族（如IPv4）的地址结构体传递给一个接受 sockaddr 结构体参数的函数时，
    就需要使用 (struct sockaddr*)&sin 进行类型强制转换，以避免编译器报错。*/
    if (0 != connect(sockfd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)))
    {
        return -1;
    }

    // 将 sockfd 文件描述符设置为非阻塞模式
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    return sockfd;
}

/*该函数发送 HTTP 请求并获取响应结果。它首先通过 host_to_ip 函数将主机名转换为 IP 地址，
然后使用 http_create_socket 函数创建套接字并连接到目标服务器。接着构建 HTTP 请求报文，
并使用 send 函数将其发送给服务器。最后等待服务器响应并读取响应结果。*/
char *http_send_request(const char *hostname, const char *resource)
{
    char *ip = host_to_ip(hostname);
    printf(" %s() line %d, 获取本机IP: %s\n", __FUNCTION__, __LINE__, ip);
    int sockfd = http_create_socket(ip);
    printf(" %s() line %d, socket connect noblock...\n", __FUNCTION__, __LINE__);

    char buffer[BUFFER_SIZE] = {0};

    // HTTP协议中使用的行结束符是 "\r\n" ，每个字段之间需要用两个连续的回车符和换行符 "\r\n\r\n" 分隔开来
    sprintf(buffer, "GET %s %s\r\nHost: %s\r\n%s\r\n\r\n",
            resource, HTTP_VERSION, hostname, CONNETION_TYPE);

    /*如果在使用 UDP 协议进行通信或者需要对多个目标地址进行发送数据操作，则应该选择使用 sendto 函数；
    如果在使用 TCP 协议进行通信且只涉及一个目标地址，则可以选择使用 send 函数。*/
    send(sockfd, buffer, strlen(buffer), 0);

    /*fd_set 是一个数据类型，可以同时监视多个文件描述符的状态，比如可读、可写或者异常等。
    在使用前必须先通过FD_ZERO宏清空，并使用FD_SET来添加或删除其中的文件描述符*/
    fd_set fdread;
    FD_ZERO(&fdread);
    FD_SET(sockfd, &fdread);

    /*timeval 结构体是一个用于表示时间的结构体，tv_sec 表示秒数，tv_usec 表示微秒数*/
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    char *result = malloc(sizeof(int));
    memset(result, 0, sizeof(int));
    while (1)
    {
        /*select函数可以监听多个文件描述符（包括Socket），并等待其中任意一个或多个文件描述符上有I/O事件发生时通知调用者
        通过使用select函数，程序可以同时监听多个I/O事件，并且只要有任何一个事件发生了变化就会被立即通知。
        这样可以大大提高程序的性能和响应速度，避免阻塞或死循环等问题*，具体参数分别如下：
        nfds：需要监听的文件描述符最大值+1。即所有待监听文件描述符中最大的那个数加一。
        readfds：指向fd_set类型的指针，用于设置需要检查可读性的文件描述符集合。
        writefds：指向fd_set类型的指针，用于设置需要检查可写性的文件描述符集合。
        exceptfds：指向fd_set类型的指针，用于设置需要检查异常条件（错误）的文件描述符集合。
        timeout：等待超时时间。如果为NULL，则表示无限等待；如果为0，则表示立即返回。*/
        int selection = select(sockfd + 1, &fdread, NULL, NULL, &tv);
        if (!selection || !FD_ISSET(sockfd, &fdread))
        {
            break;
        }
        else
        {
            // buffer之前已经被send之后，需要清空
            memset(buffer, 0, BUFFER_SIZE);
            int len = recv(sockfd, buffer, BUFFER_SIZE, 0);
            if (len == 0)
            {
                break;
            }

            result = realloc(result, (strlen(result) + len + 1) * sizeof(char));
            strncat(result, buffer, len); // strncat()将指定长度的字符串追加到目标字符串末尾
        }
    }
    return result;
}

int main(int argc, char *argv[])
{

    int fd, err;
    if (argc < 3)
        return -1;

    char *response = http_send_request(argv[1], argv[2]);
    fd = open("response.html", O_RDWR | O_CREAT | O_TRUNC, 0777);
    err = write(fd, response, strlen(response));
    printf("write err : %d\n", err);
    // printf("response : %s\n", response);

    free(response);
}

/*
gcc zmohttp.c -o httptest.out
./httptest.out www.baidu.com /


*/