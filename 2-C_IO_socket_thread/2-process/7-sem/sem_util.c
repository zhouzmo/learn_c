#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun
{
    int val;               /* SETVAL 操作的值 */
    struct semid_ds *buf;  /* IPC_STAT 和 IPC_SET 操作的缓冲区 */
    unsigned short *array; /* GETALL 和 SETALL 操作的数组 */
    struct seminfo *__buf; /* IPC_INFO 操作的缓冲区（特定于 Linux） */
};

void sem_p(int semid)
{
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = -1; /* P 操作 */
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}

void sem_v(int semid)
{
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = 1; /* V 操作 */
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}

void sem_delete(int semid)
{
    semctl(semid, 0, IPC_RMID);
}

void sem_setval(int semid, int value)
{
    union semun sem_val;
    sem_val.val = value;
    semctl(semid, 0, SETVAL, sem_val);
}

void sem_getmode(int semid)
{
    struct semid_ds sem_info;
    semctl(semid, 0, IPC_STAT, &sem_info);
    printf("Semaphore permissions: %o\n", sem_info.sem_perm.mode);
}

void sem_getval(int semid)
{
    int val = semctl(semid, 0, GETVAL);
    printf("Semaphore value: %d\n", val);
}

void sem_setmode(int semid, char *mode)
{
    struct semid_ds sem_info;
    sem_info.sem_perm.mode = strtol(mode, 0, 8);
    semctl(semid, 0, IPC_SET, &sem_info);
}

void usage(void)
{
    fprintf(stderr, "usage: \n");
    fprintf(stderr, "semtool -c 创建信号量\n");
    fprintf(stderr, "semtool -d 删除信号量\n");
    fprintf(stderr, "semtool -p 信号量p操作\n");
    fprintf(stderr, "semtool -u 信号量u操作\n");
    fprintf(stderr, "semtool -s <val> 设置信号量初始化值\n");
    fprintf(stderr, "semtool -g 获取信号量的值\n");
    fprintf(stderr, "semtool -f 打开权限\n");
    fprintf(stderr, "semtool -n <mode> 设置权限\n");
}

int main(int argc, char *argv[])
{
    int opt;
    key_t key;
    int semid;

    key = ftok("/", 's');

    while ((opt = getopt(argc, argv, "cdpusg:fd:")) != -1)
    {
        switch (opt)
        {
        case 'c':
            semid = sem_create(key);
            break;
        case 'p':
            semid = sem_open(key);
            sem_p(semid);
            sem_getval(semid);
            break;
        case 'u':
            semid = sem_open(key);
            sem_v(semid);
            sem_getval(semid);
            break;
        case 'd':
            semid = sem_open(key);
            sem_delete(semid);
            break;
        case 's':
            semid = sem_open(key);
            sem_setval(semid, atoi(optarg));
            break;
        case 'g':
            semid = sem_open(key);
            sem_getval(semid);
            break;
        case 'f':
            semid = sem_open(key);
            sem_getmode(semid);
            break;
        case 'n':
            semid = sem_open(key);
            sem_setmode(semid, argv[2]);
            break;
        default:
            usage();
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}