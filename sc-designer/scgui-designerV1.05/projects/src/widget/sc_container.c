#include "../sc_focus.h"
#include "../sc_draw.h"
#include "sc_container.h"

#if SC_WIDGET_CONTAINER_ENABLE

#if SC_USER_OBJ && SC_WIDGET_CONTAINER_ENABLE

#define SC_CONTAINER_CONTENT_W(obj_, props_) \
    ((props_)->content_w > (int16_t)(obj_)->view.w ? (props_)->content_w : (int16_t)(obj_)->view.w)
#define SC_CONTAINER_CONTENT_H(obj_, props_) \
    ((props_)->content_h > (int16_t)(obj_)->view.h ? (props_)->content_h : (int16_t)(obj_)->view.h)
#define SC_CONTAINER_MIN_X(obj_, props_) \
    ((int16_t)((obj_)->view.w - SC_CONTAINER_CONTENT_W(obj_, props_)))
#define SC_CONTAINER_MIN_Y(obj_, props_) \
    ((int16_t)((obj_)->view.h - SC_CONTAINER_CONTENT_H(obj_, props_)))

#define SC_CONTAINER_ANIM_MS 10U

/* 就近翻页：从手势起点 start 出发，沿位移方向 |offset - start| 超过 1/4 页宽
 * （SC_CONTAINER_PAGE_BIAS_LOG2）即翻一页 —— 左右滑动阈值对称。
 * 按键路径 delta = ±grid 恒超阈值，等价于逐页移动。 */
static int16_t sc_container_snap_page(int16_t offset, int16_t start, int16_t grid)
{
    if (grid <= 0)
    {
        return offset;
    }
    int16_t threshold = (int16_t)(grid >> SC_CONTAINER_PAGE_BIAS_LOG2);
    int16_t delta = (int16_t)(offset - start);
    if (delta <= -threshold)
    {
        return (int16_t)(start - grid);
    }
    if (delta >= threshold)
    {
        return (int16_t)(start + grid);
    }
    return start;
}

/* Clamp a horizontal scroll offset to the content bounds unless looping. */
static int16_t sc_container_clamp_x(const sc_obj_t *obj, const sc_container_t *props, int16_t scroll_x)
{
    if ((props->flags & SC_CONTAINER_F_LOOP) != 0U)
    {
        return scroll_x;
    }
    return SC_CLAMP(scroll_x, SC_CONTAINER_MIN_X(obj, props), 0);
}

/* Clamp a vertical scroll offset to the content bounds unless looping. */
static int16_t sc_container_clamp_y(const sc_obj_t *obj, const sc_container_t *props, int16_t scroll_y)
{
    if ((props->flags & SC_CONTAINER_F_LOOP) != 0U)
    {
        return scroll_y;
    }
    return SC_CLAMP(scroll_y, SC_CONTAINER_MIN_Y(obj, props), 0);
}

/* Fill the container background and draw its border when enabled. */
static void sc_draw_container(sc_pfb_t *pfb, const sc_obj_t *obj, const sc_container_t *props)
{
    sc_draw_fill_rect_opa(pfb, obj->view.x, obj->view.y, obj->view.w, obj->view.h,
                          props->border_color, props->opa);
    if ((props->flags & SC_CONTAINER_F_BORDER) != 0U)
    {
        sc_draw_rect(pfb, obj->view.x, obj->view.y, obj->view.w, obj->view.h, props->border_color, 1);
    }
}

/* Scroll all descendants by (dx, dy), wrapping items around in loop mode and
 * invalidating the affected areas. */
