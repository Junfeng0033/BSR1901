#include "../sc_draw.h"
#include "sc_button.h"
#include "sc_keyboard.h"
#include "sc_line_edit.h"

#if SC_USER_OBJ && SC_WIDGET_COMPOSITE_ENABLE && SC_WIDGET_KEYBOARD_ENABLE

/*
 * Constant layouts are adapted from LingDongGUI:
 * https://gitee.com/gzbkey/LingDongGUI
 * The runtime/event implementation is native to scgui's composite widget.
 */

#define SC_KEYBOARD_GRID_COLS 20U
#define SC_KEYBOARD_GRID_ROWS 4U
#define SC_KEYBOARD_KEY_GAP 1U
#define SC_KEYBOARD_EDIT_GAP 10U /* 弹键盘上移后，line_edit 底部与键盘顶界保留的间距 */
#define SC_KEY_ROW_MASK 0x03U
/* Pack a key's start row (bits 0-1) and row span (bits 2-3) into one byte. */
#define SC_KEY_ROW_INFO(row_, row_span_) ((row_) | ((row_span_) << 2))
#define SC_KEY_ROW(key_) ((uint8_t)((key_)->row_info & SC_KEY_ROW_MASK))
#define SC_KEY_ROW_SPAN(key_) ((uint8_t)((key_)->row_info >> 2))
/* Build a single-row key layout entry (row span of 1). */
#define SC_KEY(value_, x_, row_, span_) \
    {(value_), (x_), (span_), SC_KEY_ROW_INFO((row_), 1U)}
/* Build a key layout entry spanning multiple rows. */
#define SC_KEY_H(value_, x_, row_, span_, row_span_) \
    {(value_), (x_), (span_), SC_KEY_ROW_INFO((row_), (row_span_))}

static sc_keyboard_t k_auto_keyboard_props;

typedef struct sc_keyboard_key_t
{
    uint8_t value;
    uint8_t x;
    uint8_t span;
    uint8_t row_info; /* row: bits 0-1; row_span: bits 2-3 */
} sc_keyboard_key_t;

typedef struct sc_keyboard_layout_t
{
    const sc_keyboard_key_t *keys;
    uint8_t count;
} sc_keyboard_layout_t;

static const sc_keyboard_key_t k_number_keys[] = {
    SC_KEY('1', 0, 0, 4), SC_KEY('2', 4, 0, 4), SC_KEY('3', 8, 0, 4),
    SC_KEY('+', 12, 0, 4), SC_KEY(SC_KEYBOARD_KEY_BACKSPACE, 16, 0, 4),
    SC_KEY('4', 0, 1, 4), SC_KEY('5', 4, 1, 4), SC_KEY('6', 8, 1, 4),
    SC_KEY('-', 12, 1, 4), SC_KEY(SC_KEYBOARD_KEY_SYMBOL_MODE, 16, 1, 4),
    SC_KEY('7', 0, 2, 4), SC_KEY('8', 4, 2, 4), SC_KEY('9', 8, 2, 4),
    SC_KEY('*', 12, 2, 4), SC_KEY_H(SC_KEYBOARD_KEY_ENTER, 16, 2, 4, 2),
    SC_KEY(SC_KEYBOARD_KEY_ASCII_MODE, 0, 3, 4), SC_KEY('0', 4, 3, 4),
    SC_KEY('.', 8, 3, 4), SC_KEY('/', 12, 3, 4),
};

