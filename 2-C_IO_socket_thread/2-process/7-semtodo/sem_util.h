#ifndef _SEM_UTIL_H_
#define _SEM_UTIL_H_


#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

int sem_create(key_t key);
int sem_open(key_t key);
int sem_setval(int semid, int value);
int sem_getval(int semid, int *val);
int sem_v(int semid);
int sem_p(int semid);

#endif /* _SEM_UTIL_H_ */