static void sc_container_move_children(sc_obj_t *obj, int16_t dx, int16_t dy)
{
    sc_container_t *props = (sc_container_t *)obj->props;
    bool invalidate_container = (obj->flags & SC_OBJ_F_VIRTUAL) == 0U;
    if (dx == 0 && dy == 0)
    {
        return;
    }
    bool cycle = (props->flags & SC_CONTAINER_F_LOOP) != 0U;
    int16_t cycle_w = cycle && (props->flags & SC_CONTAINER_F_H_SCROLL) != 0U ? SC_CONTAINER_CONTENT_W(obj, props) : 0;
    int16_t cycle_h = cycle && (props->flags & SC_CONTAINER_F_V_SCROLL) != 0U ? SC_CONTAINER_CONTENT_H(obj, props) : 0;
    int16_t left_x = (int16_t)(obj->view.x - obj->view.w);
    int16_t top_y = (int16_t)(obj->view.y - obj->view.h);
    int16_t right_x = (int16_t)(obj->view.x + cycle_w - obj->view.w);
    int16_t bottom_y = (int16_t)(obj->view.y + cycle_h - obj->view.h);

    /* A normal container is the dirty viewport for all clipped descendants. */
    if (invalidate_container)
    {
        sc_obj_invalidate_area(obj, NULL);
    }
    props->scroll_x += dx;
    props->scroll_y += dy;
    /* Hidden descendants still move; their coordinates must stay current when shown. */
    for (sc_obj_t *child = obj->next; child && sc_obj_level(child) > sc_obj_level(obj); child = child->next)
    {
        int16_t move_x = dx;
        int16_t move_y = dy;
        int16_t new_x = (int16_t)(child->view.x + dx);
        int16_t new_y = (int16_t)(child->view.y + dy);
        if (cycle_w > 0)
        {
            if (dx < 0 && (int16_t)(new_x + child->view.w) <= left_x)
            {
                move_x = (int16_t)(move_x + cycle_w);
            }
            else if (dx > 0 && new_x >= right_x)
            {
                move_x = (int16_t)(move_x - cycle_w);
            }
        }
        if (cycle_h > 0)
        {
            if (dy < 0 && (int16_t)(new_y + child->view.h) <= top_y)
            {
                move_y = (int16_t)(move_y + cycle_h);
            }
            else if (dy > 0 && new_y >= bottom_y)
            {
                move_y = (int16_t)(move_y - cycle_h);
            }
        }
        if (move_x != 0 || move_y != 0)
        {
            /* Keep old/new child areas for content that escapes the viewport. */
            bool child_invalidate = !invalidate_container || (child->flags & SC_OBJ_F_IGNORE_PARENT_CLIP) != 0U;
            if (child_invalidate)
            {
                sc_obj_invalidate_area(child, NULL);
            }
            child->view.x = (int16_t)(child->view.x + move_x);
            child->view.y = (int16_t)(child->view.y + move_y);
            if (child_invalidate)
            {
                sc_obj_invalidate_area(child, NULL);
            }
        }
    }
}

/* Animation tick: ease the scroll offset toward the target, normalizing the
 * offset and stopping once the target is reached. */
static void sc_container_anim_timer(sc_delay_t *timer)
{
    sc_obj_t *obj = (sc_obj_t *)timer->arg;
    sc_container_t *props = (sc_container_t *)obj->props;
    int err_x = props->target_x - props->scroll_x;
    int err_y = props->target_y - props->scroll_y;
    int16_t dx = 0;
    int16_t dy = 0;
    if (err_x == 0 && err_y == 0)
    {
        if ((props->flags & SC_CONTAINER_F_LOOP) != 0U)
        {
            if ((props->flags & SC_CONTAINER_F_H_SCROLL) != 0U)
            {
                props->scroll_x %= SC_CONTAINER_CONTENT_W(obj, props);
            }
            if ((props->flags & SC_CONTAINER_F_V_SCROLL) != 0U)
            {
                props->scroll_y %= SC_CONTAINER_CONTENT_H(obj, props);
            }
            props->target_x = props->scroll_x;
            props->target_y = props->scroll_y;
        }
        props->flags &= (uint8_t)~SC_CONTAINER_F_ANIM;
        /* 翻页动画完成：保持内部模式和原焦点，用户再按方向键时用新位置定位 */
        return;
    }
    if (err_x != 0)
    {
        dx = SC_ABS(err_x) <= 2 ? (int16_t)err_x : (int16_t)(SC_MAX((SC_ABS(err_x) * SC_CONTAINER_ANIM_GAIN_Q8) >> 8, 1) * ((err_x > 0) - (err_x < 0)));
    }
    if (err_y != 0)
    {
        dy = SC_ABS(err_y) <= 2 ? (int16_t)err_y : (int16_t)(SC_MAX((SC_ABS(err_y) * SC_CONTAINER_ANIM_GAIN_Q8) >> 8, 1) * ((err_y > 0) - (err_y < 0)));
    }
    sc_container_move_children(obj, dx, dy);
    sc_reload_delay(timer, SC_CONTAINER_ANIM_MS);
}

