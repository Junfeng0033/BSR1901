#ifndef SC_KEYBOARD_H
#define SC_KEYBOARD_H

/*
 *  键盘驱动偷懒实现，借鉴灵动GUI的常量布局https://gitee.com/gzbkey/LingDongGUI
 *  感谢作都开源，请保留作者版权
 */

#include "sc_gui.h"
///-----------------------
#define LD_CFG_SCREEN_WIDTH (SC_SCREEN_WIDTH)   // 屏幕宽度
#define LD_CFG_SCREEN_HEIGHT (SC_SCREEN_HEIGHT) // 屏幕高度
#define KB_SPACE (SC_SCREEN_WIDTH / 80)

#define KB_VALUE_NONE (0x00)      // 无按键
#define KB_VALUE_BACKSPACE (0x08) // 退格键
#define KB_VALUE_ENTER (0x0d)     // 回车键

#define KB_VALUE_UP (0x7F)    // 上
#define KB_VALUE_DOWN (0x7E)  // 下
#define KB_VALUE_LEFT (0x7D)  // 左
#define KB_VALUE_RIGHT (0x7C) // 右
#define KB_VALUE_ESC (0x7B)

#define KB_VALUE_NUMBER_MODE (0x80) // 数字键盘模式
#define KB_VALUE_QWERTY_MODE (0x81) // 字母键盘模式
#define KB_VALUE_SYMBOL_MODE (0x82) // 符号键盘模式
#define KB_VALUE_SHIFT_MODE (0x83)  // 大写键盘模式

typedef struct
{
    sc_rect_t region;
    uint8_t *pText;
    uint8_t value;
    bool colorType;
} kbBtnInfo_t;

typedef enum
{
    KB_TYPE_NUM = 0,    // 数字键盘
    KB_TYPE_ASCII = 1,  // 字母键盘
    KB_TYPE_SYMBOL = 2, // 符号键盘
    KB_TYPE_SHIFT = 3   // 大写键盘
} KB_TYPE_E;

typedef struct
{
    KB_TYPE_E curr_type;       // 当前键盘类型（数字/字母/符号）
    const kbBtnInfo_t *kb_tab; // 当前键盘文本数组指针（兼容4列/10列）
    const kbBtnInfo_t *now;    // 当前按键指针
    uint8_t col_num;           // 当前键盘列数（4列/10列）
    uint8_t row_num : 7;       // 当前键盘行数（固定4行）
    uint8_t upper : 1;         // 大写键盘模式
} kb_ctx_t;

#endif // KEYBOARD_LAYOUT_H
