#include "../sc_focus.h"
#include "../sc_draw.h"
#include "sc_msgbox.h"

#if SC_WIDGET_MSGBOX_ENABLE

#define SC_MSGBOX_BTN_W 72
#define SC_MSGBOX_BTN_H 28
#define SC_MSGBOX_BTN_GAP 12
#define SC_MSGBOX_PAD 12

/* Draw one message box button with centered text; swaps colors when selected. */
static void sc_draw_msgbox_button(sc_pfb_t *pfb, rect_t view, const char *text, const sc_msgbox_t *props, bool selected)
{
    uint16_t text_w;
    uint16_t text_h;
    color_t bg = selected ? props->accent : props->bg;
    color_t fg = selected ? props->bg : props->fg;
    if (!sc_draw_fill_rect(pfb, view.x, view.y, view.w, view.h, bg))
    {
        return;
    }
    sc_draw_rect(pfb, view.x, view.y, view.w, view.h, props->fg, 1);
    sc_text_measure_lines(text, sc_get_font(props->font_id), 0U, &text_w, &text_h, NULL);
    sc_draw_str_range_opa(pfb,
                          view.x + sc_align_offset_x(view.w, text_w, ALIGN_CENTER),
                          view.y + sc_align_offset_y(view.h, text_h, ALIGN_CENTER),
                          text,
                          NULL,
                          fg,
                          NULL,
                          sc_get_font(props->font_id),
                          0U,
                          NULL,
                          255U);
}

/* Draw the message box background, border, centered text, and OK/Cancel buttons. */
static void sc_draw_msgbox(sc_pfb_t *pfb, const sc_msgbox_t *props, rect_t view)
{
    uint16_t text_w;
    uint16_t text_h;
    if (!props ||
        !sc_draw_fill_rect(pfb, view.x, view.y, view.w, view.h,
                           props->bg))
    {
        return;
    }
    sc_draw_rect(pfb, view.x, view.y, view.w, view.h,
                 props->fg, 2);
    if (props->text)
    {
        sc_text_measure_lines(props->text, sc_get_font(props->font_id), 0U, &text_w, &text_h, NULL);
        sc_draw_str_range_opa(pfb,
                              view.x + sc_align_offset_x(view.w, text_w, ALIGN_CENTER),
                              (int16_t)(view.y + SC_MSGBOX_PAD),
                              props->text,
                              NULL,
                              props->fg,
                              NULL,
                              sc_get_font(props->font_id),
                              0U,
                              NULL,
                              255U);
    }
    int16_t buttons_w = (int16_t)(SC_MSGBOX_BTN_W * 2 + SC_MSGBOX_BTN_GAP);
    int16_t buttons_x = view.x + sc_align_offset_x(view.w, buttons_w, ALIGN_CENTER);
    rect_t ok = (rect_t){buttons_x, (int16_t)(view.y + view.h - SC_MSGBOX_PAD - SC_MSGBOX_BTN_H), SC_MSGBOX_BTN_W, SC_MSGBOX_BTN_H};
    rect_t cancel = (rect_t){(int16_t)(buttons_x + SC_MSGBOX_BTN_W + SC_MSGBOX_BTN_GAP), ok.y, SC_MSGBOX_BTN_W, SC_MSGBOX_BTN_H};
    sc_draw_msgbox_button(pfb, ok, "OK", props,
                          props->selected == SC_MSGBOX_SELECTED_OK);
    sc_draw_msgbox_button(pfb, cancel, "Cancel", props,
                          props->selected == SC_MSGBOX_SELECTED_CANCEL);
}

/* Draw a message box at the given rectangle directly to the LCD or PFB. */
void sc_lcd_draw_msgbox(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h, const sc_msgbox_t *props)
{
    rect_t view = {x, y, w, h};
    if (!props)
    {
        return;
    }
    if (pfb)
    {
        sc_draw_msgbox(pfb, props, view);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_msgbox(sc_lcd_draw_pfb__, props, view));
}

#if SC_USER_OBJ && SC_WIDGET_MSGBOX_ENABLE

#if SC_TOUCH_ENABLE
/* Hit-test a point against the OK/Cancel buttons; returns the selected id or -1. */
static int8_t sc_msgbox_hit_button(rect_t view, int16_t x, int16_t y)
{
    int16_t buttons_w = (int16_t)(SC_MSGBOX_BTN_W * 2 + SC_MSGBOX_BTN_GAP);
    int16_t buttons_x = view.x + sc_align_offset_x(view.w, buttons_w, ALIGN_CENTER);
    int16_t button_y = (int16_t)(view.y + view.h - SC_MSGBOX_PAD - SC_MSGBOX_BTN_H);
    if (y < button_y || y >= button_y + SC_MSGBOX_BTN_H)
    {
        return -1;
    }
    if (x >= buttons_x && x < buttons_x + SC_MSGBOX_BTN_W)
    {
        return SC_MSGBOX_SELECTED_OK;
    }
    buttons_x = (int16_t)(buttons_x + SC_MSGBOX_BTN_W + SC_MSGBOX_BTN_GAP);
    if (x >= buttons_x && x < buttons_x + SC_MSGBOX_BTN_W)
    {
        return SC_MSGBOX_SELECTED_CANCEL;
    }
    return -1;
}
#endif

