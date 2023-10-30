/*
================================================================
? int socket(int domain, int type, int protocol);
用于创建套接字，即一个用于网络通信的文件描述符。
它允许您指定套接字的地址家族（通常是 IPv4 地址家族）、类型（面向连接或无连接
套接字）以及协议（通常为 0，表示自动选择合适的协议）。
参数
domain 
	参数指定了套接字的地址家族或协议族，通常是 AF_INET 
	表示 IPv4 地址家族。
type 
	参数指定了套接字的类型，常用的是 SOCK_STREAM 表示面
	向连接的套接字（如 TCP 套接字）或 SOCK_DGRAM 表示无
	连接的套接字（如 UDP 套接字）。
protocol 
	参数通常设置为 0，表示根据 domain 和 type 的
	值自动选择合适的协议。
返回值：
	如果函数调用成功，它将返回一个新创建的套接字的文件描述符，
	如果失败，则返回 -1，并设置 errno 来指示错误的原因。
================================================================
? int inet_aton(const char *cp, struct in_addr *inp);
用于将点分十进制的 IP 地址字符串转换为二进制形式的 IPv4 地址。
参数
cp：
	是一个指向包含点分十进制 IP 地址字符串的 C 字符串的指针。
inp：
	是一个指向 struct in_addr 结构的指针，用于存储转换后的结果。
返回值：
	如果函数调用成功，它将返回 1，并且将转换后的 IPv4 地址存储在 inp 指向的结构体中。
	如果失败，它将返回 0，表示无效的 IPv4 地址字符串。
================================================================
? ssize_t read(int fd, void *buf, size_t count);
用于从文件或套接字中读取数据，并将读取的数据存储到指定的缓冲区中。
参数
fd：
	文件描述符或套接字描述符，表示要从哪个文件或套接字读取数据。
buf：
	一个指向缓冲区的指针，用于存储读取的数据。
count：
	要读取的字节数，即你希望读取多少数据。

返回值
	是一个 ssize_t 类型的整数，表示实际读取的字节数。
	如果成功读取数据，返回的值将是大于等于 0 的字节数。
	如果到达文件末尾，返回值将为 0。
	如果出现错误，返回值将是 -1，并且 errno 变量将包含有关错误的信息。


================================================================
? int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
将套接字绑定到特定的网络地址和端口上的操作，通常在服务器端程序中使用。
参数：
sockfd：
	是要绑定的套接字的文件描述符，通常是通过调用 socket 函数创建的套接字。
addr：
	是一个指向 struct sockaddr 结构的指针，用于指定要绑定的地址信息。
	通常，这个结构会被强制类型转换为 struct sockaddr* 类型。
	这个结构包含了地址族（如 AF_INET 表示 IPv4 地址族）以及具体的地址和端口信息。
addrlen：
	是 addr 结构的长度，通常使用 sizeof 运算符来获取。
返回值：
   如果 bind 函数成功将套接字绑定到指定的地址和端口，它将返回 0。
	如果失败，返回值将是一个非零的错误码，用于指示错误的原因。
================================================================
? int listen(int sockfd, int backlog);
将指定的套接字 sockfd 设置为监听状态，以便等待客户端的连接请求。
一旦套接字处于监听状态，服务器可以接受多个客户端的连接，
每个客户端连接都会创建一个新的套接字，用于与客户端通信。
参数：
sockfd：是要设置为监听状态的套接字的文件描述符，通常是通过调用 socket 函数创建的套接字。
backlog：是一个整数，指定在等待队列中允许排队的连接请求的最大数量。这个参数决定了服务器可以同时处理的等待连接的客户端数目。
返回值：
如果 listen 函数成功将套接字设置为监听状态，它将返回 0。
如果失败，返回值将是一个非零的错误码，用于指示错误的原因。
================================================================
? int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
等待并接受客户端的连接请求，并创建一个新的套接字用于与客户端通信。
一旦客户端连接成功，accept 函数返回一个新的套接字文件描述符，
该套接字用于与特定客户端进行通信。这使得服务器能够同时处理多个客户端连接请求。
参数：
sockfd：是服务器套接字的文件描述符，即要等待客户端连接请求的套接字。
addr：是一个指向 struct sockaddr 结构的指针，用于存储客户端的地址信息。通常，这个参数可以设置为 NULL，表示不需要获取客户端的地址信息。如果您需要获取客户端的地址信息，可以提供一个指向 struct sockaddr 的有效指针。
addrlen：是一个指向 socklen_t 类型的指针，指向 addr 结构的长度。如果您不关心客户端的地址信息，可以将该参数设置为 NULL。
返回值：
如果 accept 函数成功接受客户端连接，它将返回一个新的套接字文件描述符，该套接字用于与客户端进行通信。这个文件描述符是一个整数值。
如果失败，返回值将是一个负数，通常是 -1，
================================================================
? ssize_t recv(int sockfd, void *buf, size_t len, int flags);
作用：
recv 函数的主要作用是从指定的套接字 sockfd 中接收数据，并将数据存储到指定的缓冲区 buf 中。这个函数通常用于接收网络套接字上的数据，例如从客户端接收数据。

参数：
sockfd：是要接收数据的套接字的文件描述符。
buf：是一个指向用于存储接收数据的缓冲区的指针。
len：是要接收的最大字节数，即您希望接收多少数据。
flags：是一个标志参数，用于控制接收操作的行为。通常可以设置为 0，表示没有特殊的标志。
返回值：
recv 函数的返回值是一个 ssize_t 类型的整数，表示实际接收的字节数。可能的返回值包括：

如果成功接收数据，返回值将是大于等于 0 的字节数，表示已接收的数据大小。
如果连接已关闭（客户端断开连接），返回值将为 0，表示已经接收到 EOF（文件结束符）。
如果出现错误，返回值将是 -1，并且可以使用 perror 函数或其他错误处理机制来获取错误信息。
================================================================
? ssize_t recv(int sockfd, void *buf, size_t len, int flags);
作用：
recv 函数的主要作用是从指定的套接字 sockfd 中接收数据，并将数据存储到指定的缓冲区 buf 中。这个函数通常用于接收网络套接字上的数据，例如从客户端接收数据。

参数：
sockfd：是要接收数据的套接字的文件描述符。
buf：是一个指向用于存储接收数据的缓冲区的指针。
len：是要接收的最大字节数，即您希望接收多少数据。
flags：是一个标志参数，用于控制接收操作的行为。通常可以设置为 0，表示没有特殊的标志。
返回值：
recv 函数的返回值是一个 ssize_t 类型的整数，表示实际接收的字节数。可能的返回值包括：

如果成功接收数据，返回值将是大于等于 0 的字节数，表示已接收的数据大小。
如果连接已关闭（客户端断开连接），返回值将为 0，表示已经接收到 EOF（文件结束符）。
如果出现错误，返回值将是 -1，并且可以使用 perror 函数或其他错误处理机制来获取错误信息。
================================================================
?ssize_t send(int sockfd, const void *buf, size_t len, int flags);
通过已连接的套接字发送数据
原型：
sockfd：表示要发送数据的套接字的文件描述符。
buf：是一个指向包含要发送数据的缓冲区的指针。
len：是要发送的数据的字节数。
flags：是一个用于控制发送操作行为的标志参数，通常可以设置为 0。
作用：
send 函数的主要作用是将数据从套接字发送到远程主机。它通常用于在网络编程中向其他计算机发送数据。

参数：

sockfd：要发送数据的套接字的文件描述符。
buf：指向包含要发送数据的缓冲区的指针。
len：要发送的数据的字节数。
flags：标志参数，用于控制发送操作的行为。通常可以设置为 0。
返回值：
send 函数的返回值是一个 ssize_t 类型的整数，表示实际发送的字节数。可能的返回值包括：

如果成功发送数据，返回值将是大于等于 0 的字节数，表示已成功发送的数据大小。
如果发送操作被中断，返回值将为 -1，并且可以使用 perror 函数或其他错误处理机制来获取错误信息。
如果连接已关闭（对方断开连接），返回值将为 0，表示没有数据被发送。
*/