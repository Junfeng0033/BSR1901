

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
static void sc_draw_lv_letter(sc_pfb_t *dest, int xs, int ys, lv_font_glyph_dsc_t *dsc, uint32_t unicode, sc_label_t *label)
{
    sc_pfb_t tpfb;           // 临时pfb
    sc_dirty_t intersection; // 交集
    lv_font_t *font = label->font;
    font_dsc_t fdsc = sc_get_font_dsc(font); // 字体信息
    int16_t offs_y = ys + (font->line_height - dsc->box_h - dsc->ofs_y - font->base_line);
    int16_t offs_x = xs + dsc->ofs_x;
    sc_rect_t box = {.x = xs, .y = ys, .w = dsc->adv_w + fdsc.Xspace, .h = fdsc.line_height}; // 字体包围盒
    int16_t x, y, src_x, src_y;
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &box, gui->bkc);
    }
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
                    if (label->bcolor != gui->bkc)
                    {
                        dest_out[x] = label->bcolor;
                    }
                    if (src_x >= 0 && src_x < dsc->box_w && src_y >= 0 && src_y < dsc->box_h)
                    {
                        uint32_t offset = src_y * dsc->box_w + src_x;
                        uint8_t alpha = (dsc->bpp == 8) ? src[offset] : get_bpp_value(offset, src, dsc->bpp);
                        dest_out[x] = alphaBlend(label->color, dest_out[x], alpha);
                    }
                }
            }
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest));
}

// 取得字符串宽度
uint16_t _get_Label_width(uint16_t line_width_max, sc_label_t *label, uint32_t *indx, unicode_t *unicode, lv_font_glyph_dsc_t *dsc)
{
    lv_font_t *font = label->font;
    font_dsc_t fdsc = sc_get_font_dsc(font); // 字体信息
    uint16_t line_width = 0;                 // 字符串总宽
    uint16_t glyph_cnt = 0;
    uint32_t code = 0;
    for (;;)
    {
        code = lv_txt_utf8_next(label->text, indx); // txt转unicode
        if (code == '\n' || code == '\r' || code == 0)
        {
            unicode[glyph_cnt++] = code;
            break;
        }
        if (font->get_glyph_dsc(font, dsc, code, 0) == 0)
        {
            code = ' '; // 未知字符空格替代
            font->get_glyph_dsc(font, dsc, code, 0);
        }
        uint16_t adv_w = dsc->adv_w + fdsc.Xspace;
        line_width += adv_w;
        unicode[glyph_cnt++] = code;
        if (line_width + adv_w >= line_width_max || glyph_cnt >= SC_UINCODE_SIZE)
        {
            break;
        }
    }
    unicode[glyph_cnt] = 0; // 结束符
    return line_width;
}

/*显示一个lvgl字符串支持支持换行
 dest: 目标pfb
 xoffs: x对齐偏移
 yoffs: y对齐偏移
 parent: 父级矩形NULL表示全屏
 align: 父级对齐方式
 */
