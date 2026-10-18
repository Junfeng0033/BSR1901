#ifndef SC_WIDGET_LIST_ITEMS_H
#define SC_WIDGET_LIST_ITEMS_H

#include "../sc_common.h"
#include "../sc_obj.h"

/* step == 0: display-only row (name only, or name + fixed value/unit). */
#define SC_LIST_ITEM(name_, unit_, val_, min_, max_, step_) \
    {.name = (name_), .unit = (unit_), .value = (val_), .min = (min_), \
     .max = (max_), .step = (step_)}
#define SC_LIST_ITEM_LABEL(name_) SC_LIST_ITEM((name_), NULL, 0, 0, 0, 0)

typedef struct sc_list_item_t
{
    const char *name;
    const char *unit;
    int16_t value;
    int16_t min;
    int16_t max;
    int16_t step;
} sc_list_item_t;

/* 滚动范围：上下各半个 pitch 弹性空间 */
static inline void sc_scroll_range(int content_h, int view_h, int pitch,
                                   int16_t *out_min, int16_t *out_max)
{
    int half = pitch / 2;
    int min_y = view_h - content_h - half;
    if (min_y > -half) min_y = -half; /* 内容不溢出时，下限钳制为 -half */
    *out_min = (int16_t)min_y;
    *out_max = (int16_t)half;
}

/* 列表滚动：确保 current 条目内容(item_h)可见，自动调整 yofs（list_items/dropdown/
 * list_menu 共用）。item_h = 条目内容高（不含行间 gap）；最后一个条目没有 gap，
 * 内容总高 = (total_cnt-1)*pitch + item_h。行填满 pitch 的调用方传 item_h=pitch。 */
static inline void sc_list_ensure_visible(int16_t *yofs, int pitch, int item_h, int view_h,
                                          int total_cnt, int current)
{
    if (pitch <= 0 || view_h <= 0 || total_cnt == 0)
    {
        return;
    }
    /* 像素位置判断：只有条目内容(item_h)完全移出 view 才滚动对齐 */
    int item_top = current * pitch + *yofs;
    int item_bottom = item_top + item_h;
    if (item_top < 0)
    {
        /* 选中项在上方，顶部对齐 view 顶部 */
        *yofs = (int16_t)(-current * pitch);
    }
    else if (item_bottom > view_h)
    {
        /* 选中项在下方，内容底对齐 view 底部 */
        *yofs = (int16_t)(view_h - current * pitch - item_h);
    }
    /* 钳制到滚动范围（末条无 gap） */
    int total_h = (total_cnt - 1) * pitch + item_h;
    int16_t min_yofs, max_yofs;
    sc_scroll_range(total_h, view_h, pitch, &min_yofs, &max_yofs);
    if (min_yofs > 0)
    {
        min_yofs = 0;
    }
    *yofs = (int16_t)SC_CLAMP(*yofs, min_yofs, max_yofs);
}

#if SC_USER_OBJ && SC_WIDGET_LIST_ITEMS_ENABLE
void sc_list_items_default_cb(sc_obj_t *obj, const sc_event_t *event);
#define SC_LIST_ITEMS_CB_INIT sc_list_items_default_cb
#else
#define SC_LIST_ITEMS_CB_INIT NULL
#endif

#define SC_LIST_ITEMS_NEW(items_, cnt_, fg_, bg_, sel_bg_) \
    {.items = (items_), .cb = SC_LIST_ITEMS_CB_INIT, .yofs = 0, .fg = (fg_), .bg = (bg_), \
     .sel_bg = (sel_bg_), .items_cnt = (cnt_), .cursor = 0U, .item_gap = 2U, \
     .bar_w = 6U, .radius = 6U, .font_id = 0U, .align = ALIGN_LEFT, .setting = false}

typedef struct sc_list_items_t
{
    sc_list_item_t *items;
    sc_obj_cb_t cb; /* default: Enter enter/exit setting; UP/DOWN adjust while setting */
    int16_t yofs;
    color_t fg;
    color_t bg;
    color_t sel_bg;
    uint8_t items_cnt;
    uint8_t cursor;
    uint8_t item_gap;
    uint8_t bar_w;
    uint8_t radius;
    uint8_t font_id;
    sc_align_t align;
    bool setting; /* true: editing current item (value drawn red) */
} sc_list_items_t;

void sc_lcd_draw_list_items(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h,
                            const sc_list_items_t *props);

#if SC_USER_OBJ && SC_WIDGET_LIST_ITEMS_ENABLE
sc_obj_t *sc_list_items_create(sc_obj_t *parent, sc_list_items_t *props);
void sc_list_items_set_data(sc_obj_t *obj, sc_list_item_t *items, uint8_t items_cnt);
void sc_list_items_set_cursor(sc_obj_t *obj, uint8_t cursor);
bool sc_list_items_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_list_items_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
