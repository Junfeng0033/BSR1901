#include "sc_gui.h"
#include "sc_draw.h"
#include "sc_mem.h"
#include <stdio.h>

static sc_gui_t g_default_gui;
sc_gui_t *g_gui = &g_default_gui;
#if SC_DIRTY_FPS_DEBUG
sc_fps_t g_fps = {
    .view = {0, 0, 80, 30},
    .str_buf = "FPS:0 ",
    .cnt = 0,
    .last_tick = 0,
};
void sc_get_fps_cnt(sc_fps_t *fps)
{
    uint32_t now = sc_systick_get();
    if (now - fps->frame_tick >= 10)
    {
        fps->frame_tick = now;
        fps->cnt++;
    }
    if (now - fps->last_tick >= 500)
    {
        fps->last_tick = now;
        sprintf(fps->str_buf, "FPS:%d  ", fps->cnt + fps->last_cnt);
        fps->last_cnt = fps->cnt;
        fps->cnt = 0;
        sc_invalidate_area(rect_to_area(fps->view));
    }
}
void sc_draw_fps(sc_pfb_t *pfb, sc_fps_t *fps, sc_align_t align)
{
    area_t inter = rect_to_area(fps->view);
    if (!sc_pfb_intersection(pfb, &inter, NULL))
    {
        return;
    }
    const lv_font_t *font = sc_get_font(SC_FONT_DEFAULT);
    uint16_t text_w = 0U;
    uint16_t text_h = 0U;
    sc_text_measure_lines(fps->str_buf, font, 0, &text_w, &text_h, NULL);
    int16_t base_x = (int16_t)(fps->view.x + sc_align_offset_x(fps->view.w, text_w, align));
    int16_t base_y = (int16_t)(fps->view.y + sc_align_offset_y(fps->view.h, text_h, align));
    sc_draw_str_range_opa(pfb, base_x, base_y, fps->str_buf, NULL, C_WHEAT, NULL,
                          font, 0, &inter, 255);
}
#endif
/* Initialize the global GUI state with a flush callback, default font, and background color. */
void sc_gui_init(sc_flush_cb_t flush, const lv_font_t *font, color_t bg)
{
    sc_memset(&g_default_gui, 0, sizeof(g_default_gui));
    g_default_gui.flush = flush;
    g_default_gui.fonts[0] = font;
#if SC_EXTERNAL_FONT_ENABLE
    for (uint8_t i = 0U; i < SC_FONT_MAX; ++i)
    {
        g_default_gui.font_flash_base[i] = SC_FONT_INTERNAL;
    }
#endif
    g_default_gui.bg = bg;
    g_default_gui.screen = (rect_t){0, 0, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT};
#if SC_USER_OBJ
    sc_obj_pool_init();
    sc_focus_init(); /* 焦点栈清零：focus_stack[0]=NIL，无焦点时 sc_focus_current()=NULL */
    for (uint8_t i = 0U; i < SC_PAGE_CNT; ++i)
    {
        g_default_gui.roots[i] = (sc_root_t){SC_OBJ_NIL, SC_OBJ_NIL};
    }
#endif
}

/* Register a font in the given slot; returns false if the id is out of range. */
bool sc_gui_add_font(uint8_t font_id, const lv_font_t *font, uint32_t flash_base)
{
    if (font_id >= SC_FONT_MAX)
    {
        return false;
    }
    g_gui->fonts[font_id] = font;
#if SC_EXTERNAL_FONT_ENABLE
    g_gui->font_flash_base[font_id] = flash_base;
    sc_external_font_invalidate_cache();
#else
    (void)flash_base;
#endif
    return true;
}

