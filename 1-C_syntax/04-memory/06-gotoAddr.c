#include <stdio.h>

typedef void(*pFun)(int) ;


int main(int argc, char const *argv[])
{
    
    /*
    要对绝对地址0x100000赋值，我们可以用(unsigned int*)0x100000 = 1234;  
    那么要是想让程序跳转到绝对地址是0x100000去执行，应该怎么做？

    答案：*((void (*)( ))0x100000 )( );
　　首先要将0x100000强制转换成函数指针,即:
　　(void (*)())0x100000
　　然后再调用它:
　　*((void (*)())0x100000)();

　　用typedef可以看得更直观些:
　　typedef void(*voidFuncPtr)() ;
　　*((voidFuncPtr)0x100000)(); 
    */
   
   ((void (*)())0x100000)();
 
    
    //知道地址 如何取值
    int* p;
    p = 0x100000;
    


    return 0;
}
