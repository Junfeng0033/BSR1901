#include "../sc_draw.h"
#include "sc_line_edit.h"
#include "sc_keyboard.h"

#include "../sc_mem.h"

#if SC_USER_OBJ && SC_WIDGET_LINE_EDIT_ENABLE

typedef struct sc_line_edit_line_t
{
    const char *start;
    const char *end;
    const char *next;
    uint16_t width;
} sc_line_edit_line_t;

/* Inner content rect: view inset by the 1px border on each side. */
#define SC_LINE_EDIT_CONTENT_VIEW(view_) (rect_grow((view_), -1, -1))

/* Return the length of the edit text, or 0 if unset. */
static uint16_t sc_line_edit_text_len(const sc_line_edit_t *props)
{
    return props->text ? (uint16_t)sc_strlen(props->text) : 0U;
}

/* Fill a line descriptor for the line starting at `start`, honoring auto-break wrapping. */
static void sc_line_edit_get_line(const sc_line_edit_t *props,
                                  const lv_font_t *font,
                                  const char *start,
                                  int max_width,
                                  sc_line_edit_line_t *line)
{
    line->start = start;
    line->next = sc_text_next_line(start,
                                   font,
                                   props->x_gap,
                                   (props->align & ALIGN_AUTO_BREAK) != 0U ? max_width : 0,
                                   &line->end,
                                   &line->width);
}

/* Return the pixel x-offset of `cursor` within the given line. */
static int sc_line_edit_cursor_offset(const sc_line_edit_t *props,
                                      const lv_font_t *font,
                                      const sc_line_edit_line_t *line,
                                      const char *cursor)
{
    if (cursor <= line->start)
    {
        return 0;
    }
    if (cursor > line->end)
    {
        cursor = line->end;
    }
    int offset = sc_lvgl_measure_text_range(font, line->start, cursor, props->x_gap);
    if (cursor < line->end)
    {
        offset += props->x_gap;
    }
    return offset;
}

/* Map a target x-pixel within a line to the nearest cursor index into the text. */
static uint16_t sc_line_edit_cursor_at_x(const sc_line_edit_t *props,
                                         const lv_font_t *font,
                                         const sc_line_edit_line_t *line,
                                         int target_x)
{
    const char *cursor = line->start;
    int x = 0;
    if (target_x <= 0)
    {
        return (uint16_t)(line->start - props->text);
    }
    while (cursor < line->end)
    {
        const char *next = cursor;
        uint32_t codepoint = 0U;
        if (!sc_utf8_next_range(&next, line->end, &codepoint))
        {
            break;
        }
        int next_x = x + sc_font_glyph_advance(font, codepoint);
        if (next < line->end)
        {
            next_x += props->x_gap;
        }
        if (target_x < ((x + next_x) >> 1))
        {
            return (uint16_t)(cursor - props->text);
        }
        x = next_x;
        cursor = next;
    }
    return (uint16_t)(line->end - props->text);
}

/* Recompute text width/height, clamp the cursor, and invalidate the object. */
static void sc_line_edit_relayout(sc_obj_t *obj)
{
    sc_line_edit_t *props = (sc_line_edit_t *)obj->props;
    uint16_t len = sc_line_edit_text_len(props);
    if (!props->text)
    {
        props->text_w = 0U;
        props->text_h = 0U;
        props->cursor = 0U;
    }
    else
    {
        const lv_font_t *font = sc_get_font(props->font_id);
        rect_t content = SC_LINE_EDIT_CONTENT_VIEW(obj->view);
        if (props->cursor > len)
        {
            props->cursor = len;
        }
        sc_text_measure_layout(props->text,
                               font,
                               props->x_gap,
                               (props->align & ALIGN_AUTO_BREAK) != 0U ? content.w : 0,
                               &props->text_w,
                               &props->text_h,
                               NULL);
        if (len == 0U || props->text[len - 1U] == '\r' || props->text[len - 1U] == '\n')
        {
            props->text_h += (uint16_t)sc_font_line_height(font);
        }
    }
    sc_obj_invalidate(obj);
}

/* Set the edit text: copy into the owned buffer if any, else store the pointer;
   move the cursor to the end and re-layout. */
static void sc_line_edit_set_text(sc_obj_t *obj, const char *text)
{
    sc_line_edit_t *props = (sc_line_edit_t *)obj->props;
    if (props->capacity != 0U && props->text)
    {
        char *dst = (char *)props->text;
        uint16_t i = 0U;
        if (text)
        {
            while (text[i] != '\0' && i + 1U < props->capacity)
            {
                dst[i] = text[i];
                ++i;
            }
        }
        dst[i] = '\0';
    }
    else
    {
        props->text = text;
    }
    props->cursor = sc_line_edit_text_len(props);
    sc_line_edit_relayout(obj);
}

