
int socket_init()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        return -1;
    }
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(80);
    saddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    int res = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (res == -1)
    {
        printf("bind err\n");
        return -1;
    }
    res = listen(sockfd, 5);
    if (res == -1)
    {
        return -1;
    }
    return sockfd;
}

void *loop_thread(void *arg)
{
    while (1)
    {
        struct mess m;
        msgrcv(msgid, &m, sizeof(int), 1, 0); // 从消息队列中读取消息
        int c = m.c;
        if (c == sockfd)
        {
            struct sockaddr_in caddr;
            int len = sizeof(caddr);
            int cli = accept(sockfd, (struct sockaddr *)&caddr, &len);
            if (cli < 0)
            {
                continue;
            }
            epoll_add(epfd, cli);
        }
        else
        {
            char buff[1024] = {0};
            int n = recv(c, buff, 1023, 0);
            if (n <= 0)
            {
                epoll_del(epfd, c); // 调用移除描述符函数
                close(c);
                printf("close\n");
                continue;
            }
            char *filename = get_filename(buff); // 调用资源名获取函数
            if (filename == NULL)
            {
                send_404status(c);  // 调用发送错误应答报文函数
                epoll_del(epfd, c); // 调用移除描述符函数
                close(c);
                continue;
            }
            printf("filename:%s\n", filename);

            if (send_httpfile(c, filename) == -1) // 调用发送正确应答报文函数
            {
                printf("主动关闭连接\n");
                epoll_del(epfd, c);
                close(c);
                continue;
            }
        }
        epoll_mod(epfd, c); // 调用重置函数
    }
}

// 添加描述符函数
void epoll_add(int epfd, int fd)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLONESHOT;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        printf("epoll add err\n");
    }
}
// 移除描述符函数
void epoll_del(int epfd, int fd)
{
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
    {
        printf("epoll del err\n");
    }
}
// 重置描述符函数
void epoll_mod(int epfd, int fd)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLONESHOT;
    if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
    {
        printf("epoll mod err\n");
    }
}

char *get_filename(char buff[])
{
    char *ptr = NULL;
    char *s = strtok_r(buff, " ", &ptr);
    if (s == NULL)
    {
        printf("请求报文错误\n");
        return NULL;
    }
    printf("请求方法:%s\n", s);
    s = strtok_r(NULL, " ", &ptr);
    if (s == NULL)
    {
        printf("请求报文 无资源名字\n");
        return NULL;
    }
    if (strcmp(s, "/") == 0)
    {
        return "/index.html";
    }
    return s;
}

int send_httpfile(int c, char *filename)
{
    if (filename == NULL || c < 0)
    {
        send(c, "err", 3, 0);
        return -1;
    }

    char path[128] = {PATH};
    strcat(path, filename); //  /home/ubuntu/ligong/day12/index.hmtl
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        // send(c,"404",3,0);
        send_404status(c);
        return -1;
    }

    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char head_buff[512] = {"HTTP/1.1 200 OK\r\n"};
    strcat(head_buff, "Server: myhttp\r\n");
    sprintf(head_buff + strlen(head_buff), "Content-Length: %d\r\n", size);
    strcat(head_buff, "\r\n"); // 分隔报头和数据 空行
    send(c, head_buff, strlen(head_buff), 0);
    printf("send file:\n%s\n", head_buff);

    int num = 0;
    char data[1024] = {0};
    while ((num = read(fd, data, 1024)) > 0)
    {
        send(c, data, num, 0);
    }
    close(fd);

    return 0;
}

int send_404status(int c)
{
    int fd = open("err404.html", O_RDONLY);
    if (fd == -1)
    {
        send(c, "404", 3, 0);
        return 0;
    }

    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char head_buff[512] = {"HTTP/1.1 404 Not Found\r\n"};
    strcat(head_buff, "Server: myhttp\r\n");
    sprintf(head_buff + strlen(head_buff), "Content-Length: %d\r\n", size);
    strcat(head_buff, "\r\n"); // 分隔报头和数据 空行
    send(c, head_buff, strlen(head_buff), 0);

    char data[1024] = {0};
    int num = 0;
    while ((num = read(fd, data, 1024)) > 0)
    {
        send(c, data, num, 0);
    }
    close(fd);
    return 0;
}

int main()
{
    signal(SIGPIPE, sig_fun);
    sockfd = socket_init(); // 调用创建套接字函数
    if (sockfd == -1)
    {
        exit(0);
    }
    msgid = msgget((key_t)1234, IPC_CREAT | 0600); // 创建消息队列
    if (msgid == -1)
    {
        exit(0);
    }
    pthread_t id[4];
    for (int i = 0; i < 4; i++) // 循环创建线程池
    {
        pthread_create(&id[i], NULL, loop_thread, NULL);
    }
    epfd = epoll_create(MAXFD); // 创建内核事件表
    if (epfd == -1)
    {
        printf("create epoll err\n");
        exit(0);
    }
    epoll_add(epfd, sockfd); // 调用封装的函数添加描述符和事件
    struct epoll_event evs[MAXFD];
    while (1)
    {
        int n = epoll_wait(epfd, evs, MAXFD, -1); // 获取就绪描述符
        if (n == -1)
        {
            continue;
        }
        else
        {
            struct mess m;
            m.type = 1;
            for (int i = 0; i < n; i++)
            {
                m.c = evs[i].data.fd;
                if (evs[i].events & EPOLLIN)
                {
                    msgsnd(msgid, &m, sizeof(int), 0); // 向消息队列发送消息
                }
            }
        }
    }
}
