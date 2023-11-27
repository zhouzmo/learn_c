#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void testFunc()
{
    printf("hello world!\n");
}

int main()
{
    int procNum = 2;
    int loopNum = 3;
    int i = 0, j = 0;
    pid_t pid;

    for (i = 0; i < procNum; i++)
    {
        pid = fork();

        if (pid == 0)
        {
            for (j = 0; j < loopNum; j++)
            {
                testFunc();
            }
            printf("PID:%d exit!\n",getpid());
            exit(0);
        }
    }
    return 0;
}

/*

s$ ./a.out
hello world!
hello world!
hello world!
PID:47877 exit!
hello world!
hello world!
hello world!
PID:47878 exit!




*/