#ifndef SC_OBJ_H
#define SC_OBJ_H

#include "sc_obj_list.h"
#include "sc_keyboard.h"
// 触摸状态枚举
typedef uint8_t sc_touch_state_t;
enum
{
    SC_TOUCH_STATE_IDLE = 0x00,   // 无触摸
    SC_TOUCH_STATE_DOWN = 0x01,   //  触摸按下
    SC_TOUCH_STATE_MOVE = 0x02,   // 触摸移动
    SC_TOUCH_STATE_BOUNCE = 0x04, // 回弹动画
};

typedef struct sc_touch_ctx
{
    sc_obj_t *srceen;    // 根对象
    sc_obj_t *obj_focus; // 当前焦点对象
    void (*anim_cb)(struct sc_touch_ctx *);
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
    CANVAS_MOVE_DISABLE = 0, // 禁用移动
    CANVAS_MOVE_X = 0x01,
    CANVAS_MOVE_Y = 0x02,
    CANVAS_MOVE_XY = 0x03,
    CANVAS_MOVE_CYCLE = 0x08, // 启用循环
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

typedef struct Label_t /// 标签控件
{
    sc_obj_t base;
    color_t color;        // 填充色
    color_t border_color; // 边界色
    sc_label_t Label;
} Label_t;

typedef struct Line_edit_t /// 输入框
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    sc_label_t   Label;
    int32_t pos_x : 12;
    int32_t pos_y : 12;
    int32_t editbuf_size : 8; // 编辑框缓冲区大小
} Line_edit_t;

typedef struct Button_t /// 按钮控件
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    sc_label_t Label;
} Button_t;

typedef struct Keyboard_t /// 标签控件
{
    sc_obj_t base;
    color_t color;        // 填充色
    color_t border_color; // 边界色
    kb_ctx_t kb;
    Line_edit_t *edit;
} Keyboard_t;


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

typedef struct Txtbox_t /// 文本框控件
{
    sc_obj_t base;
    color_t color;
    color_t border_color;
    uint8_t r;
    uint8_t ir;
    const char *text;
} Txtbox_t;



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
} Rotate_t;


sc_obj_t *sc_create_srceen(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align);
sc_obj_t *sc_create_rect(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align);
sc_obj_t *sc_create_canvas(sc_obj_t *parent, int x, int y, int w, int h, int virtual_w, int virtual_h, sc_align_t align);

sc_obj_t *sc_create_label(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align);
void sc_set_label_text(sc_obj_t *obj, char *text);

sc_obj_t *sc_create_line_edit(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align);
void sc_set_edit_text(sc_obj_t *obj, char *text, uint8_t editbuf_size);

/*============================ 2. 强制转换宏 ============================*/

// ui事件分发
void sc_touch_event(sc_event_t *e);
void sc_touch_loop(void);
void sc_touch_demo(void);

#endif // SC_WIDGET_H
