#ifndef SC_TRANSFORM_H
#define SC_TRANSFORM_H

#include "../sc_common.h"
#include "../sc_obj.h"
#include "../sc_style.h"
#include "sc_image.h"

/* 自 SC4 SCGUI/src_draw/sc_transform.h 移植；仅 mem RGB565 / RGB565A8。
 * 恢复自 5a2c296（正常转动）：Transform_t 只存变换状态，AABB 直接写 obj->view。
 * 适配当前运行时：
 * - sc_obj_invalidate → sc_obj_invalidate_area
 * - 分页根已拆出 SC_OBJ_ROOT（本质仍是容器）：sc_get_Rotate_parent 用 <= SC_OBJ_CONTAINER
 * - 补 sc_transform_style（sc_style.c 使用） */
#define TRAMSFOM_DEBUG 0

typedef struct Transform_t
{
    const void *src;  /* sc_image_t * */
    int16_t center_x; /* source pivot, x8 */
    int16_t center_y;
    int16_t move_x;  /* pivot on parent, pixels */
    int16_t move_y;
    int16_t scaleX;   /* Q8, 256 = 1.0 */
    int16_t scaleY;
    int16_t sinA;
    int16_t cosA;
} Transform_t;

/* scale: Q8（256=1.0，匹配 rotate_demo / WASM 调用） */
void sc_set_transform_scale(Transform_t *p, int32_t scale_x_q8, int32_t scale_y_q8);
/* center: x8 定点 */
void sc_set_transform_center(Transform_t *p, int32_t center_x_x8, int32_t center_y_x8);
/* Writes current AABB into *view (caller invalidates old/new as needed). */
void sc_set_transform_angle(Transform_t *p, int angle, const rect_t *parent, rect_t *view);
void sc_draw_transform(sc_pfb_t *dest, Transform_t *p, const rect_t *parent, rect_t view);

#if SC_USER_OBJ && SC_WIDGET_TRANSFORM_ENABLE
/* 父对象原点：ROOT/CONTAINER 用 scroll_x/y（内容原点），页面平移后旋转中心保持正确。 */
rect_t sc_get_Rotate_parent(sc_obj_t *obj);
sc_obj_t *sc_Rotate_create(sc_obj_t *parent, Transform_t *props, const void *src);
void sc_set_Rotate_angle(sc_obj_t *obj, int angle);
void sc_set_Rotate_scale(sc_obj_t *obj, int32_t scale_x_q8, int32_t scale_y_q8);
void sc_set_Rotate_center(sc_obj_t *obj, int32_t center_x_x8, int32_t center_y_x8);
void sc_set_Rotate_move(sc_obj_t *obj, int32_t move_x, int32_t move_y);
bool sc_transform_style(sc_obj_t *obj, sc_style_t style, const void *value);
#endif

#endif
