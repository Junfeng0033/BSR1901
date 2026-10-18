
#include "sc_gui.h"

#if SC_EXTERNAL_FONT_ENABLE
#if SC_EXTERNAL_FONT_GLYPH_BUFFER_SIZE < 1U || SC_EXTERNAL_FONT_GLYPH_BUFFER_SIZE > UINT16_MAX
#error "SC_EXTERNAL_FONT_GLYPH_BUFFER_SIZE must be 1..65535"
#endif

sc_external_font_cache_t g_sc_external_font = {
    .flash_base = SC_XBF_EXTERNAL_FLASH_BASE,
};

void sc_external_font_invalidate_cache(void)
{
    g_sc_external_font.valid = 0U;
}

void sc_external_font_select(uint8_t font_id, uint32_t flash_base)
{
    if (g_sc_external_font.font_id != font_id ||
        g_sc_external_font.flash_base != flash_base)
    {
        g_sc_external_font.font_id = font_id;
        g_sc_external_font.flash_base = flash_base;
        g_sc_external_font.valid = 0U;
    }
}

static bool sc_external_font_read(uint8_t *dst, uint32_t offset, uint32_t size)
{
    if (offset > UINT32_MAX - g_sc_external_font.flash_base)
    {
        return false;
    }
    uint32_t address = g_sc_external_font.flash_base + offset;
    if (size > UINT32_MAX - address)
    {
        return false;
    }
    spiflash_read(dst, address, size);
    return true;
}

/* The official V7 callbacks perform five reads per new/repeated glyph. */
uint8_t *sc_xbf_external_getdata(int offset, int size)
{
    if (offset < 0 || size < 0 ||
        (uint32_t)size > SC_EXTERNAL_FONT_GLYPH_BUFFER_SIZE)
    {
        return NULL;
    }
    /* V7 uses no glyph cache; its transient read invalidates V8 state. */
    g_sc_external_font.valid = 0U;
    if (size != 0 &&
        !sc_external_font_read(g_sc_external_font.bitmap.bytes,
                               (uint32_t)offset, (uint32_t)size))
    {
        return NULL;
    }
    return g_sc_external_font.bitmap.bytes;
}

