#ifndef SC_WIDGET_NUMBER_H
#define SC_WIDGET_NUMBER_H

#include "../sc_common.h"

#include "../sc_obj.h"

#define SC_NUMBER_BUF_MAX 16U      /* 格式化字符串缓冲区长度（含结束符） */
#define SC_NUMBER_DECIMALS_MAX 6U  /* 小数位上限：除数 10^6，定点表示安全 */
#define SC_NUMBER_INT_WIDTH_MAX 7U /* 整数最小位宽上限，保证 text 不溢出 */

/* 初始宏。value 为定点值（×10^decimals）。int_width 为整数最小位宽（补 0 对齐，
 * 0=自动不补；实际位数超过时按实际显示）。align 默认居中。 */
#define SC_NUMBER_NEW(value_, decimals_, int_width_, fg_, font_id_, opa_) \
    {.value = (value_), .decimals = (decimals_), .int_width = (int_width_), \
     .fg = (fg_), .font_id = (font_id_), .opa = (opa_), .x_gap = 0U, .align = ALIGN_CENTER}

/* 字段按对齐降序排列，消除对齐空位：decimals/int_width 位域共用 1 字节，
 * 32/64 位下 10B 均无空洞。text 不存 props（格式化结果每次绘制时写入局部缓冲区）。 */
typedef struct sc_number_t
{
    int32_t value;                  /* 数值（定点，小数位由 decimals 决定） */
    color_t fg;
    uint8_t decimals : 4;           /* 小数位 0..SC_NUMBER_DECIMALS_MAX */
    uint8_t int_width : 4;          /* 整数最小位宽（补 0 对齐，0=自动） */
    uint8_t font_id;
    uint8_t opa;
    uint8_t x_gap;
    sc_align_t align;               /* 文字在 view 内对齐（同 button） */
} sc_number_t;

/* 把 props->value 按 decimals 格式化为十进制字符串到 text（长度 ≥ SC_NUMBER_BUF_MAX）。 */
void sc_number_format(sc_number_t *props, char *text);
/* 直接绘制数字到 LCD 或 PFB（裸绘制入口，会先刷新格式）。 */
void sc_lcd_draw_number(sc_pfb_t *pfb, int16_t x, int16_t y, sc_number_t *props);

#if SC_USER_OBJ && SC_WIDGET_NUMBER_ENABLE
sc_obj_t *sc_number_create(sc_obj_t *parent, sc_number_t *props);
void sc_number_set_value(sc_obj_t *obj, int32_t value);
bool sc_number_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_number_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