void sc_draw_Label(sc_pfb_t *dest, int xoffs, int yoffs, sc_label_t *label)
{
    if (label == NULL || label->font == NULL || label->text == NULL)
        return;
    lv_font_t *font = label->font;           // 字体
    font_dsc_t fdsc = sc_get_font_dsc(font); // 字体信息
    lv_font_glyph_dsc_t glyph_dsc;
    unicode_t unicode[SC_UINCODE_SIZE]; // unicode缓存
    uint32_t text_indx = 0;             // 字符串索引
    uint16_t total_height = 0;          // 累计行高
    int16_t glyph_cnt = 0;              // 字符统计
    sc_rect_t *parent = label->box;     // 父级矩形
    uint16_t line_width_max = parent ? parent->w : gui->lcd_rect.w;
    uint16_t line_height_max = parent ? parent->h : gui->lcd_rect.h;
    do
    {
        uint16_t line_width = _get_Label_width(line_width_max - xoffs, label, &text_indx, unicode, &glyph_dsc); // 计算行宽度（自动换行）
        int draw_x = xoffs;
        int draw_y = yoffs;
        sc_parent_align(label->box, &draw_x, &draw_y, line_width, font->line_height, label->align);
        if (dest != NULL)
        {
            sc_rect_t line_box = {.x = draw_x, .y = draw_y + total_height, .w = line_width, .h = fdsc.line_height};
            if (sc_rect_nor_intersect((sc_rect_t *)dest, &line_box))
            {
                for (int k = 0; unicode[k] != 0; k++, glyph_cnt++)
                    ;
                total_height += fdsc.line_height; // 累计总高度+字符统计
                if (total_height > line_height_max)
                    break;
                continue;
            }
        }
        for (int k = 0; unicode[k] != 0; k++, glyph_cnt++)
        {
            if (font->get_glyph_dsc(font, &glyph_dsc, unicode[k], 0) == 0)
                continue;
            sc_draw_lv_letter(dest, draw_x, draw_y + total_height, &glyph_dsc, unicode[k], label);
            draw_x += glyph_dsc.adv_w + fdsc.Xspace; // 更新下一个字符的X坐标（字形前进宽度 + 水平间距）
        }
        total_height += fdsc.line_height;
    } while (label->text[text_indx - 1] != 0); // 终止条件：解析到文本结束符（0）
}
/* label初始化
 * parent: 父级矩形
 * label: label构体
 * font: 字体
 * text: 初始文本常量
 * color: 字体颜色
 * align: 对齐方式，无对齐偏移
 */
void sc_init_Label(sc_rect_t *parent, sc_label_t *label, lv_font_t *font,  char *text, color_t tc, color_t bc, sc_align_t align)
{
    label->box = parent;
    label->font = font;
    label->text = text;
    label->color = tc;
    label->bcolor = bc;
    label->alpha = 255;
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
    sc_init_Label(&gui->lcd_rect, &label, font, (char *)text, tc,bc, 0);
    sc_draw_Label(dest, tx, ty, &label);
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
void sc_draw_Num(sc_pfb_t *dest, int tx, int ty, int w, int h, lv_font_t *font,int num, int den, color_t tc, color_t bc)
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
    sc_init_Label(&rect, &label, font, (char *)num_str, tc,bc, ALIGN_LEFT);
    do
    {
        sc_draw_Label(dest, 0, 0, &label);
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
}

/* 编辑框初始化,可以替代label,但label更轻量
 * parent: 父级矩形
 * edit: 编辑框结构体
 * font: 字体
 * text: 初始文本数组
 * color: 字体颜色
 * align: 对齐方式
 * xoffs：对齐后偏移，文本滚动支持
 * editbuf_size: 缓冲区大小为0不可以编辑退化为label
 */
void sc_init_Edit(sc_rect_t *parent, sc_edit_t *edit, lv_font_t *font, char *text, color_t tc, sc_align_t align, uint8_t editbuf_size)
{
    sc_init_Label(parent, &edit->label, font, text, tc,gui->bkc, align);
    edit->editbuf_size = editbuf_size; // 编辑缓冲区大小
    edit->xoffs = 2;                   // X偏移
    edit->yoffs = 2;                   // Y偏移
}
// 编辑框绘制
void sc_draw_Edit(sc_pfb_t *dest, sc_edit_t *edit)
{
    sc_pfb_t tpfb;
    sc_dirty_t intersection;
    sc_rect_t *box = edit->label.box;
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, box, gui->bkc);
    }
    do
    {
        sc_draw_Frame(dest, box->x, box->y, box->w, box->h, 1, C_BLUE,255);
        if (sc_pfb_intersection(dest, box, &intersection))
        {
            sc_draw_Label(dest, edit->xoffs, edit->yoffs, &edit->label); // 绘制文本
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest));
}

// 全局编辑状态
void *g_edit = NULL;           // 当前编辑对象
static int16_t g_pos_mark = 0; // 光标位置标记

