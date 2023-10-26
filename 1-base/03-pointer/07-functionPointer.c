#include <stdio.h>


typedef void (*opFunc)(void); //自定义函数指针 新类型
void power(){
  printf("开机\n");  
}
int vol_up(){
  printf("音量加\n");  
}
int vol_down(){
  printf("音量减\n");  
}

int main(){
   opFunc control[]={power,vol_up,vol_down};
   //void (*control[])(void) ={power,vol_up,vol_down};
   control[0](); //替换法： control[0]等于power ->  power();
   control[1]();   
   control[2]();      
   return 0;
}