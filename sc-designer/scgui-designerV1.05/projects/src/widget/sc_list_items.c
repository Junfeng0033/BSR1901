#include "../sc_draw.h"
#include "sc_list_items.h"
#include "sc_round_rect.h"

#if SC_WIDGET_LIST_ITEMS_ENABLE

#define SC_LIST_VAL_BUF 16

static int sc_list_items_pitch(const sc_list_items_t *props, const lv_font_t *font)
{
    int pitch = sc_font_line_height(font) + (int)props->item_gap;
    return pitch > 0 ? pitch : 1;
}

/* Format value[+unit] into buf; returns length. buf must hold SC_LIST_VAL_BUF. */
static int sc_list_fmt_value(const sc_list_item_t *item, char *buf)
{
    int16_t v = item->value;
    char tmp[6];
    int n = 0;
    int i = 0;
    unsigned abs_v;
    if (v < 0)
    {
        buf[n++] = '-';
        abs_v = (unsigned)(-(int)v);
    }
    else
    {
        abs_v = (unsigned)v;
    }
    do
    {
        tmp[i++] = (char)('0' + (abs_v % 10U));
        abs_v /= 10U;
    } while (abs_v != 0U);
    while (i > 0 && n < SC_LIST_VAL_BUF - 1)
    {
        buf[n++] = tmp[--i];
    }
    if (item->unit)
    {
        const char *u = item->unit;
        while (*u != '\0' && n < SC_LIST_VAL_BUF - 1)
        {
            buf[n++] = *u++;
        }
    }
    buf[n] = '\0';
    return n;
}

static void sc_draw_list_items(sc_pfb_t *pfb, const sc_list_items_t *props, rect_t view)
{
    if (!props || view.w == 0U || view.h == 0U)
    {
        return;
    }
    uint8_t radius = props->radius;
    if (!sc_draw_Rounded_mask(pfb, view, (int)radius, 1, props->fg, props->bg, 255U, 0U, NULL))
    {
        return;
    }

    int pad = (int)(radius >> 1);
    int bar_w = (int)props->bar_w;
    rect_t list_box = {
        (int16_t)(view.x + pad),
        (int16_t)(view.y + pad),
        (uint16_t)SC_MAX((int)view.w - bar_w - 2 * pad, 0),
        (uint16_t)SC_MAX((int)view.h - 2 * pad, 0),
    };
    const lv_font_t *font = sc_get_font(props->font_id);
    int item_h = sc_font_line_height(font);
    int item_pitch = sc_list_items_pitch(props, font);
    uint8_t cursor = props->cursor;
    if (props->items_cnt != 0U && cursor >= props->items_cnt)
    {
        cursor = 0U;
    }
    area_t list_clip = rect_to_area(list_box);
    int y = list_box.y + props->yofs;
    if (props->items)
    {
        uint8_t i;
        for (i = 0U; i < props->items_cnt; i++)
        {
            rect_t item_box = {
                list_box.x,
                (int16_t)y,
                list_box.w,
                (uint16_t)item_pitch,
            };
            if (item_box.y + (int)item_box.h > list_box.y &&
                item_box.y < list_box.y + (int)list_box.h)
            {
                const sc_list_item_t *item = &props->items[i];
                bool selected = (i == cursor);
                color_t text_color = selected ? C_WHITE : props->fg;
                const char *name = item->name;
                uint16_t name_h = 0U;
                if (name)
                {
                    uint16_t name_w = 0U;
                    sc_text_measure_lines(name, font, 0U, &name_w, &name_h, NULL);
                }
                int16_t ty = (int16_t)(item_box.y + sc_align_offset_y(item_h, name_h, ALIGN_VER));
                if (selected && name)
                {
                    /* 选中底色铺满整条宽度（不再按文字长度测量）。 */
                    rect_t sel = {item_box.x, item_box.y, (uint16_t)item_box.w, (uint16_t)item_h};
                    (void)sc_draw_Rounded_mask(pfb, sel, (int)(radius >> 1), 0,
                                               props->sel_bg, props->sel_bg, 255U, 0U, &list_clip);
                }
                if (name)
                {
                    sc_draw_str_range_opa(pfb, (int16_t)(item_box.x + 2), ty, name, NULL,
                                          text_color, NULL, font, 0U, &list_clip, 255U);
                }
                /* Right column: value + unit when adjustable or unit provided. */
                if (item->unit != NULL || item->step != 0)
                {
                    char val_buf[SC_LIST_VAL_BUF];
                    uint16_t val_w = 0U;
                    uint16_t val_h = 0U;
                    color_t val_color = (selected && props->setting) ? C_RED : text_color;
                    (void)sc_list_fmt_value(item, val_buf);
                    sc_text_measure_lines(val_buf, font, 0U, &val_w, &val_h, NULL);
                    int16_t vx = (int16_t)(item_box.x + (int)item_box.w - (int)val_w - 2);
                    int16_t vy = (int16_t)(item_box.y + sc_align_offset_y(item_h, val_h, ALIGN_VER));
                    sc_draw_str_range_opa(pfb, vx, vy, val_buf, NULL, val_color, NULL, font, 0U,
                                          &list_clip, 255U);
                }
            }
            y += item_pitch;
        }
    }

    if (bar_w > 0)
    {
        rect_t bar = {
            (int16_t)(view.x + (int)view.w - bar_w - pad),
            (int16_t)(view.y + pad),
            (uint16_t)bar_w,
            (uint16_t)SC_MAX((int)view.h - (int)radius, 0),
        };
        sc_draw_bar(pfb, bar, bar_w / 2, props->fg, C_RGB(100, 100, 100), 128U,
                    -props->yofs);
    }
}

