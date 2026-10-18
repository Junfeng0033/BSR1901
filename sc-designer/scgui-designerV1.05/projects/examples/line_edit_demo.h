#ifndef LINE_EDIT_DEMO_H
#define LINE_EDIT_DEMO_H

#include "../src/sc_gui.h"

void sc_line_edit_demo_init(void);
const char *sc_line_edit_demo_text(void);
uint16_t sc_line_edit_demo_cursor(void);
bool sc_line_edit_demo_keyboard_visible(void);
bool sc_line_edit_demo_keyboard_focused(void);

#endif
