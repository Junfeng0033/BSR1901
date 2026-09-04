#include "lcd_driver.h"
#include "stdint.h"
#include "gpio.h"
#include "spi.h"
#include "iomux.h"
#include "dma.h"
#include "Font.h"
#include "ui_app.h"

#include "lcd_config.h"



/******************************************************************************************************

//游程编码（Run-Length Encoding, RLE）压缩算法

//https://github.com/KOUFU-DIY/WeGui_RGB

//https://gitee.com/li_yucheng/scgui

//https://gitee.com/landlord_money/ugui

//https://github.com/olikraus/u8g2

//https://gitee.com/Polarix/simplegui

******************************************************************************************************/




app_lcd_t lcd;



void delay_1ms(void)
{
    unsigned int i;
    unsigned int count = 2;

    for (i=0; i<110; i++)
    {
		while(count--);
		count = 2;
    }
}

void delay_ms(unsigned int delay_val)
{
    unsigned int i;

    for (i=0; i<delay_val; i++)
    {
		delay_1ms();
    }
}



//液晶IO初始化配置
void LCD_GPIO_Init(void)
{
	//gecko_pinmux_config(PAD24,GPIO_B_4);//BL control,default function,do not needed to configure
	gecko_pinmux_config(PAD21,GPIO_A_3);//DC control	
	gecko_pinmux_config(PAD7,GPIOB_7);//RES(reset) control
	
	gpio_set_output(LCD_BL_PORT, LCD_BL_PIN);
	gpio_set_output(LCD_DC_PORT, LCD_DC_PIN);
	gpio_set_output(LCD_RST_PORT, LCD_RST_PIN);

}





void Lcd_Reset(void)
{
	LCD_RST_SET;
	delay_ms(100);
	LCD_RST_CLR;
	delay_ms(600);
	LCD_RST_SET;
	delay_ms(50);
}






void Lcd_Init(void)
{	
	LCD_GPIO_Init();

	Lcd_Reset(); //Reset before LCD Init.

	
#if defined(LCD_TYPE_GC9A01_1P28_240X240)
    LCD_Init_GC9A01_M128T_240240();
#elif defined(LCD_TYPE_ST7735S_0P96_80X160)
    LCD_Init_ST7735S_UN096T_80X160();	
#elif defined(LCD_TYPE_NV3022B_1P47_128X28)
    LCD_Init_NV3022B_1P47_128X128();
#else
#endif	

	
}





void app_lcd_set_direction(app_lcd_direction_t dir)
{
  lcd.dir = dir;
#ifdef LCD_TYPE_ST7735S_0P96_80X160
  uint8_t data;
  switch(dir)
  {
    case LCD_DIRECTION_NORMAL:
      lcd.x_max = 80;
      lcd.y_max = 160;
      lcd.x_offset = 24;
      lcd.y_offset = 0;
      data = 0x00;            //正常刷新
      break;
    case LCD_DIRECTION_90:
      lcd.x_max = 160;
      lcd.y_max = 80;
      lcd.x_offset = 1;
      lcd.y_offset = 26;
      data = 0x70;            //顺时针旋转90°
      break;
    case LCD_DIRECTION_180:
      lcd.x_max = 80;
      lcd.y_max = 160;
      lcd.x_offset = 24;
      lcd.y_offset = 0;
      data = 0xC0;            //顺时针旋转180°
      break;
    case LCD_DIRECTION_270:
      lcd.x_max = 160;
      lcd.y_max = 80;
      lcd.x_offset = 0;
      lcd.y_offset = 24;
      data = 0xA0;            //顺时针旋转270°
      break;
  }
  data |= 0x08;               //此款屏幕采用了 BGR 的硬件形式，需要设置0x36 的BIT4为 1
  app_lcd_write_cmd(0x36);    //内存访问控制 带1个参数
  app_lcd_write_data8(data);
#endif
}



/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{	

  //SPI_8bit_Transfer();
	
	Lcd_WriteIndex(0x2a);

	LCD_WriteData_16Bit(x_start);//LCD_WriteData_16Bit

	LCD_WriteData_16Bit(x_end);

	Lcd_WriteIndex(0x2b);
	
/*
RASET(0x2B) 写入 Y 坐标时加上偏移
LCD_WriteData_16Bit(y_start + 60);
LCD_WriteData_16Bit(y_end + 60);
*/	
	

	LCD_WriteData_16Bit(y_start);

	LCD_WriteData_16Bit(y_end);	
	
	
	Lcd_WriteIndex(0x2c);

}





	
/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	
#if 0	
	
	Lcd_SetRegion(x,y,x+1,y+1);

