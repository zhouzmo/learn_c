
#include <stdio.h>
#include <string.h>

/*
================================================================
char *strstr(const char *haystack, const char *needle);
该函数用于在字符串 haystack 中查找第一次出现字符串 needle 的位置，


参数：
haystack: 要搜索的主字符串。
needle: 要查找的子字符串。
返回值：
如果在 haystack 中找到 needle，则返回指向第一次出现 needle 的位置的指针。
如果未找到 needle，则返回 NULL。
================================================================
int sprintf(char *str, const char *format, ...);
将格式化的数据写入字符串中
str
    是一个指向字符数组的指针，用于存储格式化后的字符串。
format
    是一个字符串，其中包含了格式化指令，类似于 printf 中的格式化字符串。
...
    表示可变数量的参数，这些参数根据 format 字符串中的格式化指令进行格式化并插入到结果字符串中。

返回值：
    sprintf 函数的返回值是一个整数，表示写入到字符串中的字符数，
    不包括字符串末尾的 null 终止符。如果发生错误，返回值为负数。
*/

void sprintfTest()
{
    char formattedString[100]; // 声明一个足够大的字符数组来存储格式化的字符串
    int age = 30;
    double height = 175.5;

    // 使用 sprintf 将格式化的数据写入 formattedString
    sprintf(formattedString, "年龄: %d, 身高: %.2lf cm", age, height);

    // 打印格式化后的字符串
    printf("格式化后的字符串: %s\n", formattedString); // 格式化后的字符串: 年龄: 30, 身高: 175.50 cm
    return 0;
}

int main()
{
     sprintfTest();
}