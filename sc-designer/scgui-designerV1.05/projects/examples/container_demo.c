#include "container_demo.h"
#include "sc_draw.h"


static sc_container_t k_page;
static sc_container_t k_panel;
static sc_label_t k_title;
static sc_button_t k_buttons[4];

void sc_container_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);
    sc_obj_t *panel = sc_container_create(screen, &k_panel);
    sc_container_set_flags(panel, SC_CONTAINER_F_BORDER);
    sc_obj_set_bg(panel, C_RGB(32, 44, 64));
    sc_obj_set_border(panel, C_CYAN);
    sc_obj_set_rect(panel, 36, 24, 248, 192, ALIGN_NONE);

    sc_obj_t *title = sc_label_create(panel, &k_title);
    sc_obj_set_fg(title, C_YELLOW);
    sc_obj_set_text(title, "Container buttons", SC_FONT_DEFAULT);
    sc_obj_set_pos(title, 16, 14);

    sc_obj_t *btn1 = sc_button_create(panel, &k_buttons[0]);
    sc_obj_set_radius(btn1, 10U);
    sc_obj_set_border_width(btn1, 1U);
    sc_obj_set_border(btn1, C_WHITE);
    sc_obj_set_fg(btn1, C_WHITE);
    sc_obj_set_bg(btn1, C_RED);
    sc_obj_set_text(btn1, "Button 1", SC_FONT_DEFAULT);
    sc_obj_set_rect(btn1, 20, 54, 132, 64, ALIGN_NONE);

    sc_obj_t *btn2 = sc_button_create(panel, &k_buttons[1]);
    sc_obj_set_radius(btn2, 10U);
    sc_obj_set_border_width(btn2, 2U);
    sc_obj_set_border(btn2, C_WHITE);
    sc_obj_set_fg(btn2, C_WHITE);
    sc_obj_set_bg(btn2, C_GREEN);
    sc_obj_set_text(btn2, "Button 2", SC_FONT_DEFAULT);
    sc_obj_set_rect(btn2, 44, 70, 132, 64, ALIGN_NONE);

    sc_obj_t *btn3 = sc_button_create(panel, &k_buttons[2]);
    sc_obj_set_radius(btn3, 10U);
    sc_obj_set_border_width(btn3, 2U);
    sc_obj_set_border(btn3, C_WHITE);
    sc_obj_set_fg(btn3, C_WHITE);
    sc_obj_set_bg(btn3, C_BLUE);
    sc_obj_set_text(btn3, "Button 3", SC_FONT_DEFAULT);
    sc_obj_set_rect(btn3, 68, 86, 132, 64, ALIGN_NONE);

    sc_obj_t *btn4 = sc_button_create(panel, &k_buttons[3]);
    sc_obj_set_radius(btn4, 10U);
    sc_obj_set_border_width(btn4, 0U);
    sc_obj_set_border(btn4, C_BLACK);
    sc_obj_set_fg(btn4, C_BLACK);
    sc_obj_set_bg(btn4, C_YELLOW);
    sc_obj_set_text(btn4, "Button 4", SC_FONT_DEFAULT);
    sc_obj_set_rect(btn4, 92, 102, 132, 64, ALIGN_NONE);

    sc_invalidate_all();
}
