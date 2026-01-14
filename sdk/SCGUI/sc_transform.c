
#include "sc_gui.h"

// 回调函数指针
typedef void (*Bilinear_cb)(Transform_t *p, int32_t x, int32_t y, const uint8_t fx, const uint8_t fy, uint16_t *dst);
// 图片插值算法
static inline void _Bilinear_Image(Transform_t *p, int32_t x, int32_t y, const uint8_t fx, const uint8_t fy, uint16_t *dst)
{
    sc_image_t *image = (sc_image_t *)p->src;
    uint16_t *map = (uint16_t *)image->map;
    uint8_t *mask = (uint8_t *)image->mask;

    int32_t offs = y * image->w + x;
    const int16_t xe = image->w - 2;
    const int16_t ye = image->h - 2;
    uint16_t p00 = (x < 0 || y < 0) ? *dst : map[offs];
    uint16_t p01 = (x > xe || y < 0) ? *dst : map[offs + 1];
    uint16_t p10 = (y > ye || x < 0) ? *dst : map[offs + image->w];
    uint16_t p11 = (y > ye || x > xe) ? *dst : map[offs + image->w + 1];
    if (mask)
    {
        p00 = (x < 0 || y < 0) ? *dst : alphaBlend(p00, *dst, mask[offs]);                  // 左上角像素
        p01 = (x > xe || y < 0) ? *dst : alphaBlend(p01, *dst, mask[offs + 1]);             // 右上角像素
        p10 = (y > ye || x < 0) ? *dst : alphaBlend(p10, *dst, mask[offs + image->w]);      // 左下角像素
        p11 = (y > ye || x > xe) ? *dst : alphaBlend(p11, *dst, mask[offs + image->w + 1]); // 右下角像素
    }
    if (p->sinA == 0 || p->cosA == 32767)
    {
        *dst = *dst = alphaBlend(p00, p10, 255 - fy); // 在Y方向进行权重插值，返回最终结果;
        return;
    }
    // 在X方向进行权重插值
    uint16_t y0 = alphaBlend(p00, p01, 255 - fx); // 左上和右上的插值
    uint16_t y1 = alphaBlend(p10, p11, 255 - fx); // 左下和右下的插值
    *dst = alphaBlend(y0, y1, 255 - fy);          // 在Y方向进行权重插值，返回最终结果
}

// 字体插值算法
static inline void _Bilinear_font(Transform_t *p, int32_t x, int32_t y, const uint8_t fx, const uint8_t fy, uint16_t *dst)
{
    sc_image_t *image = (sc_image_t *)p->src;
    uint8_t *mask = (uint8_t *)image->mask;

    const uint8_t bpp = image->bpp;
    int32_t offs = y * image->w + x;
    const int16_t xe = image->w - 2;
    const int16_t ye = image->h - 2;
    uint8_t p00 = (x < 0 || y < 0) ? 0 : get_bpp_value(offs, mask, bpp);                  // 左上角像素
    uint8_t p01 = (x > xe || y < 0) ? 0 : get_bpp_value(offs + 1, mask, bpp);             // 右上角像素
    uint8_t p10 = (y > ye || x < 0) ? 0 : get_bpp_value(offs + image->w, mask, bpp);      // 左下角像素
    uint8_t p11 = (y > ye || x > xe) ? 0 : get_bpp_value(offs + image->w + 1, mask, bpp); // 右下角像素
    // 在X方向进行权重插值
    const uint16_t y0 = (uint32_t)p00 * (255U - fx) + (uint32_t)p01 * fx;
    const uint16_t y1 = (uint32_t)p10 * (255U - fx) + (uint32_t)p11 * fx;
    const uint16_t ret = (y0 * (255U - fy) + y1 * fy) >> 16;
    *dst = alphaBlend(gui->fc, *dst, ret);
}


/*初始化源图，默认为源图中心点
merge: 合并后的包围盒
x:平移x
ty:平移y
src:源图
p:结构体指针
*/
void sc_init_trans(int tx, int ty, void *src, Transform_t *p)
{
    sc_image_t *image = (sc_image_t *)src;
    p->center_x = image->w / 2; // 源图中心点
    p->center_y = image->h / 2; // 源图中心点
    p->move_x = tx;             // 平移x
    p->move_y = ty;             // 平移y
    p->src = src;               // 源图
    p->box.x = tx;               // 包围盒初始化
    p->box.y = ty;
    p->box.w = 0;
    p->box.h = 0;
}

