#include "../sc_draw.h"
#include "sc_text_box.h"
#include "sc_round_rect.h" /* sc_draw_Rounded_mask / sc_draw_bar */
#include "sc_list_items.h" /* sc_scroll_min_yofs 公共函数 */
#include "sc_list_items.h" /* sc_scroll_min_yofs 公共滚动函数 */

#if SC_WIDGET_TEXT_BOX_ENABLE

/* 文本内容区宽度：rect_grow 收半径后再减滚动条宽（与绘制裁剪区同一口径）。
 * 布局换行与绘制都用它，保证文字对齐内收区域、不被滚动条/圆角裁掉。 */
static int sc_text_box_content_w(const sc_text_t *props, rect_t view)
{
    int w = (int)view.w - 2 * (int)props->radius;
    if (props->bar_w > 0)
    {
        w -= (int)props->bar_w;
    }
    return w > 0 ? w : 1;
}

/* Recompute the first visible line offset and its y-position for the current vertical scroll (offs_y). */
static void sc_text_box_relayout_view(sc_text_t *props, const char *text, rect_t view)
{
    props->first_vis_offset = 0;
    props->first_vis_y = 0;
    if (!text || *text == '\0')
    {
        return;
    }
    const lv_font_t *font = sc_get_font(props->font_id);
    int line_height = props->line_height > 0 ? props->line_height : sc_font_line_height(font);
    int max_width = sc_text_box_content_w(props, view);
    int visible_top = -props->offs_y;
    int visible_bottom = visible_top + view.h;
    bool has_visible = false;
    int y = 0;
    const char *line_start = text;
    while (line_start && *line_start != '\0')
    {
        const char *prev_line_start = line_start;
        const char *line_end = NULL;
        uint16_t line_width = 0;
        int line_ye = y + line_height;
        if (y >= visible_bottom)
        {
            break;
        }
        const char *next_line = sc_text_next_line(line_start, font, 0U, max_width, &line_end, &line_width);
        if (line_ye > visible_top)
        {
            if (!has_visible)
            {
                props->first_vis_offset = (uint16_t)(line_start - text);
                props->first_vis_y = (int16_t)y;
                has_visible = true;
            }
        }
        line_start = next_line;
        if (!line_start || line_start <= prev_line_start)
        {
            break;
        }
        y += line_height;
    }
}

/* Recompute full text layout: line height, total lines, bounding box, clamp scroll, and refresh visible range. */
static void sc_text_box_update_layout(sc_text_t *props, const char *text, rect_t view)
{
    int line_height;
    props->text_bbox = (area_t){0, 0, 0, 0};
    props->total_lines = 0;
    props->first_vis_offset = 0;
    props->first_vis_y = 0;
    if (!text || *text == '\0')
    {
        props->line_height = 0;
        props->offs_y = 0;
        return;
    }
    const lv_font_t *font = sc_get_font(props->font_id);
    line_height = sc_font_line_height(font);
    props->line_height = (uint8_t)line_height;
    int max_width = sc_text_box_content_w(props, view);
    uint16_t max_line_width = 0;
    int line_index = 0;
    const char *line_start = text;
    while (line_start && *line_start != '\0')
    {
        const char *prev_line_start = line_start;
        const char *line_end = NULL;
        uint16_t line_width = 0;

        const char *next_line = sc_text_next_line(line_start, font, 0U, max_width, &line_end, &line_width);
        if (line_width > max_line_width)
        {
            max_line_width = line_width;
        }
        line_index++;
        line_start = next_line;
        if (!line_start || line_start <= prev_line_start)
        {
            break;
        }
    }
    props->total_lines = (uint16_t)line_index;
    props->text_bbox.xe = (int16_t)max_line_width;
    props->text_bbox.ye = (int16_t)(line_index * line_height);
    /* 钳制滚动偏移（含上下各半个行高弹性空间） */
    int16_t min_offs, max_offs;
    sc_scroll_range(props->text_bbox.ye, view.h, line_height, &min_offs, &max_offs);
    props->offs_y = SC_CLAMP(props->offs_y, min_offs, max_offs);
    sc_text_box_relayout_view(props, text, view);
}

/* Render a text box into a partial frame buffer: fills rounded background (same style as
 * the list items widget), draws visible wrapped/aligned lines, and adds a scrollbar when
 * the content overflows the view. */
