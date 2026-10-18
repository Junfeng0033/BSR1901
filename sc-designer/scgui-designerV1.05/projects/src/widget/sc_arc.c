#include "../sc_draw.h"
#include "../sc_gui.h"
#include "sc_arc.h"

/* 0-45 degree mapping table */
static const uint8_t angle_0_45_to_alpha_base[] = {
    0, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 9, 10,
    11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21,
    22, 22, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 32};
static const uint8_t zone_map[8] = {2, 3, 1, 0, 5, 4, 6, 7};

uint8_t sc_atan2_alpha(int16_t dx, int16_t dy)
{
    uint32_t u_dx;
    uint32_t u_dy;
    uint8_t is_near_y;
    uint8_t theta_45;
    uint8_t alpha_base;
    uint8_t zone;
    static const int8_t op_type[8] = {0, 1, 0, 1, 0, 1, 0, 1};
    static const uint8_t base_val[8] = {0, 63, 64, 127, 128, 191, 192, 255};
    if (dy == 0)
    {
        return (dx < 0) ? 63 : 191;
    }
    if (dx == 0)
    {
        return (dy > 0) ? 0 : 127;
    }
    u_dx = (uint32_t)(dx < 0 ? -dx : dx);
    u_dy = (uint32_t)(dy < 0 ? -dy : dy);
    is_near_y = (u_dx > u_dy) ? 0 : 1;
    theta_45 = is_near_y ? (uint8_t)(u_dx * 45U / u_dy) : (uint8_t)(u_dy * 45U / u_dx);
    alpha_base = angle_0_45_to_alpha_base[theta_45];
    zone = zone_map[((dx >= 0) << 2) | ((dy >= 0) << 1) | is_near_y];
    if (op_type[zone])
    {
        return (uint8_t)(base_val[zone] - alpha_base);
    }
    return (uint8_t)(base_val[zone] + alpha_base);
}

/* Shared angular gradient (255-space). Non-inline to avoid duplicating atan2 in both draw paths. */
typedef struct sc_arc_grad_t
{
    uint8_t a0;
    uint8_t span;
    uint8_t mid; /* nearer-end split for out-of-sector clamp */
    uint16_t scale;
} sc_arc_grad_t;

static void sc_arc_grad_setup(sc_arc_grad_t *g, int s, int e)
{
    uint8_t a1;
    /* Caller keeps s/e in [0, 360). */
    g->a0 = (uint8_t)(((uint32_t)s * 256U) / 360U);
    a1 = (uint8_t)(((uint32_t)e * 256U) / 360U);
    /* uint8 (a1 - a0) wraps across 0°, e.g. 330°->60°. */
    g->span = (uint8_t)(a1 - g->a0);
    if (g->span == 0U)
    {
        g->span = 255U;
    }
    g->mid = (uint8_t)(g->span + ((255U - g->span) >> 1));
    g->scale = (uint16_t)((255U << 8) / g->span);
}

static uint16_t sc_arc_grad_color(uint16_t st_c, uint16_t end_c,
                                  int32_t dx, int32_t dy, int32_t rsq,
                                  const sc_arc_grad_t *g)
{
    uint8_t rel = (uint8_t)(sc_atan2_alpha((int16_t)dx, (int16_t)dy) - g->a0);
    uint8_t t;
    if (rel > g->span)
    {
        rel = (rel > g->mid) ? 0U : g->span;
    }
    t = (uint8_t)((rel * g->scale) >> 8);
    if (t < 248U)
    {
        t = (uint8_t)(t + ((rsq + t) & 0x07));
    }
    return alphaBlend(end_c, st_c, t);
}

static inline uint8_t sc_get_big_flag(int s, int e)
{
    int diff = (e < s) ? e + 360 - s : e - s;
    if (diff == 360)
    {
        return 0;
    }
    return (diff > 180) ? 2 : 1;
}

/* ---- Cap setup only (outside pixel loop; hot path keeps formulas in place) ---- */