// 获取光标位置
static bool sc_get_Edit_pos(sc_edit_t *edit, sc_rect_t *pos, int16_t x, int16_t y, int cmd)
{
    static int16_t g_pos_xs = 0;
    sc_rect_t *parent = edit->label.box;
    lv_font_t *font = edit->label.font;
    font_dsc_t fdsc = sc_get_font_dsc(font);
    lv_font_glyph_dsc_t g;
    unicode_t unicode[SC_UINCODE_SIZE];
    uint16_t total_height = 0;
    uint16_t glyph_cnt = 0;
    uint32_t indx = 0;
    uint16_t line_width_max = parent != NULL ? parent->w : gui->lcd_rect.w;
    if (cmd)
    {
        x = pos->x;
        y = pos->y;
        if (cmd == CMD_RIGHT)
        {
            g_pos_mark++;
        }
        else if (cmd == CMD_LEFT)
        {
            if (g_pos_mark <= 0)
                return false;
            g_pos_mark--;
        }
        else if (cmd == CMD_UP)
        {
            y = y - pos->h;
            cmd = 0; // 模拟触控
        }
        else if (cmd == CMD_DOWN)
        {
            y = y + pos->h;
            cmd = 0; // 模拟触控
        }
    }
    do
    {
        uint16_t line_width = _get_Label_width(line_width_max - edit->xoffs, &edit->label, &indx, unicode, &g);
        int xs = edit->xoffs;
        int ys = edit->yoffs;
        sc_parent_align(parent, &xs, &ys, line_width, fdsc.line_height, edit->label.align);
        g_pos_xs = xs;

        sc_rect_t box = {.x = edit->label.box->x, .y = ys + total_height, .w = edit->label.box->w, .h = fdsc.line_height};
        if (cmd != 0)
        {
            box.x = xs;
            for (int k = 0; unicode[k] != 0; k++) // 通过g_pos_mark查找光标
            {
                font->get_glyph_dsc(font, &g, unicode[k], 0);
                box.w = g.adv_w + fdsc.Xspace;
                if (glyph_cnt >= g_pos_mark)
                {
                    *pos = box; // 找到位置返回
                    return true;
                }
                box.x += box.w;
                glyph_cnt++;
            }
            *pos = box; // 找不到返回最后一个
        }
        else if (sc_rect_touch_ctx(&box, x, y)) // 通过位置查找光标
        {
            box.x = xs;
            for (int k = 0; unicode[k]; k++)
            {
                if (font->get_glyph_dsc(font, &g, unicode[k], 0) == 0)
                    continue;
                box.w = g.adv_w + fdsc.Xspace;
                if (x < g_pos_xs)
                {
                    break; // 如果点在字符外左边返回第一个
                }
                if (sc_rect_touch_ctx(&box, x, y)) // 字符内
                {
                    int right_half = box.x + box.w / 2;                                   // 字符中心
                    if ((x > right_half) && font->get_glyph_dsc(font, &g, unicode[k], 0)) // 下一个
                    {
                        box.w = g.adv_w + fdsc.Xspace;
                        box.x += box.w;
                        glyph_cnt++;
                    }
                    break;
                }
                box.x += box.w;
                glyph_cnt++;
            }
            *pos = box;             // 记录位置
            g_pos_mark = glyph_cnt; // 记录mark下标
            return true;
        }
        else
        {
            for (int k = 0; unicode[k]; k++, glyph_cnt++)
                ;
        }
        total_height += fdsc.line_height;
    } while (edit->label.text[indx - 1] != 0); // 判断是否结束符
    g_pos_mark = g_pos_mark > glyph_cnt ? glyph_cnt : g_pos_mark; // 越界保护
    return false;
}
/**
 * @brief  字符串编辑函数
 * @param   text_buf    可修改的ASCII字符串缓冲区
 */
