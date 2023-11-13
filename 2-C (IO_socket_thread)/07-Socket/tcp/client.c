#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

/* socket
 * connect
 * send/recv

创建 socket
建立 TCP 连接
发送/接收数据


 */
//  ./client 127.0.0.1 
#define SERVER_PORT 8888

int main(int argc, char **argv)
{
	int iSocketClient;
	struct sockaddr_in tSocketServerAddr;

	int iRet;
	unsigned char ucSendBuf[1000];
	int iSendLen;

	if (argc != 2)
	{
		printf("Usage:\n");
		printf("%s <server_ip>\n", argv[0]);
		return -1;
	}
	/*=========== 创建 socket ====================*/
	iSocketClient = socket(AF_INET, SOCK_STREAM, 0);

	tSocketServerAddr.sin_family = AF_INET;
	tSocketServerAddr.sin_port = htons(SERVER_PORT); /* host to net, short */
	// tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
	if (0 == inet_aton(argv[1], &tSocketServerAddr.sin_addr))
	{
		printf("invalid server_ip\n");
		return -1;
	}
	memset(tSocketServerAddr.sin_zero, 0, 8);

	/*==========建立 TCP 连接=====================*/
	iRet = connect(iSocketClient, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("connect error!\n");
		return -1;
	}

	while (1)
	{
		if (fgets(ucSendBuf, 999, stdin))
		{
			/*=======发送/接收数据=========================*/
			iSendLen = send(iSocketClient, ucSendBuf, strlen(ucSendBuf), 0);
			if (iSendLen <= 0)
			{
				close(iSocketClient);
				return -1;
			}
		}
	}

	return 0;
}

/*
zmo@zmohost:~/code/ccode/01-c_learn/5-advance/socket/tcp$ ./client 127.0.0.1 
12

zmo@zmohost:~/code/ccode/01-c_learn/5-advance/socket/tcp$ ./client 127.0.0.1 
212312321

*/