static void sc_arc_dot_setup(int cx, int cy, int r, int ir, uint8_t alpha,
                             int32_t sx, int32_t sy, int32_t ex, int32_t ey,
                             int32_t *csx, int32_t *csy, int32_t *cex, int32_t *cey,
                             int32_t *cap_r2, int32_t *cap_rmax2, int32_t *cap_inv)
{
    int32_t mid_r = r + ir;
    *csx = (cx << 1) + ((sx * mid_r + 16384) >> 15);
    *csy = (cy << 1) + ((sy * mid_r + 16384) >> 15);
    *cex = (cx << 1) + ((ex * mid_r + 16384) >> 15);
    *cey = (cy << 1) + ((ey * mid_r + 16384) >> 15);
    *cap_r2 = SC_POW2(r - ir - 2);
    *cap_rmax2 = SC_POW2(r - ir + 1);
    *cap_inv = (int32_t)SC_INV(*cap_rmax2, *cap_r2, alpha << 8);
}

static inline void sc_arc_dot_pt(int cx, int cy, int32_t dx, int32_t dy, int mid_r,
                                 int32_t *ox, int32_t *oy)
{
    *ox = (cx << 1) + ((dx * mid_r + 16384) >> 15);
    *oy = (cy << 1) + ((dy * mid_r + 16384) >> 15);
}

static inline void sc_arc_flat_zones(int32_t sx, int32_t sy, int32_t ex, int32_t ey,
                                    int32_t *zs, int32_t *ze)
{
    *zs = (sx > 0) | ((sy > 0) << 1);
    *ze = (ex > 0) | ((ey > 0) << 1);
}

