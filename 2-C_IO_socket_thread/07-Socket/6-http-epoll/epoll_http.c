#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define MAX_BUFFER_SIZE 1024
#define PORT 8888

int main()
{
    int serverSocket, clientSocket, epollFd, numReady;
    struct sockaddr_in serverAddr, clientAddr;
    struct epoll_event event, events[MAX_EVENTS];
    char buffer[MAX_BUFFER_SIZE];

    /*==1、socket bind listen */
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Failed to bind");
        exit(EXIT_FAILURE);
    }
    if (listen(serverSocket, 5) == -1)
    {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    // 创建 epoll 实例
    epollFd = epoll_create1(0);
    if (epollFd == -1)
    {
        perror("Failed to create epoll instance");
        exit(EXIT_FAILURE);
    }

    // 添加服务器 socket 到 epoll 实例中
    event.data.fd = serverSocket;
    event.events = EPOLLIN;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &event) == -1)
    {
        perror("Failed to add server socket to epoll");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Listening on port %d...\n", PORT);

    while (1)
    {
        // 等待事件发生
        numReady = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numReady == -1)
        {
            perror("Failed to wait for events");
            exit(EXIT_FAILURE);
        }

        // 处理就绪的事件
        printf("###log：%s:%d numReady %d\n", __FUNCTION__, __LINE__, numReady);
        for (int i = 0; i < numReady; i++)
        {
            printf("log：%s:%d \n", __FUNCTION__, __LINE__);
            if (events[i].data.fd == serverSocket)
            {
                printf("log：%s:%d \n", __FUNCTION__, __LINE__);
                socklen_t clientAddrLen = sizeof(clientAddr);
                clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
                if (clientSocket == -1)
                {
                    perror("Failed to accept client connection");
                    exit(EXIT_FAILURE);
                }
                printf("New client connected: %s\n", inet_ntoa(clientAddr.sin_addr));

                // 将客户端 socket 添加到 epoll 实例中，在下一次 epoll_wait ，会直接返回 1
                event.data.fd = clientSocket;
                event.events = EPOLLIN;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1)
                {
                    perror("Failed to add client socket to epoll");
                    exit(EXIT_FAILURE);
                }
            }
            // 处理客户端请求
            else
            {
                printf("log：%s:%d \n", __FUNCTION__, __LINE__);
                int bytesRead = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                if (bytesRead == -1)
                {
                    perror("Failed to receive data from client");
                    exit(EXIT_FAILURE);
                }
                else if (bytesRead == 0)
                {
                    // 客户端连接已关闭
                    printf("Client disconnected\n");
                    close(events[i].data.fd);
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
                    int bytesSent = send(events[i].data.fd, response, strlen(response), 0);
                    if (bytesSent == -1)
                    {
                        perror("Failed to send response to client");
                        exit(EXIT_FAILURE);
                    }
                    printf("response 已发送，关闭客户端连接\n");
                  
                    close(events[i].data.fd);
                }
            }
        }
    }

    return 0;
}