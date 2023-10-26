
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int status = -1;
    pid_t pid;

    pid = fork();
    if (pid == 0)
    { // 子进
        printf("fork\n");
        exit(1);
    }
    else if (pid > 0)
    { // 父进
        pid = waitpid(pid, &status, 0);
        printf("status=0x%x\n", status);
    }
    else
    {
        perror("fork\n");
    }

    return 0;
}