#include <stdio.h>
int swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

/*

1、定义头尾，定义 midVal
2、头向左移动，找到大于 midVal 的值，尾向右移动，找到小于 midVal 的值，交换。，只要头 <= 尾，跳出循环
3、交换完成，出现 头尾有等于 midVal 的情况，头等于，尾移动，因为已经交换值了，，
4、跳出循环后，头尾是否相等，相等需要 头左移，尾右移
5、递归调用

*/
void qucitSort(int *arr, int start, int end)
{

    int left = start;
    int right = end;
    int midVal = arr[(start + end) / 2];

    while (left < right)
    {

        while (midVal > arr[left])
        {
            left++;
        }

        while (midVal < arr[right])
        {
            right--;
        }

        if (left >= right)
        {
            break;
        }
        swap(&arr[left], &arr[right]);

        if (arr[left] == midVal)
        {
            right--;
        }
        if (arr[right] == midVal)
        {
            left++;
        }
    }

    if (left == right)
    {
        left++;
        right--;
    }

    if (start < right)
    {
        qucitSort(arr, start, right);
    }
    if (end > left)
    {
        qucitSort(arr, left, end);
    }
}

int main(int argc, char const *argv[])
{
    int arr[] = {0, 78, 456, 54, 3, 45, 7, 43, 53, 45, 34, 5, 345, 45, 4};
    // int arr[] = {0};
    int end = sizeof(arr) / 4;
    qucitSort(arr, 0, end);

    for (int i = 0; i < end; i++)
    {
        printf("%d,", arr[i]);
    }
    return 0;
}
