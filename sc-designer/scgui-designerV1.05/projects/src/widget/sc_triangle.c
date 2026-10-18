#include "../sc_gui.h"
#include "sc_triangle.h"

/* 边叉乘（带符号二倍面积分量）。 */
static inline int64_t sc_triangle_edge(int ax, int ay, int bx, int by, int px, int py)
{
    return (int64_t)(bx - ax) * (py - ay) - (int64_t)(by - ay) * (px - ax);
}

/* 三角形绘制：填充（半平面测试，fill==bg 跳过）+ 三边 1px AA 描边。 */
void sc_draw_triangle(sc_pfb_t *dest, int x1, int y1, int x2, int y2, int x3, int y3, color_t color,
                      color_t fill, uint16_t alpha)
{
    area_t area = {
        (int16_t)(SC_MIN(x1, SC_MIN(x2, x3)) - 1),
        (int16_t)(SC_MIN(y1, SC_MIN(y2, y3)) - 1),
        (int16_t)(SC_MAX(x1, SC_MAX(x2, x3)) + 2),
        (int16_t)(SC_MAX(y1, SC_MAX(y2, y3)) + 2),
    };
    area_t inter = area;
    int64_t area2;

    if (!dest || !sc_pfb_intersection(dest, &inter, NULL))
    {
        return;
    }
    area2 = sc_triangle_edge(x1, y1, x2, y2, x3, y3);
    if (area2 < 0)
    {
        int tx = x2;
        int ty = y2;
        x2 = x3;
        y2 = y3;
        x3 = tx;
        y3 = ty;
        area2 = -area2;
    }
    if (area2 > 0 && fill != g_gui->bg)
    {
        uint16_t *row = sc_pfb_pixel_ptr(dest, inter.xs, inter.ys);
        for (int y = inter.ys; y < inter.ye; y++, row += dest->width)
        {
            for (int x = inter.xs; x < inter.xe; x++)
            {
                int64_t e1 = sc_triangle_edge(x1, y1, x2, y2, x, y);
                int64_t e2 = sc_triangle_edge(x2, y2, x3, y3, x, y);
                int64_t e3 = sc_triangle_edge(x3, y3, x1, y1, x, y);
                if (e1 >= 0 && e2 >= 0 && e3 >= 0)
                {
                    row[x - inter.xs] = alphaBlend(fill, row[x - inter.xs], (uint8_t)alpha);
                }
            }
        }
    }
    sc_draw_line_fast_aa(dest, (int16_t)x1, (int16_t)y1, (int16_t)x2, (int16_t)y2, color, (uint8_t)alpha);
    sc_draw_line_fast_aa(dest, (int16_t)x2, (int16_t)y2, (int16_t)x3, (int16_t)y3, color, (uint8_t)alpha);
    sc_draw_line_fast_aa(dest, (int16_t)x3, (int16_t)y3, (int16_t)x1, (int16_t)y1, color, (uint8_t)alpha);
}

#if SC_USER_OBJ && SC_WIDGET_TRIANGLE_ENABLE

/* 点是否在三角形内：半平面测试，三边同号即在内（兼容顺/逆时针绕序）。 */
#if SC_TOUCH_ENABLE
static bool sc_triangle_contains(const sc_obj_t *obj, int16_t x, int16_t y)
{
    const sc_triangle_t *p = (const sc_triangle_t *)obj->props;
    const int ax = obj->view.x + p->x1;
    const int ay = obj->view.y + p->y1;
    const int bx = obj->view.x + p->x2;
    const int by = obj->view.y + p->y2;
    const int cx = obj->view.x + p->x3;
    const int cy = obj->view.y + p->y3;
    const int64_t e1 = sc_triangle_edge(ax, ay, bx, by, x, y);
    const int64_t e2 = sc_triangle_edge(bx, by, cx, cy, x, y);
    const int64_t e3 = sc_triangle_edge(cx, cy, ax, ay, x, y);
    return (e1 >= 0 && e2 >= 0 && e3 >= 0) || (e1 <= 0 && e2 <= 0 && e3 <= 0);
}
#endif

