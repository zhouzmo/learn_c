#include <stdio.h>
#define people 380  //定义了宏，宏名是 people(用于替代380) -> 提高 可读性(给数字赋予意义)
                    //底层: 在预处理时 ，只是做简单的替换，未做类型安全检查
                    //思想: 提高 可读性(给数字赋予意义)

#define swap(x,y) x=x+y; y=x-y; x = x-y

#define money 380

int main(void)
{
    printf("有 %d 人 获得奖金 %d\n",people,money);
    return 0;
}	