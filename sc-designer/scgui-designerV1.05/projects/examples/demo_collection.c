#include "demo_collection.h"

#include "container_demo.h"
#include "label_demo.h"
#include "line_edit_demo.h"
#include "round_rect_demo.h"
#include "text_box_demo.h"

#if SC_USER_OBJ

#define SC_DEMO_CLOCK_MS 1000U
#define SC_DEMO_PAGE_MAIN 0U
#define SC_DEMO_PAGE_CHILD 1U
#if SC_PAGE_CNT < 2U
#error Demo collection requires at least two page slots
#endif
static sc_container_t k_main_page;
static sc_obj_t *k_uptime_obj;
static sc_label_t k_menu_title;
static sc_label_t k_uptime_label;
static sc_button_t k_menu_buttons[5];
static sc_button_t k_back_button;
static char k_uptime_text[16] = "Uptime 00:00:00";

static bool sc_demo_event_activated(const sc_event_t *event)
{
    return event->type == SC_EVENT_TOUCH_UP ||
           (event->type == SC_EVENT_KEY_UP && event->dat.key == CMD_ENTER);
}

static void sc_demo_enter(void (*demo_init)(void));

static void sc_demo_container_cb(sc_obj_t *obj, const sc_event_t *event)
{
    (void)obj;
    if (sc_demo_event_activated(event))
    {
        sc_demo_enter(sc_container_demo_init);
    }
}

static void sc_demo_label_cb(sc_obj_t *obj, const sc_event_t *event)
{
    (void)obj;
    if (sc_demo_event_activated(event))
    {
        sc_demo_enter(sc_label_demo_init);
    }
}

static void sc_demo_line_edit_cb(sc_obj_t *obj, const sc_event_t *event)
{
    (void)obj;
    if (sc_demo_event_activated(event))
    {
        sc_demo_enter(sc_line_edit_demo_init);
    }
}

static void sc_demo_round_rect_cb(sc_obj_t *obj, const sc_event_t *event)
{
    (void)obj;
    if (sc_demo_event_activated(event))
    {
        sc_demo_enter(sc_round_rect_demo_init);
    }
}

static void sc_demo_text_box_cb(sc_obj_t *obj, const sc_event_t *event)
{
    (void)obj;
    if (sc_demo_event_activated(event))
    {
        sc_demo_enter(sc_text_box_demo_init);
    }
}

static void sc_demo_return_to_menu(void)
{
    sc_root_clear();
    (void)sc_page_switch(SC_DEMO_PAGE_MAIN, NULL);
}

static void sc_demo_root_cb(sc_obj_t *obj, const sc_event_t *event)
{
    (void)obj;
    if (event->type == SC_EVENT_KEY_DOWN && event->dat.key == CMD_BACK)
    {
        sc_demo_return_to_menu();
    }
}

static void sc_demo_back_cb(sc_obj_t *obj, const sc_event_t *event)
{
    (void)obj;
    if (sc_demo_event_activated(event))
    {
        sc_demo_return_to_menu();
    }
}

static void sc_demo_update_uptime(void)
{
    uint32_t seconds = sc_systick_get() / SC_DEMO_CLOCK_MS;
    uint32_t hours = (seconds / 3600U) % 100U;
    uint32_t minutes = (seconds / 60U) % 60U;
    uint32_t secs = seconds % 60U;
    k_uptime_text[7] = (char)('0' + hours / 10U);
    k_uptime_text[8] = (char)('0' + hours % 10U);
    k_uptime_text[10] = (char)('0' + minutes / 10U);
    k_uptime_text[11] = (char)('0' + minutes % 10U);
    k_uptime_text[13] = (char)('0' + secs / 10U);
    k_uptime_text[14] = (char)('0' + secs % 10U);
    if (sc_page_current() == SC_DEMO_PAGE_MAIN)
    {
        sc_obj_invalidate(k_uptime_obj);
    }
}

static void sc_demo_uptime_timer(sc_delay_t *timer)
{
    sc_demo_update_uptime();
    sc_reload_delay(timer, SC_DEMO_CLOCK_MS);
}

