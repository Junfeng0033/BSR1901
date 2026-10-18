#include "../sc_draw.h"
#include "sc_button.h"
#include "sc_round_rect.h"

#if SC_WIDGET_BUTTON_ENABLE 

/* Render a button (rounded fill/border/texture + aligned text) into a partial frame buffer. */
void sc_pfb_draw_button(sc_pfb_t *pfb, const sc_button_t *props, rect_t view, int pressed)
{
    rect_t draw = view;
    if (pressed != 0)
    {
        /* Press feedback: inset 1px so solid and textured buttons both show motion. */
        draw = rect_grow(draw, -1, -1);
    }
    int r = (int)props->radius;
    int b = (int)props->border;
    if (b < 0)
    {
        b = 0;
    }
    if (r < b)
    {
        r = b;
    }
    (void)sc_draw_Rounded_texture(pfb, draw, r, b,
                                  props->text_c, props->bg, props->opa,
                                  props->texture, props->rect_mask, NULL);
    if (!props->text)
    {
        return;
    }
    const lv_font_t *font = sc_get_font(props->font_id);
    uint16_t text_w = 0U;
    uint16_t text_h = 0U;
    int wrap_width = (props->align & ALIGN_AUTO_BREAK) != 0U ? (int)draw.w : 0;
    if (wrap_width != 0)
    {
        sc_text_measure_layout(props->text, font, 0U, wrap_width, &text_w, &text_h, NULL);
    }
    else
    {
        sc_text_measure_lines(props->text, font, 0U, &text_w, &text_h, NULL);
    }
    int16_t base_x = (int16_t)(draw.x + sc_align_offset_x(draw.w, text_w, props->align));
    int16_t base_y = (int16_t)(draw.y + sc_align_offset_y(draw.h, text_h, props->align));
    area_t inter = rect_to_area(draw);
    if (!sc_pfb_intersection(pfb, &inter, NULL))
    {
        return;
    }
    int line_height = sc_font_line_height(font);
    bool single_line = text_h == (uint16_t)line_height;
    if (single_line)
    {
        sc_draw_str_range_opa(pfb, base_x, base_y, props->text, NULL,
                              props->text_c, NULL, font, 0U, &inter, props->opa);
        return;
    }
    const char *line_start = props->text;
    while (base_y < inter.ye && *line_start != '\0')
    {
        uint16_t line_width = 0U;
        const char *line_end = NULL;
        const char *next_line = sc_text_next_line(line_start, font, 0U, wrap_width, &line_end, &line_width);
        if (base_y + line_height > inter.ys)
        {
            int16_t line_x = base_x + sc_align_offset_x(text_w, line_width, props->align);
            sc_draw_str_range_opa(pfb, line_x, base_y, line_start, line_end,
                                  props->text_c, NULL, font, 0U, &inter, props->opa);
        }
        base_y = (int16_t)(base_y + line_height);
        line_start = next_line;
    }
}

/* Draw a button to the LCD via the given pfb or the LCD draw macro. */
void sc_lcd_draw_button(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h,
                        const sc_button_t *props, int pressed)
{
    rect_t view = {x, y, w, h};
    if (!props)
    {
        return;
    }
    if (pfb)
    {
        sc_pfb_draw_button(pfb, props, view, pressed);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_pfb_draw_button(sc_lcd_draw_pfb__, props, view, pressed));
}

#if SC_USER_OBJ && SC_WIDGET_BUTTON_ENABLE
/* Create a focusable button object under parent and initialize its props with defaults. */
static const sc_obj_vt_t sc_button_vt = {
    .event_cb = sc_obj_handle_button_event,
    .set_style = sc_button_style,
};

sc_obj_t *sc_button_create(sc_obj_t *parent, sc_button_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_BUTTON, &sc_button_vt, props);
    if (obj)
    {
        *props = (sc_button_t)SC_BUTTON_NEW(NULL, C_WHITE, C_BLUE, SC_FONT_DEFAULT, ALIGN_CENTER);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

/* Apply a style property to a button; returns true if handled. */
bool sc_button_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_button_t *props = (sc_button_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->text_c = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->bg = *(const color_t *)value;
        return true;
    case SC_STYLE_OPA:
        props->opa = *(const uint8_t *)value;
        return true;
    case SC_STYLE_RADIUS:
        props->radius = *(const uint8_t *)value;
        return true;
    case SC_STYLE_BORDER_WIDTH:
        props->border = *(const uint8_t *)value;
        return true;
    case SC_STYLE_RECT_MASK:
        props->rect_mask = *(const uint8_t *)value;
        return true;
    case SC_STYLE_TEXTURE:
        return sc_obj_apply_texture(&props->texture, obj->view, value);
    case SC_STYLE_FONT:
        props->font_id = *(const uint8_t *)value;
        return true;
    case SC_STYLE_ALIGN:
        props->align = *(const sc_align_t *)value;
        return true;
    case SC_STYLE_TEXT:
    {
        const sc_obj_text_style_t *text = (const sc_obj_text_style_t *)value;
        props->font_id = text->font_id;
        props->text = text->text;
        sc_obj_invalidate(obj);
        return true;
    }
    case SC_STYLE_CALLBACK:
        props->cb = *(const sc_obj_cb_t *)value;
        return true;
    case SC_STYLE_RADIUS_READ:
        *(uint8_t *)value = ((const sc_button_t *)obj->props)->radius;
        return true;

    default:
        return false;
    }
}

/* Handle button events: draw, press/release (key or touch) with pressed state, then invoke user callback. */
bool sc_obj_handle_button_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_button_t *props = (sc_button_t *)obj->props;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_pfb_draw_button((sc_pfb_t *)event->dat.arg, props, obj->view,
                           (obj->flags & SC_OBJ_F_PRESSED) != 0U);
        return true;
    }
    return sc_obj_common_event(obj, event, props->cb);
}

#endif /* SC_USER_OBJ && SC_WIDGET_BUTTON_ENABLE */

#endif /* SC_WIDGET_BUTTON_ENABLE */
