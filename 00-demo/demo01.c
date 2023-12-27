#include <stdio.h>

void quickSort(int arr[], int start, int end)
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
        if (left <= right)
        {
            int temp = arr[left];
            arr[left] = arr[right];
            arr[right] = temp;

            left++;
            right--;
        }
    }

    if (left == right)
    {
        left++;
        right--;
    }

    if (right > start)
    {
        quickSort(arr, start, right);
    }
    if (left < end)
    {
        quickSort(arr, left, end);
    }
}

int main()
{

    int arr[] = {4, 3, 4, 32, 3, 2, 12, 1, 0};
    int end = sizeof(arr) / 4;
    quickSort(arr, 0, end - 1);

    for (int i = 0; i < end; i++)
    {
        printf("%d,", arr[i]);
    }

    printf("\n");
    return 0;
}
