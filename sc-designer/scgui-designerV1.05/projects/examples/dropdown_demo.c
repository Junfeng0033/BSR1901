#include "../sc_config.h"
#include "../src/sc_gui.h"
#include "../src/widget/sc_dropdown.h"
#include "../src/widget/sc_button.h"

static const char *k_dropdown_items[] = {
    "Option A", "Option B", "Option C", "Option D",
    "Option E", "Option F", "Option G", "Option H",
};
#define DROPDOWN_ITEM_CNT (sizeof(k_dropdown_items) / sizeof(k_dropdown_items[0]))

static sc_dropdown_t k_dropdown_props;
static sc_button_t k_btn_props;
static sc_container_t k_page;

void sc_dropdown_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);
    sc_obj_t *dropdown = sc_dropdown_create(screen, &k_dropdown_props);
    sc_dropdown_set_items(dropdown, k_dropdown_items, DROPDOWN_ITEM_CNT);
    sc_obj_set_rect(dropdown, 10, 10, 120, 24, ALIGN_NONE);

    /* 底部按钮，测试下拉框展开时是否抢命中 */
    sc_obj_t *btn = sc_button_create(screen, &k_btn_props);
    sc_obj_set_text(btn, "Button", SC_FONT_DEFAULT);
    sc_obj_set_rect(btn, 10, 40, 120, 50, ALIGN_NONE);
}