static void sc_draw_arc_props(sc_pfb_t *dest, const sc_arc_t *props, rect_t view)
{
    area_t inter = rect_to_area(view);
    if (!sc_pfb_intersection(dest, &inter, NULL))
        return;
    const int16_t start_deg = props->start_deg;
    const int16_t end_deg = props->end_deg;
    const uint16_t st_c = props->start_c;
    const uint16_t end_c = props->end_c;
    const uint16_t fill = props->fill;
    const uint16_t alpha = props->opa;
    const uint8_t dot = props->dot;
    const int r = SC_MIN(view.w, view.h) / 2;
    const int ir = props->stroke_width > r ? 0 : r - props->stroke_width;
    const int cx = view.x + r;
    const int cy = view.y + r;
    const uint32_t rmax2 = (uint32_t)SC_POW2(r);
    const uint32_t r2 = (uint32_t)SC_POW2(r - 1);
    const uint32_t ir2 = (uint32_t)SC_POW2(ir) + props->stroke_width;
    const uint32_t rmin2 = ir < 0 ? 0U : (uint32_t)SC_POW2(ir - 1);
    const uint32_t inv_outer = SC_INV(rmax2, r2, alpha << 8);
    const uint32_t inv_inner = SC_INV(ir2, rmin2, alpha << 8);
    const uint8_t big_total = (start_deg == end_deg) ? 0U : sc_get_big_flag(start_deg, end_deg);
    const bool need_fill = (fill != g_gui->bg);
    const bool use_grad = (st_c != end_c);
    sc_arc_grad_t grad;
    if (use_grad)
    {
        sc_arc_grad_setup(&grad, start_deg, end_deg);
    }
    int32_t csx_q1 = 0, csy_q1 = 0, cex_q1 = 0, cey_q1 = 0;
    int32_t cap_r2 = 0, cap_rmax2 = 0, cap_inv = 0;
    int32_t sx = 0, ex = 0, sy = 0, ey = 0;
    if (big_total)
    {
        sx = -sc_sin((int16_t)start_deg);
        sy = sc_cos((int16_t)start_deg);
        ex = -sc_sin((int16_t)end_deg);
        ey = sc_cos((int16_t)end_deg);
        if (dot)
        {
            sc_arc_dot_setup(cx, cy, r, ir, (uint8_t)alpha, sx, sy, ex, ey,
                             &csx_q1, &csy_q1, &cex_q1, &cey_q1,
                             &cap_r2, &cap_rmax2, &cap_inv);
        }
        else
        {
            sc_arc_flat_zones(sx, sy, ex, ey, &csx_q1, &cex_q1);
        }
    }
    for (int y = inter.ys; y < inter.ye; y++)
    {
        int32_t dy = y - cy;
        int32_t ysq = dy * dy;
        uint16_t *row = sc_pfb_pixel_ptr(dest, inter.xs, y);
        int x;
        for (x = inter.xs; x < inter.xe; x++)
        {
            int32_t dx = x - cx;
            int32_t rsq = dx * dx + ysq;
            if (rsq > (int32_t)rmax2)
            {
                if (x > cx)
                {
                    break;
                }
            }
            else if (rsq <= (int32_t)rmin2)
            {
                if (x < cx)
                {
                    x = (cx * 2 - x);
                }
            }
            else
            {
                uint8_t bc_a = alpha;
                uint8_t fc_a;
                uint16_t *buf;
                if (rsq > (int32_t)r2)
                {
                    bc_a = (uint8_t)(((rmax2 - (uint32_t)rsq) * inv_outer) >> 8);
                }
                else if (rsq < (int32_t)ir2)
                {
                    bc_a = (uint8_t)((((uint32_t)rsq - rmin2) * inv_inner) >> 8);
                }
                fc_a = bc_a;
                if (big_total)
                {
                    int32_t cur_ds = dy * sx - dx * sy;
                    int32_t cur_de = dx * ey - dy * ex;
                    bool in_total = (big_total == 2) ? (cur_ds >= 0 || cur_de >= 0)
                                                     : (cur_ds >= 0 && cur_de >= 0);
                    if (!in_total)
                    {
                        fc_a = 0;
                        if (dot)
                        {
                            int32_t x_q1 = (cx + dx) << 1;
                            int32_t y_q1 = y << 1;
                            int32_t d2_s = SC_POW2(x_q1 - csx_q1) + SC_POW2(y_q1 - csy_q1);
                            int32_t d2_e = SC_POW2(x_q1 - cex_q1) + SC_POW2(y_q1 - cey_q1);
                            int32_t d2_min = (d2_s < d2_e) ? d2_s : d2_e;
                            if (d2_min < cap_rmax2)
                            {
                                fc_a = bc_a;
                                if (d2_min > cap_r2)
                                {
                                    fc_a = (uint8_t)(((cap_rmax2 - d2_min) * cap_inv) >> 8);
                                    if (!need_fill)
                                    {
                                        bc_a = fc_a;
                                    }
                                }
                            }
                        }
                        else
                        {
                            uint8_t zone = (uint8_t)(((dx > 0)) | ((dy > 0) << 1));
                            uint32_t sd = (zone == csx_q1) ? (uint32_t)SC_ABS(cur_ds) : 32767U;
                            uint32_t ed = (zone == cex_q1) ? (uint32_t)SC_ABS(cur_de) : 32767U;
                            int32_t abs_min = (int32_t)(sd < ed ? sd : ed);
                            if (abs_min < 32767)
                            {
                                uint8_t a = (uint8_t)(((255 - (abs_min >> 7)) * alpha) >> 8);
                                fc_a = SC_MIN(bc_a, a);
                            }
                        }
                    }
                }
                buf = row + (x - inter.xs);
                if (fc_a > 0)
                {
                    uint16_t color = st_c;
                    if (use_grad)
                    {
                        color = sc_arc_grad_color(st_c, end_c, dx, dy, rsq, &grad);
                    }
                    if (fc_a < 250)
                    {
                        color = alphaBlend(color, fill, fc_a);
                        color = alphaBlend(color, *buf, bc_a);
                    }
                    *buf = color;
                }
                else if (need_fill)
                {
                    *buf = alphaBlend(fill, *buf, bc_a);
                }
            }
        }
    }
}

static inline bool sc_is_progress(int32_t x, int32_t y, int32_t ax, int32_t ay)
{
    return !(((ax ^ x) < 0) || ((ay ^ y) < 0));
}

