#include <stdio.h>

int* createArray(int size) {
    int* array = (int*)malloc(size * sizeof(int));
    return array;
}

int main() {
    int* dynamicArray = createArray(5); // 调用指针函数，返回一个动态分配的整数数组指针
    dynamicArray[0] = 1;
    printf("Value: %d\n", dynamicArray[0]);
    free(dynamicArray); // 释放动态分配的内存
    return 0;
}
