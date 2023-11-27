#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    fork();
    fork();
    fork();
    printf("hello world!\n");
    return 0;
}

/*
                        1
                    1       1
                1   1       1   1
                11  11      11  11


hello world!
hello world!
hello world!
hello world!
hello world!
hello world!
hello world!
hello world!






*/