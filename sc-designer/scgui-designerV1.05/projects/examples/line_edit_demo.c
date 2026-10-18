#include "line_edit_demo.h"

#if SC_USER_OBJ

static sc_container_t k_edit_frame;
static sc_container_t k_page;
static sc_line_edit_t k_edit_props;
static sc_button_t k_wrap_button_props;
static sc_obj_t *k_edit_obj;
static char k_edit_buffer[64] = "123\n4568";

const char *sc_line_edit_demo_text(void)
{
    return k_edit_buffer;
}

uint16_t sc_line_edit_demo_cursor(void)
{
    return k_edit_props.cursor;
}

bool sc_line_edit_demo_keyboard_visible(void)
{
    return k_edit_props.keyboard != NULL &&
           (g_gui->flags & SC_RT_KEYBOARD_ACTIVE) != 0U;
}

bool sc_line_edit_demo_keyboard_focused(void)
{
    return k_edit_props.keyboard && sc_focus_current() == k_edit_props.keyboard;
}

void sc_line_edit_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);
    sc_obj_t *frame = sc_container_create(screen, &k_edit_frame);
    sc_obj_set_rect(frame, 16, 136, 288, 58, ALIGN_NONE);
    sc_container_set_flags(frame, SC_CONTAINER_F_BORDER);

    k_edit_obj = sc_line_edit_create(frame, &k_edit_props);
    sc_obj_set_rect(k_edit_obj, 4, 4, 280, 50, ALIGN_NONE);
    sc_line_edit_set_buffer(k_edit_obj, k_edit_buffer, sizeof(k_edit_buffer));
    //sc_line_edit_set_x_gap(k_edit_obj, 1U);
    sc_obj_set_align(k_edit_obj, (sc_align_t)(ALIGN_AUTO_BREAK | ALIGN_LEFT));

    sc_obj_t *wrap_button = sc_button_create(screen, &k_wrap_button_props);
    sc_obj_set_rect(wrap_button, 40, 80, 240, 40, ALIGN_NONE);
    sc_obj_set_text(wrap_button,
                    "AUTO BREAK CENTER LONG TITLE WRAPS ON SECOND LINE",
                    SC_FONT_DEFAULT);
    sc_obj_set_align(wrap_button, (sc_align_t)(ALIGN_AUTO_BREAK | ALIGN_CENTER));

    sc_invalidate_all();
}

#endif
