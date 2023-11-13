
/*
func2栈帧

x=3
y=3
name[6]
sum
返回地址
前栈栈底 rbp

*/
#include <stdio.h>
int func2(int x,int y){
   char name[6];
   int sum; 
   printf("input: "); 
   gets(name); //输入字符过多,name会越界，导致覆盖sum和返回地址得值，栈溢出，程序崩溃
   return x+y;
}

int func1(int a){
   int x=1;
   printf(" func1 a=%d\n",a);
   func2(3,4);
   return a+x;
}

int main (int argc,char *argv[]) {
    func1(2);
    printf("main end\n");
    return 0;
} 