//#if 0	
//	LCD_WriteData_16Bit(Data);
//#else	
//	LCD_DC_SET;
//	SPI_Write16bitData(Data);
//#endif

#endif
	

	Lcd_SetRegion(x,y,x,y);//Note: not "(x,y,x+1,y+1)"
	
/*
	
LCD_SetRegion(x1, y1, x2, y2)
参数含义：矩形左上角 (x1,y1)，右下角 (x2,y2)
单个像素：左上角 = 右下角，所以 LCD_SetRegion(x, y, x, y) 完全正确。不需要 x+1、y+1！


部分屏驱动：区间是 [start, end) 左闭右开（极少）
这种场景下，要绘制 x 这一列，需要写到 x+1
LCD_SetRegion(x, y, x+1, y+1)
绝大多数 ST7789/ST7735/RGB MCU 屏都是闭区间 [x1,y1 ~ x2,y2]，包含两端

*/	
	
	LCD_WriteData_16Bit(Data);
	
}    






/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无

uint16_t blue_color = BLACK;//C_TOMATO;//C_BLACK;//C_BLUE;
lcd_dma_refresh_colorblock(0, 0, X_MAX_PIXEL, Y_MAX_PIXEL,&blue_color);

*************************************************/
void Lcd_Clear(uint16_t Color)               
{	
	
#if 0
	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    {	
	  	LCD_WriteData_16Bit(Color);
    } 

#else
	
	uint32_t len = X_MAX_PIXEL*Y_MAX_PIXEL;
	
	Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
	
	HW_SPI_Tx_DMA_16bit_ColorBlock(&Color,len);
	
#endif 
	
}








/*************************************************
函数名：Lcd_Fill
功能：指定区域填充颜色
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Fill(uint16_t x,uint16_t y,uint16_t xend,uint16_t yend,uint16_t Color)               
{	
	//unsigned int i;
	uint16_t num = (xend-x+1)*(yend-y+1);
	
	Lcd_SetRegion(x,y,xend,yend);
	#if 0
	for(i=0; i<num; i++)
		LCD_WriteData_16Bit(Color);
	#else
	HW_SPI_Tx_DMA_16bit_ColorBlock(&Color,num);
	#endif	
}













/*******************************************GUI************************************************************/


/*******************************************GUI************************************************************/


/*******************************************GUI************************************************************/


/*******************************************GUI************************************************************/








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
				LCD_DrawPoint(x + j, y + i, GREEN);
//			else
//				LCD_DrawPoint(x + j, y + i, GRAY0);
		}
	}
}




#if 0

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
#endif




// ========== 优化 GuiShowNum ==========
void GuiShowNum(uint8_t x, uint8_t y, uint16_t num, uint8_t len)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    uint16_t divisor = 1;                    // 新增：预计算除数

    // 预计算 10^(len-1)，只算一次
    for (t = 0; t < len - 1; t++)
        divisor *= 10;                       // len=3 → divisor=100

    for (t = 0; t < len; t++)
    {
        temp = (num / divisor) % 10;         // 直接除，不再调 oled_pow
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                GuiShowChar_16(x + 8 * t, y, ' ');
                divisor /= 10;               // 新增：除数递减
                continue;
            }
            else
                enshow = 1;
        }
        GuiShowChar_16(x + 8 * t, y, temp + '0');
        divisor /= 10;                       // 新增：除数递减
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
				LCD_DrawPoint(x + j, y + i, GREEN);
			else
				LCD_DrawPoint(x + j, y + i, BLACK);
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
				LCD_DrawPoint(x + j, y + i, GREEN);
			else
				LCD_DrawPoint(x + j, y + i, BLACK);
		}
	}
}



//显示字符

//`n` 就是你要在屏幕上画出来的那个 0-9 之间的具体数字

void ShowNum_48(uint8_t n)
{
	unsigned char i, j;
	uint8_t row_buf[48];
	
	for (i = 0; i < 48; i++){
		for (j = 0; j < 24; j++){
			if (Font48_dital_Table[n * 144 + i*3 + j/8] & (0x80 >> (j%8))){
				row_buf[j*2] = (GREEN>>8)&0xFF;
				row_buf[j*2+1] = GREEN&0xFF;
			} else {
				row_buf[j*2] = 0;
				row_buf[j*2+1] = 0;
			}
		}
		Lcd_Write_data_dma(row_buf, 48);
	}
}







