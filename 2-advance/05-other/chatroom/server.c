#include "LinkList.h"

int max_fd = 0;
int acceptfd = 0;
int ret = 0;
int i = 0;
int nbytes = 0;
char buff[N] = { 0 };
int loop = 0;
char send_buf[128] = { 0 };
//创建要监视的文件描述集合
fd_set readfds; //母本
fd_set readfds_temp; //给select擦除用的

void* sendThread(void* arg)
{
    char sys_send_buf[256] = { 0 };
    while (1) {
        memset(send_buf, 0, sizeof(send_buf));
        scanf("%s", send_buf);
        /*for (loop = 4; loop < max_fd + 1 && ret != 0; loop++)
            这样写是不对的，因为，和主线程共享ret，主线程中的ret每次结束都是会减到0的，
            所以如果这么写，一次循环也不会进入！！！！
        */
        memset(sys_send_buf, 0, sizeof(sys_send_buf));
        sprintf(sys_send_buf, "[系统消息]:%s\n", send_buf);
        printf("%s\n", sys_send_buf);

        for (loop = 4; loop < max_fd + 1; loop++) {
            if (FD_ISSET(loop, &readfds)) {
                if (-1 == send(loop, sys_send_buf, N, 0)) {
                    ERRLOG("send error");
                }
            }
        }
    }
}

int main(int argc, const char* argv[])
{
    //参数合理性检查
    if (3 != argc) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(-1);
    }

    //创建套接字 流式套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        ERRLOG("socket error");
    }

    //填充网络信息结构体
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    serveraddr.sin_port = htons(atoi(argv[2]));

    socklen_t serveraddr_len = sizeof(serveraddr);

    //绑定
    if (-1 == bind(sockfd, (struct sockaddr*)&serveraddr, serveraddr_len)) {
        ERRLOG("bind error");
    }

    //将套接字设置成被动监听状态
    if (-1 == listen(sockfd, 5)) {
        ERRLOG("listen error");
    }

    //清空集合
    FD_ZERO(&readfds);
    FD_ZERO(&readfds_temp);
    //将sockfd添加到集合中
    FD_SET(sockfd, &readfds);

    //更新最大文件描述符
    max_fd = max_fd > sockfd ? max_fd : sockfd;

    socklen_t clientaddr_len = sizeof(struct sockaddr_in);
    usr_info_t* usr_info_head = LinkListNodeCreate();
    usr_info_t* usr_info_node;
    usr_info_t* usr = NULL;

    pthread_t tid;
    int err_code;
    if (0 != (err_code = pthread_create(&tid, NULL, sendThread, NULL))) {
        printf("pthread_create error %s\n", strerror(err_code));
        exit(-1);
    }

    while (1) {
        readfds_temp = readfds;

        if (-1 == (ret = select(max_fd + 1, &readfds_temp, NULL, NULL, NULL))) {
            ERRLOG("select error");
        }
        //遍历集合 看哪个文件描述符就绪了
        for (i = 3; i < max_fd + 1 && ret != 0; i++) {
            if (FD_ISSET(i, &readfds_temp)) {
                if (sockfd == i) {
                    //说明有新客户端连接了,创建一个新节点保存客户信息
                    usr_info_node = LinkListNodeCreate();
                    if (-1 == (acceptfd = accept(sockfd, (struct sockaddr*)&usr_info_node->clientaddr, &clientaddr_len))) {
                        ERRLOG("accept error");
                    }
                    usr_info_node->acceptfd = acceptfd;
                    LinkListInsertHead(usr_info_head, usr_info_node); //用户信息入队

                    printf("客户端[%d]号连接到服务器..\n", usr_info_node->acceptfd);
                    printf("客户端[%d]已连接\n", ntohs(usr_info_node->clientaddr.sin_port));
                    memset(send_buf, 0, sizeof(send_buf));

                    //将新客户端的acceptfd加入到集合
                    FD_SET(acceptfd, &readfds);
                    //更新最大文件描述符
                    max_fd = max_fd > acceptfd ? max_fd : acceptfd;
                } else {
                    //说明有客户端发来数据了
                    usr = LinkListSearchUsrByAcceptfd(usr_info_head, i); //先根据acceptfd找一下用户信息

                    if (-1 == (nbytes = recv(i, buff, N, 0))) {
                        ERRLOG("recv error");
                    } else if (0 == nbytes) {
                        printf("客户端[%d]断开连接..\n", i);
                        memset(send_buf, 0, sizeof(send_buf));
                        sprintf(send_buf, "用户[%s]断开连接..", usr->name);
                        for (loop = 4; loop < max_fd + 1 && ret != 0; loop++) {
                            if (FD_ISSET(loop, &readfds) && loop != i) {
                                if (-1 == send(loop, send_buf, N, 0)) {
                                    ERRLOG("send error");
                                }
                            }
                        }
                        //将该客户端的文件描述符在集合中删除
                        FD_CLR(i, &readfds);
                        close(i);
                        continue;
                    }
                    if (!strncmp(buff, "quit", 4)) {
                        printf("客户端[%d]退出了..\n", i);
                        memset(send_buf, 0, sizeof(send_buf));
                        sprintf(send_buf, "用户[%s]退出了..", usr->name);
                        for (loop = 4; loop < max_fd + 1 && ret != 0; loop++) {
                            if (FD_ISSET(loop, &readfds) && loop != i) {
                                if (-1 == send(loop, send_buf, N, 0)) {
                                    ERRLOG("send error");
                                }
                            }
                        }
                        //将该客户端的文件描述符在集合中删除
                        FD_CLR(i, &readfds);
                        close(i);
                        continue;
                    }

                    //第一次发过来的是用户的名字，所以，应该保存一下
                    if (usr->named != 9) {
                        strcpy(usr->name, buff);
                        usr->named = 9;
                        continue;
                    }

                    printf("客户端[%d]发来数据[%s]..\n", i, buff);
                    //组装应答
                    if (usr->sayHi == 7) {
                        memset(send_buf, 0, sizeof(send_buf));
                        sprintf(send_buf, "用户[%s]:%s\n", usr->name, buff);
                    } else { //只有第一次才会对所有已经加入群聊的用户显示**加入群聊
                        memset(send_buf, 0, sizeof(send_buf));
                        sprintf(send_buf, "用户[%s]加入群聊..\n", usr->name);
                        usr->sayHi = 7;
                    }
                    //如果用户的acceptfd还在readfds里，就接收本次用户发送的消息.注意不能从3开始，3是sockfd，给sockfd发消息，会出错！！！！
                    for (loop = 4; loop < max_fd + 1 && ret != 0; loop++) {
                        if (FD_ISSET(loop, &readfds) && loop != i) {
                            if (-1 == send(loop, send_buf, N, 0)) {
                                ERRLOG("send error");
                            }
                        }
                    }
                    ret--;
                }
            }
        }
    }
    close(sockfd);

    return 0;
}
