#ifndef SC_WIDGET_KEYBOARD_H
#define SC_WIDGET_KEYBOARD_H

#include "sc_composite.h"

#define SC_KEYBOARD_KEY_NONE 0x00U
#define SC_KEYBOARD_KEY_BACKSPACE 0x08U
#define SC_KEYBOARD_KEY_NEWLINE 0x0AU
#define SC_KEYBOARD_KEY_ENTER 0x0DU
#define SC_KEYBOARD_KEY_DELETE 0x7FU
#define SC_KEYBOARD_KEY_NUMBER_MODE 0x80U
#define SC_KEYBOARD_KEY_ASCII_MODE 0x81U
#define SC_KEYBOARD_KEY_SYMBOL_MODE 0x82U
#define SC_KEYBOARD_KEY_SHIFT 0x83U
#define SC_KEYBOARD_KEY_UP 0x85U
#define SC_KEYBOARD_KEY_DOWN 0x86U
#define SC_KEYBOARD_KEY_LEFT 0x87U
#define SC_KEYBOARD_KEY_RIGHT 0x88U

#ifndef SC_KEYBOARD_REPEAT_DELAY_MS
#define SC_KEYBOARD_REPEAT_DELAY_MS 200U
#endif
#ifndef SC_KEYBOARD_REPEAT_INTERVAL_MS
#define SC_KEYBOARD_REPEAT_INTERVAL_MS 50U
#endif

typedef uint8_t sc_keyboard_mode_t;
enum
{
    SC_KEYBOARD_MODE_NUMBER = 0,
    SC_KEYBOARD_MODE_ASCII,
    SC_KEYBOARD_MODE_SYMBOL,
    SC_KEYBOARD_MODE_MAX,
};

#if SC_USER_OBJ && SC_WIDGET_COMPOSITE_ENABLE && SC_WIDGET_KEYBOARD_ENABLE
typedef void (*sc_keyboard_cb_t)(sc_obj_t *obj, uint8_t key);
typedef struct sc_keyboard_t
{
    sc_composite_t composite;
    sc_keyboard_cb_t cb;
    sc_obj_t *edit;
    color_t fg;
    color_t bg;
    color_t focus_bg;
    uint8_t mode;
    uint8_t upper;
} sc_keyboard_t;

/* Mode keys are handled internally; cb receives printable and control keys only. */
sc_obj_t *sc_keyboard_create(sc_obj_t *parent, sc_keyboard_t *props,
                             sc_keyboard_mode_t mode, sc_keyboard_cb_t cb);
bool sc_keyboard_set_mode(sc_obj_t *obj, sc_keyboard_mode_t mode);
/* Create or rebind the shared automatic keyboard used by Line Edit. */
sc_obj_t *sc_keyboard_open_for_edit(sc_obj_t *edit);

#endif

#endif
