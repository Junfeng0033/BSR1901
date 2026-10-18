#include "../sc_draw.h"
#include "sc_numeric_keyboard.h"
#include "sc_line_edit.h"

#if SC_USER_OBJ && SC_WIDGET_COMPOSITE_ENABLE && SC_WIDGET_NUMERIC_KEYBOARD_ENABLE

static const char k_numeric_key_text[SC_NUMERIC_KEY_COUNT][3] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "BS", "0", "OK"};

static sc_numeric_keyboard_t k_auto_keyboard_props;
static sc_obj_t *k_auto_keyboard_obj;

/* Map a focus cell index to its key value (digit, back, or enter). */
static uint8_t sc_numeric_keyboard_key(uint8_t focus_idx)
{
    if (focus_idx < 9U)
    {
        return (uint8_t)(focus_idx + 1U);
    }
    if (focus_idx == 10U)
    {
        return 0U;
    }
    return focus_idx == 9U ? SC_NUMERIC_KEY_BACK : SC_NUMERIC_KEY_ENTER;
}

/* Compute the rectangle of a key cell in the 3x4 grid, absorbing rounding at edges. */
static rect_t sc_numeric_keyboard_item_view(const sc_obj_t *obj, uint8_t item_idx)
{
    uint16_t cell_w = (uint16_t)(obj->view.w / 3U);
    uint16_t cell_h = (uint16_t)(obj->view.h / 4U);
    uint8_t col = (uint8_t)(item_idx % 3U);
    uint8_t row = (uint8_t)(item_idx / 3U);
    return (rect_t){
        (int16_t)(obj->view.x + col * cell_w),
        (int16_t)(obj->view.y + row * cell_h),
        col == 2U ? (uint16_t)(obj->view.w - cell_w * 2U) : cell_w,
        row == 3U ? (uint16_t)(obj->view.h - cell_h * 3U) : cell_h,
    };
}

#if SC_TOUCH_ENABLE
/* Hit-test a point to a grid cell index, or -1 if outside the keyboard. */
static int8_t sc_numeric_keyboard_hit(const sc_obj_t *obj, int16_t x, int16_t y)
{
    if (x < obj->view.x || y < obj->view.y ||
        x >= obj->view.x + (int16_t)obj->view.w || y >= obj->view.y + (int16_t)obj->view.h)
    {
        return -1;
    }
    uint16_t cell_w = (uint16_t)(obj->view.w / 3U);
    uint16_t cell_h = (uint16_t)(obj->view.h / 4U);
    if (cell_w == 0U || cell_h == 0U)
    {
        return -1;
    }
    int16_t local_x = (int16_t)(x - obj->view.x);
    int16_t local_y = (int16_t)(y - obj->view.y);
    uint8_t col = (uint8_t)(local_x / (int16_t)cell_w);
    uint8_t row = (uint8_t)(local_y / (int16_t)cell_h);
    if (col > 2U)
    {
        col = 2U;
    }
    if (row > 3U)
    {
        row = 3U;
    }
    return (int8_t)(row * 3U + col);
}
#endif

/* Invalidate (mark for redraw) the screen area of a single key cell. */
static void sc_numeric_keyboard_invalidate(sc_obj_t *obj, uint8_t key_id)
{
    area_t area;
    if (key_id >= SC_NUMERIC_KEY_COUNT)
    {
        return;
    }
    uint16_t cell_w = (uint16_t)(obj->view.w / 3U);
    uint16_t cell_h = (uint16_t)(obj->view.h / 4U);
    if (cell_w == 0U || cell_h == 0U)
    {
        return;
    }
    uint8_t col = (uint8_t)(key_id % 3U);
    uint8_t row = (uint8_t)(key_id / 3U);
    area = rect_to_area((rect_t){
        (int16_t)(obj->view.x + col * cell_w),
        (int16_t)(obj->view.y + row * cell_h),
        col == 2U ? (uint16_t)(obj->view.w - cell_w * 2U) : cell_w,
        row == 3U ? (uint16_t)(obj->view.h - cell_h * 3U) : cell_h});
    sc_obj_invalidate_area(obj, &area);
}

/* Change the focused key, invalidating both old and new cells; false if unchanged. */
static bool sc_numeric_keyboard_set_focus(sc_obj_t *obj, sc_composite_t *composite, uint8_t focus_idx)
{
    uint8_t old_focus = composite->focus_idx;
    if (focus_idx >= composite->item_count || old_focus == focus_idx)
    {
        return false;
    }
    composite->focus_idx = focus_idx;
    sc_numeric_keyboard_invalidate(obj, old_focus);
    sc_numeric_keyboard_invalidate(obj, focus_idx);
    return true;
}

/* Convert a numeric key value (0-9) to its corresponding CMD_* command code. */
static int sc_numeric_keyboard_digit_cmd(uint8_t key)
{
    if (key == 0U)
    {
        return CMD_0;
    }
    return key <= 9U ? CMD_1 + key - 1U : CMD_NONE;
}

