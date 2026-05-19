/*****************************************************************************************************

RLE_decompression

RLE解压缩

*****************************************************************************************************/
/******************************************************************************************************

//游程编码（Run-Length Encoding, RLE）压缩算法


例:在"RGB565原始RLE压缩"数据储存格式 =
{
  dat0的数量,dat0h,dat0l,
  dat1的数量,dat1h,dat1l,
  ...,
  datn的数量,datnh,datnl,
  0x00,//0x00表示结束
};


******************************************************************************************************/



#include "lcd_driver.h"
#include "stdint.h"
#include "spi.h"
#include "dma.h"





extern void Lcd_Write_data_dma(uint8_t *p_data, uint16_t len);
extern void HW_SPI_Tx_DMA_16bit_ColorBlock(uint16 *pData, uint16 DataLen);

extern void HW_SPI_Tx_DMA_16bit_ColorBlock_Opt(uint16 *pData, uint16 DataLen);//optimize



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






// ==============================
// 配置宏：可自由调整 DMA 触发阈值
// ==============================
#define RLE_DMA_THRESHOLD  8    // 像素数 > 此值时使用DMA，否则CPU直接发送


__RAM_CODE__ static void RLE_Decode_Send(const uint8_t *arry)
{
    uint8_t count;
    uint8_t dat[2];
		uint16_t color;
    const uint8_t *p = arry;

    // 整个 RLE 解码循环（完全独立）
    for (;;)
    {
        count = *p++;
        if (count == 0) break;

        // 读取RGB565颜色
        dat[0] = *p++;
        dat[1] = *p++;


        // ================================
        // 【自适应发送：优化逻辑】
        // ================================			
			  if (count <= RLE_DMA_THRESHOLD)
				{
					// 短数据：CPU 直接发，比 DMA 快
					do
					{
							//SPI_WriteData(dat[0]);//高8位
							//SPI_WriteData(dat[1]);//低8位
							SPI_Write16bitData(dat[0]<<8|dat[1]);//LSBF (1<<6) // 1'b0:MSB first (高位在前)
					} while (--count);
					
				}
				else
				{
					// 长数据：DMA 批量发送，效率拉满
					color = (uint16_t)dat[0]<<8|dat[1];
					//HW_SPI_Tx_DMA_16bit_ColorBlock(&color,count);
					HW_SPI_Tx_DMA_16bit_ColorBlock_Opt(&color,count);
			  }

    }
}







//RGB565原始RLE数据解压例程:
//字符串或数组解码例程(无文件头)RLE-2解码

void TFT565_draw_arry_rle2_img(uint16_t x, uint16_t y, uint16_t sizex, uint16_t sizey, const uint8_t *arry)
{

    // 设置显示区域（只执行1次，不动它）
    Lcd_SetRegion(x, y, x + sizex - 1, y + sizey - 1);

    // 调用RAM中的解码函数
    RLE_Decode_Send(arry);
}








