#ifndef SC_DROPDOWN_CARD_H
#define SC_DROPDOWN_CARD_H

#include "sc_gui.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 下拉卡片控件：
 * - 独立类型 SC_OBJ_DROPDOWN_CARD，绘制复用 sc_draw_Rounded_texture
 * - create 时自动注册 screen 回调捕捉下拉/上拉手势
 * - 链式调用 screen 原回调，不覆盖用户已有逻辑
 * - 初始化在屏内（方便上位机布局），运行时可调用 show(false) 收起
 * - 宽高从 obj->view 读取，结构体不重复定义
 */

#define SC_DROPDOWN_CARD_THRESHOLD 40         /* 手势触发阈值（px） */


typedef struct sc_dropdown_card_t
{
    /* 样式（与 sc_plain_rect_t 同序，绘制代码可共用） */
    color_t color;       /* 边框色 */
    color_t fill;        /* 填充色 */
    uint8_t radius;
    uint8_t border;      /* 0 = 实心 */
    uint8_t alpha;
    uint8_t rect_mask;   /* 四角直角掩码 */
    /* 运行时状态 */
    sc_obj_t *screen;    /* 绑定的 screen（注册回调用） */
    sc_obj_cb_t orig_cb; /* 原 screen 回调（链式调用） */
    int16_t target_y;    /* 手势起点卡片 y（UP 按位移方向判定目标，同容器 target） */
    int16_t shown_y;     /* 显示位置 y（closed 时自动保存当前位置） */
} sc_dropdown_card_t;

#if SC_USER_OBJ && SC_WIDGET_DROPDOWN_CARD_ENABLE
/* 创建下拉卡片并绑定 screen。
 * - create 后需调用 sc_obj_set_rect 设置位置和大小
 * - 自动注册 screen 回调捕捉手势，链式调用原回调
 * 返回卡片对象指针，可用于往卡片里添加子控件。 */
sc_obj_t *sc_dropdown_card_create(sc_obj_t *screen, sc_dropdown_card_t *ctrl);

/* 展开/收起卡片。
 * - show=true: 展开到 shown_y；show=false: 收起至 -view.h
 * - animated=true: 用动画移动；animated=false: 直接设置位置（子控件一起移动）
 * 收起时自动保存当前位置为 shown_y。 */
void sc_dropdown_card_show(sc_obj_t *obj, bool show, bool animated);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SC_DROPDOWN_CARD_H */
