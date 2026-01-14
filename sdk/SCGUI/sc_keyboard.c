#include "sc_keyboard.h"

/*
 *  键盘驱动偷懒实现，借鉴灵动GUI的常量布局https://gitee.com/gzbkey/LingDongGUI
 *  感谢作都开源，请保留作者版权
 */

// 数字键盘按钮信息
#define NUM_COL_NUM (5)
#define NUM_BTN_W_SPACE ((LD_CFG_SCREEN_WIDTH - KB_SPACE) / NUM_COL_NUM)
#define NUM_START ((LD_CFG_SCREEN_WIDTH - NUM_BTN_W_SPACE * NUM_COL_NUM - KB_SPACE) / 2)
#define NUM_BTN_W (NUM_BTN_W_SPACE - KB_SPACE)
#define NUM_BTN_H (((LD_CFG_SCREEN_HEIGHT >> 1) - KB_SPACE) / 4 - KB_SPACE)
#define NUM_OFFSET_W(num) (KB_SPACE + (NUM_BTN_W + KB_SPACE) * num)
#define NUM_OFFSET_H(num) (KB_SPACE + (NUM_BTN_H + KB_SPACE) * num + (LD_CFG_SCREEN_HEIGHT >> 1))

const kbBtnInfo_t numBtnInfo[4][5] = {
    {
        {NUM_START + NUM_OFFSET_W(0), NUM_OFFSET_H(0), NUM_BTN_W, NUM_BTN_H, 0, '1', 0},
        {NUM_START + NUM_OFFSET_W(1), NUM_OFFSET_H(0), NUM_BTN_W, NUM_BTN_H, 0, '2', 0},
        {NUM_START + NUM_OFFSET_W(2), NUM_OFFSET_H(0), NUM_BTN_W, NUM_BTN_H, 0, '3', 0},
        {NUM_START + NUM_OFFSET_W(3), NUM_OFFSET_H(0), NUM_BTN_W, NUM_BTN_H, 0, '+', 1},
        {NUM_START + NUM_OFFSET_W(4), NUM_OFFSET_H(0), NUM_BTN_W, NUM_BTN_H, "<-", KB_VALUE_BACKSPACE, 1},
    },
    {
        {NUM_START + NUM_OFFSET_W(0), NUM_OFFSET_H(1), NUM_BTN_W, NUM_BTN_H, 0, '4', 0},
        {NUM_START + NUM_OFFSET_W(1), NUM_OFFSET_H(1), NUM_BTN_W, NUM_BTN_H, 0, '5', 0},
        {NUM_START + NUM_OFFSET_W(2), NUM_OFFSET_H(1), NUM_BTN_W, NUM_BTN_H, 0, '6', 0},
        {NUM_START + NUM_OFFSET_W(3), NUM_OFFSET_H(1), NUM_BTN_W, NUM_BTN_H, 0, '-', 0},
        {NUM_START + NUM_OFFSET_W(4), NUM_OFFSET_H(1), NUM_BTN_W, NUM_BTN_H, "ABC", KB_VALUE_QWERTY_MODE, 1},
    },
    {
        {NUM_START + NUM_OFFSET_W(0), NUM_OFFSET_H(2), NUM_BTN_W, NUM_BTN_H, 0, '7', 0},
        {NUM_START + NUM_OFFSET_W(1), NUM_OFFSET_H(2), NUM_BTN_W, NUM_BTN_H, 0, '8', 0},
        {NUM_START + NUM_OFFSET_W(2), NUM_OFFSET_H(2), NUM_BTN_W, NUM_BTN_H, 0, '9', 0},
        {NUM_START + NUM_OFFSET_W(3), NUM_OFFSET_H(2), NUM_BTN_W, NUM_BTN_H, 0, '*', 0},
        {NUM_START + NUM_OFFSET_W(4), NUM_OFFSET_H(2), 0, 0, 0, 0, 0},
    },
    {
        {NUM_START + NUM_OFFSET_W(0), NUM_OFFSET_H(3), NUM_BTN_W, NUM_BTN_H, "!@", KB_VALUE_SYMBOL_MODE, 1},
        {NUM_START + NUM_OFFSET_W(1), NUM_OFFSET_H(3), NUM_BTN_W, NUM_BTN_H, 0, '0', 0},
        {NUM_START + NUM_OFFSET_W(2), NUM_OFFSET_H(3), NUM_BTN_W, NUM_BTN_H, 0, '.', 1},
        {NUM_START + NUM_OFFSET_W(3), NUM_OFFSET_H(3), NUM_BTN_W, NUM_BTN_H, 0, '/', 0},
        {NUM_START + NUM_OFFSET_W(4), NUM_OFFSET_H(2), NUM_BTN_W, (NUM_BTN_H << 1) + KB_SPACE, "Enter", KB_VALUE_ENTER, 1},
    }};

