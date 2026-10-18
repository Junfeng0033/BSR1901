#include "../sc_draw.h"
#include "sc_label.h"

#if SC_WIDGET_LABEL_ENABLE

/* Rebuild a gradient texture to match new width/height; no-op for image textures. */
static void sc_label_resize_texture(sc_texture_t *texture,
                                    uint16_t width, uint16_t height)
{
    if (!texture || texture->dir == SC_GRAD_IMAGE ||
        width == 0U || height == 0U)
    {
        return;
    }
    color_t c0 = texture->data.gradient.c0;
    color_t c1 = texture->data.gradient.c1;
    sc_texture_init_custom(texture, texture->dir, width, height,
                           c0, c1, NULL);
}

/* Measure text; returns width and height via out params. No caching. */
static void sc_label_measure_to(const sc_label_t *props,
                                uint16_t *out_w, uint16_t *out_h)
{
    if (!props->text)
    {
        *out_w = 0U;
        *out_h = 0U;
        return;
    }
    sc_text_measure_lines(props->text, sc_get_font(props->font_id), props->x_gap,
                          out_w, out_h, NULL);
}

/* Draw the label text within the given view, clipped to the frame buffer.
 * x_ofs 已是确切值（含 align_offset_x），绘制时直接用，免计算。 */
static void sc_draw_label(sc_pfb_t *pfb, sc_label_t *props, rect_t view)
{
    if (!props->text)
    {
        return;
    }
    const lv_font_t *font = sc_get_font(props->font_id);
    uint16_t text_h = font ? font->line_height : 8U;
    int16_t base_x = (int16_t)(view.x + props->x_ofs);
    int16_t base_y = (int16_t)(view.y + sc_align_offset_y(view.h, text_h, props->align));
    area_t inter = rect_to_area(view);
    if (!sc_pfb_intersection(pfb, &inter, NULL))
    {
        return;
    }
    sc_draw_str_range_opa(pfb, base_x, base_y, props->text, NULL, props->fg, props->texture,
                          font, props->x_gap, &inter, props->opa);
    if (props->align & ALIGN_BORDER_VIS)
    {
        sc_draw_rect(pfb, view.x, view.y, view.w, view.h, C_WHITE, 1);
    }
}

static void sc_draw_str(sc_pfb_t *pfb, const sc_str_t *props, rect_t view)
{
    if (!props->text)
    {
        return;
    }
    area_t inter = rect_to_area(view);
    if (!sc_pfb_intersection(pfb, &inter, NULL))
    {
        return;
    }
    sc_draw_str_range_opa(pfb,
                          view.x,
                          view.y,
                          props->text,
                          NULL,
                          props->fg,
                          NULL,
                          sc_get_font(props->font_id),
                          0,
                          &inter,
                          props->opa);
}
/* Measure text, size the label at (x,y), and draw it directly to the LCD or PFB. */
void sc_lcd_draw_label(sc_pfb_t *pfb, int16_t x, int16_t y, sc_label_t *props)
{
    uint16_t w;
    uint16_t h;
    if (!props)
    {
        return;
    }
    sc_label_measure_to(props, &w, &h);
    sc_label_resize_texture(props->texture, w, h);
    props->x_ofs = sc_align_offset_x(w, w, props->align);
    rect_t view = (rect_t){x, y, w, h};
    if (pfb)
    {
        sc_draw_label(pfb, props, view);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_label(sc_lcd_draw_pfb__, props, view));
}

#if SC_USER_OBJ && SC_WIDGET_LABEL_ENABLE
/* Create a label object under parent and initialize its default properties. */
static const sc_obj_vt_t sc_label_vt = {
    .event_cb = sc_obj_handle_label_event,
    .set_style = sc_label_style,
};

sc_obj_t *sc_label_create(sc_obj_t *parent, sc_label_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_LABEL, &sc_label_vt, props);
    if (obj)
    {
        *props = (sc_label_t)SC_LABEL_NEW(NULL, C_WHITE, SC_FONT_DEFAULT, 255U);
    }
    return obj;
}
sc_obj_t *sc_str_create(sc_obj_t *parent, sc_str_t *props)
{
    return sc_obj_create(parent, SC_OBJ_STR, &sc_label_vt, props);
}

