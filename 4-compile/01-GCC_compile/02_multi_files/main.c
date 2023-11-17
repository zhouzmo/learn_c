#include <stdio.h>
#include "sub.h" // 使用 “” 会从当前路径查找
// #include <sub.h>     // 使用 <> 会从，，，

int main(int argc, char *argv[])
{
       int i;
       printf("Main fun!\n");
       sub_fun();
       return 0;
}

/*

$ gcc hello.c -o hello.out
$ gcc -o test main.c sub.c -v > 多个文件编译过程-v.json 2>&1

指定头文件目录
$ gcc main.c -o main.out -c  -I ./

*/

/*
== 静态链接

编译汇编
gcc -c -o main.o main.c
gcc -c -o sub.o sub.c
# 生成静态库 libsub.a
ar crs libsub.a sub.o
# 如果.a 不在当前目录下，需要指定它的绝对或相对路径
gcc -o test_static.out main.o libsub.a

*/

/*
== 动态链接

编译汇编
gcc -c -o main.o main.c
gcc -c -o sub.o sub.c
# 生成动态库 libsub.so
gcc -shared -o libsub.so sub.o
# 可以省略 lib**.so，但需需要指定 LIBRARY_PATH，或者拷贝到  LIBRARY_PATH中
gcc -o test main.o -lsub 
# 链接成功，但是无法运行，需要将库文件放到指定文件下
gcc -o test_dynamic.out main.o  -L ./ -lsub

# 新增库文件路径为当前路径，success
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
./test_dynamic.out
*/