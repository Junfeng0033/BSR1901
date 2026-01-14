#ifndef SC_OBJ_H
#define SC_OBJ_H

#include "sc_obj_list.h"

// 触摸状态枚举
typedef uint8_t sc_touch_state_t;
enum
{
    SC_TOUCH_STATE_IDLE =   0x00,   // 无触摸
    SC_TOUCH_STATE_DOWN =   0x01,   //  触摸按下
    SC_TOUCH_STATE_MOVE =   0x02,   // 触摸移动
    SC_TOUCH_STATE_BOUNCE = 0x04, // 回弹动画
};

typedef struct sc_touch_ctx
{
    sc_obj_t *srceen;      // 根对象
    sc_obj_t *obj_focus; // 当前焦点对象
    void  (*anim_cb)(struct sc_touch_ctx *);
    int16_t anim_x;   
    int16_t anim_y;
    int16_t last_x;     
    int16_t last_y;   
    uint8_t touch_state; // 触摸状态
} sc_touch_ctx;

extern sc_touch_ctx g_touch;
// -------------------------- 枚举定义 --------------------------
// 循环模式（X/Y轴独立控制）
typedef enum
{
    CANVAS_MOVE_DISABLE = 0,   // 禁用移动
    CANVAS_MOVE_X =      0x01,
    CANVAS_MOVE_Y =      0x02,
    CANVAS_MOVE_XY =     0x03,
    CANVAS_MOVE_CYCLE=   0x08,    // 启用循环
} Canvas_move_t;

// -------------------------- 静态辅助函数 --------------------------
// Canvas控件（继承自基类控件）
typedef struct
{
    sc_obj_t base;            // 基类控件（绝对坐标存储）
    int16_t virtual_w;        // 虚拟画布宽度（2字节）
    int16_t virtual_h;        // 虚拟画布高度（2字节）
    int16_t view_x;           // 虚拟窗口X偏移（2字节）
    int16_t view_y;           // 虚拟窗口Y偏移（2字节） 
    Canvas_move_t cycle_mode; // 模式共用（1字节）
} Canvas_t;

typedef struct
{
    sc_obj_t base;        // 基类控件（绝对坐标存储）
    color_t color;        // 填充色
    color_t border_color; // 边界色
    uint8_t r;
    uint8_t ir;
    int16_t vol;
} Rect_t;

typedef Rect_t Led_t;    /// LED控件
typedef Rect_t Switch_t; /// 开关控件
typedef Rect_t Slider_t; /// 进度条控件

typedef struct Arc_t /// 圆弧
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    int16_t start_deg; // 角度
    int16_t end_deg;   // 角度
    uint8_t dot;       // 端点
} Arc_t;

typedef struct Label_t /// 标签控件
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    const char *text;
} Label_t;

typedef struct Line_edit_t /// 输入框
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    const char text[64];
} Line_edit_t;

typedef struct Txtbox_t /// 文本框控件
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    const char *text;
} Txtbox_t;

typedef struct Button_t /// 按钮控件
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    const char *text;
} Button_t;

typedef struct Menulist_t // 菜单项结构体
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    const char *text;
    uint8_t draw_st;
    uint8_t draw_cnt;
    // SC_Menu  menu;
} Menulist_t;

typedef struct
{
    sc_obj_t base; // 基础控件
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    const char *text; // 文本
    // SC_Keyboard  Keyboard;
} Keyboard_t;

/// 示波器控件
typedef struct
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t xd;
    uint8_t yd;
    // SC_chart  buf[2];       //波形数量
} Chart_t;

/// 图片控件
typedef struct
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    // const SC_img_t *src;
} Image_t;

/// 图片zip
typedef struct
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    // const SC_img_zip *src;
    // SC_dec_zip        dec;
} Imagezip_t;

/// 图片变换
typedef struct
{
    sc_obj_t base;
    int16_t move_x;
    int16_t move_y;
    // const SC_img_t *src;
    // Transform    params;
} Rotate_t;

void sc_handle_event(sc_event_t *e);
bool sc_screen_loop(void);

sc_obj_t *sc_create_srceen(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align);
sc_obj_t *sc_create_canvas(sc_obj_t *parent, int x, int y, int w, int h, int virtual_w, int virtual_h, sc_align_t align);
void sc_set_canvas_mode(sc_obj_t *Canvas, Canvas_move_t mode);
sc_obj_t *sc_create_rect(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align);

void sc_obj_test(void);
/*============================ 2. 强制转换宏 ============================*/
void sc_obj_dirty(sc_obj_t *obj, sc_rect_t *last);

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

/* 设置透明度 */
static inline void sc_set_alpha(void *p, uint8_t alpha)
{
    sc_obj_t *obj = (sc_obj_t *)(p);
    obj->alpha = alpha;
    sc_obj_dirty(obj, NULL);
}

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

#endif // SC_WIDGET_H
