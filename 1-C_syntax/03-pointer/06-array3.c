#include <stdio.h>

int main(int argc, char const *argv[])
{

    printf("指针与数组（各种的定义）--------------------------------------\n");

    /*



    d)

    j) int (*pfun)(int);//定义函数指针:其指向的类型 是函数(该函数有一个整型参数并返回一个整型)
                      //pfun指针，存放的是函数名(函数代码的起始地址)
                      //例：
                      //  int add10(int num){
                      //     return num+10;
                      //  }
                      //  int (*pfun)(int);
                      //  pfun = add10;
                      //  printf("%d",pfun(5));
    o) int (*a[10])(int); //定义函数指针数组： 数组的元素类型是函数指针
                          //                 (该函数有一个整型参数并返回一个整型数)
                          //替换法：用pfun 替换 a[10]
                          //    类似 pfun a[10]  数组里存放10个函数指针
    */
    printf("1-指针的指针--------------------------------------\n");
    int num = 1;   // 定义变量，变量类型是   int
    int *p = &num; // 定义指针, 其指向的类型是 int
    int **pp = &p; // 定义指针的指针，它指向的是指针(其指向的类型是 int)
    printf("pp的值%x --> p的地址%x,p的值%x --->num的地址%x，num的值%d\n", pp, &p, p, &num, num);

    printf("2-指针数组嵌套--------------------------------------\n");
    int a[3] = {1, 2, 3};

    int *ap[3]; // 定义指针数组: 数组元素的类型是指针(其指向的类型是 int)
                //(int *) ap[3];  优先级 从右到左,
    ap[0] = &a[0];
    ap[1] = &a[1];
    ap[2] = &a[2];
    printf("定义 int* 类型的数组，即有3个指向int型的指针构成的数组\n");
    printf("ap=[%x-->%d,%x-->%d,%x-->%d]\n", ap[0], *ap[0], ap[1], *ap[1], ap[2], *ap[2]);

    int(*pa)[5]; // 定义数组指针:
    // pa是指向整型数组中三个元素的指针
    // 其指向的类型是数组(有 n 个整型数元素) int[]* pa
    // 替换法(用a 替代*pa ，则pa是指向a数组的指针)
    int arr[3][5] = {
        {10, 20, 30, 40, 5},
        {50, 60, 70, 80, 6},
        {90, 100, 110, 120, 7}};

    pa = &arr[0]; // 或者 int (*pa)[4] = arr
    printf("%d,%d\n", *pa, (*pa)[0]);
    printf("%d\n", (*(pa + 1))[3]); // 不能理解，下次再说

    printf("3-指针数组的指针--------------------------------------\n");
    int *(*apa)[4]; // 定义指针数组的指针: 其指向的类型是数组，而数组元素的类型是指针
                    //                                      (其指向的类型是 int)
    int a2[3] = {11, 22, 33};
    int a3[3] = {111, 222, 333};

    return 0;
}
