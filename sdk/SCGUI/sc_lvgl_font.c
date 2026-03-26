

#include "sc_gui.h"

#if 0
//将unicode编码转为utf-8编码（函数来自LVGL）
uint8_t lv_txt_utf8_get_byte_count(uint32_t c)
{
    if (c <= 0x7F)
        return 1; // ASCII 字符（0-127）：1 字节
    else if (c <= 0x7FF)
        return 2; // 扩展ASCII：2 字节（极少用）
    else if (c <= 0xFFFF)
        return 3; // 大部分中文/日文等：3 字节
    else if (c <= 0x10FFFF)
        return 4; // 罕见字符：4 字节
    else
        return 1; // 非法字符，默认1字节
}
//将unicode编码转为utf-8编码（函数来自LVGL）
uint8_t lv_txt_utf8_encode(char *buf, uint32_t c)
{
    if (buf == NULL)
        return 0;
    if (c <= 0x7F)
    {
        buf[0] = (char)c;
        return 1;
    }
    else if (c <= 0x7FF)
    {
        buf[0] = (char)(0xC0 | ((c >> 6) & 0x1F)); // 110xxxxx
        buf[1] = (char)(0x80 | (c & 0x3F));        // 10xxxxxx
        return 2;
    }
    else if (c <= 0xFFFF)
    {
        buf[0] = (char)(0xE0 | ((c >> 12) & 0x0F)); // 1110xxxx
        buf[1] = (char)(0x80 | ((c >> 6) & 0x3F));  // 10xxxxxx
        buf[2] = (char)(0x80 | (c & 0x3F));         // 10xxxxxx
        return 3;
    }
    else if (c <= 0x10FFFF)
    {
        buf[0] = (char)(0xF0 | ((c >> 18) & 0x07)); // 11110xxx
        buf[1] = (char)(0x80 | ((c >> 12) & 0x3F)); // 10xxxxxx
        buf[2] = (char)(0x80 | ((c >> 6) & 0x3F));  // 10xxxxxx
        buf[3] = (char)(0x80 | (c & 0x3F));         // 10xxxxxx
        return 4;
    }
    else
    {
        return 0; // 非法Unicode码值（超出UTF-8范围）
    }
}
#endif
// 将utf-8编码转为unicode编码（函数来自LVGL）
static uint32_t lv_txt_utf8_next(const char *txt, uint32_t *i)
{
    uint32_t result = 0;
    uint32_t i_tmp = 0;
    if (i == NULL)
        i = &i_tmp;
    if ((txt[*i] & 0x80) == 0) // Normal ASCII
    {
        result = txt[*i];
        (*i)++;
    }
    else // Real UTF-8 decode
    {
        // bytes UTF-8 code
        if ((txt[*i] & 0xE0) == 0xC0)
        {
            result = (uint32_t)(txt[*i] & 0x1F) << 6;
            (*i)++;
            if ((txt[*i] & 0xC0) != 0x80)
                return 0; // Invalid UTF-8 code
            result += (txt[*i] & 0x3F);
            (*i)++;
        }
        // 3 bytes UTF-8 code
        else if ((txt[*i] & 0xF0) == 0xE0)
        {
            result = (uint32_t)(txt[*i] & 0x0F) << 12;
            (*i)++;

            if ((txt[*i] & 0xC0) != 0x80)
                return 0;
            result += (uint32_t)(txt[*i] & 0x3F) << 6;
            (*i)++;

            if ((txt[*i] & 0xC0) != 0x80)
                return 0;
            result += (txt[*i] & 0x3F);
            (*i)++;
        }
        else if ((txt[*i] & 0xF8) == 0xF0)
        {
            result = (uint32_t)(txt[*i] & 0x07) << 18;
            (*i)++;
            if ((txt[*i] & 0xC0) != 0x80)
                return 0;
            result += (uint32_t)(txt[*i] & 0x3F) << 12;
            (*i)++;

            if ((txt[*i] & 0xC0) != 0x80)
                return 0;
            result += (uint32_t)(txt[*i] & 0x3F) << 6;
            (*i)++;

            if ((txt[*i] & 0xC0) != 0x80)
                return 0;
            result += txt[*i] & 0x3F;
            (*i)++;
        }
        else
        {
            (*i)++;
        }
    }
    return result;
}

