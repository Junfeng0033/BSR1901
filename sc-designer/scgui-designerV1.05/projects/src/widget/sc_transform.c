#include "../sc_draw.h"
#include "../sc_gui.h"
#include "sc_transform.h"

static inline void _Bilinear_Image(const color_t *map, const uint8_t *mask, int32_t w, int32_t h,
                                   int32_t x, int32_t y, uint8_t fx, uint8_t fy, color_t *dst)
{
    color_t bg = *dst;
    int32_t xe = w - 1;
    int32_t ye = h - 1;
    int32_t offs = y * w + x;
    color_t p00 = (x >= 0 && y >= 0) ? map[offs] : bg;
    color_t p01 = (x < xe && y >= 0) ? map[offs + 1] : bg;
    color_t p10 = (y < ye && x >= 0) ? map[offs + w] : bg;
    color_t p11 = (y < ye && x < xe) ? map[offs + w + 1] : bg;
    if (mask)
    {
        if (p00 != bg)
        {
            p00 = alphaBlend(p00, bg, mask[offs]);
        }
        if (p01 != bg)
        {
            p01 = alphaBlend(p01, bg, mask[offs + 1]);
        }
        if (p10 != bg)
        {
            p10 = alphaBlend(p10, bg, mask[offs + w]);
        }
        if (p11 != bg)
        {
            p11 = alphaBlend(p11, bg, mask[offs + w + 1]);
        }
    }
    else
    {
        p00 = p00 ? p00 : bg;
        p01 = p01 ? p01 : bg;
        p10 = p10 ? p10 : bg;
        p11 = p11 ? p11 : bg;
    }
    color_t y0 = alphaBlend(p01, p00, fx);
    color_t y1 = alphaBlend(p11, p10, fx);
    *dst = alphaBlend(y1, y0, fy);
}

void sc_set_transform_scale(Transform_t *p, int32_t scale_x_q8, int32_t scale_y_q8)
{
    p->scaleX = (int16_t)(scale_x_q8 > 0 ? scale_x_q8 : 1);
    p->scaleY = (int16_t)(scale_y_q8 > 0 ? scale_y_q8 : 1);
}

void sc_set_transform_center(Transform_t *p, int32_t center_x_x8, int32_t center_y_x8)
{
    p->center_x = (int16_t)center_x_x8;
    p->center_y = (int16_t)center_y_x8;
}

/* Compute rotated AABB into *view. parent may be NULL (origin 0,0). */
void sc_set_transform_angle(Transform_t *p, int angle, const rect_t *parent, rect_t *view)
{
    const sc_image_t *image = (const sc_image_t *)p->src;
    int32_t w = (int32_t)image->width << 3;
    int32_t h = (int32_t)image->height << 3;
    int32_t v[4][2] = {{0, 0}, {w, 0}, {0, h}, {w, h}};
    int32_t parent_x = parent ? parent->x : 0;
    int32_t parent_y = parent ? parent->y : 0;

    p->sinA = (int16_t)sc_sin((int16_t)angle);
    p->cosA = (int16_t)sc_cos((int16_t)angle);
    int32_t m00 = (p->cosA * p->scaleX) >> 8;
    int32_t m01 = (-p->sinA * p->scaleY) >> 8;
    int32_t m10 = (p->sinA * p->scaleX) >> 8;
    int32_t m11 = (p->cosA * p->scaleY) >> 8;
    int32_t pivot_x = (p->move_x + parent_x) << 15;
    int32_t pivot_y = (p->move_y + parent_y) << 15;
    int32_t min_x = 0;
    int32_t max_x = 0;
    int32_t min_y = 0;
    int32_t max_y = 0;
    for (int i = 0; i < 4; i++)
    {
        int32_t x = v[i][0] - p->center_x;
        int32_t y = v[i][1] - p->center_y;
        int32_t dx = ((x * m00 + y * m01) >> 3) + pivot_x;
        int32_t dy = ((x * m10 + y * m11) >> 3) + pivot_y;
        if (i == 0)
        {
            min_x = max_x = dx;
            min_y = max_y = dy;
        }
        else
        {
            min_x = SC_MIN(min_x, dx);
            min_y = SC_MIN(min_y, dy);
            max_x = SC_MAX(max_x, dx);
            max_y = SC_MAX(max_y, dy);
        }
    }
    min_x = (min_x >> 15) - 1;
    min_y = (min_y >> 15) - 1;
    max_x = (max_x >> 15) + 2;
    max_y = (max_y >> 15) + 2;
    view->x = (int16_t)min_x;
    view->y = (int16_t)min_y;
    view->w = (uint16_t)(max_x - min_x);
    view->h = (uint16_t)(max_y - min_y);
}

