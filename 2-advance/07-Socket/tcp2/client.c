#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_USERNAME_LEN 50
#define MAX_MSG_LEN 1024

#define PORT 5555	   // 1024-49151
#define IP "127.0.0.1" // 本机ip地址 用ifconfig查看

struct message
{
	char username[MAX_USERNAME_LEN];
	char content[MAX_MSG_LEN];
};

void *recv_message(void *arg)
{
	int sockfd = *(int *)arg;
	struct message msg;

	while (1)
	{
		ssize_t len = recv(sockfd, &msg, sizeof(msg), 0);
		if (len <= 0)
		{
			fprintf(stderr, "错误: 服务器断开连接 \n");
			break;
		}
		else
		{
			printf("%s: %s\n", msg.username, msg.content);
		}
	}

	close(sockfd); // 关闭套接字
	exit(0);
}

int main(int argc, char *argv[])
{

	int sockfd;
	struct sockaddr_in serv_addr;
	char username[MAX_USERNAME_LEN];
	char password[MAX_USERNAME_LEN];
	struct message msg;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "错误: 套接字创建失败 \n");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0)
	{
		fprintf(stderr, "错误: 无效地址 \n");
		exit(1);
	}

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "错误: 连接服务器失败 \n");
		exit(1);
	}

	printf("请输入账户: ");
	fgets(username, MAX_USERNAME_LEN, stdin);
	username[strlen(username) - 1] = '\0';
	// printf("请输入密码: ");
	// fgets(password, MAX_USERNAME_LEN, stdin);
	// password[strlen(password) - 1] = '\0';

	// 拷贝到 msg 内部
	strcpy(msg.username, username);

	if (send(sockfd, &msg, sizeof(msg), 0) == -1)
	{
		fprintf(stderr, "错误: 发送消息失败 \n");
		close(sockfd); // 关闭套接字
		exit(1);
	}

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, recv_message, (void *)&sockfd);

	while (1)
	{
		printf("发送消息: ");
		fgets(msg.content, MAX_MSG_LEN, stdin);
		msg.content[strlen(msg.content) - 1] = '\0';

		if (send(sockfd, &msg, sizeof(msg), 0) == -1)
		{
			fprintf(stderr, "错误: 发送消息失败 \n");
			break;
		}
	}

	pthread_cancel(thread_id);
	close(sockfd);

	return 0;
}