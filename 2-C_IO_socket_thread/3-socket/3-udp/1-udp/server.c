#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

/*

#### recvfrom函数
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
				struct sockaddr *src_addr, socklen_t *addrlen);

- 功能：从已连接的套接字接收数据，并将数据存储在缓冲区中。
- 参数：
  - `sockfd`：套接字描述符。
  - `buf`：指向接收数据的缓冲区。
  - `len`：缓冲区的大小。
  - `flags`：接收操作的标志，通常设置为0。
  - `src_addr`：指向发送方的`sockaddr`结构体的指针，用于存储发送方的地址信息（可选）。
  - `addrlen`：指向`src_addr`结构体的长度的指针（可选）。
- 返回值：返回接收到的字节数，如果出现错误，返回-1。

#### sendto函数
#include <sys/types.h>
#include <sys/socket.h>

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
			   const struct sockaddr *dest_addr, socklen_t addrlen);

- 功能：向指定目标发送数据。
- 参数：
  - `sockfd`：套接字描述符。
  - `buf`：指向要发送的数据的缓冲区。
  - `len`：要发送的数据的大小。
  - `flags`：发送操作的标志，通常设置为0。
  - `dest_addr`：指向目标主机的`sockaddr`结构体的指针，包含目标主机的地址信息。
  - `addrlen`：`dest_addr`结构体的长度。
- 返回值：返回发送的字节数，如果出现错误，返回-1。



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