/* Flag the container as animating and schedule the scroll animation timer. */
static void sc_container_start_anim(sc_obj_t *obj)
{
    sc_container_t *props = (sc_container_t *)obj->props;
    props->flags |= SC_CONTAINER_F_ANIM;
    sc_add_delay(sc_container_anim_timer, SC_CONTAINER_ANIM_MS, obj);
}

/* Scroll the container to a clamped target offset, animating or moving now. */
void sc_container_set_offset(sc_obj_t *obj, int16_t scroll_x, int16_t scroll_y, bool animated)
{
    if (!obj || !sc_obj_is_container(obj))
    {
        return;
    }
    sc_container_t *props = (sc_container_t *)obj->props;
    scroll_x = sc_container_clamp_x(obj, props, scroll_x);
    scroll_y = sc_container_clamp_y(obj, props, scroll_y);
    props->target_x = scroll_x;
    props->target_y = scroll_y;
    if (animated)
    {
        sc_container_start_anim(obj);
    }
    else
    {
        sc_container_move_children(obj, (int16_t)(scroll_x - props->scroll_x), (int16_t)(scroll_y - props->scroll_y));
    }
}

/* 按方向翻页：dir=0 复位到(0,0)，dir=-1 上一页/左一页，dir=+1 下一页/右一页。
 * animated=true 启动动画，false 立即移动。返回是否触发翻页/复位。 */
bool sc_container_page_by_key(sc_obj_t *obj, int dir, bool animated)
{
    if (!obj || !sc_obj_is_container(obj))
    {
        return false;
    }
    sc_container_t *props = (sc_container_t *)obj->props;
    if ((props->flags & SC_CONTAINER_F_ANIM) != 0U)
    {
        return true;
    }
    int16_t target_x = props->target_x;
    int16_t target_y = props->target_y;
    if (dir == 0)
    {
        target_x = 0;
        target_y = 0;
    }
    else
    {
        if ((props->flags & SC_CONTAINER_F_V_SCROLL) != 0U)
        {
            target_y = (int16_t)(props->scroll_y - dir * props->grid_h);
        }
        if ((props->flags & SC_CONTAINER_F_H_SCROLL) != 0U)
        {
            target_x = (int16_t)(props->scroll_x - dir * props->grid_w);
        }
    }
    if ((props->flags & SC_CONTAINER_F_LOOP) == 0U)
    {
        target_x = sc_container_clamp_x(obj, props,
                     sc_container_snap_page(target_x, props->scroll_x, props->grid_w));
        target_y = sc_container_clamp_y(obj, props,
                     sc_container_snap_page(target_y, props->scroll_y, props->grid_h));
    }
    if (target_x != props->target_x || target_y != props->target_y)
    {
        int16_t dx = (int16_t)(target_x - props->scroll_x);
        int16_t dy = (int16_t)(target_y - props->scroll_y);
        props->target_x = target_x;
        props->target_y = target_y;
        if (animated)
        {
            sc_container_start_anim(obj);
        }
        else
        {
            sc_container_move_children(obj, dx, dy);
            props->scroll_x = target_x;
            props->scroll_y = target_y;
        }
        return true;
    }
    return false;
}

