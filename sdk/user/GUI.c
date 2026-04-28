
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
static uint8_t Gui_PointInAngle(int x, int y, int angle_s, int angle_e)
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





// 实心填充矩形
// x0, y0 : 左上角坐标
// w      : 矩形宽度
// h      : 矩形高度
// color  : 填充颜色
void Gui_DrawFillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color)
{
    // 防止宽度/高度为0，避免死循环
    if(w == 0 || h == 0) return;

    // 计算右下角坐标
    uint16_t x1 = x0 + w;
    uint16_t y1 = y0 + h;

    // 逐行画线填充（最稳定、最通用、M0无压力）
    for(uint16_t y = y0; y <= y1; y++)
    {
        Gui_DrawLine(x0, y, x1, y, color);
    }
}





// 水平进度条（带边框 + 内部填充）
// x0,y0    左上角坐标
// w,h      总宽高
// percent  进度 0~100
// frameCol 边框颜色
// barCol   进度条颜色
// bgCol    背景颜色
void Gui_ProgressBar(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
                    uint8_t percent, uint16_t frameCol, uint16_t barCol, uint16_t bgCol)
{
    // 限制进度 0~100
    if(percent > 100) percent = 100;

    // 画背景
    Gui_DrawFillRect(x0, y0, w-1, h-1, bgCol);

    // 画进度
    uint16_t bar_w = (percent * (w-4)) / 100;
    if(bar_w > 0)
    {
        Gui_DrawFillRect(x0+2, y0+2, bar_w, h-5, barCol);
    }

    // 画边框
    Gui_DrawRect(x0, y0, w-1, h-1, frameCol);
}



// 坐标(50,100)，宽150，高20
// 进度 60%
// 边框：黑色
// 进度条：绿色
// 背景：灰色
//Gui_ProgressBar(50, 100, 150, 20, 60, BLACK, GREEN, GRAY);











// 内部辅助：判断角度并画点（纯整数，超快）
static void DrawArcPoint(uint16_t cx, uint16_t cy, int x, int y,
                         int sa, int ea, uint16_t color)
{
    int angle;

    if(x == 0 && y == 0) return;
    if(x >= 0 && y > 0)  angle = (x*45)/y;
    else if(x < 0 && y >= 0) angle = 90 - ((-x)*45)/y;
    else if(x <= 0 && y < 0) angle = 180 + ((-x)*45)/(-y);
    else angle = 270 - ((x)*45)/(-y);

    if( (angle >= sa && angle <= ea) ||
        (sa > ea && (angle >= sa || angle <= ea)) )
    {
        Gui_DrawPoint(cx + x, cy + y, color);
    }
}






/*
Gui_DrawArc 函数参数说明：
x0      圆心的X轴坐标
y0      圆心的Y轴坐标
r       圆弧的半径，必须大于等于1
sa      圆弧的起始角度，范围0~360
ea      圆弧的结束角度，范围0~360
color   绘制圆弧使用的颜色
*/

// 纯整数 Bresenham 圆弧函数（Cortex-M0 专用）
void Gui_DrawArc(uint16_t x0, uint16_t y0, uint16_t r,
                 int16_t sa, int16_t ea, uint16_t color)
{
    int x = 0, y = r;
    int d = 3 - 2 * r;

    while (x <= y)
    {
        // 8 对称点绘制（已做角度判断）
        DrawArcPoint(x0, y0, x, y, sa, ea, color);
        DrawArcPoint(x0, y0,-x, y, sa, ea, color);
        DrawArcPoint(x0, y0, x,-y, sa, ea, color);
        DrawArcPoint(x0, y0,-x,-y, sa, ea, color);
        DrawArcPoint(x0, y0, y, x, sa, ea, color);
        DrawArcPoint(x0, y0,-y, x, sa, ea, color);
        DrawArcPoint(x0, y0, y,-x, sa, ea, color);
        DrawArcPoint(x0, y0,-y,-x, sa, ea, color);

        if (d < 0)
            d += 4 * x + 6;
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}



//画右半圆 0° → 180°

//Gui_DrawArc(120, 120, 50, 0, 180, RED);

//画上半圆 270° → 90°
//Gui_DrawArc(120, 120, 50, 270, 90, BLUE);


//画 1/4 圆弧（右下角）0° → 90°
//Gui_DrawArc(120, 120, 50, 0, 90, GREEN);

//画一个完整圆环
//Gui_DrawArc(120, 120, 50, 0, 360, YELLOW);


//画进度条常用圆弧：135°~405°（270° 大圆弧）
//Gui_DrawArc(120, 120, 50, 135, 405, 0xFFFF);


//Gui_DrawArc(圆心X, 圆心Y, 半径, 起始角度, 结束角度, 颜色);
//从起始角度 顺时针 画到 结束角度。





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





void Task_UI_Refresh(void)
{
	
		uint16_t uicount = 90;
	
	
		//ui_paint_color_circle();
	
		ui_paint_bat_percent(uicount);

		if(uicount >= 100)
			uicount = 0;
		else 
			uicount ++;
		
		
		//Gui_ProgressBar(50, 100, 150, 20, 60, BLACK, GREEN, GRAY1);
		
		Gui_DrawArc(120, 120, 50, 135, 405, 0xFFFF);


//		if(count%10 == 1){
//			if(count <= 10)
//				HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_charge_10, 20000);
//			if(count <= 20)
//				HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_charge_20, 20000);
//			if(count <= 30)
//				HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_charge_30, 20000);
//		}
		

		
//		GuiShowNumString_16(50, 18, count, 2);
//		GuiShowNumString_48(32, 35, count, 2);
//		GuiShowPersent(45+40, 45);
//		GuiShowNumString_16(50, 85, count, 2);

	
}




// 实心圆填充函数
// X, Y : 圆心坐标
// R    : 半径
// fc   : 填充颜色
void Gui_FillCircle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc)
{
    unsigned short a, b;
    int c;
    a = 0;
    b = R;
    c = 3 - 2 * R;
    while (a < b)
    {
        // 填充水平线
        Gui_DrawLine(X - a, Y + b, X + a, Y + b, fc);
        Gui_DrawLine(X - a, Y - b, X + a, Y - b, fc);
        Gui_DrawLine(X - b, Y + a, X + b, Y + a, fc);
        Gui_DrawLine(X - b, Y - a, X + b, Y - a, fc);

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
        Gui_DrawLine(X - a, Y + b, X + a, Y + b, fc);
        Gui_DrawLine(X - a, Y - b, X + a, Y - b, fc);
        Gui_DrawLine(X - b, Y + a, X + b, Y + a, fc);
        Gui_DrawLine(X - b, Y - a, X + b, Y - a, fc);
    }
}