static bool sc_put_Edit(sc_edit_t *edit, uint8_t cmd)
{
    char *text_buf = edit->label.text;
    if (edit == NULL || text_buf == NULL)
        return false;
    if (cmd == CMD_BACK)
    {
        // 边界：空串/光标在首位（无当前字符），删除失败
        if (g_pos_mark <= 0)
            return false;
        char *p = &text_buf[g_pos_mark - 1];
        do
        {
            p[0] = p[1];
            p++;
        } while (*p);
        g_pos_mark--; // 光标前移
        return true;
    }
    else if (cmd == CMD_DEL)
    {
        // 边界：空串/光标在末尾（无右侧字符），删除失败
        char *p = &text_buf[g_pos_mark];
        while (*p)
        {
            p[0] = p[1];
            p++;
        }
        return true; // 光标位置不变
    }
    else if (cmd >= CMD_A && cmd <= CMD_Z)
    {
        cmd = 'a' + (cmd - 4);
        uint16_t curr_len = strlen(text_buf); // 英文下字符数=字节数
        if (curr_len >= edit->editbuf_size - 1)
            return false;
        // 光标在末尾：直接追加到最后（光标后无字符）
        if (g_pos_mark >= curr_len)
        {
            text_buf[curr_len] = cmd;
            text_buf[curr_len + 1] = '\0';
        }
        else
        {
            for (int i = curr_len; i > g_pos_mark; i--)
            {
                text_buf[i] = text_buf[i - 1];
            }
            text_buf[g_pos_mark] = cmd;
        }
        g_pos_mark++; // 光标后移（跟随插入的字符）
        return true;
    }
    return false;
}
/*光标绘制 */
static void sc_draw_Edit_pos(sc_rect_t *pos, color_t color)
{
    sc_draw_Fill(NULL, pos->x - 1, pos->y, 1, pos->h, color,255); //
}
// 编辑框事件处理
bool sc_event_Edit(sc_event_t *event, sc_edit_t *edit)
{
    static sc_rect_t pos_last;
    if (edit == NULL || edit->editbuf_size == 0)
        return false;
    bool hit = false;
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        if (g_edit)
        {
            sc_draw_Edit_pos(&pos_last, gui->bkc);
        }
        if (sc_get_Edit_pos(edit, &pos_last, event->dat.pos[0], event->dat.pos[1], 0))
        {
            sc_draw_Edit_pos(&pos_last, C_RED);
            g_edit = edit;
            hit = true;
        }
        else if (g_edit)
        {
            g_edit = NULL;
        }
    }
    else if (event->type == SC_EVENT_TYPE_CMD && g_edit == edit)
    {
        switch (event->dat.cmd)
        {
        case CMD_RIGHT:
        case CMD_LEFT:
        case CMD_UP:
        case CMD_DOWN:
            sc_draw_Edit_pos(&pos_last, gui->bkc);
            sc_get_Edit_pos(edit, &pos_last, 0, 0, event->dat.cmd);
            sc_draw_Edit_pos(&pos_last, C_RED);
            hit = true;
            break;
        default:
            if (sc_put_Edit(edit, event->dat.cmd))
            {
                sc_draw_Edit(NULL, edit);
                sc_get_Edit_pos(edit, &pos_last, 0, 0, event->dat.cmd);
                sc_draw_Edit_pos(&pos_last, C_RED);
                hit = true;
            }
            break;
        }
    }
    return hit;
}

// 编辑框演示
sc_edit_t edit1;
void sc_demo_Edit(void)
{
    static char label_str[64] = "123456\n78901234567";
    static sc_rect_t box = {100, 60, 120, 100};
    sc_init_Edit(&box, &edit1, gui->font, label_str, C_BLACK, ALIGN_HOR, 64);
    sc_draw_Edit(NULL, &edit1);
}
void sc_demo_Edit_event(sc_event_t *event)
{
    // 取得字符串光标位置
    if (event->type == SC_EVENT_TOUCH_DOWN || event->type == SC_EVENT_TYPE_CMD)
    {
        sc_event_Edit(event, &edit1);
    }
}
