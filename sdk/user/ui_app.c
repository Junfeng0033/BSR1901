#include "lcd_driver.h"
#include "dma.h"
#include "string.h"
#include "stdbool.h"

#include "ui_app.h"

//多用局部刷新（只更新变化区域），不要每帧 LCD_ClearScreen


//static img_source_t bat_persent_img = {(uint8_t*)gImage_bat_90x49, 90, 49};


#if 0
static uint8_t ui_buf[1024];

	
//通过大的区域截取小区域
static void img_cut_out(img_source_t *source, uint16_t x, uint16_t y, uint16_t width, uint16_t high)
{
	uint16_t i, j;
	
	if(source == NULL)
		return ;

	for(i=0; i<high; i++){
		if((y+i) >= source->high)
			break;
		
		for(j=0; j<width; j++){
			if((x+j) >= source->width)
				break;
			
			ui_buf[(i * width + j)*2] = source->p_img[((y+j)*source->width+(x+j))*2];
			ui_buf[(i * width + j)*2 + 1] = source->p_img[((y+j)*source->width+(x+j))*2 + 1];		
		}
	}
}
#endif












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
	Lcd_SetRegion(20, 39, 109, 87);
	Lcd_Write_data_dma((uint8_t*)gImage_bat_90x49, 8820);
	
	//img_cut_out(&bat_persent_img, 0, 0, 81, 43);	


	
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
		



    //BSR1901_FireEye_Demo();

	
}