void sc_lcd_draw_list_items(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h,
                            const sc_list_items_t *props)
{
    rect_t view = {x, y, w, h};
    if (!props)
    {
        return;
    }
    if (pfb)
    {
        sc_draw_list_items(pfb, props, view);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_list_items(sc_lcd_draw_pfb__, props, view));
}

#if SC_USER_OBJ && SC_WIDGET_LIST_ITEMS_ENABLE

/* Enter toggles setting; while setting, UP/DOWN steps value; LEFT/RIGHT exits setting. */
void sc_list_items_default_cb(sc_obj_t *obj, const sc_event_t *event)
{
    sc_list_items_t *props = (sc_list_items_t *)obj->props;
    sc_list_item_t *item;
    int8_t dir;
    int32_t next;

    if (event->type != SC_EVENT_KEY_DOWN || !props->items || props->items_cnt == 0U ||
        props->cursor >= props->items_cnt)
    {
        return;
    }
    item = &props->items[props->cursor];

    if (event->dat.key == CMD_ENTER)
    {
        if (props->setting)
        {
            props->setting = false;
            sc_obj_invalidate(obj);
            return;
        }
        if (item->step == 0)
        {
            return;
        }
        props->setting = true;
        sc_obj_invalidate(obj);
        return;
    }

    if (!props->setting)
    {
        return;
    }
    /* Exit setting on LEFT/RIGHT (consume so focus does not move). */
    if (event->dat.key == CMD_LEFT || event->dat.key == CMD_RIGHT)
    {
        props->setting = false;
        sc_obj_invalidate(obj);
        return;
    }
    if (event->dat.key != CMD_UP && event->dat.key != CMD_DOWN)
    {
        return;
    }
    if (item->step == 0)
    {
        return;
    }
    dir = (event->dat.key == CMD_UP) ? 1 : -1;
    next = (int32_t)item->value + (int32_t)dir * (int32_t)item->step;
    next = SC_CLAMP(next, (int32_t)item->min, (int32_t)item->max);
    if (next == (int32_t)item->value)
    {
        return;
    }
    item->value = (int16_t)next;
    sc_obj_invalidate(obj);
}

