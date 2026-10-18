#ifndef SC_WIDGET_LINE_EDIT_H
#define SC_WIDGET_LINE_EDIT_H

#include "../sc_common.h"
#include "../sc_obj.h"

#if SC_USER_OBJ && SC_WIDGET_LINE_EDIT_ENABLE

#define SC_LINE_EDIT_NEW(text_, fg_, font_id_, opa_, align_) \
    {.text = (text_), .fg = (fg_), .offs_x = 0, .offs_y = 0, \
     .font_id = (font_id_), .opa = (opa_), .align = (align_), .x_gap = 0U}

typedef struct sc_line_edit_t
{
    const char *text;
    color_t fg;
    int16_t offs_x;
    int16_t offs_y;
    uint16_t text_w;
    uint16_t text_h;
    uint16_t capacity;
    uint16_t cursor;
    sc_obj_t *keyboard;
    sc_obj_cb_t cb;
    uint8_t font_id;
    uint8_t opa;
    uint8_t x_gap;
    sc_align_t align;
} sc_line_edit_t;

void sc_lcd_draw_line_edit(sc_pfb_t *pfb,
                           int16_t x,
                           int16_t y,
                           uint16_t w,
                           uint16_t h,
                           const sc_line_edit_t *props);

sc_obj_t *sc_line_edit_create(sc_obj_t *parent, sc_line_edit_t *props);
/* Bind a caller-owned NUL-terminated buffer. Capacity includes the trailing NUL. */
void sc_line_edit_set_buffer(sc_obj_t *obj, char *buffer, uint16_t capacity);
void sc_line_edit_set_x_gap(sc_obj_t *obj, uint8_t x_gap);
bool sc_line_edit_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_line_edit_event(sc_obj_t *obj, const sc_event_t *event);

#endif /* SC_USER_OBJ && SC_WIDGET_LINE_EDIT_ENABLE */

#endif
