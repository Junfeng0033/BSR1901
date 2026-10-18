#ifndef SC_STYLE_H
#define SC_STYLE_H

#include "sc_common.h"
#include "sc_event.h"

typedef uint8_t sc_grad_dir_t;
enum
{
    SC_GRAD_HOR,
    SC_GRAD_VER,
    SC_GRAD_DIAG_DOWN,
    SC_GRAD_DIAG_UP,
    SC_GRAD_CENTER,  /* Manhattan diamond (legacy) */
    SC_GRAD_RADIAL,  /* Circular spread 0..r, r = (width + height) / 2 */
    SC_GRAD_IMAGE,
};
struct sc_image_t;
typedef struct sc_texture_t sc_texture_t;
typedef color_t (*sc_texture_color_cb_t)(const sc_texture_t *texture,
                                        uint16_t x, uint16_t y);
typedef struct sc_texture_gradient_t
{
    color_t c0;
    color_t c1;
    uint16_t width;
    uint16_t height;
    uint16_t den;
    uint16_t scale;
} sc_texture_gradient_t;

struct sc_texture_t
{
    sc_texture_color_cb_t get_color;
    int16_t offset_x;
    int16_t offset_y;
    sc_grad_dir_t dir;
    union
    {
        sc_texture_gradient_t gradient;
        const struct sc_image_t *image;
    } data;
};

void sc_texture_init_custom(sc_texture_t *texture, sc_grad_dir_t dir,
                            uint16_t width, uint16_t height,
                            color_t c0, color_t c1,
                            const struct sc_image_t *image);

struct sc_obj_t;
typedef void (*sc_obj_cb_t)(struct sc_obj_t *obj, const sc_event_t *event);

typedef uint8_t sc_style_t;
enum
{
    SC_STYLE_FG,
    SC_STYLE_BG,
    SC_STYLE_BORDER,
    SC_STYLE_ON,
    SC_STYLE_FONT,
    SC_STYLE_ALIGN,
    SC_STYLE_OPA,
    SC_STYLE_TEXT,
    SC_STYLE_CALLBACK,
    SC_STYLE_TEXTURE,
    SC_STYLE_RADIUS,       /* uint8_t corner radius */
    SC_STYLE_BORDER_WIDTH, /* uint8_t stroke width */
    SC_STYLE_RECT_MASK,    /* uint8_t per-corner square mask */
SC_STYLE_RADIUS_READ,  /* read-only: *(uint8_t *)value = corner radius */
};

typedef struct sc_obj_text_style_t
{
    const char *text;
    uint8_t font_id;
} sc_obj_text_style_t;

typedef struct sc_obj_texture_style_t
{
    sc_texture_t *texture;
    const struct sc_image_t *image;
    color_t c0;
    color_t c1;
    sc_grad_dir_t dir;
} sc_obj_texture_style_t;

bool sc_obj_set_style(struct sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_get_style(struct sc_obj_t *obj, sc_style_t style, void *value);

static inline bool sc_obj_add_texture(struct sc_obj_t *obj,
                                      sc_texture_t *texture,
                                      sc_grad_dir_t dir,
                                      color_t c0, color_t c1)
{
    sc_obj_texture_style_t value = {texture, NULL, c0, c1, dir};
    return sc_obj_set_style(obj, SC_STYLE_TEXTURE, &value);
}

static inline bool sc_obj_add_image_texture(struct sc_obj_t *obj,
                                            sc_texture_t *texture,
                                            const struct sc_image_t *image)
{
    sc_obj_texture_style_t value = {
        texture, image, 0U, 0U, SC_GRAD_IMAGE};
    return sc_obj_set_style(obj, SC_STYLE_TEXTURE, &value);
}

static inline bool sc_obj_apply_texture(sc_texture_t **target, rect_t view,
                                        const void *value)
{
    const sc_obj_texture_style_t *texture_style =
        (const sc_obj_texture_style_t *)value;
    if (texture_style->texture)
    {
        if (view.w == 0U || view.h == 0U)
        {
            return false;
        }
        sc_texture_init_custom(texture_style->texture, texture_style->dir,
                               view.w, view.h, texture_style->c0,
                               texture_style->c1, texture_style->image);
    }
    *target = texture_style->texture;
    return true;
}

static inline bool sc_obj_set_fg(struct sc_obj_t *obj, color_t value)
{
    return sc_obj_set_style(obj, SC_STYLE_FG, &value);
}

static inline bool sc_obj_set_bg(struct sc_obj_t *obj, color_t value)
{
    return sc_obj_set_style(obj, SC_STYLE_BG, &value);
}

static inline bool sc_obj_set_border(struct sc_obj_t *obj, color_t value)
{
    return sc_obj_set_style(obj, SC_STYLE_BORDER, &value);
}

static inline bool sc_obj_set_on(struct sc_obj_t *obj, color_t value)
{
    return sc_obj_set_style(obj, SC_STYLE_ON, &value);
}

static inline bool sc_obj_set_font(struct sc_obj_t *obj, uint8_t value)
{
    return sc_obj_set_style(obj, SC_STYLE_FONT, &value);
}

static inline bool sc_obj_set_align(struct sc_obj_t *obj, sc_align_t value)
{
    return sc_obj_set_style(obj, SC_STYLE_ALIGN, &value);
}

static inline bool sc_obj_set_opa(struct sc_obj_t *obj, uint8_t value)
{
    return sc_obj_set_style(obj, SC_STYLE_OPA, &value);
}

static inline bool sc_obj_set_radius(struct sc_obj_t *obj, uint8_t value)
{
    /* 圆角钳制在 API 层：radius ≤ 短边一半。
     * sc_obj_t 不完整，但首成员就是 view，强转 (const rect_t *) 即可访问。 */
    const rect_t *view = (const rect_t *)obj;
    int max_r = (int)SC_MIN(view->w, view->h) >> 1;
    if ((int)value > max_r)
    {
        value = (uint8_t)max_r;
    }
    return sc_obj_set_style(obj, SC_STYLE_RADIUS, &value);
}

static inline bool sc_obj_set_border_width(struct sc_obj_t *obj, uint8_t value)
{
    return sc_obj_set_style(obj, SC_STYLE_BORDER_WIDTH, &value);
}

static inline bool sc_obj_set_rect_mask(struct sc_obj_t *obj, uint8_t value)
{
    return sc_obj_set_style(obj, SC_STYLE_RECT_MASK, &value);
}

static inline bool sc_obj_set_text(struct sc_obj_t *obj, const char *text, uint8_t font_id)
{
    sc_obj_text_style_t value = {text, font_id};
    return sc_obj_set_style(obj, SC_STYLE_TEXT, &value);
}

static inline bool sc_obj_set_callback(struct sc_obj_t *obj, sc_obj_cb_t value)
{
    return sc_obj_set_style(obj, SC_STYLE_CALLBACK, &value);
}

#endif
