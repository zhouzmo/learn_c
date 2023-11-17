/*
要使用 gbd 调试需要通过 gcc -g hello.c
直接通过 gcc hello.c 进行调试会报错
生成可执行文件 a.out 后
通过 gdb a.out 进入调试模式


$ gcc -g  main.c  //编译生成带有调试信息的执行文件a.tou
$ gdb a.out //调试程序
$ l     //list: 查看源码(默认10行)
        //list 8  查看8行附近的代码
        //list main 查看函数名附近的码
$ b 5   //break: 在第5行设置断点
$ r     //run: 运行到断点处（如无则运行直到结束）
$ s     //step: 单步执行（如果是函数则进入）
$ n     //next: 单步跳过（如果是函数则跳过）
$ p a   //print: 查看变量内容
        // p &a  查看变量a的地址
        // p &b+1   b变量的地址根据数据类型递增(int型 会加4，char型则加1)
        // p *(&b+1)
        // p a[1] 
        // p *p1 查看p1指针里的内容 
        // p  add(4,7)  调用函数add 并打印返回值
$ p/t b // t  二进制显示 变量的值  
	// x  按十六进制格式显示变量。
	// d  按十进制格式显示变量。
	// u  按十六进制格式显示无符号整型。
	// o  按八进制格式显示变量。
	// a  按十六进制格式显示变量。
	// c  按字符格式显示变量。
	// f  按浮点数格式显示变量。      
$ x &a  //显示某地址的内存值 
        //例如 float a = 0.45; 的内存值为
        // 0x7fffffffdf8c:	00111110111001100110011001100110        
上下键  //可查看历史命令
$ bt     //查看堆栈信息 （如嵌套调用多个函数时）
$ c      //continue： 继续执行程序（直到下一个断点或者结束）
$ q      //quit: 退出gdb调试
$ dmesg  //查看内核信息     



x/nfu addr
其中，n、f 和 u 参数是可选的，分别表示输出的单位数量、格式和单位大小。
默认情况下，n=1，f=x，u=4。

n：输出的单位数量，可以是十进制数字或者表达式；
f：输出的格式，包括以下几种类型：
x：以十六进制格式输出；
d：以有符号十进制格式输出；
u：以无符号十进制格式输出；
o：以八进制格式输出；
t：以二进制格式输出；
a：以地址格式输出；
i：以指令格式输出。
u：输出的单位大小，可以是 1、2、4、8 等值。




*/




#include <stdio.h>
#define MY_NAME "yzg"
int add(int x,int y)
{
   int ret=x+y;
   return ret;
}

int main()    
{    
   int a[5] = { 1, 2, 3, 4, 5 };  
   int *p1  = (int*)(&a + 1);    
   int *p2  = (int *)((int)a + 1);    

   int b=3;
   int c=5;
   int ret=0; 
   ret = add(b,c);
   printf("helloworld \n");  
   printf("my name is %s \n",MY_NAME);
   printf("add result = %d\n",ret);
   printf("p1[-1] = 0x%x \n", p1[-1]);  
   printf("*p2 = 0x%x \n", *p2);   
   return 0;    
}  