// 字母键盘按钮信息
#define QWERT_COL_NUM (10)

#define QWERT_BTN_W_SPACE ((LD_CFG_SCREEN_WIDTH - KB_SPACE) / QWERT_COL_NUM)
#define QWERT_START ((LD_CFG_SCREEN_WIDTH - QWERT_BTN_W_SPACE * QWERT_COL_NUM - KB_SPACE) / 2)
#define QWERT_BTN_W (QWERT_BTN_W_SPACE - KB_SPACE)
#define QWERT_BTN_H (((LD_CFG_SCREEN_HEIGHT >> 1) - KB_SPACE) / 4 - KB_SPACE)
#define QWERT_OFFSET_W(num) (QWERT_BTN_W_SPACE * (num / 2) + KB_SPACE + QWERT_BTN_W * (num % 2) / 2) // (KB_SPACE+((QWERT_BTN_W+KB_SPACE)>>1)*num)
#define QWERT_OFFSET_H(num) (KB_SPACE + (QWERT_BTN_H + KB_SPACE) * num + (LD_CFG_SCREEN_HEIGHT >> 1))

const kbBtnInfo_t asciiBtnInfo[4][10] = {
    {
        {QWERT_START + QWERT_OFFSET_W(0), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, 'q', 0},
        {QWERT_START + QWERT_OFFSET_W(2), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, 'w', 0},
        {QWERT_START + QWERT_OFFSET_W(4), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, 'e', 0},
        {QWERT_START + QWERT_OFFSET_W(6), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, 'r', 0},
        {QWERT_START + QWERT_OFFSET_W(8), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, 't', 0},
        {QWERT_START + QWERT_OFFSET_W(10), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, 'y', 0},
        {QWERT_START + QWERT_OFFSET_W(12), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, 'u', 0},
        {QWERT_START + QWERT_OFFSET_W(14), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, 'i', 0},
        {QWERT_START + QWERT_OFFSET_W(16), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, 'o', 0},
        {QWERT_START + QWERT_OFFSET_W(18), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, 'p', 0},
    },
    {
        {QWERT_START + QWERT_OFFSET_W(0), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, 'a', 0},
        {QWERT_START + QWERT_OFFSET_W(2), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, 's', 0},
        {QWERT_START + QWERT_OFFSET_W(4), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, 'd', 0},
        {QWERT_START + QWERT_OFFSET_W(6), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, 'f', 0},
        {QWERT_START + QWERT_OFFSET_W(8), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, 'g', 0},
        {QWERT_START + QWERT_OFFSET_W(10), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, 'h', 0},
        {QWERT_START + QWERT_OFFSET_W(12), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, 'j', 0},
        {QWERT_START + QWERT_OFFSET_W(14), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, 'k', 0},
        {QWERT_START + QWERT_OFFSET_W(16), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, 'l', 0},
        {QWERT_START + QWERT_OFFSET_W(18), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, "<-", KB_VALUE_BACKSPACE, 1},
    },
    {

        {QWERT_START + QWERT_OFFSET_W(0), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, "!@", KB_VALUE_SYMBOL_MODE, 1},
        {QWERT_START + QWERT_OFFSET_W(2), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, 'z', 0},
        {QWERT_START + QWERT_OFFSET_W(4), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, 'x', 0},
        {QWERT_START + QWERT_OFFSET_W(6), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, 'c', 0},
        {QWERT_START + QWERT_OFFSET_W(8), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, 'v', 0},
        {QWERT_START + QWERT_OFFSET_W(10), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, 'b', 0},
        {QWERT_START + QWERT_OFFSET_W(12), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, 'n', 0},
        {QWERT_START + QWERT_OFFSET_W(14), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, 'm', 0},
        {QWERT_START + QWERT_OFFSET_W(16), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, "^", KB_VALUE_UP, 1},
        {QWERT_START + QWERT_OFFSET_W(18), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, "123", KB_VALUE_NUMBER_MODE, 1},

    },
    {
        {QWERT_START + QWERT_OFFSET_W(0), QWERT_OFFSET_H(3), QWERT_BTN_W, QWERT_BTN_H, "Esc", KB_VALUE_ESC, 1},
        {QWERT_START + QWERT_OFFSET_W(2), QWERT_OFFSET_H(3), QWERT_BTN_W / 2 + QWERT_BTN_W, QWERT_BTN_H, "shift", KB_VALUE_SHIFT_MODE, 1},
        {QWERT_START + QWERT_OFFSET_W(5), QWERT_OFFSET_H(3), QWERT_BTN_W_SPACE * 5 / 2, QWERT_BTN_H, 0, ' ', 0},
        {QWERT_START + QWERT_OFFSET_W(10), QWERT_OFFSET_H(3), QWERT_BTN_W * 2 + KB_SPACE, QWERT_BTN_H, "Enter", KB_VALUE_ENTER, 1},
        {QWERT_START + QWERT_OFFSET_W(14), QWERT_OFFSET_H(3), QWERT_BTN_W, QWERT_BTN_H, "<", KB_VALUE_LEFT, 1},
        {QWERT_START + QWERT_OFFSET_W(16), QWERT_OFFSET_H(3), QWERT_BTN_W, QWERT_BTN_H, "v", KB_VALUE_DOWN, 1},
        {QWERT_START + QWERT_OFFSET_W(18), QWERT_OFFSET_H(3), QWERT_BTN_W, QWERT_BTN_H, ">", KB_VALUE_RIGHT, 1},

        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
    },
};

