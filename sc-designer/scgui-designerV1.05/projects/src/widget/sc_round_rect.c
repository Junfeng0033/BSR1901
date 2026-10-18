#include "../sc_draw.h"
#include "sc_round_rect.h"

/* 圆角滚动条：圆角轨道 + 滑块（无边框）。vol 为滚动偏移（负值=内容下移），
 * 滑块长度按剩余可滚距离自适应（列表/文本框共用）。 */
void sc_draw_bar(sc_pfb_t *dest, rect_t box, int r, color_t fc, color_t bc,
                 uint8_t alpha, int vol)
{
    if (!sc_draw_Rounded_mask(dest, box, r, 0, bc, bc, alpha, 0U, NULL))
    {
        return;
    }
    int max_vol = SC_MAX(box.w, box.h);
    int bar_vol = max_vol >> 1;
    if (vol < 0)
    {
        bar_vol = bar_vol * max_vol / (max_vol - vol);
        vol = 0;
    }
    else if (vol + bar_vol >= max_vol)
    {
        bar_vol = bar_vol * max_vol / (vol + bar_vol);
        vol = max_vol - bar_vol;
    }
    bool along_x = box.w > box.h;
    rect_t bar = {
        (int16_t)(along_x ? box.x + vol : box.x),
        (int16_t)(along_x ? box.y : box.y + vol),
        (uint16_t)(along_x ? bar_vol : box.w),
        (uint16_t)(along_x ? box.h : bar_vol),
    };
    (void)sc_draw_Rounded_mask(dest, bar, r, 0, fc, fc, alpha, 0U, NULL);
}

