#include "../sc_draw.h"
#include "../sc_gui.h"
#include "sc_line.h"

void sc_draw_hline_fast(sc_pfb_t *dest, int16_t x1, int16_t x2, int16_t y, uint16_t color, uint8_t alpha)
{
    if (!dest || alpha < 4)
    {
        return;
    }
    if (y < dest->area.ys || y >= dest->area.ye)
    {
        return;
    }
    if (x1 > x2)
    {
        int16_t t = x1;
        x1 = x2;
        x2 = t;
    }
    int32_t xs = SC_MAX((int32_t)x1, (int32_t)dest->area.xs);
    int32_t xe = SC_MIN((int32_t)x2, (int32_t)dest->area.xe - 1);
    if (xs > xe)
    {
        return;
    }
    uint16_t *buf = sc_pfb_pixel_ptr(dest, xs, y);
    if (alpha > 251)
    {
        for (; xs <= xe; xs++)
        {
            *buf++ = color;
        }
    }
    else
    {
        const uint32_t fc_pack = sc_rgb565_pack_alpha(color);
        for (; xs <= xe; xs++, buf++)
        {
            sc_blend_ptr_fast(buf, color, fc_pack, alpha);
        }
    }
}

void sc_draw_vline_fast(sc_pfb_t *dest, int16_t x, int16_t y1, int16_t y2, uint16_t color, uint8_t alpha)
{
    if (!dest || alpha < 4)
    {
        return;
    }
    if (x < dest->area.xs || x >= dest->area.xe)
    {
        return;
    }
    if (y1 > y2)
    {
        int16_t t = y1;
        y1 = y2;
        y2 = t;
    }
    int32_t ys = SC_MAX((int32_t)y1, (int32_t)dest->area.ys);
    int32_t ye = SC_MIN((int32_t)y2, (int32_t)dest->area.ye - 1);
    if (ys > ye)
    {
        return;
    }
    uint16_t *buf = sc_pfb_pixel_ptr(dest, x, ys);
    if (alpha > 251)
    {
        for (; ys <= ye; ys++, buf += dest->width)
        {
            *buf = color;
        }
    }
    else
    {
        const uint32_t fc_pack = sc_rgb565_pack_alpha(color);
        for (; ys <= ye; ys++, buf += dest->width)
        {
            sc_blend_ptr_fast(buf, color, fc_pack, alpha);
        }
    }
}