// 符号键盘按钮信息
const kbBtnInfo_t symbolBtnInfo[4][10] = {
    {
        {QWERT_START + QWERT_OFFSET_W(0), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, '-', 0},
        {QWERT_START + QWERT_OFFSET_W(2), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, '/', 0},
        {QWERT_START + QWERT_OFFSET_W(4), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, ':', 0},
        {QWERT_START + QWERT_OFFSET_W(6), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, ';', 0},
        {QWERT_START + QWERT_OFFSET_W(8), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, '(', 0},
        {QWERT_START + QWERT_OFFSET_W(10), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, ')', 0},
        {QWERT_START + QWERT_OFFSET_W(12), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, '_', 0},
        {QWERT_START + QWERT_OFFSET_W(14), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, '$', 0},
        {QWERT_START + QWERT_OFFSET_W(16), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, '&', 0},
        {QWERT_START + QWERT_OFFSET_W(18), QWERT_OFFSET_H(0), QWERT_BTN_W, QWERT_BTN_H, 0, '"', 0},
    },
    {
        {QWERT_START + QWERT_OFFSET_W(0), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, '[', 0},
        {QWERT_START + QWERT_OFFSET_W(2), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, ']', 0},
        {QWERT_START + QWERT_OFFSET_W(4), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, '{', 0},
        {QWERT_START + QWERT_OFFSET_W(6), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, '}', 0},
        {QWERT_START + QWERT_OFFSET_W(8), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, '#', 0},
        {QWERT_START + QWERT_OFFSET_W(10), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, '%', 0},
        {QWERT_START + QWERT_OFFSET_W(12), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, '^', 0},
        {QWERT_START + QWERT_OFFSET_W(14), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, '*', 0},
        {QWERT_START + QWERT_OFFSET_W(16), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, '+', 0},
        {QWERT_START + QWERT_OFFSET_W(18), QWERT_OFFSET_H(1), QWERT_BTN_W, QWERT_BTN_H, 0, '=', 0},
    },
    {
        {QWERT_START + QWERT_OFFSET_W(0), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, '\\', 0},
        {QWERT_START + QWERT_OFFSET_W(2), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, '|', 0},
        {QWERT_START + QWERT_OFFSET_W(4), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, '<', 0},
        {QWERT_START + QWERT_OFFSET_W(6), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, '>', 0},
        {QWERT_START + QWERT_OFFSET_W(8), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, '~', 0},
        {QWERT_START + QWERT_OFFSET_W(10), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, ',', 0},
        {QWERT_START + QWERT_OFFSET_W(12), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, '@', 0},
        {QWERT_START + QWERT_OFFSET_W(14), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, '!', 0},
        {QWERT_START + QWERT_OFFSET_W(16), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, '`', 0},
        {QWERT_START + QWERT_OFFSET_W(18), QWERT_OFFSET_H(2), QWERT_BTN_W, QWERT_BTN_H, 0, '\'', 0},
    },
    {
        {QWERT_START + QWERT_OFFSET_W(0), QWERT_OFFSET_H(3), QWERT_BTN_W * 2 + KB_SPACE, QWERT_BTN_H, "ABC", KB_VALUE_QWERTY_MODE, 1},
        {QWERT_START + QWERT_OFFSET_W(4), QWERT_OFFSET_H(3), QWERT_BTN_W, QWERT_BTN_H, 0, '.', 0},
        {QWERT_START + QWERT_OFFSET_W(6), QWERT_OFFSET_H(3), QWERT_BTN_W_SPACE * 3 + QWERT_BTN_W, QWERT_BTN_H, 0, ' ', 0},
        {QWERT_START + QWERT_OFFSET_W(14), QWERT_OFFSET_H(3), QWERT_BTN_W, QWERT_BTN_H, 0, '?', 0},
        {QWERT_START + QWERT_OFFSET_W(16), QWERT_OFFSET_H(3), QWERT_BTN_W * 2 + KB_SPACE, QWERT_BTN_H, "<-", KB_VALUE_BACKSPACE, 1},

        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
    },
};

