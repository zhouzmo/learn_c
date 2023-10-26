#include <stdio.h>;

int main(int argc, char const *argv[])
{

    printf("指针与数组--------------------------------------\n");
    int a[4] = {1, 2, 3, 4};
    int a1[7] = {1, 2, 3, 4, 5};
    printf("\n");
    int a2[] = {1, 2, 3, 4, 5, 6};
    printf("%d\n", a[0]); // 1
    // printf("%d\n",a1[6]);   //0 这个0也是根据编译器决定的
    // printf("%d\n",a1[9]); // 2 数据越界，根据不同的编译器可能导致程序崩溃

    int len = sizeof(a) / sizeof(int);
    printf("数组长度：%d\n", len);
    for (int i = 0; i < len; i++)
    {
        printf("%d ", a[i]);
    }
    return 0;
}