static void sc_draw_arc(sc_pfb_t *dest, const sc_arc_t *props, rect_t view)
{
    if(props->is_bar==0)
    {
        sc_draw_arc_props(dest, props,view);
        return;
    }
    area_t inter = rect_to_area(view);
    if (!sc_pfb_intersection(dest, &inter, NULL))   return;
    const int16_t start_deg = props->start_deg;
    const int16_t end_deg = props->end_deg;
     const int16_t bar_deg = props->bar_deg;
    const uint16_t st_c = props->start_c;
    const uint16_t end_c = props->end_c;
    const uint16_t fill = props->fill;
    const uint16_t alpha = props->opa;
    const uint8_t dot = props->dot&0x01;

    const int r = SC_MIN(view.w, view.h) / 2;
    const int ir = props->stroke_width > r ? 0 : r - props->stroke_width;
    const int cx = view.x + r;
    const int cy = view.y + r;
    
    const uint32_t rmax2 = (uint32_t)SC_POW2(r);
    const uint32_t r2 = (uint32_t)SC_POW2(r - 1);
    const uint32_t ir2 = (uint32_t)SC_POW2(ir) + props->stroke_width;
    const uint32_t rmin2 = ir < 0 ? 0U : (uint32_t)SC_POW2(ir - 1);
    const uint32_t inv_outer = SC_INV(rmax2, r2, alpha << 8);
    const uint32_t inv_inner = SC_INV(ir2, rmin2, alpha << 8);
    const bool full_total = (start_deg == end_deg);
    const uint8_t big_total = full_total ? 0U : sc_get_big_flag(start_deg, end_deg);
    const uint8_t big_prog = sc_get_big_flag(start_deg, bar_deg);
    const bool use_grad = (st_c != end_c);
    sc_arc_grad_t grad;
    if (use_grad)
    {
        sc_arc_grad_setup(&grad, start_deg, end_deg);
    }

    int32_t sx = -sc_sin((int16_t)start_deg);
    int32_t sy = sc_cos((int16_t)start_deg);
    int32_t ex = -sc_sin((int16_t)end_deg);
    int32_t ey = sc_cos((int16_t)end_deg);
    int32_t px = -sc_sin((int16_t)bar_deg);
    int32_t py = sc_cos((int16_t)bar_deg);

    int32_t csx_q1 = 0, csy_q1 = 0, cex_q1 = 0, cey_q1 = 0, cpx_q1 = 0, cpy_q1 = 0;
    int32_t cap_r2 = 0, cap_rmax2 = 0, cap_inv = 0;
    if (dot)
    {
        sc_arc_dot_setup(cx, cy, r, ir, (uint8_t)alpha, sx, sy, ex, ey,
                         &csx_q1, &csy_q1, &cex_q1, &cey_q1,
                         &cap_r2, &cap_rmax2, &cap_inv);
        sc_arc_dot_pt(cx, cy, px, py, r + ir, &cpx_q1, &cpy_q1);
    }
    else
    {
        sc_arc_flat_zones(sx, sy, ex, ey, &csx_q1, &cex_q1);
    }
    for (int y = inter.ys; y < inter.ye; y++)
    {
        int32_t dy = y - cy;
        int32_t ysq = dy * dy;
        int32_t dx = inter.xs - cx;
        int32_t cur_ds = dy * sx - dx * sy;
        int32_t cur_de = dx * ey - dy * ex;
        int32_t cur_dp = dx * py - dy * px;
        uint16_t *row = sc_pfb_pixel_ptr(dest, inter.xs, y);
        for (int x = inter.xs; x < inter.xe; x++, dx++)
        {
            int32_t rsq = dx * dx + ysq;
            uint8_t bc_a;
            uint8_t fc_a;
            bool in_total;
            bool in_prog;
            if (rsq > (int32_t)rmax2)
            {
                if (x > cx)
                {
                    break;
                }
                goto next_pixel;
            }
            if (rsq <= (int32_t)rmin2)
            {
                if (x < cx)
                {
                    int end = (cx * 2 - x);
                    for (; x < end; x++)
                    {
                        cur_ds -= sy;
                        cur_de += ey;
                        cur_dp += py;
                        dx++;
                    }
                }
                goto next_pixel;
            }
            bc_a = alpha;
            if (rsq > (int32_t)r2)
            {
                bc_a = (uint8_t)(((rmax2 - (uint32_t)rsq) * inv_outer) >> 8);
            }
            else if (rsq < (int32_t)ir2)
            {
                bc_a = (uint8_t)((((uint32_t)rsq - rmin2) * inv_inner) >> 8);
            }
            in_total = full_total ? true
                                  : ((big_total == 2) ? (cur_ds >= 0 || cur_de >= 0)
                                                      : (cur_ds >= 0 && cur_de >= 0));
            in_prog = (big_prog == 2) ? (cur_ds >= 0 || cur_dp >= 0) : (cur_ds >= 0 && cur_dp >= 0);
            fc_a = in_prog ? bc_a : 0;
            if (dot)
            {
                int32_t x_q1 = (cx + dx) << 1;
                int32_t y_q1 = y << 1;
                if (!in_total)
                {
                    int32_t d2_s = SC_POW2(x_q1 - csx_q1) + SC_POW2(y_q1 - csy_q1);
                    int32_t d2_e = SC_POW2(x_q1 - cex_q1) + SC_POW2(y_q1 - cey_q1);
                    int32_t d2_min = (d2_s < d2_e) ? d2_s : d2_e;
                    if (d2_min < cap_rmax2)
                    {
                        if (d2_min > cap_r2)
                        {
                            bc_a = (uint8_t)(((cap_rmax2 - d2_min) * cap_inv) >> 8);
                        }
                        if (d2_s < d2_e)
                        {
                            fc_a = bc_a;
                        }
                    }
                    else
                    {
                        bc_a = 0;
                    }
                }
                if (!in_prog)
                {
                    int32_t d2_p = SC_POW2(x_q1 - cpx_q1) + SC_POW2(y_q1 - cpy_q1);
                    if (d2_p < cap_rmax2)
                    {
                        uint8_t a = bc_a;
                        if (d2_p > cap_r2)
                        {
                            a = (uint8_t)(((cap_rmax2 - d2_p) * cap_inv) >> 8);
                        }
                        fc_a = SC_MAX(a, fc_a);
                    }
                }
            }
            else
            {
                if (!in_total)
                {
                    uint8_t zone = (uint8_t)(((dx > 0)) | ((dy > 0) << 1));
                    uint32_t sd = (zone == csx_q1) ? (uint32_t)SC_ABS(cur_ds) : 32767U;
                    uint32_t ed = (zone == cex_q1) ? (uint32_t)SC_ABS(cur_de) : 32767U;
                    uint32_t abs_min = (sd < ed ? sd : ed);
                    if (abs_min < 32767U)
                    {
                        uint8_t a = (uint8_t)(((255 - (abs_min >> 7)) * alpha) >> 8);
                        bc_a = SC_MIN(bc_a, a);
                        if (sd < ed)
                        {
                            fc_a = bc_a;
                        }
                    }
                    else
                    {
                        bc_a = 0;
                    }
                }
                if (!in_prog)
                {
                    uint32_t abs_dp = sc_is_progress(dx, dy, px, py) ? (uint32_t)SC_ABS(cur_dp) : 32767U;
                    if (abs_dp < 32767U)
                    {
                        uint8_t a = (uint8_t)(((255 - (abs_dp >> 7)) * alpha) >> 8);
                        fc_a = SC_MIN(bc_a, a);
                    }
                }
            }
            if (bc_a > 0)
            {
                uint16_t *dest_out = row + (x - inter.xs);
                if (fc_a > 0)
                {
                    uint16_t color = st_c;
                    if (use_grad)
                    {
                        color = sc_arc_grad_color(st_c, end_c, dx, dy, rsq, &grad);
                    }
                    if (fc_a < 250)
                    {
                        color = alphaBlend(color, fill, fc_a);
                        color = alphaBlend(color, *dest_out, bc_a);
                    }
                    *dest_out = color;
                }
                else
                {
                    *dest_out = alphaBlend(fill, *dest_out, bc_a);
                }
            }
        next_pixel:
            cur_ds -= sy;
            cur_de += ey;
            cur_dp += py;
        }
    }
}

