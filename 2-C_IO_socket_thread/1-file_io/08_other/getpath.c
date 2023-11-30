
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    char path[100];
    if (getcwd(path, sizeof(path)) != NULL)
    {
        printf("Current file path: %s\n", path);
    }
    else
    {
        perror("getcwd() error");
        return -1;
    }

    return 0;
}
