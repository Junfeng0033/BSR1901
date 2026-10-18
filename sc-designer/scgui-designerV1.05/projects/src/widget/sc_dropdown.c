#include "../sc_draw.h"
#include "sc_dropdown.h"
#include "sc_round_rect.h"
#include "sc_list_items.h" /* sc_list_ensure_visible 公共函数 */

#if SC_WIDGET_DROPDOWN_ENABLE

/* 主框与展开列表之间的间隙（像素） */
#define SC_DROPDOWN_GAP (2U)

/* 行高：字体行高 + 间距 */
static inline int sc_dropdown_pitch(const sc_dropdown_t *props)
{
    const lv_font_t *font = sc_get_font(props->font_id);
    int pitch = sc_font_line_height(font) + 2;
    return pitch > 0 ? pitch : 1;
}

/* 展开列表总高度 */
static uint16_t sc_dropdown_total_h(const sc_dropdown_t *props)
{
    if (props->list_h > 0)
    {
        return props->list_h;
    }
    return (uint16_t)(sc_dropdown_pitch(props) * props->items_cnt);
}

/* 绘制下拉箭头（向下等腰三角形），for行+while列直接写pfb缓存，边缘近似AA */
static void sc_draw_dropdown_arrow(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t size, color_t color)
{
    uint32_t fc_pack = sc_rgb565_pack_alpha(color);
    for (uint16_t row = 0; row < size; row++)
    {
        int py = y + row;
        if (py < pfb->area.ys || py >= pfb->area.ye) continue;
        color_t *buf = sc_pfb_pixel_ptr(pfb, x, py); /* 外循环算一次行首指针 */
        /* 等腰三角形（向下▼）：顶部宽，第row行宽度=size-row，居中，底部尖 */
        int width = (int)size - (int)row;
        int left = (int)row / 2; /* 左边缘向下取整，奇数行实际在left+0.5 */
        int right = left + width + ((int)row & 1); /* 奇数行右边缘多一个半覆盖像素 */
        /* 裁剪到pfb区域 */
        int x_min = pfb->area.xs - x;
        int x_max = pfb->area.xe - x;
        if (left < x_min) left = x_min;
        if (right > x_max) right = x_max;
        /* 边缘近似AA：row奇数时边缘半覆盖(128)，偶数时全覆盖(255) */
        uint8_t edge_opa = (row & 1U) ? 128U : 255U;
        /* 左边缘 */
        if (left < right)
        {
            sc_blend_ptr_fast(&buf[left], color, fc_pack, edge_opa);
            left++;
        }
        /* 右边缘 */
        if (left < right)
        {
            right--;
            sc_blend_ptr_fast(&buf[right], color, fc_pack, edge_opa);
        }
        /* 中间不透明 */
        int col = left;
        while (col < right)
        {
            buf[col] = color;
            col++;
        }
    }
}

/* 绘制列表区域（参照list_items） */
static void sc_draw_dropdown_list(sc_pfb_t *pfb, const sc_dropdown_t *props, rect_t list_box)
{
    if (!props->items || props->items_cnt == 0)
    {
        return;
    }
    int pitch = sc_dropdown_pitch(props);
    int bar_w = (int)props->bar_w;
    rect_t content = {list_box.x, list_box.y,
                      (uint16_t)SC_MAX((int)list_box.w - bar_w, 0), list_box.h};
    /* 滚动条（参考list_items，bar_w>0一直显示） */
    if (bar_w > 0)
    {
        rect_t bar = {
            (int16_t)(list_box.x + list_box.w - bar_w),
            list_box.y+2,
            (uint16_t)bar_w,
            list_box.h-4,
        };
        sc_draw_bar(pfb, bar, bar_w / 2, props->fg, C_RGB(100, 100, 100), 128U, -props->yofs);
    }
    /* 列表项 */
    area_t content_clip = rect_to_area(rect_grow(content, -1, -1) );  // （内缩1像素）
    int y = content.y + props->yofs;
    for (uint8_t i = 0; i < props->items_cnt; i++)
    {
        rect_t item = {content.x, (int16_t)y, content.w, (uint16_t)pitch};
        /* 可见性判断：非选中项只需y范围，选中项才需area_intersect计算clipped */
        if (y + pitch > content.y && y < content.y + content.h)
        {
            bool selected = (i == props->selected);
            if (selected)
            {
                area_t item_area = rect_to_area(item);
                area_t clipped;
                if (area_intersect(item_area, content_clip, &clipped))
                {
                    rect_t sel_rect = {clipped.xs, clipped.ys,
                                       (uint16_t)(clipped.xe - clipped.xs),
                                       (uint16_t)(clipped.ye - clipped.ys)};
                    sc_draw_Rounded_mask(pfb, sel_rect, props->radius, 0,
                                         props->sel_bg, props->sel_bg, 255U, 0U, NULL);
                }
            }
            const char *name = props->items[i];
            if (name)
            {
                sc_draw_str_range_opa(pfb, item.x + 4, item.y + 1, name, NULL,
                                      selected ? C_WHITE : props->fg, NULL,
                                      sc_get_font(props->font_id), 0, &content_clip, 255U);
            }
        }
        y += pitch;
    }
}

