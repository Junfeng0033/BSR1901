#include "sc_gui.h"
#include "widget/sc_image.h"

/* Blend gradient endpoints for a position along the gradient, adding a 2x2 Bayer dither to smooth banding. */
static inline color_t sc_texture_gradient_blend(const sc_texture_gradient_t *gradient,
                                                int pos, uint16_t x, uint16_t y)
{
    uint16_t alpha = (uint16_t)(((uint32_t)SC_CLAMP(pos, 0, gradient->den) *
                                 gradient->scale) >>
                                8);
    uint8_t bayer = (uint8_t)(((x & 1U) << 1) ^ ((y & 1U) * 3U));
    alpha = (uint16_t)(alpha + ((uint16_t)bayer << 1));
    if (alpha > 251U)
    {
        return gradient->c1;
    }
    return alphaBlend(gradient->c1, gradient->c0, (uint8_t)alpha);
}

/* Texture color callback for a horizontal gradient (position taken from the x coordinate). */
static color_t sc_texture_hor_color(const sc_texture_t *texture,
                                    uint16_t x, uint16_t y)
{
    const sc_texture_gradient_t *gradient = &texture->data.gradient;
    uint16_t texture_x = (uint16_t)SC_MAX((int)x - texture->offset_x, 0);
    uint16_t texture_y = (uint16_t)SC_MAX((int)y - texture->offset_y, 0);
    return sc_texture_gradient_blend(gradient, texture_x,
                                     texture_x, texture_y);
}

/* Texture color callback for a vertical gradient (position taken from the y coordinate). */
static color_t sc_texture_ver_color(const sc_texture_t *texture,
                                    uint16_t x, uint16_t y)
{
    const sc_texture_gradient_t *gradient = &texture->data.gradient;
    uint16_t texture_x = (uint16_t)SC_MAX((int)x - texture->offset_x, 0);
    uint16_t texture_y = (uint16_t)SC_MAX((int)y - texture->offset_y, 0);
    return sc_texture_gradient_blend(gradient, texture_y,
                                     texture_x, texture_y);
}

/* Texture color callback for diagonal/center gradients, computing the gradient position from the direction. */
static color_t sc_texture_gradient_color(const sc_texture_t *texture,
                                         uint16_t x, uint16_t y)
{
    const sc_texture_gradient_t *gradient = &texture->data.gradient;
    x = (uint16_t)SC_MAX((int)x - texture->offset_x, 0);
    y = (uint16_t)SC_MAX((int)y - texture->offset_y, 0);
    int pos;
    switch (texture->dir)
    {
    case SC_GRAD_DIAG_UP:
        pos = x + gradient->height - 1U - y;
        break;
    case SC_GRAD_CENTER:
        pos = SC_ABS((int)x - (int)(gradient->width >> 1)) +
              SC_ABS((int)y - (int)(gradient->height >> 1));
        break;
    case SC_GRAD_DIAG_DOWN:
    default:
        pos = x + y;
        break;
    }
    return sc_texture_gradient_blend(gradient, pos, x, y);
}

/*
 * Circular radial gradient: distance 0 (center) -> r.
 * r = (width + height) / 2.
 */
static color_t sc_texture_radial_color(const sc_texture_t *texture,
                                       uint16_t x, uint16_t y)
{
    const sc_texture_gradient_t *gradient = &texture->data.gradient;
    x = (uint16_t)SC_MAX((int)x - texture->offset_x, 0);
    y = (uint16_t)SC_MAX((int)y - texture->offset_y, 0);
    int32_t dx = (int32_t)x - (int32_t)(gradient->width >> 1);
    int32_t dy = (int32_t)y - (int32_t)(gradient->height >> 1);
    uint32_t dist = sc_isqrt_u32((uint32_t)SC_POW2(dx) + (uint32_t)SC_POW2(dy));
    return sc_texture_gradient_blend(gradient, (int)dist, x, y);
}

/* Texture color callback that samples a source image pixel, returning 0 for out-of-bounds coordinates.
 * offset_x/offset_y 为正时纹理内容向右/下移（src = pos - offset），
 * 与设计器「纹理 X/Y 偏移」同口径：+20 即右移/下移 20。 */
static color_t sc_texture_image_color(const sc_texture_t *texture,
                                      uint16_t x, uint16_t y)
{
    const sc_image_t *image = texture->data.image;
    int32_t src_x = (int32_t)x - texture->offset_x;
    int32_t src_y = (int32_t)y - texture->offset_y;
    if ((uint32_t)src_x >= image->width ||
        (uint32_t)src_y >= image->height ||
        image->type != SC_IMAGE_RGB565)
    {
        return 0U;
    }
    return ((const color_t *)image->data.pixels)
        [(uint32_t)src_y * image->width + (uint32_t)src_x];
}

/* Initialize a texture as an image or a gradient of the given direction, precomputing scale and selecting the color callback. */
void sc_texture_init_custom(sc_texture_t *texture, sc_grad_dir_t dir,
                            uint16_t width, uint16_t height,
                            color_t c0, color_t c1,
                            const sc_image_t *image)
{
    texture->dir = dir;
    texture->offset_x = 0;
    texture->offset_y = 0;
    if (dir == SC_GRAD_IMAGE)
    {
        texture->get_color = sc_texture_image_color;
        texture->data.image = image;
        return;
    }
    sc_texture_gradient_t *gradient = &texture->data.gradient;
    gradient->c0 = c0;
    gradient->c1 = c1;
    gradient->width = width;
    gradient->height = height;
    if (dir == SC_GRAD_RADIAL)
    {
        /* r = (w + h) / 2 — between corner distance and full w+h. */
        uint32_t r = ((uint32_t)width + (uint32_t)height) >> 1;
        if (r == 0U)
        {
            r = 1U;
        }
        gradient->den = (uint16_t)r;
        gradient->scale = (uint16_t)((255U << 8) / r);
        texture->get_color = sc_texture_radial_color;
        return;
    }
    uint16_t den = (dir == SC_GRAD_HOR)      ? (uint16_t)(width - 1U)
                   : (dir == SC_GRAD_VER)    ? (uint16_t)(height - 1U)
                   : (dir == SC_GRAD_CENTER) ? (uint16_t)((width >> 1) + (height >> 1))
                                             : (uint16_t)(width + height - 2U);
    gradient->den = den;
    gradient->scale = den ? (uint16_t)((255U << 8) / den) : 0U;
    if (dir == SC_GRAD_HOR)
    {
        texture->get_color = sc_texture_hor_color;
        return;
    }
    texture->get_color = (dir == SC_GRAD_VER) ? sc_texture_ver_color
                                              : sc_texture_gradient_color;
}

#if SC_USER_OBJ
/* Apply a style property via the object's virtual table and invalidate on success. */
bool sc_obj_set_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    if (!obj || !obj->vt || !obj->vt->set_style)
    {
        return false;
    }
    bool ok = obj->vt->set_style(obj, style, value);
    if (!ok)
    {
        return false;
    }
    sc_obj_invalidate(obj);
    return true;
}
bool sc_obj_get_style(sc_obj_t *obj, sc_style_t style, void *value)
{
    if (!obj || !obj->vt || !obj->vt->set_style)
    {
        return false;
    }
    /* Reuse the same vt set_style dispatch: read-only styles write out. */
    return obj->vt->set_style(obj, style, value);
}
#endif /* SC_USER_OBJ */
