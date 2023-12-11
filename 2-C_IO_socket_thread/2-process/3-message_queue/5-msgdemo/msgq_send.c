#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

#define MSGMAX 8192

struct msgbuf
{
    long mtype;         // 消息类型，必须大于0
    char mtext[MSGMAX]; // 消息数据
};

/*
    消息队列只会在一台机器上，所以 pid 是唯一的

    思路:
    客户端发给服务器消息类型总是 1
    服务器端回复给客户端的 type 是对方进程号
    相当于服务器端从消息队列中收消息，然后服务器端分类型回复客户端（通过消息队列)

    n个进程通过消息队列进行交换
    有没有产生死锁的可能
    n个客户端向服务器发送消息（本质上是向内核消息队列发送消息〕，
    若消息队列满了﹔服务端回调时。
    会阻塞，造成程序死锁
    即使，非阻塞。。。仍然回阻塞。。
*/
void echo_cli(int msgid)
{
    int n;
    int pid;
    pid = getpid();
    struct msgbuf msg;

    // 消息内容由自己的 pid + 键盘输入组成，即消息头由 pid 构成
    *((int *)msg.mtext) = pid;
    printf("my pid = %d\n", pid);

    while (fgets(msg.mtext + 4, MSGMAX, stdin) != NULL)
    {
        msg.mtype = 1;
        if (msgsnd(msgid, &msg, 4 + strlen(msg.mtext + 4), 0) < 0)
            ERR_EXIT("msgsnd");

        printf("send msg success ,type=%ld,msg=%s\n", msg.mtype, msg.mtext + 4);
        // 前四个字节是自己的pid，不清空
        memset(msg.mtext + 4, 0, MSGMAX - 4);

        if ((n = msgrcv(msgid, &msg, MSGMAX, pid, 0)) < 0)
            ERR_EXIT("msgrcv");

        // fputs(msg.mtext + 4, stdout);
        printf("recv msg,type=%ld,msg=%s\n", msg.mtype, msg.mtext + 4);

        memset(msg.mtext + 4, 0, MSGMAX - 4);
    }
}

int main(int argc, char *argv[])
{
    int msgid;
    msgid = msgget(0x1234, 0);
    if (msgid == -1)
        ERR_EXIT("msgget");

    echo_cli(msgid);

    return 0;
}