#include "lcd_driver.h"
#include "GUI.h"
#include "Font.h"

/*
使用示例
GuiShowChar(30, 30, 'T');
GuiShowNum(30, 60, 123, 3);
GuiShowString(30, 90, 0, 0xFFFF, "STRING");
Gui_Circle(60, 60, 10, 0);
Gui_DrawLine(100, 100, 200, 200, 0);
*/

//如果报错可以替换以下两个函数定义以匹配显示驱动
//#define Lcd_SetXY(X, Y) 			LCD_1IN69_SetWindows(X, Y, X, Y)
//#define Gui_DrawPoint(x, y, c) 	LCD_1IN69_DrawPoint(x, y, c)

// Bresenham算法
void Gui_arc(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc)
{
	unsigned short a, b;
	int c;
	a = 0;
	b = R;
	c = 3 - 2 * R;
	while (a < b)
	{
		Gui_DrawPoint(X + a, Y + b, fc); //        7
		Gui_DrawPoint(X - a, Y + b, fc); //        6
		Gui_DrawPoint(X + a, Y - b, fc); //        2
		Gui_DrawPoint(X - a, Y - b, fc); //        3
		Gui_DrawPoint(X + b, Y + a, fc); //        8
		Gui_DrawPoint(X - b, Y + a, fc); //        5
		Gui_DrawPoint(X + b, Y - a, fc); //        1
		Gui_DrawPoint(X - b, Y - a, fc); //        4

		if (c < 0)
			c = c + 4 * a + 6;
		else
		{
			c = c + 4 * (a - b) + 10;
			b -= 1;
		}
		a += 1;
	}
	
	if (a == b)
	{
		Gui_DrawPoint(X + a, Y + b, fc);
		Gui_DrawPoint(X + a, Y + b, fc);
		Gui_DrawPoint(X + a, Y - b, fc);
		Gui_DrawPoint(X - a, Y - b, fc);
		Gui_DrawPoint(X + b, Y + a, fc);
		Gui_DrawPoint(X - b, Y + a, fc);
		Gui_DrawPoint(X + b, Y - a, fc);
		Gui_DrawPoint(X - b, Y - a, fc);
	}
}

// Bresenham算法
void Gui_Circle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc)
{
	unsigned short a, b;
	int c;
	a = 0;
	b = R;
	c = 3 - 2 * R;
	while (a < b)
	{
		Gui_DrawPoint(X + a, Y + b, fc); //        7
		Gui_DrawPoint(X - a, Y + b, fc); //        6
		Gui_DrawPoint(X + a, Y - b, fc); //        2
		Gui_DrawPoint(X - a, Y - b, fc); //        3
		Gui_DrawPoint(X + b, Y + a, fc); //        8
		Gui_DrawPoint(X - b, Y + a, fc); //        5
		Gui_DrawPoint(X + b, Y - a, fc); //        1
		Gui_DrawPoint(X - b, Y - a, fc); //        4

		if (c < 0)
			c = c + 4 * a + 6;
		else
		{
			c = c + 4 * (a - b) + 10;
			b -= 1;
		}
		a += 1;
	}
	if (a == b)
	{
		Gui_DrawPoint(X + a, Y + b, fc);
		Gui_DrawPoint(X + a, Y + b, fc);
		Gui_DrawPoint(X + a, Y - b, fc);
		Gui_DrawPoint(X - a, Y - b, fc);
		Gui_DrawPoint(X + b, Y + a, fc);
		Gui_DrawPoint(X - b, Y + a, fc);
		Gui_DrawPoint(X + b, Y - a, fc);
		Gui_DrawPoint(X - b, Y - a, fc);
	}
}

