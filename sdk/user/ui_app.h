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
extern const uint8_t gImage_bat_90x49[];
extern const unsigned char gImage_circle_100x100[20000];



extern void Lcd_Write_data_dma(uint8_t *p_data, uint16_t len);





//绘制空电池
void ui_paint_bat(void);

//绘制电池容量
void ui_paint_bat_remain(uint8_t percent);


void GuiShowNum(uint8_t x, uint8_t y, uint16_t num, uint8_t len);

void GuiShowChar_16(uint16_t x, uint16_t y, uint8_t s);


void Gui_Circle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc);


void Gui_DrawArc(uint16_t x0, uint16_t y0, uint16_t r,
                 int16_t sa, int16_t ea, uint16_t color);


#endif


