#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

/*
1. `int msgget(key_t key, int msgflg)`
    - 功能：创建或访问一个消息队列
    - 参数：
        - `key`：消息队列的键值，用于唯一标识消息队列
        - `msgflg`：标志位，用于指定创建或访问消息队列的方式和权限
    - 返回值：成功时返回消息队列的标识符（非负整数），失败时返回-1

2. `int msgctl(int msqid, int cmd, struct msqid_ds *buf)`
    - 功能：控制消息队列的属性
    - 参数：
        - `msqid`：消息队列的标识符
        - `cmd`：控制命令，用于指定要执行的操作
        - `buf`：指向`msqid_ds`结构体的指针，用于传递和获取消息队列的属性
    - 返回值：成功时返回0，失败时返回-1

3. `int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)`
    - 功能：将消息发送到消息队列
    - 参数：
        - `msqid`：消息队列的标识符
        - `msgp`：指向消息数据的指针
        - `msgsz`：消息的大小（字节数）
        - `msgflg`：标志位，用于指定发送消息的方式和行为
    - 返回值：成功时返回0，失败时返回-1

4. `ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)`
    - 功能：从消息队列接收消息
    - 参数：
        - `msqid`：消息队列的标识符
        - `msgp`：指向接收消息的缓冲区的指针
        - `msgsz`：缓冲区的大小（字节数），用于指定接收消息的最大大小
        - `msgtyp`：消息类型，用于指定要接收的消息的类型
        - `msgflg`：标志位，用于指定接收消息的方式和行为
    - 返回值：成功时返回接收到的消息的大小（字节数），失败时返回-1

*/

int main01()
{
    int msgid;
    /*
    表示打开文件，
    ENOENT No message queue exists for key and msgflg did not specify IPC_CREAT.
    IPC_PRIVATE 表示每一次创建的消息队列不一样，即使 msgid 发送给其他无亲属关系进程也不可以使用
    IPC_PRIVATE = 0
    */
    // msgid = msgget(0x1234, 0666);// 打开消息队列，不存在，报错
    // msgid = msgget(0x1234, 0666 | IPC_CREAT);            // 打开，不存在创建，存在就使用
    // msgid = msgget(0x1234, 0666 | IPC_CREAT | IPC_EXCL); // 打开，不存在创建，存在就提示存在，单独使用 IPC_EXCL 没有意义
    msgid = msgget(IPC_PRIVATE, 0666); // 声明私有,只能自己和有亲属关系的进程（子进程）中使用

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
    printf("创建共享内存 success，msgid=%d\n", msgid);

    return 0;
}

int main02()
{
    int msgid;
    // 创建消息队列,低权限
    msgid = msgget(0x1234, 0000 | IPC_CREAT); // 声明私有,只能自己和有亲属关系的进程（子进程）中使用

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

    // 高权限获取
    msgid = msgget(0x1234, 0666);
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

    return 0;
}

int main()
{
    //   main01();
    main02();

    return 0;
}

/*
$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0x00001234 0          book       666        0            0


$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0x00000000 402620448  book       666        0            0
0x00000000 402620449  book       666        0

*/