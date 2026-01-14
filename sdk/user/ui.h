#ifndef _UI_H_
#define _UI_H_

#include "stdint.h"

//绘制空电池
void ui_paint_bat(void);

//绘制电池容量
void ui_paint_bat_remain(uint8_t percent);

void ui_dma_busy_release(void);


#endif


