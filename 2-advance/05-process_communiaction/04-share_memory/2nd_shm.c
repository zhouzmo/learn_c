/**********************************************************************
 * 功能描述： 1.使用ftok函数生成的key创建共享内存
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人          修改内容
 * -----------------------------------------------
 * 2020/05/16       V1.0      zh(ryan)        创建
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
    int key;

    key = ftok("./b.out", 'a');  //先创建一个key值
    if (key < 0) {
        printf("create key fail\n");
        return -1;
    }
    printf("create key sucess key = 0x%X\n",key);
    
    shmid = shmget(key, 128, IPC_CREAT | 0777);
    if (shmid < 0) {
        printf("create shared memory fail\n");
        return -1;
    }
    printf("create shared memory sucess, shmid = %d\n", shmid);
    system("ipcs -m");
    return 0;
}

/*
ory$ echo "hello" > b.out
$ cat b.out
hello

$ gcc 2nd_shm.c
$ ./a.out
create key sucess key = 0x61032405
create shared memory sucess, shmid = 5

------------ 共享内存段 --------------
键        shmid      拥有者  权限     字节     连接数  状态
0x00000000 4          lu         777        128        0
0x61032405 5          lu         777        128        0


*/