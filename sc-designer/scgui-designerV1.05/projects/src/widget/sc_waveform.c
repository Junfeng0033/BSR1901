#include "../sc_mem.h"

#include "../sc_draw.h"
#include "../sc_gui.h"
#include "sc_line.h"
#include "sc_round_rect.h"
#include "sc_waveform.h"

#define WAVEFORM_MERGE_ERR_Q8 16

static void sc_waveform_draw_line(sc_pfb_t *dest, int x1, int y1, int x2, int y2, color_t color, uint8_t alpha)
{
#if SC_WAVEFORM_STROKE_WIDTH == 1
    sc_draw_line_fast_aa(dest, (int16_t)x1, (int16_t)y1, (int16_t)x2, (int16_t)y2, color, alpha);
#else
    sc_draw_line_aa(dest, x1, y1, x2, y2, SC_WAVEFORM_STROKE_WIDTH, color, 0U, alpha);
#endif
}

void sc_waveform_set_scale_x(sc_waveform_t *props, int scale_x)
{
    if (scale_x < SC_WAVEFORM_SCALE_0_5X)
    {
        scale_x = SC_WAVEFORM_SCALE_0_5X;
    }
    if (scale_x >= SC_WAVEFORM_SCALE_MAX)
    {
        scale_x = SC_WAVEFORM_SCALE_MAX - 1;
    }
    props->x_scale = (uint8_t)scale_x;
    props->x_index_offset = (int16_t)(-props->view.w - ((1 << props->x_scale) >> SC_WAVEFORM_SCALE_1X));
}

void sc_waveform_set_scale_y(sc_waveform_t *props, int scale_y)
{
    if (scale_y < 0)
    {
        scale_y = 0;
    }
    if (scale_y >= SC_WAVEFORM_SCALE_MAX)
    {
        scale_y = SC_WAVEFORM_SCALE_MAX - 1;
    }
    props->y_scale = (uint8_t)scale_y;
}

void sc_waveform_init(sc_waveform_t *props, int16_t x, int16_t y, uint16_t w, uint16_t h)
{
    props->view = (rect_t){x, y, w, h};
    props->opa = 255U;
    props->scale_y_axis = 0U;
    props->y_center_offset = (int16_t)(h / 2);
    sc_waveform_set_scale_x(props, SC_WAVEFORM_SCALE_4X);
    sc_waveform_set_scale_y(props, SC_WAVEFORM_SCALE_1X);
    for (uint8_t ch = 0; ch < SC_WAVEFORM_MAX_CH; ch++)
    {
        props->data[ch].buf = NULL;
        props->data[ch].size = 0U;
    }
    for (uint8_t i = 0; i < SC_WAVEFORM_MAX_CHUNKS; i++)
    {
        props->last_ranges[i].min_y = SC_INT16_MAX;
        props->last_ranges[i].max_y = SC_INT16_MIN;
    }
}

void sc_waveform_data_init(sc_waveform_t *props, uint8_t ch, int16_t *buf, uint32_t size, color_t color)
{
    if (!buf || size == 0U || ch >= SC_WAVEFORM_MAX_CH)
    {
        return;
    }
    sc_waveform_data_t *d = &props->data[ch];
    uint32_t pow2_size = 1U;
    while ((pow2_size << 1) != 0U && (pow2_size << 1) <= size)
    {
        pow2_size <<= 1;
    }
    d->buf = buf;
    d->size = (uint16_t)pow2_size;
    d->write_ptr = 0U;
    d->logic_start = 0U;
    d->color = color;
    sc_memset(d->buf, 0, (size_t)d->size * sizeof(int16_t));
}

void sc_waveform_data_put(sc_waveform_t *props, uint8_t ch, int16_t val)
{
    if (ch >= SC_WAVEFORM_MAX_CH)
    {
        return;
    }
    sc_waveform_data_t *d = &props->data[ch];
    if (!d->buf || d->size == 0U)
    {
        return;
    }
    const uint32_t mask = (uint32_t)d->size - 1U;
    d->buf[d->write_ptr] = (int16_t)(-val);
    d->write_ptr = (uint16_t)((d->write_ptr + 1U) & mask);
    d->logic_start = d->write_ptr;
}

