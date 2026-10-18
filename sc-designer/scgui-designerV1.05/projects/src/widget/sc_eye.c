#include "../sc_gui.h"
#include "../sc_draw.h" /* SC_LCD_DRAW（裸绘封装用） */
#include "sc_eye.h"

/* 抛物线包围盒：顶点 + 描边 pad + AA ±1 余量（绘制与裸绘封装共用，省一份常量计算）。
 * h=0：退化为顶点处水平线，包围盒 = 线 + 两侧描边走廊。 */
static inline void sc_parabola_area(int cx, int cy, int w, int h, int dir, int stroke_width, area_t *area)
{
    const int pad = (stroke_width + 2) >> 1;
    const int half_w = (w + 1) >> 1;
    area->xs = (int16_t)(cx - half_w - pad - 1);
    area->xe = (int16_t)(cx + half_w + pad + 1);
    if (h == 0)
    {
        area->ys = (int16_t)(cy - pad - 1);
        area->ye = (int16_t)(cy + pad + 2);
    }
    else
    {
        area->ys = (int16_t)((dir > 0) ? (cy - pad - 1) : (cy - h));
        area->ye = (int16_t)((dir > 0) ? (cy + h) : (cy + pad + 2));
    }
}

/* 睁眼深度缩放：blink 0..255（0=闭眼 1px 缝，255=最大开眼）→ 0..h（h=0 退化水平线）。
 * 除法 /255 改乘倒数 >>8（255 的 Q8 倒数 = 256/255≈1，即 (x+128)>>8 近似 /255 取整，
 * MCU 无硬件除法）。绘制与脏区共用，保证深度一致。 */
static inline int sc_eye_depth(int16_t h, uint8_t blink)
{
    return SC_MAX(0, ((int)h * blink + 128) >> 8);
}

/* 抛物线绘制（边框+填充，不透明）：描边走廊内画 color（1px AA），走廊内侧（f<0 = 曲线
 * 凸向一侧）画 fill（texture 非 NULL 时填充取纹理色，坐标相对 tex_origin——上下两条共用
 * 同一原点，整眼一张纹理；fill == g_gui->bg 且无纹理时只描边）。按 dest pfb 区域裁剪。 */
