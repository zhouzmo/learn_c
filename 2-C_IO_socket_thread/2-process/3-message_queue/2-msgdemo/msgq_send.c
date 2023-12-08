#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/msg.h>

/*

int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);

参数：
- `msqid`：由`msgget`函数返回的消息队列标识符。
- `msgp`：指向要发送的消息的指针。
- `msgsz`：要发送的消息的大小，不包括消息类型字段的大小。
- `msgflg`：控制在消息队列满或达到系统上限时的行为。

返回值：
- 成功发送消息时，返回0。
- 失败时，返回-1，并设置`errno`来指示错误类型。

`msgsnd`函数用于将一条消息添加到消息队列中。
它将指定的消息（通过`msgp`参数指向的指针）发送到由`msqid`参数指定的消息队列中。

消息结构体的定义通常如下所示：

struct msgbuf {
    long mtype;  // 消息类型
    char mtext[1];  // 消息内容
};

在实际使用时，可以根据需要调整`mtext`数组的大小。
`msgflg`参数控制在消息队列满或达到系统上限时的行为。
例如，可以使用`IPC_NOWAIT`标志来指示非阻塞发送，即如果消息队列已满，则函数立即返回错误。






ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);

**参数：**
- `msqid`：由`msgget`函数返回的消息队列标识码。
- `msgp`：指向准备接收消息的缓冲区的指针。
- `msgsz`：`msgp`指向的消息缓冲区的长度，不包括保存消息类型的`long int`长度。
- `msgtyp`：消息类型，用于实现接收优先级的简单形式。
- `msgflg`：控制当队列中没有相应类型的消息可供接收时的行为。

**返回值：**
- 成功时，返回实际接收到的消息的字节数。
- 失败时，返回-1，并设置`errno`以指示错误类型。

**消息类型`msgtyp`的取值：**
- 当`msgtyp`为0时，返回队列中的第一条消息。
- 当`msgtyp`大于0时，返回队列中第一条类型等于`msgtyp`的消息。
- 当`msgtyp`小于0时，返回队列中第一条类型小于等于`msgtyp`绝对值的消息，并且是满足条件的消息类型中最小的消息。

**`msgflg`的取值：**
- 当`msgflg`为`IPC_NOWAIT`时，如果队列中没有可读的消息，不等待并立即返回，返回错误码`ENOMSG`。
- 当`msgflg`为`MSG_NOERROR`时，如果消息大小超过`msgsz`，消息将被截断。
- 当`msgtyp`大于0且`msgflg`为`MSG_EXCEPT`时，接收第一条类型不等于`msgtyp`的消息。



*/
#define Msg_MAX 1024 * 2
struct msgbuf
{
    long mtype;          // 消息类型
    char mtext[Msg_MAX]; // 消息内容
};

int printMsgQInfo(int msgid, struct msqid_ds *bufp)
{
    int ret;
    struct msqid_ds buf;
    if (bufp == NULL)
    {
        memset(&buf, 0, sizeof(struct msqid_ds));
    }
    else
    {
        buf = *bufp;
    }

    /*
    获取消息队列结构体
   */
    ret = msgctl(msgid, IPC_STAT, &buf);
    if (ret == -1)
    {
        perror("msgget");
    }

    printf("权限信息：%o\n", buf.msg_perm.mode);
    printf("当前消息队列中的字节数：%ld\n", buf.msg_cbytes);
    printf("队列中当前的消息数量:%ld\n", buf.msg_qnum);
    return ret;
}

int printMsgQDelete(int msgid)
{
    int ret;
    ret = msgctl(msgid, IPC_RMID, NULL);
    if (ret == -1)
    {
        perror("msgget");
        return -1;
    }
    else
    {
        printf("msgid(%d) IPC_RMID 删除成功\n", msgid);
    }
    return ret;
}

int main()
{
    int msgid;
    int ret;
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
    char getbuf[1024] = {0};
    while (fgets(getbuf, sizeof(getbuf), stdin))
    {

        if (!strcmp("info\n", getbuf))
        {
            struct msqid_ds msqbuf;
            printMsgQInfo(msgid, &msqbuf);
            memset(&msgBuf, 0, sizeof(struct msgbuf));
            continue;
        }

        if (!strcmp("delete\n", getbuf))
        {
            printMsgQDelete(msgid);
            break;
        }

        msgBuf.mtype = 1;
        strcpy(msgBuf.mtext, getbuf);
        ret = msgsnd(msgid, &msgBuf, strlen(getbuf), IPC_NOWAIT);
        if (ret < 0)
        {
            perror("msgsnd");
            return -1;
        }
    }

    return 0;
}
