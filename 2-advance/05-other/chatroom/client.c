#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ERRLOG(msg)                                         \
    do {                                                    \
        printf("%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        perror(msg);                                        \
        exit(-1);                                           \
    } while (0)

void recycle()
{
    wait(NULL);
}

void tuichu()
{
    exit(0);
}

int main(int argc, const char* argv[])
{
    //参数合理性检查
    if (3 != argc) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(-1);
    }

    //创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        ERRLOG("socket error");
    }

    //填充服务器网络信息结构体
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[2]));
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);

    socklen_t serveraddr_len = sizeof(serveraddr);

    //与服务器建立连接
    if (-1 == connect(sockfd, (struct sockaddr*)&serveraddr, serveraddr_len)) {
        ERRLOG("connect error");
    }

    printf("与服务器连接成功..\n");

    char buff[128] = { 0 };
    int num = 0;

    //等待子进程退出信号SIGCHLD,回收子进程资源
    signal(SIGCHLD, recycle);

    //需要分出来一个子进程，专门用于接收消息，父进程用于发送消息
    pid_t pid = fork();
    int named = 0;
    while (1) {
        if (pid == 0) { //子进程,接收消息
            signal(SIGINT, tuichu); //子进程捕获到SIGINT信号就退出
            //接收应答消息
            memset(buff, 0, sizeof(buff));
            if (-1 == (num = recv(sockfd, buff, 128, 0))) {
                ERRLOG("recv ersockfdror");
            }
            printf("%s\n", buff);

            //如果收到自己的退出消息，就结束子进程

        } else { //父进程，发送消息
            if (named == 0) {
                printf("请输入群聊名称：\n");
                named = 1;
            } else if (named == 1) {
                printf("请再次输入群聊名称：\n");
                named = 2;
            }

            fgets(buff, 128, stdin);
            if (strlen(buff) != 0) {
                buff[strlen(buff) - 1] = '\0';
            }

            //如果用户输入quit就退出
            if (!strcmp(buff, "quit")) {
                //如果退出了，最后也要发送数据"quit"
                if (-1 == send(sockfd, buff, 128, 0)) {
                    ERRLOG("send error");
                }
                char buf[22] = { 0 };
                sprintf(buf, "kill %d", pid);
                system(buf);
                wait(NULL);
                break;
            }

            //发送数据
            if (-1 == send(sockfd, buff, 128, 0)) {
                ERRLOG("send error");
            }
        }
    }
    //关闭套接字
    close(sockfd);

    return 0;
}
