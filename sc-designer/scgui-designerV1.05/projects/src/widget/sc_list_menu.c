#include "../sc_draw.h"
#include "sc_list_items.h"
#include "sc_list_menu.h"

#if SC_WIDGET_LIST_MENU_ENABLE

/* 返回项识别：文本以 Back/Break/Exit 结尾，ENTER 等价 CMD_BACK */
static bool sc_menu_is_back_item(const char *str)
{
    if (!str)
    {
        return false;
    }
    static const char suffixes[][6] = {"Back", "Break", "Exit"};
    const uint8_t lens[] = {4, 5, 4};
    int len = 0;
    while (str[len])
    {
        len++;
    }
    for (uint8_t s = 0U; s < 3U; s++)
    {
        if (len < lens[s])
        {
            continue;
        }
        bool match = true;
        for (uint8_t j = 0U; j < lens[s]; j++)
        {
            if (str[len - lens[s] + j] != suffixes[s][j])
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            return true;
        }
    }
    return false;
}

/* 子级查找：nodes 表里 parent_item == 本级光标条目的节点 */
static const sc_menu_node_t *sc_menu_find_child(const sc_list_menu_t *props,
                                                const sc_menu_item_t *parent_item)
{
    for (uint16_t i = 0U; i < props->nodes_cnt; i++)
    {
        if (props->nodes[i].parent_item == parent_item)
        {
            return &props->nodes[i];
        }
    }
    return NULL;
}

/* 父级查找：哪个节点的条目数组里有 parent_item */
static const sc_menu_node_t *sc_menu_find_parent(const sc_list_menu_t *props,
                                                 const sc_menu_item_t *parent_item)
{
    if (!parent_item)
    {
        return NULL;
    }
    for (uint16_t i = 0U; i < props->nodes_cnt; i++)
    {
        const sc_menu_node_t *node = &props->nodes[i];
        for (uint16_t j = 0U; j < node->items_cnt; j++)
        {
            if (&node->items[j] == parent_item)
            {
                return node;
            }
        }
    }
    return NULL;
}

static uint8_t sc_menu_level_cnt(const sc_list_menu_t *props)
{
    return props->head ? (uint8_t)props->head->items_cnt : 0U;
}

static const sc_menu_item_t *sc_menu_level_items(const sc_list_menu_t *props)
{
    return props->head ? props->head->items : NULL;
}

/* 行高 = max(字体行高, 本级最高图示)；分界水平线画在 item_gap 间隙里 */
static void sc_list_menu_scroll_visible(sc_obj_t *obj); /* 光标可见性滚动（cursor 移动/进出级共用） */

static int sc_list_menu_row_h(const sc_list_menu_t *props)
{
    const lv_font_t *font = sc_get_font(props->font_id);
    int row = sc_font_line_height(font);
    const sc_menu_item_t *items = sc_menu_level_items(props);
    uint8_t cnt = sc_menu_level_cnt(props);
    for (uint8_t i = 0U; i < cnt; i++)
    {
        if (items[i].icon && (int)items[i].icon->height > row)
        {
            row = items[i].icon->height;
        }
    }
    return row > 0 ? row : 1;
}

/* 行内容高：显式 item_h 优先，0 = 自动（字体行高与最高图示取大） */
static int sc_list_menu_item_h(const sc_list_menu_t *props)
{
    return props->item_h != 0U ? (int)props->item_h : sc_list_menu_row_h(props);
}

static int sc_list_menu_pitch(const sc_list_menu_t *props)
{
    int pitch = sc_list_menu_item_h(props) + (int)props->item_gap;
    return pitch > 0 ? pitch : 1;
}

static void sc_list_menu_parent(sc_obj_t *obj)
{
    sc_list_menu_t *props = (sc_list_menu_t *)obj->props;
    if (props->level == 0U)
    {
        return;
    }
    const sc_menu_node_t *parent = sc_menu_find_parent(props, props->head->parent_item);
    if (!parent)
    {
        return;
    }
    props->level--;
    props->head = parent;
    props->yofs = 0;
    props->cb_view = 0U; /* leave callback view: back to normal list */
    sc_list_menu_scroll_visible(obj);
    sc_obj_invalidate(obj);
}

/* Unified back: leave callback view first (delete its child objects), else go to parent.
 * Shared by CMD_BACK key and touch left-slide to avoid duplicated logic. */

static void sc_list_menu_back(sc_obj_t *obj)
{
    sc_list_menu_t *props = (sc_list_menu_t *)obj->props;
    if (props->cb_view)
    {
        sc_obj_t *cur = obj->next;
        uint8_t level=sc_obj_level(obj);
        while (cur && sc_obj_level(cur) > level)
        {
            sc_obj_t *next = cur->next;
            sc_obj_del(cur);
            cur = next;
        }
        props->cb_view = 0U;
        sc_obj_invalidate(obj);
        return;
    }
    sc_list_menu_parent(obj); /* parent() no-ops at level 0 */
}

