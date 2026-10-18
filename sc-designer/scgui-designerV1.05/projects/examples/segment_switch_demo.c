#include "segment_switch_demo.h"
#include "../src/sc_gui.h"

#if SC_USER_OBJ && SC_WIDGET_SEGMENT_SWITCH_ENABLE

/* 每档独立配置：label + selected_color + selected_text_color */
static const sc_segment_item_t g_items_3[] = {
    {"L1", 0x07E0, 0xFFFF},  /* 绿底白字 */
    {"L2", 0x001F, 0xFFFF},  /* 黄底白字 */
    {"L3", 0xF800, 0xFFFF},  /* 红底白字 */
};
static const sc_segment_item_t g_items_2[] = {
    {"ON",  0x07E0, 0xFFFF},
    {"OFF", 0x8410, 0xFFFF},
};
static const sc_segment_item_t g_items_4[] = {
    {"A", 0x001F, 0xFFFF},
    {"B", 0x07E0, 0xFFFF},
    {"C", 0x8410, 0xFFFF},
    {"D", 0xFFE0, 0xFFFF},
};

static sc_container_t g_page;

/* props 必须为全局变量，create 内部用 SC_SEGMENT_SWITCH_NEW 初始化默认值 */
static sc_segment_switch_t g_seg3;
static sc_segment_switch_t g_seg2;
static sc_segment_switch_t g_seg4;


void sc_segment_switch_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &g_page,
                                       SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    int16_t cx = (SC_SCREEN_WIDTH - 240) / 2;  /* 水平居中 */

    /* 3 段开关（L1/L2/L3，绿黄红） */
    sc_obj_t *sw1 = sc_segment_switch_create(screen, &g_seg3);
    g_seg3.track_color = 0xE71C;
    g_seg3.text_color = 0x0000;
    sc_segment_switch_set_items(sw1, g_items_3, 3);
    sc_obj_set_rect(sw1, cx, 30, 240, 40, ALIGN_NONE);

    /* 2 段开关（ON/OFF） */
    sc_obj_t *sw2 = sc_segment_switch_create(screen, &g_seg2);
    g_seg2.track_color = 0xDEF7;
    g_seg2.text_color = 0x0000;
    sc_segment_switch_set_items(sw2, g_items_2, 2);
    sc_segment_switch_set_selected(sw2, 1);
    sc_obj_set_rect(sw2, cx, 90, 240, 40, ALIGN_NONE);

    /* 4 段开关（A/B/C/D） */
    sc_obj_t *sw3 = sc_segment_switch_create(screen, &g_seg4);
    g_seg4.track_color = 0xF800;
    g_seg4.text_color = 0xFFFF;
    sc_segment_switch_set_items(sw3, g_items_4, 4);
    sc_segment_switch_set_selected(sw3, 2);
    sc_obj_set_rect(sw3, cx, 150, 240, 40, ALIGN_NONE);
}

#elif SC_USER_OBJ

void sc_segment_switch_demo_init(void)
{
}

#endif
