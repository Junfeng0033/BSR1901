#ifndef SC_ARC_H
#define SC_ARC_H

#include "../sc_common.h"
#include "../sc_obj.h"
#include "../sc_style.h"

#define SC_ARC_NEW(start_c_, end_c_, fill_, sw_) \
    {.start_c = (start_c_), .end_c = (end_c_), .fill = (fill_), \
     .start_deg = 60, .end_deg = 300, .bar_deg = 120, \
     .stroke_width = (uint8_t)(sw_), .opa = 255U, .dot = 1U, .is_bar = 0U, .cb = NULL}

typedef struct sc_arc_t
{
    color_t start_c;
    color_t end_c;
    color_t fill;
    int16_t start_deg;
    int16_t end_deg;
    int16_t bar_deg;
    sc_obj_cb_t cb;
    uint8_t stroke_width;
    uint8_t opa; /* opacity (SC_STYLE_OPA) */
    uint8_t dot;
    uint8_t is_bar;
} sc_arc_t;

/* Normalize degrees to [0, 360); see sc_norm_deg in sc_common.h. */
#define sc_arc_norm_deg(deg) sc_norm_deg(deg)

uint8_t sc_atan2_alpha(int16_t dx, int16_t dy);

void sc_lcd_draw_arc(sc_pfb_t *pfb, int16_t cx, int16_t cy, int r, const sc_arc_t *props);

#if SC_USER_OBJ && SC_WIDGET_ARC_ENABLE

area_t sc_calc_arc_dirty(int16_t cx, int16_t cy, int16_t r, int16_t ir, int16_t start, int16_t end);
void sc_arc_invalidate(sc_obj_t *obj, const area_t *dirty);
sc_obj_t *sc_arc_create(sc_obj_t *parent, sc_arc_t *props);

void sc_arc_set_start_deg(sc_obj_t *obj, int start_deg);
void sc_arc_set_end_deg(sc_obj_t *obj, int end_deg);
void sc_arc_set_bar_deg(sc_obj_t *obj, int bar_deg);
void sc_arc_set_range(sc_obj_t *obj, int start_deg, int end_deg);
bool sc_arc_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_arc_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
