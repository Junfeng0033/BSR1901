#ifndef SC_WIDGET_BUTTON_H
#define SC_WIDGET_BUTTON_H

#include "../sc_common.h"

#include "../sc_obj.h"
#include "../sc_style.h"

#define SC_BUTTON_NEW(text_, text_c_, bg_, font_id_, align_) \
    {.text = (text_), .texture = NULL, .text_c = (text_c_), .bg = (bg_), .font_id = (font_id_), .align = (align_), .radius = 6U, .border = 1U, .opa = 255U, .rect_mask = 0U}

typedef struct sc_button_t
{
    const char *text;
    sc_texture_t *texture; /* NULL = solid fill via Rounded path */
    color_t text_c;        /* text + border color */
    color_t bg;            /* fill color */
    sc_obj_cb_t cb;
    uint8_t font_id;
    sc_align_t align;
    uint8_t radius;
    uint8_t border; /* stroke width */
    uint8_t opa;
    uint8_t rect_mask; /* per-corner square mask, same as sc_rounded_t */
} sc_button_t;

/* PFB-only path; measures text against the full view each draw.
 * pressed != 0 draws into a 1px-inset temporary view (works with textures). */
void sc_pfb_draw_button(sc_pfb_t *pfb, const sc_button_t *props, rect_t view, int pressed);
void sc_lcd_draw_button(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h,
                        const sc_button_t *props, int pressed);

#if SC_USER_OBJ && SC_WIDGET_BUTTON_ENABLE
sc_obj_t *sc_button_create(sc_obj_t *parent, sc_button_t *props);
bool sc_button_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_button_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
