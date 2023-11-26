#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT 8888
#define MAX_CLIENTS 100

// 存储待监听的文件描述符
struct pollfd fds[MAX_CLIENTS];
int nfds = 1; // 当前待监听的文件描述符数量，初始值为 1（服务器 socket）

// 将文件描述符添加到待监听列表中
void addClient(int client_sock_fd)
{
    // 检查文件描述符数量是否超过最大值
    if (nfds >= MAX_CLIENTS)
    {
        fprintf(stderr, "Too many clients\n");
        return;
    }

    // 将文件描述符添加到待监听列表中
    fds[nfds].fd = client_sock_fd;
    fds[nfds].events = POLLIN;
    nfds++;
}

int main()
{
    int server_sock_fd, client_sock_fd;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen;

    /*==1、socket bind listen */
    {
        server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_sock_fd == -1)
        {
            perror("Failed to create socket");
            exit(EXIT_FAILURE);
        }
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);
        if (bind(server_sock_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        {
            perror("Failed to bind socket");
            exit(EXIT_FAILURE);
        }
        if (listen(server_sock_fd, SOMAXCONN) == -1)
        {
            perror("Failed to listen on socket");
            exit(EXIT_FAILURE);
        }
    }

    // 监听 server_sock_fd 的 POLLIN 事件
    fds[0].fd = server_sock_fd;
    fds[0].events = POLLIN;

    printf("Server started. Waiting for connections...\n");

   
    while (1)
    {
         /*==2、阻塞等待 server_sock_fd 的事件发生，
         1、当 server 被监听到有事件，说明有客户端请求连接
         2、接受客户端连接，并将 client_sock_fd 加入监听队列
            并接受该 client_sock_fd
          */
        if (poll(fds, nfds, -1) == -1)
        { //
            perror("Failed to poll");
            exit(EXIT_FAILURE);
        }

        // 遍历待监听的文件描述符列表，处理就绪的事件
        for (int i = 0; i < nfds; i++)
        {
            // 检查是否有新的连接请求
            if (fds[i].fd == server_sock_fd && fds[i].revents & POLLIN)
            {
                clientAddrLen = sizeof(clientAddr);
                client_sock_fd = accept(server_sock_fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
                if (client_sock_fd == -1)
                {
                    perror("Failed to accept client connection");
                    exit(EXIT_FAILURE);
                }
                printf("New client connected: %s\n", inet_ntoa(((struct sockaddr_in *)&clientAddr)->sin_addr));

                // 将客户端 socket 添加到待监听列表中
                addClient(client_sock_fd);
            }
            // 处理客户端请求
            else if (fds[i].revents & POLLIN)
            {
                // 处理客户端发送的请求
                char buffer[1024];
                int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesRead == -1)
                {
                    perror("Failed to receive data from client");
                    exit(EXIT_FAILURE);
                }
                else if (bytesRead == 0)
                {
                    // 客户端连接已关闭
                    printf("Client disconnected\n");
                    close(fds[i].fd);
                }
                else
                {
                    // 处理客户端发送的请求
                    printf("来自客户端的请求:\n%s\n", buffer);

                    // 构造响应
                    const char *response = "HTTP/1.1 200 OK\r\n"
                                           "Content-Type: text/plain\r\n"
                                           "\r\n"
                                           "Hello, World!\r\n";

                    // 发送响应给客户端
                    int bytesSent = send(fds[i].fd, response, strlen(response), 0);
                    if (bytesSent == -1)
                    {
                        perror("Failed to send response to client");
                        exit(EXIT_FAILURE);
                    }

                    close(fds[i].fd);
                }
            }
        }
    }

    return 0;
}