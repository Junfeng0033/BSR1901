#include "arc_demo.h"

#if SC_USER_OBJ && SC_WIDGET_ARC_ENABLE

#define ARC_LOAD_MS 30U

static sc_container_t k_page;
static sc_arc_t k_arc_load;
static sc_arc_t k_arc_bar;
static sc_label_t k_title;
static sc_label_t k_hint;
static sc_obj_t *k_arc_load_obj;

static void sc_arc_load_timer(sc_delay_t *timer)
{
    sc_obj_t *obj= timer->arg;
    if (!obj) return;
    sc_arc_t *arc=obj->props;
    int start = (int16_t)((arc->start_deg + 5) % 360);
    int end = (int16_t)((arc->end_deg + 5) % 360);
    sc_arc_set_range(k_arc_load_obj, start, end);
    sc_reload_delay(timer, ARC_LOAD_MS);
}

void sc_arc_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *title = sc_label_create(screen, &k_title);
    sc_obj_set_text(title, "Arc demo (loading / drag)", SC_FONT_DEFAULT);
    sc_obj_set_pos(title, 12, 8);

    /* Loading spinner: short gradient sweep rotated by timer. */
    k_arc_load_obj = sc_arc_create(screen, &k_arc_load);
    k_arc_load.start_c = C_DODGER_BLUE;
    k_arc_load.end_c = C_CYAN;
    k_arc_load.fill = g_gui->bg;
    k_arc_load.dot = 1U;
    k_arc_load.stroke_width = 14U;

    sc_obj_set_rect(k_arc_load_obj,
                    (int16_t)(SC_SCREEN_WIDTH / 3 - 48),
                    (int16_t)(SC_SCREEN_HEIGHT / 2 - 48),
                    96U, 96U, ALIGN_NONE);
    sc_arc_set_range(k_arc_load_obj, 0, 300);
    sc_add_delay(sc_arc_load_timer, ARC_LOAD_MS, k_arc_load_obj);

    /* Interactive arc bar: touch / drag to change progress. */
    sc_obj_t *arc_bar = sc_arc_create(screen, &k_arc_bar);
    k_arc_bar.start_c = C_ORANGE;
    k_arc_bar.end_c = C_RED;
    k_arc_bar.fill = C_DIM_GRAY;
    k_arc_bar.dot = 1U;
    k_arc_bar.is_bar = 1U;
    k_arc_bar.stroke_width = 14U;
    k_arc_bar.bar_deg = 120;
    sc_obj_set_rect(arc_bar,
                    (int16_t)(SC_SCREEN_WIDTH * 2 / 3 - 48),
                    (int16_t)(SC_SCREEN_HEIGHT / 2 - 48),
                    96U, 96U, ALIGN_NONE);
    sc_arc_set_range(arc_bar, 30, 330);
    sc_arc_set_bar_deg(arc_bar, 120);

    sc_obj_t *hint = sc_label_create(screen, &k_hint);
    sc_obj_set_text(hint, "loading / arc_bar", SC_FONT_DEFAULT);
    sc_obj_set_pos(hint, 12, SC_SCREEN_HEIGHT - 28);
}

#endif /* SC_USER_OBJ && SC_WIDGET_ARC_ENABLE */
