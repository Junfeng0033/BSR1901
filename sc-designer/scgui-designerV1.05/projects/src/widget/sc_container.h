#ifndef SC_WIDGET_CONTAINER_H
#define SC_WIDGET_CONTAINER_H

#include "../sc_common.h"
#include "../sc_obj.h"

#define SC_CONTAINER_F_H_SCROLL 0x01U
#define SC_CONTAINER_F_V_SCROLL 0x02U
#define SC_CONTAINER_F_LOOP 0x04U
#define SC_CONTAINER_F_BORDER 0x08U
#define SC_CONTAINER_F_BOUNCE 0x40U
#define SC_CONTAINER_F_ANIM 0x80U

#define SC_CONTAINER_F_SCROLL (SC_CONTAINER_F_H_SCROLL | SC_CONTAINER_F_V_SCROLL)
#define SC_CONTAINER_ANIM_GAIN_Q8 128U
/* 翻页吸附阈值（2^N 分之一页宽）：拖过 1/4 页即吸到下一页。1 = 1/2，2 = 1/4。 */
#define SC_CONTAINER_PAGE_BIAS_LOG2 2


typedef struct sc_container_t
{
    int16_t scroll_x;
    int16_t scroll_y;
    int16_t target_x;
    int16_t target_y;
    int16_t content_w;
    int16_t content_h;
    int16_t grid_w;
    int16_t grid_h;
    color_t border_color;
    sc_obj_cb_t cb; /* 滚动回调（SC_STYLE_CALLBACK） */
    uint8_t flags;
    uint8_t opa;
} sc_container_t;

#if SC_USER_OBJ && SC_WIDGET_CONTAINER_ENABLE
sc_obj_t *sc_container_create(sc_obj_t *parent, sc_container_t *props);
/* One shared transparent paging container; parent == NULL creates the root screen. */
sc_obj_t *sc_page_create(sc_obj_t *parent, sc_container_t *props,uint16_t w, uint16_t h, uint8_t page_count);
                             

void sc_container_set_grid(sc_obj_t *obj, int16_t grid_w, int16_t grid_h);
void sc_container_set_content(sc_obj_t *obj, int16_t content_w, int16_t content_h);

void sc_container_set_flags(sc_obj_t *obj, uint8_t flags);
/* 按方向翻页：dir=-1 上一页/左一页，dir=+1 下一页/右一页。返回是否触发翻页。 */
void sc_container_set_offset(sc_obj_t *obj, int16_t scroll_x, int16_t scroll_y, bool animated);
/* dir=0 复位到(0,0)，dir=-1 上一页/左一页，dir=+1 下一页/右一页；animated=true 动画 false 立即移动 */
bool sc_container_page_by_key(sc_obj_t *obj, int dir, bool animated);
bool sc_container_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_container_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
