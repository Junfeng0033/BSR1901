
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  生成日期   : 2018-1111
//  最近修改   : 
//  功能描述   : 0.96寸ISP LCD 4接口演示例程
/******************************************************************************
//本程序适用与STM32F103C8
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PA5（SCL）
//              SDA   接PA7（SDA）
//              RES   接PB0
//              DC    接PB1
//              CS    接PA4//可以直接接地
//              BLK    接P10 BLK可以悬空不接；在需要关闭背光的时候将BLK置低电平
*******************************************************************************/
// 修改历史   :
// 日    期   : 
// 修改内容   : 
//******************************************************************************/

#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

#include "stdint.h"

#define X_MAX_PIXEL		128
#define Y_MAX_PIXEL		128




#define USE_HORIZONTAL 1  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
//#define LCD_W 80
//#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 80
#endif

#define RED  			0xf800
#define GREEN			0x07e0
#define BLUE 			0x001f
#define WHITE			0xffff
#define BLACK			0x0000
#define YELLOW  		0xFFE0
#define GRAY0   		0xEF7D   	//灰色0 3165 00110 001011 00101
#define GRAY1   		0x8410      //灰色1      00000 000000 00000
#define GRAY2   		0x4208      //灰色2  1111111111011111




#define RED32  			0xf800f800
#define GREEN32			0x07e007e0
#define BLUE32 			0x001f001f
#define WHITE32			0xffffffff
#define BLACK32			0x00000000





// backlight control
#define LCD_BL_PORT  		GPIOB			//MCU_PB2--->>TFT --LCD_LEDK
#define LCD_BL_PIN       	2  	//MCU_PB2--->>TFT --BL

#define LCD_RS_PORT			GPIOB
#define LCD_RS_PIN         	1		//PB1--->>TFT --RS/DC

#define LCD_RST_PORT		GPIOB
#define LCD_RST_PIN     	0		//PB0--->>TFT --RST

//液晶控制口置1操作语句宏定义

#define	LCD_RS_SET  	gpio_set_value(LCD_RS_PORT, 1, LCD_RS_PIN)
#define	LCD_BL_SET  	gpio_set_value(LCD_BL_PORT, 1, LCD_BL_PIN)
#define	LCD_RST_SET  	gpio_set_value(LCD_RST_PORT, 1, LCD_RST_PIN)


//液晶控制口置0操作语句宏定义

#define	LCD_RS_CLR  	gpio_set_value(LCD_RS_PORT, 0, LCD_RS_PIN)
#define	LCD_BL_CLR  	gpio_set_value(LCD_BL_PORT, 0, LCD_BL_PIN)
#define	LCD_RST_CLR  	gpio_set_value(LCD_RST_PORT, 0, LCD_RST_PIN)



#define IP2366_INT_PORT		GPIOA
#define IP2366_INT_PIN     	6		//PA6--->>INT_IP2366

#define	IP2366_INT_SET  	gpio_set_value(IP2366_INT_PORT, 1, IP2366_INT_PIN)
#define	IP2366_INT_CLR  	gpio_set_value(IP2366_INT_PORT, 0, IP2366_INT_PIN)


#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
}

void LCD_GPIO_Init(void);
void Lcd_WriteIndex(uint8_t Index);
void Lcd_WriteData(uint8_t Data);
void Lcd_WriteReg(uint8_t Index,uint8_t Data);
uint16_t Lcd_ReadReg(uint8_t LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);

//void RefreshColorBlockDynamic(uint32_t Color);
	
void Lcd_SetXY(uint16_t x,uint16_t y);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y);
void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end);
void LCD_WriteData_16Bit(uint16_t Data);
void Lcd_Fill(uint16_t x,uint16_t y,uint16_t xend,uint16_t yend,uint16_t Color);
//向lcd写入数据通过dma的方式
void Lcd_Write_data_dma(uint8_t *p_data, uint16_t len);

#endif
