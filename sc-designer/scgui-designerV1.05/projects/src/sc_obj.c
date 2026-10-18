#include "sc_gui.h"
#include "sc_draw.h"

#include "sc_mem.h"

#if SC_USER_OBJ

#if SC_OBJ_MAX < 1
#error SC_OBJ_MAX must be >= 1
#endif

#if SC_OBJ_MAX > 255
#error SC_OBJ_MAX must be <= 255 because object links use uint8_t index
#endif

#ifndef SC_HIT_STACK_MAX
#define SC_HIT_STACK_MAX 16U
#endif

/* Initialize the object pool as a free list linked through each slot's id field. */
void sc_obj_pool_init(void)
{
    sc_gui_t *gui = g_gui;
    sc_memset(gui->obj_pool, 0, sizeof(gui->obj_pool));
    for (uint8_t i = 0; i < SC_OBJ_MAX; i++)
    {
        gui->obj_pool[i].id = (uint8_t)(i + 1U);
    }
    gui->obj_pool[SC_OBJ_MAX - 1U].id = SC_OBJ_NIL;
    gui->obj_free_head = 0U;
}

/* Resolve a pool index to an object pointer, returning NULL for the nil index. */
sc_obj_t *sc_obj_at(uint8_t idx)
{
    if (idx == SC_OBJ_NIL)
    {
        return NULL;
    }
    return &g_gui->obj_pool[idx];
}

/* Allocate a zeroed object from the free list, or NULL if the pool is exhausted. */
static sc_obj_t *sc_obj_malloc(void)
{
    sc_gui_t *gui = g_gui;
    uint8_t idx = gui->obj_free_head;
    if (idx == SC_OBJ_NIL)
    {
        return NULL;
    }
    sc_obj_t *obj = &gui->obj_pool[idx];
    gui->obj_free_head = obj->id;
    sc_memset(obj, 0, sizeof(*obj));
    obj->id = idx;
    return obj;
}

/* Return an object to the free list, clearing any focus/touch references to it. */
static void sc_obj_free(sc_obj_t *obj)
{
    sc_gui_t *gui = g_gui;
    uint8_t idx = obj ? obj->id : SC_OBJ_NIL;
    if (idx == SC_OBJ_NIL)
    {
        return;
    }
    sc_focus_clear_obj(idx);
    sc_memset(obj, 0, sizeof(*obj));
    obj->id = gui->obj_free_head;
    gui->obj_free_head = idx;
}

/* Return the last node in the subtree rooted at obj (its deepest last descendant). */
static sc_obj_t *sc_obj_tree_tail(sc_obj_t *obj)
{
    uint8_t level = sc_obj_level(obj);
    while (obj->next && sc_obj_level(obj->next) > level)
    {
        obj = obj->next;
    }
    return obj;
}

/* Return the next sibling subtree, skipping over all of cur's descendants. */
sc_obj_t *sc_obj_next_tree(sc_obj_t *cur)
{
    if (!cur)
    {
        return NULL;
    }
    uint8_t level = sc_obj_level(cur);
    while (cur->next && sc_obj_level(cur->next) > level)
    {
        cur = cur->next;
    }
    return cur->next;
}

/* Find the node whose next link points at target, or NULL if none. */
sc_obj_t *sc_obj_prev(sc_obj_t *target)
{
    sc_obj_t *prev;
    sc_obj_t *cur;
    if (!target)
    {
        return NULL;
    }
    sc_obj_t *parent = sc_obj_parent(target);
    if (!parent)
    {
        prev = NULL;
        cur = sc_obj_at(sc_active_root(g_gui)->head);
    }
    else
    {
        prev = parent;
        cur = parent->next;
    }
    while (cur && cur != target && (!parent || sc_obj_level(cur) > sc_obj_level(parent)))
    {
        prev = cur;
        cur = cur->next;
    }
    return cur == target ? prev : NULL;
}

