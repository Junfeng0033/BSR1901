#ifndef DRAW_H
#define DRAW_H

#include "sc_gui.h"

bool sc_invalidate_area(area_t area);
void sc_invalidate_all(void);
area_t *sc_dirty_pop(uint8_t *valid_cnt);
#define SC_LCD_DRAW(dirty_, draw_code_)                              \
    do                                                               \
    {                                                                \
        sc_pfb_t sc_lcd_draw_local_pfb__;                            \
        sc_pfb_t *sc_lcd_draw_pfb__ = &sc_lcd_draw_local_pfb__;      \
        if (pfb_init_slices(sc_lcd_draw_pfb__, (dirty_), g_gui->bg)) \
        {                                                            \
            do                                                       \
            {                                                        \
                draw_code_;                                          \
            } while (pfb_next_slice(sc_lcd_draw_pfb__));             \
        }                                                            \
    } while (0)

bool sc_draw_fill_rect_opa(sc_pfb_t *pfb, int x, int y, int w, int h,
                           color_t color, uint8_t opa);
/* Opaque fill = sc_draw_fill_rect_opa with opa=255; kept as inline wrapper for
 * existing call sites that don't need alpha. */
static inline bool sc_draw_fill_rect(sc_pfb_t *pfb, int x, int y, int w, int h,
                                     color_t color)
{
    return sc_draw_fill_rect_opa(pfb, x, y, w, h, color, 255U);
}
void sc_draw_rect(sc_pfb_t *pfb, int x, int y, int w, int h,
                  color_t color, int border);
void sc_draw_point(sc_pfb_t *pfb, int x, int y, color_t color);
/* Packed row-major alpha bitmap; bpp supports 1, 2, 4, or 8 bits per pixel. */
void sc_draw_icon(sc_pfb_t *pfb, int x, int y, int w, int h,
                  const uint8_t *bitmap, uint8_t bpp, color_t color,
                  sc_texture_t *texture, const area_t *clip, uint8_t opa);
void sc_draw_str_range_opa(sc_pfb_t *pfb, int x, int y,
                           const char *start, const char *end,
                           color_t color, sc_texture_t *texture,
                           const lv_font_t *font, uint8_t x_gap,
                           const area_t *clip, uint8_t opa);

/* 单行文本对齐绘制：text_w=0 时内部测量，否则用传入值；text_h 取 font->line_height。
 * font_id 内部转 font 指针，支持 x_ofs 偏移和纹理，超出 view 部分裁剪。 */
void sc_draw_text_aligned(sc_pfb_t *pfb, rect_t view, const char *text,
                          uint8_t font_id, uint8_t x_gap,
                          color_t fg, sc_texture_t *texture,
                          sc_align_t align, int16_t x_ofs, uint8_t opa,
                          uint16_t text_w);

#endif
