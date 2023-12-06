#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

/*

实验1：
不启动服务器，启动客户端，发送数据
现象：发送成功，并阻塞在  recvfrom
333333333
ret = 10
sendto success
结论:
1、UDP发送报文时，只把数据复制到发送缓冲区。在服务器没有启动的情况下，可以发送成功。


实验2：
不启动服务器，启动客户端，先 connet ，再发送数据
现象：发送成功，但是出现异常 recvfrom: Connection refused
qqqq
ret = 5
sendto success
recvfrom: Connection refused
结论:
1、客户端调用 connent ，udp 也可以调用 connect
2、udp 客户端调用 connect 后，不会阻塞在 recv 里

*/

/*
- ICMP异步错误是指在发送UDP报文时没有错误，但在调用`recvfrom`接收报文时收到了ICMP错误消息的情况。

	现象：当UDP服务器关闭后，若UDP客户端在从键盘接收数据后再发送数据，UDP客户端可能会阻塞在`sendto`位置。

	1. 说明1：UDP发送报文时，只把数据复制到发送缓冲区。在服务器没有启动的情况下，可以发送成功。
	2. 说明2：所谓ICMP异步错误是指，在发送报文时没有错误，但在接收报文时收到了ICMP应答。
	3. 说明3：异步错误无法返回未连接的套接字。UDP也可以调用`connect`函数。

- UDP connect

	- 说明1：UDP调用`connect`并没有进行三次握手，只是维护了一个与对等方的状态信息。
	- 说明2：一旦调用`connect`，就可以使用`send`函数。

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
	int ret = 0;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		ERR_EXIT("socket");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8888);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// UDP调用`connect`并没有进行三次握手，只是维护了一个与对等方的状态信息。
	connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));

	char sendbuf[1024];
	while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
	{
		// ret = sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		// if (ret == -1)
		// 	ERR_EXIT("sendto");

		// 当调用 connect 后，可以字节使用 send，而不用 sendto
		ret = send(sock, sendbuf, strlen(sendbuf), 0);

		printf("ret = %d\n", ret);
		printf("sendto success\n");

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