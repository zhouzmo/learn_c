#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define ERR_EXIT(msg)       \
	do                      \
	{                       \
		perror(msg);        \
		exit(EXIT_FAILURE); \
	} while (0)

int main(void)
{
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		ERR_EXIT("socket");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8888);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 修改为服务器的IP地址

	char sendbuf[1524];
	while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
	{
		int ret = sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		if (ret == -1)
			ERR_EXIT("sendto");

		memset(sendbuf, 0, sizeof(sendbuf));

		ret = recvfrom(sock, sendbuf, sizeof(sendbuf), 0, NULL, NULL);
		if (ret == -1)
		{
			if (errno == EINTR)
				continue;
			ERR_EXIT("recvfrom");
		}

		fputs(sendbuf, stdout);
	}

	close(sock);

	return 0;
}