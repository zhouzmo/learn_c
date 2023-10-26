#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>




#define SERVER_PORT 8888
#define BUF_SIZE (4096)
#define PERSON_MAX (5)		//群聊上限人数

//信号量--判断群聊人数
sem_t sem;
//服务端文件描述符
int server_fd;
//存储群友，多一个是为了当群人满时，空一个出来接发信息
int cli_fd[PERSON_MAX+1] = {};

//群发函数
void* send_all(char* buf)
{
	for(int i=0; i<PERSON_MAX; i++)
	{
		//若值为-1，则没有此群友，表示已经退出或未被占有
		if(-1 != cli_fd[i])
		{
			printf("%s\n",buf);
			printf("send to %d\n",cli_fd[i]);
			write(cli_fd[i],buf,strlen(buf)+1);
		}
	}
}


//服务端接收函数
void* server(void* arg)
{
	int fd = *(int*)arg;
	char buf[BUF_SIZE];
	char name[BUF_SIZE],ts[BUF_SIZE];
	
	//获取昵称
	read(fd,name,sizeof(name));
	sprintf(ts,"热烈欢迎 %s 进入群聊",name);
	send_all(ts);
	
	for(;;)
	{
		//接收信息
		int recv_size = read(fd,buf,sizeof(buf));
		//收到退出信息
		if(0 >= recv || NULL != strstr(buf,"quit"))
		{

			sprintf(ts,"欢送 %s 离开群聊\n",name);
			int index = 0;
			//找到要退出的那个人，并将其置为-1
			for(; index < PERSON_MAX; index++)
			{
				if(cli_fd[index] == fd)
				{
					cli_fd[index] = -1;
					break;
				}
			}
			send_all(ts);
			
			//群友退出，信号量+1
			int n;
			sem_post(&sem);
			sem_getvalue(&sem,&n);
			
			printf("%s 离开群聊,群聊还剩%d人\n",name,PERSON_MAX-n);
			strcpy(buf,"quit");
			write(fd,buf,strlen(buf)+1);
			close(fd);
			pthread_exit(NULL);
		}
		send_all(buf);
	}
}

void sigint(int signum)
{
	close(server_fd);
	sem_destroy(&sem);
	printf("服务器关闭\n");
	exit(0);
}

int main()
{
	int iRet ;
	// 当接受到 SIGINT  信号时要调用名为 sigint 的信号处理函数。
	signal(SIGINT,sigint);
	//初始化信号量，群聊上限PERSON_MAX人
	sem_init(&sem,0,PERSON_MAX);
	
	//创建socket对象
	printf("创建socket对象...\n");
	server_fd = socket(AF_INET,SOCK_STREAM,0);
	// if(0 > server_fd)
	// {
	// 	perror("socket");
	// 	return -1;
	// }
	
	//准备通信地址(自己)
	printf("准备通信地址...\n");
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6789);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//绑定socket对象与地址
	printf("绑定socket对象与地址...\n");
	socklen_t addrlen = sizeof(addr);
	iRet = bind(server_fd,(struct sockaddr*)&addr,addrlen);
	// if(iRet)
	// {
	// 	perror("bind");
	// 	return -1;
	// }
	
	//设置监听和排除数量
	printf("设置监听");
	iRet = listen(server_fd,10);
	// if(iRet)
	// {
	// 	perror("listen");
	// 	return -1;
	// }
	
	printf("等待客户端链接...\n");
	//将初始值置全为-1，表示该聊天位置没有人占领
	memset(cli_fd,-1,sizeof(cli_fd));
	int sem_num;
	int index;
	for(;;)
	{
		
		sem_getvalue(&sem,&sem_num);
		
		//找到没有人占领的聊天位
		 index = 0;
		while(-1 != cli_fd[index]) index++;

		cli_fd[index] = accept(server_fd,(struct sockaddr*)&addr,&addrlen);
		
		if(0 > cli_fd[index])
		{
			perror("accept");
			return -1;
		}
		
		char buf[BUF_SIZE];
		if(0 >= sem_num)
		{
			printf("人数已满，%d号客户端链接失败\n",cli_fd[index]);
			sprintf(buf,"人数已满，客户端链接失败");
			write(cli_fd[index],buf,strlen(buf)+1);
			close(cli_fd[index]);
			cli_fd[index] = -1;
		}
		else
		{
			sem_trywait(&sem);
			sem_getvalue(&sem,&sem_num);
			char msg[BUF_SIZE] = {};
			printf("%d号客户端链接成功,当前聊天人数%d人\n",cli_fd[index],PERSON_MAX-sem_num);
			sprintf(msg,"客户端链接成功,当前聊天人数%d人\n",PERSON_MAX-sem_num);
			write(cli_fd[index],msg,strlen(msg)+1);
			
			//创建线程客户端
			pthread_t tid;
			pthread_create(&tid,NULL,server,&cli_fd[index]);
		}
	}
}
