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

void echo_srv(int msgid)
{
    int n;
    struct msgbuf msg;

    while (1)
    {
        memset(&msg, 0, sizeof(msg));
        printf("wait msg....\n");

        if ((n = msgrcv(msgid, &msg, MSGMAX, 1, 0)) < 0)
            ERR_EXIT("msgrcv");

        int pid;
        pid = *((int *)msg.mtext);
        printf("recv msg,type=%ld,pid = %d,msg=%s\n", msg.mtype, pid, msg.mtext + 4);
        // fputs(msg.mtext + 4, stdout);
        msg.mtype = pid;
        msgsnd(msgid, &msg, n, 0);
        printf("send msg success ,type=%ld,msg=%s\n", msg.mtype, msg.mtext + 4);
    }
}

int main(int argc, char *argv[])
{
    int msgid;
    msgid = msgget(0x1234, IPC_CREAT | 0666);
    if (msgid == -1)
        ERR_EXIT("msgget");

    echo_srv(msgid);

    return 0;
}