/* Handle label object events; renders on the system draw event. */
bool sc_obj_handle_label_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        if (sc_obj_type(obj) == SC_OBJ_STR)
        {
            sc_draw_str((sc_pfb_t *)event->dat.arg, obj->props, obj->view);
        }
        else
        {
            sc_draw_label((sc_pfb_t *)event->dat.arg, obj->props, obj->view);
        }
        return true;
    }
    /* 公共按下/释放状态机；仅 label（非 str）有 cb 字段，str 传 NULL 不回调。 */
    return sc_obj_common_event(obj, event,
                               sc_obj_type(obj) == SC_OBJ_LABEL ? ((sc_label_t *)obj->props)->cb : NULL);
}

/* Set the inter-character gap; re-measure and recompute x_ofs. */
void sc_label_set_x_gap(sc_obj_t *obj, uint8_t x_gap)
{
    if (sc_obj_type(obj) == SC_OBJ_LABEL)
    {
        sc_label_t *props = (sc_label_t *)obj->props;
        props->x_gap = x_gap;
        uint16_t tw, th;
        sc_label_measure_to(props, &tw, &th);
        sc_label_resize_texture(props->texture, tw, th);
        props->x_ofs = sc_align_offset_x(obj->view.w, tw, props->align);
    }
}

/* Apply a style property (color, font, opacity, text, texture) to the label.
 * SC_OBJ_STR 只允许修改它有的字段(text/fg/font_id/opa)，
 * 由用户保证 props 不是 const 限定的；align/texture/x_gap 等字段不存在，返回 false。 */
bool sc_label_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    if (sc_obj_type(obj) == SC_OBJ_STR)
    {
        sc_str_t *props = (sc_str_t *)obj->props;
        switch (style)
        {
        case SC_STYLE_FG:
            props->fg = *(const color_t *)value;
            return true;
        case SC_STYLE_OPA:
            props->opa = *(const uint8_t *)value;
            return true;
        case SC_STYLE_TEXT:
        {
            const sc_obj_text_style_t *text = (const sc_obj_text_style_t *)value;
            props->font_id = text->font_id;
            props->text = text->text;
            /* const label 自动测量包围盒，修改文本后更新 view。 */
            uint16_t tw, th;
            sc_text_measure_lines(props->text, sc_get_font(props->font_id), 0, &tw, &th, NULL);
            sc_obj_set_rect(obj, obj->view.x, obj->view.y, tw, th, ALIGN_NONE);
            return true;
        }
        default:
            return false;
        }
    }
    sc_label_t *props = (sc_label_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->fg = *(const color_t *)value;
        return true;
    case SC_STYLE_OPA:
        props->opa = *(const uint8_t *)value;
        return true;
    case SC_STYLE_TEXT:
    {
        const sc_obj_text_style_t *text = (const sc_obj_text_style_t *)value;
        props->font_id = text->font_id;
        props->text = text->text;
        /* 立即测量：重建纹理，重算 x_ofs。 */
        uint16_t tw, th;
        sc_label_measure_to(props, &tw, &th);
        sc_label_resize_texture(props->texture, tw, th);
        props->x_ofs = sc_align_offset_x(obj->view.w, tw, props->align);
        return true;
    }
    case SC_STYLE_TEXTURE:
    {
        const sc_obj_texture_style_t *ts = (const sc_obj_texture_style_t *)value;
        props->texture = ts->texture;
        if (ts->texture && ts->dir != SC_GRAD_IMAGE)
        {
            uint16_t tw, th;
            sc_label_measure_to(props, &tw, &th);
            if (tw == 0U)
                tw = 1U;
            if (th == 0U)
                th = 1U;
            sc_texture_init_custom(ts->texture, ts->dir, tw, th, ts->c0, ts->c1, NULL);
        }
        return true;
    }
    case SC_STYLE_ALIGN:
    {
        sc_align_t new_align = *(const sc_align_t *)value;
        props->align = new_align;
        uint16_t tw, th;
        sc_label_measure_to(props, &tw, &th);
        props->x_ofs = sc_align_offset_x(obj->view.w, tw, new_align);
        return true;
    }
    case SC_STYLE_CALLBACK:
        props->cb = *(const sc_obj_cb_t *)value;
        obj->flags |= SC_OBJ_F_FOCUSABLE; /* 注册回调即开启可聚焦（label 默认不聚焦） */
        return true;
    default:
        return false;
    }
}

#endif /* SC_USER_OBJ && SC_WIDGET_LABEL_ENABLE */

#endif /* SC_WIDGET_LABEL_ENABLE */
