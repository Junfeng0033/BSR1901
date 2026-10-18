#include "../sc_draw.h"
#include "sc_number.h"

#if SC_WIDGET_NUMBER_ENABLE
/* 把 value 按 decimals 位小数格式化为定点字符串，写入 text 缓冲区（局部变量）。
 * 最省运算：位数用 10 的幂查表比较（无除法），逐位 %10//10 都是常量除法
 * （GCC 合并成乘移位 divmod），直接倒退写 text（无中间缓冲/拷贝）。 */
void sc_number_format(sc_number_t *props, char *text)
{
    static const uint32_t k_pow10[10U] =
        {10U, 100U, 1000U, 10000U, 100000U, 1000000U,
         10000000U, 100000000U, 1000000000U, 0U};
    uint8_t decimals = SC_MIN(props->decimals, SC_NUMBER_DECIMALS_MAX);
    uint32_t mag = props->value < 0 ? (0U - (uint32_t)props->value) : (uint32_t)props->value;
    uint8_t t = 1U;  /* mag 总位数（查表比较，无除法） */
    uint8_t ip;      /* 整数实际位数 */
    uint8_t iw;      /* 整数显示位宽 = max(实际位数, int_width)，≥1 */
    uint8_t i;
    char *p;
    while (t < 10U && mag >= k_pow10[t - 1U])
    {
        t++;
    }
    ip = (t > decimals) ? (uint8_t)(t - decimals) : 0U;
    iw = SC_MAX(SC_MAX(ip, SC_MIN(props->int_width, SC_NUMBER_INT_WIDTH_MAX)), 1U);
    /* 总长 = 符号 + 整数位宽 + 可选 小数点+小数；定位到末尾倒退写 */
    p = text + (uint8_t)((props->value < 0 ? 1U : 0U) +
                         iw +
                         (decimals != 0U ? (uint8_t)(1U + decimals) : 0U));
    *p = '\0';
    /* 小数部分：倒退写 decimals 位，mag 用尽后直接补 '0'（不发除法） */
    if (decimals != 0U)
    {
        for (i = 0U; i < decimals; i++)
        {
            if (mag != 0U)
            {
                *--p = (char)('0' + (uint8_t)(mag % 10U));
                mag /= 10U;
            }
            else
            {
                *--p = '0';
            }
        }
        *--p = '.';
    }
    /* 整数部分：先倒退写 ip 个真实数字，再补 (iw-ip) 个前导 0（补 0 对齐） */
    for (i = 0U; i < ip; i++)
    {
        *--p = (char)('0' + (uint8_t)(mag % 10U));
        mag /= 10U;
    }
    for (i = 0U; i < (uint8_t)(iw - ip); i++)
    {
        *--p = '0';
    }
    if (props->value < 0)
    {
        *--p = '-';
    }
}

/* 在 pfb 内按 view 对齐绘制数字文本（无纹理），超出视口部分被裁剪。 */
static void sc_draw_number(sc_pfb_t *pfb, const sc_number_t *props, const char *text, rect_t view)
{
    sc_draw_text_aligned(pfb, view, text, props->font_id,
                          props->x_gap, props->fg, NULL,
                          props->align, 0, props->opa, 0U);
}

/* 直接绘制数字到 LCD 或 PFB（裸绘制入口）。x/y 处按文字自然尺寸绘制。 */
void sc_lcd_draw_number(sc_pfb_t *pfb, int16_t x, int16_t y, sc_number_t *props)
{
    char text[SC_NUMBER_BUF_MAX];
    uint16_t w;
    uint16_t h;
    sc_number_format(props, text);
    sc_text_measure_lines(text, sc_get_font(props->font_id), props->x_gap,
                          &w, &h, NULL);
    rect_t view = (rect_t){x, y, w, h};
    if (pfb)
    {
        sc_draw_number(pfb, props, text, view);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_number(sc_lcd_draw_pfb__, props, text, view));
}

#if SC_USER_OBJ && SC_WIDGET_NUMBER_ENABLE
/* 创建数字控件（纯显示，无输入事件）。默认 value=0/decimals=0/int_width=0，
 * 之后可直接改 props 的 decimals/int_width/align 等配置字段，再 sc_number_set_value 改值。
 * 不改尺寸：view 由 sc_obj_set_rect 固定，文字在 view 内按 align 对齐。 */
static const sc_obj_vt_t sc_number_vt = {
    .event_cb = sc_obj_handle_number_event,
    .set_style = sc_number_style,
};


sc_obj_t *sc_number_create(sc_obj_t *parent, sc_number_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_NUMBER, &sc_number_vt, props);
    if (obj)
    {
        *props = (sc_number_t)SC_NUMBER_NEW(0, 0U, 0U, C_WHITE, SC_FONT_DEFAULT, 255U);
        sc_obj_invalidate(obj);
    }
    return obj;
}

/* 设置数值并整 view 失效重绘。decimals/int_width/align/font_id/x_gap 可在 create 后直接
 * 改 props，因此即使 value 相同也要重绘，让新配置生效（如 value=0 + decimals=2 → "0.00"）。 */
void sc_number_set_value(sc_obj_t *obj, int32_t value)
{
    sc_number_t *props = (sc_number_t *)obj->props;
    props->value = value;
    sc_obj_invalidate(obj);
}

/* 应用样式属性（前景色/对齐/字体/透明度）。 */
bool sc_number_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_number_t *props = (sc_number_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->fg = *(const color_t *)value;
        return true;
    case SC_STYLE_ALIGN:
        props->align = *(const sc_align_t *)value;
        sc_obj_invalidate(obj);
        return true;
    case SC_STYLE_FONT:
        props->font_id = *(const uint8_t *)value;
        sc_obj_invalidate(obj);
        return true;
    case SC_STYLE_OPA:
        props->opa = *(const uint8_t *)value;
        return true;
    default:
        return false;
    }
}

/* 处理数字控件事件：格式化到局部缓冲区后按 view 对齐绘制（纯显示控件，无输入事件）。 */
bool sc_obj_handle_number_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        char text[SC_NUMBER_BUF_MAX];
        sc_number_format((sc_number_t *)obj->props, text);
        sc_draw_number((sc_pfb_t *)event->dat.arg, (sc_number_t *)obj->props, text, obj->view);
        return true;
    }
    return false;
}
#endif /* SC_USER_OBJ && SC_WIDGET_NUMBER_ENABLE */

#endif /* SC_WIDGET_NUMBER_ENABLE */
