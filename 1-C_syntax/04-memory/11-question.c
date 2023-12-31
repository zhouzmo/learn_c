//问：下面程序，变量a的内存地址为：0x0034fc50  问printf输出值是多少
int main()    
{    
  int a[5] = { 1, 2, 3, 4, 5 };  
  /*
 0x0034fc50   01 00 00 00 
 0x0034fc54   02 00 00 00
 0x0034fc58   03 00 00 00
 0x0034fc5c   04 00 00 00 
 0x0034fc60   05 00 00 00
  */
  printf("%x\n",a);//61fdf0
  printf("%x\n",&a);//61fdf0 相同
  int *p1  = (int*)(&a + 1);   
  //p1 = 0x0034fc64 
  int *p2  = (int *)((int)a + 1);    
  // 转为int后加一 结果为 0x0034fc51 ，占4个字节，后后数4个
  // 00 00 00 02 ，小段机器，结果 02000000

  printf("0x%x, 0x%x\n", p1[-1], *p2);  
  // p1 [-1] == *(p1-1) , 0x0034fc51
  //
  return 0;    
}  	


/*
首先分析p1

&a为数组指针，类型是"int (*)[5]"，数组指针是指向数组首元素的地址的指针

&a的内存地址为：0x0034fc50

&a + 1指针加一，向后移动一个元素，这里的元素类型是"int (*)[5]"，其空间大小为20字节，因此

&a + 1的内存地址为：0x0034fc64（0x0034fc50 + 0x14）

p1的内存地址为：0x0034fc64

p1[-1]数组下标为负数，向前移动一个元素（注：数组越界在编译时与运行时都不会报错，除非操作非法内存）

p1[-1]元素所在的地址为：0x0034fc60

因此，可知p1[-1]的地址为a + 4的地址

p1[-1] == *(a + 4) == 5

继续分析p2

(int)a的值为0x0034fc50

(int)a + 1等于0x0034fc51

分析内存结构，测试环境字节序为little endian

01 00 00 00 02 00 00 00 03 00 0...

*p2等于0x2000000




*/