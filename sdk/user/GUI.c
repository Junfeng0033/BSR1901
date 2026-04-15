
#include "platform_config.h"

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





/*
X	uint16_t	圆心的 水平坐标（左上角为 (0,0)）
Y	uint16_t	圆心的 垂直坐标
R	uint16_t	圆的 半径（像素数）
fc	uint16_t	前景颜色 color

该函数不填充圆内部，只绘制圆的边缘。

*/

#if 0
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

#endif




void Gui_Circle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc)
{
    int a, b;
    int p;  // 判别式

    a = 0;
    b = R;
    p = 3 - 2 * R;  // 初始判别式

    // 正确循环：a <= b，覆盖到45°最后一个点
    while (a <= b)
    {
        // 八分法对称画 8 个点
        Gui_DrawPoint(X + a, Y + b, fc);
        Gui_DrawPoint(X - a, Y + b, fc);
        Gui_DrawPoint(X + a, Y - b, fc);
        Gui_DrawPoint(X - a, Y - b, fc);

        Gui_DrawPoint(X + b, Y + a, fc);
        Gui_DrawPoint(X - b, Y + a, fc);
        Gui_DrawPoint(X + b, Y - a, fc);
        Gui_DrawPoint(X - b, Y - a, fc);

        // 中点算法判别式更新
        if (p < 0)
        {
            p += 4 * a + 6;
        }
        else
        {
            p += 4 * (a - b) + 10;
            b--;
        }
        a++;
    }
}





#if 0
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

#endif


// Bresenham 完美画线函数（优化版）
void Gui_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int x_inc = (dx >= 0) ? 1 : -1;
    int y_inc = (dy >= 0) ? 1 : -1;

    dx = (dx >= 0) ? dx : -dx;
    dy = (dy >= 0) ? dy : -dy;

    int dx2 = dx << 1;
    int dy2 = dy << 1;
    int error;

    if (dx > dy)  // 斜率 |k| < 1，以 x 为主步进
    {
        error = dy2 - dx;
        for (int i = 0; i <= dx; i++)
        {
            Gui_DrawPoint(x0, y0, Color);

            if (error >= 0)
            {
                error -= dx2;
                y0 += y_inc;
            }

            error += dy2;
            x0 += x_inc;
        }
    }
    else   // 斜率 |k| >= 1，以 y 为主步进
    {
        error = dx2 - dy;
        for (int i = 0; i <= dy; i++)
        {
            Gui_DrawPoint(x0, y0, Color);

            if (error >= 0)
            {
                error -= dy2;
                x0 += x_inc;
            }

            error += dx2;
            y0 += y_inc;
        }
    }
}






// 画空心矩形
// x0,y0 = 左上角坐标
// w    = 宽度
// h    = 高度
// Color= 颜色
void Gui_DrawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t Color)
{
    uint16_t x1 = x0 + w;
    uint16_t y1 = y0 + h;

    // 上边框
    Gui_DrawLine(x0, y0, x1, y0, Color);
    // 下边框
    Gui_DrawLine(x0, y1, x1, y1, Color);
    // 左边框
    Gui_DrawLine(x0, y0, x0, y1, Color);
    // 右边框
    Gui_DrawLine(x1, y0, x1, y1, Color);
}



// 画一个左上角在(50,50)，宽100，高60，红色的矩形
//Gui_DrawRect(50, 50, 100, 60, RED);











// 辅助函数：判断点 (x,y) 是否在 [angle_s, angle_e] 角度范围内
// 纯整数运算，超快
//static uint8_t Gui_PointInAngle(int x, int y, int angle_s, int angle_e)
__RAM_CODE__  static uint8_t Gui_PointInAngle(int x, int y, int angle_s, int angle_e)
{
    int quad = 0;
    int degrees;

    if (x == 0 && y == 0) return 0;

    if (x >= 0 && y > 0)  quad = 0;   // 0~90
    else if (x < 0 && y >= 0) quad = 90;  // 90~180
    else if (x <= 0 && y < 0) quad = 180; // 180~270
    else if (x > 0 && y <= 0) quad = 270; // 270~360

    int ax = (x >= 0) ? x : -x;
    int ay = (y >= 0) ? y : -y;

    if (ax == 0) degrees = 90;
    else if (ay == 0) degrees = 0;
    else
    {
        if (ay > ax) degrees = 90 - (ax * 45) / ay;
        else         degrees = (ay * 45) / ax;
    }

    degrees += quad;

    if (angle_s < angle_e)
    {
        return (degrees >= angle_s && degrees <= angle_e);
    }
    else
    {
        return (degrees >= angle_s || degrees <= angle_e);
    }
}






// ===================== 核心函数 =====================
// 功能：画圆弧 / 扇形
// X0,Y0：圆心
// R    ：半径
// angleStart, angleEnd：0~360
// color：颜色
// isSector：0=圆弧  1=扇形
void Gui_DrawSector_M0(uint16_t X0, uint16_t Y0, uint16_t R,
                       int angleStart, int angleEnd,
                       uint16_t color, uint8_t isSector)
{
    int x = 0, y = R;
    int d = 3 - 2 * R;

    // 画8对称点，但只画在角度范围内的
    while (x <= y)
    {
        if (Gui_PointInAngle( x,  y, angleStart, angleEnd)) Gui_DrawPoint(X0 + x, Y0 - y, color);
        if (Gui_PointInAngle(-x,  y, angleStart, angleEnd)) Gui_DrawPoint(X0 - x, Y0 - y, color);
        if (Gui_PointInAngle( x, -y, angleStart, angleEnd)) Gui_DrawPoint(X0 + x, Y0 + y, color);
        if (Gui_PointInAngle(-x, -y, angleStart, angleEnd)) Gui_DrawPoint(X0 - x, Y0 + y, color);
        if (Gui_PointInAngle( y,  x, angleStart, angleEnd)) Gui_DrawPoint(X0 + y, Y0 - x, color);
        if (Gui_PointInAngle(-y,  x, angleStart, angleEnd)) Gui_DrawPoint(X0 - y, Y0 - x, color);
        if (Gui_PointInAngle( y, -x, angleStart, angleEnd)) Gui_DrawPoint(X0 + y, Y0 + x, color);
        if (Gui_PointInAngle(-y, -x, angleStart, angleEnd)) Gui_DrawPoint(X0 - y, Y0 + x, color);

        if (d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }

    // 如果是扇形，画两条边线
    if (isSector)
    {
        int x1 = X0 + R;
        int y1 = Y0;
        int x2 = X0;
        int y2 = Y0 - R;

        Gui_DrawLine(X0, Y0, x1, y1, color);
        Gui_DrawLine(X0, Y0, x2, y2, color);
    }
}






// 画 0~90 度 扇形（超快）
//Gui_DrawSector_M0(120, 120, 40, 0, 90, RED, 1);

// 画 90~180 度 圆弧
//Gui_DrawSector_M0(120, 120, 40, 90, 180, BLUE, 0);







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









#if 1
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

#endif






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



