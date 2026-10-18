#ifndef SC_LINE_H
#define SC_LINE_H

#include "../sc_common.h"
#include "../sc_obj.h"
#include "../sc_style.h"

void sc_draw_hline_fast(sc_pfb_t *dest, int16_t x1, int16_t x2, int16_t y, uint16_t color, uint8_t alpha);
void sc_draw_vline_fast(sc_pfb_t *dest, int16_t x, int16_t y1, int16_t y2, uint16_t color, uint8_t alpha);
void sc_draw_line_fast_aa(sc_pfb_t *dest, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color,
                          uint8_t alpha);
void sc_draw_line_aa(sc_pfb_t *dest, int x1, int y1, int x2, int y2, int stroke_width, uint16_t color,
                     uint8_t round_cap, uint8_t alpha);

/* ---- Line widget ---- */

/* 端点帽样式：round_cap 取值（0 锯齿 / 1 平帽 / 2 圆帽）。 */
#define SC_LINE_CAP_JAGGED 0U
#define SC_LINE_CAP_FLAT   1U
#define SC_LINE_CAP_ROUND  2U

typedef struct sc_line_t
{
    color_t color;
    int16_t x1; /* start point relative to obj view */
    int16_t y1;
    int16_t x2; /* end point relative to obj view */
    int16_t y2;
    uint8_t stroke_width;
    uint8_t round_cap; /* SC_LINE_CAP_* */
    uint8_t opa; /* opacity (SC_STYLE_OPA) */
} sc_line_t;

#define SC_LINE_NEW(color_, sw_) \
    {.color = (color_), .x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0, \
     .stroke_width = (sw_), .round_cap = SC_LINE_CAP_ROUND, .opa = 255U}

void sc_lcd_draw_line_widget(sc_pfb_t *pfb, int16_t x, int16_t y,
                             uint16_t w, uint16_t h, const sc_line_t *props);

#if SC_USER_OBJ && SC_WIDGET_LINE_ENABLE
sc_obj_t *sc_line_create(sc_obj_t *parent, sc_line_t *props);
void sc_line_set_points(sc_obj_t *obj, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
bool sc_line_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_line_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
