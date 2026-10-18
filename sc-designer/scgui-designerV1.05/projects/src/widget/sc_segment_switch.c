#include "../sc_draw.h"
#include "../sc_event.h"
#include "sc_segment_switch.h"
#include "sc_round_rect.h"

#if SC_WIDGET_SEGMENT_SWITCH_ENABLE

/* 选中色块拨动动画参数：16ms 间隔（~60fps），Q8 缓动系数 0.25。 */
#define SC_SEGMENT_ANIM_MS       16U
#define SC_SEGMENT_ANIM_GAIN_Q8  84

/* 动画定时器回调：缓动将 anim_x 移向目标位置，到达后停止。 */
static void sc_segment_switch_anim_timer(sc_delay_t *timer)
{
    sc_obj_t *obj = (sc_obj_t *)timer->arg;
    sc_segment_switch_t *props = (sc_segment_switch_t *)obj->props;
    if (!props || props->seg_count == 0) return;

    int inner_w = obj->view.w - 2 * (int)props->border;
    int seg_w = inner_w / props->seg_count;
    int target_x = (int)props->selected * seg_w;
    int err = target_x - props->anim_x;
    if (err == 0)
    {
        /* 已到达目标：同步视觉选中段（若尚未同步），重绘一次后停止。 */
        if (props->vis_selected != props->selected)
        {
            props->vis_selected = props->selected;
            sc_obj_invalidate(obj);
        }
        return;
    }

    /* 缓动：误差 <=2 直接到位，否则每帧移动 err*0.25（至少 1 像素）。 */
    int dx = (SC_ABS(err) <= 2)
                 ? err
                 : (SC_MAX((SC_ABS(err) * SC_SEGMENT_ANIM_GAIN_Q8) >> 8, 1) *
                    ((err > 0) - (err < 0)));
    props->anim_x = (int16_t)(props->anim_x + dx);
    sc_obj_invalidate(obj);
    sc_reload_delay(timer, SC_SEGMENT_ANIM_MS);
}

/* 绘制多段开关：轨道背景 + 选中段高亮 + 段标签文字 */
static void sc_draw_segment_switch(sc_pfb_t *pfb, const sc_segment_switch_t *props,
                                    rect_t view, int border)
{
    /* 半径：props->radius 未设(0)时用胶囊形（高度一半）。 */
    int r = props->radius ? props->radius : (view.h >> 1);
    int bw = (int)props->border;


    /* 1. 绘制整个轨道背景（圆角矩形，未选中色）。
     *    外框不相交 pfb 时直接返回，跳过后续绘制。 */
    rect_t track = {view.x, view.y, view.w, view.h};
    if (!sc_draw_Rounded_mask(pfb, track, r, bw,
                              props->track_color, props->track_color,
                              255U, 0, NULL))
    {
        return;
    }
    int inner_x = view.x + bw;
    int inner_y = view.y + bw;
    int inner_w = view.w - 2 * bw;
    int inner_h = view.h - 2 * bw;
    /* 2. 计算每段宽度，绘制选中段高亮背景。
     *    最后一段用剩余宽度，避免不整除时右边不对齐。 */
    if (props->seg_count > 0 && props->selected < props->seg_count && props->items)
    {
        int seg_w = inner_w / props->seg_count;
        /* 选中段 x 坐标：动画中用 anim_x，未初始化时用 selected 位置。 */
        int cur_x = (props->anim_x >= 0) ? props->anim_x : (props->selected * seg_w);
        int sel_x = inner_x + cur_x;
        int sel_w = (props->selected == props->seg_count - 1)
                        ? (inner_w - props->selected * seg_w)
                        : seg_w;
        /* 选中段高亮背景（每档独立颜色）。 */
        color_t sel_color = props->items[props->selected].selected_color;
        rect_t sel = {(int16_t)sel_x, (int16_t)inner_y, (uint16_t)sel_w, (uint16_t)inner_h};
        sc_draw_Rounded_mask(pfb, sel, r, 0,
                             sel_color, sel_color,
                             255U, 0, NULL);
    }

    /* 3. 绘制每段的文字标签（居中对齐）。
     *    最后一段用剩余宽度，避免不整除时右边不对齐。
     *    动画中 vis_selected=0xFF，所有文字用未选中色；结束后目标段用该档 selected_text_color。 */
    if (props->items && props->seg_count > 0)
    {
        int seg_w = inner_w / props->seg_count;
        uint8_t font_id = props->font_id;
        for (uint8_t i = 0; i < props->seg_count; i++)
        {
            const char *label = props->items[i].label;
            if (!label || !label[0]) continue;
            int seg_x = inner_x + i * seg_w;
            int seg_w_i = (i == props->seg_count - 1) ? (inner_w - i * seg_w) : seg_w;
            rect_t label_view = {(int16_t)seg_x, (int16_t)inner_y,
                                 (uint16_t)seg_w_i, (uint16_t)inner_h};
            color_t fg = (i == props->vis_selected)
                             ? props->items[i].selected_text_color
                             : props->text_color;
            sc_draw_text_aligned(pfb, label_view, label, font_id, 0,
                                 fg, NULL, ALIGN_CENTER, 0, 255U, 0);
        }
    }
}