static const sc_keyboard_key_t k_ascii_keys[] = {
    SC_KEY('q', 0, 0, 2), SC_KEY('w', 2, 0, 2), SC_KEY('e', 4, 0, 2),
    SC_KEY('r', 6, 0, 2), SC_KEY('t', 8, 0, 2), SC_KEY('y', 10, 0, 2),
    SC_KEY('u', 12, 0, 2), SC_KEY('i', 14, 0, 2), SC_KEY('o', 16, 0, 2),
    SC_KEY('p', 18, 0, 2),
    SC_KEY('a', 0, 1, 2), SC_KEY('s', 2, 1, 2), SC_KEY('d', 4, 1, 2),
    SC_KEY('f', 6, 1, 2), SC_KEY('g', 8, 1, 2), SC_KEY('h', 10, 1, 2),
    SC_KEY('j', 12, 1, 2), SC_KEY('k', 14, 1, 2), SC_KEY('l', 16, 1, 2),
    SC_KEY(SC_KEYBOARD_KEY_BACKSPACE, 18, 1, 2),
    SC_KEY(SC_KEYBOARD_KEY_SYMBOL_MODE, 0, 2, 2), SC_KEY('z', 2, 2, 2),
    SC_KEY('x', 4, 2, 2), SC_KEY('c', 6, 2, 2), SC_KEY('v', 8, 2, 2),
    SC_KEY('b', 10, 2, 2), SC_KEY('n', 12, 2, 2), SC_KEY('m', 14, 2, 2),
    SC_KEY(SC_KEYBOARD_KEY_UP, 16, 2, 2), SC_KEY(SC_KEYBOARD_KEY_NEWLINE, 18, 2, 2),
    SC_KEY(SC_KEYBOARD_KEY_NUMBER_MODE, 0, 3, 2), SC_KEY(SC_KEYBOARD_KEY_SHIFT, 2, 3, 3),
    SC_KEY(' ', 5, 3, 5), SC_KEY(SC_KEYBOARD_KEY_ENTER, 10, 3, 4),
    SC_KEY(SC_KEYBOARD_KEY_LEFT, 14, 3, 2), SC_KEY(SC_KEYBOARD_KEY_DOWN, 16, 3, 2),
    SC_KEY(SC_KEYBOARD_KEY_RIGHT, 18, 3, 2),
};

static const sc_keyboard_key_t k_symbol_keys[] = {
    SC_KEY('-', 0, 0, 2), SC_KEY('/', 2, 0, 2), SC_KEY(':', 4, 0, 2),
    SC_KEY(';', 6, 0, 2), SC_KEY('(', 8, 0, 2), SC_KEY(')', 10, 0, 2),
    SC_KEY('_', 12, 0, 2), SC_KEY('$', 14, 0, 2), SC_KEY('&', 16, 0, 2),
    SC_KEY('"', 18, 0, 2),
    SC_KEY('[', 0, 1, 2), SC_KEY(']', 2, 1, 2), SC_KEY('{', 4, 1, 2),
    SC_KEY('}', 6, 1, 2), SC_KEY('#', 8, 1, 2), SC_KEY('%', 10, 1, 2),
    SC_KEY('^', 12, 1, 2), SC_KEY('*', 14, 1, 2), SC_KEY('+', 16, 1, 2),
    SC_KEY('=', 18, 1, 2),
    SC_KEY('\\', 0, 2, 2), SC_KEY('|', 2, 2, 2), SC_KEY('<', 4, 2, 2),
    SC_KEY('>', 6, 2, 2), SC_KEY('~', 8, 2, 2), SC_KEY(',', 10, 2, 2),
    SC_KEY('@', 12, 2, 2), SC_KEY('!', 14, 2, 2), SC_KEY('`', 16, 2, 2),
    SC_KEY('\'', 18, 2, 2),
    SC_KEY(SC_KEYBOARD_KEY_ASCII_MODE, 0, 3, 4), SC_KEY('.', 4, 3, 2),
    SC_KEY(' ', 6, 3, 8), SC_KEY('?', 14, 3, 2),
    SC_KEY(SC_KEYBOARD_KEY_BACKSPACE, 16, 3, 4),
};

static const sc_keyboard_layout_t k_layouts[SC_KEYBOARD_MODE_MAX] = {
    {k_number_keys, (uint8_t)SC_ARRAY_SIZE(k_number_keys)},
    {k_ascii_keys, (uint8_t)SC_ARRAY_SIZE(k_ascii_keys)},
    {k_symbol_keys, (uint8_t)SC_ARRAY_SIZE(k_symbol_keys)},
};

/* Return the key layout table for the keyboard's current input mode. */
static const sc_keyboard_layout_t *sc_keyboard_layout(const sc_keyboard_t *props)
{
    return &k_layouts[props->mode];
}

/* Compute the on-screen rectangle of a key from its grid cell, snapping edge
 * keys to the object bounds and applying the inter-key gap. */
