#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid;
    int status;

    pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        // 子进程执行的代码
        printf("Child process\n");
        sleep(2); // 模拟子进程执行一些任务
        exit(123); // 子进程退出，并设置退出状态为 123
    } else {
        // 父进程执行的代码
        printf("Parent process\n");

        pid = waitpid(pid, &status, 0);

        if (pid == -1) {
            perror("Waitpid failed");
            return 1;
        }

        if (WIFEXITED(status)) {
            printf("Child process exited with status: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process terminated by signal: %d\n", WTERMSIG(status));
        }
    }

    return 0;
}