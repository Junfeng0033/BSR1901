#include "sc_gui.h"
#include "widget/sc_round_rect.h"
#include "anim_demo.h"

/* 组合动画测试：绿色矩形移动同时透明度渐变，两个动画串联同时运行。 */

static sc_container_t g_page;
static sc_plain_rect_t g_rect = SC_PLAIN_RECT_NEW(0x0000, 0x07E0, 0, 0);  /* 绿色实心矩形 */
static sc_obj_t *g_rect_obj;

void anim_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &g_page,
                                       SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    /* 创建绿色矩形，初始位置左上角，尺寸 60x40。 */
    g_rect_obj = sc_plain_rect_create(screen, &g_rect);
    sc_obj_set_rect(g_rect_obj, 10, 10, 60, 40, ALIGN_NONE);

    /* 动画1：XY 循环移动，线性匀速，从左上→右下→左上 来回循环。 */
    sc_anim_t *move_anim = sc_anim_create(g_rect_obj);
    sc_anim_set_move_xy(move_anim,
                         (int16_t)(SC_SCREEN_WIDTH - 60),
                         (int16_t)(SC_SCREEN_HEIGHT - 40));
    sc_anim_config(move_anim, SC_ANIM_CURVE_LINEAR, 500, 0xFF);

    /* 动画2：透明度渐变，正弦曲线，255↔0 淡入淡出，与移动同步循环。 */
    sc_anim_t *alpha_anim = sc_anim_create(g_rect_obj);
    sc_anim_config(alpha_anim, SC_ANIM_CURVE_SINE, 500, 0xFF);
    sc_anim_set_alpha(alpha_anim, 255, 128, 0xFF);

    /* 串联两个动画，同时运行，共用同一定时器。 */
    sc_anim_start(sc_anim_chain(move_anim, alpha_anim));
}
