#ifndef SC_ROUND_RECT_H
#define SC_ROUND_RECT_H

#include "../sc_common.h"
#include "../sc_obj.h"
#include "../sc_style.h"

enum
{
    SC_MASK_0 = 0x01,
    SC_MASK_1 = 0x02,
    SC_MASK_2 = 0x04,
    SC_MASK_3 = 0x08,
};

typedef struct sc_rounded_t
{
    /* 公共字段：与 sc_plain_rect_t 同序同名，样式处理可共用一段代码。 */
    color_t color;        /* 边框色 */
    color_t fill;         /* 填充色 */
    uint8_t radius;
    uint8_t border;       /* 0 = 实心 */
    uint8_t alpha;
    /* 圆角矩形独有 */
    uint8_t rect_mask;    /* 四角直角掩码 */
    sc_texture_t *texture; /* NULL = 纯色路径(mask) */
    sc_obj_cb_t cb;
 
} sc_rounded_t;

#define SC_ROUNDED_NEW(color_, fill_, radius_, border_)  \
    {.color = (color_), .fill = (fill_), \
     .radius = (radius_), .border = (border_), .alpha = 255U, \
     .texture = NULL, .cb = NULL, .rect_mask = 0U}

/* 普通矩形：无纹理/无渐变/无回调的最小结构（常用控件省内存），共享 round_rect 的绘制代码。 */
typedef struct sc_plain_rect_t
{
    color_t color;   /* 边框色 */
    color_t fill;    /* 填充色 */
    uint8_t radius;
    uint8_t border;  /* 0 = 实心 */
    uint8_t alpha;
    uint8_t rect_mask;    /* 四角直角掩码 */
} sc_plain_rect_t;

#define SC_PLAIN_RECT_NEW(color_, fill_, radius_, border_) \
    {.color = (color_), .fill = (fill_), \
     .radius = (uint8_t)(radius_), .border = (uint8_t)(border_), .alpha = 255U, .rect_mask = 0U}


/* mask 版本已合并到 texture 版本，传 NULL texture 等价；减少重复代码 */
#define sc_draw_Rounded_mask(dest, view, r, border, fc, fill, alpha, rect_mask, clip) \
    sc_draw_Rounded_texture(dest, view, r, border, fc, fill, alpha, NULL, rect_mask, clip)

/* 圆角滚动条：圆角轨道 + 滑块（无边框）。vol 为滚动偏移（负值=内容下移），
 * 滑块长度按剩余可滚距离自适应。列表（sc_list_items）与文本框（sc_text_box）共用。 */
void sc_draw_bar(sc_pfb_t *dest, rect_t box, int r, color_t fc, color_t bc,
                 uint8_t alpha, int vol);


bool sc_draw_Rounded_texture(sc_pfb_t *dest, rect_t view, int r, int border,
                             color_t fc_color,color_t fill, uint8_t alpha,
                             sc_texture_t *texture, uint8_t rect_mask,
                             const area_t *clip);

void sc_lcd_draw_Rounded_texture(sc_pfb_t *pfb, int16_t x, int16_t y,
                                 uint16_t w, uint16_t h, int radius, int border,
                                 color_t fc_color,color_t fill, uint8_t alpha,
                                 sc_texture_t *texture, uint8_t rect_mask);

void sc_lcd_draw_Rounded_mask(sc_pfb_t *pfb, int16_t x, int16_t y,
                              uint16_t w, uint16_t h, int radius, int border,
                              color_t fc_color, color_t fill, uint8_t alpha,
                              uint8_t rect_mask);


#if SC_USER_OBJ && SC_WIDGET_ROUND_RECT_ENABLE

sc_obj_t *sc_round_rect_create(sc_obj_t *parent, sc_rounded_t *props);
/* 普通矩形：共享样式/事件代码，仅类型与结构不同（无 texture，默认 radius 0 = 直角）。 */
sc_obj_t *sc_plain_rect_create(sc_obj_t *parent, sc_plain_rect_t *props);
bool sc_round_rect_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_round_rect_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
