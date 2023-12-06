#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

/*
udp server
	1、socket(PF_INET, SOCK_DGRAM, 0)
	2、bind
	3、recvfrom/sendto
	4、close
udp client
	1、socket(PF_INET, SOCK_DGRAM, 0)
	2、recvfrom/sendto
	3、close
*/

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
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("bind");

	struct sockaddr_in peeraddr;
	socklen_t peerlen;
	char recvbuf[1524];

	while (1)
	{
		memset(recvbuf, 0, sizeof(recvbuf));
		peerlen = sizeof(peeraddr);

		int n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&peeraddr, &peerlen);
		if (n == -1)
		{
			if (errno == EINTR)
				continue;
			ERR_EXIT("recvfrom");
		}

		fputs(recvbuf, stdout);

		int ret = sendto(sock, recvbuf, n, 0, (struct sockaddr *)&peeraddr, peerlen);
		if (ret == -1)
			ERR_EXIT("sendto");

		printf("ret: %d\n", ret);
	}

	close(sock);

	return 0;
}

/*
没有连接
 netstat -na | grep 8888
udp        0      0 0.0.0.0:8888            0.0.0.0:*

*/