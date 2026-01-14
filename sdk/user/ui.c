#include "lcd_driver.h"
#include "dma.h"
#include "string.h"
#include "stdbool.h"

typedef struct source{
	uint8_t *p_img;
	uint16_t width;
	uint16_t high;
}img_source_t;

extern const uint8_t Font_8x32_percent[];
extern const uint8_t gImage_black_128x128[32768];
extern const uint8_t Font48_dital_Table [];
extern const uint8_t gImage_bat_90x49[];
extern const unsigned char gImage_circle_100x100[20000];
static uint8_t ui_buf[2560];
static img_source_t bat_persent_img = {(uint8_t*)gImage_bat_90x49, 90, 49};
static volatile bool dma_busy = 0;

//dma发送结束后会调用此函数通知
__RAM_CODE__ void ui_dma_busy_release(void)
{
	dma_busy = 0;
}
	
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

//显示字符
void ShowNum_48(uint8_t *buf, uint8_t n)
{
	unsigned char i, j;
	
	memset(ui_buf, 0, 48*48);

	for (i = 0; i < 48; i++){
		for (j = 0; j < 24; j++){
			if (Font48_dital_Table[n * 144 + i*3 + j/8] & (0x80 >> (j%8))){
				buf[i*48+j*2] = (GREEN>>8)&0xFF;
				buf[i*48+j*2+1] = GREEN&0xFF;
			}
		}
	}
}

//显示百分比符号
void ShowPersent(uint8_t *buf)
{
	unsigned char i, j;

	memset(ui_buf, 0, 48*48);
	
	for (i = 0; i < 32; i++){
		for (j = 0; j < 8; j++){
			if (Font_8x32_percent[i] & (0x80 >> j)){
				buf[i*16+j*2] = (GREEN>>8)&0xFF;
				buf[i*16+j*2+1] = GREEN&0xFF;
			}
		}
	}
}

//绘制空电池
void ui_paint_bat(void)
{
	Lcd_SetRegion(20, 39, 109, 87);						//坐标设置
	Lcd_Write_data_dma((uint8_t*)gImage_bat_90x49, 8820);
}

//绘制电池容量
void ui_paint_bat_remain(uint8_t percent)
{
	img_cut_out(&bat_persent_img, 0, 0, 81, 43);
	Lcd_SetRegion(32, 39, 109, 87);						//坐标设置
	Lcd_Write_data_dma(ui_buf, 81*43*2);
}


__RAM_CODE__ static void wait_dma(void)
{
	dma_busy = 1;
	while(dma_busy);	
}


//电池百分比
void ui_paint_bat_percent(uint8_t percent)
{
	static uint8_t history = 0;
	
	if(percent >= 100){
		Lcd_SetRegion(22, 35, 22+23, 35+47);
		ShowNum_48(ui_buf, 1);
		Lcd_Write_data_dma(ui_buf, 48*48);
		wait_dma();

		Lcd_SetRegion(46, 35, 46+23, 35+47);
		ShowNum_48(ui_buf, 0);
		Lcd_Write_data_dma(ui_buf, 48*48);
		wait_dma();
		
		Lcd_SetRegion(70, 35, 70+23, 35+47);
		ShowNum_48(ui_buf, 0);
		Lcd_Write_data_dma(ui_buf, 48*48);	
		wait_dma();
	}
	else{
		if(history == 100){
			Lcd_SetRegion(22, 35, 70+23, 35+47);
			Lcd_Write_data_dma((uint8_t*)gImage_black_128x128, 72*47*2);
			wait_dma();
		}
		
		if(percent%10 == 0){
			Lcd_SetRegion(32, 35, 32+23, 35+47);
			ShowNum_48(ui_buf, percent/10);
			Lcd_Write_data_dma(ui_buf, 48*48);
			wait_dma();
			
			Lcd_SetRegion(80, 40, 80+7, 40+31);
			ShowPersent(ui_buf);
			Lcd_Write_data_dma(ui_buf, 8*32*2);
			wait_dma();
		}
		
		Lcd_SetRegion(56, 35, 56+23, 35+47);
		ShowNum_48(ui_buf, percent%10);
		Lcd_Write_data_dma(ui_buf, 48*48);
		wait_dma();
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