/*显示一个lvgl字符*/
void sc_draw_lv_letter(sc_pfb_t *dest, int xs, int ys, lv_font_glyph_dsc_t *dsc, uint32_t unicode, lv_font_t *font, color_t tc, color_t bc, sc_align_t alpha)
{
    sc_pfb_t tpfb;                                                                            // 临时pfb
    sc_dirty_t intersection;                                                                  // 交集
    font_dsc_t fdsc = sc_get_font_dsc(font);                                                  // 字体信息
    sc_rect_t box = {.x = xs, .y = ys, .w = dsc->adv_w + fdsc.Xspace, .h = fdsc.line_height}; // 字体包围盒
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &box, gui->bkc);
    }
    int16_t offs_y = ys + (font->line_height - dsc->box_h - dsc->ofs_y - font->base_line);
    int16_t offs_x = xs + dsc->ofs_x;
    int16_t x, y, src_x, src_y;
    do
    {
        if (sc_pfb_intersection(dest, &box, &intersection)) // 判断是否有交集
        {
            const uint8_t *src = font->get_glyph_bitmap(font, unicode);
            uint16_t *dest_out = dest->buf + (intersection.ys - dest->y) * dest->w - dest->x;
            for (y = intersection.ys; y < intersection.ye; y++, dest_out += dest->w)
            {
                src_y = y - offs_y;
                for (x = intersection.xs; x < intersection.xe; x++)
                {
                    src_x = x - offs_x;
                    if (bc != gui->bkc)
                    {
                        dest_out[x] = bc;
                    }
                    if (src_x >= 0 && src_x < dsc->box_w && src_y >= 0 && src_y < dsc->box_h)
                    {
                        uint32_t offset = src_y * dsc->box_w + src_x;
                        uint8_t a = (dsc->bpp == 8) ? src[offset] : get_bpp_value(offset, src, dsc->bpp);
                        dest_out[x] = alphaBlend(tc, dest_out[x], a * alpha / 255);
                    }
                }
            }
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest));
}

// 取得字符串行宽度
font_dsc_t sc_get_line_width(uint16_t line_width_max, sc_label_t *label, uint32_t *indx, unicode_t *unicode, lv_font_glyph_dsc_t *dsc)
{
    lv_font_t *font = label->font;
    font_dsc_t fdsc = sc_get_font_dsc(font); // 字体信息
    uint32_t code = 0;
    uint8_t align = label->align & ALIGN_RIGHT;
    for (;;)
    {
        code = lv_txt_utf8_next(label->text, indx); // txt转unicode
        if (code == '\n' || code == '\r' || code == 0)
        {
            unicode[fdsc.line_cnt++] = code;
            break;
        }
        if (font->get_glyph_dsc(font, dsc, code, 0) == 0)
        {
            code = ' '; // 未知字符空格替代
            font->get_glyph_dsc(font, dsc, code, 0);
        }
        uint16_t adv_w = dsc->adv_w + fdsc.Xspace;
        fdsc.line_width += adv_w;
        unicode[fdsc.line_cnt++] = code;
        if ((fdsc.line_width + adv_w >= line_width_max && align == 0) || fdsc.line_cnt >= SC_UINCODE_SIZE)
        {
            break;
        }
    }
    unicode[fdsc.line_cnt] = 0; // 结束符
    return fdsc;
}

/*显示一个lvgl字符串支持支持换行
 dest: 目标pfb
 xoffs: x对齐偏移
 yoffs: y对齐偏移
 parent: 父级矩形NULL表示全屏
 align: 父级对齐方式
 */
