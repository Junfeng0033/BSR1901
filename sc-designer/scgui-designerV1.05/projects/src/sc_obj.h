#ifndef SC_OBJ_H
#define SC_OBJ_H

#include "sc_common.h"
#include "sc_style.h"

/* Widget compile switches (override before including this header). Kept outside
 * SC_USER_OBJ so bare-draw can use SC_WIDGET_IMAGE_ENABLE for sc_lcd_draw_image.
 * Object/create/event code must use (SC_USER_OBJ && SC_WIDGET_xxx_ENABLE). */
#ifndef SC_WIDGET_CONTAINER_ENABLE
#define SC_WIDGET_CONTAINER_ENABLE 1
#endif
#ifndef SC_WIDGET_BUTTON_ENABLE
#define SC_WIDGET_BUTTON_ENABLE 1
#endif
#ifndef SC_WIDGET_IMAGE_ENABLE
#define SC_WIDGET_IMAGE_ENABLE 1
#endif
#ifndef SC_WIDGET_IMAGE_EX_ENABLE
#define SC_WIDGET_IMAGE_EX_ENABLE 1
#endif
/* Optional SC_IMAGE_RGBA8888 decoder; disabled by default for small MCUs. */
#ifndef SC_WIDGET_IMAGE_RGB888_ENABLE
#define SC_WIDGET_IMAGE_RGB888_ENABLE 1
#endif
#ifndef SC_WIDGET_LABEL_ENABLE
#define SC_WIDGET_LABEL_ENABLE 1
#endif
#ifndef SC_WIDGET_NUMBER_ENABLE
#define SC_WIDGET_NUMBER_ENABLE 1
#endif
#ifndef SC_WIDGET_SWITCH_ENABLE
#define SC_WIDGET_SWITCH_ENABLE 1
#endif
#ifndef SC_WIDGET_PROGRESS_ENABLE
#define SC_WIDGET_PROGRESS_ENABLE 1
#endif
#ifndef SC_WIDGET_TEXT_BOX_ENABLE
#define SC_WIDGET_TEXT_BOX_ENABLE 1
#endif
#ifndef SC_WIDGET_MSGBOX_ENABLE
#define SC_WIDGET_MSGBOX_ENABLE 1
#endif
#ifndef SC_WIDGET_COMPOSITE_ENABLE
#define SC_WIDGET_COMPOSITE_ENABLE 1
#endif
#ifndef SC_WIDGET_NUMERIC_KEYBOARD_ENABLE
#define SC_WIDGET_NUMERIC_KEYBOARD_ENABLE 1
#endif
#ifndef SC_WIDGET_KEYBOARD_ENABLE
#define SC_WIDGET_KEYBOARD_ENABLE 1
#endif
#ifndef SC_WIDGET_LINE_EDIT_ENABLE
#define SC_WIDGET_LINE_EDIT_ENABLE 1
#endif
#ifndef SC_WIDGET_ROUND_RECT_ENABLE
#define SC_WIDGET_ROUND_RECT_ENABLE 1
#endif
#ifndef SC_WIDGET_QCODE_ENABLE
#define SC_WIDGET_QCODE_ENABLE 1
#endif
#ifndef SC_WIDGET_ARC_ENABLE
#define SC_WIDGET_ARC_ENABLE 1
#endif
#ifndef SC_WIDGET_LINE_ENABLE
#define SC_WIDGET_LINE_ENABLE 1
#endif
#ifndef SC_WIDGET_TRIANGLE_ENABLE
#define SC_WIDGET_TRIANGLE_ENABLE 1
#endif
#ifndef SC_WIDGET_EYE_ENABLE
#define SC_WIDGET_EYE_ENABLE 1
#endif
#ifndef SC_WIDGET_WAVEFORM_ENABLE
#define SC_WIDGET_WAVEFORM_ENABLE 1
#endif
#ifndef SC_WIDGET_TRANSFORM_ENABLE
#define SC_WIDGET_TRANSFORM_ENABLE 1
#endif
#ifndef SC_WIDGET_LIST_ITEMS_ENABLE
#define SC_WIDGET_LIST_ITEMS_ENABLE 1
#endif
#ifndef SC_WIDGET_DROPDOWN_ENABLE
#define SC_WIDGET_DROPDOWN_ENABLE 1
#endif
#ifndef SC_WIDGET_SEGMENT_SWITCH_ENABLE
#define SC_WIDGET_SEGMENT_SWITCH_ENABLE 1
#endif
#ifndef SC_WIDGET_DROPDOWN_CARD_ENABLE
#define SC_WIDGET_DROPDOWN_CARD_ENABLE 1
#endif
#ifndef SC_WIDGET_LIST_MENU_ENABLE
#define SC_WIDGET_LIST_MENU_ENABLE 1
#endif

