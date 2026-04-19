#ifndef _GUI_H_
#define _GUI_H_

#include "stdint.h"

#include "sc_gui.h"


void Task_UI_Refresh(void);


//void Gui_Circle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc);
//void Gui_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Color);
void GuiShowString(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s);
void GuiShowNum(uint8_t x, uint8_t y, uint16_t num, uint8_t len);
void GuiShowChar_16(uint16_t x, uint16_t y, uint8_t s);




void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);

void Gui_Circle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc);
void Gui_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Color);
void Gui_DrawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t Color);




// ===================== ºËÐÄº¯Êý =====================
// ¹¦ÄÜ£º»­Ô²»¡ / ÉÈÐÎ
// X0,Y0£ºÔ²ÐÄ
// R    £º°ë¾¶
// angleStart, angleEnd£º0~360
// color£ºÑÕÉ«
// isSector£º0=Ô²»¡  1=ÉÈÐÎ
void Gui_DrawSector_M0(uint16_t X0, uint16_t Y0, uint16_t R,
                       int angleStart, int angleEnd,
                       uint16_t color, uint8_t isSector);
											 
											 

											 
										 
extern void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end);

extern void Lcd_Clear(uint16_t Color);
extern void Lcd_Fill(uint16_t x,uint16_t y,uint16_t xend,uint16_t yend,uint16_t Color);
extern void sc_draw_Fill(sc_pfb_t *dest, int xs, int ys, int w, int h, color_t color, uint16_t alpha);
											 
extern void sc_draw_Text(sc_pfb_t *dest, int tx, int ty, lv_font_t *font, const char *text, color_t tc, color_t bc);

extern void sc_draw_Num(sc_pfb_t *dest, int tx, int ty, int w, int h, lv_font_t *font, int num, int den, color_t tc, color_t bc);	

									 
extern void ui_paint_bat_percent(uint8_t percent);

	
#endif

