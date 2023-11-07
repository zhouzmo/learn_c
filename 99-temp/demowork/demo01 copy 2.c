#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

static int* stauts;

int main()
{
    int buf[20];
    stauts = buf;
    memset(stauts,0,20);
    printf("%d",stauts[0]);
}