void sc_draw_transform(sc_pfb_t *dest, Transform_t *p, const rect_t *parent, rect_t view)
{
    area_t intersection = rect_to_area(view);
    if (!sc_pfb_intersection(dest, &intersection, NULL))
    {
        return;
    }
    int32_t center_x = (int32_t)p->center_x << 12;
    int32_t center_y = (int32_t)p->center_y << 12;
    int32_t sinA = (int32_t)p->sinA << 8;
    int32_t cosA = (int32_t)p->cosA << 8;
    int32_t m00 = cosA / p->scaleX;
    int32_t m01 = sinA / p->scaleX;
    int32_t m10 = -(sinA / p->scaleY);
    int32_t m11 = cosA / p->scaleY;
    int32_t move_x = p->move_x + (parent ? parent->x : 0);
    int32_t move_y = p->move_y + (parent ? parent->y : 0);

    int32_t xd = intersection.xs - move_x;
    int32_t rotatedX_base = m00 * xd + center_x;
    int32_t rotatedY_base = m10 * xd + center_y;
    int32_t Cx = m01 * (intersection.ys - move_y);
    int32_t Cy = m11 * (intersection.ys - move_y);

    const sc_image_t *image = (const sc_image_t *)p->src;
    const color_t *map = (const color_t *)image->data.pixels;
    const uint8_t *mask = NULL;
    if ((image->type & SC_IMAGE_FMT_MASK) == SC_IMAGE_RGB565A8)
    {
        mask = (const uint8_t *)image->data.pixels + SC_IMAGE_A8_MASK_OFFSET(image->width, image->height);
    }
    int32_t img_w = image->width;
    int32_t img_h = image->height;
    color_t *row = sc_pfb_pixel_ptr(dest, intersection.xs, intersection.ys);
    for (int32_t y = intersection.ys; y < intersection.ye; y++, row += dest->width)
    {
        int32_t rotatedX = rotatedX_base + Cx;
        int32_t rotatedY = rotatedY_base + Cy;
        bool hit = false;
        color_t *dest_out = row;
        for (int32_t x = intersection.xs; x < intersection.xe; x++, dest_out++)
        {
            int32_t sx = rotatedX >> 15;
            int32_t sy = rotatedY >> 15;
            if (sx >= -1 && sx < img_w && sy >= -1 && sy < img_h)
            {
                uint8_t fx = (uint8_t)((rotatedX & 0x7FFF) >> 7);
                uint8_t fy = (uint8_t)((rotatedY & 0x7FFF) >> 7);
                _Bilinear_Image(map, mask, img_w, img_h, sx, sy, fx, fy, dest_out);
                hit = true;
            }
            else if (hit)
            {
                break;
            }
            rotatedX += m00;
            rotatedY += m10;
        }
        Cx += m01;
        Cy += m11;
    }
#if TRAMSFOM_DEBUG
    sc_draw_Frame(dest, view.x, view.y, view.w, view.h, 1, C_RED, 255);
    sc_draw_point(dest, (int16_t)move_x, (int16_t)move_y, C_RED);
#endif
}

#if SC_USER_OBJ && SC_WIDGET_TRANSFORM_ENABLE

/* 父对象原点：内容区在屏幕上的起点 = view.xy + scroll_xy。
 * ROOT/CONTAINER（本质都是容器）统一此规则，页面平移后旋转中心保持正确，
 * 且容器内旋转图的屏幕绝对坐标与按钮 / label 等其他控件一致。 */
rect_t sc_get_Rotate_parent(sc_obj_t *obj)
{
    sc_obj_t *parent = sc_obj_at(obj->parent);
    rect_t rect;
    if (!parent)
    {
        return g_gui->screen; /* 无父对象（根级旋转）：直接以屏幕矩形为原点 */
    }
    rect = parent->view;
    if (sc_obj_type(parent) <= SC_OBJ_CONTAINER)
    {
        sc_container_t *container = (sc_container_t *)parent->props;
        /* 内容原点 = 父 view 屏幕位置 - 滚动偏移 */
        rect.x += container->scroll_x;
        rect.y += container->scroll_y;
    }
    return rect;
}

bool sc_transform_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    (void)obj;
    (void)style;
    (void)value;
    return false;
}

void sc_set_Rotate_scale(sc_obj_t *obj, int32_t scale_x_q8, int32_t scale_y_q8)
{
    sc_set_transform_scale(obj->props, scale_x_q8, scale_y_q8);
}

void sc_set_Rotate_center(sc_obj_t *obj, int32_t center_x_x8, int32_t center_y_x8)
{
    sc_set_transform_center(obj->props, center_x_x8, center_y_x8);
}

void sc_set_Rotate_move(sc_obj_t *obj, int32_t move_x, int32_t move_y)
{
    Transform_t *p = obj->props;
    p->move_x = (int16_t)move_x;
    p->move_y = (int16_t)move_y;
}

void sc_set_Rotate_angle(sc_obj_t *obj, int angle)
{
    rect_t parent = sc_get_Rotate_parent(obj);
    sc_obj_invalidate_area(obj, NULL);
    sc_set_transform_angle(obj->props, angle, &parent, &obj->view);
    sc_obj_invalidate_area(obj, NULL);
}

static bool sc_obj_handle_rotate_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        rect_t parent = sc_get_Rotate_parent(obj);
        sc_draw_transform((sc_pfb_t *)event->dat.arg, obj->props, &parent, obj->view);
        return true;
    }
    return false;
}

static const sc_obj_vt_t sc_rotate_vt = {
    .event_cb = sc_obj_handle_rotate_event,
    .set_style = sc_transform_style,
};

sc_obj_t *sc_Rotate_create(sc_obj_t *parent, Transform_t *props, const void *src)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_ROTATE, &sc_rotate_vt, props);
    if (!obj)
    {
        return NULL;
    }
    const sc_image_t *image = (const sc_image_t *)src;
    props->src = src;
    props->center_x = (int16_t)(image->width * 4);
    props->center_y = (int16_t)(image->height * 4);
    props->scaleX = 256;
    props->scaleY = 256;
    props->move_x = (int16_t)(parent ? parent->view.w / 2 : SC_SCREEN_WIDTH / 2);
    props->move_y = (int16_t)(parent ? parent->view.h / 2 : SC_SCREEN_HEIGHT / 2);
    props->sinA = 0;
    props->cosA = 32767;
    sc_set_Rotate_angle(obj, 0);
    return obj;
}

#endif /* SC_USER_OBJ && SC_WIDGET_TRANSFORM_ENABLE */
