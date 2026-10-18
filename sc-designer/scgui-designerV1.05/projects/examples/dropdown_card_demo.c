#include "sc_gui.h"
#include "widget/sc_round_rect.h"
#include "widget/sc_label.h"
#include "widget/sc_button.h"
#include "widget/sc_dropdown_card.h"
#include "dropdown_card_demo.h"

#include <stdio.h>

/* 下拉卡片控件 demo：
 * - 用 sc_dropdown_card_t 封装，create 时自动注册 screen 回调捕捉手势
 * - 卡片初始化在屏内（方便上位机布局），init 后调用 show(false) 收起
 * - 链式调用 screen 原回调，不覆盖用户已有逻辑
 */

static sc_container_t g_page;
static sc_dropdown_card_t g_dropdown = {
    .color = C_GRAY,
    .fill = C_WHITE,
    .radius = 8,
    .border = 1,
};
static sc_button_t g_content;
static sc_obj_t *g_card_obj = NULL; /* 卡片对象（root 回调方向键控制用） */
static sc_plain_rect_t g_block;
static sc_plain_rect_t g_block2;
static sc_label_t label;
static sc_label_t label2;

void dropdown_card_demo_init(void)
{

    sc_obj_t *screen = NULL;
   
  //  screen =    sc_page_create(NULL, &g_page,SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);
                                      
    /* top hint（parent=screen） */
    sc_obj_t *hint_obj = sc_label_create(screen, &label);
    sc_obj_set_rect(hint_obj, 0, 30, SC_SCREEN_WIDTH, 50, ALIGN_NONE);
    sc_obj_set_text(hint_obj, "v pull down from top", SC_FONT_DEFAULT);
    sc_obj_set_fg(hint_obj, C_GRAY);
    sc_obj_set_align(hint_obj, ALIGN_CENTER | ALIGN_BORDER_VIS);

    sc_obj_t *hint_obj2 = sc_label_create(screen, &label2);
    sc_obj_set_rect(hint_obj2, 320, 30, SC_SCREEN_WIDTH, 50, ALIGN_NONE);
    sc_obj_set_text(hint_obj2, "is page 2", SC_FONT_DEFAULT);
    sc_obj_set_fg(hint_obj2, C_GRAY);
    sc_obj_set_align(hint_obj2, ALIGN_CENTER | ALIGN_BORDER_VIS);

    /* 创建下拉卡片控件：自动注册 screen 回调，捕捉下拉/上拉手势 */
    sc_obj_t *card_obj = sc_dropdown_card_create(screen, &g_dropdown);
    g_card_obj = card_obj;
    /* create 后设置位置和大小 */
    sc_obj_set_rect(card_obj,
                    (SC_SCREEN_WIDTH - 280) / 2,
                    20,
                    280, 150, ALIGN_NONE);
    /* 布局完成后收起（直接隐藏，无动画），运行时下拉再用动画展开 */
    sc_dropdown_card_show(card_obj, false, false);

    /* 卡片内容 button（parent=card） */
    sc_obj_t *content = sc_button_create(card_obj, &g_content);
    sc_obj_set_text(content, "Click Me", SC_FONT_DEFAULT);
    sc_obj_set_rect(content, 0, 0, 280 - 80, 48, ALIGN_CENTER);

#if 0
    /* 方块动画：绿色矩形循环移动（左上→右下↔往返），与下拉卡片动画合并运行 */
    sc_obj_t *g_block_obj = sc_plain_rect_create(screen, &g_block);
    g_block.fill = C_GREEN;
    sc_obj_set_rect(g_block_obj, 10, 90, 60, 40, ALIGN_NONE);
    sc_anim_t *block_anim = sc_anim_create(g_block_obj);
    sc_anim_set_move_xy(block_anim,
                        (int16_t)(SC_SCREEN_WIDTH - 60),
                        (int16_t)(SC_SCREEN_HEIGHT - 40));
    sc_anim_config(block_anim, SC_ANIM_CURVE_EASE_IN, 1000, 0xFF);

    sc_obj_t *g_block_obj2 = sc_plain_rect_create(screen, &g_block2);
    g_block2.fill = C_BLUE;
    sc_obj_set_rect(g_block_obj2, 280, 90, 60, 40, ALIGN_NONE);
    sc_anim_t *block_anim2 = sc_anim_create(g_block_obj2);
    sc_anim_set_move_xy(block_anim2,
                        (int16_t)(0),
                        (int16_t)(SC_SCREEN_HEIGHT - 40));
    sc_anim_config(block_anim2, SC_ANIM_CURVE_EASE, 1000, 0xFF);

    sc_anim_chain(block_anim, block_anim2);
    sc_anim_start(block_anim);
#endif
}