/* 绘制下拉框 */
static void sc_draw_dropdown(sc_pfb_t *pfb, const sc_dropdown_t *props, rect_t view, int border)
{
    uint16_t list_h = sc_dropdown_total_h(props);
    uint16_t closed_h = props->expanded ? (uint16_t)(view.h - list_h - SC_DROPDOWN_GAP) : view.h;
    rect_t main_box = {view.x, view.y, view.w, closed_h};
    /* 主框 */
    sc_draw_Rounded_mask(pfb, main_box, props->radius, border, props->fg, props->bg, 255U, 0, NULL);
    /* 当前选中项名称 */
    if (props->items && props->selected < props->items_cnt && props->items[props->selected])
    {
        sc_draw_text_aligned(pfb, main_box, props->items[props->selected], props->font_id, 0,
                             props->fg, NULL, ALIGN_LEFT|ALIGN_VER, 4, 255U, 0);
    }
    /* 下拉箭头 */
    uint16_t arrow_size = (uint16_t)(closed_h / 3);
    int16_t arrow_x = (int16_t)(view.x + view.w - arrow_size - 4);
    int16_t arrow_y = (int16_t)(view.y + (closed_h - arrow_size) / 2);
    sc_draw_dropdown_arrow(pfb, arrow_x, arrow_y, arrow_size, props->fg);
    /* 展开列表 */
    if (props->expanded && props->items_cnt > 0)
    {
        rect_t list = {view.x, (int16_t)(view.y + closed_h + SC_DROPDOWN_GAP), view.w, list_h};
        sc_draw_Rounded_mask(pfb, list, props->radius, 1, props->fg, props->bg, 255U, 0, NULL);
        sc_draw_dropdown_list(pfb, props, list);
    }
}

#if SC_USER_OBJ && SC_WIDGET_DROPDOWN_ENABLE
static const sc_obj_vt_t sc_dropdown_vt = {
    .event_cb = sc_obj_handle_dropdown_event,
    .set_style = sc_dropdown_style,
};

sc_obj_t *sc_dropdown_create(sc_obj_t *parent, sc_dropdown_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_DROPDOWN, &sc_dropdown_vt, props);
    if (obj)
    {
        *props = (sc_dropdown_t)SC_DROPDOWN_NEW(NULL, 0, 0);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

/* 收起时的高度（展开时view.h包含了列表，需减去） */
static uint16_t sc_dropdown_closed_h(sc_obj_t *obj)
{
    sc_dropdown_t *props = (sc_dropdown_t *)obj->props;
    if (props->expanded)
    {
        return (uint16_t)(obj->view.h - sc_dropdown_total_h(props) - SC_DROPDOWN_GAP);
    }
    return obj->view.h;
}

/* 统一展开/收起：修改view.h + 失效新旧区域 */
static void sc_dropdown_set_expanded(sc_obj_t *obj, bool expanded)
{
    sc_dropdown_t *props = (sc_dropdown_t *)obj->props;
    if (props->expanded == expanded)
    {
        return;
    }
    uint16_t list_h = sc_dropdown_total_h(props);
    if (expanded)
    {
        /* 展开：新区域包含旧区域，修改后失效1次即可 */
        obj->view.h = (uint16_t)(obj->view.h + list_h + SC_DROPDOWN_GAP);
        props->expanded = 1U;
        obj->flags |= SC_OBJ_F_TOP;
        sc_obj_invalidate_area(obj, NULL);
    }
    else
    {
        /* 收起：旧区域大，先失效旧区域（已包含新区域） */
        sc_obj_invalidate_area(obj, NULL);
        obj->view.h = (uint16_t)(obj->view.h - list_h - SC_DROPDOWN_GAP);
        props->expanded = 0U;
        obj->flags &= (uint8_t)~SC_OBJ_F_TOP;
    }
}

void sc_dropdown_set_selected(sc_obj_t *obj, uint8_t idx)
{
    sc_dropdown_t *props = (sc_dropdown_t *)obj->props;
    if (idx < props->items_cnt && props->selected != idx)
    {
        props->selected = idx;
        sc_obj_invalidate_area(obj, NULL);
    }
}

void sc_dropdown_set_items(sc_obj_t *obj, const char **items, uint8_t cnt)
{
    sc_dropdown_t *props = (sc_dropdown_t *)obj->props;
    props->items = items;
    props->items_cnt = cnt;
    props->yofs = 0;
    /* 默认最多显示4条，超出可滚动 */
    if (props->list_h == 0)
    {
        uint8_t show = cnt < 4 ? cnt : 4;
        props->list_h = (uint16_t)(sc_dropdown_pitch(props) * show);
    }
    if (props->selected >= cnt)
    {
        props->selected = 0;
    }
    sc_obj_invalidate_area(obj, NULL);
}

bool sc_dropdown_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_dropdown_t *props = (sc_dropdown_t *)obj->props;
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
    case SC_STYLE_RADIUS:
        props->radius = *(const uint8_t *)value;
        return true;
    case SC_STYLE_CALLBACK:
        props->cb = *(const sc_obj_cb_t *)value;
        return true;
    case SC_STYLE_RADIUS_READ:
        *(uint8_t *)value = ((const sc_dropdown_t *)obj->props)->radius;
        return true;

    default:
        return false;
    }
}

