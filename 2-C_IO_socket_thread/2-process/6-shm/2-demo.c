#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*
实验：开启两个线程，查看状态

现象：
开启两个进程
------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status
0x00001314 4          book       666        1024       2

一个进程执行 暂停。发现 nattch -1
------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status
0x00001314 5          book       666        1024       1

一个进程执行 删除。发现 key=0 status=dest
------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status
0x00000000 4          book       666        1024       1          dest


结论：
如果共享内存被程序占用，删除共享内存时，不会立马删除
但是 key=0 ,其他进程无法通过 key 来获取共享内存得shmid了
只有连接所有进程都死掉，才会真正得回收
*/


/*
实验：开启两个线程，一个删除，一个读数据

现象：
输入 0暂停 1删除 2读数据 其他退出
2
read shg(stu) name = zhoumo,age = 18

结论：
被删除，还是可以读
*/

typedef struct Student
{
    int age;
    char name[64];
} Student;

int main(int argc, char *argu[])
{

    // 创建共享内存
    int shmid = 0;

    shmid = shmget(0x1314, 1024, 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        if (errno == ENOENT)
        {
            printf("检查共享内存,发现不存在\n");
        }

        if (errno == IPC_EXCL)
        {
            printf("共享内存已存在\n");
        }
        return 0;
    }

    // 链接共享内存
    void *p = NULL;
    p = shmat(shmid, NULL, 0);

    Student stu = {18, "zhoumo"};

    memcpy(p, &stu, sizeof(stu));

    printf("shg(stu)name = %s,age = %d\n", ((Student *)p)->name, ((Student *)p)->age);

    printf("输入 0暂停 1删除 2读数据 其他退出\n");
    int num;
    int ret = 0;
    scanf("%d", &num);

    if (num == 0)
    {
        // 脱离映射 unmap ,不删除
        shmdt(p);
    }

    if (num == 1)
    {
        // 脱离映射 unmap ,删除
        shmdt(p);
        ret = shmctl(shmid, IPC_RMID, NULL);
    }

    if (num == 2)
    {
        printf("shg(stu)name = %s,age = %d\n", ((Student *)p)->name, ((Student *)p)->age);
    }

    return 0;
}