#if SC_USER_OBJ

#if !(SC_WIDGET_CONTAINER_ENABLE || SC_WIDGET_BUTTON_ENABLE || SC_WIDGET_IMAGE_ENABLE ||           \
      SC_WIDGET_LABEL_ENABLE || SC_WIDGET_NUMBER_ENABLE || SC_WIDGET_SWITCH_ENABLE ||              \
      SC_WIDGET_PROGRESS_ENABLE || SC_WIDGET_TEXT_BOX_ENABLE || SC_WIDGET_MSGBOX_ENABLE ||         \
      SC_WIDGET_COMPOSITE_ENABLE || SC_WIDGET_LINE_EDIT_ENABLE || SC_WIDGET_QCODE_ENABLE ||        \
      SC_WIDGET_ROUND_RECT_ENABLE || SC_WIDGET_ARC_ENABLE || SC_WIDGET_LINE_ENABLE ||              \
      SC_WIDGET_TRIANGLE_ENABLE || SC_WIDGET_EYE_ENABLE || SC_WIDGET_LIST_ITEMS_ENABLE ||          \
      SC_WIDGET_WAVEFORM_ENABLE || SC_WIDGET_DROPDOWN_ENABLE || SC_WIDGET_SEGMENT_SWITCH_ENABLE || \
      SC_WIDGET_DROPDOWN_CARD_ENABLE || SC_WIDGET_LIST_MENU_ENABLE)
#error Enable at least one widget, or set SC_USER_OBJ=0
#endif

/*
 * Type IDs are fixed regardless of SC_WIDGET_*_ENABLE. Enable macros only
 * compile the widget implementation; unused widgets are dropped by
 * --gc-sections when nothing references their create/event_cb.
 */
typedef enum
{
    SC_OBJ_ROOT = 0,
    SC_OBJ_CONTAINER,
    SC_OBJ_BUTTON,
    SC_OBJ_IMAGE,
    SC_OBJ_LABEL,
    SC_OBJ_STR,
    SC_OBJ_SWITCH,
    SC_OBJ_PROGRESS,
    SC_OBJ_TEXT_BOX,
    SC_OBJ_MSGBOX,
    SC_OBJ_COMPOSITE,
    SC_OBJ_LINE_EDIT,
    SC_OBJ_QCODE,
    SC_OBJ_ROUND_RECT,
    SC_OBJ_ARC,
    SC_OBJ_PLAIN_RECT,
    SC_OBJ_LINE,
    SC_OBJ_LIST_ITEMS,
    SC_OBJ_WAVEFORM,
    SC_OBJ_ROTATE,
    SC_OBJ_TRIANGLE,
    SC_OBJ_EYE,
    SC_OBJ_NUMBER,
    SC_OBJ_DROPDOWN,
    SC_OBJ_SEGMENT_SWITCH, /* 多段开关（2~8档） */
    SC_OBJ_TABBAR,         /* 选项卡标签栏 */
    SC_OBJ_DROPDOWN_CARD,  /* 下拉卡片（手势捕捉 + 链式回调） */
    SC_OBJ_LIST_MENU,      /* 多级菜单（图示+文本条目，水平线分界，ENTER/BACK 进退子级） */
    SC_OBJ_TYPE_MAX,
} sc_obj_type_t;

#define SC_OBJ_NIL 0xFFU                  /* Invalid object-pool index and end-of-list sentinel. */
#define SC_OBJ_F_INVALIDATED 0x01U        /* The object's current area has already been marked dirty. */
#define SC_OBJ_F_PRESSED 0x02U            /* The object is currently in its pressed input/visual state. */
#define SC_OBJ_F_FOCUSABLE 0x04U          /* The object is eligible to receive keyboard focus. */
#define SC_OBJ_F_IGNORE_PARENT_CLIP 0x08U /* Skip clipping against the direct parent. */
#define SC_OBJ_F_VIRTUAL 0x10U            /* Skip all invalidation (both whole-object and area). */
#define SC_OBJ_F_TOP 0x20U                /* Draw after normal objects regardless of tree order. */
#define SC_OBJ_F_RESERVED 0x40U           /* Reserved for future object state. */
#define SC_OBJ_F_HIDDEN 0x80U             /* Hidden from drawing, hit testing, focus, and invalidation. */

