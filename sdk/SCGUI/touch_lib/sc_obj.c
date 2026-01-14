#include "sc_obj.h"

sc_touch_ctx g_touch =
{
        .srceen = NULL,
        .obj_focus = NULL,
        .anim_cb = NULL,
        .last_x = 0,
        .last_y = 0,
        .touch_state = 0,
};

/// 标记控件为脏矩形
void sc_obj_dirty(sc_obj_t *obj, sc_rect_t *last)
{
    if (obj == NULL || obj->attr & SC_OBJ_ATTR_VIRTUAL)
        return;
    if (last == NULL && (obj->flag & SC_OBJ_FLAG_ACTIVE)) // 无上次位置，且未标记过
    {
        return;
    }
    sc_rect_t rect = last ? sc_rect_merge(&obj->rect, last) : obj->rect;   // 合并上次位置
    sc_rect_t *parent = (obj->parent != NULL) ? &obj->parent->rect : NULL; // 获取父节点
    sc_dirty_mark(parent, &rect);
    obj->flag |= SC_OBJ_FLAG_ACTIVE; // 标记刷新
}

/// 移动控件及其子控件位置
void sc_obj_move_pos(sc_obj_t *obj, int move_x, int move_y)
{
    if (move_x == 0 && move_y == 0)
        return;
    sc_obj_t *child = obj;
    while (IS_CHILD_OR_SELF(obj, child))
    {
        sc_rect_t last = child->rect;
        child->rect.x += move_x; // 相对坐标
        child->rect.y += move_y;
        sc_obj_dirty(child, &last); // 标记脏矩形
        child = child->next;
    }
}

/// 设置焦点控件
void sc_obj_set_focus(sc_obj_t *obj)
{
    if(g_touch.touch_state == SC_TOUCH_STATE_IDLE) // 如果当前焦点释放再切换
    {
        g_touch.obj_focus = obj;
    };
}

// ui事件分发
void sc_handle_event(sc_event_t *e)
{
    switch (e->type)
    {
    case SC_EVENT_TOUCH_DOWN:  // 检查该位置是否点击在了物体上
        if (g_touch.touch_state != SC_TOUCH_STATE_IDLE)
            break;
        sc_obj_t *current = g_touch.srceen;
        while (current != NULL)
        {
            // 广度搜索不相交+隐藏节点跳过
            if ((current->attr & SC_OBJ_ATTR_HIDDEN) || sc_rect_touch_ctx(&current->rect, e->dat.pos[0], e->dat.pos[1]) == 0)
            {
                current = sc_obj_next_tree(current);
                continue;
            }
            if (current->handle_event && current->handle_event(current, e)) // 执行虚表菜单
            {
                g_touch.obj_focus = current;
            }
            current = current->next;
        }
        if (g_touch.obj_focus)
        {
            g_touch.touch_state |= SC_TOUCH_STATE_DOWN; // 按下
            //-------------分发cmd事件---------------
            sc_event_t evt;
            evt.type = SC_EVENT_TYPE_CMD;
            evt.dat.cmd = CMD_ENTER;
            g_touch.obj_focus->handle_event(g_touch.obj_focus, &evt);
        }
        g_touch.last_x = e->dat.pos[0];
        g_touch.last_y = e->dat.pos[1];
        break;
    case SC_EVENT_TOUCH_MOVE:
        if (g_touch.touch_state & SC_TOUCH_STATE_DOWN) // 按下了可以移动动
        {
            g_touch.touch_state |= SC_TOUCH_STATE_MOVE;
            g_touch.obj_focus->handle_event(g_touch.obj_focus, e);
        }
        g_touch.last_x = e->dat.pos[0];
        g_touch.last_y = e->dat.pos[1];
        break;
    case SC_EVENT_TOUCH_UP:
        if (g_touch.touch_state & SC_TOUCH_STATE_DOWN) // 释放事件
        {
            g_touch.obj_focus->handle_event(g_touch.obj_focus, e);
            g_touch.touch_state &= ~(SC_TOUCH_STATE_MOVE | SC_TOUCH_STATE_DOWN);
        }
        break;
    default:
        if (g_touch.obj_focus) // 如果有焦点物体
        {
            g_touch.obj_focus->handle_event(g_touch.obj_focus, e);
        }
        break;
    }
}

bool sc_screen_loop(void)
{
    if (g_touch.anim_cb)
    {
        g_touch.anim_cb(&g_touch);     //触摸动画回调
    }
    uint8_t dirty_cnt = 0;
    sc_dirty_t *p_dirty = sc_dirty_merge_out(&dirty_cnt);
    if (dirty_cnt == 0)
        return  false; // 无脏矩形
#if SC_DIRTY_BUCKET_COPY
    /*-------------备份脏矩形,支持绘制使用sc_drity_mark()------*/
    sc_dirty_t temp_dirty[dirty_cnt];
    for (uint8_t n = 0; n < dirty_cnt; n++)
    {
        temp_dirty[n] = p_dirty[n];
    }
    p_dirty = temp_dirty;
#endif
    // -----------------刷新控件--------------------------------
    sc_obj_t *intersect_obj[SC_DIRTY_BUCKET_N]; // 要绘制的节点
    sc_pfb_t pfb;
    for (uint8_t i = 0; i < dirty_cnt; ++i)
    {
        sc_rect_t draw_rect = sc_dirty_to_rect(&p_dirty[i]); // 脏矩形转矩形
        uint16_t intersect_cnt = 0;                          // 相交控件
        sc_obj_t *current = (sc_obj_t *)g_touch.srceen;
        while (current != NULL)
        {
            // 广度搜索不相交+隐藏节点跳过
            if (sc_rect_nor_intersect(&current->rect, &draw_rect) || (current->attr & SC_OBJ_ATTR_HIDDEN))
            {
                current = sc_obj_next_tree(current);
            }
            else
            {
                if (intersect_cnt < SC_DIRTY_BUCKET_N)
                {
                    current->flag &= ~SC_OBJ_FLAG_ACTIVE;     // 清除刷新标记
                    intersect_obj[intersect_cnt++] = current; // 要绘制的节点
                }
                current = current->next;
            }
        }
        // -----------------刷新控件--------------------------------
        sc_pfb_init_slices(&pfb, &draw_rect, gui->bkc);
        do
        {
            for (int i = 0; i < intersect_cnt; i++)
            {
                sc_obj_t *obj = intersect_obj[i];
                if (obj->handle_draw)
                {
                    obj->handle_draw(obj, &pfb);
                }
            }
        } while (sc_pfb_next_slice(&pfb)); // 分帧刷新
    }
    return  true; 
}
/// @brief 演示代码
void demo_init(void)
{
    sc_obj_t *srceen = sc_create_srceen(NULL, 0, 0, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, ALIGN_CENTER);
    sc_obj_t *canvas = sc_create_canvas(srceen, 0, 0, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, SC_SCREEN_WIDTH * 2, SC_SCREEN_HEIGHT * 2, ALIGN_CENTER);
    sc_obj_t *rect1 = sc_create_rect(canvas, 0, 0, 300, 200, ALIGN_CENTER);
    sc_obj_t *rect2 = sc_create_rect(canvas, 320, 0, 300, 200, ALIGN_CENTER);
    sc_obj_t *rect4 = sc_create_rect(srceen, 0, 100, 50, 50, ALIGN_CENTER);
    sc_set_color(rect1, C_RED, gui->bkc);
    sc_set_color(rect2, C_GREEN, gui->bkc);
    sc_obj_list_print(srceen);
    g_touch.srceen = srceen;
}