#include "../sc_draw.h"
#include "../third_party/qrcode/qrcode.h"
#include "sc_qcode.h"

#include "../sc_mem.h"

#if SC_USER_OBJ && SC_WIDGET_QCODE_ENABLE

/* Draw the QR code: fill the background, then render dark modules centered and
 * scaled to the object, coalescing horizontal runs into single fills. */
static void sc_qcode_draw(sc_pfb_t *pfb, const sc_obj_t *obj, const sc_qcode_t *props)
{
    int cell;
    int origin_x;
    int origin_y;
    if (!sc_draw_fill_rect(pfb, obj->view.x, obj->view.y,
                           obj->view.w, obj->view.h, props->bg))
    {
        return;
    }
    if (!props->valid)
    {
        return;
    }
    cell = SC_MIN(obj->view.w, obj->view.h) /
           (SC_QCODE_MODULE_COUNT + SC_QCODE_QUIET_ZONE * 2U);
    if (cell <= 0)
    {
        return;
    }
    origin_x = obj->view.x + (obj->view.w - cell * (SC_QCODE_MODULE_COUNT + SC_QCODE_QUIET_ZONE * 2U)) / 2 +
               cell * SC_QCODE_QUIET_ZONE;
    origin_y = obj->view.y + (obj->view.h - cell * (SC_QCODE_MODULE_COUNT + SC_QCODE_QUIET_ZONE * 2U)) / 2 +
               cell * SC_QCODE_QUIET_ZONE;
    for (uint8_t y = 0U; y < SC_QCODE_MODULE_COUNT; ++y)
    {
        int run = -1;
        for (uint8_t x = 0U; x <= SC_QCODE_MODULE_COUNT; ++x)
        {
            uint16_t offset = (uint16_t)(y * SC_QCODE_MODULE_COUNT + x);
            bool dark = x < SC_QCODE_MODULE_COUNT &&
                        (props->modules[offset >> 3] &
                         (uint8_t)(1U << (7U - (offset & 7U)))) != 0U;
            if (dark && run < 0)
            {
                run = x;
            }
            else if (!dark && run >= 0)
            {
                sc_draw_fill_rect(pfb, origin_x + run * cell, origin_y + y * cell,
                                  (x - run) * cell, cell, props->fg);
                run = -1;
            }
        }
    }
}

/* Create a QR code object with default black-on-white colors. */
static const sc_obj_vt_t sc_qcode_vt = {
    .event_cb = sc_obj_handle_qcode_event,
    .set_style = sc_qcode_style,
};


sc_obj_t *sc_qcode_create(sc_obj_t *parent, sc_qcode_t *props)
{
    sc_obj_t *obj = sc_obj_create(parent, SC_OBJ_QCODE, &sc_qcode_vt, props);
    if (obj)
    {
        sc_memset(props, 0, sizeof(*props));
        props->fg = C_BLACK;
        props->bg = C_WHITE;
    }
    return obj;
}

/* Encode text into the QR module bitmap, invalidate, return whether it fit. */
bool sc_qcode_set_text(sc_obj_t *obj, const char *text)
{
    sc_qcode_t *props = (sc_qcode_t *)obj->props;
    QRCode qrcode;
    props->valid = 0U;
    if (qrcode_initText(&qrcode, props->modules, text) == 0)
    {
        props->valid = 1U;
    }
    sc_obj_invalidate(obj);
    return props->valid != 0U;
}

/* Apply a style property (colors, or text to encode) to the QR code. */
bool sc_qcode_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    sc_qcode_t *props = (sc_qcode_t *)obj->props;
    switch (style)
    {
    case SC_STYLE_FG:
        props->fg = *(const color_t *)value;
        return true;
    case SC_STYLE_BG:
        props->bg = *(const color_t *)value;
        return true;
    case SC_STYLE_TEXT:
        return sc_qcode_set_text(obj, ((const sc_obj_text_style_t *)value)->text);
    default:
        return false;
    }
}

/* Handle QR code events: render on the system draw event. */
bool sc_obj_handle_qcode_event(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        sc_qcode_draw((sc_pfb_t *)event->dat.arg, obj, (const sc_qcode_t *)obj->props);
        return true;
    }
    return false;
}

#endif
