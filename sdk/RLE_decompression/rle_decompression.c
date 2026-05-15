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



//hwp_spi0->CTROL = 0x10f8b;						//8bit spi data
#define 	SPI_WriteData(data) 					{hwp_spi0->CTROL = 0x10f8b;hwp_spi0->FIFODATA = data;}

//hwp_spi0->CTROL=0x10f9b;							//16bit spi data
#define 	SPI_Write16bitData(data) 			{hwp_spi0->CTROL = 0x10f9b;hwp_spi0->FIFODATA = data;}


extern void Lcd_Write_data_dma(uint8_t *p_data, uint16_t len);
extern void HW_SPI_Tx_DMA_16bit_ColorBlock(uint16 *pData, uint16 DataLen);
extern void HW_SPI_Tx_DMA_16bit_ColorBlock_Opt(uint16 *pData, uint16 DataLen);



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



#if 0

// 【核心DMA函数】一次性发送 count 个相同RGB565像素
// 无buf、无拷贝、纯硬件重复发送
void Lcd_Write_color_repeat_dma(uint8_t dh, uint8_t dl, uint8_t count)
{
    // 1. 把颜色放入DMA源寄存器（仅2字节，无数组）
    //uint8_t dma_src_buf[2] = {dh, dl};  // 栈上2字节，不浪费RAM
		
		uint16_t color = (dh << 8) | dl;

    // 2. 配置DMA：重复发送2字节，总长度 count*2
    // 底层DMA自动循环发送这2字节，CPU不参与
    //Lcd_Write_data_dma(dma_src_buf, count * 2);
		HW_SPI_Tx_DMA_16bit_ColorBlock(&color,count);
}


// 【真正终极版】RGB565 RLE解码
// ✅ 无任何buf   ✅ 无任何拷贝
// ✅ 一次DMA    ✅ 发完count个像素
// ✅ CPU零占用  ✅ 速度拉满
void TFT565_draw_arry_rle2_img(uint16_t x, uint16_t y, 
                               uint16_t sizex, uint16_t sizey, 
                               const uint8_t *arry)
{
    const uint8_t *p = arry;
    uint8_t count;

    // 1. 配置LCD窗口（一次）
    Lcd_SetRegion(x, y, x+sizex-1, y+sizey-1);

    // 2. RLE解码（极简、高效）
    while( (count = *p++) != 0 )
    {
        // 直接读取RLE里的像素数据（无拷贝）
        uint8_t dh = *p++;
        uint8_t dl = *p++;

        // =============================================
        // 【关键】DMA 配置为：重复发送2字节 count次
        // 一次配置，硬件自动发完，CPU直接走人
        // =============================================
        Lcd_Write_color_repeat_dma(dh, dl, count);
    }
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








