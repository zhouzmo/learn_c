#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <linux/list.h>



/*
只能内核空间使用

*/

typedef struct Student
{
    int age;
    char *name;
    struct list_head list;
} Stu;

int main(int argc, char *argv[])
{
    Stu *stu;
    stu = malloc(sizeof(*stu));
    int i = 0;
    // 初始化链表头
    struct list_head head;
    INIT_LIST_HEAD(&head);

    for (i = 0; i < 10; i++)
    {
        stu->age = 18;
        stu->name = "zmo";
        // 将stu插入链表
        list_add(&stu->list, &head);
    }

    // 遍历链表
    struct list_head *pos;
    Stu *tmp;
    list_for_each(pos, &head)
    {
        tmp = list_entry(pos, Stu, list);
        printf("Name: %s, Age: %d\n", tmp->name, tmp->age);
    }

    // 释放内存
    free(stu);

    return 0;
}