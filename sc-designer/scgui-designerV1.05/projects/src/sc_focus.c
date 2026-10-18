#include "sc_focus.h"
#include "sc_gui.h"
#include "widget/sc_container.h" /* 内部边界水平翻页：sc_container_page_by_key */

/* 焦点栈初始化（sc_gui_init 调用）：depth=0, stack[0]=NIL, 默认按键输入（画红框）。 */
void sc_focus_init(void)
{
    sc_gui_t *gui = g_gui;
    gui->focus_depth = 0U;
    gui->focus_stack[0] = SC_OBJ_NIL;
}

/* 返回当前栈顶焦点对象。 */
sc_obj_t *sc_focus_current(void)
{
    return sc_obj_at(g_gui->focus_stack[g_gui->focus_depth]);
}

/* Report whether an object is focusable and currently visible.
 * VIRTUAL 布局容器（如页面 root）不参与键盘聚焦/空间导航：不渲染、无交互语义，
 * 若计入会以全屏 rect 在导航评分中抢走真实控件的焦点。 */
static bool sc_focus_can(const sc_obj_t *obj)
{
    area_t area;
    return (obj && (obj->flags & SC_OBJ_F_VIRTUAL) == 0U &&
            sc_obj_is_focusable(obj) && sc_obj_get_clip_area(obj, &area));
}

/* Move keyboard focus to obj, invalidating both the old and new focused objects. */
void sc_obj_set_key_focus(sc_obj_t *obj)
{
    sc_obj_t *old = sc_focus_current();
    if (old == obj || (obj && !sc_focus_can(obj)))
    {
        return;
    }
    g_gui->flags &= (uint8_t)~SC_RT_KEY_EDIT; /* 切换焦点时关闭编辑模式 */
    sc_obj_invalidate(old);
    g_gui->focus_stack[g_gui->focus_depth] = obj ? obj->id : SC_OBJ_NIL;
    sc_obj_invalidate(obj);
}

/* 弹栈一层：CMD_BACK 返回上级，退出容器内部回到容器本身。 */
void sc_focus_pop(void)
{
    if (g_gui->focus_depth > 0U)
    {
        g_gui->flags |= SC_RT_KEY_INPUT;
        sc_obj_invalidate(sc_focus_current());
        g_gui->focus_depth--;
        sc_obj_invalidate(sc_focus_current());
    }
    else
    {
        /* 已在栈底：取消焦点，无红框 */
        g_gui->flags &= (uint8_t)~SC_RT_KEY_INPUT;
        sc_obj_invalidate(sc_focus_current());
        g_gui->focus_stack[g_gui->focus_depth] = SC_OBJ_NIL;
    }
    g_gui->flags &= (uint8_t)~SC_RT_KEY_EDIT; /* 两个分支统一关闭编辑模式 */
}

/* 压栈进入内部模式（容器 ENTER）。 */
void sc_focus_push(sc_obj_t *obj)
{
    if (!obj)
    {
        return;
    }
    g_gui->flags |= SC_RT_KEY_INPUT;
    if (g_gui->focus_depth < SC_FOCUS_STACK_MAX - 1U)
    {
        g_gui->focus_stack[++g_gui->focus_depth] = obj->id;
    }
}

/* 对象释放时清理焦点栈中所有指向该 id 的引用，栈顶被清时 depth 回退。 */
void sc_focus_clear_obj(uint8_t idx)
{
    sc_gui_t *gui = g_gui;
    if (idx == SC_OBJ_NIL)
    {
        return;
    }
    /* 一次遍历：清除匹配idx的位置，同时记录最后一个非NIL索引，直接设depth */
    uint8_t new_depth = 0U;
    for (uint8_t fi = 0U; fi <= gui->focus_depth; fi++)
    {
        if (gui->focus_stack[fi] == idx)
        {
            gui->focus_stack[fi] = SC_OBJ_NIL;
        }
        if (gui->focus_stack[fi] != SC_OBJ_NIL)
        {
            new_depth = fi;
        }
    }
    gui->focus_depth = new_depth;
}

