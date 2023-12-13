#include <stdio.h>

/*
    大小端问题
    小端模式计算机读取数据方向：<---
    大端模式计算机读取数据方向：--->


    例子：需要存放 4 字节数据，int a = 1;
    内存地址：00---》fff 低--->高
    小端 01 00 00 00 
    大端 00 00 00 01


      
*/


//共用体
union Num{
    int a;
    char b;
} num;


int main(int argc, char const *argv[])
{
    printf("方式一---------------------------\n");
    
    int a = 0x12345678;
    char* p = (char*)&a;

    if(0x12 == *p){
        printf("大端 *p = 0x12\n");
    }

    if(0x78 == *p){
        printf("小端 0x78 == *p\n");
    }

    printf("%x %x %x %x \n",*(p+3),*(p+2),*(p+1),*(p));

    printf("方式二---------------------------\n");
    
    num.a = 0x1234; // 小端存放 34 12 00 00
                    //大端存放 00 00 12 34
    if(num.b == 0x34){
        printf("小端");
    }

      if(0x00 == *p){
        printf("大端");
    }





    return 0;
}

