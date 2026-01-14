
#include "sc_gui.h"

///-LED-
void sc_draw_Led(sc_pfb_t *dest, int cx, int cy, int r, color_t color, uint16_t alpha)
{
    sc_draw_Rounded_rect(dest, cx - r, cy - r, 2 * r + 1, 2 * r + 1, r, 0, color, gui->bkc, alpha);
}

///-按键初始化-
void sc_init_Button(sc_button_t *button, lv_font_t *font, char *text, color_t txtcolor)
{
    sc_label_t *label = &button->label;
    label->box = NULL;
    label->font = font;
    label->text = text;
    label->color = txtcolor;
    label->bcolor = gui->bkc;
    label->align = ALIGN_CENTER;
    button->state = 0;
}

///-按键组合-
void sc_draw_Button(sc_pfb_t *dest, int xs, int ys, int w, int h, uint8_t r, uint8_t ir, color_t ac, color_t fill, sc_button_t *button)
{
    sc_dirty_t intersection;
    sc_pfb_t tpfb;
    sc_rect_t rect = {xs, ys, w, h};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return;
    button->label.box = &rect;
    uint16_t alpha = gui->alpha;
    do
    {
        sc_draw_Rounded_rect(dest, xs, ys, w, h, r, ir, ac, fill, alpha);
        sc_draw_Label(dest, 0, 0, &button->label);
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
}

///-开关组合-
void sc_draw_Switch(sc_pfb_t *dest, int xs, int ys, int w, int h, color_t ac, color_t fill, uint8_t state)
{
    sc_dirty_t intersection;
    sc_pfb_t tpfb;
    sc_rect_t rect = {xs, ys, w, h};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return;
    int min = SC_MIN(w, h);
    int cw = min - 4;
    int x1 = xs + 2;
    int y1 = ys + 2;
    int r = min / 2;
    int ir = r - 2;
    uint16_t bkc = alphaBlend(ac, gui->bkc, 128);
    uint16_t alpha = gui->alpha;
    do
    {
        if (state)
        {
            sc_draw_Rounded_rect(dest, xs, ys, w, h, r, r - 1, ac, bkc, alpha);
            sc_draw_Rounded_rect(dest, x1 + w - min, y1, cw, cw, ir, 0, ac, ac, alpha);
        }
        else
        {
            sc_draw_Rounded_rect(dest, xs, ys, w, h, r, r - 1, ac, fill, alpha);
            sc_draw_Rounded_rect(dest, x1, y1, cw, cw, ir, 0, ac, ac, alpha);
        }

    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
}

// 文本框初始化
void sc_init_Textbox(sc_textbox_t *textbox, lv_font_t *font, char *text, color_t txtcolor)
{
    sc_label_t *label = &textbox->label;
    label->box = NULL;
    label->font = font;
    label->text = text;
    label->color = txtcolor;
    label->bcolor = gui->bkc;
    label->align = ALIGN_LEFT;
    textbox->xoffs = 2;
    textbox->yoffs = 2;
    textbox->bar_w = 4;
}

// 文本框组合
void sc_draw_Textbox(sc_pfb_t *dest, int xs, int ys, int w, int h, color_t boxcolor, sc_textbox_t *textbox)
{
    sc_dirty_t intersection;
    sc_pfb_t tpfb;
    sc_rect_t rect = {xs, ys, w, h};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return;
    uint8_t bar_w = textbox->bar_w;
    uint8_t bar_h = h / 3;
    int16_t bar_x = xs + w - bar_w;
    int16_t bar_y = ys - textbox->yoffs;        // 进度度条y坐标
    sc_rect_t txt_box = {xs, ys, w - bar_w, h}; // 文本框区域减进度条
    textbox->label.box = &txt_box;
    uint16_t alpha = gui->alpha;
    do
    {
        sc_draw_Frame(dest, xs, ys, w, h, 1, boxcolor, alpha);
        sc_draw_Label(dest, textbox->xoffs, textbox->yoffs, &textbox->label);
        //------------进度条显示------------
        if (bar_w > 0)
        {
            sc_draw_Fill(dest, bar_x, ys, bar_w, h, boxcolor, alpha);
            sc_draw_Fill(dest, bar_x, bar_y, bar_w, bar_h, C_RED, alpha);
        }

    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
}

void sc_draw_Slider(sc_pfb_t *dest, int xs, int ys, int w, int h, color_t ac, color_t fill, int vol, int max)
{
    sc_dirty_t intersection;
    sc_pfb_t tpfb;
    sc_rect_t rect = {xs, ys, w, h};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return;
    int16_t r = SC_MIN(w, h) / 2;
    int16_t x1 = xs + 3;
    int16_t y1 = ys + 3;
    int16_t w1 = w - 6;
    int16_t h1 = h - 6;
    do
    {
        if (sc_draw_Rounded_rect(dest, xs, ys, w, h, r, r - 1, ac, gui->bkc, gui->alpha))
        {
            if (w1 >= h1)
            {
                sc_draw_Rounded_rect(dest, x1, y1, w1 * vol / max, h1, r - 3, 0, fill, fill, gui->alpha);
            }
            else
            {
                sc_draw_Rounded_rect(dest, x1, y1, w1, h1 * vol / max, r - 3, 0, fill, fill, gui->alpha);
            }
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
}

void sc_put_Chart_ch(sc_chart_t *p, int16_t vol, uint16_t scaleX, color_t color)
{
    uint16_t fx=p->indx & 0xff; // 当前下标小数
    p->indx += scaleX;          // 累加坐标
    int wp = p->indx >> 8;      // 当前下标取整
    uint16_t  size= sizeof(p->dat_buf) / sizeof(p->dat_buf[0]);
    if (wp >= size)
    {
        wp = 0;
        p->indx = p->indx & 0xff; // 下标保留小数
    }
    int stup =(vol - p->last)<<8;  
    for (int i=1;p->wp != wp; i++)
    {
        p->dat_buf[p->wp] =  p->last + stup*(i) /(fx+scaleX); // 插值
        if (++p->wp >= size)
            p->wp = 0;
    }
    p->last = vol;
    p->color = color;
}


// 绘制曲线
void sc_draw_Chart(sc_pfb_t *dest, int xs, int ys, int w, int h, uint16_t gc, int gx, int gy, sc_chart_t *p, int ch)
{
    sc_dirty_t intersection;
    sc_pfb_t tpfb;
    sc_rect_t rect = {xs, ys, w, h};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return;
    int xe = xs + w - 1;
    int ye = ys + h - 1;
    int base_line_x = w / 2 + xs; // x基准线
    int base_line_y = h / 2 + ys; // y基准线
    int xd = gx;
    int yd = gy;
    uint16_t  size= sizeof(p->dat_buf) / sizeof(p->dat_buf[0]);
    do
    {
        int x, y, min, max;
        uint16_t *dest_out;
        for (y = intersection.ys; y < intersection.ye; y++)
        {
            if ((y - base_line_y) % yd == 0 || (y == ys || y == ye))
            {
                bool Solid = (y == base_line_y || y == ys || y == ye) ? 1 : 0; // 实线标志
                dest_out = dest->buf + (y - dest->y) * dest->w + (intersection.xs - dest->x);
                for (x = intersection.xs; x < intersection.xe; x++)
                {
                    *dest_out = (Solid || (x & 0x03) == 0) ? gc : *dest_out; // 水平线
                    dest_out++;
                }
            }
        }
        // 时间轴
        for (x = intersection.xs; x < intersection.xe; x++)
        {
            if ((x - base_line_x) % xd == 0 || x == xs || x == xe)
            {
                bool Solid = (x == base_line_x || x == xs || x == xe) ? 1 : 0; // 实线标志
                dest_out = dest->buf + (intersection.ys - dest->y) * dest->w + x - dest->x;
                for (y = intersection.ys; y < intersection.ye; y++)
                {
                    *dest_out = (Solid || (y & 0x03) == 0) ? gc : *dest_out; ////垂直线
                    dest_out += dest->w;
                }
            }
            if (x >= xe)
                break;
            for (int i = 0; i < ch; i++)
            {
                sc_chart_t *chart = &p[i];
                uint16_t wp = (chart->wp + x-xs)%size;
                uint16_t wp1 = (wp+1>=size) ? 0 : wp+1;
                int16_t v0= chart->dat_buf[wp];
                int16_t v1= chart->dat_buf[wp1];
                min = base_line_y - v0;
                max = base_line_y - v1;
                if (min > max)
                {
                    min = max + 1;
                    max = base_line_y - v0;
                }
                else if (min < max)
                {
                    max -= 1;
                }
                min = SC_MAX(min, intersection.ys);
                max = SC_MIN(max, intersection.ye - 1);
                dest_out = dest->buf + (min - dest->y) * dest->w + x - dest->x;
                for (y = min; y <= max; y++)
                {
                    *dest_out = chart->color; // 波形
                    dest_out += dest->w;
                }
            }
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
}

// 绘制组合图形
void sc_draw_compose(sc_pfb_t *dest, sc_rect_t *compose_rect, void *arg)
{
    sc_dirty_t intersection;
    sc_pfb_t tpfb;
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, compose_rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, compose_rect, &intersection))
        return;
    do
    {
        // 组合图形
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, compose_rect, &intersection));
}
