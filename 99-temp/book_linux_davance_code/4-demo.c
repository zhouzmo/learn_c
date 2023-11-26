#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/wait.h>

#define MAXLINE 4096

int main(void) {
    char buf[MAXLINE];
    pid_t pid;
    int status;

    printf("%% ");  /* print prompt (printf requires %% to print %) */

    while (fgets(buf, MAXLINE, stdin) != NULL) {
        if (buf[strlen(buf) - 1] == '\n') {
            buf[strlen(buf) - 1] = '\0'; /* replace newline with null */
        }

        if ((pid = fork()) < 0) {
            perror("fork error");
            exit(1);
        } else if (pid == 0) {  /* child */
            execlp(buf, buf, (char *) 0);
            perror("couldn't execute");
            exit(127);
        }

        /* parent */
        if ((pid = waitpid(pid, &status, 0)) < 0) {
            perror("waitpid error");
            exit(1);
        }

        printf("%% ");
    }

    exit(0);
}