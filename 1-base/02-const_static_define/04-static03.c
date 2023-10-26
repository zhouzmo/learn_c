//---main.c
#include <stdio.h>
extern void fn(); //声明 fn 是外部函数
void main(){
    fn();
}

/*

gcc .\04-static03.c .\04-static03fn.c
C:\Users\zmo\AppData\Local\Temp\ccWnz8LM.o:04-static03.c:(.text+0xe): 
undefined reference to `fn'
collect2.exe: error: ld returned 1 exit status


*/