#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include "sem_util.h"



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
