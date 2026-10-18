

#include "sc_draw.h"

#define SC_DIRTY_SIMPLE_MERGE_ENABLE (SC_DIRTY_BUCKET_N < 8) // 简单合并,不考虑阈值，适合控件少的场景

/* dirty area queue */
static area_t sc_dirty_buckets[SC_DIRTY_BUCKET_N];
static uint8_t sc_dirty_bucket_cnt = 0;

#if SC_USER_OBJ
/* Merge two dirty areas into *merged if they overlap (or are close enough per threshold); returns false if they should stay separate. */
static bool sc_dirty_merge_if_overlap(const area_t *a, const area_t *b, area_t *merged, int threshold)
{
    if (area_nor_intersect(a, b))
    {
        return false;
    }
#if (!SC_DIRTY_SIMPLE_MERGE_ENABLE)
    /* containment check */
    if ((a->xs <= b->xs && a->ys <= b->ys && a->xe >= b->xe && a->ye >= b->ye) ||
        (b->xs <= a->xs && b->ys <= a->ys && b->xe >= a->xe && b->ye >= a->ye))
    {
        *merged = area_merged(*a, *b);
        return true;
    }
    /* threshold check */
    int16_t xs = SC_MAX(a->xs, b->xs);
    int16_t ys = SC_MAX(a->ys, b->ys);
    int16_t xe = SC_MIN(a->xe, b->xe);
    int16_t ye = SC_MIN(a->ye, b->ye);
    if ((int)(xe - xs) * (ye - ys) >= threshold)
    {
        *merged = area_merged(*a, *b);
        return true;
    }
    return false;
#else
    (void)threshold;
    *merged = area_merged(*a, *b);
    return true;
#endif
}
/* Squared distance between the nearest edges of two areas (0 if they overlap). */
static inline int sc_dirty_nearest_dist(const area_t *a, const area_t *b)
{
    int dx = (a->xe <= b->xs) ? (b->xs - a->xe) : (b->xe <= a->xs) ? (a->xs - b->xe)
                                                                   : 0;
    int dy = (a->ye <= b->ys) ? (b->ys - a->ye) : (b->ye <= a->ys) ? (a->ys - b->ye)
                                                                   : 0;
    return dx * dx + dy * dy;
}
#endif

/* Add an area to the dirty queue, merging into an existing bucket when possible or into the nearest bucket when full. */
static void sc_dirty_area_push(const area_t *area)
{
#if SC_USER_OBJ
#if (!SC_DIRTY_SIMPLE_MERGE_ENABLE)
    int16_t threshold = (sc_dirty_bucket_cnt >= SC_DIRTY_BUCKET_N / 2) ? 0 : SC_DIRTY_AREA_THRESHOLD;
#endif
    for (int i = sc_dirty_bucket_cnt - 1; i >= 0; i--)
    {
        area_t *target = &sc_dirty_buckets[i];
#if SC_DIRTY_SIMPLE_MERGE_ENABLE
        if (sc_dirty_merge_if_overlap(target, area, target, 0))
#else
        if (sc_dirty_merge_if_overlap(target, area, target, threshold))
#endif
            return;
    }
    if (sc_dirty_bucket_cnt < SC_DIRTY_BUCKET_N)
    {
        sc_dirty_buckets[sc_dirty_bucket_cnt++] = *area;
    }
    else
    {
        area_t *merged = &sc_dirty_buckets[0];
        int min = 0x7fffffff;
        for (int i = sc_dirty_bucket_cnt - 1; i >= 0; i--)
        {
            area_t *target = &sc_dirty_buckets[i];
            int d = sc_dirty_nearest_dist(target, area);
            if (d < min)
            {
                min = d;
                merged = target;
            }
        }
        *merged = area_merged(*merged, *area);
    }
#else
    if (sc_dirty_bucket_cnt == 0U)
    {
        sc_dirty_buckets[0] = *area;
        sc_dirty_bucket_cnt = 1U;
    }
    else
    {
        sc_dirty_buckets[0] = area_merged(sc_dirty_buckets[0], *area);
    }
#endif
}

/* Return the current dirty-area buckets (optionally compacted by merging) and reset the queue; *valid_cnt receives the count. */
area_t *sc_dirty_pop(uint8_t *valid_cnt)
{
    if (sc_dirty_bucket_cnt == 0U)
    {
        return NULL;
    }
    *valid_cnt = sc_dirty_bucket_cnt;
#if SC_USER_OBJ && (!SC_DIRTY_SIMPLE_MERGE_ENABLE)
    if (sc_dirty_bucket_cnt > 2U)
    {
        uint8_t write = 0;
        for (uint8_t i = 0; i < sc_dirty_bucket_cnt; ++i)
        {
            bool is_merged = false;
            area_t cur = sc_dirty_buckets[i];
            for (uint8_t j = 0; j < write; ++j)
            {
                if (sc_dirty_merge_if_overlap(&cur, &sc_dirty_buckets[j], &sc_dirty_buckets[j], SC_DIRTY_AREA_THRESHOLD))
                {
                    is_merged = true;
                    break;
                }
            }
            if (!is_merged)
            {
                sc_dirty_buckets[write++] = cur;
            }
        }
        *valid_cnt = write;
    }
#endif
    sc_dirty_bucket_cnt = 0;
    return sc_dirty_buckets;
}

