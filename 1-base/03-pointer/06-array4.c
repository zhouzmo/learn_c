#include <stdio.h>

int main(int argc, char const *argv[])
{
    char name[][10] = {"yhai", "yzg", "li ming"};
    // 二维数组(实际的分配了空间)
    // 第一维可不指定，但其余维必须指定，才能自动推导分配空间

    printf("%s %c", name[1], name[2][3]);
    name[1][2] = 'w'; // 能改写(已分配了空间)
    printf("%s ", name[1]);
}
