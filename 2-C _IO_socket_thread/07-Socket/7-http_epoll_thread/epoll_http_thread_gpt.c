#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_EVENTS 10
#define MAX_BUFFER_SIZE 1024
#define PORT 8888

int handleThreadNum = 0;

// 线程处理函数
void *handle_client(void *arg)
{
    int clientSocket = *(int *)arg;
    char buffer[MAX_BUFFER_SIZE];

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1)
    {
        perror("Failed to receive data from client");
        exit(EXIT_FAILURE);
    }
    else if (bytesRead == 0)
    {
        // 客户端连接已关闭
        printf("Client disconnected\n");
        close(clientSocket);
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
        int bytesSent = send(clientSocket, response, strlen(response), 0);
        if (bytesSent == -1)
        {
            perror("Failed to send response to client");
            exit(EXIT_FAILURE);
        }
        printf("response 已发送，关闭客户端连接\n");

        close(clientSocket);
    }
    printf("创建线程数量: %d\n", handleThreadNum++);

    pthread_exit(NULL);
}

int main()
{
    int serverSocket, clientSocket, epollFd, numReady;
    struct sockaddr_in serverAddr, clientAddr;
    struct epoll_event event, events[MAX_EVENTS];
    char buffer[MAX_BUFFER_SIZE];
    int threadNum = 0;

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

                // 创建线程来处理客户端连接
                pthread_t tid;
                if (pthread_create(&tid, NULL, handle_client, &clientSocket) != 0)
                {
                    perror("Failed to create thread");
                    exit(EXIT_FAILURE);
                }

                // 分离线程，使其在退出时自动释放资源
                printf("创建线程数量: %d\n", threadNum++);

                pthread_detach(tid);
            }
        }
    }

    return 0;
}