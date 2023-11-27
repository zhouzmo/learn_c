#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>




/* socket
 * connect
 * send/recv

创建 socket
建立 TCP 连接
发送/接收数据


 */
//  ./client 127.0.0.1 
#define SERVER_PORT 8888
#define BUF_SIZE 4096


void* client_read(void* arg)
{
	int cli_fd = *(int*)arg;
	char buf[BUF_SIZE];
	
	//接受数据
	for(;;)
	{
		int recv_size = read(cli_fd,buf,BUF_SIZE);
		if(0 >= recv_size || 0 == strcmp(buf,"quit"))
		{
			printf("已经与服务器断开链接\n");
			pthread_exit(NULL);
		}
		printf("%s\n",buf);
	}
}




int main(int argc, char **argv)
{
	int clientFd;
	struct sockaddr_in sAddrIn;

	int iRet;
	unsigned char ucSendBuf[1000];
	int iSendLen;

	if (argc != 2)
	{
		printf("Usage:\n");
		printf("%s <server_ip>\n", argv[0]);
		return -1;
	}
	char* localhostIP = argv[1];

	printf("创建socket对象...\n");
	clientFd = socket(AF_INET, SOCK_STREAM, 0);
	// if(0 > clientFd)
	// {
	// 	perror("socket");
	// 	return -1;
	// }
	printf("准备通信地址...\n");
	sAddrIn.sin_family = AF_INET;
	sAddrIn.sin_port = htons(SERVER_PORT); 
	sAddrIn.sin_addr.s_addr = inet_addr(localhostIP);
	// if (0 == inet_aton(argv[1], &tSocketServerAddr.sin_addr))
	// {
	// 	printf("invalid server_ip\n");
	// 	return -1;
	// }


	printf("链接服务端口...\n");
	iRet = connect(clientFd, (const struct sockaddr *)&sAddrIn, sizeof(sAddrIn));
	// if (-1 == iRet)
	// {
	// 	printf("connect error!\n");
	// 	return -1;
	// }


	char buf[BUF_SIZE];
	
	read(clientFd,buf,BUF_SIZE);
	if(NULL == strstr(buf,"链接成功"))
	{
		printf("群聊人已满，请稍后再来\n");
		close(clientFd);
		return 0;
	}
	printf("%s\n",buf);

	//创建线程客户端
	pthread_t tid;
	pthread_create(&tid,NULL,client_read,&clientFd);
	
	//输入昵称
	char name[BUF_SIZE] = {};
	printf("请输入你的昵称：");
	gets(name);
	write(clientFd,name,strlen(name)+1);
	
	//发送数据
	for(;;)
	{
		printf(">>");
		gets(buf);
		
		char msg[BUF_SIZE];
		sprintf(msg,"%s:%s",name,buf);
		
		int send_size = write(clientFd,msg,strlen(msg)+1);
		if(0 >= send_size || 0 == strcmp(buf,"quit"))
		{
			printf("结束通信\n");
			close(clientFd);
			pthread_exit(NULL);
			return 0;
		}
	}
}

/*
zmo@zmohost:~/code/ccode/01-c_learn/5-advance/socket/tcp$ ./client 127.0.0.1 
12

zmo@zmohost:~/code/ccode/01-c_learn/5-advance/socket/tcp$ ./client 127.0.0.1 
212312321

*/