static int32_t sc_waveform_sample_y_q8(sc_waveform_t *props, sc_waveform_data_t *d, int32_t idx)
{
    const int32_t idx_q8 = (idx + props->x_index_offset) << (12 - props->x_scale);
    const uint32_t fx = (uint32_t)idx_q8 & 0xFFU;
    const uint32_t mask = (uint32_t)d->size - 1U;
    const uint32_t r0 = (d->logic_start + (uint32_t)(idx_q8 >> 8)) & mask;
    const int32_t v0 = d->buf[r0];
    int32_t res_q8 = v0 << 8;
    if (fx != 0U)
    {
        const uint32_t r1 = (r0 + 1U) & mask;
        const int32_t v1 = d->buf[r1];
        res_q8 = res_q8 + (v1 - v0) * (int32_t)fx;
    }
    return ((props->view.y + props->y_center_offset) << 8) +
           ((res_q8 << props->y_scale) >> SC_WAVEFORM_SCALE_1X);
}

static bool sc_waveform_should_split(int32_t x0, int32_t y0_q8, int32_t x1, int32_t y1_q8,
                                     int32_t x2, int32_t y2_q8)
{
    const int32_t dx = x1 - x0;
    const int32_t dy_q8 = y1_q8 - y0_q8;
    const int32_t test_dx = x2 - x0;
    const int32_t test_dy = y2_q8 - y0_q8;
    const int32_t cross_q8 = test_dx * dy_q8 - test_dy * dx;
    return SC_ABS(cross_q8) > WAVEFORM_MERGE_ERR_Q8 * dx;
}

static void sc_waveform_draw_grid(sc_pfb_t *dest, sc_waveform_t *props, area_t *dirty)
{
    const int16_t xs = dirty->xs;
    const int16_t xe = dirty->xe;
    const int16_t ys = dirty->ys;
    const int16_t ye = dirty->ye;
    const int16_t y_center = (int16_t)(props->view.y + (props->view.h >> 1));
    const int16_t x_center = (int16_t)(props->view.x + (props->view.w >> 1));
    const int16_t grid = (int16_t)(SC_MAX(props->view.w, props->view.h) / 7);
    for (int16_t y = ys; y < ye; y++)
    {
        if (grid != 0 && (y - y_center) % grid == 0)
        {
            const bool solid = (y == y_center);
            uint16_t *dest_out = sc_pfb_pixel_ptr(dest, xs, y);
            for (int16_t x = xs; x < xe; x++, dest_out++)
            {
                if (solid || (x & 0x03) == 0)
                {
                    *dest_out = C_GRAY;
                }
            }
        }
    }
    for (int16_t x = xs; x < xe; x++)
    {
        if (grid != 0 && (x - x_center) % grid == 0)
        {
            const bool solid = (x == x_center);
            uint16_t *dest_out = sc_pfb_pixel_ptr(dest, x, ys);
            for (int16_t y = ys; y < ye; y++, dest_out += dest->width)
            {
                if (solid || (y & 0x03) == 0)
                {
                    *dest_out = C_GRAY;
                }
            }
        }
    }
    (void)sc_draw_Rounded_mask(dest, props->view, 5, 1, C_GRAY, g_gui->bg, props->opa, 0U, dirty);
}

