#include "scr_util.h"

/*
 * 将文件描述符设置为非阻塞模式。
 * 参数：
 *   fd: 文件描述符
 * 返回值：
 *   无
 */
void activate_nonblock(int fd)
{
    // 设置文件描述符为非阻塞模式
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        ERR_EXIT("fcntl");
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1)
    {
        ERR_EXIT("fcntl");
    }
}

/*
 * 将文件描述符设置为阻塞模式。
 * 参数：
 *   fd: 文件描述符
 * 返回值：
 *   无
 */
void deactivate_nonblock(int fd)
{
    // 设置文件描述符为阻塞模式
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        ERR_EXIT("fcntl");
    }
    flags &= ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1)
    {
        ERR_EXIT("fcntl");
    }
}

/*
 * 判断指定文件描述符是否产生 读 事件，带有超时功能。并不读取数据
 * 参数：
 *   fd: 文件描述符
 *   wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 返回值：
 *   成功(未超时)返回0，失败返回-1，超时返回-1并且 errno = ETIMEDOUT
 */
int read_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if (wait_seconds > 0)
    {
        fd_set read_fdset;
        struct timeval timeout;

        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        int select_ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout);
        if (select_ret == -1)
        {
            return -1;
        }
        else if (select_ret == 0)
        {
            ret = -1;
            /*
            errno被设置为ETIMEDOUT，意味着在超时情况下，
            可以通过检查errno的值来确定超时的原因。
            */
            errno = ETIMEDOUT;
        }
        else
        {
            ret = 0;
        }
    }
    return ret;
}

/*
 * 判断指定文件描述符是否产生 写 事件，带有超时功能。并不写入数据
 * 参数：
 *   fd: 文件描述符
 *   wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 返回值：
 *   成功(未超时)返回 0，失败返回-1，超时返回-1并且 errno = ETIMEDOUT
 */
int write_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if (wait_seconds > 0)
    {
        fd_set write_fdset;
        struct timeval timeout;

        FD_ZERO(&write_fdset);
        FD_SET(fd, &write_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        int select_ret = select(fd + 1, NULL, &write_fdset, NULL, &timeout);
        if (select_ret == -1)
        {
            return -1;
        }
        else if (select_ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else
        {
            ret = 0;
        }
    }
    return ret;
}

/*
 * 在指定时间内等待接受连接请求。
 * 参数：
 *   fd: 监听套接字
 *   addr: 存放对方地址信息的结构体指针
 *   wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 返回值：
 *   成功(未超时)返回已连接套接字，失败返回-1，超时返回-1并且 errno = ETIMEDOUT
 */
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if (wait_seconds > 0)
    {
        fd_set accept_fdset;
        struct timeval timeout;

        FD_ZERO(&accept_fdset);
        FD_SET(fd, &accept_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        int select_ret = select(fd + 1, &accept_fdset, NULL, NULL, &timeout);
        if (select_ret == -1)
        {
            return -1;
        }
        else if (select_ret == 0)
        {
            errno = ETIMEDOUT;
            return -1;
        }
    }
    if (addr != NULL)
    {
        ret = accept(fd, (struct sockaddr *)addr, &addrlen);
    }
    else
    {
        /*
        而如果addr为NULL，则表示你不关心客户端的地址信息，只需要接受连接请求即可。
        在这种情况下，可以简化为调用accept函数时传入NULL：

        这样写的意义是忽略客户端的地址信息，只关注与客户端建立连接的结果。
        在某些情况下，如果你只需要接受连接而不需要客户端的地址信息，可以使用这种简化的方式。
        */
        ret = accept(fd, NULL, NULL);
    }
    return ret;
}

/*
 * 在指定时间内尝试连接到指定地址。
 * 参数：
 *   fd: 套接字
 *   addr: 目标地址信息的结构体指针
 *   wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 返回值：
 *   成功(未超时)返回0，失败返回-1，超时返回-1并且 errno = ETIMEDOUT
 */
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if (wait_seconds > 0)
    {
        activate_nonblock(fd);
    }
    /*
    connect函数的返回值为整型，表示连接的结果。
    如果连接成功建立，则返回0；
    如果连接正在进行中（非阻塞模式下），则返回-1，并将errno设置为EINPROGRESS；
    如果连接失败，则返回-1，并将errno设置为相应的错误代码。
    */
    ret = connect(fd, (struct sockaddr *)addr, addrlen);
    // 如果处于真正进程状态，则尝试在指定时间内重新连接
    if (ret < 0 && errno == EINPROGRESS)
    {
        fd_set connect_fdset;
        struct timeval timeout;

        FD_ZERO(&connect_fdset);
        FD_SET(fd, &connect_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        /*
        对于非阻塞模式下的connect函数，它可能会立即返回，并在后台进行连接的建立过程。
        这是因为在非阻塞模式下，connect函数不会阻塞等待连接建立完成，而是立即返回。
        连接的建立过程会在后台进行，而不会阻塞当前线程。
        */
        int select_ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
        if (select_ret == 0)
        {
            errno = ETIMEDOUT;
            return -1;
        }
        else if (select_ret < 0)
        {
            return -1;
        }
        else if (select_ret == 1)
        {
            int err;
            socklen_t socklen = sizeof(err);
            int sockopt_ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
            if (sockopt_ret == -1)
            {
                return -1;
            }
            if (err == 0)
            {
                ret = 0;
            }
            else
            {
                errno = err;
                ret = -1;
            }
        }
    }
    if (wait_seconds > 0)
    {
        deactivate_nonblock(fd);
    }
    return ret;
}

/*
 * 从指定文件描述符读取指定字节数的数据。
 * 参数：
 *   fd: 文件描述符
 *   buf: 存放读取数据的缓冲区指针
 *   count: 要读取的字节数
 * 返回值：
 *   成功返回实际读取到的字节数，失败返回-1
 */
ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count; // 剩余需要读取的字节数
    ssize_t nread;
    char *bufptr = (char *)buf;

    while (nleft > 0)
    {
        if ((nread = read(fd, bufptr, nleft)) < 0)
        {
            if (errno == EINTR) // 可中断睡眠
            {
                continue;
            }
            return -1;
        }
        else if (nread == 0) // 若对方已关闭,返回已经读到的
        {
            break;
        }
        nleft -= nread;
        bufptr += nread;
    }

    return (count - nleft); // 若已经读到指定的数量，nleft为 0 ，未读到时，返回已经读到的
}