/* ENTER：返回项回父级；有子级下钻；否则本级 items_cb 叶子动作 */
static void sc_list_menu_child(sc_obj_t *obj)
{
    sc_list_menu_t *props = (sc_list_menu_t *)obj->props;
    const sc_menu_item_t *items = sc_menu_level_items(props);
    uint8_t cnt = sc_menu_level_cnt(props);
    uint8_t cursor = props->cursor[props->level];
    if (!items || cnt == 0U || cursor >= cnt)
    {
        return;
    }
    const sc_menu_item_t *item = &items[cursor];
    if (sc_menu_is_back_item(item->text))
    {
        sc_list_menu_parent(obj);
        return;
    }
    const sc_menu_node_t *child = sc_menu_find_child(props, item);
    if (child)
    {
        if (props->level + 1U >= SC_MENU_DEPTH_MAX) /* 已到最深层，不再下钻 */
        {
            return;
        }
        props->level++;
        props->head = child;
        props->cursor[props->level] = 0U;
        props->yofs = 0;
        props->cb_view = 0U; /* drill down: no callback child */
        sc_obj_invalidate(obj);
        return;
    }
    if (props->head->items_cb && props->cb_view == 0U) /* not in callback view */
    {
        props->cb_view = 1U; 
        props->head->items_cb(obj);
        sc_obj_invalidate(obj);
    }
}

static void sc_draw_list_menu(sc_pfb_t *pfb, const sc_list_menu_t *props, rect_t view)
{
    if (!props || !props->head || view.w == 0U || view.h == 0U)
    {
        return;
    }
    uint8_t radius = props->radius;
    if (!sc_draw_Rounded_mask(pfb, view, (int)radius, 1, props->fg, props->bg, 255U, 0U, NULL))
    {
        return;
    }
    int pad = (int)(radius >> 1);
    rect_t box = {
        (int16_t)(view.x + pad),
        (int16_t)(view.y + pad),
        (uint16_t)SC_MAX((int)view.w - 2 * pad, 0),
        (uint16_t)SC_MAX((int)view.h - 2 * pad, 0),
    };
    const lv_font_t *font = sc_get_font(props->font_id);
    int item_h = sc_list_menu_item_h(props);
    int pitch = sc_list_menu_pitch(props);
    uint8_t cnt = sc_menu_level_cnt(props);
    const sc_menu_item_t *items = sc_menu_level_items(props);
    uint8_t cursor = props->cursor[props->level];

    if (cnt != 0U && cursor >= cnt)
    {
        cursor = 0U;
    }
    area_t clip = rect_to_area(box);
    int y = box.y + props->yofs;
    /* 选中色块与分界线共用同一水平区间：line_xs 起点 ～ 滚动条让位终点 */
    int16_t line_x = (int16_t)(box.x + props->line_xs);
    int16_t line_right = (int16_t)((int)box.x + (int)box.w -
                                   ((props->bar_w > 0U) ? ((int)props->bar_w + pad) : 2));
    int line_w = (int)line_right - (int)line_x;
    if (items && !props->cb_view)
    {
        uint8_t i;
        for (i = 0U; i < cnt; i++)
        {
            rect_t row = {box.x, (int16_t)y, box.w, (uint16_t)pitch};
            if ((int)row.y + (int)row.h > (int)box.y && row.y < (int16_t)(box.y + (int)box.h))
            {
                const sc_menu_item_t *item = &items[i];
                bool selected = (i == cursor);
                if (selected && line_w > 0)
                {
                    rect_t sel = {line_x, row.y, (uint16_t)line_w, (uint16_t)item_h};
                    (void)sc_draw_Rounded_mask(pfb, sel, (int)(radius >> 1), 0,
                                               props->sel_bg, props->sel_bg, 255U, 0U, &clip);
                }
                /* 图示位置 = 行左 + icon_x；文字/分界线起点 = 行左 + line_xs。
                 * 均用户自调，不做自动对齐；字符在 line_xs 基础上再 +2px */
                if (item->icon)
                {
                    int16_t img_x = (int16_t)(row.x + props->icon_x);
                    int16_t iy = (int16_t)(row.y + (item_h - (int)item->icon->height) / 2);
                    sc_draw_image(pfb, item->icon, img_x, iy);
                }
                if (item->text)
                {
                    uint16_t text_h = 0U;
                    sc_text_measure_lines(item->text, font, 0U, NULL, &text_h, NULL);
                    int16_t ty =
                        (int16_t)(row.y + sc_align_offset_y(item_h, text_h, ALIGN_VER));
                    sc_draw_str_range_opa(pfb, (int16_t)(line_x + 2), ty, item->text, NULL,
                                          selected ? C_WHITE : props->fg, NULL, font, 0U,
                                          &clip, 255U);
                }
                /* 分界水平线：贴行底（行顶 + item_h），1px 按精确可见性绘制，末条不画 */
                if (i + 1U < cnt && line_w > 0)
                {
                    int line_y = row.y + item_h;
                    if (line_y >= box.y && line_y < (int)box.y + (int)box.h)
                    {
                        (void)sc_draw_fill_rect_opa(pfb, line_x, line_y, line_w, 1,
                                                    props->fg, 255U);
                    }
                }
            }
            y += pitch;
        }
    }
    if (props->bar_w > 0U)
    {
        rect_t bar = {(int16_t)(view.x + (int)view.w - (int)props->bar_w - pad),
                      (int16_t)(view.y + pad), (uint16_t)props->bar_w,
                      (uint16_t)SC_MAX((int)view.h - (int)radius, 0)};
        sc_draw_bar(pfb, bar, props->bar_w / 2, props->fg, C_RGB(100, 100, 100), 128U,
                    -props->yofs);
    }
}

