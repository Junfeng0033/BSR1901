#ifndef SC_LVGL_FONT_H
#define SC_LVGL_FONT_H

#include "sc_common.h"
#include "sc_event_task.h"
// 字体信息类型定义
typedef struct font_dsc_t
{
    uint8_t Xspace;      // 字符间距
    uint8_t line_height; // 字体高度
} font_dsc_t;

// label类型定义
typedef struct sc_label_t
{
    sc_rect_t *box;   // 矩形
    lv_font_t *font;  // 字体
    char *text;       // 文字
    color_t color;    // 前景颜色
    color_t bcolor;   // 背景颜色
    uint8_t alpha;    // 透明度
    sc_align_t align; // 对齐方式
} sc_label_t;

// 可编辑文本类型定义
typedef struct sc_edit_t
{
    sc_label_t label;         // 标签
    int32_t xoffs : 12;       // x轴偏移
    int32_t yoffs : 12;       // y轴偏移
    int32_t editbuf_size : 8; // 编辑框缓冲区大小
} sc_edit_t;

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
// copy字体像素值（解析LVGL）
static inline void copy_bpp_value(uint8_t *dst, uint32_t dst_offs,
                                  const uint8_t *src, uint32_t src_offs,
                                  uint8_t bpp)
{
    if (bpp >= 8)
    { // 快速路径：8bpp 直接复制
        ((uint8_t *)dst)[dst_offs] = src[src_offs];
        return;
    }
    const uint8_t pix_per_byte = 8 / bpp;
    const uint8_t pix_mask = pix_per_byte - 1;
    const uint8_t bpp_mask = (1U << bpp) - 1;
    const uint8_t bpp_shift = 3 - (bpp >> 1);
    // 读取源像素
    const uint32_t src_byte_idx = src_offs >> bpp_shift;
    const uint8_t src_pix_idx = src_offs & pix_mask;
    const uint8_t src_bit_offset = (pix_per_byte - 1 - src_pix_idx) * bpp;
    const uint8_t src_byte = src[src_byte_idx];
    const uint8_t raw_pix = (src_byte >> src_bit_offset) & bpp_mask;

    // 写入目标像素
    uint8_t *dst_writable = (uint8_t *)dst;
    const uint32_t dst_byte_idx = dst_offs >> bpp_shift;
    const uint8_t dst_pix_idx = dst_offs & pix_mask;
    const uint8_t dst_bit_offset = (pix_per_byte - 1 - dst_pix_idx) * bpp;
    dst_writable[dst_byte_idx] &= ~(bpp_mask << dst_bit_offset);
    dst_writable[dst_byte_idx] |= (raw_pix << dst_bit_offset);
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
    };
}

uint16_t _get_Label_width(uint16_t line_width_max, sc_label_t *label, uint32_t *indx, unicode_t *unicode, lv_font_glyph_dsc_t *dsc);

void sc_init_Label(sc_rect_t *parent, sc_label_t *label, lv_font_t *font,  char *text, color_t tc,color_t bc, sc_align_t align);

void sc_draw_Label(sc_pfb_t *dest, int xoffs, int yoffs, sc_label_t *label);

void sc_init_Edit(sc_rect_t *parent, sc_edit_t *edit, lv_font_t *font, char *text, color_t tc, sc_align_t align, uint8_t editbuf_size);

void sc_draw_Edit(sc_pfb_t *dest, sc_edit_t *edit);

bool sc_event_Edit(sc_event_t *event, sc_edit_t *edit);

void sc_draw_Text(sc_pfb_t *dest, int tx, int ty, lv_font_t *font, const char *text, color_t tc,color_t bc);

void sc_draw_Num(sc_pfb_t *dest, int tx, int ty, int w, int h, lv_font_t *font, int num, int den, color_t tc,color_t bc);


void sc_demo_Edit(void);
void sc_demo_Edit_event(sc_event_t *event);
#endif
