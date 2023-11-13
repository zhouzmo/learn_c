#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    char s = 'a'; // 97
    int *pt;
    pt = (int *)&s;
    *pt = 0x1234;
    // 越界赋值
    //  访问未知区域(pt指向空间未完全分配，只有1个字节是s的，其它的未知)
    //  可gdb调试看看: (gdb) x &s   (gdb) x pt
    // printf("%d\n", *pt); // 段错误：访问未知区域

    printf("字符串 \\0数组越界----------------------------\n");
    char astr[5];
    char *pstr = "12345678eqweqwewqew"; // 包括末尾的’\0’,
    // strcpy(astr, pstr);   // 数组越界(pstr 需6个字节空间)
    strncpy(astr, pstr, sizeof(astr));
    printf("%s \n", astr);

    printf("无结尾 \\0 的strcpy----------------------------\n");
    char string[10], str[10];
    int i;
    for (int i = 0; i < 10; i++)
    {
        str[i] = 'a' + i;
    }
    string[9] = str[9] = '\0'; // 给结尾加上 \0
    strcpy(string, str);       // 该函数遇到 \0才会结束拷贝
    printf("%s %s\n", str, string);

    printf("strlen未统计 \\0----------------------------\n");
    char a[5];
    char *p = "12345";
    printf("%d %d\n", strlen(p), sizeof(a));
    if (strlen(p) <= sizeof(a))
    { // 去掉=号，  因strlen的结果未统计’\0’
        strcpy(a, p);
    }
    printf("%s %s\n", a, p);

    printf("越界拷贝----------------------------\n");

    // char a1;
    // char *str1 = &a1;
    char* str1 = (char*) malloc(6);
    if(str1 == NULL){
        return -1;
    }
     strncpy(str1, "hello", sizeof(str1));
    // strcpy(str1, "hello");
    printf("%s",str1);//h12345
    free(str1);
    str1 = NULL;

    return 0;
}
