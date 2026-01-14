#include "lcd_driver.h"
#include "stdint.h"
#include "gpio.h"
#include "spi.h"
#include "iomux.h"
#include "dma.h"


//游程编码（Run-Length Encoding, RLE）压缩算法

//https://gitee.com/li_yucheng/scgui



//hwp_spi0->CTROL = 0x10f8b;	//8bit spi data
#define 	SPI_WriteData(data) 				{hwp_spi0->CTROL = 0x10f8b;hwp_spi0->FIFODATA = data;}

#define 	LCD_NV3022_CMD						Lcd_WriteIndex
#define 	LCD_NV3022_Parameter				Lcd_WriteData

#define 	LCD_NV3023_CMD						Lcd_WriteIndex
#define 	LCD_NV3023_Parameter				Lcd_WriteData

//液晶IO初始化配置
void LCD_GPIO_Init(void)
{
	//gecko_pinmux_config(PAD18,GPIO_B_6);
	gecko_pinmux_config(PAD14,GPIO_B_2);
	gecko_pinmux_config(PAD13,GPIO_B_1);
	gecko_pinmux_config(PAD12,GPIO_B_0);
	
	//gpio_set_output(LCD_CS_PORT, LCD_CS_PIN);
	gpio_set_output(LCD_BL_PORT, LCD_BL_PIN);
	gpio_set_output(LCD_RS_PORT, LCD_RS_PIN);
	gpio_set_output(LCD_RST_PORT, LCD_RST_PIN);
}

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

//向lcd写入数据通过dma的方式
void Lcd_Write_data_dma(uint8_t *p_data, uint16_t len)
{
	LCD_RS_SET;

	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), p_data, len, 
						AHB_DMA_CONTROL_BYTE_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);
}

//向液晶屏写一个8位指令
void Lcd_WriteIndex(uint8_t Index)
{
	//SPI 写命令时序开始
	LCD_RS_CLR;
	SPI_WriteData(Index);
}

//向液晶屏写一个8位数据
void Lcd_WriteData(uint8_t Data)
{
   LCD_RS_SET;
   SPI_WriteData(Data); 
}

//向液晶屏写一个16位数据
void LCD_WriteData_16Bit(uint16_t Data)
{
	LCD_RS_SET;
	SPI_WriteData(Data>>8); 	//写入高8位数据
	SPI_WriteData(Data); 			//写入低8位数据
}

void Lcd_WriteReg(uint8_t Index,uint8_t Data)
{
	Lcd_WriteIndex(Index);
	Lcd_WriteData(Data);
}

void Lcd_Reset(void)
{
	LCD_RST_SET;
	delay_ms(100);
	LCD_RST_CLR;
	delay_ms(500);
	LCD_RST_SET;
	delay_ms(50);
}

