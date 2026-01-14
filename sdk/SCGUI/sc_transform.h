#ifndef SC_TRANSFORM_H
#define SC_TRANSFORM_H

#include "sc_common.h"
#include "sc_lvgl_font.h"

typedef struct Transform_t
{
    sc_rect_t box;    // 包围盒
    sc_rect_t drity;  // 合并区域
    void *src;        // 源图
    int16_t center_x; // 源图中心点
    int16_t center_y; // 源图中心点
    int16_t move_x;   // 平移x
    int16_t move_y;   // 平移y
    int16_t scaleX;   // X缩放
    int16_t scaleY;   // Y缩放
    int16_t sinA;     // 角度sin
    int16_t cosA;     // 角度cos
} Transform_t;

void sc_init_trans(int tx, int ty, void *src,  Transform_t *p);

void sc_set_trans_angle(int16_t q8_scaleX, int16_t q8_scaleY, int16_t Angle, Transform_t *p);

void sc_init_trans_label( sc_label_t *label,int cx, int cy, void *imge, Transform_t *p);

void sc_draw_trans(sc_pfb_t *dest, Transform_t *p);

#endif




