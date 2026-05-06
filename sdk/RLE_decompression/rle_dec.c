/*****************************************************************************************************

RLE_decompression

RLE压缩基本不占CPU的

*****************************************************************************************************/
/******************************************************************************************************

//游程编码（Run-Length Encoding, RLE）压缩算法

******************************************************************************************************/



#include "lcd_driver.h"
#include "stdint.h"
#include "spi.h"
#include "dma.h"



//hwp_spi0->CTROL = 0x10f8b;						//8bit spi data
#define 	SPI_WriteData(data) 					{hwp_spi0->CTROL = 0x10f8b;hwp_spi0->FIFODATA = data;}

//hwp_spi0->CTROL=0x10f9b;							//16bit spi data
#define 	SPI_Write16bitData(data) 			{hwp_spi0->CTROL = 0x10f9b;hwp_spi0->FIFODATA = data;}


#if 0

//向液晶屏写一个16位数据
void LCD_WriteData_16Bit(uint16_t Data)
{
	LCD_RS_SET;

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

#endif






//RGB565原始E数好据解压例程:
//字符串或数组解码例程(无文件头)RLE-2解码
void TFT565_draw_arry_rle2_img(uint16_t x,uint16_t y, uint16_t sizex, uint16_t sizey, const uint8_t *arry)
{

	  uint8_t num,dat0,dat1;
	
		Lcd_SetRegion(x, y, x + sizex - 1, y + sizey - 1);
	
		while (1) 
		{ 	
				num = *arry++;
			
				if (num == 0) 
				{
						break;
				}
				
				dat0 = *arry++;
				dat1 = *arry++;	
				
				while (num--) 
				{
 
					  SPI_WriteData(dat0);//高8位
						SPI_WriteData(dat1);//低8位

					  //SPI_Write16bitData(dat0<<8|dat1 );//LSBF (1<<6) // 1'b0:MSB first (高位在前)
				}
		}

}