/* Insert obj into the root list, after the parent's subtree or at the list tail. */
static void sc_obj_tree_insert(sc_obj_t *parent, sc_obj_t *obj)
{
    sc_root_t *root = sc_active_root(g_gui);
    if (parent)
    {
        sc_obj_t *tail = sc_obj_tree_tail(parent);
        obj->next = tail->next;
        tail->next = obj;
        if (root->tail == tail->id)
        {
            root->tail = obj->id;
        }
        return;
    }
    if (root->tail == SC_OBJ_NIL)
    {
        root->head = obj->id;
    }
    else
    {
        sc_obj_at(root->tail)->next = obj;
    }
    root->tail = obj->id;
}

/* Clip an area against all ancestor views; returns false if fully clipped away. */
bool sc_obj_clip_parents(const sc_obj_t *obj, area_t *clip)
{
    sc_obj_t *parent;
    while ((parent = sc_obj_parent(obj)) != NULL)
    {
        if ((obj->flags & SC_OBJ_F_IGNORE_PARENT_CLIP) == 0U &&
            !rect_area_intersect(parent->view, *clip, clip))
        {
            return false;
        }
        obj = parent;
    }
    return true;
}

/* Compute an object's visible clip area from its view intersected with all parents. */
bool sc_obj_get_clip_area(const sc_obj_t *obj, area_t *out)
{
    *out = rect_to_area(obj->view);
    return sc_obj_clip_parents(obj, out);
}

/* Mark an object's clipped area dirty for redraw; skips objects already invalidated. */
bool sc_obj_invalidate(sc_obj_t *obj)
{
    /* Reject NULL / hidden / virtual / already-invalidated in one flag test, then
     * delegate to sc_obj_invalidate_area for clipping + dirty registration. */
    if (obj && (obj->flags & (SC_OBJ_F_HIDDEN | SC_OBJ_F_VIRTUAL | SC_OBJ_F_INVALIDATED)) == 0U)
    {
        return sc_obj_invalidate_area(obj, NULL);
    }
    return false;
}

// sc_obj_invalidate_area(obj, NULL) 替代sc_obj_invalidate(sc_obj_t *obj, true);
bool sc_obj_invalidate_area(sc_obj_t *obj, area_t *area)
{
    if (!obj || (obj->flags & (SC_OBJ_F_HIDDEN | SC_OBJ_F_VIRTUAL)) != 0U)
    {
        return false;
    }
    area_t temp = area ? *area : rect_to_area(obj->view);
    if (sc_obj_clip_parents(obj, &temp))
    {
        obj->flags |= SC_OBJ_F_INVALIDATED;
        return sc_invalidate_area(temp);
    }
    return false;
}
/* 平移对象及其所有子控件（扁平树遍历），并刷新旧、新两个区域。 */
void sc_obj_move(sc_obj_t *obj, int16_t dx, int16_t dy)
{
    if (!obj || (dx == 0 && dy == 0))
    {
        return;
    }
    sc_obj_invalidate_area(obj, NULL);
    obj->view.x = (int16_t)(obj->view.x + dx);
    obj->view.y = (int16_t)(obj->view.y + dy);
    /* 遍历移动所有子控件（扁平树结构，level 大于 obj 的都是后代）。 */
    uint8_t level = sc_obj_level(obj);
    for (sc_obj_t *cur = obj->next; cur && sc_obj_level(cur) > level; cur = cur->next)
    {
        cur->view.x = (int16_t)(cur->view.x + dx);
        cur->view.y = (int16_t)(cur->view.y + dy);
    }
    sc_obj_invalidate_area(obj, NULL);
}

/* Allocate and initialize a new object under an optional parent. */
sc_obj_t *sc_obj_create(sc_obj_t *parent, sc_obj_type_t type, const sc_obj_vt_t *vt, void *props)
{
    uint8_t level = parent ? (uint8_t)(sc_obj_level(parent) + 1U) : 0U;
    if (!vt || !props || type >= SC_OBJ_TYPE_MAX || level > SC_OBJ_LEVEL_MAX)
    {
        return NULL;
    }
    sc_obj_t *obj = sc_obj_malloc();
    if (!obj)
    {
        return NULL;
    }
    obj->props = props;
    obj->vt = vt;
    obj->parent = parent ? parent->id : SC_OBJ_NIL;
    obj->flags = SC_OBJ_F_INVALIDATED;
    obj->type_level = (uint8_t)((uint8_t)type | (uint8_t)(level << SC_OBJ_LEVEL_SHIFT));
    sc_obj_tree_insert(parent, obj);
    return obj;
}