void sc_draw_line_fast_aa(sc_pfb_t *dest, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color,
                          uint8_t alpha)
{
    if (!dest || alpha < 4)
    {
        return;
    }
    if (y1 == y2)
    {
        sc_draw_hline_fast(dest, x1, x2, y1, color, alpha);
        return;
    }
    if (x1 == x2)
    {
        sc_draw_vline_fast(dest, x1, y1, y2, color, alpha);
        return;
    }

    const int32_t dest_xs = dest->area.xs;
    const int32_t dest_ys = dest->area.ys;
    /* 裁剪用 area 实际宽高（width/height 是切片尺寸，area 可能是其子区域，
     * 用切片尺寸当边界会在子区域起点非切片起点时越界写）。 */
    const int32_t dest_w = (int32_t)(dest->area.xe - dest->area.xs);
    const int32_t dest_h = (int32_t)(dest->area.ye - dest->area.ys);
    const uint32_t fc_pack = sc_rgb565_pack_alpha(color);
    const int32_t dx = x2 - x1;
    const int32_t dy = y2 - y1;
    const int32_t adx = SC_ABS(dx);
    const int32_t ady = SC_ABS(dy);
    if (adx >= ady)
    {
        if (x1 > x2)
        {
            int16_t tx = x1;
            x1 = x2;
            x2 = tx;
            int16_t ty = y1;
            y1 = y2;
            y2 = ty;
        }
        const int32_t y_step = ((int32_t)(y2 - y1) << 16) / (x2 - x1);
        /* 增量像素指针：首像素地址一次算好，之后每 x 前进 1 + y 整数行，分数进位再 +width，
         * 避免每像素重算 sc_pfb_pixel_ptr（含乘法）。 */
        color_t *p = sc_pfb_pixel_ptr(dest, x1, y1);
        const int32_t y_inc = (y_step >> 16) * dest->width;
        const uint32_t y_rem = (uint32_t)(y_step - ((y_step >> 16) << 16));
        int32_t y_acc = (int32_t)y1 << 16;
        uint32_t y_frac = 0U;
        for (int32_t x = x1; x <= x2; x++)
        {
            int32_t y = y_acc >> 16;
            int32_t rel_x = x - dest_xs;
            int32_t rel_y = y - dest_ys;
            uint8_t frac = (uint8_t)(y_frac >> 8);
            if ((uint32_t)rel_x < (uint32_t)dest_w && rel_y >= -1 && rel_y < dest_h)
            {
                if (rel_y >= 0)
                {
                    sc_blend_ptr_fast(p, color, fc_pack,
                                      (uint8_t)(((uint16_t)(255 - frac) * alpha + 255) >> 8));
                }
                if (rel_y + 1 < dest_h)
                {
                    sc_blend_ptr_fast(p + dest->width, color, fc_pack,
                                      (uint8_t)(((uint16_t)frac * alpha + 255) >> 8));
                }
            }
            y_acc += y_step;
            p += 1 + y_inc;
            y_frac += y_rem;
            if (y_frac >= 0x10000U)
            {
                y_frac -= 0x10000U;
                p += dest->width;
            }
        }
    }
    else
    {
        if (y1 > y2)
        {
            int16_t tx = x1;
            x1 = x2;
            x2 = tx;
            int16_t ty = y1;
            y1 = y2;
            y2 = ty;
        }
        const int32_t x_step = ((int32_t)(x2 - x1) << 16) / (y2 - y1);
        color_t *p = sc_pfb_pixel_ptr(dest, x1, y1);
        const int32_t x_inc = x_step >> 16;
        const uint32_t x_rem = (uint32_t)(x_step - ((x_step >> 16) << 16));
        int32_t x_acc = (int32_t)x1 << 16;
        uint32_t x_frac = 0U;
        for (int32_t y = y1; y <= y2; y++)
        {
            int32_t x = x_acc >> 16;
            int32_t rel_x = x - dest_xs;
            int32_t rel_y = y - dest_ys;
            uint8_t frac = (uint8_t)(x_frac >> 8);
            if ((uint32_t)rel_y < (uint32_t)dest_h && rel_x >= -1 && rel_x < dest_w)
            {
                if (rel_x >= 0)
                {
                    sc_blend_ptr_fast(p, color, fc_pack,
                                      (uint8_t)(((uint16_t)(255 - frac) * alpha + 255) >> 8));
                }
                if (rel_x + 1 < dest_w)
                {
                    sc_blend_ptr_fast(p + 1, color, fc_pack,
                                      (uint8_t)(((uint16_t)frac * alpha + 255) >> 8));
                }
            }
            x_acc += x_step;
            p += dest->width + x_inc;
            x_frac += x_rem;
            if (x_frac >= 0x10000U)
            {
                x_frac -= 0x10000U;
                p += 1;
            }
        }
    }
}

