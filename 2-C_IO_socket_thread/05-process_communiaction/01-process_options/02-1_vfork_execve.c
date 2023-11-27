#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    int num = 0;
    int ret = 0;
    pid_t pid = vfork();

    if (pid > 0)
    {
        // 父进程
        printf("PID:%d\tParent process\n", getpid());

        printf("Parent num = %d \n", num);
    }
    else if (pid == 0)
    {
        // 子进程
        printf("PID:%d\tChild process\n", getpid());
        // sleep(5);
        num = 11;
        printf("Child num = %d \n", num);

        /*== 拉起自己写的程序，替换代码段，数据段，保持pid不变*/
        // char *const argv[] = {"./hello.out", "-l", NULL};
        // execvp("./hello.out", argv);
        execve("/bin/ls", NULL, NULL);
        // execve("/home/book/learn_c/2-C_IO_socket_thread/05-process_communiaction/01-process_options/hello.out", NULL, NULL);
        // execve("./hello.out", NULL, NULL);// 我不李姐

        // 1、execve
        // char *const argv[] = {"ls", "-l", NULL};
        // 环境变量，不指定，默认为当前用户的环境变量，传递，只使用你的
        // char *const envp[] = {NULL};
        // ret = execve("/bin/ls", argv, envp);
        // 2、execl
        // execl("/bin/ls", "ls", "-l", NULL);
        // 3、execvp
        // char *const argv[] = {"ls", "-l", NULL};
        // execvp("ls", argv);

        perror("execve error ");
        printf("execve success ret = %d\n", ret);
        _exit(0); // 子进程调用_exit函数终止
    }

    printf("main end...\n");
    return 0;
}

/*
$ ./a.out
PID:50131       Child process
Child num = 11
PID:50130       Parent process
Parent num = 11
main end...
 total 72
-rwxrwxrwx 1 book book 1757 Nov 27 02:50 01-0_fork.c
-rwx------ 1 book book  456 Nov 27 03:01 01-1_duo_fork.c
-rwx------ 1 book book  701 Nov 27 03:10 01-2_question.c
-rwx------ 1 book book  738 Nov 27 03:47 01-3_copyOnwrite.c

*/