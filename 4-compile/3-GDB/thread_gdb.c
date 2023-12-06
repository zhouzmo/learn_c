#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *print1_msg(void *arg)
{
    while (1)
    {
        printf("print1_msg\n");
        usleep(100);
    }
}

void *print2_msg(void *arg)
{
    while (1)
    {
        printf("print2_msg\n");
        usleep(100);
    }
}

int main()
{
    pthread_t id1, id2;
    pthread_create(&id1, NULL, print1_msg, NULL);
    pthread_create(&id2, NULL, print2_msg, NULL);
    pthread_join(id1, NULL); // 使主线程等待该线程结束后才结束，否则主线程很快结束，该线程没有机会执行
    pthread_join(id2, NULL);

    return 0;
}

/*
gcc -g -pthread -o a.out thread_gdb.c

gdb -tui -q a.out




*/