/* Create a container object initialized with default scroll/border style. */
static const sc_obj_vt_t sc_container_vt = {
    .event_cb = sc_obj_handle_container_event,
    .set_style = sc_container_style,
};


sc_obj_t *sc_container_create(sc_obj_t *parent, sc_container_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_CONTAINER, &sc_container_vt, props);
    if (obj)
    {
        *props = (sc_container_t){
            .border_color = C_WHITE,
            /* 注意：FOCUSABLE(0x04) 与 SC_CONTAINER_F_LOOP(0x04) 位冲突，且 props->flags
             * 是容器标志而非对象标志 —— 对象级 FOCUSABLE 走下方 obj->flags。 */
            .flags = SC_CONTAINER_F_SCROLL | SC_CONTAINER_F_BORDER,
            .opa = 128U,
        };
        /* 命中测试只响应 FOCUSABLE：容器/页面根（sc_page_create 走这里）默认可聚焦，
         * 才能接收触摸/按键（滚动翻页）。需去掉时用 sc_obj_remove_flag。 */
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

/* Create a horizontally paged container of page_count pages sized w x h,
 * enabling bounce and looping (for multiple pages). */
sc_obj_t *sc_page_create(sc_obj_t *parent, sc_container_t *props,
                         uint16_t w, uint16_t h, uint8_t page_count)
{
    sc_obj_t *obj = sc_container_create(parent, props);
    if (obj)
    {
        props->grid_w = (int16_t)w;
        props->grid_h = (int16_t)h;
        props->content_w = (int16_t)(page_count * w);
        props->content_h = (int16_t)h;
        props->flags = SC_CONTAINER_F_H_SCROLL | SC_CONTAINER_F_BOUNCE;
        if (page_count > 1U)
        {
            props->flags |= SC_CONTAINER_F_LOOP;
        }
        props->opa = 0U;
        obj->flags |= SC_OBJ_F_VIRTUAL|SC_OBJ_F_FOCUSABLE;
        obj->type_level  &= (~SC_OBJ_TYPE_MASK) ;  //SC_OBJ_ROOT
        sc_obj_set_rect(obj, 0, 0, w, h, ALIGN_NONE);
    }
    return obj;
}

/* Set the snap grid size used when aligning scroll offsets. */
void sc_container_set_grid(sc_obj_t *obj, int16_t grid_w, int16_t grid_h)
{
    sc_container_t *props = (sc_container_t *)obj->props;
    props->grid_w = grid_w;
    props->grid_h = grid_h;
}

/* Set the scrollable content dimensions of the container. */
void sc_container_set_content(sc_obj_t *obj, int16_t content_w, int16_t content_h)
{
    sc_container_t *props = (sc_container_t *)obj->props;
    props->content_w = content_w;
    props->content_h = content_h;
}

/* Replace the container's flags while preserving its current animating state. */
void sc_container_set_flags(sc_obj_t *obj, uint8_t flags)
{
    sc_container_t *props = (sc_container_t *)obj->props;
    props->flags = (uint8_t)((props->flags & SC_CONTAINER_F_ANIM) |
                             (flags & (uint8_t)~SC_CONTAINER_F_ANIM));
}

/* Apply a style property (border color, opacity, scroll callback) to the container. */
bool sc_container_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_container_t *props = (sc_container_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_BG:
    case SC_STYLE_BORDER:
        props->border_color = *(const color_t *)value;
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

/* Handle container events: draw, touch drag/bounce scrolling. Key handling lives in
 * sc_root_dispatch_event (sc_obj_focus_root focus-stack flow). */
bool sc_obj_handle_container_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_container_t *props = (sc_container_t *)obj->props;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_container((sc_pfb_t *)event->dat.arg, obj, props);
        return true;
    }
    bool ret = false;
#if SC_TOUCH_ENABLE
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        props->target_x = props->scroll_x;
        props->target_y = props->scroll_y;
        props->flags &= (uint8_t)~SC_CONTAINER_F_ANIM;
        g_gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        ret = true;
    }
    else if (event->type == SC_EVENT_TOUCH_MOVE)
    {
        /* 帧位移模式（对齐 4d033e5）：dx/dy 在 H_SCROLL/V_SCROLL 内部算，
         * 没开启的轴不算位移。越界时对帧位移 >>3 阻尼，直接移动子对象。
         * 方向锁定基于总位移(从起点 drag_offset 算)主方向，锁另一轴。 */
        bool loop = (props->flags & SC_CONTAINER_F_LOOP) != 0U;
        /* 方向锁定：基于总位移主方向（drag_offset 是按下起点，顶层不更新） */
        int16_t total_dx = (int16_t)(event->dat.pos[0] - g_gui->drag_offset_x);
        int16_t total_dy = (int16_t)(event->dat.pos[1] - g_gui->drag_offset_y);
        int16_t abs_x = (total_dx < 0) ? (int16_t)(-total_dx) : total_dx;
        int16_t abs_y = (total_dy < 0) ? (int16_t)(-total_dy) : total_dy;
        bool lock_x = abs_y > abs_x;  /* 纵向为主：锁横向 */
        bool lock_y = abs_x >= abs_y; /* 横向为主：锁纵向 */
        int16_t dx = 0;
        int16_t dy = 0;
        if ((props->flags & SC_CONTAINER_F_H_SCROLL) != 0U && !lock_x)
        {
            dx = (int16_t)(event->dat.pos[0] - g_gui->last_pos_x);
            /* 越界阻尼：已越界且继续向越界方向拖动时，对帧位移 >>3 */
            if (dx != 0 && !loop)
            {
                int16_t min_x = SC_CONTAINER_MIN_X(obj, props);
                if (((dx > 0 && props->scroll_x > 0) ||
                     (dx < 0 && props->scroll_x < min_x)))
                {
                    dx = (int16_t)(dx >> 3);
                }
            }
        }
        if ((props->flags & SC_CONTAINER_F_V_SCROLL) != 0U && !lock_y)
        {
            dy = (int16_t)(event->dat.pos[1] - g_gui->last_pos_y);
            if (dy != 0 && !loop)
            {
                int16_t min_y = SC_CONTAINER_MIN_Y(obj, props);
                if (((dy > 0 && props->scroll_y > 0) ||
                     (dy < 0 && props->scroll_y < min_y)))
                {
                    dy = (int16_t)(dy >> 3);
                }
            }
        }
        if (dx != 0 || dy != 0)
        {
            g_gui->flags |= SC_RT_DRAGGING;
            sc_container_move_children(obj, dx, dy);
            ret = true;
        }
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        if ((props->flags & SC_CONTAINER_F_BOUNCE) != 0U)
        {
            /* 触摸释放：target_x/y 是 TOUCH_DOWN 记下的手势起点，按位移方向就近翻页。 */
            props->target_x = sc_container_snap_page(props->scroll_x, props->target_x, props->grid_w);
            props->target_y = sc_container_snap_page(props->scroll_y, props->target_y, props->grid_h);
            props->target_x = sc_container_clamp_x(obj, props, props->target_x);
            props->target_y = sc_container_clamp_y(obj, props, props->target_y);
            sc_container_start_anim(obj);
        }
        g_gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        ret = true;
    }
#endif
    /* 滚动回调：内置按键/触摸处理后统一通知（SC_STYLE_CALLBACK）。 */
    if (props->cb)
    {
        props->cb(obj, event);
    }
    return ret;
}

#endif /* SC_USER_OBJ && SC_WIDGET_CONTAINER_ENABLE */

#endif /* SC_WIDGET_CONTAINER_ENABLE */
