#include <stdio.h>

/*
语法层面 //把函数名，做参数传入，合适时候回调
思想层面 //执行时机，由对方决定(你是提供者只知道如何处理，但执行的时机，使用者才知道)
 //类比:想吃包子，包子还没好，留下电话号码，让好的时候通知我去吃(事件驱动)
		 //     异步：吃包子的 和做包子的 可独立做自己事情
		 //     阻塞等待: 吃包子的一直等在哪里，不去做自己事情。
		 //场景: 我要显示一图片，但图片需要从网卡读，
		 //    但网卡什么有图片只有网卡知道，让它有的时候回调我（有点类似中断)

*/

// 定义新类型
typedef void (*PFun)(int);

// 找鱼
void findFish(int data)
{
	printf("I find %d fish ~\n", data);
}
// 找肉
void findMeat(int data)
{
	printf("I find %d meat ~\n", data);
}

void eat2(PFun pFun, int num)
{
	pFun(num);
	printf("start eat2 food~\n");
}
void eat(void (*callback)(int), int num)
{
	callback(num);
	printf("start eat food~\n");
}

int main()
{
	printf("回调函数---------------------------\n");
	printf("想吃4条鱼\n");
	eat(findFish, 4);
	printf("想吃3肉\n");
	eat2(findMeat, 3);
}