/* 获取展开列表区域 */
static area_t sc_dropdown_list_area(sc_obj_t *obj)
{
    uint16_t closed_h = sc_dropdown_closed_h(obj);
    area_t a = {obj->view.x, (int16_t)(obj->view.y + closed_h + SC_DROPDOWN_GAP),
                (int16_t)(obj->view.x + obj->view.w),
                (int16_t)(obj->view.y + obj->view.h)};
    return a;
}

bool sc_obj_handle_dropdown_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_dropdown_t *props = (sc_dropdown_t *)obj->props;
    bool ret = false;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_dropdown((sc_pfb_t *)event->dat.arg, props, obj->view, 1U);           
        return true;
    }
#if SC_TOUCH_ENABLE
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        int16_t ty = event->dat.pos[1];
        if (!props->expanded)
        {
            /* 收起时：上层已判断命中view，直接展开 */
            sc_dropdown_set_expanded(obj, true);
            ret = true;
        }
        else
        {
            /* 展开时：上层已判断命中view，区分主框/列表 */
            uint16_t closed_h = sc_dropdown_closed_h(obj);
            if (ty < obj->view.y + closed_h)
            {
                /* 命中主框：收起 */
                sc_dropdown_set_expanded(obj, false);
            }
            else
            {
                /* 命中列表区域：开始拖动 */
                g_gui->flags |= SC_RT_DRAGGING;
                g_gui->drag_offset_y = ty;
                obj->flags |= SC_OBJ_F_PRESSED;
            }
            ret = true;
        }
    }
    else if (event->type == SC_EVENT_TOUCH_MOVE && (g_gui->flags & SC_RT_DRAGGING) != 0U)
    {
        /* 帧位移 = 当前位置 - 上一帧位置（last_pos 由顶层在 MOVE 后更新） */
        int16_t dy = (int16_t)(event->dat.pos[1] - g_gui->last_pos_y);
        if (dy != 0)
        {
            /* 移动了，清除 PRESSED（释放时不选中） */
            obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
            /* 滚动列表：上下各半个 pitch 弹性空间 */
            int pitch = sc_dropdown_pitch(props);
            int total_h = pitch * props->items_cnt;
            int list_h = sc_dropdown_total_h(props);
            int16_t min_yofs, max_yofs;
            sc_scroll_range(total_h, list_h, pitch, &min_yofs, &max_yofs);
            props->yofs = (int16_t)SC_CLAMP(props->yofs + dy, min_yofs, max_yofs);
            sc_obj_invalidate_area(obj, NULL);
        }
        ret = true;
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        g_gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        if ((obj->flags & SC_OBJ_F_PRESSED) != 0U)
        {
            /* 没有移动，释放时选中 */
            obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
            area_t list = sc_dropdown_list_area(obj);
            int16_t ty = event->dat.pos[1];
            if (ty >= list.ys && ty < list.ye)
            {
                int pitch = sc_dropdown_pitch(props);
                int idx = (ty - list.ys - props->yofs) / pitch;
                if (idx >= 0 && idx < props->items_cnt)
                {
                    props->selected = (uint8_t)idx;
                    sc_dropdown_set_expanded(obj, false);  //选中项后收起
                }
            }
            ret = true;
        }
    }
#endif
    if (event->type == SC_EVENT_KEY_DOWN)
    {
        if (event->dat.key == CMD_ENTER)
        {
            sc_dropdown_set_expanded(obj, !props->expanded);
            ret = true;
        }
        else if (props->expanded && (event->dat.key == CMD_UP || event->dat.key == CMD_DOWN))
        {
            /* 展开时上下键移动选中项 */
            int dir = (event->dat.key == CMD_DOWN) ? 1 : -1;
            int new_sel = (int)props->selected + dir;
            if (new_sel < 0) new_sel = 0;
            if (new_sel >= props->items_cnt) new_sel = props->items_cnt - 1;
            if (new_sel != props->selected)
            {
                props->selected = (uint8_t)new_sel;
                /* 行填满 pitch：item_h 传 pitch，与旧滚动行为一致 */
                sc_list_ensure_visible(&props->yofs, sc_dropdown_pitch(props),
                                       sc_dropdown_pitch(props),
                                       sc_dropdown_total_h(props), props->items_cnt, new_sel);
                sc_obj_invalidate_area(obj, NULL);
            }
            ret = true;
        }
    }
    if (props->cb)
    {
        props->cb(obj, event);
    }
    return ret;
}

#endif /* SC_USER_OBJ && SC_WIDGET_DROPDOWN_ENABLE */

#endif /* SC_WIDGET_DROPDOWN_ENABLE */