bool sc_draw_parabola_ex(sc_pfb_t *dest, int cx, int cy, int w, int h, int dir, int stroke_width,
                         color_t color, color_t fill, sc_texture_t *texture,
                         int tex_origin_x, int tex_origin_y)
{
    if (!dest || w <= 0 || h < 0 || stroke_width <= 0)
    {
        return false;
    }
    area_t inter; /* 包围盒就地裁剪，直接复用 inter */
    sc_parabola_area(cx, cy, w, h, dir, stroke_width, &inter);
    if (!sc_pfb_intersection(dest, &inter, NULL))
    {
        return false;
    }
    const uint32_t w2 = (uint32_t)w * (uint32_t)w;
    const bool need_fill = (fill != g_gui->bg) || (texture != NULL);
    int32_t solid_q8 = ((int32_t)stroke_width << 7) - 128;
    if (solid_q8 < 0)
    {
        solid_q8 = 0;
    }
    const uint32_t fade_limit_q8 = ((uint32_t)stroke_width << 7) + 128U;
    const uint32_t fade_inv_q8 = SC_INV(fade_limit_q8, (uint32_t)solid_q8, 255 << 8);
    const int half_stroke = (stroke_width + 1) >> 1;
    const int pad = (stroke_width + 2) >> 1; /* 描边走廊半宽（h=0 退化线用） */
    const int half_w = (w + 1) >> 1;         /* 抛物线半宽（h=0 退化线整行扫描用） */
    /* h=0：抛物线退化为顶点处水平线（闭眼），K 除零防护 + 整行扫描。 */
    const uint32_t K = (h > 0) ? ((uint32_t)(w >> 1) * (uint32_t)(w >> 1)) / (uint32_t)h : 0U;
    const int x_margin = half_stroke + 2;
    for (int16_t py = inter.ys; py < inter.ye; py++)
    {
        const int32_t dy = (dir > 0) ? (py - cy) : (cy - py);
        int16_t x_start = inter.xs;
        int16_t x_end = inter.xe;
        if (dy < -half_stroke || dy >= h + half_stroke + ((h == 0) ? pad + 1 : 0))
        {
            continue;
        }
        if (dy >= 0)
        {
            const uint32_t dx2_max = K * (uint32_t)(dy + 1);
            const int32_t x_limit = (h > 0) ? (int32_t)sc_isqrt_u32(dx2_max) : half_w;
            const int32_t left = cx - x_limit - x_margin;
            const int32_t right = cx + x_limit + x_margin + 1;
            if (left > x_start)
            {
                x_start = (int16_t)left;
            }
            if (right < x_end)
            {
                x_end = (int16_t)right;
            }
            if (x_start >= x_end)
            {
                continue;
            }
        }
        /* 行首像素指针一次算好，之后每 x 前进 1，避免逐点重算 sc_pfb_pixel_ptr（含乘法）。 */
        color_t *buf = sc_pfb_pixel_ptr(dest, x_start, py);
        bool in_line_corridor = false;
        for (int16_t px = x_start; px < x_end; px++, buf++)
        {
            int32_t dx = px - cx;
            const bool is_left = dx < 0;
            uint8_t stroke_alpha = 0;
            if (is_left)
            {
                dx = -dx;
            }
            const int32_t u = (dx << 1) + 1;
            const uint32_t abs_u = (uint32_t)((u < 0) ? -u : u);
            const int32_t f = (int32_t)h * (int32_t)abs_u * (int32_t)abs_u - (int32_t)w2 * dy;
            uint32_t grad = sc_hypot_fast((int32_t)(4 * h * abs_u), (int32_t)w2);
            if (grad == 0U)
            {
                grad = 1U;
            }
            const uint32_t abs_f = (f < 0) ? (uint32_t)(-f) : (uint32_t)f;
            const uint32_t dist_q8 = (uint32_t)(((uint64_t)abs_f << 8) / grad);
            /* 由外向内判断：走廊描边 alpha → 内一分支（填充+过渡，纹理取色仅此一处）→
             * 走廊外终止/背景 → 外侧抗锯齿。填充区必须先于右侧 break 被接住。 */
            if (dist_q8 < fade_limit_q8)
            {
                if (!is_left)
                {
                    in_line_corridor = true;
                }
                stroke_alpha = (dist_q8 <= (uint32_t)solid_q8)
                                   ? 255
                                   : (uint8_t)(((fade_limit_q8 - dist_q8) * fade_inv_q8) >> 8);
            }
            if (need_fill && f < 0 && h > 0)
            {
                /* 内侧统一（填充区 + 走廊过渡）：stroke_alpha=0 → 纯填充，消除边界背景孔；
                 * 有纹理取纹理色（坐标相对 tex_origin，整眼一张纹理）。
                 * h=0 退化水平线无内部：不画填充（纯描边线）。 */
                if (stroke_alpha >= 250U)
                {
                    *buf = color; /* 走廊实心：纯描边色 */
                }
                else
                {
                    *buf = alphaBlend(color, texture ? texture->get_color(texture, (uint16_t)(px - tex_origin_x), (uint16_t)(py - tex_origin_y)) : fill,
                                      stroke_alpha);
                }
                continue;
            }
            if (dist_q8 >= fade_limit_q8)
            {
                if (!is_left && in_line_corridor)
                {
                    break;
                }
                continue;
            }
            if (stroke_alpha > 0)
            {
                /* 外侧：抗锯齿到背景（纯描边色，不混填充/纹理）。 */
                *buf = alphaBlend(color, *buf, stroke_alpha);
            }
        }
    }
    return true;
}

/* 裸绘封装：pfb 非 NULL 直接绘入；NULL 走 SC_LCD_DRAW 切片刷新（用户裸绘测试入口）。
 * 用法（SDL demo main 或工程 main 里直接调）：
 *   sc_lcd_draw_parabola_ex(NULL, 160, 120, 120, 50,  1, 2, C_WHITE, C_RED, NULL, 0, 0); // 上抛物线
 *   sc_lcd_draw_parabola_ex(NULL, 160, 220, 120, 50, -1, 2, C_WHITE, C_RED, NULL, 0, 0); // 下抛物线 */
void sc_lcd_draw_parabola_ex(sc_pfb_t *pfb, int cx, int cy, int w, int h, int dir,
                             int stroke_width, color_t color, color_t fill,
                             sc_texture_t *texture, int tex_origin_x, int tex_origin_y)
{
    if (pfb)
    {
        sc_draw_parabola_ex(pfb, cx, cy, w, h, dir, stroke_width, color, fill, texture,
                            tex_origin_x, tex_origin_y);
        return;
    }
    area_t dirty;
    sc_parabola_area(cx, cy, w, h, dir, stroke_width, &dirty);
    SC_LCD_DRAW(dirty, sc_draw_parabola_ex(sc_lcd_draw_pfb__, cx, cy, w, h, dir,
                                           stroke_width, color, fill, texture,
                                           tex_origin_x, tex_origin_y));
}

/* 杏仁眼：两条抛物线各画边框+内侧填充（含纹理），合围成杏仁（不透明绘制）。
 * 裁剪由各抛物线内部完成（sc_draw_parabola_ex 各自算包围盒+交集），眼部无需重复。 */
