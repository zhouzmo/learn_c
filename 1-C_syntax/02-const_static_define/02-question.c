#include <stdio.h>

int main(int argc, char *argv[])
{
    int a = 248;
    b = 4;
    int const c = 21;
    const int *d = &a;
    int *const e = &b;
    int const *f const = &a;

    /*请问下列表达式哪些会被编译器禁止？为什么？
     *c=32; 禁止，无*c变量，只有&c

     d=&b; d为指向常量的指针，指针可变，常量不可变因此 d 可变，*d不可
     *d=43; 

     *e=34; e为常指针，即指针地址不可变，因此，*e可变，e不可
     e=&a;

     f=0x321f; 都不可
     */
}