/* Draw an anti-aliased rounded rectangle whose fill comes from a texture; falls back to solid fill if none. */
bool sc_draw_Rounded_texture(sc_pfb_t *dest, rect_t view, int r, int border,
                             color_t fc_color, color_t fill, uint8_t alpha,
                             sc_texture_t *texture, uint8_t rect_mask,
                             const area_t *clip)
{
    area_t inter = rect_to_area(view);
    if (!sc_pfb_intersection(dest, &inter, clip))
        return 0;
    const bool solid = border == 0;
    if (solid)
    {
        fc_color = fill;
    }
    if (r == 0)
    {
        r = border;
        rect_mask = 0x0f;
    }
    const bool need_fill = (fill != g_gui->bg);
    const uint32_t fc_pack = sc_rgb565_pack_alpha(fc_color);
    const uint32_t fill_pack = sc_rgb565_pack_alpha(fill);
    // --- 1px AA 预计算 ---
    const int ir = r - border;
    const uint32_t rmax = SC_POW2(r + 1);
    const uint32_t r2 = SC_POW2(r);
    const uint32_t ir2 = SC_POW2(ir + 1);
    const uint32_t rmin = (ir > 0) ? SC_POW2(ir) : 0;
    const uint32_t inv_outer = SC_INV(rmax, r2, alpha << 8);
    const uint32_t inv_inner = solid ? 0U : SC_INV(ir2, rmin, alpha << 8);
    const int ax = view.x + r;
    const int bx = view.x + view.w - r - 1;
    const int ay = view.y + r;
    const int by = view.y + view.h - r - 1;
    const int left = ax - ir;
    const int right = bx + ir;
    uint16_t *dest_buf = sc_pfb_pixel_ptr(dest, inter.xs, inter.ys);
    int x, y;
    for (y = inter.ys; y < inter.ye; y++, dest_buf += dest->width)
    {
        uint16_t *dest_out = dest_buf;
        const uint16_t local_y = (uint16_t)(y - view.y);
        // 中间填充区域(y >= ay && y <= by)
        if ((uint16_t)(y - ay) <= (by - ay)) // 优化写法
        {
            if (texture)
            {
                for (x = inter.xs; x < inter.xe; x++, dest_out++)
                {
                    if (x < left || x > right) // 左右垂直线，线宽判断
                    {
                        *dest_out = sc_blend_rgb565_fast(*dest_out, fc_color, fc_pack, alpha);
                    }
                    else
                    {
                        uint16_t tc = texture->get_color(texture, (uint16_t)(x - view.x), local_y);
                        *dest_out = alphaBlend(tc, *dest_out, alpha);
                    }
                }
            }
            else
            {
                for (x = inter.xs; x < inter.xe; x++, dest_out++)
                {
                    if (x < left || x > right) // 左右垂直线，线宽判断
                    {
                        *dest_out = sc_blend_rgb565_fast(*dest_out, fc_color, fc_pack, alpha);
                    }
                    else if (need_fill)
                    {
                        *dest_out = sc_blend_rgb565_fast(*dest_out, fill, fill_pack, alpha);
                    }
                }
            }
            continue;
        }
        int y_rel = y < by ? ay - y : y - by;
        uint32_t Ysq = SC_POW2(y_rel); // y_rel^2
        uint8_t mask_y = (y < ay) ? 0 : 2;
        for (x = inter.xs; x < inter.xe; x++, dest_out++)
        {
            // 水平直线区域(x >= ax && x <= bx)
            if ((uint16_t)(x - ax) <= (bx - ax)) // 优化写法
            {
                if (Ysq > rmin) // 上下水平直线，线宽判断
                {
                    *dest_out = sc_blend_rgb565_fast(*dest_out, fc_color, fc_pack, alpha);
                }
                else if (texture)
                {
                    uint16_t tc = texture->get_color(texture, (uint16_t)(x - view.x), local_y);
                    *dest_out = alphaBlend(tc, *dest_out, alpha);
                }
                else if (need_fill)
                {
                    *dest_out = sc_blend_rgb565_fast(*dest_out, fill, fill_pack, alpha);
                }
                continue;
            }
            int x_rel = x > ax ? x - bx : ax - x;
            if (rect_mask & ((x < ax) ? (1 << mask_y) : 1 << (mask_y + 1)))
            {
                if (x_rel > ir || y_rel > ir)
                {
                    *dest_out = sc_blend_rgb565_fast(*dest_out, fc_color, fc_pack, alpha);
                }
                else if (texture)
                {
                    uint16_t tc = texture->get_color(texture, (uint16_t)(x - view.x), local_y);
                    *dest_out = alphaBlend(tc, *dest_out, alpha);
                }
                else if (need_fill)
                {
                    *dest_out = sc_blend_rgb565_fast(*dest_out, fill, fill_pack, alpha);
                }
                continue;
            }
            uint32_t Rsq = SC_POW2(x_rel) + Ysq; // =R*R
            if (Rsq >= rmax)
            {
                if (x >= bx && x > ax)
                    break; // 右侧圆角外，本行后续无需计算
                continue;
            }
            uint16_t tc = !texture ? fill : texture->get_color(texture, (uint16_t)(x - view.x), local_y);
            if (Rsq > r2) // inv_outer
            {
                uint8_t fc_a = (uint8_t)((rmax - Rsq) * inv_outer >> 8);
                *dest_out = alphaBlend(solid ? tc : fc_color, *dest_out, fc_a);
            }
            else if (solid || Rsq <= rmin)
            {
                if (need_fill || texture)
                {
                    *dest_out = alphaBlend(tc, *dest_out, alpha);
                }
            }
            else if (Rsq < ir2) // inner
            {
                uint8_t in_a = (uint8_t)((Rsq - rmin) * inv_inner >> 8);
                if (need_fill || texture)
                {
                    uint16_t ac = alphaBlend(fc_color, tc, in_a);
                    *dest_out = alphaBlend(ac, *dest_out, alpha);
                }
                else
                {
                    *dest_out = sc_blend_rgb565_fast(*dest_out, fc_color, fc_pack, in_a);
                }
            }
            else // border
            {
                *dest_out = sc_blend_rgb565_fast(*dest_out, fc_color, fc_pack, alpha);
            }
        }
    }
    return 1;
}
/* Draw a textured rounded rectangle to the LCD via the given pfb or the LCD draw macro. */
void sc_lcd_draw_Rounded_texture(sc_pfb_t *pfb, int16_t x, int16_t y,
                                 uint16_t w, uint16_t h, int radius, int border,
                                 color_t fc_color, color_t fill, uint8_t alpha,
                                 sc_texture_t *texture, uint8_t rect_mask)
{
    if (texture && !texture->get_color)
    {
        return;
    }
    rect_t view = {x, y, w, h};
    if (pfb)
    {
        (void)sc_draw_Rounded_texture(pfb, view, radius, border,
                                      fc_color, fill, alpha, texture, rect_mask, NULL);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view),
                (void)sc_draw_Rounded_texture(sc_lcd_draw_pfb__, view, radius, border,
                                              fc_color, fill, alpha, texture, rect_mask, NULL));
}
void sc_lcd_draw_Rounded_mask(sc_pfb_t *pfb, int16_t x, int16_t y,
                              uint16_t w, uint16_t h, int radius, int border,
                              color_t fc_color, color_t fill, uint8_t alpha,
                              uint8_t rect_mask)
{
    sc_lcd_draw_Rounded_texture(pfb, x, y, w, h, radius, border,
                                fc_color, fill, alpha, NULL, rect_mask);
}