/* 触摸设置栈顶：标记触摸输入（不画红框）+ 可聚焦则设栈顶。 */
void sc_focus_touch_set(sc_obj_t *obj)
{
    sc_gui_t *gui = g_gui;
    gui->flags &= (uint8_t)~SC_RT_KEY_INPUT;
    if (obj == NULL)
    {
        gui->focus_stack[gui->focus_depth] = SC_OBJ_NIL;
    }
    else if (sc_obj_is_focusable(obj))
    {
        gui->flags &= (uint8_t)~SC_RT_KEY_EDIT; /* 触摸切换焦点时关闭编辑模式 */
        gui->focus_stack[gui->focus_depth] = obj->id;
    }
}

/* 9 宫格空间导航打分核心：以焦点 rect 为界分 3x3 宫格（左/中/右 x 上/中/下），
 * 按方向提取对应宫格候选并打分，lower is better，UINT32_MAX 拒绝该候选。
 * 无固定行高/除法/移位：LEFT/RIGHT 换行档垂直距离 ×4 加横向位次，同行（档内）由横向决胜；
 * ×4 修正同排错位布局（阶梯/瀑布）：斜邻垂直差只有几像素，同权重会被横向位次压过。
 * 用户改布局不会因魔数失效。
 * 全局导航（sc_obj_focus_root）与复合控件内部导航（sc_composite_focus_direction，
 * 如软键盘按键矩阵）共用此函数，算法统一、代码只维护一份。 */
uint32_t sc_focus_grid_score(const rect_t *focus, int key, const rect_t *cand)
{
    const int16_t fx0 = focus->x;
    const int16_t fy0 = focus->y;
    const int16_t fy1 = (int16_t)(focus->y + focus->h);
    /* 导航基准用左上角坐标（而非中心）：布局时控件左上角对齐更符合用户直觉，
     * 重叠/错位控件也能稳定判定同行同列。 */
    const int16_t cx = cand->x;
    const int16_t cy = cand->y;
    switch (key)
    {
    case CMD_RIGHT:
    case CMD_LEFT:
    {
        /* dir=+1 右 / -1 左：dx 归一化为"前方"水平距离，≤0 即不在前方 */
        const int dir = (key == CMD_RIGHT) ? 1 : -1;
        const int dx = (cx - fx0) * dir;
        /* 同行：候选左上角 y 在焦点 y 范围内，且在前方（右侧/左侧），水平近者胜 */
        /* same-row = vertical range overlap (left-top split misses
         * neighbors whose tops are offset but rows still overlap). */
        const bool same_row = (cy < (int16_t)(focus->y + focus->h) &&
                               (int16_t)(cy + cand->h) > fy0);
        if (same_row && dx > 0)
        {
            return (uint32_t)dx;
        }
        /* 换行：右取底边之下（列位最左 cx），左取顶边之上（列位最右 0x7FFF-cx）。
         * 垂直 ×4：同排错位布局（阶梯/瀑布）中斜邻控件垂直差只有几像素，
         * 同权重会被横向位次压过；加权后"更近的行"优先，横向位次只做近行决胜。
         * 距离 ×4 + 列位仍远小于 0x40000000，分档不受影响。 */
        const int vd = (dir > 0) ? (cy - fy1) : (fy0 - cy);
        if (vd > 0)
        {
            const uint32_t col = (dir > 0) ? (uint32_t)cx : 0x7FFFu - (uint32_t)cx;
            return 0x40000000u + (uint32_t)vd * 4u + col;
        }
        return UINT32_MAX;
    }
    case CMD_DOWN:
    case CMD_UP:
    {
        /* dir=+1 下 / -1 上：vd 归一化为"前方"垂直距离，≤0 即不在前方 */
        const int dir = (key == CMD_DOWN) ? 1 : -1;
        const int vd = (cy - fy0) * dir;
        if (vd <= 0)
        {
            return UINT32_MAX;
        }
        int d = cx - fx0;
        if (d < 0)
        {
            d = -d;
        }
        /* 同列（中列）零基数优先，斜列 0x40000000 退居次档 */
        /* same-col = horizontal range overlap; wide/left-offset candidates
         * must not be demoted by a left-top-corner column split. */
        const bool same_col = (cx < (int16_t)(focus->x + focus->w) &&
                               (int16_t)(cx + cand->w) > fx0);
        return (same_col ? 0u : 0x40000000u) + (uint32_t)vd + (uint32_t)d;
    }
    default:
        return UINT32_MAX;
    }
}

