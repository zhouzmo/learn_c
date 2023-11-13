#include <stdio.h> 
//入栈顺序：是从下到上，从右至左，反向进栈
int func (int a,int b )   //4.b    5.a  (从右至左)
{ 
	char name[6]; //3. name[]
	int sum;      //2.sum     从下到上
	printf("Please type your name: "); 
	gets(name); 
	printf("Hello, %s!", name);
	sum =  a+b;
	return sum; 
} 

int main (int argc,char *argv[]) 
{ 
	func(3,4); //调用函数时，入栈顺序
	           //1. 返回地址入栈( 如return 0; ) 
	return 0; 
}  


/*
栈为先进后出
rbp 栈底指针
rip 栈顶指针


$ gcc -g -fno-stack-protector -z execstack  main.c	//编译时 关闭栈保护
$ gdb ./a.out
(gdb) l 1,25   //查看1到25行代码
(gdb) b 15     //断点设在第15行(char name[6]处)
(gdb) r   



(gdb) i reg   //查看寄存器
              //rsp : 栈顶指针(进出栈时它会变化 ),有些cpu里叫esp
              //rbp : 栈底指针
              //rip : 指令指针(下一条 等待执行的 指令地址)
(gdb) x $rip  //查看rip寄存器内容 (知道程序执行到那了)
              // x/16x $rip   查看16个将要执行代码的机器码
 0x5555555551d8 <main+19>:	0x000004be  //func(1,2); 代码的内存位置和对应的机器码               
(gdb)disas main //反汇编main函数
Dump of assembler code for function main:
   0x00005555555551c5 <+0>:	endbr64 
   0x00005555555551c9 <+4>:	push   %rbp
   0x00005555555551ca <+5>:	mov    %rsp,%rbp
   0x00005555555551cd <+8>:	sub    $0x10,%rsp
   0x00005555555551d1 <+12>:	mov    %edi,-0x4(%rbp)
   0x00005555555551d4 <+15>:	mov    %rsi,-0x10(%rbp)
   	         //func(3,4); 代码的内存位置和反汇编代码
=> 0x00005555555551d8 <+19>:	mov    $0x4,%esi   //func 传参 4
   0x00005555555551dd <+24>:	mov    $0x3,%edi   //func 传参 3
   0x00005555555551e2 <+29>:	callq  0x555555555169 <func> //跳到函数 func中 
   0x00005555555551e7 <+34>:	mov    $0x0,%eax   //调用func后的返回地址
   0x00005555555551ec <+39>:	leaveq 
   0x00005555555551ed <+40>:	retq   
End of assembler dump.



(gdb) bt full  //查看全部的 函数调用栈信息
 #0  main (argc=1, argv=0x7fffffffe088) at main.c:15 
           //argc=1: 参数个数 1    对应 ./a.out
 	   //argv=0x7fffffffe088 参数指针数组起始位置  如 ./a.out  hello  word 时
 	   //argv[0] 对应 ./a.out字符串起始地址  argv[1] 对应 hello 字符串起始地址
	                            
(gdb) x $rbp  //查看栈底
  0x7fffffffdf90:	0x00000000 //main函数的栈低
(gdb) x/8 $rsp //查看栈顶 (进出栈时它会变化 )  8：查看从栈顶地址开始 连续8个int 内存单元
               //入栈:   argv=0x7fffffffe088            字节对齐补0       argc=1
  0x7fffffffdf80(rsp):	0xffffe088	0x00007fff	0x00000000	0x00000001  	       
  0x7fffffffdf90(rbp):	0x00000000	0x00000000	0xf7de30b3	0x00007fff


(gdb) info proc mappings  //查看地址映射	
process 79450
Mapped address spaces:

          Start Addr           End Addr       Size     Offset objfile
      0x555555554000     0x555555555000     0x1000        0x0 /home/yhai/C/hello/a.out
      0x555555555000     0x5555555560 00     0x1000     0x1000 /home/yhai/C/hello/a.out
      0x555555556000     0x555555557000     0x1000     0x2000 /home/yhai/C/hello/a.out
      0x7ffff7ffe000     0x7ffff7fff000     0x1000        0x0 
      0x7ffffffde000     0x7ffffffff000    0x21000        0x0 [stack] //栈地址
  0xffffffffff600000 0xffffffffff601000     0x1000        0x0 [vsyscall] //内核系统调用(3G ~4G 高位地址)






(gdb) x/20xw $rsp
0x7fffffffdf70:	0xffffdf96	0x00007fff	0x00000004	0x00000003
0x7fffffffdf80:	0xf7fb62e8	0x00007fff	0x555551f0	0x00005555
0x7fffffffdf90:	0xffffdfb0	0x00007fff	0x555551e7	0x00005555
0x7fffffffdfa0:	0xffffe0a8	0x00007fff	0x00000000	0x00000001
0x7fffffffdfb0:	0x00000000	0x00000000	0xf7de9083	0x00007fff
(gdb) n
8		gets(name); 
(gdb) n
Please type your name: abcd
9		printf("Hello, %s!", name);
(gdb) x/20xw $rsp
0x7fffffffdf70:	0xffffdf96	0x00007fff	0x00000004	0x00000003
0x7fffffffdf80:	0xf7fb62e8	0x62617fff	0x55006463	0x00005555
0x7fffffffdf90:	0xffffdfb0	0x00007fff	0x555551e7	0x00005555
0x7fffffffdfa0:	0xffffe0a8	0x00007fff	0x00000000	0x00000001
0x7fffffffdfb0:	0x00000000	0x00000000	0xf7de9083	0x00007fff






  --------4G---------0xFFFFFFFF  高地址
|      -----      | 0xffffffffff601000  
|       内核      |   
|      -----      | 0xffffffffff600000  //内核系统调用 起始地址
|                 |   
========3G=========0xC0000000
|                 |
|-----------------| 0x7ffffffde000  //系统栈的起始地址: 栈是向下生长
|    栈(stack)    |    
|    --------     | 0x7fffffffdf90  //main函数的栈底
|   0x00000001    |   //argc=1   调用main函数时入栈 
|   0x00000000    |   //字节对齐  
|   0x00007fff    |   //argv=0x7fffffffe088  指针数组首地址(8字节，64位机，小端)
|   0xffffe088    |
|    -------      |   //当main函数栈进完后，会rbp存入下一地址中，把下一地址存入rbp中，作为新栈的栈底
|   0x00007fff    | 0x7fffffffdf70 //func函数的栈底：存储的上一栈的栈底(main函数的栈底 8字节)
|   0xffffe0a8    |    
|   0x00005555    |  //返回地址： 调用func后，返回到main的地址
|   0x555551e7    |
|   0x00005555    |  0x7fffffffdf8c //sum 局部变量入栈   ->  顺序是从下到上，从右至左，反向进栈
|   0x555551f0    |  //name[6]
|   0x00007fff    |
|   0xf7facfc8    |  //补全
|   0x00000003    |  //实参拷贝到形参 b=4 a=3; 
|   0x00000004	  |                
|-----------------| 0x7ffffffff000 
|                 |
|      .....      |          
|                 |
|-----------------|
|    堆(heap)     |   
-------------------
|    DATA数据段   | 
-------------------
|                 |
|      -----      | 0x555555558000 
|   代码区(只读)   | 
|      -----      | 0x555555554000  //a.out 程序的 起始地址
|                 | 
-------------------0x00000000 低地址		

*/