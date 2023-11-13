#include <stdio.h>

int main(int argc, char const *argv[])
{

    int *pt = NULL; // 定义指针pt ： pt是一个变量，存放指向int类型数据的地址
                    //  * 表示指向
                    // 空白符无关 int *p;  int * p;都不影响
    int num = 2;
    pt = &num;           // 将 num 的地址，赋值给 pt ，所以 pt 为指向 num 的指针
                         // & 表示取地址
                         // 可以通过 gdb 查看内存值
    printf("%x\n", &num); // 
     printf("&pt = %x\n", &pt); // 

    printf("*pt = %x\n", *pt); // 2
    printf("pt = %p\n", pt);  // 地址 6422036

    // 指针的读
    printf("%x \n", pt);         // 读 指针 (指针是个变量，它的存放的是地址)
    printf("%d \n", sizeof(pt)); // 指针的大小，32位机是4，64位机是8
    printf("%x \n", *pt);        // 读 指针指向的空间:把指针pt 指向的空间的内容取出来 (可用gdb查看pt)

    /*
    调试界面
    gdb) p pt
    $1 = (int *) 0x61fe14   指针 pt 指向的地址
    (gdb) p *pt
    $2 = 2                  指针 pt 指向地址中的值
    (gdb) n
    2 20        printf("%x\n", *(pt++));   //(*pt)++ : pt,1
    (gdb) n
    2
    21          printf("%x\n", *pt++);   //*pt++:    ptpt1
    (gdb) p pt
    $3 = (int *) 0x61fe18   发现指针 pt 指向的地址增加 4 个字节（一个int）
    (gdb) p *pt
    $4 = 6422040            导致指针 pt 指向地址中的值变化
    (gdb) q


    */
    printf("指针的读--------------------------------------\n");
    // 正确做法
    printf("%x\n", (*pt)++); //(*pt)++ : 取出pt指向的空间的内容,再把内容加1
    printf("%x\n", *pt);

    // 错误做法，查看非法地址会导致程序报错或崩溃
    printf("%x\n", *pt++); //*pt++:    取出pt指向的空间的内容，再pt指针加1
    printf("%x\n", *pt);
    //                      优先级是从右到左，与*(pt++)等价
    //           加1（即指向下一个对象，如果是int 则是地址加4个字节）
    printf("%x\n", *(pt++));
    printf("%x\n", *pt);

 

    return 0;
}
