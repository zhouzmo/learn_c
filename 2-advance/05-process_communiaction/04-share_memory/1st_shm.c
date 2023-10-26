/**********************************************************************
 * 功能描述： 1.使用IPC_PRIVATE创建共享内存
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    int shmid;
    /*

    用法：`shmget` 函数用于创建一个新的 System V 共享内存段或获取一个已经存在的共享内存段的标识符。

    int shmget(key_t key, size_t size, int shmflg);

    | 参数     | 描述                                                 |
    | -------- | ---------------------------------------------------- |
    | `key`    | 共享内存段的键值，通常使用 `IPC_PRIVATE` 或 `ftok` 函数生成 |
    | `size`   | 共享内存段的大小（以字节为单位）                   |
    | `shmflg` | 标志，用于指定创建共享内存段的权限和选项             |
    | 返回值   | 成功时返回共享内存段的标识符，失败时返回-1。        |

    `shmget` 函数用于创建或获取 System V 共享内存段的标识符。
    共享内存段是一个用于多个进程之间共享数据的内存区域。
    这个函数允许您指定键值（通常使用 `IPC_PRIVATE` 或 `ftok` 函数生成），
    共享内存段的大小，以及一些权限和选项标志。
    如果成功，它将返回共享内存段的标识符，可以用于后续的共享内存操作
    。如果失败，它将返回-1，并设置 `errno` 以指示错误的原因。
    */
    shmid = shmget(IPC_PRIVATE, 128, 0777);
    if (shmid < 0)
    {
        printf("create shared memory fail\n");
        return -1;
    }
    printf("create shared memory sucess, shmid = %d\n", shmid);
    system("ipcs -m");
    return 0;
}

/*
$ gcc 1st_shm.c
$ ./a.out
create shared memory sucess, shmid = 4

------------ 共享内存段 --------------
键        shmid      拥有者  权限     字节     连接数  状态
0x00000000 4          lu         777        128        0



*/