void sc_draw_eye(sc_pfb_t *dest, rect_t view, const sc_eye_t *p)
{
    const int cx = view.x + p->cx;
    const int cy = view.y + p->cy;
    const int w = p->w;
        /* 闭眼缩放：blink=0 时深度收敛到 1px 缝（闭合的线），view 不变避免每帧重排。 */
    const int eu = sc_eye_depth(p->upper_h, p->blink);
    const int el = sc_eye_depth(p->lower_h, p->blink);
    if (!dest || w <= 0 || p->upper_h <= 0 || p->lower_h <= 0)
    {
        return;
    }
    /* 上抛物线（顶点在上，向下开）+ 下抛物线（顶点在下，向上开）：
     * 描边 + 走廊内侧填充（fill/texture 共用 view 原点 → 整眼一张纹理）。 */
    sc_draw_parabola_ex(dest, cx, cy - eu, w, eu, 1, p->stroke_width, p->color, p->fill, p->texture,
                        view.x, view.y);
    sc_draw_parabola_ex(dest, cx, cy + el, w, el, -1, p->stroke_width, p->color, p->fill, p->texture,
                        view.x, view.y);
}

#if SC_USER_OBJ && SC_WIDGET_EYE_ENABLE
static const sc_obj_vt_t sc_eye_vt = {
    .event_cb = sc_obj_handle_eye_event,
    .set_style = sc_eye_style,
};

sc_obj_t *sc_eye_create(sc_obj_t *parent, sc_eye_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_EYE, &sc_eye_vt, props);
    if (obj)
    {
        /* 逐成员写默认值（不整结构体赋 SC_EYE_NEW，便于 codegen 逐字段 diff）；
         * cx/cy 由 sc_eye_set_shape 运行时写值。 */
        props->color = C_WHITE;
        props->fill = C_BLUE;
        props->w = 40;
        props->upper_h = 20;
        props->lower_h = 20;
        props->stroke_width = 1U;
        props->blink = 255U; /* 默认最大开眼（0=闭眼 1px 缝） */
        props->texture = NULL;
    }
    return obj;
}

/* 眼形定位：view 取杏仁 AABB（含描边 pad 与 AA ±1 余量），中心/深度重定位回 view 原点。 */
void sc_eye_set_shape(sc_obj_t *obj, int16_t cx, int16_t cy, int16_t w, int16_t upper_h, int16_t lower_h)
{
    sc_eye_t *props = (sc_eye_t *)obj->props;
    const int16_t half_w = (int16_t)((w + 1) >> 1);
    const int16_t pad = (int16_t)((props->stroke_width + 2) >> 1) + 1;
    rect_t view;
    view.x = (int16_t)(cx - half_w - pad);
    view.w = (uint16_t)(2 * half_w + 2 * pad);
    view.y = (int16_t)(cy - upper_h - pad);
    view.h = (uint16_t)(upper_h + lower_h + 2 * pad);
    sc_obj_set_rect(obj, view.x, view.y, view.w, view.h, ALIGN_NONE);
    props->cx = (int16_t)(cx - view.x);
    props->cy = (int16_t)(cy - view.y);
    props->w = w;
    props->upper_h = upper_h;
    props->lower_h = lower_h;
}

void sc_eye_set_blink(sc_obj_t *obj, uint8_t blink)
{
    sc_eye_t *props = (sc_eye_t *)obj->props;
    const uint8_t old = props->blink;
    props->blink = blink;
    /* 脏区驱动：合并旧/新有效高度（上下抛物线顶点 + 描边 pad + AA 余量），
     * 宽度不变，只重绘实际变化的纵向范围，避免整 view 刷新。 */
    const int cy = obj->view.y + props->cy;
    const int pad = (props->stroke_width + 2) >> 1;
    const int eu_old = sc_eye_depth(props->upper_h, old);
    const int eu_new = sc_eye_depth(props->upper_h, blink);
    const int el_old = sc_eye_depth(props->lower_h, old);
    const int el_new = sc_eye_depth(props->lower_h, blink);
    area_t dirty = {
        obj->view.x,
        (int16_t)(cy - SC_MAX(eu_old, eu_new) - pad),
        (int16_t)(obj->view.x + obj->view.w),
        (int16_t)(cy + SC_MAX(el_old, el_new) + pad),
    };
    sc_obj_invalidate_area(obj, &dirty);
}

bool sc_eye_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_eye_t *props = (sc_eye_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->color = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->fill = *(const color_t *)value;
        return true;
    case SC_STYLE_BORDER_WIDTH:
        props->stroke_width = *(const uint8_t *)value;
        return true;
    case SC_STYLE_TEXTURE:
        return sc_obj_apply_texture(&props->texture, obj->view, value);
    default:
        return false;
    }
}

/* 纯显示控件：只处理绘制，无交互事件。 */
bool sc_obj_handle_eye_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_eye((sc_pfb_t *)event->dat.arg, obj->view, (const sc_eye_t *)obj->props);
        return true;
    }
    return false;
}

#endif /* SC_USER_OBJ && SC_WIDGET_EYE_ENABLE */
