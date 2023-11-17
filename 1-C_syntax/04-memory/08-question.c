#include <stdio.h>
#include <string.h>
#define PASS_WORD "1234567" 


/*
使用栈攻击，不输入密码
*/
int verify_password(char * password)
{
    int authentitated;
    char buffer[8];
    authentitated = strcmp(password,PASS_WORD);
    strcpy(buffer,password);
    return authentitated;
}

int main()
{
    int valid_flag = 0;
    char password[1024] = {0};
    while (1)
    {
        printf("please input password:");
        scanf("%s",password);
        valid_flag = verify_password(password);
        if(valid_flag)
        {
            printf("incorrect password!\r\n");
        }
        else
        {
            printf("Congratulation ! you have passed the verification !\r\n");
        }
    }
    return 0;
}