static rect_t sc_keyboard_key_rect(const sc_obj_t *obj, const sc_keyboard_key_t *key,
                                   uint16_t unit_w, uint16_t row_h)
{
    uint8_t row = SC_KEY_ROW(key);
    uint8_t row_span = SC_KEY_ROW_SPAN(key);
    uint16_t x = (uint16_t)(key->x * unit_w);
    uint16_t y = (uint16_t)(row * row_h);
    uint16_t w = (uint16_t)(key->span * unit_w);
    uint16_t h = (uint16_t)(row_span * row_h);
    if (key->x + key->span == SC_KEYBOARD_GRID_COLS)
    {
        w = (uint16_t)(obj->view.w - x);
    }
    if (row + row_span == SC_KEYBOARD_GRID_ROWS)
    {
        h = (uint16_t)(obj->view.h - y);
    }
    x += SC_KEYBOARD_KEY_GAP;
    y += SC_KEYBOARD_KEY_GAP;
    w -= SC_KEYBOARD_KEY_GAP * 2U;
    h -= SC_KEYBOARD_KEY_GAP * 2U;
    return (rect_t){
        (int16_t)(obj->view.x + (int16_t)x),
        (int16_t)(obj->view.y + (int16_t)y),
        w,
        h,
    };
}

/* Return the rectangle of the key at item_idx (used for focus navigation). */
static rect_t sc_keyboard_item_view(const sc_obj_t *obj, uint8_t item_idx)
{
    const sc_keyboard_t *props = (const sc_keyboard_t *)((const sc_composite_t *)obj->props)->user;
    const sc_keyboard_layout_t *layout = sc_keyboard_layout(props);
    const sc_keyboard_key_t *key = &layout->keys[item_idx];
    uint16_t unit_w = (uint16_t)(obj->view.w / SC_KEYBOARD_GRID_COLS);
    uint16_t row_h = (uint16_t)(obj->view.h / SC_KEYBOARD_GRID_ROWS);
    rect_t r = sc_keyboard_key_rect(obj, key, unit_w, row_h);
    /* 导航用 rect：跨行键（如数字模式 Enter row_span=2）实际中心偏到下一行，
     * 9 宫格会将其判定为下方宫格（vb==2）而非同行（vb==1），
     * 导致 RIGHT 从同行键出发时选错到下一行最左。覆盖高度为起始行高，
     * 使中心落在起始行内，正确判定为同行右侧/左侧候选。绘制不受影响（绘制直接用 key_rect）。 */
    if (SC_KEY_ROW_SPAN(key) > 1U)
    {
        /* 显式对齐到起始行：y 与同行前一个键相等，h 为单行高度，
         * 确保导航 rect 完全等同于起始行的一个标准键，9 宫格判定为同行。 */
        uint8_t row = SC_KEY_ROW(key);
        r.y = (int16_t)(obj->view.y + (int16_t)(row * row_h) + SC_KEYBOARD_KEY_GAP);
        r.h = (uint16_t)(row_h - SC_KEYBOARD_KEY_GAP * 2U);
    }
    return r;
}

/* Convert a lowercase letter to uppercase when shift/upper mode is active. */
static uint8_t sc_keyboard_key_value(const sc_keyboard_t *props, uint8_t value)
{
    if (props->upper && value >= 'a' && value <= 'z')
    {
        return (uint8_t)(value - ('a' - 'A'));
    }
    return value;
}

/* Return the label to render for a key: a name for special keys, otherwise the
 * (case-adjusted) character written into the caller-provided text buffer. */
static const char *sc_keyboard_key_text(const sc_keyboard_t *props, uint8_t value, char text[2])
{
    switch (value)
    {
    case SC_KEYBOARD_KEY_BACKSPACE:
        return "BS";
    case SC_KEYBOARD_KEY_NEWLINE:
        return "\\n";
    case SC_KEYBOARD_KEY_ENTER:
        return "Enter";
    case SC_KEYBOARD_KEY_DELETE:
        return "Del";
    case SC_KEYBOARD_KEY_NUMBER_MODE:
        return "123";
    case SC_KEYBOARD_KEY_ASCII_MODE:
        return "ABC";
    case SC_KEYBOARD_KEY_SYMBOL_MODE:
        return "!@";
    case SC_KEYBOARD_KEY_SHIFT:
        return props->upper ? "abc" : "Shift";
    case SC_KEYBOARD_KEY_UP:
        return "^";
    case SC_KEYBOARD_KEY_DOWN:
        return "v";
    case SC_KEYBOARD_KEY_LEFT:
        return "<";
    case SC_KEYBOARD_KEY_RIGHT:
        return ">";
    case ' ':
        return "Space";
    default:
        text[0] = (char)sc_keyboard_key_value(props, value);
        text[1] = '\0';
        return text;
    }
}

