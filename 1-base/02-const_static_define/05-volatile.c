
#include <stdio.h>
/*

声明volatile后
//编译器就不会优化，会从内存重新装载内容，而不是直接从寄存器拷贝内容(副本)
//否则会优化，会读寄存器里的副本，而重新读内存（因寄存器比内存快）




一个参数可以既是 const 又是 volatile 吗？
可以的，
const只是不允许程序中的代码改变某一变量，其在编译期发挥作用，
它并没有实际地禁止某段内存的读写特性。
对于某些外设寄存器等易失性变量，可以使用volatile关键字来标识，以便于提高程序的可靠性和稳定性。

*/
#define GPA1DAT *((volatile unsigned int *)0xE0200084)
int main(int argc, char const *argv[])
{
    while (1)
    { // 反复读取GPA1DAT值，当为1时火灾报警
        if (GPA1DAT)
        { // 如不加volatile，编译器优化后，变成只读一次，
          // 后面用的是副本数据。一直为0
            fire() break;
        }
    }

    return 0;
}
