#ifndef SC_WIDGET_LABEL_H
#define SC_WIDGET_LABEL_H

#include "../sc_common.h"

#include "../sc_obj.h"

#define SC_LABEL_NEW(text_, fg_, font_id_, opa_) \
    {.text = (text_),  .fg = (fg_), .font_id = (font_id_), .opa = (opa_), .x_gap = 0U, .align = ALIGN_CENTER, .x_ofs = 0, .texture = NULL}

#define SC_STR_NEW(text_, fg_, font_id_, opa_) \
    {.text = (text_), .fg = (fg_), .font_id = (font_id_), .opa = (opa_)}

typedef struct sc_label_t
{
    const char *text;
    color_t fg;
    uint8_t font_id;
    uint8_t opa;
    uint8_t x_gap;
    sc_align_t align;
    int16_t x_ofs;
    sc_texture_t *texture;
    sc_obj_cb_t cb;
} sc_label_t;

typedef struct sc_str_t
{
    const char *text;
    color_t fg;
    uint8_t font_id;
    uint8_t opa;
} sc_str_t;

void sc_lcd_draw_label(sc_pfb_t *pfb, int16_t x, int16_t y, sc_label_t *props);

#if SC_USER_OBJ && SC_WIDGET_LABEL_ENABLE

sc_obj_t *sc_label_create(sc_obj_t *parent, sc_label_t *props);
sc_obj_t *sc_str_create(sc_obj_t *parent, sc_str_t *props);

void sc_label_set_x_gap(sc_obj_t *obj, uint8_t x_gap);
bool sc_label_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_label_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