/* 设置旋转角度
scaleX：缩放x Q8
scaleY：缩放y Q8
Angle：旋转角度 0~360
p:结构体指针
*/
void sc_set_trans_angle(int16_t q8_scaleX, int16_t q8_scaleY, int16_t Angle, Transform_t *p)
{
    if (p == NULL || p->src == NULL)
        return;
    Angle = (Angle) % 360;
    p->sinA = sc_sin(Angle);
    p->cosA = sc_cos(Angle);
    p->scaleX = q8_scaleX; // X缩放
    p->scaleY = q8_scaleY; // Y缩放
    sc_image_t *image = (sc_image_t *)p->src;
    // 统一矩阵符号：通过sinA符号控制旋转方向
    int32_t m00 = ((int32_t)p->cosA * p->scaleX) >> 8;  // x轴：cos(θ)*Kx（Q15）
    int32_t m01 = ((int32_t)-p->sinA * p->scaleY) >> 8;  // y轴：-sin(θ)*Ky（Q15）
    int32_t m10 = ((int32_t)p->sinA * p->scaleX) >> 8; // x轴： sin(θ)*Kx（Q15）
    int32_t m11 = ((int32_t)p->cosA * p->scaleY) >> 8;  // y轴：cos(θ)*Ky（Q15）
    //-----------旋转后的窗口------------------
    sc_dirty_t temp = {
        .xe = SC_INT16_MIN,
        .ye = SC_INT16_MIN,
        .xs = SC_INT16_MAX,
        .ys = SC_INT16_MAX,
    };
    // 源图4顶点（左包坐标系：0~w, 0~h）
    int32_t v[4][2] = {{0, 0}, {image->w, 0}, {0, image->h}, {image->w, image->h}};
    for (int i = 0; i < 4; i++)
    {
        int32_t dx_rel = v[i][0] - p->center_x;
        int32_t dy_rel = v[i][1] - p->center_y;
        int32_t dx = ((int64_t)dx_rel * m00 + (int64_t)dy_rel * m01) + (1 << 14);
        int32_t dy = ((int64_t)dx_rel * m10 + (int64_t)dy_rel * m11) + (1 << 14);
        // 叠加屏幕中心点平移
        dx = (dx >> 15) + p->move_x;
        dy = (dy >> 15) + p->move_y;
        // 更新包围盒极值
        temp.xs = SC_MIN(temp.xs, dx);
        temp.ys = SC_MIN(temp.ys, dy);
        temp.xe = SC_MAX(temp.xe, dx+1);
        temp.ye = SC_MAX(temp.ye, dy+1);
    }
    sc_rect_t new = sc_dirty_to_rect(&temp); // 计算新的包围盒
    p->drity = sc_rect_merge(&new, &p->box); // 合并包围盒
    p->box = new;
}

/* 旋转缩放绘制
 * dest:目标pfb
 * p:结构体指针
 */
void sc_draw_trans(sc_pfb_t *dest, Transform_t *p)
{
    sc_image_t *image = (sc_image_t *)p->src;
    sc_pfb_t tpfb;           // 临时pfb
    sc_dirty_t intersection; // 交集
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &p->drity, gui->bkc);
    }
    do
    {
        if (sc_pfb_intersection(dest, &p->box, &intersection)) // 判断是否有交集
        {
            p->scaleX = (p->scaleX) ? p->scaleX : 1; // 不能为0
            p->scaleY = (p->scaleY) ? p->scaleY : 1; // 不能为0
            // 反映射缩放系数是除
            int32_t m00 = ((int32_t)p->cosA << 8) / p->scaleX;  // cosθ/Kx（Q15）
            int32_t m01 = ((int32_t)p->sinA << 8) / p->scaleX;  // sinθ/Kx（Q15）
            int32_t m10 = ((int32_t)-p->sinA << 8) / p->scaleY; // -sinθ/Ky（Q15）
            int32_t m11 = ((int32_t)p->cosA << 8) / p->scaleY;  // cosθ/Ky（Q15）
            //----------中心0.5象素偏移-------
            int32_t offx = (image->w & 0x01) ? 0 : (m00 + m01 - 32767) / 2;
            int32_t offy = (image->h & 0x01) ? 0 : (m10 + m11 - 32767) / 2;

            // 目标图像包围盒遍历
            uint16_t *dest_out = dest->buf + (intersection.ys - dest->y) * dest->w - dest->x;
            int32_t Cx_16 = m01 * (intersection.ys - p->move_y) + (p->center_x << 15) + offx; // m01=sinθ/Kx（Q15）
            int32_t Cy_16 = m11 * (intersection.ys - p->move_y) + (p->center_y << 15) + offy; // m11=cosθ/Ky（Q15）
            Bilinear_cb Bilinear_fun = (image->map != NULL) ? _Bilinear_Image : _Bilinear_font;

            for (int y = intersection.ys; y < intersection.ye; y++, dest_out += dest->w)
            {
                int32_t rotatedX = m00 * (intersection.xs - p->move_x) + Cx_16; // 初始x项（仅计算一次乘法）
                int32_t rotatedY = m10 * (intersection.xs - p->move_x) + Cy_16; // 初始y项（仅计算一次乘法）
                bool _end = 0;
                for (int x = intersection.xs; x < intersection.xe; x++)
                {
                    int32_t sx = (rotatedX) >> 15;
                    int32_t sy = (rotatedY) >> 15;
                    // 边界钳位+采样
                    if (sx >= -1 && sx < image->w && sy >= -1 && sy < image->h)
                    {
                        const uint8_t fx = (rotatedX) >> 7;
                        const uint8_t fy = (rotatedY) >> 7;
                        Bilinear_fun(p, sx, sy, fx, fy, &dest_out[x]);
                        _end = 1;
                    }
                    else if (_end)
                    {
                        break;
                    }
                    rotatedX += m00;
                    rotatedY += m10;
                }
                Cx_16 += m01;
                Cy_16 += m11;
            }
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest));
}

