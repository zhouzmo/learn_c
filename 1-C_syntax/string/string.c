


// 实现自定义 strcpy 函数
void mycpy(char *s1, char *s2)
{
    while (*s1++ = *s2++)
        ;
}

// 实现自定义 strlen 函数
int mylen(char *s1)
{
    char *p = s1;
    while (*p++)
        ;
    return p - s1 - 1;
}

// 实现自定义 strcat 函数
void mycat(char *s1, char *s2)
{
    while (*s1++)
        ;
    s1--;
    while (*s1++ = *s2++)
        ;
}

// 实现自定义 atoi 函数
int myatoi(char *str)
{
    if (str == NULL)
    {
        printf("Invalid Input");
        return -1;
    }
    int result = 0;
    int i = 0;
    int sign = 1;
    if (str[0] == '-')
    {
        sign = -1;
        i++;
    }
    for (; str[i] != '\0'; ++i)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            result = result * 10 + (str[i] - '0');
        }
        else
        {
            printf("Invalid Input");
            return -1;
        }
    }
    return sign * result;
}


