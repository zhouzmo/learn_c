#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
/*
`msgctl`函数是用于控制消息队列的函数。以下是该函数的原型：

int msgctl(int msqid, int cmd, struct msqid_ds *buf);

参数：
- `msqid`：由`msgget`函数返回的消息队列标识符。
- `cmd`：将要采取的动作，有三个可取值。
- `buf`：指向`msqid_ds`结构的指针，用于传递消息队列的信息。

返回值：
- 成功时返回0。
- 失败时返回-1。

`cmd`参数可取的三个值及其说明如下：
- `IPC_STAT`：将消息队列的当前关联值设置为`msqid_ds`结构中的数据。
- `IPC_SET`：在进程具有足够权限的前提下，将消息队列的当前关联值设置为`msqid_ds`数据结构中给出的值。
- `IPC_RMID`：删除消息队列。

通过使用`msgctl`函数，你可以获取消息队列的信息、修改消息队列的属性或删除消息队列。
*/

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
    // 创建消息队列
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
        perror("msgget");
        return -1;
    }
    printf("创建消息队列 success，msgid=%d\n", msgid);

    struct msqid_ds buf;
    printMsgQInfo(msgid, &buf);

    /*
        设置消息
    */
    buf.msg_perm.mode = 0644;
    ret = msgctl(msgid, IPC_SET, &buf);
    if (ret == -1)
    {
        perror("msgget");
        return -1;
    }

    printf("权限信息：%o\n", buf.msg_perm.mode);

    /*
        删除消息
    */
    buf.msg_perm.mode = 0644;
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

    return 0;
}

/*


*/