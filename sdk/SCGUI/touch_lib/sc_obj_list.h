
#ifndef SC_OBJ_LAST_H
#define SC_OBJ_LAST_H

#include "sc_gui.h"

/*  控件类型枚举 */
#define OBJECT_TYPE_DEFINITIONS                           \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_SCREEN, "SCREEN")       \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_CANVAS, "CANVAS")       \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_RECT, "RECT")           \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_SLIDER, "SLIDER")       \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_ARC, "ARC")             \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_SC_LED, "SC_LED")       \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_SWITCH, "SWITCH")       \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_TEXTBOX, "TEXTBOX")     \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_BUTTON, "BUTTON")       \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_LABEL, "LABEL")         \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_LINE_EDIT, "LINE_EDIT") \
    /*----------------基础控件分界--------*/              \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_BAES_END, "BAES_END")   \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_CHART, "CHART")         \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_IMAGE, "IMAGE")         \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_IMAGEZIP, "IMAGEZIP")   \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_ROTATE, "ROTATE")       \
    OBJECT_TYPE_ENTRY(SC_OBJ_TYPE_MENU, "MENU")           \
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
    SC_OBJ_ATTR_BOOL = 0x01,    // 布尔属性
    SC_OBJ_ATTR_DFOCUS = 0x02,  // 禁用焦点
    SC_OBJ_ATTR_VIRTUAL = 0x04, // 虚拟控件
    SC_OBJ_ATTR_HIDDEN = 0x08,  // 隐藏属性
};

/* 控件标志枚举 */
typedef uint8_t sc_obj_flag_t;
enum
{
    SC_OBJ_FLAG_ACTIVE = 0x01,  // 活动标志
    SC_OBJ_FLAG_PRESSED = 0x02, // 按下标志
    SC_OBJ_FLAG_FOCUS = 0x04,   // 焦点标志
};

/* 控件结构体 */
typedef struct sc_obj_t
{
    struct sc_obj_t *next;                                     // 指向下一个控件的指针
    struct sc_obj_t *parent;                                   // 父控件
    bool (*handle_event)(struct sc_obj_t *obj, sc_event_t *e); // 系统事件
    void (*sc_user_cb)(struct sc_obj_t *obj, sc_event_t *e);   // 用户回调
    sc_rect_t rect;                                            // 矩形区域
    uint8_t type : 5;                                          // 类型
    uint8_t level : 3;                                         // 层级
    uint8_t attr;                                              // 属性
    uint8_t flag;                                              // 状态
    uint8_t alpha;                                             // 透明度
} sc_obj_t;

typedef struct
{
    sc_obj_t base;        // 基类控件（绝对坐标存储）
    color_t color;        // 填充色
    color_t border_color; // 边界色
    uint8_t r;
    uint8_t ir;
    int16_t vol;
} Rect_t;

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

/// 标记控件为脏矩形
static inline void sc_obj_dirty(sc_obj_t *obj, sc_rect_t *last)
{
    if (obj == NULL || obj->attr & SC_OBJ_ATTR_VIRTUAL)
        return;
    if (last == NULL && (obj->flag & SC_OBJ_FLAG_ACTIVE)) // 无上次位置，且未标记过
    {
        return;
    }
    sc_rect_t rect = last ? sc_rect_merge(&obj->rect, last) : obj->rect;   // 合并上次位置
    sc_rect_t *parent = (obj->parent != NULL) ? &obj->parent->rect : NULL; // 获取父节点
    sc_dirty_mark(parent, &rect);
    obj->flag |= SC_OBJ_FLAG_ACTIVE; // 标记刷新
}

/// 删除控件
static inline void sc_obj_del(sc_obj_t *obj)
{
    sc_obj_t *prev = sc_obj_prev(obj); // 前驱节点
    sc_obj_t *current = obj;           // 从obj开始遍历
    sc_obj_dirty(obj, NULL);           // 标记脏矩形
    while (IS_CHILD_OR_SELF(obj, current))
    {
        sc_obj_t *to_free = current;
        current = current->next;
        free(to_free); // 释放节点
    }
    if (prev)
    {
        prev->next = current;
    }
}
/// 隐藏控件
static inline void sc_obj_hidden(sc_obj_t *obj)
{
    sc_obj_t *child = obj;
    while (IS_CHILD_OR_SELF(obj, child))
    {
        child->attr |= SC_OBJ_ATTR_HIDDEN; // 标记隐藏
        sc_obj_dirty(child, NULL); // 标记脏矩形
        child = child->next;
    }
}

