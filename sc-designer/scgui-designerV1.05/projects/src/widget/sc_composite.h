#ifndef SC_WIDGET_COMPOSITE_H
#define SC_WIDGET_COMPOSITE_H

#include "../sc_obj.h"

#define SC_COMPOSITE_FOCUS_NONE 0xFFU

#if SC_USER_OBJ && SC_WIDGET_COMPOSITE_ENABLE

typedef struct sc_composite_t sc_composite_t;
typedef rect_t (*sc_composite_item_view_cb_t)(const sc_obj_t *obj, uint8_t item_idx);

typedef void (*sc_composite_draw_hook_t)(sc_pfb_t *pfb, const sc_obj_t *obj, const sc_composite_t *props);
typedef bool (*sc_composite_event_hook_t)(sc_obj_t *obj, sc_composite_t *props, const sc_event_t *event);
typedef bool (*sc_composite_style_hook_t)(sc_obj_t *obj, sc_composite_t *props, sc_style_t style, const void *value);

typedef struct sc_composite_hooks_t
{
    sc_composite_draw_hook_t draw;
    sc_composite_event_hook_t event;
    sc_composite_style_hook_t style;
} sc_composite_hooks_t;

struct sc_composite_t
{
    const sc_composite_hooks_t *hooks;
    void *user;
    uint8_t focus_idx;
    uint8_t item_count;
    uint8_t font_id;
};

bool sc_obj_handle_composite_event(sc_obj_t *obj, const sc_event_t *event);
bool sc_composite_style(sc_obj_t *obj, sc_style_t style, const void *value);

static const sc_obj_vt_t sc_composite_vt = {
    .event_cb = sc_obj_handle_composite_event,
    .set_style = sc_composite_style,
};

static inline sc_obj_t *sc_composite_create(sc_obj_t *parent, sc_composite_t *props,
                                            const sc_composite_hooks_t *hooks, void *user, uint8_t item_count)
{
    sc_obj_t *obj;
    if (!props || !hooks)
    {
        return NULL;
    }
    obj = sc_obj_create(parent, SC_OBJ_COMPOSITE, &sc_composite_vt, props);
    if (obj)
    {
        props->hooks = hooks;
        props->user = user;
        props->item_count = item_count;
        props->focus_idx = item_count ? 0U : SC_COMPOSITE_FOCUS_NONE;
        props->font_id = 0U;
        obj->flags |= SC_OBJ_F_FOCUSABLE;
    }
    return obj;
}

static inline bool sc_composite_set_focus(sc_obj_t *obj, uint8_t focus_idx)
{
    sc_composite_t *props = (sc_composite_t *)obj->props;
    if (focus_idx >= props->item_count || props->focus_idx == focus_idx)
    {
        return false;
    }
    props->focus_idx = focus_idx;
    sc_obj_invalidate(obj);
    return true;
}

/* Item views use absolute screen coordinates; wrap keeps keyboard-style edge cycling. */
uint8_t sc_composite_focus_direction(const sc_obj_t *obj, int key,
                                     sc_composite_item_view_cb_t item_view,
                                     bool wrap);
#endif

#endif
