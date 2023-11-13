#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sigterm_handler(int signo) {
    printf("Have caught signal %d.\n", signo);
    exit(0);
}

int main(void) {
    signal(SIGINT, sigterm_handler);
    signal(SIGTERM, sigterm_handler);
    
    while(1);
    
    return 0;
}