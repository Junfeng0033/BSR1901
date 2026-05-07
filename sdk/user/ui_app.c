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
			Lcd_Write_data_dma((uint8_t*)gImage_black_128x128, 72*47*2);

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
