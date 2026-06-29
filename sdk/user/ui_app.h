#ifndef _UI_H_
#define _UI_H_

#include "stdint.h"
#include "platform_config.h"

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

extern void BSR1901_FireEye_Demo(void);

extern void Lcd_Write_data_dma(uint8_t *p_data, uint16_t len);
extern void HW_SPI_Tx_DMA_16bit_ColorBlock(uint16 *pData, uint16 DataLen);


//绘制空电池
void ui_paint_bat(void);

//绘制电池剩余
void ui_paint_bat_remain(uint8_t percent);


void GuiShowNum(uint8_t x, uint8_t y, uint16_t num, uint8_t len);

void GuiShowChar_16(uint16_t x, uint16_t y, uint8_t s);


void Gui_Circle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc);


void Gui_DrawArc(uint16_t x0, uint16_t y0, uint16_t r,
                 int16_t sa, int16_t ea, uint16_t color);


void Gui_ProgressBar(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
                    uint8_t percent, uint16_t frameCol, uint16_t barCol, uint16_t bgCol);

void Gui_RingProgress(uint16_t cx,uint16_t cy,uint16_t r_out,uint8_t ring_w,uint8_t per,uint16_t col_bg,uint16_t col_pro);

	
#endif