/*内部函数lvgl字符转mask用于字符串缩放*/
static void _label_to_mask(int xs, int ys, lv_font_glyph_dsc_t *dsc, uint32_t unicode, lv_font_t *font, uint8_t *mask, int mask_w)
{
    int16_t offs_y = ys + (font->line_height - dsc->box_h - dsc->ofs_y - font->base_line);
    int16_t offs_x = xs + dsc->ofs_x;
    font_dsc_t fdsc = sc_get_font_dsc(font);
    sc_rect_t box = {.x = xs, .y = ys, .w = dsc->adv_w + fdsc.Xspace, .h = fdsc.line_height}; // 字体包围盒
    const uint8_t *src = font->get_glyph_bitmap(font, unicode);
    int16_t x, y, src_x, src_y;
    uint32_t dest_offs = ys * mask_w;
    for (y = box.y; y < box.y + box.h; y++, dest_offs += mask_w)
    {
        src_y = y - offs_y;
        for (x = box.x; x < box.x + box.w; x++)
        {
            src_x = x - offs_x;
            if (src_x >= 0 && src_x < dsc->box_w && src_y >= 0 && src_y < dsc->box_h)
            {
                uint32_t src_offs = src_y * dsc->box_w + src_x;
                copy_bpp_value(mask, dest_offs + x, src, src_offs, dsc->bpp);
            }
        }
    }
}

/* 设置label图像用于缩放
merge: 合并后的包围盒
label: 标签
cx,cy: 中心点
imge:  label生成的图像
p: 参数
*/
void sc_init_trans_label(sc_label_t *label, int cx, int cy, void *imge, Transform_t *p)
{
    if (label == NULL || label->font == NULL)
        return;
    sc_image_t *label_imge = (sc_image_t *)imge;
    // ----------初始化参数----------------
    lv_font_t *font = label->font;
    lv_font_glyph_dsc_t g; // 字体信息
    unicode_t unicode[SC_UINCODE_SIZE];
    uint32_t indx = 0;
    sc_rect_t *parent = label->box;
    uint16_t line_width_max = parent ? parent->w : gui->lcd_rect.w;
    uint16_t width = _get_Label_width(line_width_max, label, &indx, unicode, &g); // 获取label宽度
    size_t size = (width * font->line_height) * g.bpp / 8;                        // 内存大小
    // size_t size_last = (label_imge->w * label_imge->h) * label_imge->bpp / 8;
    void *mask = (void *)label_imge->mask;
    mask = sc_malloc(size);
    if (mask == NULL)
        return;
    sc_memset(mask, 0, size);
    // ----------生成labelmask----------------
    font_dsc_t fdsc = sc_get_font_dsc(font);
    indx = 0;
    for (int k = 0; unicode[k]; k++)
    {
        if (font->get_glyph_dsc(font, &g, unicode[k], 0) == 0)
            continue;
        _label_to_mask(indx, 0, &g, unicode[k], font, mask, width);
        indx += (g.adv_w + fdsc.Xspace);
    }
    label_imge->map = NULL; // 清空map
    label_imge->mask = mask;
    label_imge->w = width;             // 设置宽度
    label_imge->h = font->line_height; // 设置高度
    label_imge->bpp = g.bpp;           // 设置bpp
    sc_init_trans(cx, cy, imge, p);
}

// 标签旋转缩放示列
void sc_demo_transform(void)
{
    extern const sc_image_t tempC_img_48;
    Transform_t p; // 旋转

    // 图像旋转缩放
    sc_init_trans(100, 200, (void *)&tempC_img_48, &p);
    sc_set_trans_angle(300, 300, 90, &p);
    sc_draw_trans(NULL, &p);

    Transform_t p1; // 旋转

    sc_label_t label;
    sc_image_t label_image;
    sc_init_Label(NULL, &label, gui->font, "SCGUI", gui->fc, gui->bkc, 0);

    sc_init_trans_label(&label, 150, 20, &label_image, &p1);

    sc_set_trans_angle(256, 256, 10, &p1);
    sc_draw_trans(NULL, &p1);
}