/* Insert a character at the cursor into the buffer; returns false if full/no buffer. */
static bool sc_line_edit_insert(sc_obj_t *obj, char ch)
{
    sc_line_edit_t *props = (sc_line_edit_t *)obj->props;
    char *text;
    if (!props->text || props->capacity == 0U)
    {
        return false;
    }
    uint16_t len = sc_line_edit_text_len(props);
    if (len + 1U >= props->capacity)
    {
        return false;
    }
    text = (char *)props->text;
    sc_memmove(text + props->cursor + 1U,
            text + props->cursor,
            (size_t)(len - props->cursor + 1U));
    text[props->cursor++] = ch;
    sc_line_edit_relayout(obj);
    return true;
}

/* Delete the character before (backward) or at the cursor; returns false if none. */
static bool sc_line_edit_delete(sc_obj_t *obj, bool backward)
{
    sc_line_edit_t *props = (sc_line_edit_t *)obj->props;
    char *text;
    if (!props->text || props->capacity == 0U)
    {
        return false;
    }
    uint16_t len = sc_line_edit_text_len(props);
    if ((backward && props->cursor == 0U) || (!backward && props->cursor >= len))
    {
        return false;
    }
    uint16_t at = backward ? (uint16_t)(props->cursor - 1U) : props->cursor;
    text = (char *)props->text;
    sc_memmove(text + at, text + at + 1U, (size_t)(len - at));
    if (backward)
    {
        --props->cursor;
    }
    sc_line_edit_relayout(obj);
    return true;
}

/* Draw the border and wrapped text lines, plus the cursor when visible. */
static void sc_line_edit_draw(sc_pfb_t *pfb,
                              const sc_line_edit_t *props,
                              rect_t view,
                              bool cursor_visible)
{
    sc_draw_rect(pfb, view.x, view.y, view.w, view.h, props->fg, 1);
    if (!props->text)
    {
        return;
    }
    rect_t content = SC_LINE_EDIT_CONTENT_VIEW(view);
    area_t inter = rect_to_area(content);
    if (!sc_pfb_intersection(pfb, &inter, NULL))
    {
        return;
    }
    const lv_font_t *font = sc_get_font(props->font_id);
    int line_height = sc_font_line_height(font);
    int block_x = content.x + sc_align_offset_x(content.w, props->text_w, props->align) + props->offs_x;
    int line_y = content.y + sc_align_offset_y(content.h, props->text_h, props->align) + props->offs_y;
    const char *line_start = props->text;
    const char *cursor = props->text + props->cursor;
    bool single_line = props->text_h == (uint16_t)line_height;
    for (;;)
    {
        sc_line_edit_line_t line;
        if (single_line)
        {
            line.start = line_start;
            line.end = line_start + sc_line_edit_text_len(props);
            line.next = line.end;
            line.width = props->text_w;
        }
        else
        {
            sc_line_edit_get_line(props, font, line_start, content.w, &line);
        }
        int line_x = block_x + sc_align_offset_x(props->text_w, line.width, props->align);
        if (line.start < line.end && line_y < inter.ye && line_y + line_height > inter.ys)
        {
            sc_draw_str_range_opa(pfb, line_x, line_y, line.start, line.end,
                                  props->fg, NULL, font, props->x_gap,
                                  &inter, props->opa);
        }
        bool cursor_on_line = cursor <= line.end || (*line.end != '\0' && cursor < line.next);
        if (cursor_visible && cursor_on_line)
        {
            int cursor_x = line_x + sc_line_edit_cursor_offset(props, font, &line, cursor);
            int cursor_y = SC_MAX(line_y, inter.ys);
            int cursor_bottom = SC_MIN(line_y + line_height, inter.ye);
            cursor_x = SC_CLAMP(cursor_x, inter.xs, inter.xe - 1);
            if (cursor_y < cursor_bottom)
            {
                sc_draw_fill_rect(pfb, cursor_x, cursor_y, 1, cursor_bottom - cursor_y, props->fg);
            }
            cursor_visible = false;
        }
        if (single_line || *line.end == '\0')
        {
            break;
        }
        line_start = line.next;
        line_y += line_height;
    }
}

