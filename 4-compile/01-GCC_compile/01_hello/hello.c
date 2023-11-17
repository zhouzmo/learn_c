#include <stdio.h>

#define   MAX  20 
#define   MIN  10 

//#define  _DEBUG
#define   SetBit(x)  (1<<x) 

int main(int argc, char* argv[])
{
    printf("Hello World \n");
    printf("MAX = %d,MIN = %d,MAX + MIN = %d\n",MAX,MIN,MAX + MIN); 

#ifdef _DEBUG
    printf("SetBit(5) = %d,SetBit(6) = %d\n",SetBit(5),SetBit(6));
    printf("SetBit( SetBit(2) ) = %d\n",SetBit( SetBit(2) ));		
#endif    
	return 0;
}

/*
在 linux 环境下执行，最好
$  gcc -E hello.c -o 预处理-E.i
$  gcc -S hello.c -o 汇编-S.s
$  gcc -c hello.c -o 机器码-c.o

编译详细过程
$  gcc hello.c -o hello.out -v > 编译过程-v.json 2>&1  


*/