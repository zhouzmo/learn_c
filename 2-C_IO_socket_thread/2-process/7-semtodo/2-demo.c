#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include "sem_util.h"
/*
实验：当  union semun su.val 得值为 1 和 2，是得实验效果
现象：
val=1 时，程序不支持并发，val=2，程序支持并发
sem get value=1
i = 0,pid=85635
i = 1,pid=85635
i = 0,pid=85636
i = 1,pid=85636


sem get value=2
i = 0,pid=85650
i = 0,pid=85651
i = 1,pid=85651
i = 1,pid=85650

*/
int main(int argc, char *argv[])
{
    int semid;
    int val;
    semid = sem_open(0x1111);
    sem_setval(semid, 1);
    sem_getval(semid, &val);
    printf("sem get value=%d\n", val);

    int pid = fork();

    sem_p(semid);
    {
        int i = 0;
        printf("i = %d,pid=%d\n", i++, getpid());
        sleep(3);
        printf("i = %d,pid=%d\n", i++, getpid());
    }
    sem_v(semid);

    return 0;
}
