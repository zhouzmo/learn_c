/*
消息队列
消息队列提供了一种从一个进程向另一个进程发送数据块的方式。
每个数据块都被赋予一个类型，接收进程可以接收具有不同类型的数据块。
消息队列与管道类似，但是每个消息的最大长度是有上限的（MSGMAX），
而且每个消息队列的总字节数也有上限（MSGMNB）。
系统上消息队列的总数也有一个上限（MSGMNI）。

与管道相比：

- 管道是一种流管道，而消息队列是有边界的，每个消息是一个独立的数据块。
- 消息队列遵循先进先出的原则，而管道没有固定的顺序。
- 消息队列允许后进入的消息先出来，而管道只能按照先进先出的顺序读取。

消息队列的大小受到三个限制：

- 最大消息长度限制：可以通过查看`/proc/sys/kernel/msgmax`文件来获取最大消息长度的限制。
- 消息队列总字节数限制：可以通过查看`/proc/sys/kernel/msgmnb`文件来获取消息队列总字节数的限制。
- 消息条目数限制：可以通过查看`/proc/sys/kernel/msgmni`文件来获取消息条目数的限制。



$ cat /proc/sys/kernel/msgmax
8192 =
$ cat /proc/sys/kernel/msgmnb
16384
$ cat /proc/sys/kernel/msgmni
32000
最大消息长度限制（msgmax）：8192字节
消息队列总字节数限制（msgmnb）：16384字节
消息条目数限制（msgmni）：32000条




**IPC对象数据结构**
下面是使用代码和注释方式体现IPC对象数据结构的示例：
*/
struct ipc_perm
{
    key_t key;
    /* 供 xxxget(2) 使用的键值 */
    uid_t uid;
    /* 所有者的有效用户ID */
    gid_t gid;
    /* 所有者的有效组ID */
    uid_t cuid;
    /* 创建者的有效用户ID */
    gid_t cgid;
    /* 创建者的有效组ID */
    unsigned short mode;
    /* 权限 */
    unsigned short _seq;
    /* 序列号 */
};

struct msqid64_ds
{
    struct ipc64_perm msg_perm;
    /* 所有权和权限 */
    time_t msg_stime;
    /* 最后一次调用 msgsnd(2) 的时间 */
    time_t msg_rtime;
    /* 最后一次调用 msgrcv(2) 的时间 */
    time_t msg_ctime;
    /* 最后一次修改的时间 */
    unsigned long msg_cbytes;
    /* 队列中当前的字节数（非标准字段） */
    msgqnum_t msg_qnum;
    /* 队列中当前的消息数量 */
    msglen_t msg_qbytes;
    /* 队列的最大字节数 */
    pid_t msg_lspid;
    /* 最后发送消息的进程ID */
    pid_t msg_lrpid;
    /* 最后接收消息的进程ID */
};
