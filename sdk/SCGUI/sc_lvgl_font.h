#ifndef SC_LVGL_FONT_H
#define SC_LVGL_FONT_H

#include "sc_common.h"
#include "sc_event_task.h"
// 字体信息类型定义
typedef struct font_dsc_t
{
    uint8_t  Xspace;      // 字符间距
    uint8_t  line_height; // 字体高度
    uint16_t line_width; // 字符串总宽
    uint16_t line_cnt;  // 字符串总字符数
} font_dsc_t;

// label类型定义
typedef struct sc_label_t
{
    char *text;         // 文字
    lv_font_t *font;    // 字体
    sc_rect_t *box;     // 矩形
    int32_t xoffs : 12; // x轴偏移
    int32_t yoffs : 12; // y轴偏移
    int32_t align : 8;  // 对齐方式
} sc_label_t;

// 获取字体像素值（解析LVGL）
static inline uint8_t get_bpp_value(uint32_t scr_offs, const uint8_t *src, uint8_t bpp)
{
    const uint8_t byte = src[scr_offs >> (3 - (bpp >> 1))]; // 1→3  2→2  4→1  8→0
    const uint8_t pix_mask = (8 / bpp) - 1;                 // 1→7  2→3  4→1  8→0
    const uint8_t bit_offset = (pix_mask - (scr_offs & pix_mask)) * bpp;
    const uint8_t raw = (byte >> bit_offset) & ((1U << bpp) - 1U);
    static const uint8_t mul[] = {0, 255, 85, 0, 17, 0, 0, 0, 1};
    return raw * mul[bpp];
}

/* 设置字体间隙距 */
static inline void sc_set_font_space(lv_font_t *font, uint8_t xspace, uint8_t yspace)
{
    lv_font_fmt_txt_dsc_t *dsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
    dsc->kern_scale = (xspace << 8) | yspace;
}
/* 取字体间隙距 */
static inline font_dsc_t sc_get_font_dsc(lv_font_t *font)
{
    lv_font_fmt_txt_dsc_t *dsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
    return (font_dsc_t){
        .Xspace = dsc->kern_scale >> 8,
        .line_height = font->line_height + (dsc->kern_scale & 0xff),
        .line_width = 0,
        .line_cnt = 0,
    };
}

font_dsc_t sc_get_line_width(uint16_t line_width_max, sc_label_t *label, uint32_t *indx, unicode_t *unicode, lv_font_glyph_dsc_t *dsc);

void sc_draw_lv_letter(sc_pfb_t *dest, int xs, int ys, lv_font_glyph_dsc_t *dsc, uint32_t unicode, lv_font_t *font, color_t tc, color_t bc, sc_align_t alpha);

void sc_init_Label(sc_rect_t *parent, int xoffs, int yoffs, sc_label_t *label, lv_font_t *font, char *text, sc_align_t align);

void sc_draw_Label(sc_pfb_t *dest, sc_label_t *label, color_t tc, color_t bc, uint8_t alpha);

void sc_draw_Text(sc_pfb_t *dest, int tx, int ty, lv_font_t *font, const char *text, color_t tc, color_t bc);

void sc_draw_Num(sc_pfb_t *dest, int tx, int ty, int w, int h, lv_font_t *font, int num, int den, color_t tc, color_t bc);

#endif
