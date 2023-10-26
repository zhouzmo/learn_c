#include <stdio.h>



typedef int (*opFunc)(int, int) ; // 自定义函数指针 新类型

int add(int a, int b)
{
	return a + b;
}

int sub(int a, int b)
{
	return a - b;
}

// 功能：通过操作码，选择不同的操作函数,// 函数指针 做返回值
opFunc select(char opcode)
{
	opFunc res = NULL;
	switch (opcode)
	{
	case '+':
		res = add;
		break;
	case '-':
		res = sub;
		break;

	default:
		printf("不存在该操作函数");
		break;
	}

	return res;
}




int run(char opcode, int a, int b)
{
	opFunc fn = select(opcode); // fn是存放返回的函数地址的指针
	return fn(a, b);			// 通过函数指针调用函数
}

int main()
{
	printf("%d\n", run('+', 6, 2));
	printf("%d\n", run('-', 6, 2));
	printf("%d\n", run('*', 6, 2));
	return 0;
}