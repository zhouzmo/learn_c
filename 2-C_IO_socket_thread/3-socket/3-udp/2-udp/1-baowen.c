#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/*
  数据报方式...不是字节流
   当接收数据时，指定的缓冲区的大小较小，剩余部分将会被截断扔掉

   具体现象，
   给自己发送 4 字节数据
   接收 1 字节，接收 4 次
   结果：只收到一字节数据，无法收到后续的字节数
   结论：udp 是数据报的方式，只能接收一次
        当容器size < 报文size ,会字节造成报文丢失，无法找回
  */
#define ERR_EXIT(msg)       \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

int main(void)
{
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8803);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind");

    // 发送
    sendto(sock, "ABCD", 2, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

    char recubuf[1];
    int n;
    int i;
    for (i = 0; i < 4; i++)
    { // 接受
        n = recvfrom(sock, recubuf, sizeof(recubuf), 0, NULL, NULL);
        if (n == -1)
        {
            if (errno == EINTR)
                continue;
            ERR_EXIT("recvfrom");
        }
        else if (n > 0)
        {
            printf("n=%d, recubuf[0]=%c\n", n, recubuf[0]); // n=1, recubuf[0]=A
        }
    }

    return 0;
}