

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

/*
`ftok` 是一个函数，用于根据给定的文件路径名和项目标识符生成一个唯一的键值(key)。它的原型如下：

key_t ftok(const char *pathname, int proj_id);

`ftok` 函数接受两个参数：`pathname` 和 `proj_id`。

- `pathname` 是一个字符串，表示一个存在的文件的路径名。
通常选择一个已经存在的文件，因为 `ftok` 函数会使用该文件
的索引节点号（inode number）和项目标识符来生成键值。
如果文件不存在，或者路径名不可访问，`ftok` 函数将返回一个无效的键值。

- `proj_id` 是一个整数，用于区分不同项目之间的键值。
通常使用一个字符来表示项目标识符。

`ftok` 函数将根据 `pathname` 和 `proj_id` 生成一个唯一的键值，
并将其作为返回值返回。这个键值可以用于创建或访问共享内存、消息队列等 IPC（进程间通信）机制。

在你的代码中，`ftok("/tmp/", 'a')` 的意思是使用路径名 `"/tmp/"` 和
项目标识符 `'a'` 生成一个键值。这个键值将用于创建或访问消息队列。

*/

#define Msg_MAX 1024 * 2

struct msg_buf
{
    long mtype;          // 消息类型
    char mtext[Msg_MAX]; // 消息内容
};

int main()
{
    key_t key;
    int msgid;
    int ret;

    // 通过路径获取键值 key
    // key = ftok("/tmp/", 'a');
    key = ftok("./temp", 'a');
    printf("key: %d\n", key);
    // 通过文件路径生成 key，通过 key 获取唯一的 msgid
    msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (msgid == -1)
    {
        if (errno == EEXIST)
        {
            printf("EEXIST: - ----1\n");
            key = ftok("./temp", 'a');
            msgid = msgget(key, IPC_CREAT | 0666);
        }
        else
        {
            printf("create error\n");
            perror("msgget: ");
            return -1;
        }
    }
    printf("msgid: %d\n", msgid);
    printf("getpid(): %d\n", getpid());

    struct msg_buf msgbuf;
    int msgtype = getpid();
    msgbuf.mtype = msgtype;
    strcpy(msgbuf.mtext, "hello,world");
    ret = msgsnd(msgid, &msgbuf, sizeof(msgbuf.mtext), IPC_NOWAIT);
    if (ret == -1)
    {
        printf("send message error\n");
        perror("senderr: ");
        return -1;
    }

    sleep(1);

    memset(&msgbuf, 0, sizeof(msgbuf));
    ret = msgrcv(msgid, &msgbuf, sizeof(msgbuf.mtext), msgtype, IPC_NOWAIT);
    if (ret == -1)
    {
        printf("receive message error\n");
        perror("dd: ");
        return -1;
    }

    printf("received message: %s\n", msgbuf.mtext);

    return 0;
}