/* Return the font for the given slot, falling back to the default font if unset or invalid. */
const lv_font_t *sc_get_font(uint8_t font_id)
{
    if (font_id >= SC_FONT_MAX || !g_gui->fonts[font_id])
    {
        font_id = SC_FONT_DEFAULT;
    }
#if SC_EXTERNAL_FONT_ENABLE
    uint32_t flash_base = g_gui->font_flash_base[font_id];
    if (flash_base != SC_FONT_INTERNAL)
    {
        sc_external_font_select(font_id, flash_base);
    }
#endif
    return g_gui->fonts[font_id];
}

#if SC_USER_OBJ

/* Delete all objects on the active root and reset its focus, touch, and runtime flags. */
void sc_root_clear(void)
{
    sc_gui_t *gui = g_gui;
    sc_root_t *root = sc_active_root(gui);
    while (root->head != SC_OBJ_NIL)
    {
        sc_obj_del(sc_obj_at(root->head));
    }
    root->head = SC_OBJ_NIL;
    root->tail = SC_OBJ_NIL;
    gui->root_callback = NULL;
    sc_focus_init();
    gui->flags &= (uint8_t)~(SC_RT_MSGBOX_ACTIVE | SC_RT_KEYBOARD_ACTIVE |
                             SC_RT_KEYBOARD_OFFSET);
#if SC_TOUCH_ENABLE
    gui->flags &= (uint8_t)~SC_RT_DRAGGING;
#endif
}

/* Switch to another page, resetting runtime state and forcing a full redraw. */
bool sc_page_switch(uint8_t page_id, sc_obj_cb_t callback)
{
    sc_gui_t *gui = g_gui;
    if (page_id >= SC_PAGE_CNT || page_id == gui->page_id)
    {
        return false;
    }

    sc_obj_set_key_focus(NULL);
#if SC_TOUCH_ENABLE
#endif
    gui->flags = 0U;
    gui->page_id = page_id;
    gui->root_callback = callback;
    sc_invalidate_all();
    return true;
}
typedef struct
{
    sc_obj_t *obj;
    area_t clip;
} sc_draw_cache_t;

/* Read the widget corner radius (0 = sharp corners) so the focus outline
   hugs the widget shape instead of drawing a plain rectangle. */
static uint8_t sc_obj_focus_radius(sc_obj_t *obj)
{
    uint8_t radius = 0U;
    if (obj && sc_obj_get_style(obj, SC_STYLE_RADIUS_READ, &radius))
    {
        return radius;
    }
    return 0U;
}

/* Redraw all visible objects intersecting a dirty area, drawing TOP objects last, slice by slice.
 * HIDDEN/VIRTUAL objects (root/pages) never enter the draw cache. */