/* Dispatch a draw event to the object's own event callback. */
void sc_obj_dispatch_draw_event(sc_obj_t *obj, const sc_event_t *event)
{
    obj->vt->event_cb(obj, event);
}
/* 公共按键/触摸按下-释放状态机（无拖拽控件共用）。
 * SYS_DRAW 由各控件自行处理；此处仅跟踪按压状态并调用用户回调。
 * cb 为空则不回调。 */
bool sc_obj_common_event(sc_obj_t *obj, const sc_event_t *event, sc_obj_cb_t cb)
{
    bool ret = false;
    if ((event->type == SC_EVENT_KEY_DOWN && event->dat.key == CMD_ENTER)
#if SC_TOUCH_ENABLE
        || event->type == SC_EVENT_TOUCH_DOWN
#endif
    )
    {
        if ((obj->flags & SC_OBJ_F_PRESSED) == 0U)
        {
            obj->flags |= SC_OBJ_F_PRESSED;
            sc_obj_invalidate(obj);
        }
        ret = true;
    }
#if SC_TOUCH_ENABLE
    else if (event->type == SC_EVENT_TOUCH_MOVE)
    {
        ret = false;
    }
#endif
    else if ((event->type == SC_EVENT_KEY_UP && event->dat.key == CMD_ENTER)
#if SC_TOUCH_ENABLE
             || event->type == SC_EVENT_TOUCH_UP
#endif
    )
    {
        if ((obj->flags & SC_OBJ_F_PRESSED) != 0U)
        {
            obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
            sc_obj_invalidate(obj);
        }
        ret = true;
    }
    if (cb)
    {
        cb(obj, event);
    }
    return ret;
}

/* Bubble an event from obj up through its parents until one handles it; returns the handler. */
sc_obj_t *sc_obj_dispatch_event(sc_obj_t *obj, const sc_event_t *event)
{
    while (obj)
    {
        if (obj->vt->event_cb(obj, event))
        {
            return obj;
        }
        obj = sc_obj_parent(obj);
    }
    return NULL;
}

/* Delete an object and its whole subtree, unlinking it and invalidating affected areas. */
void sc_obj_del(sc_obj_t *obj)
{
    sc_root_t *root = sc_active_root(g_gui);
    sc_obj_t *cur;
    if (!obj || !obj->props)
    {
        return;
    }
    sc_obj_t *prev = sc_obj_prev(obj);
    sc_obj_t *tail = sc_obj_tree_tail(obj);
    sc_obj_t *stop = tail->next;
    sc_obj_invalidate(obj);
    /* Normal descendants are covered by obj; overflow descendants need their own dirty area. */
    for (cur = obj->next; cur != stop; cur = cur->next)
    {
        if ((cur->flags & SC_OBJ_F_IGNORE_PARENT_CLIP) != 0U)
        {
            sc_obj_invalidate(cur);
        }
    }
    if (prev)
    {
        prev->next = stop;
    }
    else
    {
        root->head = stop ? stop->id : SC_OBJ_NIL;
    }
    if (root->tail == tail->id)
    {
        root->tail = prev ? prev->id : SC_OBJ_NIL;
    }
    cur = obj;
    while (cur != stop)
    {
        sc_obj_t *next = cur->next;
        sc_obj_free(cur);
        cur = next;
    }
}

/* Set an object's position relative to its parent and invalidate old and new areas. */
void sc_obj_set_pos(sc_obj_t *obj, int16_t x, int16_t y)
{
    if (obj)
    {
        sc_obj_t *parent = sc_obj_parent(obj);
        sc_obj_invalidate(obj);
        if (parent)
        {
            x += parent->view.x;
            y += parent->view.y;
        }
        obj->view.x = x;
        obj->view.y = y;
        sc_obj_invalidate_area(obj, NULL);
    }
}

/* Resize an object and invalidate its old and new areas if the size changed. */
void sc_obj_set_size(sc_obj_t *obj, uint16_t w, uint16_t h)
{
    if (obj && (obj->view.w != w || obj->view.h != h))
    {
        sc_obj_invalidate(obj);
        obj->view.w = w;
        obj->view.h = h;
        sc_obj_invalidate_area(obj, NULL);
    }
}

