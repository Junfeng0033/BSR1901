
SCGUI porting guide


1,修改显示屏分辨率


..\BSR1901\software\sdk\SCGUI\sc_common.h

#define BSR1901_MCU 1

#define PY32_MCU 0

#if BSR1901_MCU

#define SC_SCREEN_WIDTH (240)
#define SC_SCREEN_HEIGHT (240)
#define SC_PFB_BUF_SIZE (SC_SCREEN_WIDTH * 2) // 示例：2行高度
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


sc_gui_init(lcd_dma_8bit_refresh, 0, C_ROYAL_BLUE,C_BLUE, &lv_font_16);



在BSR1901上是这样写的:
void lcd_dma_8bit_refresh(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color)
{
	uint32_t len = w*h;
	
	Lcd_SetRegion(xs, ys, xs+w-1, ys+h-1);	
	
	HW_SPI_Tx_DMA_8bit(HAL_SPI_0,color,len);
	
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




