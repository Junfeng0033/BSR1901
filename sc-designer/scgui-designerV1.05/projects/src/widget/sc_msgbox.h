#ifndef SC_WIDGET_MSGBOX_H
#define SC_WIDGET_MSGBOX_H

#include "../sc_common.h"
#include "../sc_obj.h"

#define SC_MSGBOX_SELECTED_OK 0U
#define SC_MSGBOX_SELECTED_CANCEL 1U
#define SC_MSGBOX_F_DELETE_ON_CLOSE 0x01U

#define SC_MSGBOX_NEW(text_, fg_, bg_, accent_, font_id_) \
    {.text = (text_), .fg = (fg_), .bg = (bg_), .accent = (accent_), .font_id = (font_id_)}

typedef struct sc_msgbox_t
{
    const char *text;
    sc_obj_cb_t cb;
    color_t fg;
    color_t bg;
    color_t accent;
    uint8_t font_id;
    uint8_t selected;
    uint8_t flags;
} sc_msgbox_t;

void sc_lcd_draw_msgbox(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h, const sc_msgbox_t *props);

#if SC_USER_OBJ && SC_WIDGET_MSGBOX_ENABLE
sc_obj_t *sc_msgbox_create(sc_obj_t *parent, sc_msgbox_t *props);
bool sc_msgbox_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_msgbox_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