/* 在指定矩形绘制多段开关，支持 pfb 帧缓冲或 LCD 直接绘制。 */
void sc_lcd_draw_segment_switch(sc_pfb_t *pfb, int16_t x, int16_t y,
                                 uint16_t w, uint16_t h,
                                 const sc_segment_switch_t *props, int border)
{
    rect_t view = {x, y, w, h};
    if (!props)
    {
        return;
    }
    if (pfb)
    {
        sc_draw_segment_switch(pfb, props, view, border);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view),
                sc_draw_segment_switch(sc_lcd_draw_pfb__, props, view, border));
}

#if SC_USER_OBJ

/* 控件虚表：事件回调 + 样式设置。 */
static const sc_obj_vt_t sc_segment_switch_vt = {
    .event_cb = sc_obj_handle_segment_switch_event,
    .set_style = sc_segment_switch_style,
};

/* 创建多段开关控件，props 必须为有效指针（全局/静态变量）。
 * create 时先用 SC_SEGMENT_SWITCH_NEW 初始化默认值，再由 api 加载 items。 */
sc_obj_t *sc_segment_switch_create(sc_obj_t *parent, sc_segment_switch_t *props)
{
    *props = (sc_segment_switch_t)SC_SEGMENT_SWITCH_NEW(NULL, 0U, 0U);
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_SEGMENT_SWITCH,
                                   &sc_segment_switch_vt, props);
    if (obj)
    {
        obj->flags |= (uint8_t)SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

/* 设置段配置数组（label + selected_color + selected_text_color），参考 sc_list_items_set_data。 */
void sc_segment_switch_set_items(sc_obj_t *obj, const sc_segment_item_t *items, uint8_t count)
{
    if (!obj || sc_obj_type(obj) != SC_OBJ_SEGMENT_SWITCH) return;
    sc_segment_switch_t *props = (sc_segment_switch_t *)obj->props;
    if (!props) return;
    props->items = items;
    props->seg_count = count;
    if (props->selected >= count)
    {
        props->selected = 0U;
        props->vis_selected = 0U;
    }
    sc_obj_invalidate(obj);
}

/* 设置当前选中段索引，启动拨动动画并触发回调。 */
void sc_segment_switch_set_selected(sc_obj_t *obj, uint8_t index)
{
    if (!obj || sc_obj_type(obj) != SC_OBJ_SEGMENT_SWITCH) return;
    sc_segment_switch_t *props = (sc_segment_switch_t *)obj->props;
    if (!props || index >= props->seg_count) return;
    if (props->selected != index)
    {
        uint8_t old_selected = props->selected;
        props->selected = index;
        if (props->anim_x < 0)
        {
            /* 首次切换：先初始化到旧 selected 位置，再动画滑到新位置。 */
            int inner_w = obj->view.w - 2 * (int)props->border;
            int seg_w = inner_w / props->seg_count;
            props->anim_x = (int16_t)(old_selected * seg_w);
        }
        /* 启动缓动动画定时器：动画过程中 vis_selected=0xFF，所有文字用未选中色。 */
        props->vis_selected = 0xFFU;
        sc_add_delay(sc_segment_switch_anim_timer, SC_SEGMENT_ANIM_MS, obj);
        sc_obj_invalidate(obj);
        if (props->cb)
        {
            /* 构造用户事件通知选中段变化，选中段索引放在 dat.cmd 中。 */
            sc_event_t evt = {0};
            evt.type = SC_EVENT_USER;
            evt.user_type = 0x01; /* 0x01 = 选中段变化 */
            evt.dat.cmd = index;
            props->cb(obj, &evt);
        }
    }
}

/* 样式设置：支持颜色、选中段、段数等属性。 */
bool sc_segment_switch_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    if (!obj || sc_obj_type(obj) != SC_OBJ_SEGMENT_SWITCH) return false;
    sc_segment_switch_t *props = (sc_segment_switch_t *)obj->props;
    if (!props) return false;
    switch (style)
    {
    case SC_STYLE_BG:
        props->track_color = *(const color_t *)value;
        break;
    case SC_STYLE_FG:
        props->text_color = *(const color_t *)value;
        break;
    case SC_STYLE_ON:
        sc_segment_switch_set_selected(obj, *(const uint8_t *)value);
        return true;  /* set_selected 已触发重绘 */
    case SC_STYLE_BORDER_WIDTH:
        props->border = *(const uint8_t *)value;
        break;
    case SC_STYLE_RADIUS:
        props->radius = *(const uint8_t *)value;
        break;
    case SC_STYLE_RADIUS_READ:
        *(uint8_t *)value = ((const sc_segment_switch_t *)obj->props)->radius;
        return true;

    default:
        return false;
    }
    sc_obj_invalidate(obj);
    return true;
}

