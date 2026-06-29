#include "lcd_driver.h"
#include "stdint.h"
#include "gpio.h"
#include "spi.h"
#include "iomux.h"
#include "dma.h"

/******************************************************************************************************

//游程编码（Run-Length Encoding, RLE）压缩算法

//https://github.com/KOUFU-DIY/WeGui_RGB

//https://gitee.com/li_yucheng/scgui

//https://gitee.com/landlord_money/ugui

//https://github.com/olikraus/u8g2

//https://gitee.com/Polarix/simplegui

******************************************************************************************************/




app_lcd_t lcd;



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




//向液晶屏写一个8位指令
void Lcd_WriteIndex(uint8_t Index)
{
	//SPI 写命令时序开始
	LCD_DC_CLR;
	SPI_WriteData(Index);
}

//向液晶屏写一个8位数据
void Lcd_WriteData(uint8_t Data)
{
   LCD_DC_SET;
   SPI_WriteData(Data); 
}

//向液晶屏写一个16位数据
void LCD_WriteData_16Bit(uint16_t Data)
{
	LCD_DC_SET;

#if 0	
	SPI_WriteData(Data>>8); 	//写入高8位数据
	SPI_WriteData(Data); 			//写入低8位数据
#else

//#define LSBF								(1<<6)
/*
input lsbf;

1'b0:MSB first (高位在前)

1'b1:LSB first
*/
	
	//hwp_spi0->CTROL=0x10f9b;							//16bit spi data
  //#define 	SPI_Write16bitData(data) 			{hwp_spi0->CTROL = 0x10f9b;hwp_spi0->FIFODATA = data;}




	SPI_Write16bitData(Data);
	
	
	
#endif	
	
}



//void Lcd_WriteReg(uint8_t Index,uint8_t Data)
//{
//	Lcd_WriteIndex(Index);
//	Lcd_WriteData(Data);
//}



void Lcd_Reset(void)
{
	LCD_RST_SET;
	delay_ms(100);
	LCD_RST_CLR;
	delay_ms(600);
	LCD_RST_SET;
	delay_ms(50);
}