static void sc_list_items_scroll_range(const sc_obj_t *obj, const sc_list_items_t *props,
                                       int16_t *out_min, int16_t *out_max)
{
    const lv_font_t *font = sc_get_font(props->font_id);
    int pitch = sc_list_items_pitch(props, font);
    if (props->items_cnt == 0U || pitch <= 0)
    {
        *out_min = 0;
        *out_max = 0;
        return;
    }
    int content_h = (int)props->items_cnt * pitch;
    int view_h = (int)obj->view.h;
    sc_scroll_range(content_h, view_h, pitch, out_min, out_max);
}

#if SC_TOUCH_ENABLE
static void sc_list_items_scroll(sc_obj_t *obj, int16_t dy)
{
    sc_list_items_t *props = (sc_list_items_t *)obj->props;
    if (dy == 0 || props->items_cnt == 0U)
    {
        return;
    }
    int16_t new_yofs = (int16_t)(props->yofs + dy);
    int16_t min_yofs, max_yofs;
    sc_list_items_scroll_range(obj, props, &min_yofs, &max_yofs);
    new_yofs = SC_CLAMP(new_yofs, min_yofs, max_yofs);
    if (new_yofs == props->yofs)
    {
        return;
    }
    props->yofs = new_yofs;
    sc_obj_invalidate(obj);
}
#endif

static void sc_list_items_ensure_cursor_visible(sc_obj_t *obj)
{
    sc_list_items_t *props = (sc_list_items_t *)obj->props;
    const lv_font_t *font = sc_get_font(props->font_id);
    int pitch = sc_list_items_pitch(props, font);
    /* 行填满 pitch：item_h 传 pitch，滚动行为与旧实现一致；共用公共内联省 flash */
    sc_list_ensure_visible(&props->yofs, pitch, pitch,
                           (int)obj->view.h, (int)props->items_cnt, (int)props->cursor);
}

#if SC_TOUCH_ENABLE
static int sc_list_items_hit(const sc_obj_t *obj, int16_t y)
{
    const sc_list_items_t *props = (const sc_list_items_t *)obj->props;
    if (props->items_cnt == 0U)
    {
        return -1;
    }
    const lv_font_t *font = sc_get_font(props->font_id);
    int pad = (int)(props->radius >> 1);
    int item_h = sc_font_line_height(font);
    int pitch = sc_list_items_pitch(props, font);
    int item_y = y - obj->view.y - pad - props->yofs;
    if (item_y < 0)
    {
        return -1;
    }
    int index = item_y / pitch;
    int inner_y = item_y % pitch;
    if (inner_y >= item_h || index < 0 || index >= (int)props->items_cnt)
    {
        return -1;
    }
    return index;
}
#endif

static const sc_obj_vt_t sc_list_items_vt = {
    .event_cb = sc_obj_handle_list_items_event,
    .set_style = sc_list_items_style,
};



