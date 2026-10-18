#include "waveform_demo.h"

#if SC_USER_OBJ && SC_WIDGET_WAVEFORM_ENABLE

#define WF_DEMO_MS 40U
#define WF_DEMO_BUF 256U

static sc_container_t k_page;
static sc_waveform_t k_wf;
static sc_button_t k_btn_a;
static sc_button_t k_btn_b;
static sc_label_t k_title;
static sc_label_t k_hint;
static sc_obj_t *k_wf_obj;
static int16_t k_ch0[WF_DEMO_BUF];
static int16_t k_ch1[WF_DEMO_BUF];
static int16_t k_ch2[WF_DEMO_BUF];
static uint16_t k_angle;
static uint16_t k_vol;
static uint32_t k_rand = 1U;

static int16_t sc_wf_demo_rand_range(int16_t span)
{
    k_rand = k_rand * 1103515245U + 12345U;
    return (int16_t)((k_rand >> 16) % (uint32_t)span) - (int16_t)(span / 2);
}

static void sc_waveform_demo_timer(sc_delay_t *timer)
{
    if (!k_wf_obj) return;
    k_angle = (uint16_t)((k_angle + 16U) % 360U);
    k_vol = (uint16_t)((k_vol + 1U) % 60U);
    sc_waveform_put(k_wf_obj, 0U, (int16_t)(sc_sin((int16_t)k_angle) >> 9));
    sc_waveform_put(k_wf_obj, 1U, sc_wf_demo_rand_range(60));
    sc_waveform_put(k_wf_obj, 2U, (int16_t)(k_vol - 30));
    sc_reload_delay(timer, WF_DEMO_MS);
}

void sc_waveform_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *title = sc_label_create(screen, &k_title);
    sc_obj_set_text(title, "Waveform demo", SC_FONT_DEFAULT);
    sc_obj_set_pos(title, 12, 8);

    k_wf_obj = sc_waveform_create(screen, &k_wf);
    sc_waveform_set_geometry(k_wf_obj, 8, 36,
                             (uint16_t)(SC_SCREEN_WIDTH - 16),
                             (uint16_t)(SC_SCREEN_HEIGHT - 120),
                             ALIGN_NONE);
    sc_waveform_set_channel(k_wf_obj, 0U, k_ch0, WF_DEMO_BUF, C_RED);
    sc_waveform_set_channel(k_wf_obj, 1U, k_ch1, WF_DEMO_BUF, C_LIME);
    sc_waveform_set_channel(k_wf_obj, 2U, k_ch2, WF_DEMO_BUF, C_YELLOW);
    sc_waveform_obj_set_scale_x(k_wf_obj, SC_WAVEFORM_SCALE_4X);
    sc_waveform_obj_set_scale_y(k_wf_obj, SC_WAVEFORM_SCALE_1X);
    sc_obj_set_key_focus(k_wf_obj);
    sc_add_delay(sc_waveform_demo_timer, WF_DEMO_MS, k_wf_obj);

    const int16_t btn_y = (int16_t)(SC_SCREEN_HEIGHT - 72);
    sc_obj_t *btn_a = sc_button_create(screen, &k_btn_a);
    sc_obj_set_text(btn_a, "Btn A", SC_FONT_DEFAULT);
    sc_obj_set_radius(btn_a, 8U);
    sc_obj_set_border_width(btn_a, 1U);
    sc_obj_set_border(btn_a, C_WHITE);
    sc_obj_set_fg(btn_a, C_WHITE);
    sc_obj_set_bg(btn_a, C_DODGER_BLUE);
    sc_obj_set_rect(btn_a, 12, btn_y, 100, 36, ALIGN_NONE);

    sc_obj_t *btn_b = sc_button_create(screen, &k_btn_b);
    sc_obj_set_text(btn_b, "Btn B", SC_FONT_DEFAULT);
    sc_obj_set_radius(btn_b, 8U);
    sc_obj_set_border_width(btn_b, 1U);
    sc_obj_set_border(btn_b, C_WHITE);
    sc_obj_set_fg(btn_b, C_WHITE);
    sc_obj_set_bg(btn_b, C_ORANGE);
    sc_obj_set_rect(btn_b, 124, btn_y, 100, 36, ALIGN_NONE);

    sc_obj_t *hint = sc_label_create(screen, &k_hint);
    sc_obj_set_text(hint, "Enter:X/Y  Left/Right:scale  swipe:X/Y", SC_FONT_DEFAULT);
    sc_obj_set_pos(hint, 12, SC_SCREEN_HEIGHT - 28);
}

#endif /* SC_USER_OBJ && SC_WIDGET_WAVEFORM_ENABLE */
