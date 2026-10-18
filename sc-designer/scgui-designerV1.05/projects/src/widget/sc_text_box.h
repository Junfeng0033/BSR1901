#ifndef SC_WIDGET_TEXT_BOX_H
#define SC_WIDGET_TEXT_BOX_H

#include "../sc_common.h"

#include "../sc_obj.h"

#define SC_TEXT_NEW(text_, fg_, bg_, font_id_, align_) \
    {.text = (text_), .fg = (fg_), .bg = (bg_), .font_id = (font_id_), .align = (align_), \
     .radius = 6U, .bar_w = 6U}
#define SC_TEXT_BOX_NEW(text_, fg_, bg_, font_id_, align_) SC_TEXT_NEW((text_), (fg_), (bg_), (font_id_), (align_))

typedef struct sc_text_t
{
    const char *text;
    area_t text_bbox;
    color_t fg;
    color_t bg;
    int16_t offs_y;
    uint16_t total_lines;
    uint16_t first_vis_offset;
    int16_t first_vis_y;
    uint8_t line_height;
    uint8_t font_id;
    uint8_t radius; /* 圆角半径（同列表） */
    uint8_t bar_w;  /* 滚动条宽，0 = 不显示 */
    sc_align_t align;
} sc_text_t;

void sc_lcd_draw_text_box(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h, const sc_text_t *props);
void sc_draw_text_box(sc_pfb_t *pfb, const sc_text_t *props, rect_t view);

#if SC_USER_OBJ && SC_WIDGET_TEXT_BOX_ENABLE
sc_obj_t *sc_text_box_create(sc_obj_t *parent, sc_text_t *props);
void sc_text_box_sync_text(sc_obj_t *obj, const char *text);
void sc_text_box_scroll(sc_obj_t *obj, int16_t delta_y);
bool sc_text_box_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_text_box_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
