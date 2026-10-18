#ifndef SC_WIDGET_DROPDOWN_H
#define SC_WIDGET_DROPDOWN_H

#include "../sc_common.h"
#include "../sc_obj.h"

#define SC_DROPDOWN_NEW(items_, cnt_, sel_) \
    {.items = (items_), .cb = NULL, .yofs = 0, .fg = C_WHITE, .bg = C_DARK_GRAY, \
     .sel_bg = C_BLUE, .items_cnt = (cnt_), .selected = (sel_), \
     .expanded = 0, .bar_w = 6U, .radius = 4U, .font_id = SC_FONT_DEFAULT, .list_h = 0}

typedef struct sc_dropdown_t
{
    const char **items;    /* 选项字符串数组（外部持有，不拷贝） */
    sc_obj_cb_t cb;        /* 选中变更回调 */
    int16_t yofs;          /* 列表滚动偏移（像素，向下为正） */
    color_t fg;            /* 文字+边框颜色 */
    color_t bg;            /* 主框+列表背景色 */
    color_t sel_bg;        /* 选中项高亮背景色 */
    uint8_t items_cnt;     /* 选项总数 */
    uint8_t selected;      /* 当前选中索引 */
    uint8_t expanded;      /* 是否展开（0=收起，1=展开） */
    uint8_t bar_w;         /* 滚动条宽度（0=隐藏） */
    uint8_t radius;        /* 圆角半径 */
    uint8_t font_id;       /* 字体ID */
    uint16_t list_h;       /* 展开列表区域高度（0=自动按4条计算） */
} sc_dropdown_t;


#if SC_USER_OBJ && SC_WIDGET_DROPDOWN_ENABLE
sc_obj_t *sc_dropdown_create(sc_obj_t *parent, sc_dropdown_t *props);
void sc_dropdown_set_selected(sc_obj_t *obj, uint8_t idx);
void sc_dropdown_set_items(sc_obj_t *obj, const char **items, uint8_t cnt);
bool sc_dropdown_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_dropdown_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