/* Detach the keyboard from its line edit, clear the active flag, and delete it. */
static void sc_numeric_keyboard_close_edit(sc_obj_t *obj, sc_numeric_keyboard_t *props)
{
    sc_obj_t *edit = props->edit;
    if (edit)
    {
        ((sc_line_edit_t *)edit->props)->keyboard = NULL;
        sc_obj_invalidate(edit);
    }
    props->edit = NULL;
    k_auto_keyboard_obj = NULL;
    g_gui->flags &= (uint8_t)~SC_RT_KEYBOARD_ACTIVE;
    /* 回车关闭删除前强制整对象失效（sc_obj_del 内部失效会被 INVALIDATED 守卫挡掉）。 */
    sc_obj_invalidate_area(obj, NULL);
    sc_obj_del(obj);
}

/* Activate the focused key: dispatch input to the bound edit (or close on enter),
   otherwise invoke the standalone user callback. */
static void sc_numeric_keyboard_activate(sc_obj_t *obj, sc_composite_t *composite)
{
    sc_numeric_keyboard_t *props = (sc_numeric_keyboard_t *)composite->user;
    uint8_t key = sc_numeric_keyboard_key(composite->focus_idx);
    if (props->edit)
    {
        sc_event_t event;
        if (key == SC_NUMERIC_KEY_ENTER)
        {
            sc_numeric_keyboard_close_edit(obj, props);
            return;
        }
        event = (sc_event_t){
            .type = SC_EVENT_KEY_DOWN,
            .dat.key = key == SC_NUMERIC_KEY_BACK ? CMD_BACK : sc_numeric_keyboard_digit_cmd(key),
        };
        sc_obj_dispatch_event(props->edit, &event);
        event.type = SC_EVENT_KEY_UP;
        sc_obj_dispatch_event(props->edit, &event);
        return;
    }
    if (props->cb && composite->focus_idx < SC_NUMERIC_KEY_COUNT)
    {
        props->cb(obj, key);
    }
}

/* Draw all keys as buttons in the 3x4 grid, highlighting the focused/pressed key. */
static void sc_numeric_keyboard_draw(sc_pfb_t *pfb, const sc_obj_t *obj, const sc_composite_t *composite)
{
    const sc_numeric_keyboard_t *props = (const sc_numeric_keyboard_t *)composite->user;
    sc_button_t key = SC_BUTTON_NEW(NULL, props->fg, props->bg, composite->font_id, ALIGN_CENTER);
    uint16_t cell_w = (uint16_t)(obj->view.w / 3U);
    uint16_t cell_h = (uint16_t)(obj->view.h / 4U);
    for (uint8_t i = 0U; i < SC_NUMERIC_KEY_COUNT; ++i)
    {
        uint8_t col = (uint8_t)(i % 3U);
        uint8_t row = (uint8_t)(i / 3U);
        uint16_t w = col == 2U ? (uint16_t)(obj->view.w - cell_w * 2U) : cell_w;
        uint16_t h = row == 3U ? (uint16_t)(obj->view.h - cell_h * 3U) : cell_h;
        key.text = k_numeric_key_text[i];
        key.bg = i == composite->focus_idx ? props->focus_bg : props->bg;
        sc_lcd_draw_button(pfb,
                           (int16_t)(obj->view.x + col * cell_w),
                           (int16_t)(obj->view.y + row * cell_h),
                           w, h, &key,
                           i == composite->focus_idx && (obj->flags & SC_OBJ_F_PRESSED) != 0U);
    }
}

/* Handle keyboard events: arrow-key focus navigation, enter press/activate, and
   touch down/move/up selection and activation. */
static bool sc_numeric_keyboard_event(sc_obj_t *obj, sc_composite_t *composite, const sc_event_t *event)
{
    uint8_t focus = composite->focus_idx;
    if (event->type == SC_EVENT_KEY_DOWN)
    {
        switch (event->dat.key)
        {
        case CMD_LEFT:
        case CMD_RIGHT:
            /* wrap=false：9宫格档2负责正常换行（行尾->下一行最左），
             * 到头无候选时 focus 不变 -> return false 上抛回到绑定的 edit。
             * 与 UP/DOWN 行为一致，不再循环回绕。 */
            focus = sc_composite_focus_direction(obj, event->dat.key,
                                                 sc_numeric_keyboard_item_view, false);
            if (focus == composite->focus_idx)
            {
                return false;
            }
            break;
        case CMD_UP:
        case CMD_DOWN:
            focus = sc_composite_focus_direction(obj, event->dat.key,
                                                 sc_numeric_keyboard_item_view, false);
            if (focus == composite->focus_idx)
            {
                return false;
            }
            break;
        case CMD_ENTER:
            if ((obj->flags & SC_OBJ_F_PRESSED) == 0U)
            {
                obj->flags |= SC_OBJ_F_PRESSED;
                sc_numeric_keyboard_invalidate(obj, focus);
            }
            return true;
        default:
            return false;
        }
        sc_numeric_keyboard_set_focus(obj, composite, focus);
        return true;
    }
    if (event->type == SC_EVENT_KEY_UP && event->dat.key == CMD_ENTER)
    {
        if ((obj->flags & SC_OBJ_F_PRESSED) != 0U)
        {
            obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
            sc_numeric_keyboard_invalidate(obj, composite->focus_idx);
            sc_numeric_keyboard_activate(obj, composite);
        }
        return true;
    }
#if SC_TOUCH_ENABLE
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        int8_t hit = sc_numeric_keyboard_hit(obj, event->dat.pos[0], event->dat.pos[1]);
        if (hit < 0)
        {
            return false;
        }
        if (!sc_numeric_keyboard_set_focus(obj, composite, (uint8_t)hit))
        {
            sc_numeric_keyboard_invalidate(obj, (uint8_t)hit);
        }
        obj->flags |= SC_OBJ_F_PRESSED;
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_MOVE)
    {
        int8_t hit = sc_numeric_keyboard_hit(obj, event->dat.pos[0], event->dat.pos[1]);
        if (hit >= 0)
        {
            sc_numeric_keyboard_set_focus(obj, composite, (uint8_t)hit);
        }
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_UP)
    {
        int8_t hit = sc_numeric_keyboard_hit(obj, event->dat.pos[0], event->dat.pos[1]);
        bool activate = hit >= 0 && (uint8_t)hit == composite->focus_idx &&
                        (obj->flags & SC_OBJ_F_PRESSED) != 0U;
        obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
        sc_numeric_keyboard_invalidate(obj, composite->focus_idx);
        if (activate)
        {
            sc_numeric_keyboard_activate(obj, composite);
        }
        return true;
    }
