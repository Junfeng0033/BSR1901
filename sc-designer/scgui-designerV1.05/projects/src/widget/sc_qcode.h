#ifndef SC_WIDGET_QCODE_H
#define SC_WIDGET_QCODE_H

#include "../sc_obj.h"
#include "../third_party/qrcode/qrcode.h"


#define SC_QCODE_VERSION QRCODE_VERSION
#define SC_QCODE_MODULE_COUNT QRCODE_SIZE
#define SC_QCODE_QUIET_ZONE 4U
#define SC_QCODE_TEXT_MAX QRCODE_TEXT_MAX
#define SC_QCODE_BUFFER_SIZE QRCODE_BUFFER_SIZE

typedef struct sc_qcode_t
{
    color_t fg;
    color_t bg;
    uint8_t modules[SC_QCODE_BUFFER_SIZE];
    uint8_t valid;
} sc_qcode_t;

#if SC_USER_OBJ && SC_WIDGET_QCODE_ENABLE
sc_obj_t *sc_qcode_create(sc_obj_t *parent, sc_qcode_t *props);
bool sc_qcode_set_text(sc_obj_t *obj, const char *text);
bool sc_qcode_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_qcode_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
