
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

/*
网络字节序

网络字节序是大端模式
字节序转换函数 man htonl
地址转换函数man inet_addr

htonl ==  host to net long
*/
int main(int argc, char const *argv[])
{
    int ret;
    unsigned int data = 0x12345678;
    char *p = &data;

    printf("值：0x%x,存储方式：%x,%x,%x,%x\n", data, p[0], p[1], p[2], p[3]);

    if (0x12 == *p)
    {
        printf("da大端\n");
    }
    else if (0x78 == *p)
    {
        printf("xiao小端\n");
    }

    printf("本地字节序转换为网络字节序\n");

    /*==字节序转换函数 man htonl*/
    ret = htonl(data); // host to net long
    p = &ret;
    printf("值：0x%x,存储方式：%x,%x,%x,%x\n", ret, p[0], p[1], p[2], p[3]);
    return 0;
}

/*

值：0x12345678,存储方式：78,56,34,12
xiao小端
本地字节序转换为网络字节序
值：0x78563412,存储方式：12,34,56,78

*/

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