#if 0
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
#endif



// ========== 优化 GuiShowNumString_32 ==========
void GuiShowNumString_32(uint8_t x, uint8_t y, uint16_t num, uint8_t len)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    uint16_t divisor = 1;

    for (t = 0; t < len - 1; t++)
        divisor *= 10;

    for (t = 0; t < len; t++)
    {
        temp = (num / divisor) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                GuiShowNum_32(x + 16 * t, y, 0);
                divisor /= 10;
                continue;
            }
            else
                enshow = 1;
        }
        GuiShowNum_32(x + 16 * t, y, temp + 0);
        divisor /= 10;
    }
}




#if 0
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
#endif

// ========== 优化 GuiShowNumString_48 ==========
void GuiShowNumString_48(uint8_t x, uint8_t y, uint16_t num, uint8_t len)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    uint16_t divisor = 1;

    for (t = 0; t < len - 1; t++)
        divisor *= 10;

    for (t = 0; t < len; t++)
    {
        temp = (num / divisor) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                GuiShowNum_48(x + 24 * t, y, 0);
                divisor /= 10;
                continue;
            }
            else
                enshow = 1;
        }
        GuiShowNum_48(x + 24 * t, y, temp + 0);
        divisor /= 10;
    }
}





//Gui_Draw_Line(10,20, 200,20, BLUE);  // 水平线
//Gui_Draw_Line(50,10, 50,150, GREEN); // 垂直线
void Gui_Draw_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Color)
{
    uint16_t i;
    // 水平线 y固定
    if(y0 == y1)
    {
        uint16_t x_start = (x0 < x1) ? x0 : x1;
        uint16_t x_end   = (x0 > x1) ? x0 : x1;
        for(i = x_start; i <= x_end; i++)
        {
            LCD_DrawPoint(i, y0, Color);
        }
    }
    // 垂直线 x固定
    else if(x0 == x1)
    {
        uint16_t y_start = (y0 < y1) ? y0 : y1;
        uint16_t y_end   = (y0 > y1) ? y0 : y1;
        for(i = y_start; i <= y_end; i++)
        {
            LCD_DrawPoint(x0, i, Color);
        }
    }
    // 如需斜线，请使用上面完整Bresenham版本
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
            LCD_DrawPoint(x0, y0, Color);
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
            LCD_DrawPoint(x0, y0, Color);
        }
    }

    // 不支持斜线（你要求只画水平/垂直，这里可以空着）
    else
    {
        // 斜线直接不处理
    }
}









/*
X	uint16_t	圆心的 水平坐标（左上角为 (0,0)）
Y	uint16_t	圆心的 垂直坐标
R	uint16_t	圆的 半径（像素数）
fc	uint16_t	前景颜色 color

该函数不填充圆内部，只绘制圆的边缘。

*/

//Gui_Circle(100,100,30,RED);
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
        LCD_DrawPoint(X + a, Y + b, fc);
        LCD_DrawPoint(X - a, Y + b, fc);
        LCD_DrawPoint(X + a, Y - b, fc);
        LCD_DrawPoint(X - a, Y - b, fc);

        LCD_DrawPoint(X + b, Y + a, fc);
        LCD_DrawPoint(X - b, Y + a, fc);
        LCD_DrawPoint(X + b, Y - a, fc);
        LCD_DrawPoint(X - b, Y - a, fc);

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
	
	  #if 0
    if(ax == 0) ang_part = 0;        // 0°
    else if(ay > ax) ang_part = 45; // 0~45°
    else ang_part = 90;             // 45~90°
		#else
		if(ax == 0) ang_part = 90;        // 垂直轴边界 → 象限内 90°
		else if(ay == 0) ang_part = 0;    // 水平轴边界 → 象限内 0°
		else if(ay > ax) ang_part = 90;   // 偏垂直 → 45~90°
		else ang_part = 45;               // 偏水平 / 45° 对角线 → 0~45°
    #endif

    // 5. 直接计算角度（纯加法，极快）
    int angle;
    if(quad == 1) angle = ang_part;
    else if(quad == 2) angle = 180 - ang_part;
    else if(quad == 3) angle = 180 + ang_part;
    else angle = 360 - ang_part;

    // 6. 角度区间判断（不变）
		#if 0
    if( (angle >= sa && angle <= ea) ||
        (sa > ea && (angle >= sa || angle <= ea)) )
    {
        LCD_DrawPoint(cx + x, cy + y, color);
    }
		#else
		int span = ea - sa;               // 弧长，先算（sa 可能 >360）
		while(sa > 360) sa -= 360;        // 折叠起点到 [0,360]
		int d = angle - sa;
		if(d < 0) d += 360;
		if(d <= span)
		{
				LCD_DrawPoint(cx + x, cy + y, color);
		}		
		#endif
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
                            LCD_DrawPoint(x + j, y + i, fc);
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







