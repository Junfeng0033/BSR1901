
#ifndef SC_OBJ_LAST_H
#define SC_OBJ_LAST_H

#include "sc_gui.h"

/*  控件类型枚举 */
#define OBJECT_TYPE_DEFINITIONS                         \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_SCREEN, "SCREEN")     \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_CANVAS, "CANVAS")     \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_RECT, "RECT")         \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_SLIDER, "SLIDER")     \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_ARC, "ARC")           \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_SC_LED, "SC_LED")     \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_SWITCH, "SWITCH")     \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_TEXTBOX, "TEXTBOX")   \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_BUTTON, "BUTTON")     \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_LABEL, "LABEL")       \
    /*----------------基础控件分界--------*/            \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_BAES_END, "BAES_END") \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_CHART, "CHART")       \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_IMAGE, "IMAGE")       \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_IMAGEZIP, "IMAGEZIP") \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_ROTATE, "ROTATE")     \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_MENU, "MENU")         \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_KEYBOARD, "KEYBOARD")

/* 展开 OBJECT_TYPE_DEFINITIONS 来定义枚举  */
typedef uint8_t sc_obj_type_t;
enum
{
#define OBJECT_TYPE_ENTRY(id, name) id,
    OBJECT_TYPE_DEFINITIONS
#undef OBJECT_TYPE_ENTRY // 立即取消定义，防止污染
        SC_OBJ_TYPE_MAX, // 自动计算枚举的总数
};

/* 控件属性枚举 */
typedef uint8_t sc_obj_Attr_t;
enum
{
    SC_OBJ_ATTR_BOOL =   0x01,   // 布尔属性
    SC_OBJ_ATTR_DFOCUS = 0x02,   // 禁用焦点
    SC_OBJ_ATTR_VIRTUAL = 0x04,  // 虚拟控件容器
    SC_OBJ_ATTR_HIDDEN =  0x08,  // 隐藏属性
};

/* 控件标志枚举 */
typedef uint8_t sc_obj_flag_t;
enum
{
    SC_OBJ_FLAG_ACTIVE =   0x01,   // 脏矩阵标志
    SC_OBJ_FLAG_PRESSED = 0x02,    // 按下标志
    SC_OBJ_FLAG_FOCUS =   0x04,    // 焦点标志
};
typedef struct sc_obj_t
{
    struct sc_obj_t *next;                                                // 指向下一个控件的指针
    struct sc_obj_t *parent;                                              // 父控件
    void (*handle_draw)(struct  sc_obj_t *obj, sc_pfb_t *dest);           // 绘制回调
    bool (*handle_event)(struct sc_obj_t *obj, sc_event_t *e);            // 系统事件
    void (*sc_user_cb)(struct   sc_obj_t *obj, sc_event_t *e);            // 用户回调
    sc_rect_t rect;                                                       // 矩形区域
    uint8_t type : 5;                                                     // 类型
    uint8_t level : 3;                                                    // 层级
    uint8_t attr;                                                          // 属性
    uint8_t flag;                                                          // 状态
    uint8_t alpha;                                                         // 透明度
} sc_obj_t;

/*宏定义：判断是否为当前节点或其子节点*/
#define IS_CHILD_OR_SELF(parent, child) \
    (child != NULL && (child->level > parent->level || child == parent))

/*添加节点到树链表*/
int sc_obj_list_add(sc_obj_t *parent, sc_obj_t *obj);

/*创建新节点并加入*/
sc_obj_t *sc_obj_init(sc_obj_t *parent, sc_obj_t *obj, uint8_t type);

/*直接跳到下一棵树*/
sc_obj_t *sc_obj_next_tree(sc_obj_t *cur);

/*获取当前节点的下一个或上一个节点*/
sc_obj_t *sc_obj_prev(sc_obj_t *target);

/*打印树结构*/
void sc_obj_list_print(sc_obj_t *tree);

#endif