// 画线函数，使用Bresenham 画线算法
void Gui_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Color)
{
	int dx,	 // difference in x's
		dy,	 // difference in y's
		dx2, // dx,dy * 2
		dy2,
		x_inc, // amount in pixel space to move during drawing
		y_inc, // amount in pixel space to move during drawing
		error, // the discriminant i.e. error i.e. decision variable
		index; // used for looping

	Lcd_SetXY(x0, y0);
	dx = x1 - x0; // 计算x距离
	dy = y1 - y0; // 计算y距离

	if (dx >= 0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx = -dx;
	}

	if (dy >= 0)
	{
		y_inc = 1;
	}
	else
	{
		y_inc = -1;
		dy = -dy;
	}

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy) // x距离大于y距离，那么每个x轴上只有一个点，每个y轴上有若干个点
	{			 // 且线的点数等于x距离，以x轴递增画点
		// initialize error term
		error = dy2 - dx;

		// draw the line
		for (index = 0; index <= dx; index++) // 要画的点数不会超过x距离
		{
			// 画点
			Gui_DrawPoint(x0, y0, Color);

			// test if error has overflowed
			if (error >= 0) // 是否需要增加y坐标值
			{
				error -= dx2;

				// move to next line
				y0 += y_inc; // 增加y坐标值
			} // end if error overflowed

			// adjust the error term
			error += dy2;

			// move to the next pixel
			x0 += x_inc; // x坐标值每次画点后都递增1
		} // end for
	} // end if |slope| <= 1
	else // y轴大于x轴，则每个y轴上只有一个点，x轴若干个点
	{	 // 以y轴为递增画点
		// initialize error term
		error = dx2 - dy;

		// draw the line
		for (index = 0; index <= dy; index++)
		{
			// set the pixel
			Gui_DrawPoint(x0, y0, Color);

			// test if error overflowed
			if (error >= 0)
			{
				error -= dy2;

				// move to next line
				x0 += x_inc;
			} // end if error overflowed

			// adjust the error term
			error += dx2;

			// move to the next pixel
			y0 += y_inc;
		} // end for
	} // end else |slope| > 1
}

void GuiShowString(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s)
{
	unsigned char i, j;
	unsigned short k, x0;
	x0 = x;

	while (*s)
	{
		if ((*s) < 128)
		{
			k = *s;
			if (k == 13)
			{
				x = x0;
				y += 16;
			}
			else
			{
				if (k > 32)
					k -= 32;
				else
					k = 0;

				for (i = 0; i < 16; i++){
					for (j = 0; j < 8; j++)
					{
						if (asc16[k * 16 + i] & (0x80 >> j))
							Gui_DrawPoint(x + j, y + i, fc);
						else
						{
							if (fc != bc)
								Gui_DrawPoint(x + j, y + i, bc);
						}
					}
				}
				x += 8;
			}
			s++;
		}
	}
}

//显示字符
void GuiShowChar_16(uint16_t x, uint16_t y, uint8_t s)
{
	unsigned char i, j;

	if (s > 32)
		s -= 32;
	else
		s = 0;

	for (i = 0; i < 16; i++){
		for (j = 0; j < 8; j++){
			if (asc16[s * 16 + i] & (0x80 >> j))
				Gui_DrawPoint(x + j, y + i, GREEN);
//			else
//				Gui_DrawPoint(x + j, y + i, GRAY0);
		}
	}
}

extern const uint8_t Font16_dital_Table [];

//显示字符
void GuiShowNum_16(uint16_t x, uint16_t y, uint8_t n)
{
	unsigned char i, j;

	for (i = 0; i < 16; i++){
		for (j = 0; j < 8; j++){
			if (Font16_dital_Table[n * 16 + i] & (0x80 >> j))
				Gui_DrawPoint(x + j, y + i, GREEN);
			else
				Gui_DrawPoint(x + j, y + i, BLACK);
		}
	}
}

extern const uint8_t Font32_dital_Table [];

//显示字符
void GuiShowNum_32(uint16_t x, uint16_t y, uint8_t n)
{
	unsigned char i, j;

	for (i = 0; i < 32; i++){
		for (j = 0; j < 16; j++){
			if (Font32_dital_Table[n * 64 + i*2 + j/8] & (0x80 >> (j%8)))
				Gui_DrawPoint(x + j, y + i, GREEN);
			else
				Gui_DrawPoint(x + j, y + i, BLACK);
		}
	}
}