static void sc_obj_flush_dirty(area_t dirty)
{
    sc_gui_t *gui = g_gui;
    sc_draw_cache_t cache[SC_DRAW_CACHE_N];
    uint8_t cache_cnt = 0U;
    uint8_t top_cnt = 0U;
    sc_pfb_t pfb;
    sc_obj_t *focus = sc_focus_current();
    sc_obj_t *obj = sc_obj_at(sc_active_root(gui)->head);
#if SC_LCD_QSPI_ALIGN
    dirty.xs = dirty.xs & (~0x01);
    dirty.ys = dirty.ys & (~0x01);
    dirty.xe = (dirty.xe + 1) & (~0x01);
    dirty.ye = (dirty.ye + 1) & (~0x01);
#endif
    if (!pfb_init_slices(&pfb, dirty, gui->bg))
    {
        return;
    }
    while (obj)
    {
        sc_obj_t *next = obj->next;
        area_t clip;
        obj->flags &= (uint8_t)~SC_OBJ_F_INVALIDATED;
        if ((obj->flags & (SC_OBJ_F_HIDDEN | SC_OBJ_F_VIRTUAL)) != 0U || !sc_obj_get_clip_area(obj, &clip) ||
            !area_intersect(clip, dirty, NULL))
        {
            obj = next;
            continue;
        }
        bool top = (obj->flags & SC_OBJ_F_TOP) != 0U;
        if (cache_cnt < SC_DRAW_CACHE_N)
        {
            /* Append TOP objects after normal objects so they draw last. */
            uint8_t idx = top ? cache_cnt : (uint8_t)(cache_cnt - top_cnt);
            if (!top && top_cnt != 0U)
            {
                sc_memmove(&cache[idx + 1U], &cache[idx],
                           (size_t)top_cnt * sizeof(cache[0]));
            }
            ++cache_cnt;
            top_cnt += top;
            cache[idx].obj = obj;
            cache[idx].clip = clip;
        }
#if !PY32_MCU
        else
        {
            printf("SC_DRAW_CACHE_N overflow; increase it in sc_common.h\n");
        }
#endif
        obj = next;
    }
    sc_pfb_t local_pfb;
    sc_event_t draw_event = {.type = SC_EVENT_SYS_DRAW, .dat.arg = &local_pfb};
    do
    {
        local_pfb = pfb;
        for (uint8_t i = 0; i < cache_cnt; i++)
        {
            sc_obj_t *obj = cache[i].obj;
            if (area_intersect(pfb.area, cache[i].clip, &local_pfb.area))
            {
                local_pfb.buffer = pfb.buffer +
                                   (local_pfb.area.ys - pfb.area.ys) * pfb.width +
                                   (local_pfb.area.xs - pfb.area.xs);
                sc_obj_dispatch_draw_event(obj, &draw_event);
                if (obj == focus && (gui->flags & SC_RT_KEY_INPUT) != 0U)
                {
                    color_t frame = (gui->flags & SC_RT_KEY_EDIT) != 0U ? C_BLUE : C_RED;
                    uint8_t radius = sc_obj_focus_radius(obj);
                    if (radius != 0U)
                    {
                        sc_draw_Rounded_texture(&local_pfb, obj->view, radius, 2, frame, g_gui->bg, 255U, NULL, 0U, NULL);
                    }
                    else
                    {
                        sc_draw_rect(&local_pfb, obj->view.x, obj->view.y, obj->view.w, obj->view.h, frame, 2);
                    }
                }
            }
        }
#if SC_DIRTY_FPS_DEBUG
        sc_draw_fps(&pfb, &g_fps, ALIGN_CENTER);
#endif
    } while (pfb_next_slice(&pfb));
}

#endif /* SC_USER_OBJ */

/* Main GUI tick: run timers, drain pending events, and flush dirty regions to the screen. */
void sc_gui_loop(void)
{
    uint8_t dirty_cnt = 0U;
    area_t *dirty;
    sc_timer_loop();

#if SC_DIRTY_FPS_DEBUG
    sc_get_fps_cnt(&g_fps);
#endif
#if SC_USER_OBJ
    sc_event_t event;
    while (sc_event_pop(&event))
    {
        sc_obj_handle_event(&event);
    }
    dirty = sc_dirty_pop(&dirty_cnt);
    for (uint8_t i = 0U; i < dirty_cnt; ++i)
    {
        sc_obj_flush_dirty(dirty[i]);
    }
#else
    dirty = sc_dirty_pop(&dirty_cnt);
    if (g_gui->bare_draw)
    {
        for (uint8_t i = 0U; i < dirty_cnt; ++i)
        {
            SC_LCD_DRAW(dirty[i], g_gui->bare_draw(sc_lcd_draw_pfb__));
        }
    }
#endif
}

/* Weak default LCD DMA refresh stub; override with a real driver implementation. */
_SC_WEAK void demo_lcd_dma_refresh(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h,
                                   color_t *color)
{
    (void)xs;
    (void)ys;
    (void)w;
    (void)h;
    (void)color;
}

/* Weak default SPI flash read stub; override with a real driver implementation. */
_SC_WEAK void spiflash_read(uint8_t *buf, uint32_t offset, uint32_t size)
{
    (void)buf;
    (void)offset;
    (void)size;
}