void sc_waveform_draw(sc_pfb_t *dest, sc_waveform_t *props, area_t *dirty)
{
    if (!dest || !props || !dirty)
    {
        return;
    }
    const int32_t chart_xs = props->view.x;
    const int32_t chart_xe = props->view.x + props->view.w;
    const int32_t dirty_xs = SC_MAX(chart_xs, dirty->xs - SC_WAVEFORM_STROKE_WIDTH);
    const int32_t dirty_xe = SC_MIN(chart_xe, dirty->xe + SC_WAVEFORM_STROKE_WIDTH);
    sc_waveform_draw_grid(dest, props, dirty);
    for (uint8_t cn = 0; cn < SC_WAVEFORM_MAX_CH; cn++)
    {
        sc_waveform_data_t *d = &props->data[cn];
        if (!d->buf || d->size == 0U)
        {
            continue;
        }
        int32_t prv_x = dirty_xs;
        int32_t prv_y_q8 = sc_waveform_sample_y_q8(props, d, prv_x - chart_xs);
        int32_t curr_x = dirty_xs + 1;
        int32_t curr_y_q8 = sc_waveform_sample_y_q8(props, d, curr_x - chart_xs);
        for (int32_t x = dirty_xs + 2; x < dirty_xe; x++)
        {
            const int32_t y_q8 = sc_waveform_sample_y_q8(props, d, x - chart_xs);
            if (sc_waveform_should_split(prv_x, prv_y_q8, curr_x, curr_y_q8, x, y_q8))
            {
                sc_waveform_draw_line(dest, prv_x, prv_y_q8 >> 8, curr_x, curr_y_q8 >> 8,
                                      d->color, props->opa);
                prv_x = curr_x;
                prv_y_q8 = curr_y_q8;
            }
            curr_x = x;
            curr_y_q8 = y_q8;
        }
        if (curr_x > prv_x)
        {
            sc_waveform_draw_line(dest, prv_x, prv_y_q8 >> 8, curr_x, curr_y_q8 >> 8,
                                  d->color, props->opa);
        }
    }
}

void sc_waveform_get_chunk_dirty(sc_waveform_t *props, area_t *dirty_buf, uint8_t chunk_size)
{
    if (!props || !dirty_buf || chunk_size == 0U)
    {
        return;
    }
    if (chunk_size > SC_WAVEFORM_MAX_CHUNKS)
    {
        chunk_size = SC_WAVEFORM_MAX_CHUNKS;
    }
    const int16_t total_w = (int16_t)props->view.w;
    const int16_t chunk_samples = (int16_t)(total_w / chunk_size);
    for (uint8_t cid = 0; cid < chunk_size; cid++)
    {
        dirty_buf[cid].ys = SC_INT16_MAX;
        dirty_buf[cid].ye = SC_INT16_MIN;
    }
    for (uint8_t cn = 0; cn < SC_WAVEFORM_MAX_CH; cn++)
    {
        sc_waveform_data_t *d = &props->data[cn];
        if (!d->buf || d->size == 0U)
        {
            continue;
        }
        uint8_t current_cid = 0;
        int32_t sample_cnt = chunk_samples;
        int32_t min_q8 = SC_INT32_MAX;
        int32_t max_q8 = SC_INT32_MIN;
        int32_t y0_q8 = sc_waveform_sample_y_q8(props, d, -1);
        for (int32_t j = 0; j < total_w && current_cid < chunk_size; j++)
        {
            const int32_t y_q8 = sc_waveform_sample_y_q8(props, d, j);
            if (y_q8 < min_q8)
            {
                min_q8 = y_q8;
            }
            if (y_q8 > max_q8)
            {
                max_q8 = y_q8;
            }
            sample_cnt--;
            if ((sample_cnt <= 0 && current_cid < chunk_size - 1) || j == total_w - 1)
            {
                const int32_t next_y_q8 = sc_waveform_sample_y_q8(props, d, j + 1);
                if (next_y_q8 < min_q8)
                {
                    min_q8 = next_y_q8;
                }
                if (next_y_q8 > max_q8)
                {
                    max_q8 = next_y_q8;
                }
                if (y0_q8 < min_q8)
                {
                    min_q8 = y0_q8;
                }
                if (y0_q8 > max_q8)
                {
                    max_q8 = y0_q8;
                }
                y0_q8 = y_q8;

                int16_t curr_min = (int16_t)(min_q8 >> 8);
                int16_t curr_max = (int16_t)((max_q8 + 255) >> 8);
                curr_min = (int16_t)SC_MAX(props->view.y, curr_min - SC_WAVEFORM_STROKE_WIDTH);
                curr_max = (int16_t)SC_MIN(props->view.y + props->view.h, curr_max + SC_WAVEFORM_STROKE_WIDTH);
                if (curr_min < dirty_buf[current_cid].ys)
                {
                    dirty_buf[current_cid].ys = curr_min;
                }
                if (curr_max > dirty_buf[current_cid].ye)
                {
                    dirty_buf[current_cid].ye = curr_max;
                }
                min_q8 = y_q8;
                max_q8 = y_q8;
                sample_cnt = chunk_samples;
                current_cid++;
            }
        }
    }
    for (uint8_t cid = 0; cid < chunk_size; cid++)
    {
        area_t *dirty = &dirty_buf[cid];
        const int16_t curr_min = dirty->ys;
        const int16_t curr_max = dirty->ye;
        const int16_t prev_min = props->last_ranges[cid].min_y;
        const int16_t prev_max = props->last_ranges[cid].max_y;
        dirty->xs = (int16_t)(props->view.x + cid * chunk_samples);
        dirty->xe = (cid == chunk_size - 1) ? (int16_t)(props->view.x + props->view.w)
                                            : (int16_t)(dirty->xs + chunk_samples);
        if (curr_min > curr_max)
        {
            if (prev_min > prev_max)
            {
                dirty->ys = props->view.y;
                dirty->ye = props->view.y;
            }
            else
            {
                dirty->ys = prev_min;
                dirty->ye = prev_max;
            }
        }
        else
        {
            if (prev_min > prev_max)
            {
                dirty->ys = curr_min;
                dirty->ye = curr_max;
            }
            else
            {
                dirty->ys = (int16_t)SC_MIN(curr_min, prev_min);
                dirty->ye = (int16_t)SC_MAX(curr_max, prev_max);
            }
            props->last_ranges[cid].min_y = curr_min;
            props->last_ranges[cid].max_y = curr_max;
        }
    }
}