// 坐标(30,30)，白色文字，内容：HELLO
//Gui_ShowString_Transparent(30, 30, WHITE, "HELLO");



// 先画一个实心圆
//Gui_FillCircle(64, 64, 30, BLUE);

// 在圆中心透明显示数字（不会擦掉圆）
//Gui_ShowString_Transparent(50, 55, WHITE, "100");





//Gui_ProgressBar(50,100, 150,20, 60, BLACK,GREEN,GRAY);

// 透明显示 60%，不破坏进度条
//Gui_ShowString_Transparent(70, 102, WHITE, "60%");











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
	
#if 1
    // 限制进度 0~100
    if(percent > 100) percent = 100;

    // 画背景
    //Gui_DrawFillRect(x0, y0, w-1, h-1, bgCol);
	  Lcd_Fill(x0, y0, x0 + w - 1, y0 + h - 1, bgCol);

    // 画进度
    uint16_t bar_w = (percent * (w-4)) / 100;
    if(bar_w > 0)
    {
        Gui_DrawFillRect(x0+2, y0+2, bar_w, h-5, barCol);
    }

    // 画边框
    Gui_DrawRect(x0, y0, w-1, h-1, frameCol);
	
#else
	


#endif	
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
*/


void Gui_RingProgress(uint16_t cx,uint16_t cy,uint16_t r_out,uint8_t ring_w,uint8_t per,uint16_t col_bg,uint16_t col_pro)
{
    uint16_t r_in;
    uint16_t span;

    if(per > 100) per = 100;
    r_in = r_out - ring_w;
    if(r_in < 1) r_in = 1;

    span = (uint16_t)(((uint32_t)per * 360) / 100);  // 0..360

    /* 1. 背景：未填充部分（当前终点 → 顺时针回到正上）*/
    Gui_DrawArc(cx,cy,r_out, 270 + span, 630, col_bg);
    Gui_DrawArc(cx,cy,r_in,  270 + span, 630, col_bg);
    for(uint16_t i = r_in + 1; i < r_out; i++)
        Gui_DrawArc(cx,cy,i, 270 + span, 630, col_bg);

    /* 2. 前景：已填充部分（正上 270° → 顺时针到当前终点），per=0 不画 */
    if(per > 0)
    {
        Gui_DrawArc(cx,cy,r_out, 270, 270 + span, col_pro);
        Gui_DrawArc(cx,cy,r_in,  270, 270 + span, col_pro);
        for(uint16_t i = r_in + 1; i < r_out; i++)
            Gui_DrawArc(cx,cy,i, 270, 270 + span, col_pro);
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











// ===================== 外部LCD基础绘图接口实现 =====================

// 清屏：填充整个屏幕
void LCD_ClearScreen(uint16_t bg)
{
    // 使用之前定义的快速填充矩形，填充全屏 (240x240)
    Gui_DrawFillRect(0, 0, X_MAX_PIXEL - 1, Y_MAX_PIXEL - 1, bg);
}

// 实心圆填充
void LCD_FillCircle(int16_t cx, int16_t cy, int16_t r, uint16_t color)
{
    Gui_FillCircle((uint16_t)cx, (uint16_t)cy, (uint16_t)r, color);
}

// 绘制椭圆轮廓（M0极致优化：采用标准的 Midpoint 整数算法，完全消除循环内乘除法）
void LCD_DrawEllipseOutline(int16_t cx, int16_t cy, int16_t rw, int16_t rh, uint16_t color, uint8_t width)
{
    for (uint8_t w = 0; w < width; w++)
    {
        int32_t cur_rw = rw - w;
        int32_t cur_rh = rh - w;
        if (cur_rw <= 0 || cur_rh <= 0) break;

        // 实际上，为了绝对性能，我们直接使用对称点步进
        // 为保证在 M0 上不出现乘法，我们改用最稳健的步进法
        for (int16_t x_i = 0; x_i <= cur_rw; x_i++) {
            // 利用对称性，仅计算 y。y 是单调递减的
            // 使用一个简单的 y 逼近法，避免每次循环除法
            static int16_t last_y; 
            if (x_i == 0) last_y = cur_rh;
            
            // 检查 (x/rw)^2 + (y/rh)^2 <= 1
            // 转化为: (x*rh)^2 + (y*rw)^2 <= (rw*rh)^2
            while (last_y > 0) {
                int32_t term1 = (int32_t)x_i * cur_rh;
                int32_t term2 = (int32_t)last_y * cur_rw;
                if ((term1 * term1 + term2 * term2) <= (cur_rw * cur_rh * cur_rw * cur_rh)) break;
                last_y--;
            }
            
            LCD_DrawPoint(cx + x_i, cy + last_y, color);
            LCD_DrawPoint(cx - x_i, cy + last_y, color);
            LCD_DrawPoint(cx + x_i, cy - last_y, color);
            LCD_DrawPoint(cx - x_i, cy - last_y, color);
        }
    }
}

// 实心椭圆填充（M0极致优化：Y轴递增 + X轴单调递减，消除除法和 sqrt）
void LCD_FillEllipse(int16_t cx, int16_t cy, int16_t rw, int16_t rh, uint16_t color)
{
    if (rw <= 0 || rh <= 0) return;

    int32_t rw2 = (int32_t)rw * rw;
    int32_t rh2 = (int32_t)rh * rh;
    int32_t x_val = rw; // y=0 时，x=rw

    for (int16_t current_y = 0; current_y <= rh; current_y++) {
        int32_t y2 = (int32_t)current_y * current_y;
        
        // 目标：x^2 = rw^2 * (rh^2 - y^2) / rh^2
        // 优化：由于 y 递增，x 必然单调递减。
        // 我们不需要每次计算 sqrt 或除法，只需要在当前 x_val 基础上递减，直到满足方程
        while (x_val > 0) {
            // 检查 (x_val^2 * rh2) <= rw2 * (rh2 - y2)
            int32_t left = (int32_t)x_val * x_val * rh2;
            int32_t right = rw2 * (rh2 - y2);
            if (left <= right) break;
            x_val--;
        }

        Gui_DrawLine_Fast(cx - x_val, cy + current_y, cx + x_val, cy + current_y, color);
        Gui_DrawLine_Fast(cx - x_val, cy - current_y, cx + x_val, cy - current_y, color);
    }
}

// 实心多边形填充 (M0优化版：减少重复计算，使用定点数思想简化)
void LCD_FillPolygon(int16_t *pts, uint16_t point_cnt, uint16_t fill)
{
    if (point_cnt < 3) return;

    int16_t minY = pts[1], maxY = pts[1];
    for (uint16_t i = 0; i < point_cnt; i++) {
        int16_t y = pts[i * 2 + 1];
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    }

    for (int16_t y = minY; y <= maxY; y++) {
        int16_t nodes[20];
        uint8_t node_cnt = 0;

        for (uint16_t i = 0; i < point_cnt; i++) {
            int16_t x1 = pts[i * 2];
            int16_t y1 = pts[i * 2 + 1];
            int16_t x2 = pts[((i + 1) % point_cnt) * 2];
            int16_t y2 = pts[((i + 1) % point_cnt) * 2 + 1];

            // 只有当 y 在边 y1, y2 之间时才计算交点
            if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) {
                // 优化：x = x1 + (y - y1) * (x2 - x1) / (y2 - y1)
                // 将除法放在最后，尽量使用 32 位中间变量防止溢出
                int32_t num = (int32_t)(y - y1) * (x2 - x1);
                int32_t den = y2 - y1;
                nodes[node_cnt++] = x1 + (int16_t)(num / den);
            }
        }

        // 简单的冒泡排序（node_cnt 很小，此时最快）
        for (uint8_t i = 0; i < node_cnt - 1; i++) {
            for (uint8_t j = i + 1; j < node_cnt; j++) {
                if (nodes[i] > nodes[j]) {
                    int16_t temp = nodes[i];
                    nodes[i] = nodes[j];
                    nodes[j] = temp;
                }
            }
        }

        for (uint8_t i = 0; i < node_cnt - 1; i += 2) {
            Gui_DrawLine_Fast(nodes[i], y, nodes[i + 1], y, fill);
        }
    }
}


