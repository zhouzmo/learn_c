#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAX_CLIENTS 10
#define PORT 8080

int main() {
    int server_fd, new_socket, client_sockets[MAX_CLIENTS];
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 绑定套接字
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听连接
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    int max_clients = MAX_CLIENTS;
    int activity, i;

    struct pollfd fds[max_clients];
    for (i = 0; i < max_clients; i++) {
        client_sockets[i] = 0;
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    fds[0].fd = server_fd;

    while (1) {
        activity = poll(fds, max_clients, -1); // 阻塞等待事件

        if (activity < 0) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, IP is : %s, port : %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            for (i = 1; i < max_clients; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    fds[i].fd = new_socket;
                    break;
                }
            }
        }

        for (i = 1; i < max_clients; i++) {
            int client_socket = client_sockets[i];
            if (client_socket > 0 && (fds[i].revents & POLLIN)) {
                char buffer[1024] = {0};
                ssize_t valread = recv(client_socket, buffer, sizeof(buffer), 0);
                if (valread <= 0) {
                    printf("Client disconnected\n");
                    close(client_socket);
                    client_sockets[i] = 0;
                    fds[i].fd = -1;
                } else {
                    printf("Received: %s", buffer);
                }
            }
        }
    }

    return 0;
}