/* Set an object's full rect and apply alignment offsets relative to its parent (or screen). */
void sc_obj_set_rect(sc_obj_t *obj, int16_t x, int16_t y, uint16_t w, uint16_t h, sc_align_t parent_align)
{
    if (!obj)
    {
        return;
    }
    sc_obj_t *parent = sc_obj_parent(obj);
    int16_t ref_x, ref_y;
    int32_t ref_w, ref_h;
    if (parent)
    {
        ref_x = parent->view.x;
        ref_y = parent->view.y;
        ref_w = parent->view.w;
        ref_h = parent->view.h;
    }
    else
    {
        ref_x = 0;
        ref_y = 0;
        ref_w = g_gui->screen.w;
        ref_h = g_gui->screen.h;
    }
    sc_obj_invalidate(obj);
    obj->view.x = (int16_t)(x + ref_x + sc_align_offset_x(ref_w, w, parent_align));
    obj->view.y = (int16_t)(y + ref_y + sc_align_offset_y(ref_h, h, parent_align));
    obj->view.w = w;
    obj->view.h = h;
    sc_obj_invalidate_area(obj, NULL);
}

/* Toggle an object's hidden flag and invalidate its area when visibility changes.
 * 与 sc_obj_del 同口径：显隐作用于整棵子树——容器隐藏时子节点一并隐藏，
 * 避免子节点残留绘制/命中/聚焦；显示时整棵子树恢复。 */
void sc_obj_set_hidden(sc_obj_t *obj, bool hidden)
{
    sc_obj_t *cur;
    if (!obj)
    {
        return;
    }
    sc_obj_t *tail = sc_obj_tree_tail(obj);
    sc_obj_t *stop = tail->next;
    for (cur = obj; cur != stop; cur = cur->next)
    {
        uint8_t flags = hidden ? (uint8_t)(cur->flags | SC_OBJ_F_HIDDEN) : (uint8_t)(cur->flags & (uint8_t)~SC_OBJ_F_HIDDEN);
        if (cur->flags == flags)
        {
            continue;
        }
        sc_obj_invalidate(cur);
        cur->flags = flags;
        if (!hidden)
        {
            sc_obj_invalidate_area(cur, NULL);
        }
    }
}

#if SC_TOUCH_ENABLE
/* Return the topmost visible object whose clipped area contains the point. */
static sc_obj_t *sc_obj_hit_test(int x, int y)
{
    sc_obj_t *hits[SC_HIT_STACK_MAX];
    uint8_t hit_cnt = 0U;
    sc_obj_t *obj = sc_obj_at(sc_active_root(g_gui)->head);
    while (obj)
    {
        if ((obj->flags & SC_OBJ_F_HIDDEN) == 0U && hit_cnt < SC_HIT_STACK_MAX &&
            area_contains_point(rect_to_area(obj->view), x, y))
        {
            hits[hit_cnt++] = obj;
        }
        obj = obj->next;
    }
    sc_obj_t *focus = NULL;
    while (hit_cnt != 0U)
    {
        area_t clip;
        obj = hits[--hit_cnt];
        if ((obj->flags & SC_OBJ_F_FOCUSABLE) && sc_obj_get_clip_area(obj, &clip) && area_contains_point(clip, x, y))
        {
            if (obj->flags & SC_OBJ_F_TOP) // 高优先级
            {
                return obj;
            }
            if (!focus)
            {
                focus = obj; // 低优先级
            }
        }
    }
    return focus;
}