void sc_lcd_draw_list_menu(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h,
                           const sc_list_menu_t *props)
{
    rect_t view = {x, y, w, h};
    if (!props)
    {
        return;
    }
    if (pfb)
    {
        sc_draw_list_menu(pfb, props, view);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_list_menu(sc_lcd_draw_pfb__, props, view));
}

#if SC_USER_OBJ && SC_WIDGET_LIST_MENU_ENABLE

static void sc_list_menu_scroll_visible(sc_obj_t *obj)
{
    sc_list_menu_t *props = (sc_list_menu_t *)obj->props;
    int pad = (int)(props->radius >> 1);
    int view_h = (int)obj->view.h - 2 * pad;
    /* 共用 list_items 内联（对齐方向 + sc_scroll_range 钳制）。
     * 可见性判据用文字行高：文字完整可见就不移动 yofs，item_h 底部留白不触发滚动 */
    int text_h = sc_font_line_height(sc_get_font(props->font_id));
    sc_list_ensure_visible(&props->yofs, sc_list_menu_pitch(props), text_h, view_h,
                           (int)sc_menu_level_cnt(props), (int)props->cursor[props->level]);
    sc_obj_invalidate(obj);
}

static const sc_obj_vt_t sc_list_menu_vt = {
    .event_cb = sc_obj_handle_list_menu_event,
    .set_style = sc_list_menu_style,
};

sc_obj_t *sc_list_menu_create(sc_obj_t *parent, sc_list_menu_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_LIST_MENU, &sc_list_menu_vt, props);
    if (obj)
    {
        *props = (sc_list_menu_t){.fg = C_GRAY,
                                  .bg = g_gui->bg,
                                  .sel_bg = C_BLUE,
                                  .item_gap = 2U,
                                  .radius = 6U,
                                  .bar_w = 6U};
        /* icon_x/line_xs 用户自调，给一组按默认字体算的可用起点 */
        props->icon_x = 4;
        props->line_xs = (int16_t)(4 + sc_font_line_height(sc_get_font(props->font_id)) + 4);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

void sc_list_menu_set_data(sc_obj_t *obj, const sc_menu_node_t *nodes, uint16_t nodes_cnt)
{
    sc_list_menu_t *props = (sc_list_menu_t *)obj->props;
    props->nodes = nodes;
    props->nodes_cnt = nodes_cnt;
    props->head = nodes; /* 顶层节点 */
    props->level = 0U;
    props->cursor[0] = 0U;
    props->yofs = 0;
    sc_obj_invalidate(obj);
}

const sc_menu_item_t *sc_list_menu_current(sc_obj_t *obj)
{
    sc_list_menu_t *props = (sc_list_menu_t *)obj->props;
    const sc_menu_item_t *items = sc_menu_level_items(props);
    uint8_t cnt = sc_menu_level_cnt(props);
    uint8_t cursor = props->cursor[props->level];
    if (!items || cursor >= cnt)
    {
        return NULL;
    }
    return &items[cursor];
}

bool sc_list_menu_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_list_menu_t *props = (sc_list_menu_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->fg = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->bg = *(const color_t *)value;
        return true;
    case SC_STYLE_ON:
        props->sel_bg = *(const color_t *)value;
        return true;
    case SC_STYLE_FONT:
        props->font_id = *(const uint8_t *)value;
        return true;
    case SC_STYLE_RADIUS_READ:
        *(uint8_t *)value = ((const sc_list_menu_t *)obj->props)->radius;
        return true;

    default:
        return false;
    }
}