/* 统一空间导航：内部模式（depth>0）在栈顶容器子树内打分选优；
 * 全局模式（depth==0）在活动根第一层（兄弟 + head 直接子级）选优。
 * 打分细节见 sc_focus_grid_score 注释。 */
bool sc_obj_focus_root(sc_obj_t *focus, int key)
{
    sc_obj_t *best = NULL;
    uint32_t best_score = UINT32_MAX;
    sc_obj_t *container = NULL; /* 内部模式的宿主容器（栈中上一层） */
    rect_t focus_rect = (rect_t){0, 0, 1, 1};
    if (focus && sc_focus_can(focus))
    {
        focus_rect = (rect_t){focus->view.x, focus->view.y, focus->view.w, focus->view.h};
    }
    if (g_gui->focus_depth > 0U) /* 进入内部焦点模式 */
    {
        container = sc_obj_at(g_gui->focus_stack[g_gui->focus_depth - 1U]);
        if (!container)
        {
            return false;
        }
        sc_obj_t *obj = container->next;
        uint8_t level = sc_obj_level(container);
        while (obj && sc_obj_level(obj) > level)
        {
            if (obj != focus && sc_focus_can(obj))
            {
                uint32_t score = sc_focus_grid_score(&focus_rect, key, &obj->view);
                if (score < best_score)
                {
                    best = obj;
                    best_score = score;
                }
            }
            obj = obj->next;
        }
    }
    else
    {
        sc_obj_t *head = sc_obj_at(sc_active_root(g_gui)->head);
        if (!head)
        {
            return false; /* 空页面：无候选 */
        }
        sc_obj_t *obj = head;
        while (obj)
        {
            /* 推进（下方）已限定候选集=兄弟层+根的儿子，这里只需查可聚焦 */
            if (obj != focus && sc_focus_can(obj))
            {
                uint32_t score = sc_focus_grid_score(&focus_rect, key, &obj->view);
                if (score < best_score)
                {
                    best = obj;
                    best_score = score;
                }
            }
            /* 推进：只有页面根（ROOT）走进直接子级——"儿子"专指根的儿子；
             * 其余节点（含无 root 时平铺的容器）整树跳过，其子级须 ENTER 进入。
             * 路过的页面根记入 container：全局到左右边界时由尾部翻它 */
            if (sc_obj_type(obj) == SC_OBJ_ROOT)
            {
                container = obj;
                obj = obj->next;
            }
            else
            {
                obj = sc_obj_next_tree(obj);
            }
        }
    }
    if (best)
    {
        g_gui->flags |= SC_RT_KEY_INPUT;
        sc_obj_set_key_focus(best);
        return true;
    }
    /* 到左右边界翻宿主：内部模式=栈顶容器，全局模式=页面根（均需 H_SCROLL，
     * 焦点控件/页面随翻页平移）；翻动成功消费事件保持原模式，
     * 非 LOOP 翻到尽头返回 false，pop 退出内部模式/清焦点回卷 */
    if (container != NULL && sc_obj_is_container(container) &&
        (key == CMD_LEFT || key == CMD_RIGHT))
    {
        const sc_container_t *props = (const sc_container_t *)container->props;
        if ((props->flags & SC_CONTAINER_F_H_SCROLL) != 0U &&
            sc_container_page_by_key(container, (key == CMD_RIGHT) ? 1 : -1, true))
        {
            return true;
        }
    }
    sc_focus_pop();
    return false;
}