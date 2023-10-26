#include "LinkList.h"
#include <string.h>
/*
 *功能：创建单链表
 *参数：
 *	@:无
 *返回值:成功返回单链表节点的首地址，失败返回NULL
 *注：创建链表节点，将指针域指向NULL，并将节点内容清零
 */
usr_info_t* LinkListNodeCreate(void)
{
    usr_info_t* h;

    h = (usr_info_t*)malloc(sizeof(*h));
    if (h == NULL) {
        printf("alloc memory error\n");
        return NULL;
    }
    memset(h, 0, sizeof(*h));
    h->next = NULL;

    return h;
}

/*
 *功能：单链表按照头插法插入数据
 *参数：
 *	@head: 用户信息链表头节点的首地址
 *   @node: 要插入的节点的地址
 *返回值:成功返回0
 */
int LinkListInsertHead(usr_info_t* head, usr_info_t* node)
{
    // 1.将node插入到head中即可
    node->next = head->next;
    head->next = node;

    // 2.成功返回0
    return 0;
}

/*
 *功能：通过Acceptfd查询用户数据
 *参数：
 *	@h:用户信息链表头指针
 *  @acceptfd:accept函数产生的文件描述符
 *返回值:成功返回0，失败返回-1
 */
usr_info_t* LinkListSearchUsrByAcceptfd(usr_info_t* h, int acceptfd)
{
    while (h->next != NULL) {
        if (h->next->acceptfd == acceptfd) {
            return h->next;
        }
        h = h->next;
    }

    printf("用户不存在，查询失败\n");
}
