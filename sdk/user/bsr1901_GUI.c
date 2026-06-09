
#include "platform_config.h"

#include "lcd_driver.h"
#include "bsr1901_GUI.h"
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


void Gui_Circle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc)
{
    int a, b;
    int p;  // 判别式

    a = 0;
    b = R;
    //p = 3 - 2 * R;  // 初始判别式
	  p = 3 - (R<<1);

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
            //p += 4 * a + 6;
					  p += (a<<2) + 6;
        }
        else
        {
            //p += 4 * (a - b) + 10;
					  p += ((a - b)<<2) + 10;
            b--;
        }
        a++;
    }
		
}







// Bresenham 完美画线函数
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





// 专为低主频CPU优化：只画 水平线 + 垂直线，速度极致
void Gui_DrawLine_Fast(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Color)
{
    uint16_t tmp;

    // ==================== 水平线（y 相等）====================
    if(y0 == y1)
    {
        // 保证 x 从小到大，仅一次判断
        if(x0 > x1)
        {
            tmp = x0;
            x0 = x1;
            x1 = tmp;
        }
        // 纯连续画点，无任何复杂运算
        for(; x0 <= x1; x0++)
        {
            Gui_DrawPoint(x0, y0, Color);
        }
    }

    // ==================== 垂直线（x 相等）====================
    else if(x0 == x1)
    {
        // 保证 y 从小到大，仅一次判断
        if(y0 > y1)
        {
            tmp = y0;
            y0 = y1;
            y1 = tmp;
        }
        // 纯连续画点，无任何复杂运算
        for(; y0 <= y1; y0++)
        {
            Gui_DrawPoint(x0, y0, Color);
        }
    }

    // 不支持斜线（你要求只画水平/垂直，这里可以空着）
    else
    {
        // 斜线直接不处理
    }
}







#if 0


