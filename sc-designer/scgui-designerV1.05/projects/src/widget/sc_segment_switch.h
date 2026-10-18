#ifndef SC_WIDGET_SEGMENT_SWITCH_H
#define SC_WIDGET_SEGMENT_SWITCH_H

#include "../sc_common.h"
#include "../sc_obj.h"

/* 单段配置：标签文字 + 选中时的背景色/文字色，支持每档不同样式（如绿/黄/红）。 */
typedef struct sc_segment_item_t
{
    const char *label;        /* 段标签文字（如 "L1"、"ON"） */
    color_t selected_color;   /* 该段选中时的背景色 */
    color_t selected_text_color; /* 该段选中时的文字色 */
} sc_segment_item_t;

#define SC_SEGMENT_SWITCH_NEW(items_, count_, selected_) \
    {.items = (items_), .seg_count = (count_), .selected = (selected_), \
     .track_color = 0x8410U, .text_color = 0x0000U, \
     .radius = 6U, .border = 0U, .font_id = 0U, .cb = NULL, \
     .vis_selected = (selected_), .anim_x = -1}

typedef struct sc_segment_switch_t
{
    const sc_segment_item_t *items; /* 段配置数组（label + selected_color + selected_text_color） */
    sc_obj_cb_t cb;                 /* 状态变化回调 */
    color_t track_color;            /* 轨道背景色（未选中） */
    color_t text_color;             /* 未选中文字颜色（全局） */
    uint8_t seg_count;              /* 段数（2~8） */
    uint8_t selected;               /* 当前选中段索引 */
    uint8_t radius;                 /* 圆角（0=胶囊形） */
    uint8_t border;                 /* 边框宽度 */
    uint8_t font_id;                /* 字体ID */
    uint8_t vis_selected;           /* 视觉选中段（动画最后一帧才更新，用于文字颜色） */
    int16_t anim_x;                 /* 选中色块当前 x 偏移（动画用，-1=未初始化） */
} sc_segment_switch_t;

void sc_lcd_draw_segment_switch(sc_pfb_t *pfb, int16_t x, int16_t y,
                                 uint16_t w, uint16_t h,
                                 const sc_segment_switch_t *props, int border);

#if SC_USER_OBJ && SC_WIDGET_SEGMENT_SWITCH_ENABLE
sc_obj_t *sc_segment_switch_create(sc_obj_t *parent, sc_segment_switch_t *props);
void sc_segment_switch_set_items(sc_obj_t *obj, const sc_segment_item_t *items, uint8_t count);
void sc_segment_switch_set_selected(sc_obj_t *obj, uint8_t index);
bool sc_segment_switch_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_segment_switch_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
