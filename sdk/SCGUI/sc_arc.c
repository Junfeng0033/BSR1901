
#include "sc_gui.h"

// 0-45度映射表
static const uint8_t angle_0_45_to_alpha_base[] = {
    0, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7,
    7, 8, 9, 9, 10, 11, 12, 12, 13, 14,
    14, 15, 16, 16, 17, 18, 18, 19, 20, 20,
    21, 22, 22, 23, 24, 24, 25, 26, 26, 27,
    28, 28, 29, 30, 32};
static const uint8_t zone_map[8] = {2, 3, 1, 0, 5, 4, 6, 7};
// 渐变alpha值 0~255 对应0~360度
static uint8_t sc_atan2_alpha(int16_t dx, int16_t dy)
{
    if (dy == 0)
        return (dx < 0) ? 63 : 191; // 90,270度特殊处理
    if (dx == 0)
        return (dy > 0) ? 0 : 127;                                            // 0,180度特殊处理
    uint16_t u_dx = SC_ABS(dx);                                               // 取绝对值
    uint16_t u_dy = SC_ABS(dy);                                               // 取绝对值
    uint8_t is_near_y = (u_dx > u_dy) ? 0 : 1;                                // 判断接近Y轴
    uint8_t theta_45 = is_near_y ? u_dx * 45 / u_dy : u_dy * 45 / u_dx;       // 计算角度
    uint8_t alpha_base = angle_0_45_to_alpha_base[theta_45];                  // 查表获取alpha_base
    uint8_t zone = zone_map[((dx >= 0) << 2) | ((dy >= 0) << 1) | is_near_y]; // 计算象限
    switch (zone)
    {
    case 0:
        return alpha_base;
    case 1:
        return 63 - alpha_base;
    case 2:
        return 64 + alpha_base;
    case 3:
        return 127 - alpha_base;
    case 4:
        return 128 + alpha_base;
    case 5:
        return 191 - alpha_base;
    case 6:
        return 192 + alpha_base;
    case 7:
        return 255 - alpha_base;
    }
    return 0;
}
//-------圆头端点------------------------
typedef struct
{
    int16_t cx;
    int16_t cy;
    int16_t r;
    int16_t r2;
    uint16_t rmax;
    uint16_t outer;
} sc_arc_dot;

// 圆头端点计算
static inline uint8_t sc_get_dot(sc_arc_dot *dot, int ax, int ay)
{
    uint16_t temp;
    uint8_t k, alpha, max = 0;
    for (k = 0; k < 2; k++)
    {
        sc_arc_dot *p = &dot[k];
        int x = ax > p->cx ? ax - p->cx : p->cx - ax;
        int y = ay > p->cy ? ay - p->cy : p->cy - ay;
        if (x < p->r && y < p->r)
        {
            temp = x * x + y * y;
            if (temp >= p->rmax)
            {
                alpha = 0;
            }
            else if (temp > p->r2)
            {
                if (dot->outer == 0)
                {
                    dot->outer = (0xff00) / (dot->rmax - dot->r2); // 除法优化
                }
                alpha = (p->rmax - temp) * dot->outer >> 8; // 外边缘抗锯齿
            }
            else
            {
                alpha = 255;
            }
            max = max > alpha ? max : alpha; // 重合取最大的值
        }
    }
    return max;
}

// 圆头端点坐标运算无浮点会有一定误差
static inline void sc_dot_sin_cos(int cx, int cy, int cr, int cir, int sin, int cos, sc_arc_dot *dot)
{
    int len = (cr + cir) >> 1;
    int r = (cr - cir) >> 1;
    // 圆心坐标4舍5入+0.5
    int dot_cx = sin * len + (1 << 14);
    int dot_cy = cos * len + (1 << 14);
    dot->cx = cx - (dot_cx >> 15);
    dot->cy = cy - (dot_cy >> 15);
    dot->r = r + 1;
    dot->r2 = r * r;
    dot->rmax = (r + 1) * (r + 1);
    dot->outer = 0;
}
// 判断正负符号 ax==x  ay==y  同符号为真
// #define CHECK_XY(x, y,ax, ay)   ((ax*x) < 0 || (ay*y) < 0)
static inline bool CHECK_XY(int x, int y, int ax, int ay)
{
    return ((x != 0 && ((ax ^ x) < 0)) ||
            (y != 0 && ((ay ^ y) < 0)));
}