/* Clip an area to the screen and queue it as dirty; returns false if it is empty after clipping. */
bool sc_invalidate_area(area_t area)
{
    area.xs = SC_MAX(area.xs, 0);
    area.ys = SC_MAX(area.ys, 0);
    /* 运行时屏幕尺寸（桥 set_screen 可改；MCU 恒为编译期宏）。 */
    area.xe = SC_MIN(area.xe, g_gui->screen.w);
    area.ye = SC_MIN(area.ye, g_gui->screen.h);
    if (area.xs >= area.xe || area.ys >= area.ye)
    {
        return false;
    }
    sc_dirty_area_push(&area);
    return true;
}

/* Mark the entire screen as dirty, split into 4 buckets (2x2) to reduce PFB memory per flush. */
void sc_invalidate_all(void)
{
    uint16_t hw = g_gui->screen.w / 2U;
    uint16_t hh = g_gui->screen.h / 2U;
    sc_dirty_buckets[0] = (area_t){0, 0, hw, hh};
    sc_dirty_buckets[1] = (area_t){hw, 0, g_gui->screen.w, hh};
    sc_dirty_buckets[2] = (area_t){0, hh, hw, g_gui->screen.h};
    sc_dirty_buckets[3] = (area_t){hw, hh, g_gui->screen.w, g_gui->screen.h};
    sc_dirty_bucket_cnt = 4;
}
/* Plot one pixel in screen coordinates; no-op if outside the current PFB slice. */
void sc_draw_point(sc_pfb_t *pfb, int x, int y, color_t color)
{
    if (area_contains_point(pfb->area, x, y))
    {
        pfb->buffer[(y - pfb->area.ys) * pfb->width + (x - pfb->area.xs)] = color;
    }
}
/* Fill a rectangle, opaque (opa=255) or alpha-blended. Opaque path skips blend cost;
 * opa < 4 is rejected as visually transparent. */
bool sc_draw_fill_rect_opa(sc_pfb_t *pfb, int x, int y, int w, int h,
                           color_t color, uint8_t opa)
{
    if (opa < 4U)
    {
        return false;
    }
    area_t rect = rect_to_area((rect_t){x, y, w, h});
    if (!sc_pfb_intersection(pfb, &rect, NULL))
    {
        return false;
    }
    int fill_w = rect.xe - rect.xs;
    color_t *row = sc_pfb_pixel_ptr(pfb, rect.xs, rect.ys);
    if (opa > 251U)
    {
        /* Opaque path: direct write, no blend. */
        for (int yy = rect.ys; yy < rect.ye; ++yy)
        {
            for (int xx = 0; xx < fill_w; ++xx)
            {
                row[xx] = color;
            }
            row += pfb->width;
        }
    }
    else
    {
        /* Alpha blend path. */
        uint32_t fg_pack = sc_rgb565_pack_alpha(color);
        for (int yy = rect.ys; yy < rect.ye; ++yy)
        {
            for (int xx = 0; xx < fill_w; ++xx)
            {
                row[xx] = sc_blend_rgb565_fast(row[xx], color, fg_pack, opa);
            }
            row += pfb->width;
        }
    }
    return true;
}

/* Draw a rectangle outline of the given border thickness (fills solid if the border is thicker than the rect). */
void sc_draw_rect(sc_pfb_t *pfb, int x, int y, int w, int h,
                  color_t color, int border)
{
    if (w <= 0 || h <= 0 || border <= 0)
    {
        return;
    }
    int inset = border << 1;
    if (inset >= w || inset >= h)
    {
        sc_draw_fill_rect(pfb, x, y, w, h, color);
        return;
    }
    int inner_h = h - inset;
    sc_draw_fill_rect(pfb, x, y, w, border, color);
    sc_draw_fill_rect(pfb, x, y + h - border, w, border, color);
    sc_draw_fill_rect(pfb, x, y + border, border, inner_h, color);
    sc_draw_fill_rect(pfb, x + w - border, y + border, border, inner_h, color);
}