// 极快 水平线（仅水平方向，速度最大化）
static inline void Fast_HLine(uint16_t x1, uint16_t x2, uint16_t y, uint16_t color)
{
    // 确保左 <= 右
    if (x1 > x2) {
        uint16_t tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    // 无分支、无判断、纯连续画点，速度极限
    for (uint16_t x = x1; x <= x2; ++x) {
        Gui_DrawPoint(x, y, color);
    }
}

// 极快 垂直线（仅垂直方向，速度最大化）
static inline void Fast_VLine(uint16_t x, uint16_t y1, uint16_t y2, uint16_t color)
{
    // 确保上 <= 下
    if (y1 > y2) {
        uint16_t tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    // 无分支、无判断、纯连续画点，速度极限
    for (uint16_t y = y1; y <= y2; ++y) {
        Gui_DrawPoint(x, y, color);
    }
}



// 画一条从 (10, 50) 到 (100, 50) 的水平线
//Fast_HLine(10, 100, 50, RED);

// 画一条从 (50, 10) 到 (50, 100) 的垂直线
//Fast_VLine(50, 10, 100, BLUE);



#endif



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
    //Gui_DrawLine(x0, y0, x1, y0, Color);
	  Gui_DrawLine_Fast(x0, y0, x1, y0, Color);
    // 下边框
    //Gui_DrawLine(x0, y1, x1, y1, Color);
		Gui_DrawLine_Fast(x0, y1, x1, y1, Color);
    // 左边框
    //Gui_DrawLine(x0, y0, x0, y1, Color);
	  Gui_DrawLine_Fast(x0, y0, x0, y1, Color);
    // 右边框
    //Gui_DrawLine(x1, y0, x1, y1, Color);
	  Gui_DrawLine_Fast(x1, y0, x1, y1, Color);
}



// 画一个左上角在(50,50)，宽100，高60，红色的矩形
//Gui_DrawRect(50, 50, 100, 60, RED);











// 【弱CPU终极优化】无乘法、无除法、超快角度判断
static uint8_t Gui_PointInAngle(int x, int y, int angle_s, int angle_e)
{
    // 1. 原点直接返回
    if (x == 0 && y == 0) return 0;

    // 2. 取绝对值（仅符号判断）
    int ax = (x < 0) ? -x : x;
    int ay = (y < 0) ? -y : y;

    // 3. 快速象限判断（无运算）
    int quad;
    if (x >= 0 && y > 0)  quad = 0;    // 0~90°
    else if (x < 0 && y >= 0) quad = 90;  // 90~180°
    else if (x <= 0 && y < 0) quad = 180; // 180~270°
    else quad = 270;                     // 270~360°

    // 4. 核心：无乘除！只判断 0° / 45° / 90°
    int degrees;
    if (ax == 0)        degrees = 90;  // 垂直向上
    else if (ay == 0)   degrees = 0;   // 水平向右
    else if (ay > ax)   degrees = 45;  // 0~45°区间
    else                degrees = 45;  // 45~90°区间

    degrees += quad;

    // 5. 角度范围判断
    if (angle_s < angle_e) {
        return (degrees >= angle_s && degrees <= angle_e);
    } else {
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
// 【弱CPU优化】圆弧/扇形绘制（无慢运算）
void Gui_DrawSector_M0(uint16_t X0, uint16_t Y0, uint16_t R,
                       int angleStart, int angleEnd,
                       uint16_t color, uint8_t isSector)
{
    int x = 0, y = R;
    int d = 3 - (R << 1);  // 2*R → 位移，无乘法

    // 8对称点画弧（仅判断角度）
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

        // Bresenham 核心：无乘法，全部位移
        if (d < 0) {
            d += (x << 2) + 6;    // 4*x → 位移
        } else {
            d += ((x - y) << 2) + 10; // 4*(x-y) → 位移
            y--;
        }
        x++;
    }

    // 扇形边线（使用你优化后的超快直线）
    if (isSector)
    {
        // 起始角度边线
        int sx = X0 + R;
        int sy = Y0;
        Gui_DrawLine(X0, Y0, sx, sy, color);

        // 结束角度边线
        int ex = X0;
        int ey = Y0 - R;
        Gui_DrawLine(X0, Y0, ex, ey, color);
    }
}




// 画 0~90 度 扇形（超快）
//Gui_DrawSector_M0(120, 120, 40, 0, 90, RED, 1);

// 画 90~180 度 圆弧
//Gui_DrawSector_M0(120, 120, 40, 90, 180, BLUE, 0);









// 【弱CPU终极优化】圆弧画点：无除法、无乘法、极快
static void DrawArcPoint(uint16_t cx, uint16_t cy, int x, int y,
                         int sa, int ea, uint16_t color)
{
    uint8_t quad;  // 象限：1~4
    int ax, ay;

    // 1. 原点直接返回（极快判断）
    if(x == 0 && y == 0) return;

    // 2. 取绝对值（无运算，仅符号处理）
    ax = (x < 0) ? -x : x;
    ay = (y < 0) ? -y : y;

    // 3. 快速判断象限（无运算）
    if(x >= 0 && y > 0)  quad = 1;    // 第一象限 0~90°
    else if(x < 0 && y >= 0) quad = 2; // 第二象限 90~180°
    else if(x <= 0 && y < 0) quad = 3; // 第三象限 180~270°
    else quad = 4;                     // 第四象限 270~360°

    // 4. 核心优化：用 ax/ay 大小判断角度区间，**无除法！无乘法！**
    //  Bresenham 圆弧 x<=y 区间，只需要判断 0°、45°、90° 三个点
    uint8_t ang_part;
    if(ax == 0) ang_part = 0;        // 0°
    else if(ay > ax) ang_part = 45; // 0~45°
    else ang_part = 90;             // 45~90°

    // 5. 直接计算角度（纯加法，极快）
    int angle;
    if(quad == 1) angle = ang_part;
    else if(quad == 2) angle = 180 - ang_part;
    else if(quad == 3) angle = 180 + ang_part;
    else angle = 360 - ang_part;

    // 6. 角度区间判断（不变）
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
    //int d = 3 - 2 * r;
	  int d = 3 - (r << 1);  // 2*r → 位移，无乘法

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
            //d += 4 * x + 6;
				    d += (x << 2) + 6;  // 4*x → 位移
        else
        {
            //d += 4 * (x - y) + 10;
					  d += ((x - y) << 2) + 10;  // 4*(x-y) → 位移
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
        //Gui_DrawLine(x0, y, x1, y, color);
			  Gui_DrawLine_Fast(x0, y, x1, y, color);
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





/*
* 环形圆环进度条
* cx,cy:圆环圆心坐标
* r_out:圆环外圆半径
* ring_w:圆环线条宽度
* per:进度0~100(uint8_t)
* col_bg:圆环底色
* col_pro:进度前景色
* 绘制规则：起始135°，顺时针走到135+270°，总跨度270°(缺右上一小段开口)
*/
void Gui_RingProgress(uint16_t cx,uint16_t cy,uint16_t r_out,uint8_t ring_w,uint8_t per,uint16_t col_bg,uint16_t col_pro)
{
    uint16_t r_in;
    uint16_t end_ang;

    /* 进度限幅，uint8无符号无需判小于0 */
    if(per > 100)
        per = 100;
    r_in = r_out - ring_w;
    if(r_in < 1) r_in = 1;

    /* 总有效角度270度，计算结束角度 */
    end_ang = 135 + ((uint32_t)per * 270)/100;

    /* 1.先画整圈底色环：内外两层圆弧 */
    Gui_DrawArc(cx,cy,r_out,135,405,col_bg);
    Gui_DrawArc(cx,cy,r_in,135,405,col_bg);
    /* 填充环中间空隙 */
    for(uint16_t i=r_in+1;i<r_out;i++)
    {
        Gui_DrawArc(cx,cy,i,135,405,col_bg);
    }

    /* 2.绘制当前进度前景 */
    Gui_DrawArc(cx,cy,r_out,135,end_ang,col_pro);
    Gui_DrawArc(cx,cy,r_in,135,end_ang,col_pro);
    for(uint16_t i=r_in+1;i<r_out;i++)
    {
        Gui_DrawArc(cx,cy,i,135,end_ang,col_pro);
    }
}



//圆心(80,80)，外半径35，环宽6，进度65%，灰底色，绿色进度
//Gui_RingProgress(80,80,35,6,65,GRAY,GREEN);


//关键特点
//圆环粗细由 ring_w 控制，ring_w=6 就是 6 像素粗的圆环；
//0%：整段圆环全是背景灰色；
//100%：整段 270° 圆环全部被前景色填满；


//Gui_RingProgress(80,80,35,6,0,GRAY,GREEN); → 全灰圆环
//Gui_RingProgress(80,80,35,6,100,GRAY,GREEN); → 全绿圆环






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
					  //if (asc16[k * 16 + i] & (0x80 >> j))
						if (asc16[(k<<4)+ i] & (0x80 >> j))
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











// 【极致优化】实心圆填充函数（低主频CPU专用，无冗余运算）
// X, Y : 圆心坐标
// R    : 半径
// fc   : 填充颜色
void Gui_FillCircle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc)
{
    uint16_t a = 0;
    uint16_t b = R;
    int32_t c = 3 - 2 * R;  // 用32位避免溢出，运算更快

    // 合并判断，去掉最后冗余的 a==b 重复代码
    while (a <= b)
    {
        // 直接调用你优化后的超快直线，4行对称水平线
			
//        Gui_DrawLine(X - a, Y + b, X + a, Y + b, fc);  // 上
//        Gui_DrawLine(X - a, Y - b, X + a, Y - b, fc);  // 下
//        Gui_DrawLine(X - b, Y + a, X + b, Y + a, fc);  // 中
//        Gui_DrawLine(X - b, Y - a, X + b, Y - a, fc);  // 中

        Gui_DrawLine_Fast(X - a, Y + b, X + a, Y + b, fc);  // 上
        Gui_DrawLine_Fast(X - a, Y - b, X + a, Y - b, fc);  // 下
        Gui_DrawLine_Fast(X - b, Y + a, X + b, Y + a, fc);  // 中
        Gui_DrawLine_Fast(X - b, Y - a, X + b, Y - a, fc);  // 中			
			

        // Bresenham 核心算法：简化运算、减少计算量
        if (c < 0)
        {
            c += (a << 2) + 6;  // 4*a → 左移2位，CPU 1个周期完成
        }
        else
        {
            c += ((a - b) << 2) + 10;  // 4*(a-b) → 位移运算
            b--;
        }
        a++;
    }
}





//空心圆角矩形

void Gui_DrawRoundedRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color)
{
    uint16_t x2 = x + w;
    uint16_t y2 = y + h;

    // 4 条边
    Gui_DrawLine_Fast(x + r, y, x2 - r, y, color);
    Gui_DrawLine_Fast(x + r, y2, x2 - r, y2, color);
    Gui_DrawLine_Fast(x, y + r, x, y2 - r, color);
    Gui_DrawLine_Fast(x2, y + r, x2, y2 - r, color);

    // 4 个圆角弧
    Gui_DrawArc(x + r, y + r, r, 180, 270, color);
    Gui_DrawArc(x2 - r, y + r, r, 270, 360, color);
    Gui_DrawArc(x2 - r, y2 - r, r, 0, 90, color);
    Gui_DrawArc(x + r, y2 - r, r, 90, 180, color);
}




//透明文字（不覆盖背景，必用）
//Gui_ShowString_Transparent(左上角X, 左上角Y, 文字颜色, "你要显示的文字");
void Gui_ShowString_Transparent(uint16_t x, uint16_t y, uint16_t fc, uint8_t *s)
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
                if (k > 32) k -= 32;
                else k = 0;

                for (i = 0; i < 16; i++){
                    for (j = 0; j < 8; j++)
                    {
                        if (asc16[(k << 4) + i] & (0x80 >> j))
                            Gui_DrawPoint(x + j, y + i, fc);
                    }
                }
                x += 8;
            }
            s++;
        }
    }
}




// 坐标(30,30)，白色文字，内容：HELLO
//Gui_ShowString_Transparent(30, 30, WHITE, "HELLO");



// 先画一个实心圆
//Gui_FillCircle(64, 64, 30, BLUE);

// 在圆中心透明显示数字（不会擦掉圆）
//Gui_ShowString_Transparent(50, 55, WHITE, "100");





//Gui_ProgressBar(50,100, 150,20, 60, BLACK,GREEN,GRAY);

// 透明显示 60%，不破坏进度条
//Gui_ShowString_Transparent(70, 102, WHITE, "60%");