/* 键盘：UP/DOWN/LEFT/RIGHT 移光标（单列列表四键同义，不循环、到边界不消费）；
 * ENTER 返回项回父级 / 有子级下钻 / 叶子交本级 items_cb；
 * BACK 弹回父级，顶层不消费交给全局导航。 */
bool sc_obj_handle_list_menu_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_list_menu_t *props = (sc_list_menu_t *)obj->props;
    bool ret = false;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_list_menu((sc_pfb_t *)event->dat.arg, props, obj->view);
        return true;
    }
    if (event->type == SC_EVENT_KEY_DOWN)
    {
        if (event->dat.key == CMD_BACK)
        {
            sc_list_menu_back(obj); /* back() handles callback view internally */
            return true;
        }
        if (props->cb_view)
        {
            return false; /* callback view: other keys bubble to children (e.g. progress) */
        }
        if (event->dat.key == CMD_ENTER)
        {
            sc_list_menu_child(obj);
            return true; /* edit-mode ENTER consumed, prevents focus overflow */
        }
        uint8_t cnt = sc_menu_level_cnt(props);
        uint8_t cursor = props->cursor[props->level];
        uint8_t next = cursor;
        if (event->dat.key == CMD_UP || event->dat.key == CMD_LEFT)
        {
            next = (uint8_t)(cursor > 0U ? cursor - 1U : cursor);
        }
        else if (event->dat.key == CMD_DOWN || event->dat.key == CMD_RIGHT)
        {
            next = (uint8_t)(cursor + 1U < cnt ? cursor + 1U : cursor);
        }
        if (next != cursor)
        {
            props->cursor[props->level] = next;
            sc_list_menu_scroll_visible(obj);
            return true; 
        }
        return false;
    }
#if SC_TOUCH_ENABLE
    else if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        g_gui->flags |= SC_RT_DRAGGING;
        obj->flags |= SC_OBJ_F_PRESSED;
        ret = true;
    }
    else if (event->type == SC_EVENT_TOUCH_MOVE && ((g_gui->flags & SC_RT_DRAGGING) != 0U || props->cb_view))
    {
        if (props->cb_view)
        {
            return true; /* callback view: lock touch, do not bubble to container */
        }
        int16_t dy = (int16_t)(event->dat.pos[1] - g_gui->last_pos_y);
        if (dy != 0)
        {
            int pitch = sc_list_menu_pitch(props);
            int view_h = (int)obj->view.h - 2 * (int)(props->radius >> 1);
            int16_t min_yofs;
            int16_t max_yofs;
            sc_scroll_range((int)sc_menu_level_cnt(props) * pitch, view_h, pitch, &min_yofs,
                            &max_yofs);
            int16_t new_yofs = (int16_t)SC_CLAMP((int)props->yofs + dy, min_yofs, max_yofs);
            if (new_yofs != props->yofs)
            {
                props->yofs = new_yofs;
                sc_obj_invalidate(obj);
            }
        }
        ret = true;
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        g_gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        if ((obj->flags & SC_OBJ_F_PRESSED) != 0U)
        {
            int dx = (int)event->dat.pos[0] - (int)g_gui->drag_offset_x;
            int dy = (int)event->dat.pos[1] - (int)g_gui->drag_offset_y;
            if (dx <= -(int)SC_MENU_SLIDE_BACK_PX)
            {
                sc_list_menu_back(obj);
            }
            else if ((dy/4) == 0 && !props->cb_view) /* callback view: tap does nothing */
            {
                int pitch = sc_list_menu_pitch(props);
                int pad = (int)(props->radius >> 1);
                int item_y = event->dat.pos[1] - obj->view.y - pad - props->yofs;
                int hit = (item_y >= 0) ? item_y / pitch : -1;
                if (item_y >= 0 && hit < (int)sc_menu_level_cnt(props))
                {
                    if (props->cursor[props->level] != (uint8_t)hit)
                    {
                        props->cursor[props->level] = (uint8_t)hit;
                        sc_obj_invalidate(obj);
                    }
                    /* 点选即执行（对齐旧版九宫格）：进子级/叶子 items_cb/Back 回退 */
                    sc_list_menu_child(obj);
                }
            }
        }
        obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
        ret = true;
    }
#endif /* SC_TOUCH_ENABLE */

    return ret;
}

#endif /* SC_USER_OBJ && SC_WIDGET_LIST_MENU_ENABLE */

#endif /* SC_WIDGET_LIST_MENU_ENABLE */