/* Move the cursor to the text position nearest the given (x,y) pixel coordinate. */
static bool sc_line_edit_set_cursor_from_point(sc_obj_t *obj, int x, int y)
{
    sc_line_edit_t *props = (sc_line_edit_t *)obj->props;
    int line_index = 0;
    if (!props->text)
    {
        return false;
    }
    const lv_font_t *font = sc_get_font(props->font_id);
    int line_height = sc_font_line_height(font);
    rect_t content = SC_LINE_EDIT_CONTENT_VIEW(obj->view);
    int block_x = content.x + sc_align_offset_x(content.w, props->text_w, props->align) + props->offs_x;
    int block_y = content.y + sc_align_offset_y(content.h, props->text_h, props->align) + props->offs_y;
    int target_line = y <= block_y ? 0 : (y - block_y) / line_height;
    const char *line_start = props->text;
    for (;;)
    {
        sc_line_edit_line_t line;
        sc_line_edit_get_line(props, font, line_start, content.w, &line);
        if (line_index >= target_line || *line.end == '\0')
        {
            int line_x = block_x + sc_align_offset_x(props->text_w, line.width, props->align);
            props->cursor = sc_line_edit_cursor_at_x(props, font, &line, x - line_x);
            return true;
        }
        ++line_index;
        line_start = line.next;
    }
}

/* Move the cursor up (direction<0) or down (direction>0) one line, keeping its x. */
static bool sc_line_edit_move_vertical(sc_obj_t *obj, int direction)
{
    sc_line_edit_t *props = (sc_line_edit_t *)obj->props;
    int line_index = 0;
    if (!props->text)
    {
        return false;
    }
    const lv_font_t *font = sc_get_font(props->font_id);
    rect_t content = SC_LINE_EDIT_CONTENT_VIEW(obj->view);
    int block_x = content.x + sc_align_offset_x(content.w, props->text_w, props->align) + props->offs_x;
    const char *line_start = props->text;
    const char *cursor = props->text + props->cursor;
    for (;;)
    {
        sc_line_edit_line_t line;
        sc_line_edit_get_line(props, font, line_start, content.w, &line);
        if (cursor <= line.end || (*line.end != '\0' && cursor < line.next))
        {
            if ((direction < 0 && line_start == props->text) ||
                (direction > 0 && *line.end == '\0'))
            {
                return false;
            }
            int line_x = block_x + sc_align_offset_x(props->text_w, line.width, props->align);
            int block_y = content.y + sc_align_offset_y(content.h, props->text_h, props->align) + props->offs_y;
            return sc_line_edit_set_cursor_from_point(
                obj,
                line_x + sc_line_edit_cursor_offset(props, font, &line, cursor),
                block_y + (line_index + direction) * sc_font_line_height(font));
        }
        if (*line.end == '\0')
        {
            return false;
        }
        ++line_index;
        line_start = line.next;
    }
}

/* Draw a line edit at the given rectangle directly to the LCD or PFB (no cursor). */
void sc_lcd_draw_line_edit(sc_pfb_t *pfb,
                           int16_t x,
                           int16_t y,
                           uint16_t w,
                           uint16_t h,
                           const sc_line_edit_t *props)
{
    rect_t view = {x, y, w, h};
    if (!props)
    {
        return;
    }
    if (pfb)
    {
        sc_line_edit_draw(pfb, props, view, false);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_line_edit_draw(sc_lcd_draw_pfb__, props, view, false));
}

/* Create a focusable line edit object under parent with default properties. */
static const sc_obj_vt_t sc_line_edit_vt = {
    .event_cb = sc_obj_handle_line_edit_event,
    .set_style = sc_line_edit_style,
};


sc_obj_t *sc_line_edit_create(sc_obj_t *parent, sc_line_edit_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_LINE_EDIT, &sc_line_edit_vt, props);
    if (obj)
    {
        *props = (sc_line_edit_t)SC_LINE_EDIT_NEW(NULL,
                                                  C_WHITE,
                                                  SC_FONT_DEFAULT,
                                                  255U,
                                                  ALIGN_NONE);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

/* Attach an editable buffer of given capacity, null-terminate it, and re-layout. */
void sc_line_edit_set_buffer(sc_obj_t *obj, char *buffer, uint16_t capacity)
{
    sc_line_edit_t *props = (sc_line_edit_t *)obj->props;
    props->text = (buffer && capacity != 0U) ? buffer : NULL;
    props->capacity = props->text ? capacity : 0U;
    if (props->text)
    {
        buffer[capacity - 1U] = '\0';
    }
    props->cursor = sc_line_edit_text_len(props);
    sc_line_edit_relayout(obj);
}

/* Set the inter-character gap and re-layout the edit text. */
void sc_line_edit_set_x_gap(sc_obj_t *obj, uint8_t x_gap)
{
    sc_line_edit_t *props = (sc_line_edit_t *)obj->props;
    props->x_gap = x_gap;
    sc_line_edit_relayout(obj);
}

/* Apply a style property (fg, font, align, opacity, callback, text) to the line edit. */
bool sc_line_edit_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_line_edit_t *props = (sc_line_edit_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->fg = *(const color_t *)value;
        return true;
    case SC_STYLE_FONT:
        props->font_id = *(const uint8_t *)value;
        sc_line_edit_relayout(obj);
        return true;
    case SC_STYLE_ALIGN:
        props->align = *(const sc_align_t *)value;
        sc_line_edit_relayout(obj);
        return true;
    case SC_STYLE_OPA:
        props->opa = *(const uint8_t *)value;
        return true;
    case SC_STYLE_CALLBACK:
        props->cb = *(const sc_obj_cb_t *)value;
        return true;
    case SC_STYLE_TEXT:
    {
        const sc_obj_text_style_t *text = (const sc_obj_text_style_t *)value;
        props->font_id = text->font_id;
        sc_line_edit_set_text(obj, text->text);
        return true;
    }
    default:
        return false;
    }
}

