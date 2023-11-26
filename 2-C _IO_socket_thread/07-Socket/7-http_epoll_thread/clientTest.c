#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_THREADS 10
#define REQUEST_COUNT 10000
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

void *send_requests(void *arg) {
    struct timeval start_time, end_time;
    int clientSocket;
    struct sockaddr_in serverAddr;
    char request[] = "GET / HTTP/1.1\r\n"
                     "Host: localhost\r\n"
                     "Connection: keep-alive\r\n"
                     "\r\n";

    // 创建客户端 socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Failed to create socket");
        pthread_exit(NULL);
    }

    // 设置服务器地址
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(SERVER_PORT);

    // 连接服务器
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Failed to connect to server");
        close(clientSocket);
        pthread_exit(NULL);
    }

    // 发送请求并计时
    gettimeofday(&start_time, NULL);
    for (int i = 0; i < REQUEST_COUNT; i++) {
        if (send(clientSocket, request, strlen(request), 0) == -1) {
            perror("Failed to send request");
            close(clientSocket);
            pthread_exit(NULL);
        }
    }
    gettimeofday(&end_time, NULL);

    // 计算响应时间
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                          (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    printf("Thread %lu: Elapsed time: %.3f seconds\n", pthread_self(), elapsed_time);

    // 关闭客户端 socket
    close(clientSocket);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[MAX_THREADS];
    
    // 创建多个线程模拟多个客户端发送请求
    for (int i = 0; i < MAX_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, send_requests, NULL) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // 等待所有线程结束
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}