void sc_demo_collection_init(void)
{
    sc_obj_t *main_root = sc_page_create(NULL, &k_main_page,
                                         SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *title = sc_label_create(main_root, &k_menu_title);
    sc_obj_set_text(title, "SCGUI Demo Collection", SC_FONT_DEFAULT);
    sc_obj_set_rect(title, 0, 14, title->view.w, title->view.h, ALIGN_HOR);

    k_uptime_obj = sc_label_create(main_root, &k_uptime_label);
    sc_demo_update_uptime();
    sc_obj_set_text(k_uptime_obj, k_uptime_text, SC_FONT_DEFAULT);
    sc_obj_set_rect(k_uptime_obj, 0, 36,
                    k_uptime_obj->view.w, k_uptime_obj->view.h, ALIGN_HOR);

    sc_obj_t *container_button = sc_button_create(main_root, &k_menu_buttons[0]);
    sc_obj_set_text(container_button, "Container", SC_FONT_DEFAULT);
    sc_obj_set_callback(container_button, sc_demo_container_cb);
    sc_obj_set_radius(container_button, 8U);
    sc_obj_set_border_width(container_button, 1U);
    sc_obj_set_border(container_button, C_WHITE);
    sc_obj_set_bg(container_button, C_RGB(28, 104, 168));
    sc_obj_set_fg(container_button, C_WHITE);
    sc_obj_set_rect(container_button, 16, 54, 136U, 48U, ALIGN_NONE);

    sc_obj_t *label_button = sc_button_create(main_root, &k_menu_buttons[1]);
    sc_obj_set_text(label_button, "Label", SC_FONT_DEFAULT);
    sc_obj_set_callback(label_button, sc_demo_label_cb);
    sc_obj_set_radius(label_button, 8U);
    sc_obj_set_border_width(label_button, 1U);
    sc_obj_set_border(label_button, C_WHITE);
    sc_obj_set_bg(label_button, C_RGB(40, 132, 84));
    sc_obj_set_fg(label_button, C_WHITE);
    sc_obj_set_rect(label_button, 168, 54, 136U, 48U, ALIGN_NONE);

    sc_obj_t *round_rect_button = sc_button_create(main_root, &k_menu_buttons[2]);
    sc_obj_set_text(round_rect_button, "Rounded", SC_FONT_DEFAULT);
    sc_obj_set_callback(round_rect_button, sc_demo_round_rect_cb);
    sc_obj_set_radius(round_rect_button, 8U);
    sc_obj_set_border_width(round_rect_button, 1U);
    sc_obj_set_border(round_rect_button, C_WHITE);
    sc_obj_set_bg(round_rect_button, C_RGB(152, 72, 40));
    sc_obj_set_fg(round_rect_button, C_WHITE);
    sc_obj_set_rect(round_rect_button, 16, 110, 136U, 48U, ALIGN_NONE);

    sc_obj_t *text_box_button = sc_button_create(main_root, &k_menu_buttons[3]);
    sc_obj_set_text(text_box_button, "Text Box", SC_FONT_DEFAULT);
    sc_obj_set_callback(text_box_button, sc_demo_text_box_cb);
    sc_obj_set_radius(text_box_button, 8U);
    sc_obj_set_border_width(text_box_button, 1U);
    sc_obj_set_border(text_box_button, C_WHITE);
    sc_obj_set_bg(text_box_button, C_RGB(104, 72, 152));
    sc_obj_set_fg(text_box_button, C_WHITE);
    sc_obj_set_rect(text_box_button, 168, 110, 136U, 48U, ALIGN_NONE);

    sc_obj_t *line_edit_button = sc_button_create(main_root, &k_menu_buttons[4]);
    sc_obj_set_text(line_edit_button, "Line Edit", SC_FONT_DEFAULT);
    sc_obj_set_callback(line_edit_button, sc_demo_line_edit_cb);
    sc_obj_set_radius(line_edit_button, 8U);
    sc_obj_set_border_width(line_edit_button, 1U);
    sc_obj_set_border(line_edit_button, C_WHITE);
    sc_obj_set_bg(line_edit_button, C_RGB(56, 112, 120));
    sc_obj_set_fg(line_edit_button, C_WHITE);
    sc_obj_set_rect(line_edit_button, 92, 166, 136U, 48U, ALIGN_NONE);

    sc_add_delay(sc_demo_uptime_timer, SC_DEMO_CLOCK_MS, NULL);
    sc_invalidate_all();
}

static void sc_demo_enter(void (*demo_init)(void))
{
    if (!sc_page_switch(SC_DEMO_PAGE_CHILD, sc_demo_root_cb))
    {
        return;
    }

    demo_init();
    sc_obj_t *back_button = sc_button_create(NULL, &k_back_button);
    sc_obj_set_text(back_button, "Back", SC_FONT_DEFAULT);
    sc_obj_set_callback(back_button, sc_demo_back_cb);
    sc_obj_set_radius(back_button, 6U);
    sc_obj_set_border_width(back_button, 1U);
    sc_obj_set_border(back_button, C_WHITE);
    sc_obj_set_bg(back_button, C_RGB(48, 56, 72));
    sc_obj_set_fg(back_button, C_WHITE);
    sc_obj_set_rect(back_button, 248, 6, 64U, 30U, ALIGN_NONE);
    sc_invalidate_all();
}

#endif /* SC_USER_OBJ */