void sc_draw_Label(sc_pfb_t *dest, sc_label_t *label, color_t tc, color_t bc, uint8_t alpha)
{
    if (label == NULL || label->font == NULL || label->text == NULL)
        return;
    lv_font_t *font = label->font; // 字体
    lv_font_glyph_dsc_t glyph_dsc;
    unicode_t unicode[SC_UINCODE_SIZE]; // unicode缓存
    uint32_t text_indx = 0;             // 字符串索引
    uint16_t total_height = 0;          // 累计行高
    sc_rect_t *parent = label->box;     // 父级矩形
    uint16_t line_width_max = parent ? parent->w : gui->lcd_rect.w;
    do
    {
        font_dsc_t fdsc = sc_get_line_width(line_width_max - label->xoffs, label, &text_indx, unicode, &glyph_dsc); // 计算行宽度（自动换行）
        int draw_x = label->xoffs;
        int draw_y = label->yoffs;
        sc_parent_align(label->box, &draw_x, &draw_y, fdsc.line_width, font->line_height, label->align);
        if (dest != NULL)
        {
            sc_rect_t box = {.x = draw_x, .y = draw_y + total_height, .w = fdsc.line_width, .h = fdsc.line_height};
            if (sc_rect_nor_intersect((sc_rect_t *)dest, &box))
            {
                total_height += fdsc.line_height; // 累计总高度+字符统计
                continue;
            }
        }
        for (int k = 0; k < fdsc.line_cnt; k++)
        {
            if (font->get_glyph_dsc(font, &glyph_dsc, unicode[k], 0))
            {
                sc_draw_lv_letter(dest, draw_x,draw_y + total_height, &glyph_dsc, unicode[k], font, tc, bc, alpha);
            }
            draw_x += glyph_dsc.adv_w + fdsc.Xspace; // 更新下一个字符的X坐标（字形前进宽度 + 水平间距）
        }
        total_height += fdsc.line_height;
    } while (label->text[text_indx - 1] != 0); // 终止条件：解析到文本结束符（0）
}
/* label初始化
 * parent: 父级矩形
 * offsx: x对齐偏移
 * offsy: y对齐偏移
 * label: label构体
 * font: 字体
 * text: 初始文本常量
 * align: 对齐方式
 */
void sc_init_Label(sc_rect_t *parent, int offsx, int offsy, sc_label_t *label, lv_font_t *font, char *text, sc_align_t align)
{
    label->box = parent;
    label->font = font;
    label->text = text;
    label->xoffs = offsx;
    label->yoffs = offsy;
    label->align = align;
}

/* 字符绘制，支持换行
 * dest: 目标pfb
 * tx: tx坐标
 * ty: ty坐标
 * font: 字体
 * text: 字符串
 * tc: 字符色色
 */
void sc_draw_Text(sc_pfb_t *dest, int tx, int ty, lv_font_t *font, const char *text, color_t tc, color_t bc)
{
    sc_label_t label;
    sc_init_Label(&gui->lcd_rect, tx, ty, &label, font, (char *)text, 0);
    sc_draw_Label(dest, &label, tc, bc, 255);
}

/* 显示数字，分子分母形式显示小数
 * dest: 目标pfb
 * tx: tx坐标
 * ty: ty坐标
 * font: 字体
 * num: 分子
 * den: 分母 1,10,100,1000,10000,100000,1000000
 * tc: 字体颜色
 * bc: 边框颜色
 */
void sc_draw_Num(sc_pfb_t *dest, int tx, int ty, int w, int h, lv_font_t *font, int num, int den, color_t tc, color_t bc)
{
    sc_pfb_t tpfb;
    sc_dirty_t intersection;
    sc_label_t label;
    sc_rect_t rect = {.x = tx, .y = ty, .w = w, .h = h};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return;
    char num_str[10];
    char *p = num_str;
    if (num < 0)
    {
        num *= -1;
        *p++ = '-'; //-号
    }
    for (int32_t temp = 100000; temp; temp /= 10)
    {
        if (num >= temp || den >= temp)
        {
            *p++ = num / temp % 10 + '0'; //
        }
        else if (temp < 10)
        {
            *p++ = '0';
        }
        if (den >= 10 && temp == den)
        {
            *p++ = '.'; // 小数点
        }
    }
    *p = '\0';
    sc_init_Label(&rect, 0, 0, &label, font, (char *)num_str, ALIGN_LEFT);
    do
    {
        sc_draw_Label(dest, &label, gui->fc, gui->bc, 255);
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
}