/* Route touch down/move/up events to the captured object, handling drag hand-off. */
static bool sc_obj_handle_touch_event(const sc_event_t *event)
{
    sc_gui_t *gui = g_gui;
    sc_obj_t *obj;
    bool handled;
    switch (event->type)
    {
    case SC_EVENT_TOUCH_DOWN:
        gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        gui->drag_offset_x = event->dat.pos[0];
        gui->drag_offset_y = event->dat.pos[1];
        gui->last_pos_x = event->dat.pos[0];
        gui->last_pos_y = event->dat.pos[1];
        if ((gui->flags & SC_RT_MSGBOX_ACTIVE) != 0U)
        {
            obj = sc_focus_current();
        }
        else
        {
            /* Rectangle/clip is only a coarse hit; the widget validates its private shape before capture. */
            obj = sc_obj_hit_test(event->dat.pos[0], event->dat.pos[1]);
        }
        obj = obj ? sc_obj_dispatch_event(obj, event) : NULL;
        sc_focus_touch_set(obj);
        return obj != NULL;
    case SC_EVENT_TOUCH_MOVE:
    {
        sc_obj_t *touch = sc_focus_current();
        /* 控件在事件中用 drag_offset(起点)算总位移、用 last_pos(上一帧)算帧位移；
         * 方向锁定由控件自己处理，顶层不干预。 */
        obj = touch ? sc_obj_dispatch_event(touch, event) : NULL;
        if (obj && (gui->flags & SC_RT_DRAGGING) != 0U && obj != touch)
        {
            if ((touch->flags & SC_OBJ_F_PRESSED) != 0U)
            {
                touch->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
                sc_obj_invalidate(touch);
            }
            gui->focus_stack[gui->focus_depth] = obj ? obj->id : SC_OBJ_NIL;
        }
        /* 控件处理完后更新上一帧位置，作为下一帧帧位移的参考 */
        gui->last_pos_x = event->dat.pos[0];
        gui->last_pos_y = event->dat.pos[1];
        return obj != NULL;
    }
    case SC_EVENT_TOUCH_UP:
        obj = sc_focus_current();
        handled = obj && sc_obj_dispatch_event(obj, event);
        gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        return handled;
    default:
        return false;
    }
}
#endif

/* Direction keys go through sc_obj_focus_root (unified spatial navigation);
 * ENTER descends a generation (push), BACK ascends (pop). Boundary events are
 * not forwarded to root_callback. */
static bool sc_root_dispatch_event(const sc_event_t *event)
{
    /* 方向键统一走 sc_obj_focus_root：内部模式在栈顶容器子树内导航，
     * 全局模式在活动根第一层（兄弟或儿子）导航；焦点移动则消费事件，
     * 到边界返回 false、事件不再下传 root_callback。
     * ENTER 压栈进入下一辈，BACK 弹栈回上一辈（焦点回退优先、被消费后不下传）。 */
    if (event->type == SC_EVENT_KEY_DOWN)
    {
        sc_obj_t *focus = sc_focus_current();
        if (event->dat.key == CMD_LEFT || event->dat.key == CMD_RIGHT ||
            event->dat.key == CMD_UP || event->dat.key == CMD_DOWN)
        {
            int key = event->dat.key;
            if (sc_obj_focus_root(focus, key))
            {
                return true; /* 焦点已移动，导航消费 */
            }
        }
        if (event->dat.key == CMD_ENTER)
        {
            sc_focus_push(focus);
            if (sc_obj_focus_root(focus, CMD_RIGHT))
            {
                return true; /* 焦点已移动，导航消费 */
            }
        }
        if (event->dat.key == CMD_BACK)
        {
            sc_focus_pop(); /* 弹栈一层：返回上级，退出容器内部 */
            return true;
        }
        return false;
    }
    if (g_gui->root_callback)
    {
        g_gui->root_callback(NULL, event);
        return true;
    }
    return false;
}

/* Top-level event entry point: route key and touch events to focus/touch targets then the root. */
bool sc_obj_handle_event(const sc_event_t *event)
{
    sc_obj_t *obj;
    if (!event)
    {
        return false;
    }
    switch (event->type)
    {
    case SC_EVENT_KEY_DOWN:
    case SC_EVENT_KEY_UP:
        obj = sc_focus_current();
        if (sc_obj_is_focusable(obj) && sc_obj_dispatch_event(obj, event))
        {
            return true;
        }
        return sc_root_dispatch_event(event);
#if SC_TOUCH_ENABLE
    case SC_EVENT_TOUCH_DOWN:
    case SC_EVENT_TOUCH_MOVE:
    case SC_EVENT_TOUCH_UP:
        if (sc_obj_handle_touch_event(event))
        {
            return true;
        }
        return sc_root_dispatch_event(event);
#endif
    case SC_EVENT_NONE:
    default:
        return false;
    }
}

#endif /* SC_USER_OBJ */
