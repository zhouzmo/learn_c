#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

extern char **environ;

int main(int argc, char *argv[])
{
    int i = 0;
    for (i = 0; environ[i] != NULL; i++)
    {
        printf("%s:\n", environ[i]);
    }

    printf("hello,world\n");
    return 0 ;
}