extern const uint8_t Font48_dital_Table [];
//显示字符
void GuiShowNum_48(uint16_t x, uint16_t y, uint8_t n)
{
	unsigned char i, j;

	for (i = 0; i < 48; i++){
		for (j = 0; j < 24; j++){
			if (Font48_dital_Table[n * 144 + i*3 + j/8] & (0x80 >> (j%8)))
				Gui_DrawPoint(x + j, y + i, GREEN);
			else
				Gui_DrawPoint(x + j, y + i, BLACK);
		}
	}
}
extern const uint8_t Font32_char_Table [];

//显示字符
void GuiShowChar_32(uint16_t x, uint16_t y, uint8_t n)
{
	unsigned char i, j;

	for (i = 0; i < 32; i++){
		for (j = 0; j < 16; j++){
			if (Font32_char_Table[n * 64 + i*2 + j/8] & (0x80 >> (j%8)))
				Gui_DrawPoint(x + j, y + i, GREEN);
			else
				Gui_DrawPoint(x + j, y + i, BLACK);
		}
	}
}

static int oled_pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--)
		result *= m;
	return result;
}

void GuiShowNum(uint8_t x, uint8_t y, uint16_t num, uint8_t len)
{
	uint8_t t, temp;
	uint8_t enshow = 0;

	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				GuiShowChar_16(x + 8 * t, y, ' ');
				continue;
			}
			else
				enshow = 1;
		}
		GuiShowChar_16(x + 8 * t, y, temp + '0');
	}
}

extern const uint8_t Font_8x32_percent[];

void GuiShowPersent(uint8_t x, uint8_t y)
{
	unsigned char i, j;

	for (i = 0; i < 32; i++){
		for (j = 0; j < 8; j++){
			if (Font_8x32_percent[i] & (0x80 >> j))
				Gui_DrawPoint(x + j, y + i, GREEN);
			else
				Gui_DrawPoint(x + j, y + i, BLACK);
		}
	}
}

void GuiShowNumString_16(uint8_t x, uint8_t y, uint16_t num, uint8_t len)
{
	uint8_t t, temp;
	uint8_t enshow = 0;

	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				GuiShowNum_16(x + 8 * t, y, 0);
				continue;
			}
			else
				enshow = 1;
		}
		GuiShowNum_16(x + 8 * t, y, temp + 0);
	}
}

void GuiShowNumString_32(uint8_t x, uint8_t y, uint16_t num, uint8_t len)
{
	uint8_t t, temp;
	uint8_t enshow = 0;

	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				GuiShowNum_32(x + 16 * t, y, 0);
				continue;
			}
			else
				enshow = 1;
		}
		GuiShowNum_32(x + 16 * t, y, temp + 0);
	}
}

void GuiShowNumString_48(uint8_t x, uint8_t y, uint16_t num, uint8_t len)
{
	uint8_t t, temp;
	uint8_t enshow = 0;

	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				GuiShowNum_48(x + 24 * t, y, 0);
				continue;
			}
			else
				enshow = 1;
		}
		GuiShowNum_48(x + 24 * t, y, temp + 0);
	}
}





void set_bat_percentage(uint8_t percent)
{
	static uint8_t history = 0;
	
	if(percent > 100)
		return ;
	
	if(history == percent)
		return ;
	
	Lcd_Fill(22, 41, 22+80.0/100*percent, 84, 0x051d);
	Lcd_Fill(22+80.0/100*percent+1, 41, 102, 84, BLACK);

	GuiShowNum(50, 55, percent, 3);
	GuiShowChar_16(77,55, '%');
}

void set_charge_circle(uint8_t percent)
{
	static uint8_t history = 0;
	
	if(percent > 100)
		return ;
	
	if(history == percent)
		return ;
	
	if(0 == percent)
		Gui_Circle(64, 64, 60, 0x051d);
	
	Gui_Circle(64, 64, 60, 0x051d);
	

	GuiShowNum(50, 55, percent, 3);
	GuiShowChar_16(77,55, '%');
}



