#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

#define MAX_CLIENTS 100
#define MAX_MSG_LEN 1024

#define PORT 5555	   // 1024-49151
#define IP "127.0.0.1" // 本机ip地址 用ifconfig查看

struct message
{
	char username[50];
	char content[MAX_MSG_LEN];
};

int clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void send_message(struct message msg, int self)
{
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (clients[i] && i != self)
		{
			send(clients[i], &msg, sizeof(msg), 0);
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg)
{
	int client_fd = (int)(intptr_t)arg;
	struct message msg;
	recv(client_fd, &msg, sizeof(msg), 0);
	printf("%s 加入聊天室\n", msg.username);

	while (1)
	{
		if (recv(client_fd, &msg, sizeof(msg), 0) <= 0)
		{
			pthread_mutex_lock(&clients_mutex);
			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				if (clients[i] == client_fd)
				{
					printf("%s 离开聊天室\n", msg.username);
					clients[i] = 0;
					break;
				}
			}
			pthread_mutex_unlock(&clients_mutex);
			break;
		}
		else
		{
			printf("%s: %s\n", msg.username, msg.content);
			send_message(msg, client_fd);
		}
	}

	close(client_fd);
}

void *send_to_client(void *arg)
{
	struct message msg;
	strcpy(msg.username, "群发消息");

	char allmsg[MAX_MSG_LEN];
	while (1)
	{
		printf("群发消息:\n");
		fgets(msg.content, MAX_MSG_LEN, stdin);
		msg.content[strlen(msg.content) - 1] = '\0';
		int i;
		for (i = 0; i < MAX_CLIENTS; i++)
		{
			if (clients[i] != 0)
			{
				send(clients[i], &msg, sizeof(msg), 0);
			}
			else
			{
				break;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int server_fd, client_fd, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	int err;

	memset(clients, 0, MAX_CLIENTS * sizeof(int));
	err = server_fd = socket(AF_INET, SOCK_STREAM, 0);
	// if (err== 0)
	// {
	//     perror("socket failed");
	//     exit(EXIT_FAILURE);
	// }
	err = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, &opt, sizeof(opt));
	// if (err)
	// {
	//     perror("setsockopt failed");
	//     exit(EXIT_FAILURE);
	// }

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(IP);
	address.sin_port = htons(PORT);
	err = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	// if (err< 0)
	// {
	// 	perror("bind failed");
	// 	exit(EXIT_FAILURE);
	// }
	err = listen(server_fd, 3);
	if (err != 0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	printf("服务器开启端口号: %d\n", PORT);
	printf("等待客户端连接...\n");

	pthread_t server_thread;
	err = pthread_create(&server_thread, NULL, send_to_client, (void *)(intptr_t)client_fd);
	if (err != 0)
	{
		printf("线程创建失败\n");
	}

	while (1)
	{
		err = client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
		// if (err < 0)
		// {
		// 	perror("accept failed");
		// 	exit(EXIT_FAILURE);
		// }

		int i;
		for (i = 0; i < MAX_CLIENTS; i++)
		{
			if (clients[i] == 0)
			{
				clients[i] = client_fd;
				break;
			}
		}

		if (i == MAX_CLIENTS)
		{
			printf("客户端连接失败\n");
			close(client_fd);
			continue;
		}

		pthread_t new_thread;
		err = pthread_create(&new_thread, NULL, handle_client, (void *)(intptr_t)client_fd);
		if (err != 0)
		{
			printf("线程创建失败\n");
			continue;
		}
	}

	return 0;
}