#ifndef SC_WIDGET_PROGRESS_H
#define SC_WIDGET_PROGRESS_H

#include "../sc_common.h"

#include "../sc_obj.h"
#include "../sc_style.h"

#define SC_PROGRESS_MAX 100U
#define SC_PROGRESS_NEW(fill_, border_, value_) {.texture = NULL, .fg = (fill_), .bg = (border_), .value = (value_), .opa = 255U}

typedef struct sc_progress_t
{
    sc_texture_t *texture; /* NULL = 纯色填充；非空时完成区域使用纹理渐变 */
    color_t fg; /* Completed-area fill color. */
    color_t bg; /* Rounded track border color; empty area uses g_gui->bg. */
    sc_obj_cb_t cb;
    uint8_t value; /* Normalized percentage: 0..100. */
    uint8_t opa;
} sc_progress_t;

void sc_lcd_draw_progress(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h, const sc_progress_t *props);

#if SC_USER_OBJ && SC_WIDGET_PROGRESS_ENABLE
sc_obj_t *sc_progress_create(sc_obj_t *parent, sc_progress_t *props);
void sc_progress_set_value(sc_obj_t *obj, uint16_t value);
bool sc_progress_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_progress_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
