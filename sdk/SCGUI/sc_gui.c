
#include "sc_gui.h"
sc_gui_t *gui = NULL;

/* 弱定义函数由用户重写*/
void spiflash_read(uint8_t *buf, uint32_t offset, uint32_t size)
{
    // 支持字体图片读取
}

/* 初始化系统 */
void sc_gui_init(lcd_refresh_cb refresh_cb, color_t bkc, color_t fc, color_t bc, void *font)
{
    static sc_gui_t tft;
    gui = &tft;
    gui->lcd_rect.x = 0;
    gui->lcd_rect.y = 0;
    gui->lcd_rect.w = SC_SCREEN_WIDTH;
    gui->lcd_rect.h = SC_SCREEN_HEIGHT;
    gui->g_mask = NULL;
    gui->refresh_cb = refresh_cb;
    gui->font = font;
    gui->bkc = bkc;
    gui->fc = fc;
    gui->bc = bc;
    gui->alpha = 255;
}

/**fun:清屏*/
void sc_clear(int x, int y, int w, int h, color_t color)
{
    sc_pfb_t tpfb = {0}; // 临时pfb
    sc_rect_t rect = {x, y, w, h};
    sc_pfb_init_slices(&tpfb, &rect, color);
    do
    {
    } while (sc_pfb_next_slice(&tpfb));
}

/**fun: 矩形填充*/
void sc_draw_Fill(sc_pfb_t *dest, int xs, int ys, int w, int h, color_t color, uint16_t alpha)
{
    if (dest == NULL)
    {
        color = alphaBlend(color, gui->bkc, gui->alpha); // 透明度
        sc_clear(xs, ys, w, h, color);
        return;
    }
    sc_dirty_t intersection;
    sc_rect_t rect = {xs, ys, w, h};
    if (sc_pfb_intersection(dest, &rect, &intersection))
    {
        for (int y = intersection.ys; y < intersection.ye; y++)
        {
            uint16_t *dest_out = &dest->buf[(y - (dest->y)) * dest->w - dest->x + intersection.xs];
            for (int x = intersection.xs; x < intersection.xe; x++)
            {
                *dest_out = alphaBlend(color, *dest_out, alpha);
                dest_out++;
            }
        }
    }
}

/**fun: 空芯矩形*/
void sc_draw_Frame(sc_pfb_t *dest, int xs, int ys, int w, int h, int lw, color_t color, uint16_t alpha)
{
    if(w <=  lw || h <=  lw) return;
    sc_draw_Fill(dest, xs, ys, w, lw, color, alpha);
    sc_draw_Fill(dest, xs, ys + h - lw, w, lw, color, alpha);
    sc_draw_Fill(dest, xs, ys + lw, lw, h - 2 * lw, color, alpha);
    sc_draw_Fill(dest, xs + w - lw, ys + lw, lw, h - 2 * lw, color, alpha);
}

/****** 圆角矩形**********
 * dest: 目标pfb
 * x,y: 目标位置
 * w,h: 目标大小
 * r: 圆角半径
 * ir: 内圆角半径
 * alpha: 透明度
 * color: 边框颜色
 * fill: 填充颜色
 */
