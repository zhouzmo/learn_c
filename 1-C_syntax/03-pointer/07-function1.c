#include <stdio.h>

void swap1(int x,int y)  //x=a,y=b 值拷贝(实参a,b 拷贝给形参x,y)
{
	//交换的是形参，实参没变
	int temp;
	temp =x;
	x=y;
	y = temp;
}	

void swap2(int* x,int* y)  //x=a,y=b 值拷贝(实参a,b 拷贝给形参x,y)
{
	//交换的是形参，实参没变
	int temp;
	temp =*x;
	*x=*y;
	*y = temp;
}	


int main()    
{    
   int a=3,b=4;
   swap1(a,b); //传入实参 a,b 
   printf("a=%d,b=%d\n",a,b); 	//值没变，因交换的是形参

    swap2(&a,&b); //传入实参 a,b 
   printf("a=%d,b=%d\n",a,b); 	//值变化，传入为实参地址，修改地址内部值
   return 0;    
} 