/*
 * 向指定文件描述符写入指定字节数的数据。
 * 参数：
 *   fd: 文件描述符
 *   buf: 存放要写入数据的缓冲区指针
 *   count: 要写入的字节数
 * 返回值：
 *   成功返回实际写入的字节数，失败返回-1
 */
ssize_t writen(int fd, const void *buf, size_t count)
{
    LOGGER(" ");
    size_t nleft = count;
    ssize_t nwritten;
    const char *bufptr = (const char *)buf;

    while (nleft > 0)
    {
        if ((nwritten = write(fd, bufptr, nleft)) < 0)
        {
            LOGGER(" ");
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        else if (nwritten == 0) // 没有数据写入
        {
            LOGGER(" ");
            // continue;
            //
            // return 0; // 没有写入任何字节

            // 根据具体情况处理返回值为0的情况
            if (nleft == count)
                return 0; // 没有写入任何字节，可以视为错误
            else
                break; // 部分数据已写入，终止循环
        }
        LOGGER("nwritten=%zu ", nwritten);
        nleft -= nwritten;
        bufptr += nwritten;
        LOGGER("count=%zu,nleft=%zu", count, nleft);
    }
    LOGGER("count=%zu,nleft=%zu", count, nleft);

    return count - nleft; // 成功写入的字节数
}

/*
 * 探测指定套接字是否有数据可读，但不从套接字中移除数据。
 * 参数：
 *   sockfd: 套接字
 *   buf: 存放探测到的数据的缓冲区指针
 *   len: 缓冲区的大小
 * 返回值：
 *   成功返回探测到的字节数，失败返回-1
 */
ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    while (1)
    {
        int ret = recv(sockfd, buf, len, MSG_PEEK);
        if (ret == -1 && errno == EINTR) // 因为中断导致没有读到数据，重新读
        {
            continue;
        }
        return ret;
    }
}

/*
 * 从指定套接字读取一行数据。
 * 参数：
 *   sockfd: 套接字
 *   buf: 存放读取数据的缓冲区指针
 *   maxline: 缓冲区的最大容量
 * 返回值：
 *   成功返回实际读取到的字节数，失败返回-1
 */