void sc_lcd_draw_waveform(sc_pfb_t *pfb, sc_waveform_t *props)
{
    if (!props)
    {
        return;
    }
    area_t area_buf[SC_WAVEFORM_MAX_CHUNKS];
    sc_waveform_get_chunk_dirty(props, area_buf, SC_WAVEFORM_MAX_CHUNKS);
    for (uint8_t n = 0; n < SC_WAVEFORM_MAX_CHUNKS; n++)
    {
        area_t area = area_buf[n];
        if (area.xs >= area.xe || area.ys >= area.ye)
        {
            continue;
        }
        if (pfb)
        {
            area_t inter = area;
            if (sc_pfb_intersection(pfb, &inter, NULL))
            {
                sc_waveform_draw(pfb, props, &inter);
            }
            continue;
        }
        SC_LCD_DRAW(area, {
            area_t inter = area;
            if (sc_pfb_intersection(sc_lcd_draw_pfb__, &inter, NULL))
            {
                sc_waveform_draw(sc_lcd_draw_pfb__, props, &inter);
            }
        });
    }
}

#if SC_USER_OBJ && SC_WIDGET_WAVEFORM_ENABLE
static const sc_obj_vt_t sc_waveform_vt = {
    .event_cb = sc_obj_handle_waveform_event,
    .set_style = sc_waveform_style,
};