typedef uint8_t (*atan2_cb)(int16_t dx, int16_t dy);

// 圆弧绘制函数
bool _sc_DrawRound(sc_pfb_t *dest, int16_t cx, int16_t cy, uint16_t r, uint16_t ir, int16_t start_deg, int16_t end_deg,
                   uint16_t ac, uint16_t bc, uint16_t alpha, sc_arc_dot *dot)
{
    sc_pfb_t tpfb; // 2. 临时帧缓冲初始化
    sc_dirty_t intersection;
    sc_rect_t rect = {cx - r, cy - r, r * 2 + 1, r * 2 + 1};
    if (dest == NULL)
    {
        sc_pfb_init_slices(&tpfb, &rect, gui->bkc);
        dest = &tpfb;
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return false;
    // 3. 预计算半径抗锯齿常量
    const uint16_t r1 = ir * ir;
    const uint16_t r2 = r * r;
    const uint16_t rmax = (r + 1) * (r + 1);
    const uint16_t rmin = ir > 0 ? (ir - 1) * (ir - 1) : 0;
    const uint16_t inv_outer = rmax - r2 ? ((alpha << 8) / (rmax - r2)) : 0;
    const uint16_t inv_inner = r1 - rmin ? ((alpha << 8) / (r1 - rmin)) : 0;
    int16_t sx, sy, ex, ey; // 半径大于255要用int32_t
    uint8_t big_arc = 0xff; //-1满圆，0小弧，1大弧
    if ((start_deg != 0 || end_deg != 360))
    {
        /* ---------- 角度转Q16查表定点向量 ---------- */
        if (end_deg >= start_deg)
        {
            big_arc = (end_deg - start_deg > 180) ? 1 : 0;
        }
        else
        {
            big_arc = (360 - start_deg + end_deg > 180) ? 1 : 0;
        }
        sx = sc_sin(start_deg);
        sy = -sc_cos(start_deg);
        ex = sc_sin(end_deg);
        ey = -sc_cos(end_deg);
        if (dot)
        {
            sc_dot_sin_cos(cx, cy, r, ir, sx, sy, &dot[0]);
            sc_dot_sin_cos(cx, cy, r, ir, ex, ey, &dot[1]);
        }
    }
    do
    {
        // 6. 循环绘制计算一次dest_offs
        atan2_cb Blend_cb = (ac != bc) ? sc_atan2_alpha : NULL; // 是否需渐变混合
        int32_t ds, de, sd, ed, dy_sx, dy_ex;                   // 起始角度和结束角
        int16_t x, y, dx, dy, Ysq, Xsq, Rsq;
        int16_t dist, min_dist;
        uint16_t *dest_buf = dest->buf + (intersection.ys - dest->y) * dest->w - dest->x;
        for (y = intersection.ys; y < intersection.ye; y++, dest_buf += dest->w)
        {
            dy = (y - cy); // 相对圆心Y偏移
            Ysq = dy * dy;
            if (big_arc != 0xff)
            {
                dy_sx = dy * sx;
                dy_ex = dy * ex;
            }
            for (x = intersection.xs; x < intersection.xe; x++)
            {
                dx = (x - cx); // 相对圆心X偏移
                Xsq = dx * dx;
                Rsq = Xsq + Ysq;
                if (Rsq >= rmax) // 大于 rmax 外圆跳过
                {
                    if (x > cx)
                        break;
                }
                else if (Rsq < rmin) // 小于于 rmin 内圆跳过
                {
                    if (x < cx)
                        x = cx * 2 - x; // 镜向
                }
                else // 在圆内
                {
                    if (Rsq > r2) // 外半径边缘
                    {
                        dist = ((rmax - Rsq) * inv_outer) >> 8;
                    }
                    else if (Rsq < r1) // 内半径边缘
                    {
                        dist = ((Rsq - rmin) * inv_inner) >> 8;
                    }
                    else
                    {
                        dist = alpha;
                    }
                    if (big_arc != 0xff)
                    {
                        ds = (dx * sy - dy_sx); // 到起始点的方向
                        de = (dy_ex - dx * ey); // 到结束点的方向
                        bool in_range = big_arc > 0 ? (ds > 0 || de > 0) : (ds >= 0 && de >= 0);
                        if (!in_range) // 弧外
                        {
                            if (dot == NULL)
                            {
                                sd = CHECK_XY(dx, dy, sx, sy) ? SC_ABS(ds) : SC_Q15;
                                ed = CHECK_XY(dx, dy, ex, ey) ? SC_ABS(de) : SC_Q15;
                                min_dist = SC_MIN(sd, ed) >> 7;
                                if (min_dist >= 255)
                                {
                                    continue; // 跳过
                                }
                                min_dist = (255 - min_dist) * alpha >> 8;
                                dist = SC_MIN(min_dist, dist); // 1px插值取最小
                            }
                            else
                            {
                                min_dist = sc_get_dot(dot, x, y);
                                if (min_dist == 0)
                                {
                                    continue; // 跳过
                                }
                                min_dist = (min_dist)*alpha >> 8;
                                dist = SC_MIN(min_dist, dist); // 1px插值取最小
                            }
                        }
                    }
                    uint16_t *dest_out = &dest_buf[x]; // 实时计算偏移
                    if (Blend_cb)
                    {
                        uint16_t deg = Blend_cb(dx, dy);
                        deg += (deg < 250) ? (Rsq & 0x03) : (0); // 随机抖动
                        *dest_out = alphaBlend(alphaBlend(bc, ac, deg), *dest_out, dist);
                    }
                    else
                    {
                        *dest_out = alphaBlend(ac, *dest_out, dist);
                    }
                }
            }
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
    return true;
}
/*画圆弧进度条支持渐变
 * dest: 目标缓存区
 * p:圆弧参数
 * start_deg:起始角度
 * end_deg:结束角度
 * stcolor:起始颜色
 * endcolor:结束颜色
 * alpha:透明度
 */
void sc_draw_Arc(sc_pfb_t *dest, const sc_arc_t *p, int start_deg, int end_deg, uint16_t stcolor, uint16_t endcolor, uint16_t alpha)
{
    sc_arc_dot arc_dot[2]; // 端点
    if (p->dot)
    {
        _sc_DrawRound(dest, p->cx, p->cy, p->r, p->ir, start_deg, end_deg, stcolor, endcolor, alpha, arc_dot);
    }
    else
    {
        _sc_DrawRound(dest, p->cx, p->cy, p->r, p->ir, start_deg, end_deg, stcolor, endcolor, alpha, NULL);
    }
}

// 组合绘制函数-圆弧
void sc_draw_compose_arc(sc_pfb_t *dest, sc_arc_t *arc, int start_deg, int end_deg,
                         color_t stcolor, color_t endcolor, color_t bc)
{
    sc_dirty_t intersection;
    sc_pfb_t tpfb;
    int16_t cx = arc->cx;
    int16_t cy = arc->cy;
    int16_t r = arc->r;
    sc_rect_t arc_rect = {cx - r, cy - r, r * 2 + 1, r * 2 + 1};
    if (dest == NULL)
    {
        dest = &tpfb;
        sc_pfb_init_slices(dest, &arc_rect, gui->bkc);
    }
    if (!sc_pfb_intersection(dest, &arc_rect, &intersection))
        return;
    do
    {
        // 组合图形
        sc_draw_Arc(dest, arc, 0, 360, bc, bc, 255);                        // 底圆
        sc_draw_Arc(dest, arc, start_deg, end_deg, stcolor, endcolor, 255); // 角度
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &arc_rect, &intersection));
}

#if 0
sc_arc_t arc={100,100,80,60,1 };
sc_draw_compose_arc(NULL,&arc,60, 300, C_YELLOW, C_RED,C_GREEN);
#endif

// 闭眼睛绘制函数-抛物线
int sc_draw_Eye_blink(sc_pfb_t *dest, int x1, int y1, int w, int h, int dir, uint16_t color)
{
    if (h <= 0)
        return 0;
    sc_dirty_t intersection;
    sc_rect_t rect = {x1, y1, w, h};
    sc_pfb_t tpfb; // 临时画布
    if (dest == NULL)
    {
        sc_pfb_init_slices(&tpfb, &rect, gui->bkc);
        dest = &tpfb;
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return 0;
    const int ye = y1 + h - 1;
    const int cx = x1 + w / 2;                              // X中心（左右对称）
    const uint32_t W_half_sq = (uint32_t)(w / 2) * (w / 2); // (W/2)² 预计算（仅此处除法，为保留K系数逻辑）
    const int abs_vtx_y = h;                                // 顶点高度绝对值（控制开口大小）
    const uint32_t K = W_half_sq / abs_vtx_y;
    const uint16_t inv_p = (K != 0) ? (0xFF00U / K) : 0; // 仅此处初始化用除法，抗锯齿计算无除法
    do
    {
        int32_t dest_offs = (int32_t)(intersection.ys - dest->y) * dest->w - dest->x + intersection.xs;
        for (int16_t y = intersection.ys; y < intersection.ye; y++, dest_offs += dest->w)
        {
            uint8_t alpha = 255;
            uint32_t thresh = 0;
            uint32_t dx2_max = 0;
            int16_t dy = (dir > 0) ? y - y1 : ye - y; // 开口方向判断
            if (dy < 0)
                continue;           // 超出底边界，跳过
            thresh = K * dy;        // 乘法，无除法
            dx2_max = K * (dy + 1); // 加法，无除法
            uint16_t *buf_ptr = &dest->buf[dest_offs];
            for (int16_t x = intersection.xs; x < intersection.xe; x++, buf_ptr++)
            {
                int16_t dx = x - cx;
                uint32_t dx2 = dx * dx; // 乘法，无除法
                // ========== 仅绘制外边缘抗锯齿区 ==========
                if (dx2 >= dx2_max) // 判断是否为抛物线外部
                {
                    continue; // 跳过
                }
                if (dx2 < thresh) // 判断是否为抛物线内部
                {
                    *buf_ptr = alphaBlend(color, *buf_ptr, 255);
                }
                else
                { // 插值1px
                    alpha = (((dx2 - thresh) * inv_p) >> 8);
                    alpha = 255 - alpha;
                    *buf_ptr = alphaBlend(color, *buf_ptr, alpha);
                }
            }
        }
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
    return 1;
}

void DrawEye_Blink_test(sc_pfb_t *dest, int x, int y, int w, int h, int up, int dowm, color_t color)
{
    sc_dirty_t intersection;
    sc_rect_t rect = {x, y, w, h};
    sc_pfb_t tpfb; // 临时画布
    if (dest == NULL)
    {
        sc_pfb_init_slices(&tpfb, &rect, 0);
        dest = &tpfb;
    }
    if (!sc_pfb_intersection(dest, &rect, &intersection))
        return;
    //int cx = x + w / 2; // X中心（左右对称）
    int cy = y + h / 2; // Y中心（上下对称）
    do
    {
        sc_draw_Eye_blink(dest, x, cy - up, w, up, 1, color);
        sc_draw_Eye_blink(dest, x, cy, w, dowm, -1, color);
    } while ((dest == &tpfb) && sc_pfb_next_slice(dest) && sc_pfb_intersection(dest, &rect, &intersection));
    dest->mask = NULL;
}
