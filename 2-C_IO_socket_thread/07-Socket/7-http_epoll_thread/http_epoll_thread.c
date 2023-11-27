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

int serverSocketFd;
int epollFd;

void *handle_client_request(void *arg)
{
    char buffer[MAX_BUFFER_SIZE];
    struct epoll_event *event = (struct epoll_event *)arg;
    int clientSocketFd = event->data.fd;


    int bytesRead = recv(clientSocketFd, buffer, sizeof(buffer), 0);
    if (bytesRead == -1)
    {
        perror("Failed to receive data from client");
        return NULL;
    }
    else if (bytesRead == 0)
    {
        printf("Client disconnected\n");
        close(clientSocketFd);
        return NULL;
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
        int bytesSent = send(clientSocketFd, response, strlen(response), 0);
        if (bytesSent == -1)
        {
            perror("Failed to send response to client");
            exit(EXIT_FAILURE);
        }
        printf("response 已发送，关闭客户端连接\n");

        close(clientSocketFd);
        return NULL;
    }
}

int main()
{
    int clientSocketFd, numReady;
    int ret;
    struct sockaddr_in serverAddr, clientAddr;
    struct epoll_event event, events[MAX_EVENTS];
    char buffer[MAX_BUFFER_SIZE];

    ret = pthread_mutex_init(&mutex, NULL); // 初始化互斥量
    if (ret == -1)
    {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    /*==1、socket bind listen */
    {
        serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocketFd == -1)
        {
            perror("Failed to create socket");
            exit(EXIT_FAILURE);
        }
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(PORT);
        if (bind(serverSocketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        {
            perror("Failed to bind");
            exit(EXIT_FAILURE);
        }
        if (listen(serverSocketFd, 5) == -1)
        {
            perror("Failed to listen");
            exit(EXIT_FAILURE);
        }
    }

    /*==2、创建 epollFd，指明监听 serverSocketFd 的 EPOLLIN 事件 */
    {
        epollFd = epoll_create1(0);
        if (epollFd == -1)
        {
            perror("Failed to create epoll instance");
            exit(EXIT_FAILURE);
        }

        // 添加服务器 socket 到 epoll 实例中
        event.data.fd = serverSocketFd;
        event.events = EPOLLIN;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocketFd, &event) == -1)
        {
            perror("Failed to add server socket to epoll");
            exit(EXIT_FAILURE);
        }
    }

    printf("Server started. Listening on port %d...\n", PORT);

    while (1)
    {
        /*==3、epoll_wait 阻塞等待事件发生
            当 epollFd 监听到有事件发生，会将事件的信息写入
            struct epoll_event events[MAX_EVENTS]
            并返回事件发生数量 */
        numReady = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numReady == -1)
        {
            perror("Failed to wait for events");
            exit(EXIT_FAILURE);
        }

        /*==4、处理事件
               1、当 serverSocketFd 监听到 EPOLLIN事件，说明有客户端请求连接服务器
               2、处理客户端请求，得到与客户端连接的 clientSocketFd
               3、使用 epollFd 指明监听 clientSocketFd 的 EPOLLIN 事件   */
        printf("###log：%s:%d ###\n", __FUNCTION__, __LINE__);
        for (int i = 0; i < numReady; i++)
        {

            if (events[i].data.fd == serverSocketFd)
            {
                printf("log：%s:%d 连接请求\n", __FUNCTION__, __LINE__);
                socklen_t len = sizeof(clientAddr);
                int clientSocketFd = accept(serverSocketFd, (struct sockaddr *)&clientAddr, &len);
                if (clientSocketFd < 0)
                {
                    printf("连接失败\n");
                    break;
                }

                // 将客户端 socket 添加到 epoll 实例中，在下一次 epoll_wait ，会直接返回 1
                event.data.fd = clientSocketFd;
                event.events = EPOLLIN;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocketFd, &event) == -1)
                {
                    perror("Failed to add client socket to epoll");
                    exit(EXIT_FAILURE);
                }
                printf("###log：%s:%d ###\n客户端连接成功，使用 epollFd 监听成功\n", __FUNCTION__, __LINE__);
            }
            // 处理客户端请求
            else
            {
                pthread_t tid;
                pthread_create(&tid, NULL, handle_client_request, (void *)&events[i]);
            }
        }
    }
    return 0;
}

/*


*/