void sc_draw_line_aa(sc_pfb_t *dest, int x1, int y1, int x2, int y2, int stroke_width, uint16_t color,
                     uint8_t round_cap, uint8_t alpha)
{
    if (!dest)
    {
        return;
    }
    const uint32_t fc_pack = sc_rgb565_pack_alpha(color);
    const int16_t alpha_cap = stroke_width == 1 ? 1 : 2;
    const int16_t margin = (int16_t)(stroke_width >> 1);
    area_t inter = {
        (int16_t)(SC_MIN(x1, x2) - margin),
        (int16_t)(SC_MIN(y1, y2) - margin),
        (int16_t)(SC_MAX(x1, x2) + margin + 1),
        (int16_t)(SC_MAX(y1, y2) + margin + 1),
    };
    if (!sc_pfb_intersection(dest, &inter, NULL))
    {
        return;
    }
    if (x1 == x2 && y1 == y2)
    {
        uint8_t a = (uint8_t)(((uint16_t)255 * alpha + 255) >> 8);
        sc_blend_ptr_fast(sc_pfb_pixel_ptr(dest, inter.xs, inter.ys), color, fc_pack, a);
        return;
    }

    const int32_t dx = x2 - x1;
    const int32_t dy = y2 - y1;
    const int32_t line_len_sq = dx * dx + dy * dy;
    const int32_t line_len = (int32_t)sc_isqrt_u32((uint32_t)line_len_sq);
    int32_t inner_th = (line_len * (stroke_width - alpha_cap)) >> 1;
    const int32_t outer_th = (line_len * (stroke_width + alpha_cap)) >> 1;
    if (inner_th < 0)
    {
        inner_th = 0;
    }
    const int32_t inv_range = (int32_t)SC_INV(outer_th, inner_th, 255 << 16);

    int32_t r_outer_sq = 0;
    int32_t r_inner_sq = 0;
    int32_t inv_cap_range = 0;
    if (round_cap == 2)
    {
        r_outer_sq = ((stroke_width + 1) * (stroke_width + alpha_cap)) >> 2;
        r_inner_sq = ((stroke_width - 1) * (stroke_width - alpha_cap)) >> 2;
        if (r_inner_sq < 0)
        {
            r_inner_sq = 0;
        }
        inv_cap_range = (r_outer_sq > r_inner_sq) ? (255 << 16) / (r_outer_sq - r_inner_sq) : 0;
    }
    else if (round_cap == 1)
    {
        /* 平帽：端点外 1 像素锐切（dot_dist = d*line_len → d=1 处淡到 0），避免角上多画。 */
        inv_cap_range = (line_len > 0) ? (255 << 16) / line_len : 0;
    }
    for (int y = inter.ys; y < inter.ye; y++)
    {
        const int32_t vy1 = y - y1;
        int32_t cross = (inter.xs - x1) * dy - vy1 * dx;
        int32_t dot = (inter.xs - x1) * dx + vy1 * dy;
        bool in_line_corridor = false;
        for (int x = inter.xs; x < inter.xe; x++)
        {
            const int32_t abs_cross = (cross < 0) ? -cross : cross;
            if (abs_cross < outer_th)
            {
                uint8_t final_alpha = 0;
                in_line_corridor = true;
                if (dot >= 0 && dot <= line_len_sq)
                {
                    if (abs_cross <= inner_th)
                    {
                        final_alpha = 255;
                    }
                    else
                    {
                        final_alpha = (uint8_t)(255 - (uint8_t)(((abs_cross - inner_th) * inv_range) >> 16));
                    }
                }
                else if (round_cap == 2)
                {
                    int32_t d2 = (dot < 0) ? (x - x1) : (x - x2);
                    if (dot < 0)
                    {
                        d2 = (d2 * d2) + (vy1 * vy1);
                    }
                    else
                    {
                        d2 = (d2 * d2) + ((y - y2) * (y - y2));
                    }
                    if (d2 < r_outer_sq)
                    {
                        if (d2 <= r_inner_sq)
                        {
                            final_alpha = 255;
                        }
                        else
                        {
                            final_alpha = (uint8_t)(255 - (uint8_t)(((d2 - r_inner_sq) * inv_cap_range) >> 16));
                        }
                    }
                }
                else if (round_cap == 1)
                {
                    const int32_t dot_dist = (dot < 0) ? -dot : (dot - line_len_sq);
                    final_alpha = (uint8_t)SC_MAX(0, 255 - ((dot_dist * inv_cap_range) >> 16));
                    if (abs_cross > inner_th)
                    {
                        uint8_t side_alpha  = (uint8_t)(255 - (uint8_t)(((abs_cross - inner_th) * inv_range) >> 16));
                        final_alpha = SC_MIN(final_alpha, side_alpha );    
                    }
                }
                if (final_alpha > 0)
                {
                    final_alpha = (uint8_t)(((uint16_t)final_alpha * alpha + 255) >> 8);
                    sc_blend_ptr_fast(sc_pfb_pixel_ptr(dest, x, y), color, fc_pack, final_alpha);
                }
            }
            else if (in_line_corridor)
            {
                break;
            }
            cross += dy;
            dot += dx;
        }
    }
}