sc_obj_t *sc_waveform_create(sc_obj_t *parent, sc_waveform_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_WAVEFORM, &sc_waveform_vt, props);
    if (obj)
    {
        *props = (sc_waveform_t)SC_WAVEFORM_NEW();
        sc_waveform_init(props, 0, 0, 0, 0);
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

void sc_waveform_set_geometry(sc_obj_t *obj, int16_t x, int16_t y, uint16_t w, uint16_t h,
                              sc_align_t parent_align)
{
    sc_waveform_t *props = (sc_waveform_t *)obj->props;
    sc_obj_set_rect(obj, x, y, w, h, parent_align);
    props->view = obj->view;
    props->y_center_offset = (int16_t)(obj->view.h / 2);
    sc_waveform_set_scale_x(props, props->x_scale);
}

bool sc_waveform_set_channel(sc_obj_t *obj, uint8_t ch, int16_t *buf, uint32_t size, color_t color)
{
    sc_waveform_t *props = (sc_waveform_t *)obj->props;
    if (ch >= SC_WAVEFORM_MAX_CH || !buf || size == 0U)
    {
        return false;
    }
    sc_waveform_data_init(props, ch, buf, size, color);
    return true;
}

void sc_waveform_refresh(sc_obj_t *obj)
{
    sc_waveform_t *props = (sc_waveform_t *)obj->props;
    area_t area_buf[SC_WAVEFORM_MAX_CHUNKS];
    props->view = obj->view;
    sc_waveform_get_chunk_dirty(props, area_buf, SC_WAVEFORM_MAX_CHUNKS);
    for (uint8_t n = 0; n < SC_WAVEFORM_MAX_CHUNKS; n++)
    {
        (void)sc_invalidate_area(area_buf[n]);
    }
}

void sc_waveform_put(sc_obj_t *obj, uint8_t ch, int16_t val)
{
    sc_waveform_data_put((sc_waveform_t *)obj->props, ch, val);
    sc_waveform_refresh(obj);
}

void sc_waveform_obj_set_scale_x(sc_obj_t *obj, int scale_x)
{
    sc_waveform_t *props = (sc_waveform_t *)obj->props;
    props->view = obj->view;
    sc_waveform_set_scale_x(props, scale_x);
}

void sc_waveform_obj_set_scale_y(sc_obj_t *obj, int scale_y)
{
    sc_waveform_t *props = (sc_waveform_t *)obj->props;
    sc_waveform_set_scale_y(props, scale_y);
}

bool sc_waveform_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_waveform_t *props = (sc_waveform_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->data[0].color = *(const color_t *)value;
        return true;
    case SC_STYLE_OPA:
        props->opa = *(const uint8_t *)value;
        return true;
    default:
        return false;
    }
}

bool sc_obj_handle_waveform_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_waveform_t *props = (sc_waveform_t *)obj->props;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_pfb_t *pfb = (sc_pfb_t *)event->dat.arg;
        area_t clip = rect_to_area(obj->view);
        props->view = obj->view;
        if (sc_pfb_intersection(pfb, &clip, NULL))
        {
            sc_waveform_draw(pfb, props, &clip);
        }
        return true;
    }
    if (event->type == SC_EVENT_KEY_DOWN)
    {
        if (event->dat.key == CMD_ENTER)
        {
            props->scale_y_axis = props->scale_y_axis ? 0U : 1U;
            return true;
        }
        if (event->dat.key == CMD_LEFT || event->dat.key == CMD_RIGHT)
        {
            const int delta = (event->dat.key == CMD_RIGHT) ? 1 : -1;
            if (props->scale_y_axis)
            {
                sc_waveform_obj_set_scale_y(obj, (int)props->y_scale + delta);
            }
            else
            {
                sc_waveform_obj_set_scale_x(obj, (int)props->x_scale + delta);
            }
            return true;
        }
    }
#if SC_TOUCH_ENABLE
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        g_gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_MOVE)
    {
        const int16_t dx = (int16_t)(event->dat.pos[0] - g_gui->drag_offset_x);
        const int16_t dy = (int16_t)(event->dat.pos[1] - g_gui->drag_offset_y);
        if (dx != 0 || dy != 0)
        {
            g_gui->flags |= SC_RT_DRAGGING;
        }
        /* Keep capture; do not bubble to parent page scroll. */
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_UP)
    {
        const int16_t dx = (int16_t)(event->dat.pos[0] - g_gui->drag_offset_x);
        const int16_t dy = (int16_t)(event->dat.pos[1] - g_gui->drag_offset_y);
        const int16_t threshold_x = (int16_t)(obj->view.w >> 2);
        const int16_t threshold_y = (int16_t)(obj->view.h >> 2);
        if (SC_ABS(dx) > threshold_x && SC_ABS(dx) > SC_ABS(dy))
        {
            sc_waveform_obj_set_scale_x(obj, (int)props->x_scale + (dx > 0 ? 1 : -1));
        }
        else if (SC_ABS(dy) > threshold_y && SC_ABS(dy) > SC_ABS(dx))
        {
            sc_waveform_obj_set_scale_y(obj, (int)props->y_scale + (dy < 0 ? 1 : -1));
        }
        g_gui->flags &= (uint8_t)~SC_RT_DRAGGING;
        return true;
    }
#endif
    return false;
}

#endif /* SC_USER_OBJ && SC_WIDGET_WAVEFORM_ENABLE */