/* Handle line edit events: draw, cursor movement, text insert/delete, keyboard
   input translation, and touch positioning; opens the keyboard on enter/tap. */
bool sc_obj_handle_line_edit_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_line_edit_t *props = (sc_line_edit_t *)obj->props;
    bool ret = false;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_line_edit_draw((sc_pfb_t *)event->dat.arg,
                          props,
                          obj->view,
                          (g_gui->flags & SC_RT_KEYBOARD_ACTIVE) != 0U);
        return true;
    }
    if (event->type == SC_EVENT_KEY_DOWN || event->type == SC_EVENT_KEYBOARD_INPUT)
    {
        int key = event->dat.key;
        int ch = 0;
        if (event->type == SC_EVENT_KEYBOARD_INPUT)
        {
            if (key >= (int)SC_KEYBOARD_KEY_UP && key <= (int)SC_KEYBOARD_KEY_RIGHT)
            {
                key = SC_KEYBOARD_KEY_UP + CMD_UP - key;
            }
            else if (key == SC_KEYBOARD_KEY_BACKSPACE)
            {
                key = CMD_BACK;
            }
            else if (key == SC_KEYBOARD_KEY_DELETE)
            {
                key = CMD_DEL;
            }
            else if (key == SC_KEYBOARD_KEY_NEWLINE)
            {
                ch = '\n';
                key = CMD_NONE;
            }
            else if (key >= ' ' && key < (int)SC_KEYBOARD_KEY_DELETE)
            {
                ch = key;
                key = CMD_NONE;
            }
            else
            {
                return false;
            }
        }
        else if (key >= CMD_A && key <= CMD_Z)
        {
            ch = 'A' + key - CMD_A;
        }
        else if (key >= CMD_1 && key <= CMD_0)
        {
            ch = key == CMD_0 ? '0' : '1' + key - CMD_1;
        }
        switch (key)
        {
        case CMD_LEFT:
            if (props->cursor != 0U)
            {
                --props->cursor;
                sc_obj_invalidate(obj);
                ret = true;
            }
            break;
        case CMD_RIGHT:
            if (props->cursor < sc_line_edit_text_len(props))
            {
                ++props->cursor;
                sc_obj_invalidate(obj);
                ret = true;
            }
            break;
        case CMD_UP:
        case CMD_DOWN:
            ret = sc_line_edit_move_vertical(obj, key == CMD_DOWN ? 1 : -1);
            if (ret)
            {
                sc_obj_invalidate(obj);
            }
            break;
        case CMD_BACK:
        case CMD_DEL:
            sc_line_edit_delete(obj, key == CMD_BACK);
            ret = true;
            break;
        case CMD_ENTER:
            if (!props->keyboard)
            {
                props->keyboard = sc_keyboard_open_for_edit(obj);
            }
            ret = true;
            break;
        default:
            if (ch != 0)
            {
                sc_line_edit_insert(obj, (char)ch);
                ret = true;
            }
            break;
        }
    }
    else if (event->type == SC_EVENT_KEY_UP)
    {
        int key = event->dat.key;
        ret = (key >= CMD_RIGHT && key <= CMD_UP) || key == CMD_BACK ||
              key == CMD_DEL || key == CMD_ENTER ||
              (key >= CMD_A && key <= CMD_0);
    }
#if SC_TOUCH_ENABLE
    else if (event->type == SC_EVENT_TOUCH_DOWN || event->type == SC_EVENT_TOUCH_MOVE)
    {
        if (sc_line_edit_set_cursor_from_point(obj,
                                               event->dat.pos[0],
                                               event->dat.pos[1]))
        {
            sc_obj_invalidate(obj);
        }
        ret = true;
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        if (!props->keyboard)
        {
            props->keyboard = sc_keyboard_open_for_edit(obj);
        }
        ret = true;
    }
#endif
    if (props->cb)
    {
        props->cb(obj, event);
    }
    return ret;
}
#endif /* SC_USER_OBJ && SC_WIDGET_LINE_EDIT_ENABLE */
