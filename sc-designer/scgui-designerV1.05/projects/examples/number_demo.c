#include "number_demo.h"

#if SC_USER_OBJ

#define SC_DEMO_TIMER_MS 200U   /* 定时器驱动周期 */
#define SC_DEMO_TEMP_MIN 0      /* 温度锯齿下限（定点，0.0） */
#define SC_DEMO_TEMP_MAX 400    /* 温度锯齿上限（定点，40.0） */
#define SC_DEMO_PAD_MAX 99999   /* 补 0 对齐计数上限 */

static sc_container_t k_page;
static sc_label_t k_title_props;
static sc_label_t k_hint_props;
static sc_label_t k_temp_cap_props;
static sc_label_t k_count_cap_props;
static sc_label_t k_pad_cap_props;
static sc_number_t k_temp_props;
static sc_number_t k_count_props;
static sc_number_t k_pad_props;

static sc_obj_t *k_temp_obj;
static sc_obj_t *k_count_obj;
static sc_obj_t *k_pad_obj;
static int8_t k_temp_dir = 1;   /* 温度变化方向：+0.1 / -0.1 */

/* 定时器驱动数据：温度 0.0↔40.0 锯齿往返、计数 +1、补 0 对齐计数 +1（循环）。 */
static void sc_number_demo_timer(sc_delay_t *timer)
{
    if (!k_temp_obj || !k_count_obj || !k_pad_obj) return;
    sc_number_t *temp = (sc_number_t *)k_temp_obj->props;
    int32_t pad_val;
    if (temp->value >= SC_DEMO_TEMP_MAX || temp->value <= SC_DEMO_TEMP_MIN)
    {
        k_temp_dir = (int8_t)-k_temp_dir;
    }
    sc_number_set_value(k_temp_obj, temp->value + k_temp_dir);
    sc_number_set_value(k_count_obj, ((sc_number_t *)k_count_obj->props)->value + 1);
    pad_val = ((sc_number_t *)k_pad_obj->props)->value + 1;
    if (pad_val > SC_DEMO_PAD_MAX)
    {
        pad_val = 0;
    }
    sc_number_set_value(k_pad_obj, pad_val);
    sc_reload_delay(timer, SC_DEMO_TIMER_MS);
}

void sc_number_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *title = sc_label_create(screen, &k_title_props);
    sc_obj_set_fg(title, C_YELLOW);
    sc_obj_set_text(title, "Number Widget Demo", SC_FONT_DEFAULT);
    sc_obj_set_rect(title, 0, 8, title->view.w, title->view.h, ALIGN_HOR);

    sc_obj_t *hint = sc_label_create(screen, &k_hint_props);
    sc_obj_set_fg(hint, C_CYAN);
    sc_obj_set_text(hint, "Timer-driven display-only", SC_FONT_DEFAULT);
    sc_obj_set_rect(hint, 0, 26, hint->view.w, hint->view.h, ALIGN_HOR);

    sc_obj_t *temp_cap = sc_label_create(screen, &k_temp_cap_props);
    sc_obj_set_text(temp_cap, "TEMP (1 decimal):", SC_FONT_DEFAULT);
    sc_obj_set_rect(temp_cap, 20, 56, temp_cap->view.w, temp_cap->view.h, ALIGN_NONE);

    sc_obj_t *temp = sc_number_create(screen, &k_temp_props);
    k_temp_props.int_width = 2;
    k_temp_props.decimals = 1;
    sc_number_set_value(temp, 250); /* 25.0 */
    sc_obj_set_fg(temp, C_GREEN);
    sc_obj_set_rect(temp, 0, 82, temp->view.w, temp->view.h, ALIGN_HOR);
    k_temp_obj = temp;

    sc_obj_t *count_cap = sc_label_create(screen, &k_count_cap_props);
    sc_obj_set_text(count_cap, "COUNT (integer):", SC_FONT_DEFAULT);
    sc_obj_set_rect(count_cap, 20, 118, count_cap->view.w, count_cap->view.h, ALIGN_NONE);

    sc_obj_t *count = sc_number_create(screen, &k_count_props);
    /* 整数：decimals=0、int_width=0 即默认，无需改动 */
    sc_number_set_value(count, 0);
    sc_obj_set_fg(count, C_DODGER_BLUE);
    sc_obj_set_rect(count, 0, 144, count->view.w, count->view.h, ALIGN_HOR);
    k_count_obj = count;

    sc_obj_t *pad_cap = sc_label_create(screen, &k_pad_cap_props);
    sc_obj_set_text(pad_cap, "PAD (3 int, 2 dec):", SC_FONT_DEFAULT);
    sc_obj_set_rect(pad_cap, 20, 180, pad_cap->view.w, pad_cap->view.h, ALIGN_NONE);

    sc_obj_t *pad = sc_number_create(screen, &k_pad_props);
    k_pad_props.decimals = 2;
    k_pad_props.int_width = 3;
    sc_number_set_value(pad, 45); /* 000.45 补 0 对齐 */
    sc_obj_set_fg(pad, C_ORANGE);
    sc_obj_set_rect(pad, 0, 206, pad->view.w, pad->view.h, ALIGN_HOR);
    k_pad_obj = pad;

    sc_add_delay(sc_number_demo_timer, SC_DEMO_TIMER_MS, NULL);
    sc_invalidate_all();
}

#endif /* SC_USER_OBJ */
