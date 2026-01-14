
#include "sc_obj_list.h"

// 宏定义：判断是否为当前节点或其子节点
#define IS_CHILD_OR_SELF(parent, child) \
    (child != NULL && (child->level > parent->level || child == parent))

///添加节点到链表
///单向链表排序按level排序，相当于深度优先树链表
int sc_obj_list_add(sc_obj_t *parent, sc_obj_t *obj)
{
    if (obj == NULL)   return 0;
    sc_obj_t *prev =    parent;   // 初始化前驱为parent
    sc_obj_t *tail =    obj;      // 子树尾部
    sc_obj_t *child =   parent;   // 从parent开始遍历
    while (IS_CHILD_OR_SELF(parent, child))
    {
        if (child == obj) return 0;   // 检查是否已存在相同的节点，
        prev = child;                 // 保存前驱
        child = child->next;
    }
    ///---------如果为子树-------
    child = obj->next;
    while (child != NULL && child->level > obj->level)
    {
        //整个子树(A4->B5->C6)重新挂到前驱节点(root0)后面,offse=3  (root0)->(A1->B2->C3)
        child->level = child->level+(prev->level+1)-obj->level;  
        tail = child;
        child = child->next;
    }
    obj->level = parent->level + 1; //设置新节点的level
    tail->next = prev->next;        //连接前驱节点后面
    prev->next = obj;               //将新节点插入到前驱节点后面
    return 1;
}
/// 创建新节点并加入
sc_obj_t *sc_obj_init(sc_obj_t *parent, sc_obj_t *obj, uint8_t type)
{
    if (obj == NULL) return NULL; //创建失败
    obj->type = type;
    obj->level = 0;  
    obj->flag = 0;  
    obj->attr = 0;  
    obj->alpha= 255;
    obj->sc_user_cb = NULL;
    if (parent == NULL)
    {
        obj->parent = obj; // 指向自己保证(obj->parent != NULL)
        obj->next = NULL;  // 为根节点才清空next，obj可以是一个子树
    }
    else
    {
        obj->parent = parent;
        sc_obj_list_add(parent, obj); //加入到键表
    }
    return obj;
}

static const char *obj_name[] = {
    #define OBJECT_TYPE_ENTRY(id, name) [id] = name,
    OBJECT_TYPE_DEFINITIONS
    #undef OBJECT_TYPE_ENTRY // 立即取消定义
};

/// 遍历树结构
void sc_obj_list_print(sc_obj_t *root)
{
    if (root == NULL)   return;
    int obj_cnt = 0;
    sc_obj_t *current = root;
    while (IS_CHILD_OR_SELF(root, current))
    {
        // 打印缩进和节点数据
        for (int i = 0; i < current->level; i++)
        {
            printf("  ");
        }
        const char *name = (current->type < sizeof(obj_name)/sizeof(obj_name[0])) ? obj_name[current->type] : "UNKNOWN";
        printf("|__%s (%d,%d,%d,%d)\n", name ,current->rect.x, current->rect.y, current->rect.w, current->rect.h); // 打印节点数据
        current = current->next;
        obj_cnt++;
    }
    printf("obj_cnt=%d\n", obj_cnt);
}
/*直接跳到下一棵树*/
 sc_obj_t *sc_obj_next_tree(sc_obj_t *cur)
{
    int base = cur->level;
    while (cur->next && cur->next->level > base)
    {
        cur = cur->next;
    }
    return cur->next; /* 可能 NULL，调用方判空即可 */
}
//上一个节点
 sc_obj_t *sc_obj_prev(sc_obj_t *target)
{
    sc_obj_t *parent = target->parent;   // 取出已知 parent
    sc_obj_t *prev = parent;
    for (sc_obj_t *p = parent->next; p && p != target; p = p->next) {
        if (p->level <= parent->level) break;   // 子树段结束
        prev = p;
    }
    return prev;   // target 的前一个节点
}





