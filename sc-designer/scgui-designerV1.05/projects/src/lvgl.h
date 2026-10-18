#ifndef LVGL_H
#define LVGL_H

/* Minimal LVGL fmt_txt font ABI used by scgui. */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* One switch controls the complete external V7 XBF + V8 SCF8 runtime.
 * 必须在 sc_config.h 中定义，不提供默认值避免与工程配置冲突。 */
#if !defined(SC_EXTERNAL_FONT_ENABLE)
#error "SC_EXTERNAL_FONT_ENABLE must be defined in sc_config.h"
#endif

#ifndef SC_XBF_EXTERNAL_FLASH_BASE
#define SC_XBF_EXTERNAL_FLASH_BASE 0U
#endif

#if SC_EXTERNAL_FONT_ENABLE
void sc_external_font_select(uint8_t font_id, uint32_t flash_base);
void sc_external_font_invalidate_cache(void);
/* Compatibility for the unchanged LVGL V7 external-XBF template. */
uint8_t *sc_xbf_external_getdata(int offset, int size);

#ifndef __g_font_buf
#define __g_font_buf sc_xbf_external_getdata(offset, size)
#endif

#endif

typedef int16_t lv_coord_t;

typedef struct {
    uint8_t adv_w; /* pixels */
    uint8_t box_w;
    uint8_t box_h;
    int8_t ofs_x;
    int8_t ofs_y;
    uint8_t bpp;
} lv_font_glyph_dsc_t;

struct _lv_font_struct;
typedef struct _lv_font_struct lv_font_t;


typedef const uint8_t *(*lv_font_get_bitmap_cb_t)(const lv_font_t *font,
                                                  uint32_t unicode_letter);
/* LVGL 7 passes the following code point for kerning.  LVGL 8 dropped that
 * argument, so scgui keeps the LVGL 7 superset ABI and passes 0 when no next
 * code point is available. */
typedef bool (*lv_font_get_glyph_dsc_cb_t)(const lv_font_t *font,
                                           lv_font_glyph_dsc_t *dsc_out,
                                           uint32_t unicode_letter,
                                           uint32_t unicode_letter_next);

struct _lv_font_struct {
    lv_font_get_glyph_dsc_cb_t get_glyph_dsc;
    lv_font_get_bitmap_cb_t get_glyph_bitmap;
    lv_coord_t line_height;
    lv_coord_t base_line;
    const void *dsc;
};

#if SC_EXTERNAL_FONT_ENABLE
typedef struct {
    uint32_t unicode_start;
    uint32_t unicode_end;
    uint32_t glyph_id_start;
} sc_ext_font_range_t;

typedef struct {
    const sc_ext_font_range_t *ranges;
    uint32_t file_size;
    uint32_t glyph_count;
    uint16_t range_count;
    uint8_t bpp;
} sc_ext_font_scf8_t;

const uint8_t *sc_ext_font_v8_get_bitmap(const lv_font_t *font,
                                          uint32_t unicode_letter);
bool sc_ext_font_v8_get_glyph_dsc(const lv_font_t *font,
                                   lv_font_glyph_dsc_t *dsc_out,
                                   uint32_t unicode_letter,
                                   uint32_t unicode_letter_next);
#endif

typedef struct  {
#if SC_EXTERNAL_FONT_ENABLE
    /* Full 32-bit offset for large internal fmt_txt C fonts. */
    uint32_t bitmap_index;
    uint8_t adv_w;
#else
    /* Compact internal fmt_txt C layout for bitmaps smaller than 16 MiB. */
    uint32_t bitmap_index : 24;
    uint32_t adv_w : 8;
#endif
    uint8_t box_w;
    uint8_t box_h;
    int8_t ofs_x;
    int8_t ofs_y;
} lv_font_fmt_txt_glyph_dsc_t;

typedef enum {
    LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY = 0,
    LV_FONT_FMT_TXT_CMAP_SPARSE_TINY = 1
} lv_font_fmt_txt_cmap_type_t;

typedef struct  {
    uint32_t range_start;
    const uint16_t *unicode_list;
    /* LVGL 7 font generators emit this optional sparse glyph-id map. */
    const void *glyph_id_ofs_list;
    uint16_t range_length;
    uint16_t glyph_id_start;
    uint16_t list_length;
    uint8_t type;
} lv_font_fmt_txt_cmap_t;

typedef struct {
    uint32_t last_letter;
    uint32_t last_glyph_id;
} lv_font_fmt_txt_glyph_cache_t;

typedef struct {
    const uint8_t *glyph_bitmap;
    const lv_font_fmt_txt_glyph_dsc_t *glyph_dsc;
    const lv_font_fmt_txt_cmap_t *cmaps;
    /* LVGL 8-style external cache used by scgui's built-in fonts. */
    lv_font_fmt_txt_glyph_cache_t *cache;
    /* LVGL 7 fmt_txt compatibility fields. Kerning is currently ignored by
     * scgui, while the last_* fields remain available to generated callbacks. */
    const void *kern_dsc;
    int16_t kern_scale;
    uint16_t cmap_num;
    uint8_t bpp;
    uint8_t kern_classes;
    uint32_t last_letter;
    uint32_t last_glyph_id;
} lv_font_fmt_txt_dsc_t;

const uint8_t *lv_font_get_bitmap_fmt_txt(const lv_font_t *font,
                                          uint32_t unicode_letter);
bool lv_font_get_glyph_dsc_fmt_txt(const lv_font_t *font,
                                   lv_font_glyph_dsc_t *dsc_out,
                                   uint32_t unicode_letter,
                                   uint32_t unicode_letter_next);

#define LV_FONT_DECLARE(font_name) extern const lv_font_t font_name
/* LVGL 7 font tools commonly emit a non-const lv_font_t object. */
#define LV_FONT_DECLARE_V7(font_name) extern lv_font_t font_name

#endif