void sc_lcd_draw_arc(sc_pfb_t *pfb, int16_t cx, int16_t cy, int r, const sc_arc_t *props)
{
    rect_t view = (rect_t){cx - r, cy - r, 2 * r, 2 * r};
    if (!props)
    {
        return;
    }
    if (pfb)
    {
        sc_draw_arc(pfb, props, view);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_arc(sc_lcd_draw_pfb__, props, view));
}

#if SC_USER_OBJ && SC_WIDGET_ARC_ENABLE

/* Expand dirty AABB by the cardinal extreme at the end of quadrant q (0:left 1:top 2:right 3:bottom). */
static void sc_arc_dirty_expand_q(area_t *dirty, int16_t cx, int16_t cy, uint16_t outer_r,
                                  uint16_t q)
{
    switch (q & 3U)
    {
    case 0:
        dirty->xs = SC_MIN(dirty->xs, (int16_t)(cx - (int16_t)outer_r));
        break;
    case 1:
        dirty->ys = SC_MIN(dirty->ys, (int16_t)(cy - (int16_t)outer_r));
        break;
    case 2:
        dirty->xe = SC_MAX(dirty->xe, (int16_t)(cx + (int16_t)outer_r));
        break;
    default:
        dirty->ye = SC_MAX(dirty->ye, (int16_t)(cy + (int16_t)outer_r));
        break;
    }
}

