#include <stdio.h>

int main(int argc, char const *argv[])
{
    char name0[] = "abcd";
    int len0 = strlen(name0);

    char *name1[] = {"yhai", "yzg", "li ming"}; // 注: 仅分配3个指针空间(指向的是字符串常量)
    printf("%s ", name1[1]);
    // name1[1][2] = 'w'; // 段错误: 不能改写，因未分配空间
    printf("%s ", *(name1+1));


    return 0;
}