bool sc_draw_Rounded_rect(sc_pfb_t *dest, int xs, int ys, int w, int h, int r, int ir, color_t color, color_t fill, uint16_t alpha)
{
    sc_pfb_t tpfb; // 临时pfb
    sc_dirty_t intersection;
    sc_rect_t rect = {xs, ys, w, h};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &rect, gui->bkc); /// 初始化局部切片
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return false;
    do
    {
        const bool need_fill = (fill != gui->bkc);
        const sc_dirty_t fill_mask = sc_rect_to_dirty(gui->g_mask ? gui->g_mask : &gui->lcd_rect);
        // 2. 循环外预计算
        const uint16_t r2 = r * r;
        const uint16_t rmax = (r + 1) * (r + 1);
        const uint16_t ir2 = ir * ir;
        const uint16_t rmin = ir > 0 ? (ir - 1) * (ir - 1) : 0;
        // 3. 预计算所有边界（循环外一次性算完，内层直接用）
        const int16_t ax = xs + r;
        const int16_t bx = xs + w - r - 1;
        const int16_t ay = ys + r;
        const int16_t by = ys + h - r - 1;
        const uint16_t inv_outer = (rmax - r2) ? (alpha << 8) / (rmax - r2) : 0;   // 除法优化
        const uint16_t inv_inner = (ir2 - rmin) ? (alpha << 8) / (ir2 - rmin) : 0; // 除法优化
        const uint16_t dest_w = dest->w;
        uint16_t *dest_buf = dest->buf + (intersection.ys - dest->y) * dest_w - dest->x + intersection.xs;
        int x, y;
        for (y = intersection.ys; y < intersection.ye; y++, dest_buf += dest_w)
        {
            uint16_t *dest_out = dest_buf;
            if (y > ay && y < by) // 中间填充
            {
                for (x = intersection.xs; x < intersection.xe; x++, dest_out++)
                {
                    if (x <= ax - ir || x >= bx + ir) // 左右垂直线，线宽判断
                    {
                        *dest_out = alphaBlend(color, *dest_out, alpha);
                    }
                    else if (need_fill && y >= fill_mask.ys && x < fill_mask.xe)
                    {
                        *dest_out = alphaBlend(fill, *dest_out, alpha);
                    }
                }
                continue;
            }
            int y_rel = y > ay ? y - by : y - ay;
            int Ysq = y_rel * y_rel;
            for (x = intersection.xs; x < intersection.xe; x++, dest_out++)
            {
                if (x > ax && x < bx)
                {
                    if (Ysq >= ir2) // 上下水平直线，线宽判断
                    {
                        *dest_out = alphaBlend(color, *dest_out, alpha);
                    }
                    else if (need_fill && y >= fill_mask.ys && x < fill_mask.xe) //
                    {
                        *dest_out = alphaBlend(fill, *dest_out, alpha);
                    }
                }
                else
                {
                    int x_rel = x > ax ? (x - bx) : (x - ax);
                    int Rsq = x_rel * x_rel + Ysq; // =R*R
                    if (Rsq >= rmax)
                    {
                        if (x > bx)
                            break; // 大于rmax外圆跳过
                    }
                    else if (Rsq > r2)
                    {
                        int dist = (rmax - Rsq) * inv_outer >> 8; // 外边缘抗锯齿
                        *dest_out = alphaBlend(color, *dest_out, dist);
                    }
                    else if (Rsq < ir2)
                    {
                        int dist = (Rsq - rmin);
                        if (need_fill && y >= fill_mask.ys && x < fill_mask.xe) //
                        {
                            color_t tbc = (dist > 0) ? alphaBlend(color, fill, dist * inv_inner >> 8) : fill;
                            *dest_out = alphaBlend(tbc, *dest_out, alpha);
                        }
                        else if (dist > 0)
                        {
                            dist = dist * inv_inner >> 8; // 内边缘抗锯齿
                            *dest_out = alphaBlend(color, *dest_out, dist);
                        }
                    }
                    else
                    {
                        *dest_out = alphaBlend(color, *dest_out, alpha);
                    }
                }
            }
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
    return true;
}

/*************绘制圆角进度条***********
 * dest: 目标pfb
 * x,y: 目标位置
 * w,h: 目标大小
 * r: 圆角半径
 * ir: 内圆角半径
 * ac: 圆角颜色
 * fill:  填充颜色
 * vol: 当前值
 * max: 最大值
 */
void sc_draw_Bar(sc_pfb_t *dest, int xs, int ys, int w, int h, int r, int ir, color_t ac, color_t fill, int vol, int max)
{
    int rmax = SC_MIN(w / 2, h / 2);
    if (r > rmax)
        r = rmax;
    sc_rect_t fill_mask;
    if (w >= h)
    {
        fill_mask.x = xs;
        fill_mask.y = ys;
        fill_mask.w = vol * w / max;
        fill_mask.h = h;
    }
    else
    {
        int hmax = vol * h / max;
        fill_mask.x = xs;
        fill_mask.y = ys + h - hmax;
        fill_mask.w = w;
        fill_mask.h = hmax;
    }
    gui->g_mask = &fill_mask;
    sc_draw_Rounded_rect(dest, xs, ys, w, h, r, ir, ac, fill, gui->alpha);
    gui->g_mask = NULL;
}

/* 绘制一个icon字符
 * dest: 目标pfb
 * xs,ys: 目标位置
 * w,h: 目标大小
 * src: 图像数据
 * bpp: 图像数据位宽
 * fc: 图像颜色
 */
void sc_draw_Icon(sc_pfb_t *dest, int xs, int ys, int w, int h, const uint8_t *src, uint8_t bpp, color_t fc, uint16_t alpha)
{
    if (src == NULL)
        return;
    sc_pfb_t tpfb; // 临时pfb
    sc_dirty_t intersection;
    sc_rect_t rect = {xs, ys, w, h};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return;
    do
    { //===========计算相交===============
        uint16_t *dest_buf = dest->buf + (intersection.ys - dest->y) * dest->w - dest->x+ intersection.xs;
        for (int y = intersection.ys; y < intersection.ye; y++, dest_buf += dest->w)
        {
            int src_offs = (y - ys) * w - xs + intersection.xs;
            uint16_t *dest_out = dest_buf ;
            for (int x = intersection.xs; x < intersection.xe; x++,src_offs++)
            {
                uint8_t raw = (bpp == 8) ? src[src_offs] : get_bpp_value(src_offs, src, bpp);
                *dest_out =  alphaBlend(fc, *dest_out, raw );
                dest_out++;
            }
        }

    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
}

/*
 * 显示图片16位 spi_addr
 * dest:  目标pfb
 * xs,ys: 目标位置
 * src: 图像数据
 * spi_addr: 图像数据在spi flash中的地址
 */
void sc_draw_Image(sc_pfb_t *dest, int xs, int ys, const sc_image_t *src, uint32_t spi_addr)
{
    sc_pfb_t tpfb; // 临时pfb
    sc_dirty_t intersection;
    sc_rect_t rect = {xs, ys, src->w, src->h};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return;
    //===========计算相交===============
    do
    {
        uint32_t len = intersection.xe - intersection.xs;
        uint16_t *dest_buf = dest->buf + (intersection.ys - dest->y) * dest->w - dest->x + intersection.xs;
        //uint8_t alpha = gui->alpha;
        for (int y = intersection.ys; y < intersection.ye; y++, dest_buf += dest->w)
        {
            int src_offs = (y - ys) * src->w - xs + intersection.xs;
            if (src->map == NULL)
            {
                spiflash_read((uint8_t *)dest_buf, spi_addr + src_offs * 2, len * 2);
                continue;
            }
            //----------源图copy----------
            uint16_t *src_dat = (uint16_t *)src->map + src_offs;
            uint8_t *mask = (uint8_t *)src->mask + src_offs;
            uint16_t *dest_out = dest_buf;
            for (int x = intersection.xs; x < intersection.xe; x++)
            {
                if (src->mask)
                {
                    *dest_out = alphaBlend(*src_dat, *dest_out, *mask );
                    mask++;
                }
                else
                {
                    *dest_out = *src_dat;
                }
                dest_out++;
                src_dat++;
            }
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
}


static inline void zip_map_copy(uint8_t *dest, const uint8_t *src)
{
    *((uint32_t*)dest)=*((uint32_t*)src);
}

//压缩图片绘制支持外部spi flash
void sc_drwa_Image_zip(sc_pfb_t *dest, int xs, int ys, const SC_img_zip *zip, sc_dec_zip *dec,uint32_t spi_addr)
{
    sc_pfb_t tpfb; // 临时pfb
    sc_dirty_t intersection;
    sc_rect_t rect = {xs, ys, zip->w, zip->h};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
    {
        dec->n = 0;
        return;
    }
    if (dest->y == dest->y_st || dec->n == 0) // 首次相交初始化
    {
        dec->x = xs;
        dec->y = ys;
        dec->rep_cnt = 0;
        dec->unzip = 0;
        dec->n = 0;
    }
    int xe = xs + zip->w;
    uint16_t r, g, b;
    uint8_t zip_dat[4];
    do
    {
        uint16_t *dest_buf = dest->buf + (dec->y - dest->y) * dest->w;
        while (dec->y < intersection.ye)
        {
            if (dec->rep_cnt == 0)
            {
                dec->rep_cnt = 1;
                if(zip->map)
                {
                    zip_map_copy(zip_dat,&zip->map [dec->n]);     //4字节读取
                }
                else
                {
                    spiflash_read(zip_dat, spi_addr + dec->n, 4);  //flash 4字节读取
                }
                if (zip_dat[0] & 0x20)
                {
                    uint16_t dat16 = zip_dat[0] | (zip_dat[1] << 8);
                    if (dec->unzip == dat16)
                    {
                        dec->n += 2;
                        dec->rep_cnt = (zip_dat[2] << 8) | zip_dat[3]; // 重复的长度
                    }
                    else
                    {
                        dec->unzip = dat16;
                        dec->out = dat16;
                    }
                    dec->n += 2;
                }
                else // 差值编码rgb232
                {
                    b = zip_dat[0];
                    r = (b << 5) & 0x1800;
                    g = (b << 3) & 0x00e3;
                    dec->out = dec->unzip ^ (r + g + (b & 0x03)); // 差值还原
                    dec->n++;
                }
            }
            while (dec->rep_cnt)
            {
                dec->rep_cnt--;
                if (dec->y >= intersection.ys && dec->x >= intersection.xs && dec->x < intersection.xe && dec->y < intersection.ye)
                {
                    uint16_t *dest_out = dest_buf + (dec->x - dest->x);
                    *dest_out = dec->out;
                }
                if (++dec->x >= xe)
                {
                    dec->x = xs;
                    dest_buf += dest->w;
                    if (++dec->y >= intersection.ye)
                    {
                        break;
                    }
                }
            }
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
}
///=====================快速反锯齿画线=====================
#define set_pixel_value(dest, offs, a, c) dest->buf[offs] = alphaBlend(c, dest->buf[offs], a);
void sc_draw_Line_AA(sc_pfb_t *dest, int x1, int y1, int x2, int y2, color_t colour)
{
    if (dest == NULL)
    {
        _sc_draw_Line(x1, y1, x2, y2, colour);
        return;
    }
    sc_dirty_t intersection;
    int16_t xs = SC_MIN(x1, x2);
    int16_t ys = SC_MIN(y1, y2);
    int16_t dx = x2 - x1;
    int16_t dy = y2 - y1;
    int16_t dxabs = SC_ABS(dx); // 宽度
    int16_t dyabs = SC_ABS(dy); // 高度
    sc_rect_t rect = {xs, ys, dxabs, dyabs};
    if (!sc_pfb_intersection(dest, &rect, &intersection)) // 判断是否在矩形内
    {
        return;
    }
    int16_t sgndx = (dx < 0) ? -1 : 1;                 // 确保x方向正确
    int16_t sgndy = (dy < 0) ? -1 : 1;                 // 确保y方向正确
    int16_t majorStep = dxabs > dyabs ? dxabs : dyabs; // 主方向步长
    int16_t minorStep = dxabs > dyabs ? dyabs : dxabs; // 次方向步长
    int16_t error = majorStep / 2;                     // 初始化误差项
    if (dxabs > dyabs)                                 // 横向线条的插值
    {
        error = minorStep * (intersection.xs - xs) + majorStep / 4; // 0.25
        for (x1 = intersection.xs; x1 < intersection.xe; x1++)
        {
            uint16_t a = (error << 8) / majorStep & 0xff; // 插值计算;
            if ((sgndy * sgndx) > 0)
            {
                y1 = error / majorStep + ys;
            }
            else
            {
                y1 = -error / majorStep + ys + dyabs;
                a = 255 - a; // 插值计算
            }
            int dest_offs = (y1 - dest->y) * dest->w - dest->x + x1;
            if (y1 >= intersection.ys && y1 < intersection.ye)
            {
                set_pixel_value(dest, dest_offs, 255, colour);
            }
            if (y1 + 1 >= intersection.ys && y1 + 1 < intersection.ye)
            {
                set_pixel_value(dest, dest_offs + dest->w, a, colour);
            }
            if (y1 - 1 >= intersection.ys && y1 - 1 < intersection.ye)
            {
                set_pixel_value(dest, dest_offs - dest->w, 255 - a, colour);
            }
            error += minorStep;
        }
    }
    else
    {
        error += minorStep * (intersection.ys - ys) + majorStep / 4; // 0.25
        for (y1 = intersection.ys; y1 < intersection.ye; y1++)
        {
            uint16_t a = (error << 8) / majorStep & 0xff; // 插值计算;
            if ((sgndy * sgndx) > 0)
            {
                x1 = error / majorStep + xs;
            }
            else
            {
                x1 = -error / majorStep + xs + dxabs;
                a = 255 - a;
            }
            int dest_offs = (y1 - dest->y) * dest->w - dest->x + x1;
            if (x1 >= intersection.xs && x1 < intersection.xe)
            {
                set_pixel_value(dest, dest_offs, 255, colour);
            }
            if (x1 + 1 >= intersection.xs && x1 + 1 < intersection.xe)
            {
                set_pixel_value(dest, dest_offs + 1, a, colour);
            }
            if (x1 - 1 >= intersection.xs && x1 - 1 < intersection.xe)
            {
                set_pixel_value(dest, dest_offs - 1, 255 - a, colour);
            }
            error += minorStep;
        }
    }
}
