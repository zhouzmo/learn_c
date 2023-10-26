/**********************************************************************
 * 功能描述： 1.client从标准输入获取到一个字符串，然后将这个字符串发送给server

 ***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main(int argc, char *argv[])
{
    int fd_server ,ret;
    struct sockaddr_un serv, client;
    socklen_t len = sizeof(client);
    char buf[1024] = {0};
    int recvlen;

    //创建socket
    fd_server = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd_server == -1) {
        perror("socket error");
        return -1;
    }

    //如果套接字文件存在，删除套接字文件
    unlink("client.sock");

    //给客户端绑定一个套接字文件
    client.sun_family = AF_LOCAL;
    strcpy(client.sun_path, "client.sock");
    ret = bind(fd_server, (struct sockaddr *)&client, sizeof(client));
    if (ret == -1) {
        perror("bind error");
        return -1;
    }

    //初始化server信息
    serv.sun_family = AF_LOCAL;
    strcpy(serv.sun_path, "server.sock");
    //连接
    connect(fd_server, (struct sockaddr *)&serv, sizeof(serv));

    while (1) {
        fgets(buf, sizeof(buf), stdin);
        send(fd_server, buf, strlen(buf)+1, 0);

        recv(fd_server, buf, sizeof(buf), 0);
        printf("client recv buf: %s\n", buf);
    }

    close(fd_server);
    return 0;
}