#define SC_OBJ_LEVEL_MAX 7U /* type_level stores level in its upper 3 bits. */
#define SC_OBJ_TYPE_MASK 0x1FU
#define SC_OBJ_LEVEL_SHIFT 5U

struct sc_obj_t;
struct sc_gui_t;
typedef bool (*sc_obj_event_cb_t)(struct sc_obj_t *obj, const sc_event_t *event);
typedef bool (*sc_obj_style_cb_t)(struct sc_obj_t *obj, sc_style_t style, const void *value);

/* 控件虚表：事件回调 + 样式设置，常量数据放 flash，每个控件类型一份。 */
typedef struct sc_obj_vt
{
    sc_obj_event_cb_t event_cb;
    sc_obj_style_cb_t set_style;
} sc_obj_vt_t;

typedef struct sc_obj_t
{
    rect_t view;
    void *props;
    const sc_obj_vt_t *vt;
    struct sc_obj_t *next;
    uint8_t parent;
    uint8_t id;
    uint8_t flags;
    uint8_t type_level; /* Upper 3 bits are level, lower 5 are type. */
} sc_obj_t;

sc_obj_t *sc_obj_at(uint8_t idx);
sc_obj_t *sc_obj_prev(sc_obj_t *target);
sc_obj_t *sc_obj_next_tree(sc_obj_t *cur);
sc_obj_t *sc_obj_create(sc_obj_t *parent, sc_obj_type_t type, const sc_obj_vt_t *vt, void *props);

void sc_obj_pool_init(void);
bool sc_obj_invalidate(sc_obj_t *obj);
bool sc_obj_invalidate_area(sc_obj_t *obj, area_t *area);
bool sc_obj_get_clip_area(const sc_obj_t *obj, area_t *out);

static inline uint8_t sc_obj_type(const sc_obj_t *obj)
{
    return (uint8_t)(obj->type_level & SC_OBJ_TYPE_MASK);
}
static inline bool sc_obj_is_container(const sc_obj_t *obj)
{
    return sc_obj_type(obj) <= SC_OBJ_CONTAINER;
}
static inline uint8_t sc_obj_level(const sc_obj_t *obj)
{
    return (uint8_t)(obj->type_level >> SC_OBJ_LEVEL_SHIFT);
}
static inline void sc_obj_add_flag(sc_obj_t *obj, uint8_t flag)
{
    obj->flags |= flag;
}
static inline void sc_obj_remove_flag(sc_obj_t *obj, uint8_t flag)
{
    obj->flags &= (uint8_t)~flag;
}
static inline sc_obj_t *sc_obj_parent(const sc_obj_t *obj)
{
    return sc_obj_at(obj->parent);
}
static inline bool sc_obj_is_focusable(const sc_obj_t *obj)
{
    return obj && (obj->flags & (SC_OBJ_F_HIDDEN | SC_OBJ_F_FOCUSABLE)) == SC_OBJ_F_FOCUSABLE;
}
void sc_obj_move(sc_obj_t *obj, int16_t dx, int16_t dy);
void sc_obj_del(sc_obj_t *obj);
void sc_obj_set_hidden(sc_obj_t *obj, bool hidden);

void sc_obj_set_pos(sc_obj_t *obj, int16_t x, int16_t y);
void sc_obj_set_size(sc_obj_t *obj, uint16_t w, uint16_t h);
void sc_obj_set_rect(sc_obj_t *obj, int16_t x, int16_t y, uint16_t w, uint16_t h, sc_align_t parent_align);
/* Draw only this object; SC_EVENT_SYS_DRAW must not bubble to its parent. */
void sc_obj_dispatch_draw_event(sc_obj_t *obj, const sc_event_t *event);
sc_obj_t *sc_obj_dispatch_event(sc_obj_t *obj, const sc_event_t *event);
bool sc_obj_common_event(sc_obj_t *obj, const sc_event_t *event, sc_obj_cb_t cb);
bool sc_obj_handle_event(const sc_event_t *event);

#endif /* SC_USER_OBJ */

#endif
