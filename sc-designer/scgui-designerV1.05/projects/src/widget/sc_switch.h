#ifndef SC_WIDGET_SWITCH_H
#define SC_WIDGET_SWITCH_H

#include "../sc_common.h"

#include "../sc_obj.h"

#define SC_SWITCH_NEW(knob_, off_, on_, checked_) \
    {.knob_color = (knob_), .track_off = (off_), .track_on = (on_), .checked = (checked_)}

typedef struct sc_switch_t
{
    color_t knob_color; /* 柄 + 边框色 */
    color_t track_off;  /* 关闭时轨道底色 */
    color_t track_on;   /* 开启时轨道底色 */
    sc_obj_cb_t cb;
    uint8_t radius;
    uint8_t border;
    uint8_t checked;
} sc_switch_t;

void sc_lcd_draw_switch(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h, const sc_switch_t *props, int border);

#if SC_USER_OBJ && SC_WIDGET_SWITCH_ENABLE
sc_obj_t *sc_switch_create(sc_obj_t *parent, sc_switch_t *props);
void sc_switch_set_checked(sc_obj_t *obj, bool checked);
bool sc_switch_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_switch_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
