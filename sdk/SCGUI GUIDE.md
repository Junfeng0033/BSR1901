
SCGUI porting guide


/*

Simplified Chinese(GB 2312) Encoding



按 Ctrl + Shift + P 打开命令面板。
输入 "Change File Encoding"。
选择 "Reopen with Encoding"。
从列表中选择 "Simplified Chinese(GB 2312)" 。


*/





//单屏（非双屏） BSR1901 推荐硬件端口
//本程序适配 BSR1901 推荐硬件端口
//              GND   电源地
//              VCC   接3.3v电源--LDO33_AUX_OUT
//              SCL   接SPI_CLK--------(PAD22)
//              SDA   接SPI_MOSI-------(PAD23)
//              RES   接PB7------------(PAD07)
//              DC    接PA3(MISO)------(PAD21)
//              CS    接SPI_CSN_1------(PAD20)//有些显示屏可以直接接地
//              BLK   接PB4(PWM4/CSN2)-(PAD24)



1,修改代码分辨率，适配客户显示屏分辨率


..\BSR1901\software\sdk\SCGUI\sc_common.h

#define BSR1901_SoC 1 //1=启用

#define PY32_MCU 0

#if BSR1901_SoC

#define SC_SCREEN_WIDTH (240)
#define SC_SCREEN_HEIGHT (240)
#define SC_PFB_BUF_SIZE (SC_SCREEN_WIDTH * 5) // 示例：5行高度，帧缓冲仅缓存5行数据，降低SRAM占用
#define SC_LCD_DMA_2BUF (0)                   // 是否启用DMA双buf传输
#define SC_LCD_DMA_WAP (0)                    // 是否DMA传输时高低位WAP


2,修改队列和线程的数量，减少SRAM占用

..\BSR1901\software\sdk\SCGUI\sc_event_task.h


//=======================新增多类型队列===================================
#define MAX_THREADS 4         // 任务数量
#define EVENT_QUEUE_SIZE 4    // 队列数量
#define MAX_EVENT_DATA_SIZE 8 // 队列数据长度，用于传字符串



3,增加DMA刷新LCD的回调函数


..\BSR1901\software\sdk\SCGUI\sc_gui.c


在sc_gui_init函数里的第一个参数就是DMA刷新LCD的回调函数，这里需要平台自行适配


sc_gui_init(lcd_dma_16bit_refresh, 0, C_ROYAL_BLUE,C_BLUE, &lv_font_16);



在BSR1901上是这样写的:
void lcd_dma_16bit_refresh(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color)
{
	uint32_t len = w*h;
	
	Lcd_SetRegion(xs, ys, xs+w-1, ys+h-1);	
	
	HW_SPI_Tx_DMA_16bit(HAL_SPI_0,color,len);
	
}




/* 初始化系统 */
void sc_gui_init(lcd_refresh_cb refresh_cb, color_t bkc, color_t fc, color_t bc, void *font)
{
    static sc_gui_t tft;
    gui = &tft;
    gui->lcd_rect.x = 0;
    gui->lcd_rect.y = 0;
    gui->lcd_rect.w = SC_SCREEN_WIDTH;
    gui->lcd_rect.h = SC_SCREEN_HEIGHT;
    gui->g_mask = NULL;
    gui->refresh_cb = refresh_cb;
    gui->font = font;
    gui->bkc = bkc;
    gui->fc = fc;
    gui->bc = bc;
    gui->alpha = 255;
}


4,增加Cortex-M CPU的SysTick，适配SCGUI的“system_tick”全局变量


..\BSR1901\software\sdk\SCGUI\sc_event_task.c



//======================新加多类型事件队列==========================
uint32_t system_tick = 0;  // 全局tick计数器，由中断更新
uint32_t sc_tick = 0;      // 中间tick变量，在主循环中使用



..\BSR1901\software\sdk\peripheral\isr.c


__RAM_CODE__ void SysTick_Handler(void)  //interrupt routine
{

	TimeTick++;//TimeTick will increase by "1"  every 1000us
	
	//system_tick=TimeTick;
	
}


5,在做清除屏幕时，DMA刷新“色块”更快



===================================================================================================	

  uint16_t blue_color = BLACK;//C_TOMATO;//C_BLACK;//C_BLUE;
  lcd_dma_refresh_colorblock(0, 0, X_MAX_PIXEL, Y_MAX_PIXEL,&blue_color);
	
===================================================================================================	

void lcd_dma_refresh_colorblock(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color)
{
	uint32_t len = w*h;
	
	Lcd_SetRegion(xs, ys, xs+w-1, ys+h-1);	
	
	HW_SPI_Tx_DMA_16bit_ColorBlock(HAL_SPI_0,color,len);
	
}
===================================================================================================	




/************************************************************************************

When testing LCD color blocks, call "HW_SPI_Tx_DMA_16bit_ColorBlock" function

*************************************************************************************/

void HW_SPI_Tx_DMA_16bit_ColorBlock(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)
{
	LCD_RS_SET;	
    SPI_16bit_Transfer();
	//DMA_Req_Buslock_Enable();
	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
						AHB_DMA_CONTROL_HWORD_TR, AHB_DMA_CONTROL_SRC_NOINC_DES_NOINC);	
	dma_sram_delay(1000);	
	
}




6,Cortex-M0 运算能力弱、数学计算慢 的解决方案



