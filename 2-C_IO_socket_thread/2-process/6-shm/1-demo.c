#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*

#include <sys/ipc.h>
#include <sys/shm.h>

int shmget(key_t key, size_t size, int shmflg);

**参数：**

- `key`：共享内存的键值。可以使用 `IPC_PRIVATE` 创建一个私有的共享内存，也可以使用 `ftok()` 函数生成一个键值。
- `size`：共享内存的大小，以字节为单位。
- `shmflg`：标志位，用于指定共享内存的权限和创建方式。

**返回值：**

- 如果成功创建或获取共享内存，则返回一个非负整数，表示共享内存的标识符（shmid）。
- 如果发生错误，则返回-1，并设置 `errno` 来指示错误的原因。

**功能：**

`shmget` 函数用于创建或获取一个共享内存区域。如果指定的键值对应的共享内存已经存在，则返回该共享内存的标识符；如果指定的键值对应的共享内存不存在，则根据给定的大小创建一个新的共享内存区域。

void *shmat(int shmid, const void *shmaddr, int shmflg);

**参数：**

- `shmid`：共享内存的标识符（由 `shmget` 返回）。
- `shmaddr`：指定共享内存连接到进程地址空间的地址，通常设置为 `NULL`，表示让系统自动选择一个合适的地址。
- `shmflg`：标志位，用于指定共享内存的连接方式。

**返回值：**

- 如果成功连接共享内存，则返回指向共享内存区域的指针。
- 如果发生错误，则返回 `(void *)-1`，并设置 `errno` 来指示错误的原因。

**功能：**

`shmat` 函数用于将共享内存区域连接到进程的地址空间，使进程可以访问共享内存中的数据。

int shmdt(const void *shmaddr);

**参数：**

- `shmaddr`：指向共享内存区域的指针。

**返回值：**

- 如果成功分离共享内存，则返回0。
- 如果发生错误，则返回-1，并设置 `errno` 来指示错误的原因。

**功能：**

`shmdt` 函数用于将共享内存从进程的地址空间中分离，使进程无法再访问共享内存中的数据。

int shmctl(int shmid, int cmd, struct shmid_ds *buf);

**参数：**

- `shmid`：共享内存的标识符（由 `shmget` 返回）。
- `cmd`：控制命令，用于指定要执行的操作。
- `buf`：指向 `struct shmid_ds` 结构的指针，用于传递或接收共享内存的信息。

**返回值：**

- 如果成功执行控制命令，则返回0。
- 如果发生错误，则返回-1，并设置 `errno` 来指示错误的原因。

**功能：**

`shmctl` 函数用于对共享内存进行控制操作，如删除共享内存、获取共享内存的信息等。


*/
/*
struct shmid_ds {
    struct ipc_perm shm_perm;    // 共享内存的权限信息
    size_t shm_segsz;            // 共享内存的大小（字节数）
    time_t shm_atime;            // 上次访问共享内存的时间
    time_t shm_dtime;            // 上次与共享内存断开的时间
    time_t shm_ctime;            // 上次修改共享内存的时间
    pid_t shm_cpid;              // 创建共享内存的进程ID
    pid_t shm_lpid;              // 最后操作共享内存的进程ID
    unsigned short shm_nattch;   // 当前连接到共享内存的进程数量
     扩展字段...
};

*/

typedef struct Student
{
    int age;
    char name[64];
} Student;

int main(int argc, char *argu[])
{
    int ret = 0;
    // ret = shmget(0x1314, sizeof(Student), 0666);
    // ret = shmget(0x1314, sizeof(Student), 0666 | IPC_CREAT | IPC_EXCL);
    ret = shmget(0x1314, sizeof(Student), 0666 | IPC_CREAT);
    if (ret == -1)
    {
        if (errno == ENOENT)
        {
            printf("检查共享内存,发现不存在\n");
        }

        if (errno == IPC_EXCL)
        {
            printf("共享内存已存在\n");
        }
        return 0;
    }

    Student *stup;
    stup = shmat(ret, NULL, 0);

    Student stu = {18, "zhoumo"};
    memcopy(stup, &stu, sizeof(stu));

    return 0;
}