void sc_draw_text_box(sc_pfb_t *pfb, const sc_text_t *props, rect_t view)
{
    if (!props->text)
    {
        return;
    }
    uint8_t radius = props->radius;
    if (!sc_draw_Rounded_mask(pfb, view, (int)radius, 1, props->fg, props->bg, 255U, 0U, NULL))
    {
        return;
    }
    int pad = (int)(radius >> 1);
    int bar_w = (int)props->bar_w;
    /* 文本内容区：先 rect_grow 收半径（内容区对齐圆角内切），再减滚动条宽。 */
    rect_t content = rect_grow(view, (int16_t)-radius, (int16_t)-radius);
    if (bar_w > 0)
    {
        content.w = (uint16_t)SC_MAX((int)content.w - bar_w, 0);
    }
    area_t inter = rect_to_area(content);
    sc_pfb_intersection(pfb, &inter, NULL);
    const lv_font_t *font = sc_get_font(props->font_id);
    int line_height = props->line_height > 0 ? props->line_height : sc_font_line_height(font);
    int content_w = sc_text_box_content_w(props, view);
    const char *line_start = props->text + props->first_vis_offset;
    /* 文字起点对齐内容区（内收后）原点：纵向首行从 content.y 开始，避免被圆角切顶。 */
    int base_y = content.y + props->offs_y + props->first_vis_y;
    while (line_start && *line_start != '\0')
    {
        const char *prev_line_start = line_start;
        const char *line_end = NULL;
        uint16_t line_width = 0;
        int line_x;
        const char *next_line = sc_text_next_line(line_start, font, 0U, content_w, &line_end, &line_width);
        int line_bottom = base_y + line_height;
        if (line_bottom > content.y && base_y < content.y + content.h)
        {
            if ((props->align & ALIGN_HOR) == ALIGN_HOR || (props->align & ALIGN_RIGHT) != 0U)
            {
                line_x = content.x + sc_align_offset_x(content_w, line_width, props->align);
            }
            else
            {
                line_x = content.x;
            }
            sc_draw_str_range_opa(pfb, line_x, base_y, line_start, line_end,
                                  props->fg, NULL, font, 0U, &inter, 255U);
        }
        if (base_y >= content.y + content.h)
        {
            break;
        }
        base_y += line_height;
        line_start = next_line;
        if (!line_start || line_start <= prev_line_start)
        {
            break;
        }
    }
    /* 滚动条（同列表 sc_draw_bar：bar_w > 0 即显示，vol 随 offs_y，无滚动余地时滑块占半长）。 */
    if (bar_w > 0)
    {
        rect_t bar = {
            (int16_t)(view.x + (int)view.w - bar_w - pad),
            (int16_t)(view.y + pad),
            (uint16_t)bar_w,
            (uint16_t)SC_MAX((int)view.h - (int)radius, 0),
        };
        sc_draw_bar(pfb, bar, bar_w / 2, C_BLUE, C_RGB(100, 100, 100), 128U,
                    -props->offs_y);
    }
}

/* Draw a text box to the LCD: computes layout if needed, then draws via pfb or the LCD draw macro. */
void sc_lcd_draw_text_box(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h, const sc_text_t *props)
{
    rect_t view = {x, y, w, h};
    if (!props)
    {
        return;
    }
    sc_text_t local = *props;
    if (local.text && local.total_lines == 0U)
    {
        sc_text_box_update_layout(&local, local.text, view);
    }
    if (pfb)
    {
        sc_draw_text_box(pfb, &local, view);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_text_box(sc_lcd_draw_pfb__, &local, view));
}

#if SC_USER_OBJ && SC_WIDGET_TEXT_BOX_ENABLE
/* Create a focusable text box object under parent and initialize its props with defaults. */
static const sc_obj_vt_t sc_text_box_vt = {
    .event_cb = sc_obj_handle_text_box_event,
    .set_style = sc_text_box_style,
};


