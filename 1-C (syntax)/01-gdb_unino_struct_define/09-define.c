#include <stdio.h>

// 宏
/*
命令行编译保持中间文件
 gcc 05hong.c --save-temps

 .c	.i	.s	.o	a.out
.c源代码
.i经过替换的源代码文件（例如：宏）
.s 由c语言编译器产生的汇编代码文件
a.out 目标代码文件

*/
#define PI 3.1415
#define FORMAT "%f\n"
#define PI2 2 * PI // pi*2
// 当一个宏超过一行，需要加上 "\" 来表示
#define PRT           \
	printf("%f", PI); \
	printf("%f\n", PI2)
#define PRINT printf("++++++++++++++\n")

// 像函数的宏
/*
	带参数的宏的原则
		一切都要括号
		整个值都要括号
		参数出现的每个地方都要括号
*/
#define cube(x) ((x) * (x) * (x))
#define MIN(a,b)((a)>(b)?(b):(a))
//错误示范
#define TEST(x)((x)*1.1111)



int main(void)
{
	printf("%f\n", PI + 1);
	printf(FORMAT, PI);
	printf(FORMAT, PI2 + 1);
	PRT;

	PRINT;
	// c语言中已经存在的宏
	printf("文件名__FILE__:%s\n 行号__LINE__:%d\n 日期__DATE__:%s\n 时间__TIME__:%s\n", __FILE__, __LINE__, __DATE__, __TIME__);

	PRINT;
	printf("函数宏：%d\n", cube(5));
	printf("输入一个数字：");
	int i =0;
	// scanf("%d", &i);
	printf("函数宏：%d\n", cube(i));

	PRINT;
	printf("MIN宏函数:%d",MIN(1,2));



	return 0;
}


