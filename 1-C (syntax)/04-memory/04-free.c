//问: 输出结果是什么? 为什么

/*
 答案：输出str is world。
 进程中的内存管理 是由库函数完成，不是由操作系统。
 malloc时 库函数 会向操作系统申请空间，并做记录
 但free时，库函数 不会把空间马上还给操作系统，还能继续访问。
 但后面可能这个空间分配给别人。
 所以为避免误操作 free(str) 后 需 str=NULL;

*/
#include <stdio.h>
#include <stdlib.h>
void GetMemory(char **p,int num){                          
   *p=(char *)malloc(num); 
}  
int main(){
   char *str=NULL;
   GetMemory(&str,100);  
   strcpy(str,"hello");

   printf("\n str is %s",str);//str is hello
   free(str);
//    str = NULL;
   if(str!=NULL)
   {
      strcpy(str,"world");
   }   
   printf("\n str is %s",str); // str is world
}