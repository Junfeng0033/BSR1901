
#include "sc_gui.h"

/* 全局卡片对象指针（create 时设置，回调通过 obj->props 获取 ctrl） */
static sc_obj_t *g_dropdown_obj = NULL;

#if SC_WIDGET_DROPDOWN_CARD_ENABLE

/* ===== 动画：移动卡片到目标位置 ===== */
static void dropdown_card_animate(int16_t to_y)
{
    sc_anim_t *anim = sc_anim_create(g_dropdown_obj);
    sc_anim_set_move_xy(anim, SC_ANIM_KEEP, to_y);
    sc_anim_config(anim, SC_ANIM_CURVE_EASE, 200, 0);
    sc_anim_start(anim);
}

void sc_dropdown_card_show(sc_obj_t *obj, bool show, bool animated)
{
    if (!obj)
        return;
    sc_dropdown_card_t *ctrl = (sc_dropdown_card_t *)obj->props;
    // 初始化shown_y为当前位置，避免动画时位置错误
    if (ctrl->shown_y == 0 && g_dropdown_obj->view.y >= 0)
        ctrl->shown_y = g_dropdown_obj->view.y;
    int16_t to_y = show ? ctrl->shown_y : (int16_t)(-obj->view.h);
    if (to_y != obj->view.y)
    {
        if (animated)
        {
            dropdown_card_animate(to_y);
        }
        else
        {
            int16_t move_dy = (int16_t)(to_y - obj->view.y);
            sc_obj_move(obj, 0, move_dy);
        }
    }
}

#if SC_TOUCH_ENABLE
/* ===== 核心手势处理：统一签名 bool (*)(const sc_event_t *)，screen/root 回调共用 ===== */
static bool dropdown_card_handle_event(const sc_event_t *event)
{
    sc_dropdown_card_t *ctrl = (sc_dropdown_card_t *)g_dropdown_obj->props;
    if (!ctrl)
        return false;
    int16_t y = event->dat.pos[1];
    bool handled = false;
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        sc_anim_stop(g_dropdown_obj);
        ctrl->target_y = g_dropdown_obj->view.y; /* 手势起点卡片位置（同容器 target） */
        handled = true;
    }
    else if (event->type == SC_EVENT_TOUCH_MOVE)
    {
        /* 帧位移模式：主轴判断用总位移(从起点算，准确)，
         * 实际移动用帧位移(从上一帧算，效率高，不从起点重算)。
         * last_pos_y 由顶层在 MOVE 后更新，代替旧版卡片私有 drag_offset_y。 */
        int16_t total_x = (int16_t)(event->dat.pos[0] - g_gui->drag_offset_x);
        int16_t total_y = (int16_t)(y - g_gui->drag_offset_y);
        int16_t abs_x = (total_x < 0) ? (int16_t)(-total_x) : total_x;
        int16_t abs_y = (total_y < 0) ? (int16_t)(-total_y) : total_y;
        if (abs_y > abs_x) /* 纵向为主（下拉/上拉）才跟手 */
        {
            /* total-displacement: last_pos_y updated before root_callback in
             * root mode -> frame-delta always 0; total works for both modes. */
            int16_t new_y = (int16_t)(ctrl->target_y + total_y);
            if (new_y > ctrl->shown_y) /* 下划不超过 shown_y（完全展开位置） */
                new_y = ctrl->shown_y;
            if (new_y < (int16_t)(-g_dropdown_obj->view.h)) /* 不拉出屏幕上方 */
                new_y = (int16_t)(-g_dropdown_obj->view.h);
            int16_t move_dy = (int16_t)(new_y - g_dropdown_obj->view.y);
            if (move_dy != 0)
                sc_obj_move(g_dropdown_obj, 0, move_dy);
            handled = true;
        }
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        /* 释放（容器式）：按「当前位置 - 手势起点」位移方向决定目标。
         * 上划超阈值→收起（-view.h），下划超阈值→展开（shown_y），未超阈值→回起点。 */
        int16_t delta = (int16_t)(g_dropdown_obj->view.y - ctrl->target_y);
        int16_t to_y;
        if (delta <= -SC_DROPDOWN_CARD_THRESHOLD)
            to_y = (int16_t)(-g_dropdown_obj->view.h); /* 上划 → 收起 */
        else if (delta >= SC_DROPDOWN_CARD_THRESHOLD)
            to_y = ctrl->shown_y; /* 下划 → 展开 */
        else
            to_y = ctrl->target_y; /* 未超阈值 → 回起点 */
        if (g_dropdown_obj->view.y != to_y)
            dropdown_card_animate(to_y);
        handled = true;
    }
    return handled;
}