static uint32_t sc_ext_font_u32(const uint8_t *data)
{
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

static uint32_t sc_ext_font_find_glyph(const sc_ext_font_scf8_t *font_dsc,
                                       uint32_t unicode)
{
    uint16_t low = 0U;
    uint16_t high = font_dsc->range_count;
    while (low < high)
    {
        uint16_t mid = (uint16_t)(low + ((high - low) >> 1));
        const sc_ext_font_range_t *range = &font_dsc->ranges[mid];
        if (unicode < range->unicode_start)
        {
            high = mid;
        }
        else if (unicode > range->unicode_end)
        {
            low = (uint16_t)(mid + 1U);
        }
        else
        {
            return range->glyph_id_start + unicode - range->unicode_start;
        }
    }
    return UINT32_MAX;
}

static bool sc_ext_font_v8_load_glyph(const lv_font_t *font, uint32_t unicode)
{
    if (!font || !font->dsc)
    {
        return false;
    }
    sc_external_font_cache_t *cache = &g_sc_external_font;
    const sc_ext_font_scf8_t *font_dsc =
        (const sc_ext_font_scf8_t *)font->dsc;
    uint32_t glyph_id = sc_ext_font_find_glyph(font_dsc, unicode);
    if (glyph_id == UINT32_MAX || glyph_id >= font_dsc->glyph_count)
    {
        return false;
    }
    if ((g_sc_external_font.valid & SC_EXT_CACHE_DSC) != 0U &&
        cache->unicode == unicode)
    {
        return true;
    }
    g_sc_external_font.valid = 0U;
    uint8_t raw[9];
    if (!sc_external_font_read(raw, glyph_id * 9U, sizeof(raw)))
    {
        return false;
    }
    uint32_t bitmap_offset = sc_ext_font_u32(raw);
    uint32_t bitmap_size = ((uint32_t)raw[5] * raw[6] * font_dsc->bpp + 7U) >> 3;
    uint32_t bitmap_data_offset = font_dsc->glyph_count * 9U;
    if (bitmap_size > SC_EXTERNAL_FONT_GLYPH_BUFFER_SIZE ||
        bitmap_offset < bitmap_data_offset || bitmap_offset > font_dsc->file_size ||
        bitmap_size > font_dsc->file_size - bitmap_offset)
    {
        return false;
    }
    cache->unicode = unicode;
    cache->bitmap_offset = bitmap_offset;
    cache->bitmap_size = (uint16_t)bitmap_size;
    cache->glyph_dsc.adv_w = raw[4];
    cache->glyph_dsc.box_w = raw[5];
    cache->glyph_dsc.box_h = raw[6];
    cache->glyph_dsc.ofs_x = (int8_t)raw[7];
    cache->glyph_dsc.ofs_y = (int8_t)raw[8];
    cache->glyph_dsc.bpp = font_dsc->bpp;
    g_sc_external_font.valid |= SC_EXT_CACHE_DSC;
    return true;
}

const uint8_t *sc_ext_font_v8_get_bitmap(const lv_font_t *font,
                                          uint32_t unicode_letter)
{
    if (!sc_ext_font_v8_load_glyph(font, unicode_letter))
    {
        return NULL;
    }
    sc_external_font_cache_t *cache = &g_sc_external_font;
    if ((g_sc_external_font.valid & SC_EXT_CACHE_BITMAP) == 0U)
    {
        if (cache->bitmap_size != 0U &&
            !sc_external_font_read(g_sc_external_font.bitmap.bytes,
                                   cache->bitmap_offset, cache->bitmap_size))
        {
            return NULL;
        }
        g_sc_external_font.valid |= SC_EXT_CACHE_BITMAP;
    }
    return g_sc_external_font.bitmap.bytes;
}
bool sc_ext_font_v8_get_glyph_dsc(const lv_font_t *font,
                                   lv_font_glyph_dsc_t *dsc_out,
                                   uint32_t unicode_letter,
                                   uint32_t unicode_letter_next)
{
    (void)unicode_letter_next;
    if (!dsc_out || !sc_ext_font_v8_load_glyph(font, unicode_letter))
    {
        return false;
    }
    *dsc_out = g_sc_external_font.glyph_dsc;
    return true;
}
#endif


/* Binary search a sorted relative-unicode list; returns index or -1. */
static int32_t sc_unicode_list_bsearch(const uint16_t *sorted, uint16_t len, uint16_t key)
{
    int32_t low = 0;
    int32_t high = (int32_t)len - 1;
    while (low <= high)
    {
        int32_t mid = (low + high) >> 1;
        uint16_t v = sorted[mid];
        if (key < v)
        {
            high = mid - 1;
        }
        else if (key > v)
        {
            low = mid + 1;
        }
        else
        {
            return mid;
        }
    }
    return -1;
}

/*
 * Look up glyph id for a Unicode code point.
 * FORMAT0_TINY is O(1); SPARSE_TINY uses binary search on relative offsets.
 * font_dsc->cache makes a repeated code point O(1). Returns 0 if not found
 * (glyph id 0 is reserved).
 */
static uint32_t lv_fmt_txt_lookup_glyph_id(const lv_font_fmt_txt_dsc_t *font_dsc, uint32_t unicode_letter)
{
    uint16_t cmap_index;
    uint32_t glyph_id = 0U;
    lv_font_fmt_txt_glyph_cache_t *cache;
    if (!font_dsc || !font_dsc->cmaps)
    {
        return 0U;
    }
    cache = font_dsc->cache;
    if (cache && cache->last_letter == unicode_letter)
    {
        return cache->last_glyph_id;
    }
    for (cmap_index = 0; cmap_index < font_dsc->cmap_num; ++cmap_index)
    {
        const lv_font_fmt_txt_cmap_t *cmap = &font_dsc->cmaps[cmap_index];

        if (cmap->type == LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY)
        {
            if (unicode_letter >= cmap->range_start &&
                unicode_letter < (uint32_t)cmap->range_start + cmap->range_length)
            {
                glyph_id = (uint32_t)cmap->glyph_id_start + (unicode_letter - cmap->range_start);
                break;
            }
        }
        else if (cmap->type == LV_FONT_FMT_TXT_CMAP_SPARSE_TINY)
        {
            int32_t i;
            uint16_t key;
            if (unicode_letter < cmap->range_start ||
                unicode_letter >= (uint32_t)cmap->range_start + cmap->range_length ||
                !cmap->unicode_list || cmap->list_length == 0U)
            {
                continue;
            }
            key = (uint16_t)(unicode_letter - cmap->range_start);
            i = sc_unicode_list_bsearch(cmap->unicode_list, cmap->list_length, key);
            if (i >= 0)
            {
                glyph_id = (uint32_t)cmap->glyph_id_start + (uint32_t)i;
                break;
            }
        }
    }

    if (cache)
    {
        cache->last_letter = unicode_letter;
        cache->last_glyph_id = glyph_id;
    }
    return glyph_id;
}

/* Return a pointer to the glyph bitmap data for a code point, or NULL if the glyph is missing. */
const uint8_t *lv_font_get_bitmap_fmt_txt(const lv_font_t *font, uint32_t unicode_letter)
{
    if (!font || !font->dsc)
    {
        return NULL;
    }
    const lv_font_fmt_txt_dsc_t *font_dsc = (const lv_font_fmt_txt_dsc_t *)font->dsc;
    uint32_t glyph_id = lv_fmt_txt_lookup_glyph_id(font_dsc, unicode_letter);
    if (glyph_id == 0 || !font_dsc->glyph_bitmap || !font_dsc->glyph_dsc)
    {
        return NULL;
    }
    return &font_dsc->glyph_bitmap[font_dsc->glyph_dsc[glyph_id].bitmap_index];
}

/* Fill dsc_out with a glyph's metrics (advance, box size, offsets, bpp) for a code point; returns false if not found. */
bool lv_font_get_glyph_dsc_fmt_txt(const lv_font_t *font,
                                   lv_font_glyph_dsc_t *dsc_out,
                                   uint32_t unicode_letter,
                                   uint32_t unicode_letter_next)
{
    const lv_font_fmt_txt_glyph_dsc_t *glyph_dsc;
    (void)unicode_letter_next;
    if (!font || !font->dsc || !dsc_out)
    {
        return false;
    }

    const lv_font_fmt_txt_dsc_t *font_dsc = (const lv_font_fmt_txt_dsc_t *)font->dsc;
    uint32_t glyph_id = lv_fmt_txt_lookup_glyph_id(font_dsc, unicode_letter);
    if (glyph_id == 0 || !font_dsc->glyph_dsc)
    {
        return false;
    }
    glyph_dsc = &font_dsc->glyph_dsc[glyph_id];
    dsc_out->adv_w = glyph_dsc->adv_w;
    dsc_out->box_w = glyph_dsc->box_w;
    dsc_out->box_h = glyph_dsc->box_h;
    dsc_out->ofs_x = glyph_dsc->ofs_x;
    dsc_out->ofs_y = glyph_dsc->ofs_y;
    dsc_out->bpp =  font_dsc->bpp ;
    return true;
}

/* Decode one UTF-8 code point from *text and advance the pointer; returns false at end of string. */
bool sc_utf8_next(const char **text, uint32_t *codepoint)
{
    const unsigned char *p = (const unsigned char *)*text;
    if (!p || *p == '\0')
    {
        return false;
    }
    if ((p[0] & 0x80U) == 0U)
    {
        *codepoint = p[0];
        *text += 1;
        return true;
    }
    if ((p[0] & 0xE0U) == 0xC0U && p[1] != '\0')
    {
        *codepoint = ((uint32_t)(p[0] & 0x1FU) << 6) |
                     (uint32_t)(p[1] & 0x3FU);
        *text += 2;
        return true;
    }
    if ((p[0] & 0xF0U) == 0xE0U && p[1] != '\0' && p[2] != '\0')
    {
        *codepoint = ((uint32_t)(p[0] & 0x0FU) << 12) |
                     ((uint32_t)(p[1] & 0x3FU) << 6) |
                     (uint32_t)(p[2] & 0x3FU);
        *text += 3;
        return true;
    }
    if ((p[0] & 0xF8U) == 0xF0U && p[1] != '\0' && p[2] != '\0' && p[3] != '\0')
    {
        *codepoint = ((uint32_t)(p[0] & 0x07U) << 18) |
                     ((uint32_t)(p[1] & 0x3FU) << 12) |
                     ((uint32_t)(p[2] & 0x3FU) << 6) |
                     (uint32_t)(p[3] & 0x3FU);
        *text += 4;
        return true;
    }

    *codepoint = '?';
    *text += 1;
    return true;
}

/* Decode the next UTF-8 code point but stop at the optional end bound, not overrunning a partial trailing sequence. */
bool sc_utf8_next_range(const char **cursor, const char *end, uint32_t *codepoint)
{
    if (!*cursor || !codepoint)
    {
        return false;
    }
    if (end && *cursor >= end)
    {
        return false;
    }
    const char *next = *cursor;
    if (!sc_utf8_next(&next, codepoint))
    {
        return false;
    }
    if (end && next > end)
    {
        return false;
    }
    *cursor = next;
    return true;
}

/* Measure the pixel width of the UTF-8 text in [start, end), including letter spacing between glyphs. */
int sc_lvgl_measure_text_range(const lv_font_t *font, const char *start, const char *end, uint8_t x_gap)
{
    int width = 0;
    const char *cursor = start;
    uint32_t codepoint = 0;
    bool has_glyph = false;
    while (sc_utf8_next_range(&cursor, end, &codepoint))
    {
        if (has_glyph)
        {
            width += x_gap;
        }
        width += sc_font_glyph_advance(font, codepoint);
        has_glyph = true;
    }
    return width;
}

/* Skip a single line break (\r, \n, or \r\n) at cursor and return the position after it. */
const char *sc_consume_line_break(const char *cursor)
{
    if (!cursor || *cursor == '\0')
    {
        return cursor;
    }
    if (*cursor == '\r')
    {
        cursor++;
        if (*cursor == '\n')
        {
            cursor++;
        }
        return cursor;
    }
    return *cursor == '\n' ? cursor + 1 : cursor;
}

/* Find one line of text with word/character wrapping at max_width; reports the line end and width and returns the start of the next line. */
const char *sc_text_next_line(const char *text,
                              const lv_font_t *font,
                              uint8_t x_gap,
                              int max_width,
                              const char **line_end,
                              uint16_t *line_width)
{
    int width = 0;
    bool has_glyph = false;
    if (!text)
    {
        if (line_end)
        {
            *line_end = NULL;
        }
        if (line_width)
        {
            *line_width = 0U;
        }
        return NULL;
    }
    const char *cursor = text;
    const char *last_fit = text;
    while (*cursor != '\0' && *cursor != '\r' && *cursor != '\n')
    {
        const char *glyph_start = cursor;
        uint32_t codepoint = 0U;
        if (!sc_utf8_next(&cursor, &codepoint))
        {
            break;
        }
        int next_width = width + (has_glyph ? x_gap : 0) + sc_font_glyph_advance(font, codepoint);
        if (max_width > 0 && has_glyph && next_width > max_width)
        {
            cursor = glyph_start;
            break;
        }
        width = next_width;
        last_fit = cursor;
        has_glyph = true;
        if (max_width > 0 && width >= max_width)
        {
            break;
        }
    }
    if (line_end)
    {
        *line_end = last_fit;
    }
    if (line_width)
    {
        *line_width = (uint16_t)width;
    }
    return (*last_fit == '\r' || *last_fit == '\n') ? sc_consume_line_break(last_fit) : last_fit;
}

/* Measure wrapped multi-line text, reporting total width, height and line count for the given max_width. */
bool sc_text_measure_layout(const char *text,
                            const lv_font_t *font,
                            uint8_t x_gap,
                            int max_width,
                            uint16_t *out_w,
                            uint16_t *out_h,
                            uint16_t *out_lines)
{
    uint16_t measured_w = 0U;
    uint16_t line_count = 0U;
    if (!text)
    {
        if (out_w)
        {
            *out_w = 0U;
        }
        if (out_h)
        {
            *out_h = 0U;
        }
        if (out_lines)
        {
            *out_lines = 0U;
        }
        return false;
    }
    if (!font)
    {
        font = sc_get_default_font();
    }
    int line_height = sc_font_line_height(font);
    const char *line_start = text;
    while (*line_start != '\0')
    {
        const char *prev_line_start = line_start;
        const char *line_end = NULL;
        uint16_t line_width = 0U;
        const char *next_line = sc_text_next_line(line_start,
                                                  font,
                                                  x_gap,
                                                  max_width,
                                                  &line_end,
                                                  &line_width);
        if (line_width > measured_w)
        {
            measured_w = line_width;
        }
        ++line_count;
        line_start = next_line;
        if (line_start <= prev_line_start)
        {
            break;
        }
    }
    if (out_w)
    {
        *out_w = measured_w;
    }
    if (out_h)
    {
        *out_h = (uint16_t)(line_count * line_height);
    }
    if (out_lines)
    {
        *out_lines = line_count;
    }
    return measured_w != 0U;
}

/* Measure text with no width limit (break only on explicit line breaks); returns width, height and line count. */
bool sc_text_measure_lines(const char *text,
                           const lv_font_t *font,
                           uint8_t x_gap,
                           uint16_t *out_w,
                           uint16_t *out_h,
                           uint16_t *out_lines)
{
    return sc_text_measure_layout(text, font, x_gap, 0, out_w, out_h, out_lines);
}

/* Return the next line (splitting only on line breaks) and report its end via line_end. */
const char *sc_font_next_line(const char *text, const char **line_end)
{
    return sc_text_next_line(text, NULL, 0U, 0, line_end, NULL);
}