ssize_t readline(int sockfd, void *buf, size_t maxline)
{
    int ret;
    int nread;
    char *bufptr = (char *)buf;
    int nleft = maxline;
    while (1)
    {
        ret = recv_peek(sockfd, bufptr, nleft);
        if (ret < 0)
        {
            return ret;
        }
        else if (ret == 0)
        {
            return ret;
        }
        nread = ret;
        int i;
        for (i = 0; i < nread; ++i)
        {
            if (bufptr[i] == '\n')
            {
                ret = readn(sockfd, bufptr, i + 1);
                if (ret != i + 1)
                {
                    exit(EXIT_FAILURE);
                }
                return ret;
            }
        }
        if (nread > nleft)
        {
            exit(EXIT_FAILURE);
        }
        nleft -= nread;
        ret = readn(sockfd, bufptr, nread);
        if (ret != nread)
        {
            exit(EXIT_FAILURE);
        }
        bufptr += nread;
    }
    return -1;
}

//================================================
/**
 * 初始化客户端环境，并与服务器建立连接
 *
 * @param info: 包含服务器 IP 地址和端口号的结构体
 * @param handle: 用于保存套接字句柄的指针
 *
 * @return 成功返回 0，失败返回 -1
 */
int client_init(IpAndPort info, void **handle)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(info.port);
    server_addr.sin_addr.s_addr = inet_addr(info.ipaddr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect");
        close(sockfd);
        return -1;
    }

    *handle = (void *)(intptr_t)sockfd;
    printf("log::%s, %s:%d connetct success", __FUNCTION__, info.ipaddr, info.port);
    return 0;
}

/**
 * 发送数据到服务器
 *
 * @param handle: 套接字句柄
 * @param data: 要发送的数据的指针
 * @param datalen: 要发送的数据长度
 *
 * @return 成功返回 0，失败返回 -1
 */
int pack_send(void *handle, unsigned char *data, int datalen)
{
    LOGGER("argv=%s,len=%d", data, datalen);

    int sockfd = (intptr_t)handle;
    // 给数据头部加上4字节，用于表示 data 的字节数
    DataPacket pdata;
    pdata.len = htonl(datalen);
    memcpy(pdata.buf, data, datalen);

    // 首先发送 4 + datalen长度到 服务器
    int ret = writen(sockfd, &pdata, 4 + datalen);
    if (ret == -1)
    {
        perror("send");
        return -1;
    }
    LOGGER("ret:%d,datalen=%d", ret, datalen);

    return 0;
}

/**
 * 从服务器接收数据
 *
 * @param handle: 套接字句柄
 * @param out: 用于保存接收数据的缓冲区指针
 * @param outlen: 接收数据缓冲区的大小，同时也用于返回接收到的数据长度
 *
 * @return 成功返回 0，失败返回 -1
 */
int pack_recv(void *handle, unsigned char *out, int *outlen)
{
    LOGGER("start");
    int sockfd = (intptr_t)handle;
    DataPacket pdata;
    int ret;
    int len;
    LOGGER("fd=%d", sockfd);

    // 先读 4 字节，获取对方发送内容的 data 字节数
    ret = readn(sockfd, &pdata.len, 4);
    if (ret == -1)
    {
        printf("sockfd closed\n");
        return -1;
    }
    else if (ret < 4)
    {
        printf("包头校验失败:len = %d\n", ret);
        return -1;
    }

    len = ntohl(pdata.len);
    LOGGER("get 4 head success,head:len=%d", len);

    ret = readn(sockfd, pdata.buf, len);
    if (ret == -1)
    {

        printf("sockfd closed\n");
        return -1;
    }
    else if (ret < len)
    {
        printf("ret < len,发送拆包，丢弃重读\n");
        return -1;
    }
    LOGGER("get data success,head:len=%d,data=%s", len, pdata.buf);
    memcpy(out, pdata.buf, len);
    /*
    outlen 只是一个临时变量，是一个地址符，0x11111
    outlen = &len,只是修改了这个地址的值
    应该修改内存的值
    *outlen 就是那块内存的值 ，将他修改为 len，就对了，
    卡死我了

    */
    *outlen = len;
    LOGGER("return ,outlen=%d,data=%s", *outlen, out);

    return 0;
}
