
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//
//  文 件 名   : lcd_driver.h
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
//              BLK   接P10 BLK可以悬空不接；在需要关闭背光的时候将BLK置低电平




//本程序适配 BSR1901 推荐硬件端口
//              GND   电源地
//              VCC   接3.3v电源--LDO33_AUX_OUT

//              SCL   接SPI_CLK--------(PAD22)
//              SDA   接SPI_MOSI-------(PAD23)

//              RES   接PB7------------(PAD07)
//              DC    接PA3------------(PAD21)

//              CS    接SPI_CSN_1------(PAD20)//有些显示屏可以直接接地

//              BLK   接PB4(PWM4)------(PAD24)

*******************************************************************************/
// 修改历史   :
// 日    期   : 
// 修改内容   : 
//******************************************************************************/

#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

#include "stdint.h"

//#define X_MAX_PIXEL		128
//#define Y_MAX_PIXEL		128

//#define USE_HORIZONTAL 1  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏



#define USE_HORIZONTAL   0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#define X_MAX_PIXEL 240
#define Y_MAX_PIXEL 240




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
#define YELLOW  	0xFFE0
#define GRAY0   	0xEF7D
#define GRAY1   	0x8410
#define GRAY2   	0x4208





//==================================================================================================

// backlight control
#define LCD_BL_PORT  		GPIOB			//MCU_PB4--->>TFT --LCD_LEDK
#define LCD_BL_PIN       	  4  	  //MCU_PB4--->>TFT --BL

#define LCD_DC_PORT			GPIOA
#define LCD_DC_PIN         	3		  //PA3--->>TFT --RS/DC

#define LCD_RST_PORT		GPIOB
#define LCD_RST_PIN     	  7		  //PB7--->>TFT --RST

//液晶控制口置1操作语句宏定义

#define	LCD_DC_SET  	gpio_set_value(LCD_DC_PORT, 1, LCD_DC_PIN)
//#define LCD_DC_SET   {GpiopinMode(PB1,OUTPUT);GpiopinWrite(PB1,HIGH);}
#define	LCD_BL_SET  	gpio_set_value(LCD_BL_PORT, 1, LCD_BL_PIN)
#define	LCD_RST_SET  	gpio_set_value(LCD_RST_PORT, 1, LCD_RST_PIN)


//液晶控制口置0操作语句宏定义

#define	LCD_DC_CLR  	gpio_set_value(LCD_DC_PORT, 0, LCD_DC_PIN)
//#define LCD_DC_CLR   {GpiopinMode(PB1,OUTPUT);GpiopinWrite(PB1,LOW);}
#define	LCD_BL_CLR  	gpio_set_value(LCD_BL_PORT, 0, LCD_BL_PIN)
#define	LCD_RST_CLR  	gpio_set_value(LCD_RST_PORT, 0, LCD_RST_PIN)

//==================================================================================================


#define IP2366_INT_PORT		GPIOA
#define IP2366_INT_PIN    6			//PA6--->>INT_IP2366

#define	IP2366_INT_SET  	gpio_set_value(IP2366_INT_PORT, 1, IP2366_INT_PIN)
#define	IP2366_INT_CLR  	gpio_set_value(IP2366_INT_PORT, 0, IP2366_INT_PIN)


//#define P_I2C_INT_CLR   {GpiopinMode(PA6,OUTPUT);GpiopinWrite(PA6,LOW);}
//#define P_I2C_INT_SET   {GpiopinMode(PA6,OUTPUT);GpiopinWrite(PA6,HIGH);}



//hwp_spi0->CTROL = 0x10f8b;						//8bit spi data
//#define 	SPI_WriteData(data) 					{hwp_spi0->CTROL = 0x10f8b;hwp_spi0->FIFODATA = data;}



#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
}




#define 	LCD_WR_REG										Lcd_WriteIndex //写入命令
#define 	LCD_WR_DATA8									Lcd_WriteData	 //写入数据


#define 	LCD_NV3022_CMD								Lcd_WriteIndex
#define 	LCD_NV3022_Parameter					Lcd_WriteData

#define 	LCD_NV3023_CMD								Lcd_WriteIndex
#define 	LCD_NV3023_Parameter					Lcd_WriteData


enum
{
  LCD_DIRECTION_NORMAL,
  LCD_DIRECTION_90,
  LCD_DIRECTION_180,
  LCD_DIRECTION_270,
};

typedef uint8_t app_lcd_direction_t;



typedef struct
{
  uint16_t x_max;
  uint16_t y_max;
  uint16_t x_offset;
  uint16_t y_offset;
  app_lcd_direction_t dir;
}app_lcd_t;


extern app_lcd_t lcd;



void LCD_GPIO_Init(void);
void Lcd_WriteIndex(uint8_t Index);
void Lcd_WriteData(uint8_t Data);


//void Lcd_WriteReg(uint8_t Index,uint8_t Data);


void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);


	

void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);

void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end);


void LCD_WriteData_16Bit(uint16_t Data);
void Lcd_Fill(uint16_t x,uint16_t y,uint16_t xend,uint16_t yend,uint16_t Color);



#endif
