#include <stdio.h>

void fn();
int num;   //定义 全局变量
//static int num; //定义静态全局变量(限定仅本文件内 可见)
                  //底层: 存储在全局数据区(程序退出时才释放，不像栈和堆)
                  //思想: 避免名字冲突(仅文件内可见),和强耦合(避免全局变量的多文件强耦合)
void main(){
    printf("init num=%d\n",num);  //自动初始化为0
    num =10; 
    printf("main num=%d\n",num);
    fn();
}
