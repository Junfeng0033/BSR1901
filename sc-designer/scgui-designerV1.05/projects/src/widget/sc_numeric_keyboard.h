#ifndef SC_WIDGET_NUMERIC_KEYBOARD_H
#define SC_WIDGET_NUMERIC_KEYBOARD_H

#include "sc_composite.h"

#define SC_NUMERIC_KEY_BACK 10U
#define SC_NUMERIC_KEY_ENTER 11U
#define SC_NUMERIC_KEY_COUNT 12U

#if SC_USER_OBJ && SC_WIDGET_COMPOSITE_ENABLE && SC_WIDGET_NUMERIC_KEYBOARD_ENABLE

typedef void (*sc_numeric_keyboard_cb_t)(sc_obj_t *obj, uint8_t key);

typedef struct sc_numeric_keyboard_t
{
    sc_composite_t composite;
    sc_numeric_keyboard_cb_t cb;
    sc_obj_t *edit;
    color_t fg;
    color_t bg;
    color_t focus_bg;
} sc_numeric_keyboard_t;

sc_obj_t *sc_numeric_keyboard_create(sc_obj_t *parent, sc_numeric_keyboard_t *props,
                                     sc_numeric_keyboard_cb_t cb);
/* Create or rebind the shared automatic keypad used by Line Edit. */
sc_obj_t *sc_numeric_keyboard_open_for_edit(sc_obj_t *edit);
#endif

#endif