/*
 * AABB covering the forward arc start -> end in increasing-degree direction
 * (wraps across 0 when start > end). ir = inner radius (r - stroke_width).
 */
area_t sc_calc_arc_dirty(int16_t cx, int16_t cy, int16_t r, int16_t ir, int16_t start, int16_t end)
{
    area_t dirty;
    const uint16_t half = (1 << 14);
    const uint16_t pad = 1;
    const uint16_t outer_r = (uint16_t)(r + pad);
    const int32_t mid_r = (r + ir) / 2;
    const int32_t cap_r = pad + (r - ir) / 2;
    int16_t s_cx;
    int16_t s_cy;
    int16_t e_cx;
    int16_t e_cy;
    uint16_t start_q;
    uint16_t end_q;
    uint16_t q;

    /* Caller keeps start/end in [0, 360). */
    s_cx = (int16_t)(cx - ((mid_r * sc_sin(start) + half) >> 15));
    s_cy = (int16_t)(cy + ((mid_r * sc_cos(start) + half) >> 15));
    e_cx = (int16_t)(cx - ((mid_r * sc_sin(end) + half) >> 15));
    e_cy = (int16_t)(cy + ((mid_r * sc_cos(end) + half) >> 15));
    dirty.xs = (int16_t)(SC_MIN(s_cx, e_cx) - cap_r);
    dirty.xe = (int16_t)(SC_MAX(s_cx, e_cx) + cap_r);
    dirty.ys = (int16_t)(SC_MIN(s_cy, e_cy) - cap_r);
    dirty.ye = (int16_t)(SC_MAX(s_cy, e_cy) + cap_r);

    start_q = (uint16_t)(start / 90);
    end_q = (uint16_t)(end / 90);
    if (start <= end)
    {
        for (q = start_q; q < end_q; q++)
        {
            sc_arc_dirty_expand_q(&dirty, cx, cy, outer_r, q);
        }
    }
    else
    {
        /* Forward wrap: start..360 then 0..end (do not min/max swap). */
        for (q = start_q; q < 4U; q++)
        {
            sc_arc_dirty_expand_q(&dirty, cx, cy, outer_r, q);
        }
        for (q = 0U; q < end_q; q++)
        {
            sc_arc_dirty_expand_q(&dirty, cx, cy, outer_r, q);
        }
    }
    return dirty;
}