/* Close the message box: clear active flag and focus, then delete or hide it
   depending on the delete-on-close flag and the selected result. */
static void sc_msgbox_close(sc_obj_t *obj)
{
    sc_msgbox_t *props = (sc_msgbox_t *)obj->props;
    g_gui->flags &= (uint8_t)~SC_RT_MSGBOX_ACTIVE;
    if (sc_focus_current() == obj)
    {
        sc_obj_set_key_focus(NULL);
    }
    bool del = (props->flags & SC_MSGBOX_F_DELETE_ON_CLOSE) != 0U || props->selected != SC_MSGBOX_SELECTED_OK;
    if (del)
    {
        sc_obj_del(obj);
        return;
    }
    sc_obj_set_hidden(obj, true);
}

/* Create a modal message box (fails if one is already active), set defaults,
   mark it active, and give it key focus. */
static const sc_obj_vt_t sc_msgbox_vt = {
    .event_cb = sc_obj_handle_msgbox_event,
    .set_style = sc_msgbox_style,
};


sc_obj_t *sc_msgbox_create(sc_obj_t *parent, sc_msgbox_t *props)
{
    if ((g_gui->flags & SC_RT_MSGBOX_ACTIVE) != 0U)
    {
        return NULL;
    }
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_MSGBOX, &sc_msgbox_vt, props);
    if (obj)
    {
        *props = (sc_msgbox_t)SC_MSGBOX_NEW(NULL, C_WHITE, C_RGB(24, 32, 48), C_RGB(96, 165, 250), SC_FONT_DEFAULT);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
        g_gui->flags |= SC_RT_MSGBOX_ACTIVE;
        sc_obj_set_key_focus(obj);
    }
    return obj;
}

/* Apply a style property (fg, bg, accent, font, text, callback) to the message box. */
bool sc_msgbox_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_msgbox_t *props = (sc_msgbox_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->fg = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->bg = *(const color_t *)value;
        return true;
    case SC_STYLE_ON:
        props->accent = *(const color_t *)value;
        return true;
    case SC_STYLE_FONT:
        props->font_id = *(const uint8_t *)value;
        return true;
    case SC_STYLE_TEXT:
    {
        const sc_obj_text_style_t *text = (const sc_obj_text_style_t *)value;
        props->font_id = text->font_id;
        props->text = text->text;
        return true;
    }
    case SC_STYLE_CALLBACK:
        props->cb = *(const sc_obj_cb_t *)value;
        return true;
    default:
        return false;
    }
}

/* Handle message box events: draw, key navigation/selection, and touch button
   presses; invokes the callback and closes the box on confirmation. */
bool sc_obj_handle_msgbox_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_msgbox_t *props = (sc_msgbox_t *)obj->props;
    bool close = false;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_msgbox((sc_pfb_t *)event->dat.arg, props, obj->view);
        return true;
    }
    if (event->type == SC_EVENT_KEY_DOWN)
    {
        if (event->dat.key == CMD_LEFT || event->dat.key == CMD_RIGHT)
        {
            props->selected ^= 1U;
            sc_obj_invalidate(obj);
        }
        else if (event->dat.key == CMD_ENTER)
        {
            obj->flags |= SC_OBJ_F_PRESSED;
            sc_obj_invalidate(obj);
        }
        else if (event->dat.key == CMD_BACK)
        {
            props->selected = SC_MSGBOX_SELECTED_CANCEL;
        }
    }
    else if (event->type == SC_EVENT_KEY_UP)
    {
        if (event->dat.key == CMD_ENTER || event->dat.key == CMD_BACK)
        {
            obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
            close = true;
        }
    }
#if SC_TOUCH_ENABLE
    else if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        int8_t selected = sc_msgbox_hit_button(obj->view, event->dat.pos[0], event->dat.pos[1]);
        if (selected >= 0)
        {
            props->selected = (uint8_t)selected;
            obj->flags |= SC_OBJ_F_PRESSED;
            sc_obj_invalidate(obj);
        }
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        if ((obj->flags & SC_OBJ_F_PRESSED) != 0U)
        {
            int8_t selected = sc_msgbox_hit_button(obj->view, event->dat.pos[0], event->dat.pos[1]);
            obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
            if (selected >= 0)
            {
                props->selected = (uint8_t)selected;
                close = true;
            }
            else
            {
                sc_obj_invalidate(obj);
            }
        }
    }
    else if (event->type != SC_EVENT_TOUCH_MOVE)
#else
    else
#endif
    {
        return false;
    }
    if (props->cb)
    {
        props->cb(obj, event);
    }
    if (close && obj->props)
    {
        sc_msgbox_close(obj);
    }
    return true;
}

#endif /* SC_USER_OBJ && SC_WIDGET_MSGBOX_ENABLE */

#endif /* SC_WIDGET_MSGBOX_ENABLE */