/* Mark the screen area of a single key dirty so it gets redrawn. */
static void sc_keyboard_invalidate_key(sc_obj_t *obj, uint8_t key_idx)
{
    const sc_keyboard_t *props = (const sc_keyboard_t *)((sc_composite_t *)obj->props)->user;
    const sc_keyboard_layout_t *layout = sc_keyboard_layout(props);
    area_t area;
    if (key_idx >= layout->count)
    {
        return;
    }
    area = rect_to_area(sc_keyboard_key_rect(obj, &layout->keys[key_idx],
                                                (uint16_t)(obj->view.w / SC_KEYBOARD_GRID_COLS),
                                                (uint16_t)(obj->view.h / SC_KEYBOARD_GRID_ROWS)));
    sc_obj_invalidate_area(obj, &area);
}

/* Move focus to focus_idx and repaint old/new keys; returns false if unchanged. */
static bool sc_keyboard_set_focus(sc_obj_t *obj, sc_composite_t *composite, uint8_t focus_idx)
{
    uint8_t old_focus = composite->focus_idx;
    if (focus_idx >= composite->item_count || old_focus == focus_idx)
    {
        return false;
    }
    composite->focus_idx = focus_idx;
    sc_keyboard_invalidate_key(obj, old_focus);
    sc_keyboard_invalidate_key(obj, focus_idx);
    return true;
}

#if SC_TOUCH_ENABLE
/* Return the index of the key containing point (x, y), or -1 if none. */
static int8_t sc_keyboard_hit(const sc_obj_t *obj, int16_t x, int16_t y)
{
    const sc_keyboard_t *props = (const sc_keyboard_t *)((const sc_composite_t *)obj->props)->user;
    const sc_keyboard_layout_t *layout = sc_keyboard_layout(props);
    uint16_t unit_w = (uint16_t)(obj->view.w / SC_KEYBOARD_GRID_COLS);
    uint16_t row_h = (uint16_t)(obj->view.h / SC_KEYBOARD_GRID_ROWS);
    for (uint8_t i = 0U; i < layout->count; ++i)
    {
        if (area_contains_point(rect_to_area(sc_keyboard_key_rect(obj, &layout->keys[i],
                                                                  unit_w, row_h)), x, y))
        {
            return (int8_t)i;
        }
    }
    return -1;
}
#endif

/* Switch input mode (optionally toggling shift), reset focus/item count and
 * invalidate; returns false for an invalid mode. */
static bool sc_keyboard_apply_mode(sc_obj_t *obj, sc_keyboard_t *props,
                                   sc_keyboard_mode_t mode, bool toggle_shift)
{
    if (mode >= SC_KEYBOARD_MODE_MAX)
    {
        return false;
    }
    if (toggle_shift)
    {
        props->upper ^= 1U;
    }
    props->mode = mode;
    const sc_keyboard_layout_t *layout = sc_keyboard_layout(props);
    props->composite.item_count = layout->count;
    props->composite.focus_idx = layout->count ? 0U : SC_COMPOSITE_FOCUS_NONE;
    if (obj)
    {
        /* 切模式/大小写必须整对象刷新：sc_obj_invalidate_area 无"已失效跳过"守卫，
         * NULL = 整 view 失效，不受局部键失效（sc_keyboard_invalidate_key）置位影响。 */
        sc_obj_invalidate_area(obj, NULL);
    }
    return true;
}

/* 为让出键盘而向上平移的量（0 = 未平移）。关闭 / 切换编辑框时按原量复位。 */
static int16_t k_keyboard_offset;

/* 计算让 line_edit 完全露出在键盘上方所需的上移量；无需移动返回 0。
 * 上移量 = line_edit 底部超出键盘顶界的部分，并限制顶部不越出屏幕。 */
static int16_t sc_keyboard_offset_for_edit(const sc_obj_t *edit, uint16_t keyboard_height)
{
    int32_t keyboard_top = (int32_t)g_gui->screen.h - keyboard_height;
    int32_t pan = (int32_t)edit->view.y + edit->view.h + (int32_t)SC_KEYBOARD_EDIT_GAP -
                  keyboard_top;
    if (pan <= 0)
    {
        return 0;
    }
    if (pan > edit->view.y)
    {
        pan = edit->view.y; /* 顶部不越出屏幕 */
    }
    return (int16_t)pan;
}