static area_t sc_arc_obj_deg_dirty(const sc_obj_t *obj, const sc_arc_t *props, int16_t a0,
                                   int16_t a1)
{
    const int r = SC_MIN(obj->view.w, obj->view.h) / 2;
    const int ir = props->stroke_width > r ? 0 : r - props->stroke_width;
    const int16_t cx = (int16_t)(obj->view.x + r);
    const int16_t cy = (int16_t)(obj->view.y + r);
    return sc_calc_arc_dirty(cx, cy, (int16_t)r, (int16_t)ir, a0, a1);
}

/* Gradient remaps the whole sector; solid color only needs the moved tip. */
static void sc_arc_invalidate_range_change(sc_obj_t *obj, const sc_arc_t *props,
                                           int16_t old_a, int16_t new_a, int16_t fixed,
                                           bool start_moved)
{
    area_t dirty;
    if (props->start_c != props->end_c)
    {
        area_t old_sec;
        area_t new_sec;
        if (start_moved)
        {
            old_sec = sc_arc_obj_deg_dirty(obj, props, old_a, fixed);
            new_sec = sc_arc_obj_deg_dirty(obj, props, new_a, fixed);
        }
        else
        {
            old_sec = sc_arc_obj_deg_dirty(obj, props, fixed, old_a);
            new_sec = sc_arc_obj_deg_dirty(obj, props, fixed, new_a);
        }
        dirty = area_merged(old_sec, new_sec);
    }
    else
    {
        dirty = sc_arc_obj_deg_dirty(obj, props, old_a, new_a);
    }

    sc_obj_invalidate_area(obj, &dirty);
}

#if SC_TOUCH_ENABLE
static uint16_t sc_arc_touch_deg(const sc_obj_t *obj, const sc_event_t *event)
{
    int16_t cx = (int16_t)(obj->view.x + obj->view.w / 2);
    int16_t cy = (int16_t)(obj->view.y + obj->view.h / 2);
    int16_t dx = (int16_t)(event->dat.pos[0] - cx);
    int16_t dy = (int16_t)(event->dat.pos[1] - cy);
    return (uint16_t)(((uint16_t)sc_atan2_alpha(dx, dy) * 360U + 128U) >> 8);
}

static bool sc_arc_hit_test(const sc_obj_t *obj, const sc_arc_t *props, const sc_event_t *event)
{
    rect_t view = obj->view;
    int32_t r = SC_MIN(view.w, view.h) / 2;
    int32_t ir = props->stroke_width > r ? 0 : r - props->stroke_width;
    int16_t cx = (int16_t)(view.x + r);
    int16_t cy = (int16_t)(view.y + r);
    int16_t dx = (int16_t)(event->dat.pos[0] - cx);
    int16_t dy = (int16_t)(event->dat.pos[1] - cy);
    int32_t dist_sq = (int32_t)dx * dx + (int32_t)dy * dy;
    if (dist_sq < (int32_t)ir * ir)
    {
        return false;
    }
    if (dist_sq > (int32_t)r * r)
    {
        return false;
    }
    uint16_t deg = sc_arc_touch_deg(obj, event);
    if (props->start_deg == props->end_deg)
    {
        return true;
    }
    if (props->start_deg < props->end_deg)
    {
        return deg >= (uint16_t)props->start_deg && deg <= (uint16_t)props->end_deg;
    }
    return deg >= (uint16_t)props->start_deg || deg <= (uint16_t)props->end_deg;
}
#endif

static const sc_obj_vt_t sc_arc_vt = {
    .event_cb = sc_obj_handle_arc_event,
    .set_style = sc_arc_style,
};



