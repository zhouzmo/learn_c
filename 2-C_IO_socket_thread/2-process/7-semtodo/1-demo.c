#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <stdlib.h>
/*
P操作和V操作是不可中断的程序段,称为原语.
PV原语及信号量的概念都是由荷兰科学家E.W.Dijkstra提出的.
信号量sem是一整数,
sem大于等于零时代表可供并发进程使用的资源实体数
,但sem小于零时则表示正在等待使用临界区的进程数.
　　 P原语操作的动作是:
　　(1) sem减1;
　　(2) 若sem减1后仍大于或等于零,则进程继续执行;
　　(3) 若sem减1后小于零,则该进程被阻塞后进入与该信号相对应的队列中,然后转进程调度.
　　 V原语操作的动作是:
　　(1) sem加1;
　　(2) 若相加结果大于零,则进程继续执行;
　　(3) 若相加结果小于或等于零,则从该信号的等待队列中唤醒一等待进程,然后再返回原进程继续执行或转进程调度.
　　 PV操作对于每一个进程来说,都只能进行一次,而且必须成对使用.在PV原语执行期间不允许有中断的发生.

我的理解：
sem 表示资源总数
P 操作 sem-1 ，对资源总数进行抢占，没有抢到就进入等待队列
V 操作 sem+1 ，对资源总数进行释放，

 sem >=0 表示进程还有资源，可以执行
 sem <=0 表示进程


*/
/*

### `semget` 函数
int semget(key_t key, int nsems, int semflg);

**功能：**
`semget` 函数用于创建一个新的信号量集或访问一个已存在的信号量集。

**参数：**
- `key`：信号量集的标识符。
- `nsems`：信号量集中的信号量数量。
- `semflg`：权限标志，用于指定信号量集的权限和操作方式。

**返回值：**
- 如果成功，返回一个非负整数，表示信号量集的标识符。
- 如果失败，返回-1，并设置 `errno` 来指示错误的原因。

**注意事项：**
- `key` 参数用于唯一标识一个信号量集，可以使用 `ftok` 函数生成一个唯一的 `key` 值。
- `semflg` 参数用于指定信号量集的权限和操作方式，可以使用位掩码来设置多个选项。
- 如果 `semget` 函数用于访问已存在的信号量集，只需要提供正确的 `key` 和 `semflg` 参数即可。
- 如果 `semget` 函数用于创建新的信号量集，需要提供唯一的 `key` 值，并指定 `nsems` 参数来设置信号量的数量。


### `semctl` 函数
int semctl(int semid, int semnum, int cmd, ...);

**功能：**
`semctl` 函数用于对信号量集进行控制操作，如获取或设置信号量集的属性。

**参数：**
- `semid`：信号量集的标识符。
- `semnum`：要操作的信号量的索引。
- `cmd`：要执行的操作命令。
- 可选的其他参数用于传递额外的参数给特定的命令。

**返回值：**
- 如果成功，返回一个非负整数，表示操作的结果或获取的属性值。
- 如果失败，返回-1，并设置 `errno` 来指示错误的原因。

**注意事项：**
- `semid` 参数用于指定要操作的信号量集。
- `semnum` 参数用于指定要操作的具体信号量的索引。
- `cmd` 参数用于指定要执行的操作命令，可以是获取属性、设置属性或执行其他控制操作。
- 可选的其他参数用于传递额外的参数给特定的命令，具体参数的类型和含义取决于命令的要求。


### `semop` 函数
int semop(int semid, struct sembuf *sops, unsigned nsops);

**功能：**
`semop` 函数用于对信号量集进行原子操作。

**参数：**
- `semid`：信号量集的标识符。
- `sops`：一个指向 `struct sembuf` 结构体数组的指针，每个结构体描述了一个操作。
- `nsops`：`sops` 数组中结构体的数量。

**返回值：**
- 如果成功，返回0。
- 如果失败，返回-1，并设置 `errno` 来指示错误的原因。

**注意事项：**
- `semid` 参数用于指定要操作的信号量集。
- `sops` 参数是一个指向 `struct sembuf` 结构体数组的指针，每个结构体描述了一个操作，包括信号量索引、操作类型和操作值。
- `nsops` 参数指定了 `sops` 数组中结构体的数量，即要执行的操作的数量。
- `semop` 函数会按照指定的操作顺序对信号量集进行原子操作，确保操作的原子性和顺序性。

*/

/*
struct semid_ds {
    struct ipc_perm sem_perm;  // 所有权和权限
    time_t sem_otime;          // 上次 semop 操作时间
    time_t sem_ctime;          // 上次修改时间
    unsigned short sem_nsems;  // 信号量集中的信号量数量
};



    unsigned short sem_num;   semaphore number
    short          sem_op;    semaphore operation
    short          sem_flg;   operation flags



*/
#define ERR_EXIT(msg) \
    do                \
    {                 \
        perror(msg);  \
        exit(1);      \
    } while (0)

int sem_p(int semid)
{
    struct sembuf sb = {0, -1, 0}; // P操作，信号量值减1
    int ret = semop(semid, &sb, 1);
    if (ret == -1)
    {
        ERR_EXIT("semop");
    }
    return ret;
}

int sem_v(int semid)
{
    struct sembuf sb = {0, 1, 0}; // V操作，信号量值加1
    int ret = semop(semid, &sb, 1);
    if (ret == -1)
    {
        ERR_EXIT("semop");
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int semid;
    semid = semget(0x3234, 1, IPC_CREAT | 0666); // 创建信号量集

    // 检查信号量创建是否成功
    if (semid == -1)
    {
        ERR_EXIT("semget");
    }
    int pid = fork();
    pid = fork();
    printf("semid: %d\n", semid); // 打印信号量ID

    // 对信号量进行P操作（等待）
    if (pid == 0)
    {
        sem_p(semid);
        printf("pid=%d,semid=%d\n", getpid(), semid); // 打印信号量ID
        // 休眠5秒钟，模拟临界区的执行
        sleep(5);

        // 对信号量进行V操作（释放）
        sem_v(semid);
    }

    while (1)
    {
        pause();
    }

    return 0;
}