/* 应用（offset>0 上移）或复位（offset=0）键盘平移，跟踪实际量供关闭时恢复。 */
static void sc_keyboard_offset_apply(int16_t offset)
{
    if (offset > 0)
    {
        sc_root_offset(0, -offset);
        k_keyboard_offset = offset;
        g_gui->flags |= SC_RT_KEYBOARD_OFFSET;
    }
    else if (k_keyboard_offset > 0)
    {
        sc_root_offset(0, k_keyboard_offset);
        k_keyboard_offset = 0;
        g_gui->flags &= (uint8_t)~SC_RT_KEYBOARD_OFFSET;
    }
}

/* Detach the keyboard from its line edit, delete it and undo any root offset. */
static void sc_keyboard_close_edit(sc_obj_t *keyboard)
{
    sc_keyboard_t *props = (sc_keyboard_t *)((sc_composite_t *)keyboard->props)->user;
    if (props->edit)
    {
        ((sc_line_edit_t *)props->edit->props)->keyboard = NULL;
        sc_obj_invalidate(props->edit);
        props->edit = NULL;
    }
    g_gui->flags &= (uint8_t)~SC_RT_KEYBOARD_ACTIVE;
    /* 回车/关闭删除键盘前强制整对象失效：局部键失效已置 INVALIDATED，
     * sc_obj_del 内部 sc_obj_invalidate 会被"已失效跳过"守卫挡掉 → 残留残影。 */
    sc_obj_invalidate_area(keyboard, NULL);
    sc_obj_del(keyboard);
    sc_keyboard_offset_apply(0);
}

/* Keyboard callback for the auto edit: close on Enter, else forward the key. */
static void sc_keyboard_edit_input(sc_obj_t *keyboard, uint8_t key)
{
    sc_keyboard_t *props = (sc_keyboard_t *)((sc_composite_t *)keyboard->props)->user;
    if (key == SC_KEYBOARD_KEY_ENTER)
    {
        sc_keyboard_close_edit(keyboard);
        return;
    }
    if (props->edit)
    {
        sc_event_t event = {.type = SC_EVENT_KEYBOARD_INPUT, .dat.key = key};
        sc_obj_dispatch_event(props->edit, &event);
    }
}

/* Act on the focused key: switch mode/shift for mode keys, else emit its value
 * through the user callback. */
static void sc_keyboard_activate(sc_obj_t *obj, sc_composite_t *composite)
{
    sc_keyboard_t *props = (sc_keyboard_t *)composite->user;
    const sc_keyboard_layout_t *layout = sc_keyboard_layout(props);
    if (composite->focus_idx >= layout->count)
    {
        return;
    }
    uint8_t key = layout->keys[composite->focus_idx].value;
    uint8_t value = sc_keyboard_key_value(props, key);
    switch (key)
    {
    case SC_KEYBOARD_KEY_NUMBER_MODE:
        sc_keyboard_apply_mode(obj, props, SC_KEYBOARD_MODE_NUMBER, false);
        return;
    case SC_KEYBOARD_KEY_ASCII_MODE:
        sc_keyboard_apply_mode(obj, props, SC_KEYBOARD_MODE_ASCII, false);
        return;
    case SC_KEYBOARD_KEY_SYMBOL_MODE:
        sc_keyboard_apply_mode(obj, props, SC_KEYBOARD_MODE_SYMBOL, false);
        return;
    case SC_KEYBOARD_KEY_SHIFT:
        sc_keyboard_apply_mode(obj, props, SC_KEYBOARD_MODE_ASCII, true);
        return;
    default:
        break;
    }
    if (props->cb)
    {
        props->cb(obj, value);
    }
}

/* Auto-repeat timer: while the key stays pressed, re-activate and reschedule. */
static void sc_keyboard_repeat_timer(sc_delay_t *timer)
{
    sc_obj_t *obj = (sc_obj_t *)timer->arg;
    if ((obj->flags & SC_OBJ_F_PRESSED) == 0U)
    {
        return;
    }
    sc_reload_delay(timer, SC_KEYBOARD_REPEAT_INTERVAL_MS);
    sc_keyboard_activate(obj, obj->props);
}

