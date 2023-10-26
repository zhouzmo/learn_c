#include <stdio.h>
#include <stdlib.h>

/*
申请空间
 pt = (char *)malloc(10)
 空指针判断
 if (pt == NULL)
释放空间
free(pt); 
避免野指针
pt =NULL

*/


int main()
{
    char *pt;
    pt = (char *)malloc(10); // 堆上申请空间(malloc的输入参数，是申请空间的字节数)
                             // 成功 返回值是申请空间的地址，失败返回NULL;
    if (pt == NULL)
    {
        printf("申请空间失败");
        return -1;
    }
    *pt = 0x11;
    *(pt + 1) = 0x22;
    printf("%x %x %x \n", pt[0], pt[1], pt[2]); // 11 22 ffffffad(随机值)

    free(pt); // 释放空间，避免内存泄漏
    // pt 指向的空间还可以进行操作，避免这种情况需要将 pt赋值为 NULL
       pt =NULL; //避免野指针 (迷途指针),操作已释放的空间

    *(pt + 2) = 0x33;
    printf("%x %x %x \n", pt[0], pt[1], pt[2]); // ffffffb0 52 33

    return 0;
}