#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

// Windows-specific headers
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE     4096
#define HTTP_VERSION    "HTTP/1.1"
#define CONNECTION_TYPE "Connection: close\r\n"

char *host_to_ip(const char *hostname) {
    struct addrinfo hints;
    struct addrinfo *result, *p;
    struct sockaddr_in *sockaddr;
    char *ip = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    if (getaddrinfo(hostname, NULL, &hints, &result) != 0)
        return NULL;

    for (p = result; p != NULL; p = p->ai_next) {
        sockaddr = (struct sockaddr_in *)p->ai_addr;
        ip = inet_ntoa(sockaddr->sin_addr);
        break;
    }

    freeaddrinfo(result);
    return ip;
}

SOCKET http_create_socket(const char *ip) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return INVALID_SOCKET;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        WSACleanup();
        return INVALID_SOCKET;
    }

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
        closesocket(sockfd);
        WSACleanup();
        return INVALID_SOCKET;
    }

    return sockfd;
}

char *http_send_request(const char *hostname, const char *resource) {
    char *ip = host_to_ip(hostname);
    if (ip == NULL) {
        return NULL;
    }

    SOCKET sockfd = http_create_socket(ip);
    if (sockfd == INVALID_SOCKET) {
        return NULL;
    }

    char buffer[BUFFER_SIZE] = {0};
    sprintf(buffer, "GET %s %s\r\nHost: %s\r\n%s\r\n\r\n",
            resource, HTTP_VERSION, hostname, CONNECTION_TYPE);

    send(sockfd, buffer, strlen(buffer), 0);

    fd_set fdread;
    FD_ZERO(&fdread);
    FD_SET(sockfd, &fdread);

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    char *result = malloc(sizeof(char));
    memset(result, 0, sizeof(char));

    while (1) {
        int selection = select(0, &fdread, NULL, NULL, &tv);
        if (!selection || !FD_ISSET(sockfd, &fdread)) {
            break;
        } else {
            memset(buffer, 0, BUFFER_SIZE);
            int len = recv(sockfd, buffer, BUFFER_SIZE, 0);
            if (len <= 0) {
                break;
            }

            result = realloc(result, (strlen(result) + len + 1) * sizeof(char));
            strncat(result, buffer, len);
        }
    }

    closesocket(sockfd);
    WSACleanup();

    return result;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <hostname> <resource>\n", argv[0]);
        return -1;
    }

    char *response = http_send_request(argv[1], argv[2]);
    if (response == NULL) {
        printf("An error occurred during the request.\n");
        return -1;
    }

    printf("Response:\n%s\n", response);

    free(response);

    return 0;
}

/*
todo

*/