/* Draw every key as a button, highlighting the focused (and pressed) one. */
static void sc_keyboard_draw(sc_pfb_t *pfb, const sc_obj_t *obj,
                             const sc_composite_t *composite)
{
    const sc_keyboard_t *props = (const sc_keyboard_t *)composite->user;
    const sc_keyboard_layout_t *layout = sc_keyboard_layout(props);
    sc_button_t button = SC_BUTTON_NEW(NULL, props->fg, props->bg,composite->font_id, ALIGN_CENTER);                               
    uint16_t unit_w = (uint16_t)(obj->view.w / SC_KEYBOARD_GRID_COLS);
    uint16_t row_h = (uint16_t)(obj->view.h / SC_KEYBOARD_GRID_ROWS);
    for (uint8_t i = 0U; i < layout->count; ++i)
    {
        char text[2];
        rect_t rect = sc_keyboard_key_rect(obj, &layout->keys[i], unit_w, row_h);
        bool focused = i == composite->focus_idx;
        button.text = sc_keyboard_key_text(props, layout->keys[i].value, text);
        button.bg = focused ? props->focus_bg : props->bg;
        sc_pfb_draw_button(pfb,
                           &button,
                           rect,
                           focused && (obj->flags & SC_OBJ_F_PRESSED) != 0U);
    }
}

/* Handle key and touch events: focus navigation, press/repeat and activation. */
static bool sc_keyboard_event(sc_obj_t *obj, sc_composite_t *composite,
                              const sc_event_t *event)
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
                                                 sc_keyboard_item_view, false);
            if (focus == composite->focus_idx)
            {
                return false;
            }
            break;
        case CMD_UP:
        case CMD_DOWN:
            focus = sc_composite_focus_direction(obj, event->dat.key,
                                                 sc_keyboard_item_view, false);
            if (focus == composite->focus_idx)
            {
                return false;
            }
            break;
        case CMD_ENTER:
            if ((obj->flags & SC_OBJ_F_PRESSED) == 0U)
            {
                obj->flags |= SC_OBJ_F_PRESSED;
                sc_add_delay(sc_keyboard_repeat_timer, SC_KEYBOARD_REPEAT_DELAY_MS, obj);
                sc_keyboard_invalidate_key(obj, focus);
            }
            return true;
        default:
            return false;
        }
        sc_keyboard_set_focus(obj, composite, focus);
        return true;
    }
    if (event->type == SC_EVENT_KEY_UP && event->dat.key == CMD_ENTER)
    {
        if ((obj->flags & SC_OBJ_F_PRESSED) != 0U)
        {
            obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
            sc_keyboard_invalidate_key(obj, composite->focus_idx);
            sc_keyboard_activate(obj, composite);
        }
        return true;
    }
#if SC_TOUCH_ENABLE
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        int8_t hit = sc_keyboard_hit(obj, event->dat.pos[0], event->dat.pos[1]);
        if (hit < 0)
        {
            return false;
        }
        if (!sc_keyboard_set_focus(obj, composite, (uint8_t)hit))
        {
            sc_keyboard_invalidate_key(obj, (uint8_t)hit);
        }
        obj->flags |= SC_OBJ_F_PRESSED;
        sc_add_delay(sc_keyboard_repeat_timer, SC_KEYBOARD_REPEAT_DELAY_MS, obj);
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_MOVE)
    {
        int8_t hit = sc_keyboard_hit(obj, event->dat.pos[0], event->dat.pos[1]);
        if (hit >= 0)
        {
            sc_keyboard_set_focus(obj, composite, (uint8_t)hit);
        }
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_UP)
    {
        int8_t hit = sc_keyboard_hit(obj, event->dat.pos[0], event->dat.pos[1]);
        bool activate = hit >= 0 && (uint8_t)hit == composite->focus_idx &&
                        (obj->flags & SC_OBJ_F_PRESSED) != 0U;
        obj->flags &= (uint8_t)~SC_OBJ_F_PRESSED;
        sc_keyboard_invalidate_key(obj, composite->focus_idx);
        if (activate)
        {
            sc_keyboard_activate(obj, composite);
        }
        return true;
    }
#endif
    return false;
}