/******************************************************************************************
 * 初始化键盘类型
 ******************************************************************************************/
void sc_init_keyboard(kb_ctx_t *kb, uint8_t type)
{
    kb->curr_type = (KB_TYPE_E)type;
    kb->row_num = 4; // 所有键盘固定4行
    kb->now = NULL;  // 当前选中的按键
    switch (type)
    {
    case KB_TYPE_NUM:
        // 数字键盘：5列
        kb->kb_tab = (kbBtnInfo_t *)numBtnInfo;
        kb->col_num = 5;
        break;
    case KB_TYPE_SHIFT:
        kb->upper = 1 - kb->upper;
    case KB_TYPE_ASCII:
        // 字母键盘：10列
        kb->kb_tab = (kbBtnInfo_t *)asciiBtnInfo;
        kb->col_num = 10;
        break;

    case KB_TYPE_SYMBOL:
        // 符号键盘：10列
        kb->kb_tab = (kbBtnInfo_t *)symbolBtnInfo;
        kb->col_num = 10;
        break;
    default:
        break;
    }
}

/******************************************************************************************
 * 内部函数：绘制单个按键
 ******************************************************************************************/

#define to_upper(c) ((c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c) // 大小写转换
static char draw_kb_btn(sc_pfb_t *dest, const kbBtnInfo_t *p, color_t tc, color_t kc, uint8_t upper)
{
    sc_rect_t *coord = (sc_rect_t *)&p->region;
    if (coord->w == 0 || coord->h == 0)
        return 0;
    sc_button_t button; // 按键对象
    if (p->pText)
    {
        sc_init_Button(&button, gui->font, (char*)p->pText, tc);
    }
    else
    {
        char vol[2] = {0};
        vol[0] = upper ? to_upper(p->value) : p->value; // 大写
        sc_init_Button(&button, gui->font, vol, tc);
    }
    sc_draw_Button(dest, coord->x, coord->y, coord->w, coord->h, 4, 4, tc, kc, &button);
    return p->value;
}
// 绘制键盘
void sc_draw_keyboard(sc_pfb_t *dest, kb_ctx_t *kb)
{
    sc_rect_t rect = {0, LD_CFG_SCREEN_HEIGHT >> 1, LD_CFG_SCREEN_WIDTH, LD_CFG_SCREEN_HEIGHT >> 1};
    // 1. 先更新全局键盘上下文
    sc_draw_Rounded_rect(dest, rect.x, rect.y, rect.w, rect.h, 6, 6, gui->fc, C_WHEAT, 255);
    gui->bkc = C_WHEAT;
    // 3. 遍历当前键盘的坐标+文本数组，绘制所有按键
    for (int y = 0; y < kb->row_num; y++)
    {
        for (int x = 0; x < kb->col_num; x++)
        {
            const kbBtnInfo_t *p = &kb->kb_tab[y * kb->col_num + x];
            draw_kb_btn(dest, p, gui->fc, C_WHEAT, kb->upper);
        }
    }
}

// 触摸回调
void sc_demo_keyboard_task(sc_event_t *event)
{
    static kb_ctx_t keyboard;
    if (event->type == SC_EVENT_TYPE_INIT)
    {
        sc_init_keyboard(&keyboard, 1);
        sc_draw_keyboard(NULL, &keyboard);
    }
    else if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        keyboard.now = NULL;
        for (int y = 0; y < keyboard.row_num; y++)
        {
            for (int x = 0; x < keyboard.col_num; x++)
            {
                const kbBtnInfo_t *p = &keyboard.kb_tab[y * keyboard.col_num + x];
                if (sc_rect_touch_ctx((sc_rect_t *)p, event->dat.pos[0], event->dat.pos[1]))
                {
                    keyboard.now = p;
                    int cmd = draw_kb_btn(NULL, p, gui->fc, C_RED, keyboard.upper);

                    return;
                }
            }
        }
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        if (keyboard.now)
        {
            if (keyboard.now->value & 0x80)
            {
                sc_init_keyboard(&keyboard, keyboard.now->value & 0x0f);
                sc_draw_keyboard(NULL, &keyboard);
                return;
            }
            else
            {
                draw_kb_btn(NULL, keyboard.now, gui->fc, C_WHEAT, keyboard.upper);
            }
            keyboard.now = NULL;
        }
    }
}