#if SC_USER_OBJ && SC_WIDGET_ROUND_RECT_ENABLE

/* Create a focusable rounded-rectangle object under parent and initialize its props with defaults. */
static const sc_obj_vt_t sc_round_rect_vt = {
    .event_cb = sc_obj_handle_round_rect_event,
    .set_style = sc_round_rect_style,
};

sc_obj_t *sc_round_rect_create(sc_obj_t *parent, sc_rounded_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_ROUND_RECT, &sc_round_rect_vt, props);
    if (obj)
    {
        *props = (sc_rounded_t)SC_ROUNDED_NEW(C_WHITE, C_BLUE, 8U, 1U);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

/* Plain rect: shares style/event with round_rect; distinct type and a texture-less struct. */
sc_obj_t *sc_plain_rect_create(sc_obj_t *parent, sc_plain_rect_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_PLAIN_RECT, &sc_round_rect_vt, props);
    if (obj)
    {
        *props = (sc_plain_rect_t)SC_PLAIN_RECT_NEW(C_WHITE, C_BLUE, 8U, 1U);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

/* Apply a style property; common fields (color/fill/alpha/radius/border) are shared with
 * plain_rect via the identical struct prefix; round_rect-only fields follow by type. */
bool sc_round_rect_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_plain_rect_t *common = (sc_plain_rect_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
    case SC_STYLE_BORDER:
        common->color = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        common->fill = *(const color_t *)value;
        return true;
    case SC_STYLE_OPA:
        common->alpha = *(const uint8_t *)value;
        return true;
    case SC_STYLE_RADIUS:
        common->radius = *(const uint8_t *)value;
        return true;
    case SC_STYLE_BORDER_WIDTH:
        common->border = *(const uint8_t *)value;
        return true;
    case SC_STYLE_RECT_MASK:
        common->rect_mask = *(const uint8_t *)value;
        return true;
    case SC_STYLE_RADIUS_READ:
        *(uint8_t *)value = ((const sc_plain_rect_t *)obj->props)->radius;
        return true;

    default:
        break;
    }
    if (sc_obj_type(obj) == SC_OBJ_PLAIN_RECT)
    {
        return false;
    }
    sc_rounded_t *props = (sc_rounded_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_CALLBACK:
        props->cb = *(const sc_obj_cb_t *)value;
        return true;
    case SC_STYLE_TEXTURE:
        return sc_obj_apply_texture(&props->texture, obj->view, value);
    default:
        return false;
    }
}

/* Handle round-rect/plain-rect events: draw on redraw. plain_rect is passive;
 * round_rect additionally tracks press/touch and runs its callback. */
bool sc_obj_handle_round_rect_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_pfb_t *pfb = (sc_pfb_t *)event->dat.arg;
        if (sc_obj_type(obj) == SC_OBJ_PLAIN_RECT)
        {
            const sc_plain_rect_t *p = (const sc_plain_rect_t *)obj->props;
            sc_draw_Rounded_texture(pfb, obj->view,
                                    p->radius, p->border, p->color, p->fill, p->alpha, NULL,
                                    p->rect_mask, NULL);
        }
        else
        {
            const sc_rounded_t *p = (const sc_rounded_t *)obj->props;
            sc_draw_Rounded_texture(pfb, obj->view,
                                    p->radius, p->border, p->color, p->fill, p->alpha, p->texture,
                                    p->rect_mask, NULL);
        }
        return true;
    }
    /* 公共按下/释放状态机；plain_rect 结构更短无 cb 字段，传 NULL 不回调。 */
    return sc_obj_common_event(obj, event,
                               sc_obj_type(obj) == SC_OBJ_ROUND_RECT ? ((sc_rounded_t *)obj->props)->cb : NULL);
}

#endif /* SC_USER_OBJ && SC_WIDGET_ROUND_RECT_ENABLE */