/* 统一事件回调：screen 模式和 root 模式共用（root 模式 obj=NULL）。
 * 作为 sc_obj_cb_t（void）注册，内部转调 bool 的 handle_event。 */
static void dropdown_card_event_cb(sc_obj_t *obj, const sc_event_t *event)
{

    dropdown_card_handle_event(event);
    sc_dropdown_card_t *ctrl = (sc_dropdown_card_t *)g_dropdown_obj->props;
    if (ctrl && ctrl->orig_cb)
        ctrl->orig_cb(obj, event);
}

#endif /* SC_TOUCH_ENABLE */

#if SC_USER_OBJ
/* ===== 样式：颜色/填充/透明度/圆角/边框/直角掩码 ===== */
static bool sc_dropdown_card_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_dropdown_card_t *ctrl = (sc_dropdown_card_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
    case SC_STYLE_BORDER:
        ctrl->color = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        ctrl->fill = *(const color_t *)value;
        return true;
    case SC_STYLE_OPA:
        ctrl->alpha = *(const uint8_t *)value;
        return true;
    case SC_STYLE_RADIUS:
        ctrl->radius = *(const uint8_t *)value;
        return true;
    case SC_STYLE_BORDER_WIDTH:
        ctrl->border = *(const uint8_t *)value;
        return true;
    case SC_STYLE_RECT_MASK:
        ctrl->rect_mask = *(const uint8_t *)value;
        return true;
    case SC_STYLE_RADIUS_READ:
        *(uint8_t *)value = ((const sc_dropdown_card_t *)obj->props)->radius;
        return true;

    default:
        return false;
    }
}
/* ===== 绘制：复用 sc_draw_Rounded_texture（与 plain_rect 相同） ===== */
static bool sc_dropdown_card_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_pfb_t *pfb = (sc_pfb_t *)event->dat.arg;
        sc_dropdown_card_t *ctrl = (sc_dropdown_card_t *)obj->props;
        sc_draw_Rounded_texture(pfb, obj->view,
                                ctrl->radius, ctrl->border, ctrl->color, ctrl->fill,
                                ctrl->alpha, NULL, ctrl->rect_mask, NULL);
        return true;
    }
    return false;
}
static const sc_obj_vt_t sc_dropdown_card_vt = {
    .event_cb = sc_dropdown_card_event,
    .set_style = sc_dropdown_card_style,
};
/* ===== 创建：独立类型 SC_OBJ_DROPDOWN_CARD，自动注册 screen 回调 ===== */
sc_obj_t *sc_dropdown_card_create(sc_obj_t *screen, sc_dropdown_card_t *ctrl)
{
    /* 创建卡片对象（父级=NULL，与 screen 同级） */
    sc_obj_t *obj = sc_obj_create(NULL, SC_OBJ_DROPDOWN_CARD, &sc_dropdown_card_vt, ctrl);
    if (!obj)
        return NULL;
    g_dropdown_obj = obj;
    /* 默认样式 */
    ctrl->color = C_WHITE;
    ctrl->fill = C_BLUE;
    ctrl->radius = 8U;
    ctrl->border = 1U;
    ctrl->alpha = 255U;
    ctrl->rect_mask = 0U;
    ctrl->shown_y = 0;
    obj->flags &= (uint8_t)~SC_OBJ_F_FOCUSABLE;
    /* 绑定 screen 或 root，注册回调（保存原回调用于链式调用） */
    ctrl->screen = screen;
    if (screen)
    {
        if (sc_obj_type(screen) <= SC_OBJ_CONTAINER)
        {
            sc_container_t *container = (sc_container_t *)screen->props;
            ctrl->orig_cb = container->cb;
        }
        sc_obj_set_callback(screen, dropdown_card_event_cb);
    }
    else
    {
        ctrl->orig_cb = g_gui->root_callback;
        sc_root_set_callback(dropdown_card_event_cb);
    }

    return obj;
}

#endif /* SC_USER_OBJ */
#endif /* SC_WIDGET_DROPDOWN_CARD_ENABLE */
