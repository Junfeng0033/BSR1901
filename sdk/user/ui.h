#ifndef _UI_H_
#define _UI_H_

#include "stdint.h"



typedef struct source{
	uint8_t *p_img;
	uint16_t width;
	uint16_t high;
}img_source_t;



extern const uint8_t Font_8x32_percent[];
extern const uint8_t gImage_black_128x128[32768];
extern const uint8_t Font48_dital_Table [];
//extern const uint8_t gImage_bat_90x49[];
extern const unsigned char gImage_circle_100x100[20000];







//绘制空电池
void ui_paint_bat(void);

//绘制电池容量
void ui_paint_bat_remain(uint8_t percent);




#endif


