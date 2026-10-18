#ifndef SC_EYE_H
#define SC_EYE_H

#include "../sc_common.h"
#include "../sc_obj.h"
#include "../sc_style.h"

typedef struct sc_eye_t
{
    color_t color;          /* 描边色（SC_STYLE_FG） */
    color_t fill;           /* 无纹理时的纯色填充（SC_STYLE_BG；==g_gui->bg 则不填充内部） */
    int16_t cx;             /* 眼中心 x，相对 obj view 原点 */
    int16_t cy;             /* 眼中心 y */
    int16_t w;              /* 最大宽（水平跨度，最宽处 = 眼中心行） */
    int16_t upper_h;        /* 上抛物线深度（顶点到眼中心） */
    int16_t lower_h;        /* 下抛物线深度（眼中心到顶点） */
    uint8_t stroke_width;   /* 描边宽（SC_STYLE_BORDER_WIDTH） */
    uint8_t blink;          /* 睁眼程度 0..255（0 = 全闭 1px 缝；255 = 最大开眼；绘制按比例缩放上下深度） */
    sc_texture_t *texture;  /* 填充纹理（SC_STYLE_TEXTURE；NULL = 纯色 fill） */
} sc_eye_t;

#define SC_EYE_NEW(color_, fill_, w_, uh_, lh_) \
    {.color = (color_), .fill = (fill_), .cx = 0, .cy = 0, .w = (w_), .upper_h = (uh_), \
     .lower_h = (lh_), .stroke_width = 1U, .blink = 255U, .texture = NULL}

/* 抛物线绘制（边框+填充，不透明）：走廊内描边 color（AA），走廊内侧画 fill
 * （texture 非 NULL 取纹理色，坐标相对 tex_origin_x/y——上下两条共用同一原点，
 * 整眼一张纹理；fill==g_gui->bg 且无纹理则只描边）。按 dest pfb 区域裁剪。 */
bool sc_draw_parabola_ex(sc_pfb_t *dest, int cx, int cy, int w, int h, int dir, int stroke_width,
                         color_t color, color_t fill, sc_texture_t *texture,
                         int tex_origin_x, int tex_origin_y);
/* 裸绘封装：pfb NULL 时走 SC_LCD_DRAW 切片刷新（SDL/MCU 测试入口）。 */
void sc_lcd_draw_parabola_ex(sc_pfb_t *pfb, int cx, int cy, int w, int h, int dir,
                             int stroke_width, color_t color, color_t fill,
                             sc_texture_t *texture, int tex_origin_x, int tex_origin_y);

/* 绘制杏仁眼：每行取上/下抛物线半宽较小者填充其间（mask），再描边上下两条抛物线。 */
void sc_draw_eye(sc_pfb_t *dest, rect_t view, const sc_eye_t *props);

#if SC_USER_OBJ && SC_WIDGET_EYE_ENABLE
sc_obj_t *sc_eye_create(sc_obj_t *parent, sc_eye_t *props);
void sc_eye_set_shape(sc_obj_t *obj, int16_t cx, int16_t cy, int16_t w, int16_t upper_h, int16_t lower_h);
/* 设置闭眼程度 0..255（255=全闭），动画由外部定时器驱动。 */
void sc_eye_set_blink(sc_obj_t *obj, uint8_t blink);
bool sc_eye_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_eye_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
