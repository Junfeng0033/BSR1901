#include "sc_list_items_demo.h"

#if SC_USER_OBJ && SC_WIDGET_LIST_ITEMS_ENABLE

static sc_container_t k_page;
static sc_container_t k_panel;
static sc_label_t k_title;
static sc_label_t k_hint;
static sc_list_items_t k_list_props;
static sc_list_item_t k_items[] = {
    SC_LIST_ITEM("Speed", "%", 35, 0, 100, 5),
    SC_LIST_ITEM("Temp", "C", 24, -20, 80, 1),
    SC_LIST_ITEM("Mode", NULL, 2, 0, 3, 1),
    SC_LIST_ITEM_LABEL("Network"),
    SC_LIST_ITEM("Volume", "%", 80, 0, 100, 10),
    SC_LIST_ITEM("Bright", "%", 60, 0, 100, 5),
    SC_LIST_ITEM_LABEL("System"),
    SC_LIST_ITEM("Timeout", "s", 15, 5, 60, 5),
};

void sc_list_items_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *panel = sc_container_create(screen, &k_panel);
    sc_container_set_flags(panel, SC_CONTAINER_F_BORDER);
    sc_obj_set_bg(panel, C_RGB(24, 32, 44));
    sc_obj_set_border(panel, C_CYAN);
    sc_obj_set_rect(panel, 20, 16, 280, 208, ALIGN_NONE);

    sc_obj_t *title = sc_label_create(panel, &k_title);
    sc_obj_set_fg(title, C_YELLOW);
    sc_obj_set_text(title, "sc_list_items demo", SC_FONT_DEFAULT);
    sc_obj_set_pos(title, 12, 10);

    sc_obj_t *hint = sc_label_create(panel, &k_hint);
    sc_obj_set_fg(hint, C_WHITE);
    sc_obj_set_text(hint, "UP/DOWN move, ENTER edit", SC_FONT_DEFAULT);
    sc_obj_set_pos(hint, 12, 30);

    sc_obj_t *list = sc_list_items_create(panel, &k_list_props);
    sc_obj_set_rect(list, 12, 56, 256, 132, ALIGN_NONE);
    sc_obj_set_key_focus(list);
    sc_list_items_set_data(list, k_items, (uint8_t)SC_ARRAY_SIZE(k_items));

    sc_invalidate_all();
}

#endif
