
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define ERROR_Cache(ptr)     \
    do                       \
    {                        \
        if (ptr < 0)         \
        {                    \
            perror("error"); \
        }                    \
    } while (0)

/*

*/
int main(int argc, char const *argv[])
{
    int ret;
    int ci_sk_fd;
    struct sockaddr_in sv_addr; // tcp/ip 地址结构
    socklen_t addrlen = sizeof(struct sockaddr);
    char recv_buf[1024] = {0};
    char send_buf[1024] = {0};

    ci_sk_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_Cache(ci_sk_fd);

    memset(&sv_addr, 0, sizeof(struct sockaddr_in));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(8888);
    sv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = connect(ci_sk_fd, (struct sockaddr *)&sv_addr, addrlen);
    ERROR_Cache(ret);

    printf("connect success 服务端信息：\n");
    while (fgets(send_buf, sizeof(send_buf), stdin))
    {
        // 检查字符串是否为 "quit"，如果是则中断循环
        if (strncmp(send_buf, "quit", 4) == 0)
        {
            break;
        }

        // 写数据到服务器
        write(ci_sk_fd, send_buf, strlen(send_buf));
        // 读取服务器数据，打印屏幕
        ret = read(ci_sk_fd, recv_buf, sizeof(recv_buf));
        printf("server:");
        fputs(recv_buf, stdout);

        memset(recv_buf, 0, sizeof(recv_buf));
        memset(send_buf, 0, sizeof(send_buf));
    }
    close(ci_sk_fd);
    return 0;
}

/*



*/