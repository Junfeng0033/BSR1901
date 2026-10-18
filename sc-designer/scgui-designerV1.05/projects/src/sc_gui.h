#ifndef SC_GUI_H
#define SC_GUI_H

#include "sc_common.h"
#include "sc_event.h"
#include "sc_style.h"
#include "sc_lvgl_font.h"
#include "sc_obj.h"
#include "sc_focus.h"
#include "sc_anim.h"
#include "widget/sc_button.h"
#include "widget/sc_composite.h"
#include "widget/sc_container.h"
#include "widget/sc_image.h"
#include "widget/sc_label.h"
#include "widget/sc_number.h"
#include "widget/sc_keyboard.h"
#include "widget/sc_line_edit.h"
#include "widget/sc_msgbox.h"
#include "widget/sc_numeric_keyboard.h"
#include "widget/sc_progress.h"
#include "widget/sc_qcode.h"
#include "widget/sc_arc.h"
#include "widget/sc_line.h"
#include "widget/sc_triangle.h"
#include "widget/sc_eye.h"
#include "widget/sc_list_items.h"
#include "widget/sc_dropdown.h"
#include "widget/sc_round_rect.h"
#include "widget/sc_transform.h"
#include "widget/sc_waveform.h"
#include "widget/sc_switch.h"
#include "widget/sc_segment_switch.h"
#include "widget/sc_text_box.h"
#include "widget/sc_dropdown_card.h"
#include "widget/sc_list_menu.h"

#define SC_RT_DRAGGING 0x01U
#define SC_RT_MSGBOX_ACTIVE 0x02U
#define SC_RT_KEYBOARD_ACTIVE 0x04U
#define SC_RT_KEYBOARD_OFFSET 0x08U
#define SC_RT_KEY_INPUT 0x10U /* 1=按键输入(画红框), 0=触摸输入(不画红框) */
#define SC_RT_KEY_EDIT       0x20U  /* 按键编辑模式：1=控件可编辑(进度条/text_box等消费方向键), 0=导航模式 */

#define SC_FONT_DEFAULT 0U
#define SC_FONT_SECONDARY 1U
#define SC_FONT_TERTIARY 2U
#define SC_FONT_QUATERNARY 3U
#define SC_FONT_INTERNAL UINT32_MAX
#define sc_get_default_font() sc_get_font(SC_FONT_DEFAULT)

const lv_font_t *sc_get_font(uint8_t font_id);
bool sc_gui_add_font(uint8_t font_id, const lv_font_t *font, uint32_t flash_base);

typedef void (*sc_flush_cb_t)(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color);
typedef void (*sc_draw_cb_t)(sc_pfb_t *pfb);

/* Override this weak hook to read bytes from external SPI flash. */
void spiflash_read(uint8_t *buf, uint32_t offset, uint32_t size);

/* Fallback for key/touch events not consumed by an object or its parent chain. */
/* 统一使用 sc_obj_cb_t 签名：void (*)(sc_obj_t *obj, const sc_event_t *event)，root 模式 obj=NULL */

typedef struct sc_root_t
{
    uint8_t head;
    uint8_t tail;
} sc_root_t;

typedef struct sc_gui_t
{
    sc_flush_cb_t flush;
    sc_draw_cb_t bare_draw;
    const lv_font_t *fonts[SC_FONT_MAX];
#if SC_EXTERNAL_FONT_ENABLE
    uint32_t font_flash_base[SC_FONT_MAX]; /* SC_FONT_INTERNAL: internal */
#endif

#if SC_USER_OBJ
#if SC_TOUCH_ENABLE
    int16_t drag_offset_x;  /* 按下起点（DOWN 写，MOVE 只读不更新） */
    int16_t drag_offset_y;
    int16_t last_pos_x;     /* 上一帧位置（MOVE 后更新，用于算帧位移） */
    int16_t last_pos_y;
#endif
    uint8_t focus_stack[SC_FOCUS_STACK_MAX]; /* 焦点栈，存 obj_id */
    uint8_t focus_depth;                       /* 当前栈深度，0=全局层级 */
    uint8_t focus_last_input;                  /* 0=触摸输入, 1=按键输入 */
    sc_obj_t obj_pool[SC_OBJ_MAX];
    sc_obj_cb_t root_callback;
    sc_root_t roots[SC_PAGE_CNT];
    uint8_t obj_free_head;
    uint8_t page_id;
#endif
    color_t bg;
    rect_t screen;
    uint8_t flags;
} sc_gui_t;

extern sc_gui_t *g_gui;
typedef struct sc_fps_t
{
    rect_t view;
    uint32_t last_tick;
    uint32_t frame_tick;
    uint8_t cnt;
    uint8_t last_cnt;
    char str_buf[16];
} sc_fps_t;

void sc_gui_init(sc_flush_cb_t flush, const lv_font_t *font, color_t bg);
void sc_gui_loop(void);
void sc_invalidate_all(void);

#if SC_USER_OBJ
static inline sc_root_t *sc_active_root(sc_gui_t *gui)
{
    return &gui->roots[gui->page_id];
}

/* Switch root slot and install its fallback callback; false means invalid or already active. */
bool sc_page_switch(uint8_t page_id, sc_obj_cb_t callback);
/* Set the fallback callback of the active page slot; NULL disables it. */
static inline void sc_root_set_callback(sc_obj_cb_t callback)
{
    g_gui->root_callback = callback;
}
static inline uint8_t sc_page_current(void)
{
    return g_gui->page_id;
}
void sc_root_clear(void);
/* 保留：虚拟键盘显示/隐藏时用于全屏内容偏移，即使当前项目未调用也不要删除。 */
static inline void sc_root_offset(int16_t dx, int16_t dy)
{
    sc_obj_t *obj;
    if (dx == 0 && dy == 0)
    {
        return;
    }
    obj = sc_obj_at(sc_active_root(g_gui)->head);
    while (obj)
    {
        obj->view.x = (int16_t)(obj->view.x + dx);
        obj->view.y = (int16_t)(obj->view.y + dy);
        obj = obj->next;
    }
    sc_invalidate_all();
}
#endif

#endif
