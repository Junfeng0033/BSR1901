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
/// 设置焦点控件
// void sc_obj_set_focus(sc_obj_t *obj)
// {
//     if (g_touch.touch_state == SC_TOUCH_STATE_IDLE) // 如果当前焦点释放再切换
//     {
//         g_touch.obj_focus = obj;
//     };
// }

// 事件分发
void sc_touch_event(sc_event_t *e)
{
    if (g_touch.srceen == NULL)
    {
        return;
    }
    switch (e->type)
    {
    case SC_EVENT_TOUCH_DOWN: // 检查该位置是否点击在了物体上
        if (g_touch.touch_state != SC_TOUCH_STATE_IDLE)
            break;
        sc_obj_t *focus = NULL;
        sc_obj_t *current = g_touch.srceen;
        while (current != NULL)
        {
            // 广度搜索不相交+隐藏节点跳过
            if ((current->attr & SC_OBJ_ATTR_HIDDEN) || sc_rect_touch_ctx(&current->rect, e->dat.pos[0], e->dat.pos[1]) == 0)
            {
                current = current->next;
                continue;
            }
            if (current->handle_event(current, e)) // 执行虚表菜单
            {
                focus = current;
            }
            current = current->next;
        }
        if (focus)
        {
            g_touch.obj_focus = focus;
            g_touch.touch_state |= SC_TOUCH_STATE_DOWN; // 按下
            e->type = SC_EVENT_TYPE_FOCUS;              // 分发焦点事件
            g_touch.obj_focus->handle_event(g_touch.obj_focus, e);
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

/// @brief 触摸库主循环
void sc_touch_loop(void)
{
    if (g_touch.srceen == NULL)
        return;
    if (g_touch.anim_cb)
    {
        g_touch.anim_cb(&g_touch); // 触摸动画回调
    }
    uint8_t dirty_cnt = 0;
    sc_dirty_t *p_dirty = sc_dirty_merge_out(&dirty_cnt);
    if (dirty_cnt == 0)
        return; // 无脏矩形
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
    sc_obj_t *intersect_obj[20]; // 要绘制的节点
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
                if (intersect_cnt < sizeof(intersect_obj) / sizeof(intersect_obj[0]))
                {
                    current->flag &= ~SC_OBJ_FLAG_ACTIVE;     // 清除刷新标记
                    intersect_obj[intersect_cnt++] = current; // 要绘制的节点
                }
                current = current->next;
            }
        }
        // -----------------刷新控件--------------------------------
        sc_event_t event;
        event.type = SC_EVENT_TYPE_DRAW;
        event.dat.arg = &pfb;
        sc_pfb_init_slices(&pfb, &draw_rect, gui->bkc);
        do
        {
            for (int i = 0; i < intersect_cnt; i++)
            {
                sc_obj_t *obj = intersect_obj[i];
                obj->handle_event(obj, &event);
            }
        } while (sc_pfb_next_slice(&pfb)); // 分帧刷新
    }
}
/// @brief 演示代码
void sc_touch_demo(void)
{
    static char edit_buf[32] = "";
    static char edit_buf2[32] = "";

    g_touch.srceen = sc_create_srceen(NULL, 0, 0, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, ALIGN_CENTER);

    sc_obj_t *canvas = sc_create_canvas(g_touch.srceen, 0, 0, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, SC_SCREEN_WIDTH * 2, SC_SCREEN_HEIGHT * 2, ALIGN_CENTER);
    sc_obj_t *rect1 = sc_create_rect(canvas, 0, 0, 300, 200, ALIGN_CENTER);
    sc_obj_t *rect2 = sc_create_rect(canvas, 320, 0, 300, 200, ALIGN_CENTER);
    sc_obj_t *rect4 = sc_create_rect(g_touch.srceen, 0, 100, 50, 50, ALIGN_CENTER);
    sc_obj_t *line_edit = sc_create_line_edit(rect1, 0, -40, 100, 30, ALIGN_CENTER);

    sc_set_edit_text(line_edit, edit_buf, 32);
    sc_obj_t *line_edit2 = sc_create_line_edit(rect1, 0, 0, 100, 30, ALIGN_CENTER);
    sc_set_edit_text(line_edit2, edit_buf2, 32);

    sc_set_color(line_edit, C_BLACK, gui->bkc);
    sc_set_color(line_edit2, C_BLACK, gui->bkc);

    sc_set_color(rect1, C_WHEAT, gui->bkc);
    sc_set_color(rect2, C_GREEN, gui->bkc);

    sc_obj_list_print(g_touch.srceen);
}