//LCD Init For 1.47Inch LCD Panel with NV3022B.
void Lcd_Init(void)
{	
	LCD_GPIO_Init();

	Lcd_Reset(); //Reset before LCD Init.

#if 1//GC9A01
	
//1.28寸 TFT，240×240 分辨率，圆屏方板模块
	
	LCD_WR_REG(0xEF);
	LCD_WR_REG(0xEB);
	LCD_WR_DATA8(0x14); 
	
  LCD_WR_REG(0xFE);			 
	LCD_WR_REG(0xEF); 

	LCD_WR_REG(0xEB);	
	LCD_WR_DATA8(0x14); 

	LCD_WR_REG(0x84);			
	LCD_WR_DATA8(0x40); 

	LCD_WR_REG(0x85);			
	LCD_WR_DATA8(0xFF); 

	LCD_WR_REG(0x86);			
	LCD_WR_DATA8(0xFF); 

	LCD_WR_REG(0x87);			
	LCD_WR_DATA8(0xFF);

	LCD_WR_REG(0x88);			
	LCD_WR_DATA8(0x0A);

	LCD_WR_REG(0x89);			
	LCD_WR_DATA8(0x21); 

	LCD_WR_REG(0x8A);			
	LCD_WR_DATA8(0x00); 

	LCD_WR_REG(0x8B);			
	LCD_WR_DATA8(0x80); 

	LCD_WR_REG(0x8C);			
	LCD_WR_DATA8(0x01); 

	LCD_WR_REG(0x8D);			
	LCD_WR_DATA8(0x01); 

	LCD_WR_REG(0x8E);			
	LCD_WR_DATA8(0xFF); 

	LCD_WR_REG(0x8F);			
	LCD_WR_DATA8(0xFF); 


	LCD_WR_REG(0xB6);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x20);

	LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x68);
	else LCD_WR_DATA8(0xA8);

	LCD_WR_REG(0x3A);			
	LCD_WR_DATA8(0x05); 


	LCD_WR_REG(0x90);			
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08); 

	LCD_WR_REG(0xBD);			
	LCD_WR_DATA8(0x06);
	
	LCD_WR_REG(0xBC);			
	LCD_WR_DATA8(0x00);	

	LCD_WR_REG(0xFF);			
	LCD_WR_DATA8(0x60);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x04);

	LCD_WR_REG(0xC3);			
	LCD_WR_DATA8(0x13);
	LCD_WR_REG(0xC4);			
	LCD_WR_DATA8(0x13);

	LCD_WR_REG(0xC9);			
	LCD_WR_DATA8(0x22);

	LCD_WR_REG(0xBE);			
	LCD_WR_DATA8(0x11); 

	LCD_WR_REG(0xE1);			
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x0E);

	LCD_WR_REG(0xDF);			
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x0c);
	LCD_WR_DATA8(0x02);

	LCD_WR_REG(0xF0);   
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x26);
 	LCD_WR_DATA8(0x2A);

 	LCD_WR_REG(0xF1);    
 	LCD_WR_DATA8(0x43);
 	LCD_WR_DATA8(0x70);
 	LCD_WR_DATA8(0x72);
 	LCD_WR_DATA8(0x36);
 	LCD_WR_DATA8(0x37);  
 	LCD_WR_DATA8(0x6F);


 	LCD_WR_REG(0xF2);   
 	LCD_WR_DATA8(0x45);
 	LCD_WR_DATA8(0x09);
 	LCD_WR_DATA8(0x08);
 	LCD_WR_DATA8(0x08);
 	LCD_WR_DATA8(0x26);
 	LCD_WR_DATA8(0x2A);

 	LCD_WR_REG(0xF3);   
 	LCD_WR_DATA8(0x43);
 	LCD_WR_DATA8(0x70);
 	LCD_WR_DATA8(0x72);
 	LCD_WR_DATA8(0x36);
 	LCD_WR_DATA8(0x37); 
 	LCD_WR_DATA8(0x6F);

	LCD_WR_REG(0xED);	
	LCD_WR_DATA8(0x1B); 
	LCD_WR_DATA8(0x0B); 

	LCD_WR_REG(0xAE);			
	LCD_WR_DATA8(0x77);
	
	LCD_WR_REG(0xCD);			
	LCD_WR_DATA8(0x63);		


	LCD_WR_REG(0x70);			
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0E); 
	LCD_WR_DATA8(0x0F); 
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x03);

	LCD_WR_REG(0xE8);			
	LCD_WR_DATA8(0x34);

	LCD_WR_REG(0x62);			
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xED);
	LCD_WR_DATA8(0x70); 
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xEF);
	LCD_WR_DATA8(0x70); 
	LCD_WR_DATA8(0x70);

	LCD_WR_REG(0x63);			
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x70); 
	LCD_WR_DATA8(0x70);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x71);
	LCD_WR_DATA8(0xF3);
	LCD_WR_DATA8(0x70); 
	LCD_WR_DATA8(0x70);

	LCD_WR_REG(0x64);			
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x29);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0xF1);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x07);

	LCD_WR_REG(0x66);			
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xCD);
	LCD_WR_DATA8(0x67);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x45);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x67);			
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x98);

	LCD_WR_REG(0x74);			
	LCD_WR_DATA8(0x10);	
	LCD_WR_DATA8(0x85);	
	LCD_WR_DATA8(0x80);
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x4E);
	LCD_WR_DATA8(0x00);					
	
  LCD_WR_REG(0x98);			
	LCD_WR_DATA8(0x3e);
	LCD_WR_DATA8(0x07);

	LCD_WR_REG(0x35);	
	LCD_WR_REG(0x21);

	LCD_WR_REG(0x11);
	delay_ms(120);
	LCD_WR_REG(0x29);
	delay_ms(20);	

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




void app_lcd_set_direction(app_lcd_direction_t dir)
{
  lcd.dir = dir;
#ifdef CFG_LCD_ST7735_096_80X160
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
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);

#if 0	
	//LCD_WriteData_16Bit(Data);
#else	
	LCD_DC_SET;
	SPI_Write16bitData(Data);
#endif

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
#endif

	
	uint32_t len = X_MAX_PIXEL*Y_MAX_PIXEL;
	
	Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
	
	HW_SPI_Tx_DMA_16bit_ColorBlock(&Color,len);
  
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