/* Apply a style property (colors and font) to the keyboard. */
static bool sc_keyboard_style(sc_obj_t *obj, sc_composite_t *composite,
                              sc_style_t style, const void *value)
{
    sc_keyboard_t *props = (sc_keyboard_t *)composite->user;
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

static const sc_composite_hooks_t k_keyboard_hooks = {
    .draw = sc_keyboard_draw,
    .event = sc_keyboard_event,
    .style = sc_keyboard_style,
};

/* Create a keyboard composite widget with default colors in the given mode. */
sc_obj_t *sc_keyboard_create(sc_obj_t *parent, sc_keyboard_t *props,
                             sc_keyboard_mode_t mode, sc_keyboard_cb_t cb)
{
    sc_obj_t *obj;
    if (!props || mode >= SC_KEYBOARD_MODE_MAX)
    {
        return NULL;
    }
    *props = (sc_keyboard_t){
        .cb = cb,
        .fg = C_WHITE,
        .bg = C_RGB(32, 40, 56),
        .focus_bg = C_RGB(48, 112, 192),
        .mode = mode,
    };
    obj = sc_composite_create(parent, &props->composite, &k_keyboard_hooks,
                              props, k_layouts[mode].count);
    return obj;
}

/* Change a keyboard object's input mode after validating its type/hooks. */
bool sc_keyboard_set_mode(sc_obj_t *obj, sc_keyboard_mode_t mode)
{
    if (!obj || sc_obj_type(obj) != SC_OBJ_COMPOSITE)
    {
        return false;
    }
    sc_composite_t *composite = (sc_composite_t *)obj->props;
    if (composite->hooks != &k_keyboard_hooks)
    {
        return false;
    }
    return sc_keyboard_apply_mode(obj, (sc_keyboard_t *)composite->user, mode, false);
}

/* Open (or reuse) the shared auto keyboard for a line edit, scrolling the root
 * up if needed so the edit stays visible; returns the keyboard object. */
sc_obj_t *sc_keyboard_open_for_edit(sc_obj_t *edit)
{
    if (!edit)
    {
        return NULL;
    }
    sc_line_edit_t *edit_props = (sc_line_edit_t *)edit->props;
    uint16_t height = (uint16_t)(g_gui->screen.h / 2);
    if ((g_gui->flags & SC_RT_KEYBOARD_ACTIVE) != 0U && k_auto_keyboard_props.edit)
    {
        sc_obj_t *old_edit = k_auto_keyboard_props.edit;
        sc_obj_t *keyboard = ((sc_line_edit_t *)old_edit->props)->keyboard;
        if (old_edit != edit)
        {
            /* 切到另一个编辑框：先复位旧平移，再按新编辑框位置重算，
             * 并把键盘钉回屏幕底部（sc_root_offset 会连带移动键盘）。 */
            sc_keyboard_offset_apply(0);
            ((sc_line_edit_t *)old_edit->props)->keyboard = NULL;
            sc_obj_invalidate(old_edit);
            sc_keyboard_offset_apply(sc_keyboard_offset_for_edit(edit, height));
            sc_obj_set_rect(keyboard, 0, (int16_t)(g_gui->screen.h - height),
                            g_gui->screen.w, height, ALIGN_NONE);
        }
        k_auto_keyboard_props.edit = edit;
        edit_props->keyboard = keyboard;
        sc_obj_invalidate(edit);
        return keyboard;
    }
    /* 只上移 line_edit 底部超出键盘顶界的距离（并限制顶部不越出屏幕），
     * 而不是固定平移整个键盘高度——否则靠上部的编辑框会被顶出屏幕。 */
    sc_keyboard_offset_apply(sc_keyboard_offset_for_edit(edit, height));
    sc_obj_t *keyboard = sc_keyboard_create(NULL, &k_auto_keyboard_props,
                                             SC_KEYBOARD_MODE_NUMBER,
                                             sc_keyboard_edit_input);
    if (!keyboard)
    {
        sc_keyboard_offset_apply(0);
        g_gui->flags &= (uint8_t)~SC_RT_KEYBOARD_ACTIVE;
        return NULL;
    }
    sc_obj_set_rect(keyboard, 0, (int16_t)(g_gui->screen.h - height),
                    g_gui->screen.w, height, ALIGN_NONE);
    k_auto_keyboard_props.edit = edit;
    edit_props->keyboard = keyboard;
    g_gui->flags |= SC_RT_KEYBOARD_ACTIVE;
    sc_obj_invalidate(edit);
    return keyboard;
}

#endif
