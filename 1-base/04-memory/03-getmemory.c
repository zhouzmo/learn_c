//问：下面代码有上面问题，如何改正
#include <stdio.h>
#include <stdlib.h>

void getmemory(char **p)
{
   *p=(char *) malloc(100); 
   strcpy(*p,"hello world");
}

int main( )
{
   char* str=NULL;
   getmemory(&str); 
   printf("%s",str);
   free(str);
   return 0;
}


/*
将 str 的地址传入函数
获取一段空间，将其作为 str 的值（指向 str值得地址）
给 str 指向空间中赋值

*/