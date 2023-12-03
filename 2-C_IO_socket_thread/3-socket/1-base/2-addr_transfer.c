
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
地址转换
man  inet_addr
inet_aton
inet_ntoa
inet_addr
*/
int main(int argc, char const *argv[])
{
    int ret;

    // 将一个IPv4地址的字符串表示转换为`struct in_addr`结构体，该结构体用于存储IPv4地址。
    const char *cp = "192.168.111.11";
    in_addr_t myint = inet_addr(cp);
    printf("%u\n", myint); // 191867072

    struct in_addr myaddr;
    ret = inet_aton(cp, &myaddr);
    printf("%u\n", myaddr.s_addr); // 191867072

    // 将一个`struct in_addr`结构体中存储的IPv4地址转换为字符串表示，并返回该字符串。
    char *retStr;
    retStr = inet_ntoa(myaddr);
    printf("%s\n", retStr); // 192.168.111.11
    return 0;
}

/*
struct sockaddr_in {
    uint8_t sin_len;          // 结构体的长度
    sa_family_t sin_family;   // 地址族，一般为AF_INET
    in_port_t sin_port;       // 16位端口号
    struct in_addr sin_addr;  // IPv4地址
    char sin_zero[8];         // 填充字节，通常设置为0
};


struct sockaddr {
    uint8_t sa_len;             // 结构体的长度
    sa_family_t sa_family;      // 地址族
    char sa_data[14];           // 地址数据
};

struct in_addr {
    uint32_t s_addr;  // 存储 IPv4 地址的网络字节顺序表示
};

*/