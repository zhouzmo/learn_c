#include <stdio.h>

void printBinaryWithPadding(unsigned int num, int length)
{
    for (int i = length - 1; i >= 0; i--)
    {
        int bit = (num >> i) & 1;
        printf("%d", bit);
        if (i % 4 == 0)
        { // 在每4位之后打印一个空格
            printf(" ");
        }
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    unsigned i = 0x2;
    printBinaryWithPadding(i, sizeof(unsigned) * 8);
    i = i | (0x1 << 3);

    printBinaryWithPadding(i, sizeof(unsigned) * 8);
    return 0;
}
