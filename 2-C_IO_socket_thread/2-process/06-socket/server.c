/**********************************************************************
 * 功能描述： 1.server打印client发送过来的字符串，并将该字符串回发给client

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
    int fd_server ,ret, fd_client;
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
    unlink("server.sock");

    //初始化server信息
    serv.sun_family = AF_LOCAL;
    strcpy(serv.sun_path, "server.sock");

    //绑定
    ret = bind(fd_server, (struct sockaddr *)&serv, sizeof(serv));
    if (ret == -1) {
        perror("bind error");
        return -1;
    }

    //设置监听，设置能够同时和服务端连接的客户端数量
    ret = listen(fd_server, 36);
    if (ret == -1) {
        perror("listen error");
        return -1;
    }

    //等待客户端连接
    fd_client = accept(fd_server, (struct sockaddr *)&client, &len);
    if (fd_client == -1) {
        perror("accept error");
        return -1;
    }
    printf("=====client bind file:%s\n", client.sun_path);
    
    while (1) {
        recvlen = recv(fd_client, buf, sizeof(buf), 0);
        if (recvlen == -1) {
            perror("recv error");
            return -1;
        } else if (recvlen == 0) {
            printf("client disconnet...\n");
            close(fd_client);
            break;
        } else {
            printf("server recv buf: %s\n", buf);
            send(fd_client, buf, recvlen, 0);
        }
    }
    
    close(fd_client);
    close(fd_server);
    return 0;
}


/*
$ gcc -o client.out client.c
$ gcc -o server.out server.c
$ ls
client.c  client.out  server.c  server.out
$ ./server.out &
[1] 156886
$ ./client.out
=====client bind file:client.sock
hello
server recv buf: hello

client recv buf: hello






*/