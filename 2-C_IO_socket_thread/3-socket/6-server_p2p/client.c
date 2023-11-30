
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define ERROR_Cache(ptr)           \
    do                             \
    {                              \
        if (ptr < 0)               \
        {                          \
            perror("#zmo# error"); \
            exit(0);               \
        }                          \
    } while (0)

#define SV_PORT 8888
#define SV_IPADDR "127.0.0.1"
/*

*/
void handler(int signo)
{
    if (signo == SIGRTMIN)
    {
        printf("pid:%d recv SIGRTMIN, do exit\n", getpid());
        exit(0);
    }
}
int main(int argc, char const *argv[])
{

    signal(SIGRTMIN, handler);

    pid_t pid;
    int i;
    int ret;
    int ci_sk_fd;
    struct sockaddr_in sv_addr; // tcp/ip 地址结构
    socklen_t addrlen = sizeof(struct sockaddr);
    char recv_buf[1024] = {0};
    char send_buf[1024] = {0};

    {
        ci_sk_fd = socket(AF_INET, SOCK_STREAM, 0);
        ERROR_Cache(ci_sk_fd);

        memset(&sv_addr, 0, sizeof(struct sockaddr_in));
        sv_addr.sin_family = AF_INET;
        sv_addr.sin_port = htons(SV_PORT);
        sv_addr.sin_addr.s_addr = inet_addr(SV_IPADDR);

        ret = connect(ci_sk_fd, (struct sockaddr *)&sv_addr, addrlen);
        ERROR_Cache(ret);

        printf("连接 server addr :%s:%u，成功\n", SV_IPADDR, SV_PORT);
    }

    // 父进程，接受数据，子进程发送数据
    pid = fork();
    if (pid < 0)
    {
        perror("");
    }
    else if (pid == 0)
    {
        // 子进程发送数据

        printf("pid:%d child process ....\n", getpid());
        char send_buf[1024] = {0};

        while (fgets(send_buf, sizeof(send_buf), stdin))
        {
            // 检查字符串是否为 "quit"，如果是则中断循环
            if (strncmp(send_buf, "quit", 4) == 0)
                break;

            // 写数据
            ret = write(ci_sk_fd, send_buf, strlen(send_buf));
            ERROR_Cache(ret);
            memset(send_buf, 0, sizeof(send_buf));
        }
        printf("pid:%d child exit ....\n", getpid());
        exit(0);
    }

    // 父进程，接受数据
    printf("pid:%d parent process ....\n", getpid());
    while (1)
    {
        memset(recv_buf, 0, sizeof(recv_buf));
        ret = read(ci_sk_fd, recv_buf, sizeof(recv_buf)); // 阻塞读，直到有数据
        if (ret == 0)
        {
            // 如果在读的过程中，对方已经关闭，tcp\ip 协议会返回要给 0 数据包
            exit(0);
        }
        ERROR_Cache(ret);
        printf("pid:%d recv data  消息：", getpid());

        fputs(recv_buf, stdout);
        if (0 == strncmp("quit", recv_buf,4))
        {
            printf("pid:%d exit", getpid());
            break;
        }
        printf("\n");
    }

    close(ci_sk_fd);

    return 0;
}

/*



*/