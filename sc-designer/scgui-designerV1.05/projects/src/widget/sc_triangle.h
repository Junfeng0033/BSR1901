#ifndef SC_TRIANGLE_H
#define SC_TRIANGLE_H

#include "../sc_common.h"
#include "../sc_obj.h"
#include "../sc_style.h"

typedef struct sc_triangle_t
{
    color_t color;  /* 边线色（SC_STYLE_FG） */
    color_t fill;   /* 填充色（SC_STYLE_BG；==g_gui->bg 则不填充） */
    int16_t x1;     /* 顶点 1，相对 obj view 原点 */
    int16_t y1;
    int16_t x2;     /* 顶点 2 */
    int16_t y2;
    int16_t x3;     /* 顶点 3 */
    int16_t y3;
    uint8_t opa;    /* 透明度（SC_STYLE_OPA） */
    sc_obj_cb_t cb; /* 点击回调（SC_STYLE_CALLBACK） */
} sc_triangle_t;

#define SC_TRIANGLE_NEW(color_, fill_) \
    {.color = (color_), .fill = (fill_), .x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0, .x3 = 0, .y3 = 0, \
     .opa = 255U, .cb = NULL}

/* 三角形绘制（填充 + 三边 1px AA 描边）。 */
void sc_draw_triangle(sc_pfb_t *dest, int x1, int y1, int x2, int y2, int x3, int y3, color_t color,
                      color_t fill, uint16_t alpha);

#if SC_USER_OBJ && SC_WIDGET_TRIANGLE_ENABLE
sc_obj_t *sc_triangle_create(sc_obj_t *parent, sc_triangle_t *props);
void sc_triangle_set_points(sc_obj_t *obj, int16_t x1, int16_t y1, int16_t x2, int16_t y2,
                            int16_t x3, int16_t y3);
bool sc_triangle_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_triangle_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