//LCD Init For 1.47Inch LCD Panel with NV3022B.
void Lcd_Init(void)
{	
	LCD_GPIO_Init();

	Lcd_Reset(); //Reset before LCD Init.

#if 0//NV3022

#else//NV3023
	//----------------RESET LCD Driver ------------//
	delay_ms(120);
	//----------------Star Initial Sequence-------//
	LCD_NV3023_CMD(0xff);
	LCD_NV3023_Parameter(0xa5); //
	LCD_NV3023_CMD(0x3E);
	LCD_NV3023_Parameter(0x08);
	LCD_NV3023_CMD(0x3A);
	LCD_NV3023_Parameter(0x65);
	LCD_NV3023_CMD(0x82);
	LCD_NV3023_Parameter(0x00);
	LCD_NV3023_CMD(0x98);
	LCD_NV3023_Parameter(0x00);
	LCD_NV3023_CMD(0x63);
	LCD_NV3023_Parameter(0x0f);
	LCD_NV3023_CMD(0x64);
	LCD_NV3023_Parameter(0x0f);
	LCD_NV3023_CMD(0xB4);
	LCD_NV3023_Parameter(0x34);
	LCD_NV3023_CMD(0xB5);
	LCD_NV3023_Parameter(0x30);
	LCD_NV3023_CMD(0x83);
	LCD_NV3023_Parameter(0x03);
	LCD_NV3023_CMD(0x86);//
	LCD_NV3023_Parameter(0x04);
	LCD_NV3023_CMD(0x87);
	LCD_NV3023_Parameter(0x16);
	LCD_NV3023_CMD(0x88);//VCOM
	LCD_NV3023_Parameter(0x28);
	LCD_NV3023_CMD(0x89);//
	LCD_NV3023_Parameter(0x2F);//2 F
	LCD_NV3023_CMD(0x93); //
	LCD_NV3023_Parameter(0x63);
	LCD_NV3023_CMD(0x96);
	LCD_NV3023_Parameter(0x81);
	LCD_NV3023_CMD(0xC3);
	LCD_NV3023_Parameter(0x11);
	LCD_NV3023_CMD(0xE6);
	LCD_NV3023_Parameter(0x00);
	LCD_NV3023_CMD(0x99);
	LCD_NV3023_Parameter(0x01);

	LCD_NV3023_CMD(0x44);
	LCD_NV3023_Parameter(0x00);

	////////////////////////gamma_set//////////////////////////////////////
	LCD_NV3023_CMD(0x70);LCD_NV3023_Parameter(0x02);//VRP 0 1
	LCD_NV3023_CMD(0x71);LCD_NV3023_Parameter(0x0E);//VRP 1 3
	LCD_NV3023_CMD(0x72);LCD_NV3023_Parameter(0x0a);//VRP 2 7
	LCD_NV3023_CMD(0x73);LCD_NV3023_Parameter(0x12);//VRP 3 9
	LCD_NV3023_CMD(0x74);LCD_NV3023_Parameter(0x19);//VRP 6 11
	LCD_NV3023_CMD(0x75);LCD_NV3023_Parameter(0x1D);//VRP 8 13
	LCD_NV3023_CMD(0x76);LCD_NV3023_Parameter(0x46);//VRP 10 5
	LCD_NV3023_CMD(0x77);LCD_NV3023_Parameter(0x0B);//VRP 14 15
	LCD_NV3023_CMD(0x78);LCD_NV3023_Parameter(0x0E);//VRP 17 16
	LCD_NV3023_CMD(0x79);LCD_NV3023_Parameter(0x3D);//VRP 21 6
	LCD_NV3023_CMD(0x7a);LCD_NV3023_Parameter(0x05);//VRP 23 14
	LCD_NV3023_CMD(0x7b);LCD_NV3023_Parameter(0x07);//VRP 25 12
	LCD_NV3023_CMD(0x7c);LCD_NV3023_Parameter(0x12);//VRP 28 10
	LCD_NV3023_CMD(0x7d);LCD_NV3023_Parameter(0x0B);//VRP 29 8
	LCD_NV3023_CMD(0x7e);LCD_NV3023_Parameter(0x0B);//VRP 30 4
	LCD_NV3023_CMD(0x7f);LCD_NV3023_Parameter(0x08);//VRP 31 2
	LCD_NV3023_CMD(0xa0);LCD_NV3023_Parameter(0x1E);//VRN 0 1
	LCD_NV3023_CMD(0xa1);LCD_NV3023_Parameter(0x3F);//VRN 1 3
	LCD_NV3023_CMD(0xa2);LCD_NV3023_Parameter(0x0A);//VRN 2 7
	LCD_NV3023_CMD(0xa3);LCD_NV3023_Parameter(0x0D);//VRN 3 9
	LCD_NV3023_CMD(0xa4);LCD_NV3023_Parameter(0x08);//VRN 6 11
	LCD_NV3023_CMD(0xa5);LCD_NV3023_Parameter(0x23);//VRN 8 13
	LCD_NV3023_CMD(0xa6);LCD_NV3023_Parameter(0x3D);//VRN 10 5
	LCD_NV3023_CMD(0xa7);LCD_NV3023_Parameter(0x04);//VRN 14 15
	LCD_NV3023_CMD(0xa8);LCD_NV3023_Parameter(0x09);//VRN 17 16
	LCD_NV3023_CMD(0xa9);LCD_NV3023_Parameter(0x30);//VRN 21 6
	LCD_NV3023_CMD(0xaa);LCD_NV3023_Parameter(0x0A);//VRN 23 14 //
	LCD_NV3023_CMD(0xab);LCD_NV3023_Parameter(0x0E);//VRN 25 12
	LCD_NV3023_CMD(0xac);LCD_NV3023_Parameter(0x0E);//VRN 28 10
	LCD_NV3023_CMD(0xad);LCD_NV3023_Parameter(0x07);//VRN 29 8
	LCD_NV3023_CMD(0xae);LCD_NV3023_Parameter(0x2D);//VRN 30 4
	LCD_NV3023_CMD(0xaf);LCD_NV3023_Parameter(0x10);//VRN 31 2
	//////////////////////////////////////////////////////////////////
	LCD_NV3023_CMD(0xff);
	LCD_NV3023_Parameter(0x00);

	LCD_NV3023_CMD(0x11);

	delay_ms(150);
	LCD_NV3023_CMD(0x36);
	LCD_NV3023_Parameter(0x08);
	LCD_NV3023_CMD(0x29);
	delay_ms(10);

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
	Lcd_WriteIndex(0x2a);
	//Lcd_WriteData(0x00);
	LCD_WriteData_16Bit(x_start);//LCD_WriteData_16Bit
	//Lcd_WriteData(0x00);
	LCD_WriteData_16Bit(x_end);

	Lcd_WriteIndex(0x2b);
	//Lcd_WriteData(0x00);
	LCD_WriteData_16Bit(y_start);
	//Lcd_WriteData(0x00);
	LCD_WriteData_16Bit(y_end);	
	Lcd_WriteIndex(0x2c);

}

/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(uint16_t x,uint16_t y)
{
  	Lcd_SetRegion(x,y,x,y);
}
	
/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);
	LCD_WriteData_16Bit(Data);

}    

/*****************************************
 函数功能：读TFT某一点的颜色                          
 出口参数：color  点颜色值                                 
******************************************/
unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//丢掉无用字节
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}

/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(uint16_t Color)               
{	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    {	
	  	LCD_WriteData_16Bit(Color);
    }   
}




#if 0
//DMA refresh 
void RefreshColorBlockDynamic(uint32_t Color)
{
    static uint32_t colorBuffer[128];  // 小缓冲区
	
	  Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);	
    
    // 计算填充值
    uint32_t color32 = (uint32_t)Color;
    
    // 填充缓冲区
    for(int i = 0; i < 128; i++)
    {
        colorBuffer[i] = color32;
    }
		
		HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16_t*)colorBuffer, 128);		
		
}
#endif



/*************************************************
函数名：Lcd_Fill
功能：指定区域填充颜色
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Fill(uint16_t x,uint16_t y,uint16_t xend,uint16_t yend,uint16_t Color)               
{	
	unsigned int i;
	uint16_t num = (xend-x+1)*(yend-y+1);
	
	Lcd_SetRegion(x,y,xend,yend);
	for(i=0; i<num; i++)
		LCD_WriteData_16Bit(Color);
}