/* 事件处理：绘制 + 触摸切换选中段。 */
bool sc_obj_handle_segment_switch_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (!obj || sc_obj_type(obj) != SC_OBJ_SEGMENT_SWITCH) return false;
    sc_segment_switch_t *props = (sc_segment_switch_t *)obj->props;
    if (!props || props->seg_count == 0) return false;

    /* 绘制事件：调用绘制函数，pfb 从 event->dat.arg 获取。 */
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_draw_segment_switch((sc_pfb_t *)event->dat.arg, props, obj->view,
                               (obj->flags & SC_OBJ_F_PRESSED) ? 2U : 1U);
        return true;
    }

    /* Key events: ENTER toggles edit mode; LEFT/RIGHT step the selection in
       edit mode (same convention as sc_progress). */
    if (event->type == SC_EVENT_KEY_DOWN)
    {
        if (event->dat.key == CMD_ENTER)
        {
            g_gui->flags ^= SC_RT_KEY_EDIT; /* enter/exit edit mode */
        sc_obj_invalidate(obj); /* 1H�Z&F�s�r */
            return true;
        }
        if ((g_gui->flags & SC_RT_KEY_EDIT) == 0U)
        {
            return false; /* step keys only in edit mode */
        }
        if (event->dat.key == CMD_LEFT)
        {
            if (props->selected > 0U)
            {
                sc_segment_switch_set_selected(obj, (uint8_t)(props->selected - 1U));
            }
            return true;
        }
        if (event->dat.key == CMD_RIGHT)
        {
            if (props->selected + 1U < props->seg_count)
            {
                sc_segment_switch_set_selected(obj, (uint8_t)(props->selected + 1U));
            }
            return true;
        }
        return false;
    }

    /* 触摸按下：设置 pressed 状态。 */
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        if ((obj->flags & SC_OBJ_F_PRESSED) == 0U)
        {
            obj->flags |= SC_OBJ_F_PRESSED;
            sc_obj_invalidate(obj);
        }
        return true;
    }

    /* 触摸抬起：根据 x 坐标计算选中段并切换。 */
    if (event->type == SC_EVENT_TOUCH_UP)
    {
        if ((obj->flags & SC_OBJ_F_PRESSED) != 0U)
        {
            obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
            /* 触摸坐标是屏幕绝对坐标，减去对象 view.x 得到相对坐标。 */
            int16_t click_x = event->dat.pos[0] - obj->view.x;
            int seg_w = obj->view.w / props->seg_count;
            if (seg_w > 0)
            {
                uint8_t index = (uint8_t)(click_x / seg_w);
                if (index >= props->seg_count) index = props->seg_count - 1;
                sc_segment_switch_set_selected(obj, index);
            }
            sc_obj_invalidate(obj);
        }
        return true;
    }

    return false;
}

#endif /* SC_USER_OBJ */
#endif /* SC_WIDGET_SEGMENT_SWITCH_ENABLE */