Cortex-M0 没有硬件乘法器、没有除法指令、没有浮点运算，
稍微复杂一点的 GUI 绘制（旋转、缩放、Alpha 混合、抗锯齿、圆弧）都会巨卡、巨占 CPU。

对于必须的，不可避免的，关键的乘除法运算函数，请在函数名前冠以“__RAM_CODE__”，以提升运算能力。





6.1. 禁用所有运行时数学计算（最关键）
SCGUI 里这些功能 M0 绝对不能开：
 禁止 旋转
 禁止 缩放
 禁止 抗锯齿
 禁止 Alpha 混合
 禁止 渐变圆弧
 禁止 三角函数
这些功能 M0 跑起来会慢10~50 倍。

只保留：
 纯色填充
 单色 / 简单图片
 文字显示
 脏矩形刷新
 DMA 行缓冲
 CPU 占用直接下降 80%
 
 


  
6.2. 全部使用“预渲染素材”，MCU不做任何绘图计算

这是 M0 跑 GUI 最核心的黑科技。
做法：
所有复杂界面（圆弧、圆角、进度条、波浪、图标）
不在 MCU 上画，而是在PC上生成好图片，直接放进Flash
MCU 只做一件事：
DMA 直接搬运图片到屏幕
效果：
圆弧 → 预渲染图片
进度条 → 预渲染图片
圆角 → 预渲染图片
渐变 → 预渲染图片
动画 → 预渲染序列帧

M0不需要任何计算，刷屏速度飞起来






6.3. 界面尽量用 “色块 + 文字”，少用复杂形状
M0 最强的是：
画矩形
显示文字
显示图片

所以你的产品界面要这样设计：
进度条 → 矩形填充
背景 → 纯色
图标 → 预渲染图片
按钮 → 纯色框
避免任何圆形、弧形、斜线、渐变






6.4. 全部使用“行缓冲 + DMA”，不让CPU 处理像素

强化方案：
所有刷屏 全部交给 DMA
CPU 只发指令，不处理数据
禁止 CPU 操作显存
DMA搬运不占用 CPU，M0可以睡觉等刷屏完成



7, SCGUI 在LCD上显示文字


简单文本显示 `sc_draw_Text()`

void sc_draw_Text(sc_pfb_t *dest, int tx, int ty, lv_font_t *font, const char *text, color_t tc, color_t bc);


参数说明：

- `dest`: PFB目标缓冲区（NULL表示直接绘制到屏幕）
- `tx`, `ty`: 文本起始坐标（左上角）
- `font`: 字体指针（如`gui->font`）
- `text`: 要显示的字符串
- `tc`: 文字颜色（如C_RED, C_BLUE等）
- `bc`: 背景颜色（通常使用`gui->bkc`）


使用示例：

// 在坐标(10,10)处显示"Hello World"，红色文字，默认背景
sc_draw_Text(NULL, 10, 10, gui->font, "Hello World", C_RED, gui->bkc);

// 显示变量内容
char str[20];
sprintf(str, "温度: %d°C", temperature);
sc_draw_Text(NULL, 50, 50, gui->font, str, C_WHITE, gui->bkc);



8, SCGUI 在LCD上显示数字

数字显示 - `sc_draw_Num()`

void sc_draw_Num(sc_pfb_t *dest, int tx, int ty, int w, int h, lv_font_t *font, int num, int den, color_t tc, color_t bc);


参数说明：

- `w`, `h`: 显示区域的宽高
- `num`: 分子
- `den`: 分母（用于显示小数，如100表示保留2位小数）


使用示例：

// 显示整数123
sc_draw_Num(NULL, 100, 30, 40, 20, gui->font, 123, 1, C_BLUE, C_RED);

// 显示小数12.34（分子1234，分母100）
sc_draw_Num(NULL, 100, 60, 60, 20, gui->font, 1234, 100, C_GREEN, gui->bkc);




9, SCGUI 在LCD上绘制矩形


实心矩形 - `sc_draw_Fill()`


void sc_draw_Fill(sc_pfb_t *dest, int xs, int ys, int w, int h, color_t color, uint16_t alpha);


- `dest`: 目标pfb（帧缓冲区），可为NULL表示直接绘制到屏幕

- `xs, ys`: 矩形左上角坐标

- `w, h`: 矩形的宽度和高度

- `color`: 填充颜色

- `alpha`: 透明度（0-255）



使用示例：

// 绘制红色实心矩形
sc_draw_Fill(NULL, 50, 50, 100, 80, C_RED, 255);




空心矩形 - `sc_draw_Frame()`


void sc_draw_Frame(sc_pfb_t *dest, int xs, int ys, int w, int h, int lw, color_t color, uint16_t alpha);


- `dest`: 目标pfb（帧缓冲区），可为NULL表示直接绘制到屏幕

- `xs, ys`: 矩形左上角坐标

- `w, h`: 矩形的宽度和高度

- `lw`: 边框线宽

- `color`: 填充颜色

- `alpha`: 透明度（0-255）


使用示例：

// 绘制蓝色边框矩形（线宽2）
sc_draw_Frame(NULL, 50, 50, 100, 80, 2, C_BLUE, 255);





10, 在LCD上绘制圆弧


..\BSR1901\software\sdk\user\GUI.c
..\BSR1901\software\sdk\user\ui.c



void Gui_Circle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc)

void ui_paint_bat_percent(uint8_t percent)


void Gui_ProgressBar(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,uint8_t percent, uint16_t frameCol, uint16_t barCol, uint16_t bgCol)




