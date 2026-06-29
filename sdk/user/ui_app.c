#include "lcd_driver.h"
#include "dma.h"
#include "string.h"
#include "stdbool.h"

#include "ui_app.h"


//void ShowNum_48(uint8_t n);

//static img_source_t bat_persent_img = {(uint8_t*)gImage_bat_90x49, 90, 49};


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






//显示百分比符号
void ShowPersent(void)
{
	unsigned char i, j;
	uint8_t row_buf[16];

	for (i = 0; i < 32; i++){
		for (j = 0; j < 8; j++){
			if (Font_8x32_percent[i] & (0x80 >> j)){
				row_buf[j*2] = (GREEN>>8)&0xFF;
				row_buf[j*2+1] = GREEN&0xFF;
			} else {
				row_buf[j*2] = 0;
				row_buf[j*2+1] = 0;
			}
		}
		Lcd_Write_data_dma(row_buf, 16);
	}
}





//绘制空电池
void ui_paint_bat(void)
{
	Lcd_SetRegion(20, 39, 109, 87);						//
	//Lcd_Write_data_dma((uint8_t*)gImage_bat_90x49, 8820);
}

//绘制电池容量
void ui_paint_bat_remain(uint8_t percent)
{
	//img_cut_out(&bat_persent_img, 0, 0, 81, 43);
	Lcd_SetRegion(32, 39, 109, 87);						//
	//Lcd_Write_data_dma(ui_buf, 81*43*2);
}






//电池百分比
void ui_paint_bat_percent(uint8_t percent)
{
	static uint8_t history = 0;
	
	if(percent >= 100){
		Lcd_SetRegion(22, 35, 22+23, 35+47);
		ShowNum_48(1);

		Lcd_SetRegion(46, 35, 46+23, 35+47);
		ShowNum_48(0);

		Lcd_SetRegion(70, 35, 70+23, 35+47);
		ShowNum_48(0);	

	}
	else{
		if(history == 100){
			Lcd_SetRegion(22, 35, 70+23, 35+47);
			//Lcd_Write_data_dma((uint8_t*)gImage_black_128x128, 72*47*2);
			uint16_t color = BLACK;
			HW_SPI_Tx_DMA_16bit_ColorBlock(&color,72*47);

		}
		
		if(percent%10 == 0){
			Lcd_SetRegion(32, 35, 32+23, 35+47);
			ShowNum_48(percent/10);

			Lcd_SetRegion(80, 40, 80+7, 40+31);
			ShowPersent();

		}
		
		Lcd_SetRegion(56, 35, 56+23, 35+47);
		ShowNum_48(percent%10);

	}
	
	history = percent;
}




//绘制彩色圆形
void ui_paint_color_circle(void)
{
	Lcd_SetRegion(10, 10, 109, 109);						//坐标设置
	Lcd_Write_data_dma((uint8_t*)gImage_circle_100x100, 20000);
}

//绘制圆圈百分比

//绘制充电功率

//绘制剩余时间

//绘制空沙漏

//绘制沙漏剩余电量百分比

//绘制充电功率





void set_bat_percentage(uint8_t percent)
{
	static uint8_t history = 0;
	
	if(percent > 100)
		return ;
	
	if(history == percent)
		return ;
	
	//Lcd_Fill(22, 41, 22+80.0/100*percent, 84, 0x051d);
	Lcd_Fill(22, 41, 22+(80*percent)/100, 84, 0x051d);
	//Lcd_Fill(22+80.0/100*percent+1, 41, 102, 84, BLACK);
	Lcd_Fill(22+(80*percent)/100+1, 41, 102, 84, BLACK);

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
		//Gui_RingProgress(80,80,35,6,65,GRAY1,GREEN);
		
		
		Gui_DrawArc(120, 120, 50, 135, 405, 0xFFFF);

    //BSR1901_FireEye_Demo();


		
//		if(uicount%10 == 1){
//			if(uicount <= 10)
//				HW_SPI_Tx_DMA((uint16*)gImage_charge_10, 20000);
//			if(uicount <= 20)
//				HW_SPI_Tx_DMA((uint16*)gImage_charge_20, 20000);
//			if(uicount <= 30)
//				HW_SPI_Tx_DMA((uint16*)gImage_charge_30, 20000);
//		}
		

		
//		GuiShowNumString_16(50, 18, uicount, 2);
//		GuiShowNumString_48(32, 35, uicount, 2);
//		GuiShowPersent(45+40, 45);
//		GuiShowNumString_16(50, 85, uicount, 2);

	
}


