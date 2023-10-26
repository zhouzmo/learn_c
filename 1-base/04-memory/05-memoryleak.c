#include <stdio.h>
struct birth
{
    int year;
    int month;
    int day;
};

struct student
{
    char is_male;
    char *name;
    struct birth *bi;
};

// 有借不还: malloc申请空间, 未配对free释放空间。
// 危害：不停分配，但结束才释放，运行几天后，导致耗尽内存 变慢 崩溃重启

int main(int argc, char const *argv[])
{

    printf("未配对----------------------------------\n");
    char *pt = (char *)malloc(10); // 堆上申请空间，未配对free(pt)

    printf("丢失地址----------------------------------\n");
    char *pt1 = (char *)malloc(10);
    pt1 = (char *)malloc(20); // 覆盖了指针，导致前面10个空间的地址丢失。

    printf("未分级释放----------------------------------\n");
    char *pt1 = (char *)malloc(10);
    struct student *stu;//定义为指针，即在栈上只存在一个 8（32为4）字节得指针地址，节省栈空间
    stu = (struct student *)malloc(sizeof(struct student)); // 堆上申请空间

    stu->is_male = 1;
    stu->name = "wangwei";
    stu->bi = (struct birth *)malloc(sizeof(struct birth)); // 堆上申请空间
    stu->bi->year = 2000;
    stu->bi->month = 3;
    stu->bi->day = 2;

    printf("%s %d \n", stu->name, stu->bi->day);

    // 逐级释放空间，避免内存泄漏
    // pt->name 是字符串常量 不用释放
    if (stu->bi != NULL)
    {
        free(stu->bi); // 先释放子空间
        stu->bi = NULL;
    }
    free(stu);  // 后释放父空间
    stu = NULL; // 避免野指针 (操作已释放的空间)

    return 0;
}