sc_obj_t *sc_arc_create(sc_obj_t *parent, sc_arc_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_ARC, &sc_arc_vt, props);
    if (obj)
    {
        *props = (sc_arc_t)SC_ARC_NEW(C_BLUE, C_CYAN, g_gui->bg, 28);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

void sc_arc_set_start_deg(sc_obj_t *obj, int start_deg)
{
    sc_arc_t *props = (sc_arc_t *)obj->props;
    start_deg = sc_arc_norm_deg(start_deg);
    if (props->start_deg != (int16_t)start_deg)
    {
        int16_t old = props->start_deg;
        props->start_deg = (int16_t)start_deg;
        sc_arc_invalidate_range_change(obj, props, old, (int16_t)start_deg, props->end_deg,
                                       true);
    }
}

void sc_arc_set_end_deg(sc_obj_t *obj, int end_deg)
{
    sc_arc_t *props = (sc_arc_t *)obj->props;
    end_deg = sc_arc_norm_deg(end_deg);
    if (props->end_deg != (int16_t)end_deg)
    {
        int16_t old = props->end_deg;
        props->end_deg = (int16_t)end_deg;
        sc_arc_invalidate_range_change(obj, props, old, (int16_t)end_deg, props->start_deg,
                                       false);
    }
}

void sc_arc_set_bar_deg(sc_obj_t *obj, int bar_deg)
{
    sc_arc_t *props = (sc_arc_t *)obj->props;
    int start_deg = props->start_deg;
    int end_deg = props->end_deg;
    int16_t old_bar = props->bar_deg;
    /* Caller keeps bar_deg in [0, 360); clamp into [start, end] only. */
    if (end_deg > start_deg)
    {
        if (bar_deg < start_deg)
        {
            bar_deg = start_deg;
        }
        else if (bar_deg > end_deg)
        {
            bar_deg = end_deg;
        }
    }
    else if (bar_deg > end_deg && bar_deg < start_deg)
    {
        int dist_to_end = bar_deg - end_deg;
        int dist_to_start = start_deg - bar_deg;
        bar_deg = (dist_to_end <= dist_to_start) ? end_deg : start_deg;
    }
    if (props->bar_deg == (int16_t)bar_deg)
    {
        return;
    }

    /* Same forward span as draw wedges: 2 = major arc (>180°) -> full invalidate. */
    if (sc_get_big_flag((int)old_bar, bar_deg) == 2U)
    {
        props->bar_deg = (int16_t)bar_deg;
        sc_obj_invalidate_area(obj, NULL);
        return;
    }
    props->bar_deg = (int16_t)bar_deg;
    {
        /* Bar is drawn start -> bar; not old_bar -> new_bar (avoids reverse merge). */
        area_t dirty = area_merged(
            sc_arc_obj_deg_dirty(obj, props, start_deg, old_bar),
            sc_arc_obj_deg_dirty(obj, props, (int16_t)start_deg, (int16_t)bar_deg));
        sc_obj_invalidate_area(obj, &dirty);
    }
}

void sc_arc_set_range(sc_obj_t *obj, int start_deg, int end_deg)
{
    sc_arc_set_start_deg(obj, start_deg);
    sc_arc_set_end_deg(obj, end_deg);
}

bool sc_arc_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_arc_t *props = (sc_arc_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->start_c = *(const color_t *)value;
        props->end_c = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->fill = *(const color_t *)value;
        return true;
    case SC_STYLE_ON:
        props->end_c = *(const color_t *)value;
        return true;
    case SC_STYLE_OPA:
        props->opa = *(const uint8_t *)value;
        return true;
    case SC_STYLE_CALLBACK:
        props->cb = *(const sc_obj_cb_t *)value;
        return true;
    default:
        return false;
    }
}

bool sc_obj_handle_arc_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_arc_t *props = (sc_arc_t *)obj->props;
    bool ret = false;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_arc((sc_pfb_t *)event->dat.arg, props, obj->view);
        return true;
    }
#if SC_TOUCH_ENABLE
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        ret = sc_arc_hit_test(obj, props, event);
        if (ret && props->is_bar)
        {
            g_gui->flags |= SC_RT_DRAGGING;
            sc_arc_set_bar_deg(obj, (int)sc_arc_touch_deg(obj, event));
        }
    }
    else if (event->type == SC_EVENT_TOUCH_MOVE && props->is_bar &&
             (g_gui->flags & SC_RT_DRAGGING) != 0U)
    {
        sc_arc_set_bar_deg(obj, (int)sc_arc_touch_deg(obj, event));
        ret = true;
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        g_gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        ret = true;
    }
#endif
    if (props->cb)
    {
        props->cb(obj, event);
    }
    return ret;
}

#endif /* SC_USER_OBJ && SC_WIDGET_ARC_ENABLE */