/* Blit a bitmap alpha mask (glyph/icon) into the pfb, coloring pixels from a solid color or a texture, with clipping and opacity. */
static void sc_draw_icon_texture(sc_pfb_t *pfb, int x, int y, int w, int h,
                                 const uint8_t *bitmap, uint8_t bpp,
                                 color_t color, sc_texture_t *texture,
                                 const area_t *clip, uint8_t opa)
{
    int row;
    if (!bitmap)
    {
        return;
    }
    area_t inter = rect_to_area((rect_t){x, y, (uint16_t)w, (uint16_t)h});
    if (!sc_pfb_intersection(pfb, &inter, clip))
    {
        return;
    }
    int row_start = inter.ys - y;
    int row_end = inter.ye - y;
    int col_start = inter.xs - x;
    int col_end = inter.xe - x;
    color_t *dst = sc_pfb_pixel_ptr(pfb, inter.xs, inter.ys);
    uint32_t fg_pack = texture ? 0U : sc_rgb565_pack_alpha(color);
    dst -= col_start;
    for (row = row_start; row < row_end; ++row)
    {
        uint32_t pixel_offset = (uint32_t)row * w + (uint32_t)col_start;
        for (int col = col_start; col < col_end; ++col)
        {
            uint16_t alpha = sc_bitmap_bpp_value(pixel_offset++, bitmap, bpp);
            if (alpha == 0U)
            {
                continue;
            }
            if (opa < 255U)
            {
                alpha = (uint16_t)((alpha * opa) >> 8);
            }
            if (texture)
            {
                color_t pc = texture->get_color(texture, (uint16_t)col, (uint16_t)row);
                dst[col] = alphaBlend(pc, dst[col], (uint8_t)alpha);
            }
            else
            {
                dst[col] = sc_blend_rgb565_fast(dst[col], color, fg_pack, (uint8_t)alpha);
            }
        }
        dst += pfb->width;
    }
}

/* Public wrapper to draw a bitmap icon/mask via sc_draw_icon_texture. */
void sc_draw_icon(sc_pfb_t *pfb, int x, int y, int w, int h,
                  const uint8_t *bitmap, uint8_t bpp, color_t color,
                  sc_texture_t *texture, const area_t *clip, uint8_t opa)
{
    sc_draw_icon_texture(pfb, x, y, w, h, bitmap, bpp, color, texture,
                         clip, opa);
}

/* Render the UTF-8 text in [start, end) at (x, y) glyph by glyph, applying color/texture, letter spacing, clip and opacity. */
void sc_draw_str_range_opa(sc_pfb_t *pfb, int x, int y,
                           const char *start, const char *end,
                           color_t color, sc_texture_t *texture,
                           const lv_font_t *font, uint8_t x_gap,
                           const area_t *clip, uint8_t opa)
{
    const char *cursor = start;
    uint32_t codepoint = 0;
    int texture_origin_x = x;
    int texture_origin_y = y;
    int16_t texture_offset_x = 0;
    int16_t texture_offset_y = 0;
    if (texture)
    {
        texture_offset_x = texture->offset_x;
        texture_offset_y = texture->offset_y;
    }
    if (!font)
    {
        font = sc_get_default_font();
        if (!font)
        {
            return;
        }
    }
    while (sc_utf8_next_range(&cursor, end, &codepoint))
    {
        lv_font_glyph_dsc_t dsc;
        if (font->get_glyph_dsc(font, &dsc, codepoint, 0U) == 0)
        {
            x += 6 + x_gap;
            continue;
        }
        const uint8_t *bitmap = font->get_glyph_bitmap(font, codepoint);
        if (!bitmap)
        {
            x += dsc.adv_w + x_gap;
            continue;
        }
        int glyph_x = x + dsc.ofs_x;
        int glyph_y = y + font->line_height - font->base_line -
                      dsc.box_h - dsc.ofs_y;
        if (texture)
        {
            /* get_color 为 src = pos - offset（正偏移=右/下移），这里取相反数，
             * 保持纹理锚定在字符串起点：整串读纹理同一段，不随字形滚动。 */
            texture->offset_x = (int16_t)(texture_origin_x - glyph_x -
                                          texture_offset_x);
            texture->offset_y = (int16_t)(texture_origin_y - glyph_y -
                                          texture_offset_y);
        }
        sc_draw_icon_texture(pfb,
                             glyph_x,
                             glyph_y,
                             dsc.box_w,
                             dsc.box_h,
                             bitmap,
                             dsc.bpp,
                             color,
                             texture,
                             clip,
                             opa);
        x += dsc.adv_w + x_gap;
    }
    if (texture)
    {
        texture->offset_x = (int16_t)(-texture_offset_x);
        texture->offset_y = (int16_t)(-texture_offset_y);
    }
}

/* 单行文本对齐绘制：text_w=0 时内部测量，否则用传入值；text_h 取 font->line_height。 */
void sc_draw_text_aligned(sc_pfb_t *pfb, rect_t view, const char *text,
                          uint8_t font_id, uint8_t x_gap,
                          color_t fg, sc_texture_t *texture,
                          sc_align_t align, int16_t x_ofs, uint8_t opa,
                          uint16_t text_w)
{
    const lv_font_t *font = sc_get_font(font_id);
    if (!text || !font)
    {
        return;
    }
    if (text_w == 0U)
    {
        uint16_t text_h;
        sc_text_measure_lines(text, font, x_gap, &text_w, &text_h, NULL);
    }
    uint16_t text_h = font->line_height;
    int16_t base_x = (int16_t)(view.x + sc_align_offset_x(view.w, text_w, align) + x_ofs);
    int16_t base_y = (int16_t)(view.y + sc_align_offset_y(view.h, text_h, align));
    area_t inter = rect_to_area(view);
    if (!sc_pfb_intersection(pfb, &inter, NULL))
    {
        return;
    }
    sc_draw_str_range_opa(pfb, base_x, base_y, text, NULL, fg, texture,
                          font, x_gap, &inter, opa);
}
