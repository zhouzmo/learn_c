#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define Msg_MAX 1024 * 2
struct msgbuf
{
    long mtype;          // 消息类型
    char mtext[Msg_MAX]; // 消息内容
};

int main(int argc, char *argv[])
{

    int msgid;
    int ret;
    int type;
    char *data;

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <消息类型type 0 1 2 3 .. -1-2-3> <data>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    type = atoi(argv[1]);
    data = argv[2];

    msgid = msgget(0x1234, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        if (errno == ENOENT)
        {
            printf("消息队列不存在\n");
        }

        if (errno == EEXIST)
        {
            printf("消息队列已经存在\n");
        }
        perror("msgget"); // msgget: No such file or directory
        return -1;
    }

    printf("创建消息队列 success，msgid=%d\n", msgid);

    struct msgbuf msgBuf;

    msgBuf.mtype = type;
    strcpy(msgBuf.mtext, data);
    ret = msgsnd(msgid, &msgBuf, strlen(msgBuf.mtext), IPC_NOWAIT);
    if (ret < 0)
    {
        perror("msgsnd");
        return -1;
    }

    return 0;
}


