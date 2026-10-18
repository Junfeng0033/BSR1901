#ifndef SC_LVGL_FONT_H
#define SC_LVGL_FONT_H

/*
 * LVGL font helpers.
 * The minimal ABI accepts LVGL 7's four-argument glyph descriptor callback;
 * built-in LVGL 8 fmt_txt fonts are exposed through the same compatible ABI.
 * This header also provides UTF-8 helpers and text measurement.
 */

#include "sc_common.h"
#include "lvgl.h"

#ifndef SC_EXTERNAL_FONT_GLYPH_BUFFER_SIZE
#define SC_EXTERNAL_FONT_GLYPH_BUFFER_SIZE 256U
#endif

#if SC_EXTERNAL_FONT_ENABLE
typedef union
{
    uint32_t align;
    uint8_t bytes[SC_EXTERNAL_FONT_GLYPH_BUFFER_SIZE];
} sc_external_bitmap_cache_t;

typedef struct
{
    uint32_t unicode;
    uint32_t bitmap_offset;
    uint16_t bitmap_size;
    lv_font_glyph_dsc_t glyph_dsc;
    sc_external_bitmap_cache_t bitmap;
    uint32_t flash_base;
    uint8_t valid;
    uint8_t font_id;
} sc_external_font_cache_t;

enum
{
    SC_EXT_CACHE_DSC = 1U << 0,
    SC_EXT_CACHE_BITMAP = 1U << 1
};
#endif

static inline int sc_font_line_height(const lv_font_t *font)
{
    return font && font->line_height > 0 ? font->line_height : 8;
}

bool sc_utf8_next(const char **text, uint32_t *codepoint);
bool sc_utf8_next_range(const char **cursor, const char *end, uint32_t *codepoint);

/* Return one glyph advance, or the six-pixel fallback when no font is supplied. */
static inline int sc_font_glyph_advance(const lv_font_t *font, uint32_t codepoint)
{
    lv_font_glyph_dsc_t dsc;
    return font && font->get_glyph_dsc(font, &dsc, codepoint, 0U)
               ? dsc.adv_w
               : 6;
}
int sc_lvgl_measure_text_range(const lv_font_t *font, const char *start, const char *end, uint8_t x_gap);
const char *sc_consume_line_break(const char *cursor);
/* Character-level wrapping. max_width <= 0 only splits explicit line breaks. */
const char *sc_text_next_line(const char *text,
                              const lv_font_t *font,
                              uint8_t x_gap,
                              int max_width,
                              const char **line_end,
                              uint16_t *line_width);
bool sc_text_measure_layout(const char *text,
                            const lv_font_t *font,
                            uint8_t x_gap,
                            int max_width,
                            uint16_t *out_w,
                            uint16_t *out_h,
                            uint16_t *out_lines);
bool sc_text_measure_lines(const char *text, const lv_font_t *font, uint8_t x_gap, uint16_t *out_w, uint16_t *out_h, uint16_t *out_lines);
/* Compatibility wrapper: explicit line breaks only. */
const char *sc_font_next_line(const char *text, const char **line_end);

/* Per-pixel packed alpha decode helper, shared by glyphs and icons. */
static inline uint16_t sc_bitmap_bpp_value(uint32_t offset, const uint8_t *src, const uint8_t bpp)
{
    uint16_t alpha = 0;
    switch (bpp)
    {
    case 8:
        return src[offset];
    case 4:
        alpha = (src[offset >> 1] >> ((1 - (offset & 1)) * bpp)) & 0x0F;
        return (uint16_t)((alpha << 4) + alpha);
    case 2:
        alpha = (src[offset >> 2] >> ((3 - (offset & 3)) * bpp)) & 0x03;
        return (uint16_t)(alpha * 85U);
    case 1:
        alpha = ((src[offset >> 3] >> (7 - (offset & 7))) & 0x01U) ? 255U : 0U;
        return alpha;
    default:
        return 0;
    }
}

#endif