static const sc_obj_vt_t sc_triangle_vt = {
    .event_cb = sc_obj_handle_triangle_event,
    .set_style = sc_triangle_style,
};



sc_obj_t *sc_triangle_create(sc_obj_t *parent, sc_triangle_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_TRIANGLE, &sc_triangle_vt, props);
    if (obj)
    {
        *props = (sc_triangle_t)SC_TRIANGLE_NEW(C_WHITE, C_BLUE);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

void sc_triangle_set_points(sc_obj_t *obj, int16_t x1, int16_t y1, int16_t x2, int16_t y2,
                            int16_t x3, int16_t y3)
{
    sc_triangle_t *props = (sc_triangle_t *)obj->props;
    /* 3 顶点原始包围盒（父坐标）。 */
    const int16_t xmin = (int16_t)SC_MIN(x1, SC_MIN(x2, x3));
    const int16_t ymin = (int16_t)SC_MIN(y1, SC_MIN(y2, y3));
    rect_t view = {xmin, ymin,
                   (uint16_t)(SC_MAX(x1, SC_MAX(x2, x3)) - xmin + 1),
                   (uint16_t)(SC_MAX(y1, SC_MAX(y2, y3)) - ymin + 1)};
    /* sc_draw_triangle 内部 AABB ±1 + AA 线 2px 足迹，外扩 2 像素防 AA 描边被 view 裁掉。 */
    view = rect_grow(view, 2, 2);
    /* 定位 view，并把顶点重定位回 view 原点（相对量，三角形随 view 平移）。 */
    sc_obj_set_rect(obj, view.x, view.y, view.w, view.h, ALIGN_NONE);
    props->x1 = (int16_t)(x1 - view.x);
    props->y1 = (int16_t)(y1 - view.y);
    props->x2 = (int16_t)(x2 - view.x);
    props->y2 = (int16_t)(y2 - view.y);
    props->x3 = (int16_t)(x3 - view.x);
    props->y3 = (int16_t)(y3 - view.y);
}

bool sc_triangle_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_triangle_t *props = (sc_triangle_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->color = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->fill = *(const color_t *)value;
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

bool sc_obj_handle_triangle_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        const sc_triangle_t *p = (const sc_triangle_t *)obj->props;
        sc_draw_triangle((sc_pfb_t *)event->dat.arg,
                         obj->view.x + p->x1, obj->view.y + p->y1,
                         obj->view.x + p->x2, obj->view.y + p->y2,
                         obj->view.x + p->x3, obj->view.y + p->y3,
                         p->color, p->fill, p->opa);
        return true;
    }
#if SC_TOUCH_ENABLE
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        /* 框架已做过 view-rect 粗命中；这里做三角形内精确命中。 */
        if (!sc_triangle_contains(obj, event->dat.pos[0], event->dat.pos[1]))
        {
            return false;
        }
        if ((obj->flags & SC_OBJ_F_PRESSED) == 0U)
        {
            obj->flags |= SC_OBJ_F_PRESSED;
            sc_obj_invalidate(obj);
        }
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_MOVE)
    {
        return false;
    }
    if (event->type == SC_EVENT_TOUCH_UP)
    {
        if ((obj->flags & SC_OBJ_F_PRESSED) != 0U)
        {
            sc_triangle_t *p = (sc_triangle_t *)obj->props;
            obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
            sc_obj_invalidate(obj);
            /* 抬起且在三角形内 = 一次点击。 */
            if (p->cb && sc_triangle_contains(obj, event->dat.pos[0], event->dat.pos[1]))
            {
                p->cb(obj, event);
            }
        }
        return true;
    }
#endif
    return false;
}

#endif /* SC_USER_OBJ && SC_WIDGET_TRIANGLE_ENABLE */