static void sc_draw_line_props(sc_pfb_t *pfb, const sc_line_t *props, rect_t view)
{
    const int x1 = view.x + props->x1;
    const int y1 = view.y + props->y1;
    const int x2 = view.x + props->x2;
    const int y2 = view.y + props->y2;
    if (props->stroke_width <= 1)
    {
        sc_draw_line_fast_aa(pfb, (int16_t)x1, (int16_t)y1, (int16_t)x2, (int16_t)y2, props->color, props->opa);
    }
    else
    {
        sc_draw_line_aa(pfb, x1, y1, x2, y2, props->stroke_width, props->color, props->round_cap, props->opa);
    }
}

void sc_lcd_draw_line_widget(sc_pfb_t *pfb, int16_t x, int16_t y,
                             uint16_t w, uint16_t h, const sc_line_t *props)
{
    if (!props)
    {
        return;
    }
    const rect_t view = {x, y, w, h};
    if (pfb)
    {
        sc_draw_line_props(pfb, props, view);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_line_props(sc_lcd_draw_pfb__, props, view));
}

#if SC_USER_OBJ && SC_WIDGET_LINE_ENABLE
static const sc_obj_vt_t sc_line_vt = {
    .event_cb = sc_obj_handle_line_event,
    .set_style = sc_line_style,
};

sc_obj_t *sc_line_create(sc_obj_t *parent, sc_line_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_LINE, &sc_line_vt, props);
    if (obj)
    {
        *props = (sc_line_t)SC_LINE_NEW(C_WHITE, 1U);
    }
    return obj;
}

void sc_line_set_points(sc_obj_t *obj, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    sc_line_t *props = (sc_line_t *)obj->props;
    /* 端点原始包围盒（父坐标）。 */
    const int16_t xmin = (int16_t)SC_MIN(x1, x2);
    const int16_t ymin = (int16_t)SC_MIN(y1, y2);
    rect_t view = {xmin, ymin,
                   (uint16_t)(SC_MAX(x1, x2) - xmin + 1),
                   (uint16_t)(SC_MAX(y1, y2) - ymin + 1)};
    uint16_t pad = (uint16_t)(props->stroke_width + 1U) >> 1U;
    view = rect_grow(view, (int16_t)pad, (int16_t)pad);
    /* 定位 view，并把端点重定位回 view 原点（相对量，线随 view 平移）。 */
    sc_obj_set_rect(obj, view.x, view.y, view.w, view.h, ALIGN_NONE);
    props->x1 = (int16_t)(x1 - view.x);
    props->y1 = (int16_t)(y1 - view.y);
    props->x2 = (int16_t)(x2 - view.x);
    props->y2 = (int16_t)(y2 - view.y);
}

bool sc_line_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_line_t *props = (sc_line_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->color = *(const color_t *)value;
        return true;
    case SC_STYLE_OPA:
        props->opa = *(const uint8_t *)value;
        return true;
    default:
        return false;
    }
}

bool sc_obj_handle_line_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_line_props((sc_pfb_t *)event->dat.arg, (const sc_line_t *)obj->props, obj->view);
        return true;
    }
    return false;
}

#endif /* SC_USER_OBJ && SC_WIDGET_LINE_ENABLE */