sc_obj_t *sc_text_box_create(sc_obj_t *parent, sc_text_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_TEXT_BOX, &sc_text_box_vt, props);
    if (obj)
    {
        *props = (sc_text_t)SC_TEXT_BOX_NEW(NULL, C_BLACK, C_WHITE, SC_FONT_DEFAULT, (sc_align_t)(ALIGN_NONE | ALIGN_AUTO_BREAK));
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

/* Set the text box content, recompute its layout, and invalidate the object for redraw. */
void sc_text_box_sync_text(sc_obj_t *obj, const char *text)
{
    if (!obj || sc_obj_type(obj) != SC_OBJ_TEXT_BOX)
    {
        return;
    }
    sc_text_t *props = (sc_text_t *)obj->props;
    props->text = text;
    sc_text_box_update_layout(props, props->text, obj->view);
    sc_obj_invalidate(obj);
}

/* Scroll the text box vertically by delta_y, clamping to bounds, and invalidate if the offset changed. */
void sc_text_box_scroll(sc_obj_t *obj, int16_t delta_y)
{
    if (!obj || sc_obj_type(obj) != SC_OBJ_TEXT_BOX || delta_y == 0)
    {
        return;
    }
    sc_text_t *props = (sc_text_t *)obj->props;
    int line_height = props->line_height > 0 ? props->line_height : 1;
    int content_h = props->text_bbox.ye;
    int16_t min_offs_y, max_offs_y;
    sc_scroll_range(content_h, obj->view.h, line_height, &min_offs_y, &max_offs_y);
    int16_t new_offs_y = (int16_t)(props->offs_y + delta_y);
    new_offs_y = SC_CLAMP(new_offs_y, min_offs_y, max_offs_y);
    if (new_offs_y == props->offs_y)
    {
        return;
    }

    props->offs_y = new_offs_y;
    sc_text_box_relayout_view(props, props->text, obj->view);
    sc_obj_invalidate(obj);
}

/* Apply a style property (fg/bg/font/align/text) to a text box; returns true if handled. */
bool sc_text_box_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_text_t *props = (sc_text_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->fg = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->bg = *(const color_t *)value;
        return true;
    case SC_STYLE_FONT:
        props->font_id = *(const uint8_t *)value;
        sc_text_box_sync_text(obj, props->text);
        return true;
    case SC_STYLE_ALIGN:
        props->align = *(const sc_align_t *)value;
        sc_text_box_sync_text(obj, props->text);
        return true;
    case SC_STYLE_TEXT:
    {
        const sc_obj_text_style_t *text = (const sc_obj_text_style_t *)value;
        props->font_id = text->font_id;
        sc_text_box_sync_text(obj, text->text);
        return true;
    }
    case SC_STYLE_RADIUS_READ:
        *(uint8_t *)value = ((const sc_text_t *)obj->props)->radius;
        return true;

    default:
        return false;
    }
}

/* Handle text box events: draw, key up/down scrolling, and touch drag scrolling. */
bool sc_obj_handle_text_box_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        /* Layout against the live view so set_text-before-set_rect (or resize) still draws. */
        const sc_text_t *props = (const sc_text_t *)obj->props;
        sc_text_t local = *props;
        if (local.text)
        {
            sc_text_box_update_layout(&local, local.text, obj->view);
        }
        sc_draw_text_box((sc_pfb_t *)event->dat.arg, &local, obj->view);
        return true;
    }
    if (event->type == SC_EVENT_KEY_DOWN)
    {
        if (event->dat.key == CMD_ENTER)
        {
            g_gui->flags ^= SC_RT_KEY_EDIT; /* 回车切换编辑模式 */
        sc_obj_invalidate(obj); /* 1H�Z&F�s�r */
            return true;
        }
        if ((g_gui->flags & SC_RT_KEY_EDIT) != 0U &&
            (event->dat.key == CMD_UP || event->dat.key == CMD_DOWN))
        {
            sc_text_box_scroll(obj, event->dat.key == CMD_UP ? 5 : -5);
            return true;
        }
    }
#if SC_TOUCH_ENABLE
    sc_gui_t *gui = g_gui;
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        gui->flags |= SC_RT_DRAGGING;
        gui->drag_offset_y = event->dat.pos[1];
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_UP)
    {
        gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_MOVE && (gui->flags & SC_RT_DRAGGING) != 0U)
    {
        /* 帧位移 = 当前位置 - 上一帧位置 */
        int16_t delta_y = (int16_t)(event->dat.pos[1] - gui->last_pos_y);
        sc_text_box_scroll(obj, delta_y);
        return true;
    }
#endif
    return false;
}

#endif /* SC_USER_OBJ && SC_WIDGET_TEXT_BOX_ENABLE */

#endif /* SC_WIDGET_TEXT_BOX_ENABLE */