sc_obj_t *sc_list_items_create(sc_obj_t *parent, sc_list_items_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_LIST_ITEMS, &sc_list_items_vt, props);
    if (obj)
    {
        *props = (sc_list_items_t)SC_LIST_ITEMS_NEW(NULL, 0U, C_GRAY, g_gui->bg, C_BLUE);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

void sc_list_items_set_data(sc_obj_t *obj, sc_list_item_t *items, uint8_t items_cnt)
{
    sc_list_items_t *props = (sc_list_items_t *)obj->props;
    props->items = items;
    props->items_cnt = items_cnt;
    if (props->cursor >= items_cnt)
    {
        props->cursor = 0U;
    }
    props->yofs = 0;
    props->setting = false;
    sc_obj_invalidate(obj);
}

void sc_list_items_set_cursor(sc_obj_t *obj, uint8_t cursor)
{
    sc_list_items_t *props = (sc_list_items_t *)obj->props;
    if (props->items_cnt == 0U || cursor >= props->items_cnt || props->cursor == cursor)
    {
        return;
    }
    props->cursor = cursor;
    props->setting = false;
    sc_list_items_ensure_cursor_visible(obj);
    sc_obj_invalidate(obj);
}

bool sc_list_items_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_list_items_t *props = (sc_list_items_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->fg = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->bg = *(const color_t *)value;
        return true;
    case SC_STYLE_ON:
        props->sel_bg = *(const color_t *)value;
        return true;
    case SC_STYLE_FONT:
        props->font_id = *(const uint8_t *)value;
        return true;
    case SC_STYLE_ALIGN:
        props->align = *(const sc_align_t *)value;
        return true;
    case SC_STYLE_CALLBACK:
        props->cb = value ? *(const sc_obj_cb_t *)value : sc_list_items_default_cb;
        return true;
    case SC_STYLE_RADIUS_READ:
        *(uint8_t *)value = ((const sc_list_items_t *)obj->props)->radius;
        return true;

    default:
        return false;
    }
}

bool sc_obj_handle_list_items_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_list_items_t *props = (sc_list_items_t *)obj->props;
    bool ret = false;

    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_list_items((sc_pfb_t *)event->dat.arg, props, obj->view);
        return true;
    }

    /* !setting: LEFT/RIGHT free for focus; UP/DOWN move cursor.
     * setting: Enter/LEFT/RIGHT exit; UP/DOWN adjust (all consumed). */
    if (event->type == SC_EVENT_KEY_DOWN && props->items_cnt != 0U)
    {
        if (event->dat.key == CMD_ENTER)
        {
            ret = true;
        }
        else if (event->dat.key == CMD_UP || event->dat.key == CMD_DOWN)
        {
            if (!props->setting)
            {
                uint8_t count = props->items_cnt;
                uint8_t cursor = props->cursor;
                if (event->dat.key == CMD_UP)
                {
                    cursor = (uint8_t)((cursor + count - 1U) % count);
                }
                else
                {
                    cursor = (uint8_t)((cursor + 1U) % count);
                }
                props->cursor = cursor;
                sc_list_items_ensure_cursor_visible(obj);
                sc_obj_invalidate(obj);
            }
            ret = true;
        }
        else if (props->setting &&
                 (event->dat.key == CMD_LEFT || event->dat.key == CMD_RIGHT))
        {
            ret = true;
        }
    }
#if SC_TOUCH_ENABLE
    else if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        g_gui->flags |= SC_RT_DRAGGING;
        g_gui->drag_offset_y = event->dat.pos[1];
        obj->flags |= SC_OBJ_F_PRESSED;
        ret = true;
    }
    else if (event->type == SC_EVENT_TOUCH_MOVE && (g_gui->flags & SC_RT_DRAGGING) != 0U)
    {
        /* 帧位移 = 当前位置 - 上一帧位置 */
        int16_t dy = (int16_t)(event->dat.pos[1] - g_gui->last_pos_y);
        if (dy != 0)
        {
            obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
            sc_list_items_scroll(obj, dy);
        }
        ret = true;
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        g_gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        if ((obj->flags & SC_OBJ_F_PRESSED) != 0U)
        {
            int hit = sc_list_items_hit(obj, event->dat.pos[1]);
            if (hit >= 0 && props->cursor != (uint8_t)hit)
            {
                props->cursor = (uint8_t)hit;
                props->setting = false;
                sc_obj_invalidate(obj);
            }
        }
        obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
        ret = true;
    }
#endif

    (props->cb ? props->cb : sc_list_items_default_cb)(obj, event);
    return ret;
}

#endif /* SC_USER_OBJ && SC_WIDGET_LIST_ITEMS_ENABLE */

#endif /* SC_WIDGET_LIST_ITEMS_ENABLE */
