#include "../sc_draw.h"
#include "sc_progress.h"
#include "sc_round_rect.h"

#if SC_WIDGET_PROGRESS_ENABLE

/* Draw a rounded progress bar, splitting the view into filled and empty regions
   based on props->value; supports horizontal or vertical orientation. */
static void sc_draw_progress(sc_pfb_t *pfb, const sc_progress_t *props, rect_t view)
{
    int split;

    if (view.w == 0U || view.h == 0U)
    {
        return;
    }
    area_t fill_clip = rect_to_area(view);
    area_t empty_clip = fill_clip;
    bool vertical = view.h > view.w;
    int length = vertical ? view.h : view.w;
    int fill_len = (int)(((uint32_t)length * props->value) / SC_PROGRESS_MAX);
    fill_len = SC_MIN(fill_len, length);
    if (vertical)
    {
        split = fill_clip.ye - fill_len;
        fill_clip.ys = (int16_t)split;
        empty_clip.ye = (int16_t)split;
    }
    else
    {
        split = fill_clip.xs + fill_len;
        fill_clip.xe = (int16_t)split;
        empty_clip.xs = (int16_t)split;
    }
    int radius = SC_MIN(view.w, view.h) >> 1;
    if (props->texture)
    {
        (void)sc_draw_Rounded_texture(pfb, view, radius, 1,
                                      props->bg, props->fg, props->opa, props->texture,
                                      0U, &fill_clip);
    }
    else
    {
        (void)sc_draw_Rounded_mask(pfb, view, radius, 1,
                                   props->bg, props->fg, props->opa, 0U, &fill_clip);
    }
    (void)sc_draw_Rounded_mask(pfb, view, radius, 1,
                               props->bg, g_gui->bg, props->opa, 0U, &empty_clip);
}

/* Draw a progress bar at the given rectangle directly to the LCD or PFB. */
void sc_lcd_draw_progress(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h, const sc_progress_t *props)
{
    rect_t view = {x, y, w, h};
    if (!props)
    {
        return;
    }
    if (pfb)
    {
        sc_draw_progress(pfb, props, view);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_progress(sc_lcd_draw_pfb__, props, view));
}

#if SC_USER_OBJ && SC_WIDGET_PROGRESS_ENABLE
/* Create a focusable progress object under parent with default properties. */
static const sc_obj_vt_t sc_progress_vt = {
    .event_cb = sc_obj_handle_progress_event,
    .set_style = sc_progress_style,
};


sc_obj_t *sc_progress_create(sc_obj_t *parent, sc_progress_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_PROGRESS, &sc_progress_vt, props);
    if (obj)
    {
        *props = (sc_progress_t)SC_PROGRESS_NEW(C_BLUE, C_WHITE, 0U);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

/* Set the progress value (clamped to SC_PROGRESS_MAX) and invalidate if changed. */
void sc_progress_set_value(sc_obj_t *obj, uint16_t value)
{
    sc_progress_t *props = (sc_progress_t *)obj->props;
    value = SC_MIN(value, SC_PROGRESS_MAX);
    if (props->value != value)
    {
        props->value = (uint8_t)value;
        sc_obj_invalidate(obj);
    }
}

/* Apply a style property (fg, bg, opacity, texture, callback) to the progress bar. */
bool sc_progress_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_progress_t *props = (sc_progress_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->fg = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->bg = *(const color_t *)value;
        return true;
    case SC_STYLE_OPA:
        props->opa = *(const uint8_t *)value;
        return true;
    case SC_STYLE_TEXTURE:
        return sc_obj_apply_texture(&props->texture, obj->view, value);
    case SC_STYLE_CALLBACK:
        props->cb = *(const sc_obj_cb_t *)value;
        return true;
    case SC_STYLE_RADIUS_READ:
        *(uint8_t *)value = (uint8_t)(SC_MIN(obj->view.w, obj->view.h) >> 1);
        return true;
    default:
        return false;
    }
}

/* Handle progress events: draw, key up/down or left/right stepping, and touch
   dragging to set the value; forwards events to the user callback. */
bool sc_obj_handle_progress_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_progress_t *props = (sc_progress_t *)obj->props;
    bool ret = false;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_progress((sc_pfb_t *)event->dat.arg, props, obj->view);
        return true;
    }
    if (event->type == SC_EVENT_KEY_DOWN)
    {
        if (event->dat.key == CMD_ENTER)
        {
            g_gui->flags ^= SC_RT_KEY_EDIT; /* 回车切换编辑模式 */
        sc_obj_invalidate(obj); /* 1H�Z&F�s�r */
            ret = true;
        }
        else if ((g_gui->flags & SC_RT_KEY_EDIT) != 0U)
        {
            bool vertical = obj->view.h > obj->view.w;
            bool decrease = vertical ? event->dat.key == CMD_DOWN : event->dat.key == CMD_LEFT;
            bool increase = vertical ? event->dat.key == CMD_UP : event->dat.key == CMD_RIGHT;
            if (decrease || increase)
            {
                uint16_t step = SC_PROGRESS_MAX / 20U;
                if (step == 0U)
                {
                    step = 1U;
                }
                sc_progress_set_value(obj, decrease ? (props->value > step ? (uint16_t)(props->value - step) : 0U)
                                                    : (uint16_t)(props->value + step));
                ret = true;
            }
        }
    }
#if SC_TOUCH_ENABLE
    else if (event->type == SC_EVENT_TOUCH_DOWN ||
             (event->type == SC_EVENT_TOUCH_MOVE && (g_gui->flags & SC_RT_DRAGGING) != 0U))
    {
        bool vertical = obj->view.h > obj->view.w;
        int16_t length = (int16_t)(vertical ? obj->view.h : obj->view.w) - 1;
        if (event->type == SC_EVENT_TOUCH_DOWN)
        {
            g_gui->flags |= SC_RT_DRAGGING;
        }
        if (length > 0)
        {
            int16_t pos = vertical ? (int16_t)(obj->view.y + obj->view.h - 1 - event->dat.pos[1])
                                   : (int16_t)(event->dat.pos[0] - obj->view.x);
            pos = SC_CLAMP(pos, 0, length);
            sc_progress_set_value(obj, (uint16_t)(((uint32_t)pos * SC_PROGRESS_MAX) / length));
        }
        ret = true;
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        g_gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        return true;
    }
#endif
    if (props->cb)
    {
        props->cb(obj, event);
    }
    return ret;
}

#endif /* SC_USER_OBJ && SC_WIDGET_PROGRESS_ENABLE */

#endif /* SC_WIDGET_PROGRESS_ENABLE */