#endif
    return false;
}

/* Apply a style property (fg, bg, focus bg, font) to the numeric keyboard. */
static bool sc_numeric_keyboard_style(sc_obj_t *obj, sc_composite_t *composite,
                                      sc_style_t style, const void *value)
{
    sc_numeric_keyboard_t *props = (sc_numeric_keyboard_t *)composite->user;
    (void)obj;
    switch (style)
    {
    case SC_STYLE_FG:
        props->fg = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->bg = *(const color_t *)value;
        return true;
    case SC_STYLE_ON:
        props->focus_bg = *(const color_t *)value;
        return true;
    case SC_STYLE_FONT:
        composite->font_id = *(const uint8_t *)value;
        return true;
    default:
        return false;
    }
}

static const sc_composite_hooks_t k_numeric_keyboard_hooks = {
    .draw = sc_numeric_keyboard_draw,
    .event = sc_numeric_keyboard_event,
    .style = sc_numeric_keyboard_style,
};

/* Create a numeric keyboard composite object with default colors and callback. */
sc_obj_t *sc_numeric_keyboard_create(sc_obj_t *parent, sc_numeric_keyboard_t *props,
                                     sc_numeric_keyboard_cb_t cb)
{
    if (!props)
    {
        return NULL;
    }
    *props = (sc_numeric_keyboard_t){
        .cb = cb,
        .fg = C_WHITE,
        .bg = C_RGB(32, 40, 56),
        .focus_bg = C_RGB(48, 112, 192),
    };
    return sc_composite_create(parent, &props->composite, &k_numeric_keyboard_hooks,
                               props, SC_NUMERIC_KEY_COUNT);
}

/* Open (or reuse) the shared auto keyboard bound to the given line edit, position
   it at the bottom half of the screen, and return the keyboard object. */
sc_obj_t *sc_numeric_keyboard_open_for_edit(sc_obj_t *edit)
{
    if (!edit)
    {
        return NULL;
    }
    sc_line_edit_t *edit_props = (sc_line_edit_t *)edit->props;
    if ((g_gui->flags & SC_RT_KEYBOARD_ACTIVE) != 0U &&
        k_auto_keyboard_obj && k_auto_keyboard_obj->props == &k_auto_keyboard_props)
    {
        if (k_auto_keyboard_props.edit && k_auto_keyboard_props.edit != edit)
        {
            ((sc_line_edit_t *)k_auto_keyboard_props.edit->props)->keyboard = NULL;
        }
        k_auto_keyboard_props.edit = edit;
        edit_props->keyboard = k_auto_keyboard_obj;
        sc_obj_invalidate(edit);
        return k_auto_keyboard_obj;
    }
    k_auto_keyboard_obj = sc_numeric_keyboard_create(NULL, &k_auto_keyboard_props, NULL);
    if (!k_auto_keyboard_obj)
    {
        g_gui->flags &= (uint8_t)~SC_RT_KEYBOARD_ACTIVE;
        return NULL;
    }
    uint16_t height = (uint16_t)(g_gui->screen.h / 2);
    sc_obj_set_rect(k_auto_keyboard_obj,
                    0,
                    (int16_t)(g_gui->screen.h - height),
                    g_gui->screen.w,
                    height,
                    ALIGN_NONE);
    k_auto_keyboard_props.edit = edit;
    edit_props->keyboard = k_auto_keyboard_obj;
    g_gui->flags |= SC_RT_KEYBOARD_ACTIVE;
    sc_obj_invalidate(edit);
    return k_auto_keyboard_obj;
}

#endif
