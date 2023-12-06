
#include "scr_util.h"

typedef struct SckHandle
{
    int sockfd;
    // int sockArr[100];
    // int arrayNum;
    // int connfd;
    int listenfd;
    int conntime;
    int sendtime;
    int recvtime;
} SckHandle;

/*
 * 从指定文件描述符读取指定字节数的数据。
 * 参数：
 *   fd: 文件描述符
 *   buf: 存放读取数据的缓冲区指针
 *   count: 要读取的字节数
 * 返回值：
 *   成功返回实际读取到的字节数，失败返回-1
 */
static ssize_t readn(int fd, void *buf, size_t count)
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
static ssize_t writen(int fd, const void *buf, size_t count)
{
    LOGGER(" \n");
    size_t nleft = count;
    ssize_t nwritten;
    const char *bufptr = (const char *)buf;

    while (nleft > 0)
    {
        if ((nwritten = write(fd, bufptr, nleft)) < 0)
        {
            LOGGER(" \n");
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        else if (nwritten == 0) // 没有数据写入
        {
            LOGGER(" \n");
            // continue;
            //
            // return 0; // 没有写入任何字节

            // 根据具体情况处理返回值为0的情况
            if (nleft == count)
                return 0; // 没有写入任何字节，可以视为错误
            else
                break; // 部分数据已写入，终止循环
        }
        LOGGER("nwritten=%zu \n", nwritten);
        nleft -= nwritten;
        bufptr += nwritten;
        LOGGER("count=%zu,nleft=%zu\n", count, nleft);
    }
    LOGGER("count=%zu,nleft=%zu\n", count, nleft);

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

static int activate_nonblock(int fd)
{
    int ret = 0;
    // 设置文件描述符为非阻塞模式
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        LOGGER("F_GETFL");
        ret = flags;
        return ret;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1)
    {
        LOGGER("F_SETFL");
        ret = flags;
        return ret;
    }
    return ret;
}

static int deactivate_nonblock(int fd)
{
    // 设置文件描述符为阻塞模式
    int ret = 0;
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        LOGGER("F_GETFL");
        ret = flags;
        return ret;
    }
    flags &= ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1)
    {
        LOGGER("F_SETFL");
        ret = flags;
        return ret;
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
static int write_timeout(int fd, unsigned int wait_seconds)
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
 * 判断指定文件描述符是否产生 读 事件，带有超时功能。并不读取数据
 * 参数：
 *   fd: 文件描述符
 *   wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 返回值：
 *   成功(未超时)返回0，失败返回-1，超时返回-1并且 errno = ETIMEDOUT
 */
static int read_timeout(int fd, unsigned int wait_seconds)
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
 * 在指定时间内尝试连接到指定地址。
 * 参数：
 *   fd: 套接字
 *   addr: 目标地址信息的结构体指针
 *   wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 返回值：
 *   成功(未超时)返回0，失败返回-1，超时返回-1并且 errno = ETIMEDOUT
 */
static int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if (wait_seconds > 0)
    {
        activate_nonblock(fd);
    }
    /*
    tcp/ip 在客户端连接服务器的时候，如果异常
    connect 在 fd 阻塞的情况下，返回时间是 1.5 RTT大约是 75s 以上，会造成软件质量的下降



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
        而不会阻塞当前线程。



        这里通过 select 等待监控指定的时间，通过 getsockopt 判读连接是否建立完成
        select 发现 conn 可读，并不能说明连接可用，
        可以通过 getsockopt 进一步判断
        造成可读的结果
            1、连接建立完成
            2、建立连接失败（失败回写信息，造成可读）
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
            /*
            当 ret 返回值为1时，表示套接字可写。
            这种情况可能有两种情况：一种是连接建立成功，另一种是套接字产生错误。
            在后一种情况下，错误信息不会保存在 errno 变量中，
            因此需要调用 getsockopt 函数来获取错误信息。

            */
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
 * 在指定时间内等待接受连接请求。
 * 参数：
 *   fd: 监听套接字
 *   addr: 存放对方地址信息的结构体指针
 *   wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 返回值：
 *   成功(未超时)返回已连接套接字，失败返回-1，超时返回-1并且 errno = ETIMEDOUT
 */
static int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
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



*/
static int sck_ComSend(int connfd, unsigned char *data, int datalen, int timeout)
{

    int ret = 0;

    ret = write_timeout(connfd, timeout);

    if (ret == 0)
    {
        LOGGER("可写\n");
        int writed = 0;
        unsigned char *netdata = (unsigned char *)malloc(datalen + 4);
        if (netdata == NULL)
        {
            ret = Sck_ErrMalloc;
            LOGGER("err:%d\n", ret);
            return ret;
        };
        memset(netdata, 0, sizeof(netdata + 4));

        int nlen = htonl(datalen);
        memcpy(netdata, &nlen, 4);
        memcpy(netdata + 4, data, datalen);
        LOGGER("写入len(%d)+4的数据:%s\n", datalen, data);
        writed = writen(connfd, netdata, datalen + 4);
        if (writed < datalen + 4)
        {
            LOGGER("writed:%d\n", writed);
            if (netdata != NULL)
            {
                free(netdata);
            }
            return writed;
        }
        LOGGER("成功写入len(%d)+4的数据:%s\n", datalen, netdata);
    }
    else if (ret < 0)
    {
        if (ret == -1 && errno == ETIMEDOUT)
        {
            ret = Sck_ErrTimeout;
            LOGGER("ret:%d\n", ret);
            return ret;
        }
        else
        {
            LOGGER("ret:%d\n", ret);
        }
    }
    LOGGER("sck_send end ret=%d\n", ret);
    return ret;
}
/*



*/
static int sckCom_recv(int connfd, unsigned char *out, int *outlen, int timeout)
{
    int ret;
    ret = read_timeout(connfd, timeout);
    LOGGER("start read_timeout end ret=%d\n", ret);
    if (ret == 0)
    {
        int nlen;
        LOGGER("先读 4 字节，获取包长度\n");
        ret = readn(connfd, &nlen, 4);
        if (ret == 0)
        {
            LOGGER("sockfd closed\n");
            return -1;
        }
        else if (ret < 4)
        {

            LOGGER("包头校验失败:ret = %d\n", ret);
            return ret;
        }

        int len = ntohl(nlen);
        LOGGER("读包头len=%d\n", len);

        unsigned char *netdata = (unsigned char *)malloc(len);
        if (netdata == NULL)
        {
            ret = Sck_ErrMalloc;
            LOGGER("err:%d\n", ret);
            return ret;
        }
        memset(netdata, 0, sizeof(netdata));

        int readd;
        readd = readn(connfd, netdata, len);

        if (readd < len)
        {
            LOGGER("readd:%d\n", readd);
            if (netdata != NULL)
            {
                free(netdata);
            }

            return readd;
        }

        // 这里 如果通过
        memcpy(out, netdata, len);
        *outlen = len;
        ret = 0;
        LOGGER("成功读取数据:size(%d)%s\n", len, netdata);
        LOGGER("成功读取数据:size(%d)%s\n", *outlen, out);
        return ret;
    }
    else if (ret < 0)
    {
        if (ret == -1 && errno == ETIMEDOUT)
        {
            ret = Sck_ErrTimeout;
            LOGGER("time out ret:%d\n", ret);
            return ret;
        }
        else
        {
            LOGGER("ret:%d\n", ret);
            return ret;
        }
    }
}
/*



*/
int sckClient_init(void **handle, int conntime, int sendtime, int recvtime, int nConnNum)
{
    int ret = 0;
    if (conntime < 0 || sendtime < 0 || recvtime < 0 || nConnNum < 0)
    {
        ret = Sck_ErrParam;
        printf("err: check( || contime < 0 || sendtime < 0 || revtime < 0 %d\n", Sck_ErrParam);
        return ret;
    }

    SckHandle *tmp = (SckHandle *)malloc(sizeof(SckHandle));
    if (tmp == NULL)
    {
        ret = Sck_ErrMalloc;
        printf("err: malloc%d\n", Sck_ErrMalloc);
        return ret;
    }

    // 初始化tmp的成员变量
    tmp->conntime = conntime;
    tmp->sendtime = sendtime;
    tmp->recvtime = recvtime;
    // tmp->arrayNum = nConnNum;
    // int i;
    // for (i = 0; i < nConnNum; i++)
    // {

    //     int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //     if (sockfd < 0)
    //     {
    //         ret = errno;
    //         printf("err:socket %d\n", ret);
    //     }
    //     tmp->sockArr[i] = sockfd;
    // }
    *handle = tmp;
    return ret;
}
/*



*/
int sckClient_ConnectConn(void *handle, char *ip, int port, int *connfd)
{
    int ret = 0;
    if (ip == NULL || connfd == NULL || port < 0 || port > 65535)
    {
        ret = Sck_ErrParam;
        printf("err: check(ip == NULL || connfd == NULL || port < 0 || port > 65535 %d\n", Sck_ErrParam);
        return ret;
    }

    SckHandle *tmp = NULL;
    tmp = (SckHandle *)handle;
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        ret = errno;
        printf("err:socket %d\n", ret);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    ret = connect_timeout(sockfd, (struct sockaddr_in *)&server_addr, tmp->conntime);

    if (ret < 0)
    {
        if (ret == -1 && errno == ETIMEDOUT)
        {
            ret = Sck_ErrTimeout;
            return ret;
        }
        else
        {
            LOGGER(" connect_timeout%d\n", ret);
            return ret;
        }
    }

    *connfd = sockfd;
    return ret;
}
/*


 */
/*


 */
int sckServer_init(int *listenfd, int port)
{
    int ret;

    if (port <= 0 || port > 65532)
    {
        ret = Sck_ErrParam;
        printf("error:port <= 0 || port > 65532 ret= %d\n", Sck_ErrParam);
        return ret;
    }

    LOGGER(" socket bind listen\n");
    int mylistenfd;

    if ((mylistenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOGGER("socket:%d", mylistenfd);
        return mylistenfd;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    ret = setsockopt(mylistenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (ret < 0)
    {
        LOGGER("setsockopt:%d", ret);
        return ret;
    }

    ret = bind(mylistenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0)
    {
        LOGGER("bind:%d", ret);
        return ret;
    }
    ret = listen(mylistenfd, SOMAXCONN);
    if (ret < 0)
    {
        LOGGER("listen:%d", ret);
        return ret;
    }
    LOGGER(" socket bind listen success\n");

    *listenfd = mylistenfd;
    LOGGER(" sckServer_init 初始化  success，listenfd = %d\n", *listenfd);
    return ret;
}

/*


 */
int sckServer_accept(int listenfd, int *connfd, int timeout)
{
    int ret = 0;

    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    intptr_t myConn;
    LOGGER(" start  accept_timeout\n");
    myConn = accept_timeout(listenfd, &peeraddr, timeout);

    if (myConn < 0)
    {
        if (myConn == -1 && errno == ETIMEDOUT)
        {
            ret = Sck_ErrTimeout;
            return ret;
        }
        else
        {
            LOGGER(" accept_timeout%zu\n", myConn);
            return myConn;
        }
    }
    unsigned short client_prot = ntohs(peeraddr.sin_port);
    char *ipaddr = inet_ntoa(peeraddr.sin_addr);
    LOGGER("accept client  success ip=%s port=%d\n", ipaddr, client_prot);

    *connfd = myConn;
    return ret;
}

/*


 */
int sckClient_send(void *handle, int connfd, unsigned char *data, int datalen)
{
    int ret = 0;

    if (handle == NULL || data == NULL || datalen <= 0)
    {
        ret = Sck_ErrParam;
        LOGGER("handle == NULL || data == NULL || datalen <= 0s %d\n", ret);
        return ret;
    }
    SckHandle *tmp = NULL;
    tmp = (SckHandle *)handle;
    return sck_ComSend(connfd, data, datalen, tmp->sendtime);
}
/**
 * 从指定连接中接收数据
 * @param handle 连接句柄
 * @param connfd 连接描述符
 * @param out 接收数据的缓冲区
 * @param outlen 接收数据的缓冲区长度，函数执行后将存储实际接收到的数据长度
 * @return 返回操作结果，0表示成功，其他值表示出错
 */
int sckClient_recv(void *handle, int connfd, unsigned char *out, int *outlen)
{
    LOGGER("start\n");
    int ret = 0;
    if (handle == NULL || out == NULL || outlen == NULL)
    {
        ret = Sck_ErrParam;
        LOGGER("handle == NULL || out == NULL || outlen == NULL %d\n", ret);
        return ret;
    }
    SckHandle *tmp = NULL;
    tmp = (SckHandle *)handle;
    LOGGER("read_timeout start \n");
    ret = sckCom_recv(connfd, out, outlen, tmp->recvtime);
    return ret;
}
/*


 */
int sckServer_send(int connfd, unsigned char *data, int datalen, int timeout)
{
    return sck_ComSend(connfd, data, datalen, timeout);
}
/*


 */
int sckServer_recv(int connfd, unsigned char *out, int *outlen, int timeout)
{
    return sckCom_recv(connfd, out, outlen, timeout);
}

/*


 */
int sck_close(int connfd)
{
    int ret = 0;
    if (connfd < 0)
    {
        ret = Sck_ErrParam;
        LOGGER("connfd < 0 %d\n", ret);
        return ret;
    }

    ret = close(connfd);
    if (ret < 0)
    {
        ret = errno;
        LOGGER("close error %d", errno);
    }
    return ret;
}
/*


 */
int sck_destory(void *handle, int connfd)
{

    LOGGER("sck_destory %d\n", connfd);
    int ret = 0;
    if (handle != NULL)
    {
        free(handle);
    }
    LOGGER("free %d\n", connfd);

    // ret = shutdown(connfd, SHUT_RDWR);
    // if (ret < 0)
    // {
    //     ret = errno;
    //     LOGGER("shutdown error %d\n", errno);
    // }
    // LOGGER(" shutdown  %d\n", connfd);
    // ret = sck_close(connfd);
    // if (ret < 0)
    // {
    //     LOGGER("sck_close < 0 %d\n", ret);
    //     return ret;
    // }
    // LOGGER("close  %d\n", connfd);
    LOGGER("sck_destory success %d\n", ret);
    return ret;
}
