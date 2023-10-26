#include <stdio.h>
int main()
{
   int a = 0x12345678;
   int b = 0x0A0B0C0D;  
   // 局部变量 会顺序分配地址:  
   //(gdb) x &a
   // 0x7fffffffdf78:	0x12345678  
   //(gdb) x &b
   // 0x7fffffffdf7c:	0x0a0b0c0d
   //因是小端 故内存为
   //内存： 高 0x0A0B0C0D 0x12345678  低                
   unsigned char *c=( unsigned char *)&a; //小端 a 地址指向低字节 78

   /*
   内存地址 ---->
            x x+1 ...  
   小端存放 78 56 34 12 ？
    强转为 int ，变为 4字节，加一，往后读一位
    即？123456
   
   */
   printf("%x ",*(c+1)); // 地址加1后取内容 为0x56
   printf("%x ",*(int*)(c+1)); //强转为int后再取内容 为0x0D123456
   return 0;
}	