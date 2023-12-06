#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

void getifaddrsFunc()
{
    struct ifaddrs *ifaddr, *ifa;
    char ip[INET_ADDRSTRLEN];

    // 获取网络接口地址列表
    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        }

    // 遍历地址列表
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET)
        {
            // 获取IPv4地址
            struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);
            printf("Interface: %s\t IP address: %s\n", ifa->ifa_name, ip);
        }
    }

    // 释放资源
    freeifaddrs(ifaddr);
}

void gethostnameFunc()
{
    char hostname[256];
    char ip[INET_ADDRSTRLEN];
    struct hostent *he;
    struct in_addr **addr_list;

    // 获取主机名
    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        perror("gethostname");
    }

    // 通过主机名获取主机信息
    if ((he = gethostbyname(hostname)) == NULL)
    {
        herror("gethostbyname");
    }

    // 获取IP地址列表
    addr_list = (struct in_addr **)he->h_addr_list;

    // 将第一个IP地址转换为字符串形式
    if (addr_list[0] != NULL)
    {
        inet_ntop(AF_INET, addr_list[0], ip, sizeof(ip));
        printf("IP address: %s\n", ip);
    }
}

int main()
{

    // getifaddrsFunc();
    gethostnameFunc();

    return 0;
}
