#ifndef _GUI_H_
#define _GUI_H_

#include "stdint.h"

void Gui_Circle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc);
void Gui_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Color);
void GuiShowString(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s);
void GuiShowNum(uint8_t x, uint8_t y, uint16_t num, uint8_t len);
void GuiShowChar_16(uint16_t x, uint16_t y, uint8_t s);

#endif

