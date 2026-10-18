#include "../sc_focus.h"
#include "sc_composite.h"

#if SC_USER_OBJ && SC_WIDGET_COMPOSITE_ENABLE

/* Pick the next focused item index in the direction of key, scoring candidates and optionally wrapping around. */
uint8_t sc_composite_focus_direction(const sc_obj_t *obj, int key,
                                     sc_composite_item_view_cb_t item_view,
                                     bool wrap)
{
    const sc_composite_t *props = (const sc_composite_t *)obj->props;
    uint8_t focus = props->focus_idx;
    if (!item_view || focus >= props->item_count)
    {
        return focus;
    }

    rect_t current = item_view(obj, focus);
    for (uint8_t pass = 0U; pass <= (uint8_t)wrap; ++pass)
    {
        uint8_t best = focus;
        uint32_t best_score = UINT32_MAX;
        for (uint8_t i = 0U; i < props->item_count; ++i)
        {
            if (i == focus)
            {
                continue;
            }
            rect_t candidate = item_view(obj, i);
            if (pass != 0U)
            {
                if (key == CMD_LEFT)
                {
                    candidate.x = (int16_t)(candidate.x - obj->view.w);
                }
                else if (key == CMD_RIGHT)
                {
                    candidate.x = (int16_t)(candidate.x + obj->view.w);
                }
                else if (key == CMD_UP)
                {
                    candidate.y = (int16_t)(candidate.y - obj->view.h);
                }
                else if (key == CMD_DOWN)
                {
                    candidate.y = (int16_t)(candidate.y + obj->view.h);
                }
            }
            uint32_t score = sc_focus_grid_score(&current, key, &candidate);
            if (score < best_score)
            {
                best = i;
                best_score = score;
            }
        }
        if (best != focus || !wrap)
        {
            return best;
        }
    }
    return focus;
}

/* Delegate style application to the composite's registered style hook, if any. */
bool sc_composite_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_composite_t *props = (sc_composite_t *)obj->props;
    return props->hooks->style ? props->hooks->style(obj, props, style, value) : false;
}

/* Handle composite events: call the draw hook on redraw, otherwise delegate to the event hook. */
bool sc_obj_handle_composite_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_composite_t *props = (sc_composite_t *)obj->props;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        if (props->hooks->draw)
        {
            props->hooks->draw((sc_pfb_t *)event->dat.arg, obj, props);
        }
        return true;
    }
    return props->hooks->event ? props->hooks->event(obj, props, event) : false;
}

#endif /* SC_USER_OBJ && SC_WIDGET_COMPOSITE_ENABLE */
