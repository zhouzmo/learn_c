
// #include <iostream>
#include <WinSock2.h>
#include <stdio.h>
#include <winsock2.h>


/*
预处理指令，用于指定链接器在编译时自动链接指定的库文件。

具体解释如下：

#pragma comment(lib, "iphlpapi.lib")：这条指令告诉链接器在编译时自动链接 iphlpapi.lib 库文件。iphlpapi.lib 是 Windows 网络编程接口库的一部分，提供了访问和操作网络接口、协议和路由信息的功能。

#pragma comment(lib, "ws2_32.lib")：这条指令告诉链接器在编译时自动链接 ws2_32.lib 库文件。ws2_32.lib 是 Windows 套接字编程接口库的一部分，提供了网络编程中套接字相关的函数和结构体。

通过在代码中使用这些 #pragma comment(lib, "...") 指令，开发者可以方便地指定需要链接的库文件，而无需在编译命令中显式地指定。这样可以简化构建过程，并确保必要的库文件在链接时被正确地包含进来。

请注意，#pragma comment(lib, "...") 指令是特定于编译器的非标准扩展，并且在不同的编译器和平台上可能有所不同。以上解释基于常见的 Windows 平台和相关工具链。如果您在其他平台或使用其他编译器
*/
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

int main()
{
	SOCKET socket = {0};	// 定义一个socket对象
	fd_set fdset = {0};		// 声明并定义，如果不赋初值，fd_set中存储的则是随机值

	FD_ZERO(&fdset);
	FD_SET(1, &fdset); // ’联系‘就是在这里产生的,以下4个操作会产生其他4个联系
	FD_SET(2, &fdset);
	FD_SET(3, &fdset);
	FD_SET(7, &fdset);
	FD_SET(socket, &fdset);

	int isset = FD_ISSET(socket, &fdset); // ’联系‘就是在这里产生的
	printf("isset = %d\n", isset); // isset = 1
	FD_CLR(socket, &fdset);
	isset = FD_ISSET(socket, &fdset); // isset = 0
	printf("isset = %d\n", isset);
	return 0;
}

/*
 这里的fd 实际使用都是以 句柄 传入
FD_ZERO(fd_set *fdset);              // 将set清零使集合中不含任何fd
FD_SET(int fd, fd_set *fdset);       // 将fd加入set集合
FD_CLR(int fd, fd_set *fdset);       // 将fd从set集合中清除
FD_ISSET(int fd, fd_set *fdset);     // 检测fd是否在set集合中，不在则返回0
*/