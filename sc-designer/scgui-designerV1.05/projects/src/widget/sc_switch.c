#include "../sc_draw.h"
#include "sc_switch.h"
#include "sc_round_rect.h"

#if SC_WIDGET_SWITCH_ENABLE

/* Draw the switch track, border and knob positioned by its checked state. */
static void sc_draw_switch(sc_pfb_t *pfb, const sc_switch_t *props, rect_t view, int border)
{
    /* 半径：props->radius 未设(0)时用胶囊形（高度一半）。 */
    int r = props->radius ? props->radius : (view.h >> 1);
    int bw = (int)props->border;
    /* 轨道：边框 + 填充一次画出；外框不相交 pfb 时直接返回，跳过柄。 */
    if (!sc_draw_Rounded_mask(pfb, view, r, bw,
                              props->knob_color,
                              props->checked ? props->track_on : props->track_off,
                              255U, 0, NULL))
    {
        return;
    }
    /* 柄：正圆，半径 = 外半径 - 边框 - 1，左右对齐（checked 靠右 / 未选靠左）。 */
    int margin = bw + 1;
    int knob_d = SC_MIN(view.h, view.w) - 2 * margin;
    if (knob_d > 0)
    {
        int kr = r > margin ? r - margin : 0;
        int knob_x = props->checked ? (view.x + view.w - margin - knob_d) : (view.x + margin);
        rect_t knob = {knob_x, view.y + margin, (uint16_t)knob_d, (uint16_t)knob_d};
        sc_draw_Rounded_mask(pfb, knob, kr, 0, props->knob_color, props->knob_color, 255U, 0, NULL);
    }
}

/* Draw a switch at a given rect, either into a frame buffer or via the LCD. */
void sc_lcd_draw_switch(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h, const sc_switch_t *props, int border)
{
    rect_t view = {x, y, w, h};
    if (!props)
    {
        return;
    }
    if (pfb)
    {
        sc_draw_switch(pfb, props, view, border);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_switch(sc_lcd_draw_pfb__, props, view, border));
}

#if SC_USER_OBJ && SC_WIDGET_SWITCH_ENABLE
/* Create a focusable switch object with default colors (initially off). */
static const sc_obj_vt_t sc_switch_vt = {
    .event_cb = sc_obj_handle_switch_event,
    .set_style = sc_switch_style,
};


sc_obj_t *sc_switch_create(sc_obj_t *parent, sc_switch_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_SWITCH, &sc_switch_vt, props);
    if (obj)
    {
        *props = (sc_switch_t)SC_SWITCH_NEW(C_WHITE, C_DARK_GRAY, C_GREEN, 0U);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

/* Set the switch on/off state, redrawing only when it actually changes. */
void sc_switch_set_checked(sc_obj_t *obj, bool checked)
{
    sc_switch_t *props = (sc_switch_t *)obj->props;
    if (props->checked != (uint8_t)checked)
    {
        props->checked = (uint8_t)checked;
        sc_obj_invalidate(obj);
    }
}

/* Apply a style property (colors and callback) to the switch. */
bool sc_switch_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_switch_t *props = (sc_switch_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
    case SC_STYLE_BORDER: /* 边框色与柄共用 knob_color */
        props->knob_color = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->track_off = *(const color_t *)value;
        return true;
    case SC_STYLE_ON:
        props->track_on = *(const color_t *)value;
        return true;
    case SC_STYLE_RADIUS:
        props->radius = *(const uint8_t *)value;
        return true;
    case SC_STYLE_BORDER_WIDTH:
        props->border = *(const uint8_t *)value;
        return true;
    case SC_STYLE_CALLBACK:
        props->cb = *(const sc_obj_cb_t *)value;
        return true;
    case SC_STYLE_RADIUS_READ:
        *(uint8_t *)value = (uint8_t)(props->radius ? props->radius : (obj->view.h >> 1));
        return true;

    default:
        return false;
    }
}

/* Handle switch events: draw, press tracking, toggle on release, run callback. */
bool sc_obj_handle_switch_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_switch_t *props = (sc_switch_t *)obj->props;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_switch((sc_pfb_t *)event->dat.arg, props, obj->view,
                       (obj->flags & SC_OBJ_F_PRESSED) ? 2U : 1U);
        return true;
    }
    /* 释放翻转 checked：必须在公共状态机清除 PRESSED 之前判断。 */
    if ((event->type == SC_EVENT_KEY_UP && event->dat.key == CMD_ENTER)
#if SC_TOUCH_ENABLE
        || event->type == SC_EVENT_TOUCH_UP
#endif
    )
    {
        if ((obj->flags & SC_OBJ_F_PRESSED) != 0U)
        {
            props->checked = (uint8_t)!props->checked;
        }
    }
    return sc_obj_common_event(obj, event, props->cb);
}

#endif /* SC_USER_OBJ && SC_WIDGET_SWITCH_ENABLE */

#endif /* SC_WIDGET_SWITCH_ENABLE */