/* 设置大小 */
static inline void sc_set_x_y(void *p, int16_t x, int16_t y)
{
    sc_obj_t *obj = (sc_obj_t *)(p);
    sc_rect_t last = obj->rect;
    obj->rect.x = x;
    obj->rect.y = y;
    sc_obj_dirty(obj, &last);
}
/* 设置位置 */
static inline void sc_set_w_h(void *p, int16_t w, int16_t h)
{
    sc_obj_t *obj = (sc_obj_t *)(p);
    sc_rect_t last = obj->rect;
    obj->rect.w = h;
    obj->rect.h = h;
    sc_obj_dirty(obj, &last);
}

/// 设置位置
static inline void sc_obj_set_geometry(sc_obj_t *obj, int x, int y, int w, int h, sc_align_t align)
{
    if (obj == NULL)
        return;
    if (obj->parent != obj)
    {
        sc_parent_align(&obj->parent->rect, &x, &y, w, h, align);
    }
    obj->rect.x = x; // 相对坐标
    obj->rect.y = y;
    obj->rect.w = w;
    obj->rect.h = h;
    sc_obj_dirty(obj, NULL);
}

/// 移动控件子控件位置
static inline void sc_obj_move_srceen(sc_obj_t *obj, int move_x, int move_y)
{
    if (move_x == 0 && move_y == 0)
        return;
    sc_obj_t *child = obj->next;
    while (IS_CHILD_OR_SELF(obj, child))
    {
        sc_rect_t last = child->rect;
        child->rect.x += move_x; // 相对坐标
        child->rect.y += move_y;
        sc_obj_dirty(child, &last); // 标记脏矩形
        child = child->next;
    }
}
/* 设置透明度 */
static inline void sc_set_alpha(void *p, uint8_t alpha)
{
    sc_obj_t *obj = (sc_obj_t *)(p);
    obj->alpha = alpha;
    sc_obj_dirty(obj, NULL);
}
/*设置色彩*/
static inline void sc_set_color(void *p, color_t color, color_t border_color)
{
    Rect_t *obj = (Rect_t *)(p);
    if (obj->base.type > SC_OBJ_TYPE_CANVAS && obj->base.type >= SC_OBJ_TYPE_BAES_END)
        return; // 如果没有这个属性，直接返回
    obj->color = color;
    obj->border_color = border_color;
    sc_obj_dirty(&obj->base, NULL); // 标记刷新
}

static inline void sc_set_user_cb(void *p, void (*sc_user_cb)(struct sc_obj_t *obj, sc_event_t *e))
{
    sc_obj_t *obj = (sc_obj_t *)(p);
    obj->sc_user_cb = sc_user_cb;
}

/*============================ 4. 批量关键字宏 ============================*/
/* 关键字→(函数,参数个数,...) */
#define _POS(x, y) (sc_set_xy, 2, x, y)
#define _SIZE(w, h) (sc_set_wh, 2, w, h)
#define _VAL(v) (sc_set_val, 1, v)
#define _COLOR(c) (sc_set_color, 1, c)
#define _STATE(s) (sc_set_state, 1, s)
#define _DRAW(cb) (sc_set_drawcb, 1, cb)
#define _TXT(txt) (sc_set_text, 1, txt)
#define _FONT(sz) (sc_set_font, 1, sz)
/* 调用生成 */
#define _CALL_1(f, a1) f(obj, a1);
#define _CALL_2(f, a1, a2) f(obj, a1, a2);
#define _CALL_N(n, f, ...) _CALL_##n(f, __VA_ARGS__)
#define _APPLY(item) _CALL_N item /* item 已展开成 (func,n,...) */

/* 手动展开 1~10 个关键字项 */
#define _AE(...) \
    _AE_N(__VA_ARGS__, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _AE_N(_10, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) _DO_##N(_10, _9, _8, _7, _6, _5, _4, _3, _2, _1)
#define _DO_1(a1) _APPLY(a1)
#define _DO_2(a1, a2) _APPLY(a1) _APPLY(a2)
#define _DO_3(a1, a2, a3) _APPLY(a1) _APPLY(a2) _APPLY(a3)
#define _DO_4(a1, a2, a3, a4) _APPLY(a1) _APPLY(a2) _APPLY(a3) _APPLY(a4)
#define _DO_5(a1, a2, a3, a4, a5) _APPLY(a1) _APPLY(a2) _APPLY(a3) _APPLY(a4) _APPLY(a5)
/* 如需更多继续照写 _DO_6... */

/* 用户入口 */
#define set_all(obj, ...) \
    do                    \
    {                     \
        void *_o = (obj); \
        (void)_o;         \
        void *obj = _o;   \
        _AE(__VA_ARGS__)  \
    } while (0)

#endif
