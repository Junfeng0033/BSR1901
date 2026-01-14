/****************************************************************************
 *   $Id:: spi.c 2021-06-03 22:40:16
 *   Project: Gecko SPI driver
 *
 *   Description:
 *     This file contains SPI code example
 *   Author:
 *   	 YJ
****************************************************************************/
//#include "platform_config.h"

#include "spi.h"
#include "uart.h"
#include "dma.h"
#include "gpio.h"
#include "lcd_driver.h"





#define GECKO_BIT(num)							(1u << num)

#define SPI_READ_DATAREG(REG)      				(*(volatile unsigned short*)(REG))
#define SPI_WRITE_DATAREG(REG,VAL)    			(*(volatile unsigned short*)(GECKO_APB_SPI_DATAACCESS_REG + REG)) = (unsigned short)(VAL) 

#define SPI_READ_CTLREG(REG)      				(*(volatile unsigned int*)(REG))
#define SPI_WRITE_CTLREG(REG,VAL)    			(*(volatile unsigned int*)(REG)) = (unsigned int)(VAL) 

volatile uint8 *hw_spi_rx_buffer;
uint16 hw_spi_rx_length;

//static void (*hw_spi_tx_fptr)(void);

uint16  HW_SPI_Read_Data(uint16 address)
{
	uint16 temp_data;
	while(SPI_READ_CTLREG(GECKO_APB_SPI_STATUS_REG) & GECKO_BIT(GECKO_APB_SPI_BUSBUSY_BIT));
	temp_data = SPI_READ_DATAREG(GECKO_APB_SPI_DATAACCESS_REG + address);
	while(SPI_READ_CTLREG(GECKO_APB_SPI_STATUS_REG) & GECKO_BIT(GECKO_APB_SPI_BUSBUSY_BIT));
	while(SPI_READ_CTLREG(GECKO_APB_SPI_STATUS_REG) & GECKO_BIT(GECKO_APB_SPI_BUSBUSY_BIT));
	temp_data = SPI_READ_DATAREG(GECKO_APB_SPI_DATAACCESS_REG);
	
	return temp_data;
}

//uint16  HW_SPI_Write_Data(uint16 address, uint16 data)
void HW_SPI_Write_Data(uint16 address, uint16 data)
{
	while(SPI_READ_CTLREG(GECKO_APB_SPI_STATUS_REG) & GECKO_BIT(GECKO_APB_SPI_TXFIFOFULL_BIT));
	SPI_WRITE_DATAREG(address, data);
	//return 0;
}

void SPI_32bit_Transfer(void)
{
/*

在reg32_04[0:11]这12个bit中, size有两个bit，分别是bit4和bit5.
input [1:0] size; //2'b00:8bits; 2'b01:16bits(bit4=1,bit5=0); 2'b10:32bits(bit4=0,bit5=1);2'b11:24bits
bit4和bit5 是00，表示8bit; 

*/

	//hwp_spi0->CTROL = 0x10f8b;	//8bit spi data
//#define SPI_SIZE(n)					(((n)&0x3)<<4)
	
	 //rd_data=hwp_spi0->CTROL;
	 //rd_data |= SPI_SIZE(1)
	 hwp_spi0->CTROL=0x10fab;

}


void SPI_24bit_Transfer(void)
{
		hwp_spi0->CTROL = 0x10fbb;	//24bit spi data
}



void SPI_16bit_Transfer(void)
{
/*

在reg32_04[0:11]这12个bit中, size有两个bit，分别是bit4和bit5.
input [1:0] size; //2'b00:8bits; 2'b01:16bits(bit4=1,bit5=0); 2'b10:32bits(bit4=0,bit5=1);2'b11:24bits
bit4和bit5 是00，表示8bit; 

*/

	//hwp_spi0->CTROL = 0x10f8b;	//8bit spi data
//#define SPI_SIZE(n)					(((n)&0x3)<<4)
	
	 //rd_data=hwp_spi0->CTROL;
	 //rd_data |= SPI_SIZE(1)
	 hwp_spi0->CTROL=0x10f9b;

}
void SPI_8bit_Transfer(void)
{
		hwp_spi0->CTROL = 0x10f8b;	//8bit spi data
}


void HW_SPI_Initialise(HAL_SPI_ID_T id)
{
	volatile unsigned int rd_data;
	//char *string;

	if(id==HAL_SPI_0)
	{
#if 1
		//register write test
		//HW_SPI_SET_REG(0x00,0x1234);
		//hwp_spi0->FIFODATA=0x1234;			
		HW_SPI_SET_REG(0x04,0x5678);
		hwp_spi0->CTROL=0x5678;			
		HW_SPI_SET_REG(0x08,0x5757);
		hwp_spi0->STATS=0x5678;
		HW_SPI_SET_REG(0x0C,0x3579);
		hwp_spi0->RCVINT=0x3579;

		rd_data=HW_SPI_GET_REG(0x08);
		rd_data=hwp_spi0->STATS;
		rd_data=HW_SPI_GET_REG(0x0C);
		rd_data=hwp_spi0->RCVINT;
#endif
		
		HW_SPI_SET_REG(XR7_SPI_TRANS_INT_EN,XR7_SPI_TRANS_TRIG_0);			
		hwp_spi0->TRANSINT=XR7_SPI_TRANS_TRIG_0;
		/* reset initial trans trigger value */
		HW_SPI_SET_REG(XR7_SPI_RCV_INT_EN,XR7_SPI_RECV_TRIG_1);
		hwp_spi0->RCVINT=XR7_SPI_RECV_TRIG_1;				
		/* set initial recv trigger value */

		HW_SPI_SET_REG(XR7_SPI_CTL, (XR7_SPI_CTL_LSB_ENABLE |XR7_SPI_CTL_MISO_ENABLE | XR7_SPI_CTL_SPI_ENABLE | XR7_SPI_CTL_FLUSH));
		hwp_spi0->CTROL=(XR7_SPI_CTL_LSB_ENABLE |XR7_SPI_CTL_MISO_ENABLE | XR7_SPI_CTL_SPI_ENABLE | XR7_SPI_CTL_FLUSH);
		//0x04=0x1_0fbb;
		HW_SPI_SET_REG(XR7_SPI_CTL,0x10fab);				
		SPI_WRITE_CTLREG(GECKO_APB_SPI_CONTROL_REG,0x10fbb);
		HW_SPI_SET_REG(XR7_SPI_CTL,0x10fab);

		hwp_spi0->CTROL = 0x10f8b;	//8bit spi data
		hwp_spi0->RCVINT = 0x0;		//mask all interrupt

#if 0//DEBUG_UATR0_PRINT_LOG

		rd_data=HW_SPI_GET_REG(XR7_SPI_CTL);
		rd_data=hwp_spi0->CTROL;				
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
		UATR0_PRINT_LOG((unsigned char *)("HW_SPI_0_Initialise XR7_SPI_CTL --- = 0x"));
		string=my_itoa(rd_data);
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));	

#endif		
	}		
	
	else if(id==HAL_SPI_1)
	{
#if 1
		//register write test
		//        hwp_spi1->FIFODATA=0x1234;
		hwp_spi1->CTROL=0x5678;	
		hwp_spi1->STATS=0x5678;
		hwp_spi1->RCVINT=0x3579;			
		rd_data=hwp_spi1->STATS;
		rd_data=hwp_spi1->RCVINT;			
#endif

		hwp_spi1->TRANSINT=XR7_SPI_TRANS_TRIG_0;
		/* reset initial trans trigger value */
		hwp_spi1->RCVINT=XR7_SPI_RECV_TRIG_1;				
		/* set initial recv trigger value */

		hwp_spi1->CTROL=(XR7_SPI_CTL_LSB_ENABLE |XR7_SPI_CTL_MISO_ENABLE | XR7_SPI_CTL_SPI_ENABLE | XR7_SPI_CTL_FLUSH);
		//0x04=0x1_0fbb;
		hwp_spi1->CTROL=0x10fbb;

#if 0//DEBUG_UATR0_PRINT_LOG
		rd_data=hwp_spi1->CTROL;				
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
		UATR0_PRINT_LOG((unsigned char *)("HW_SPI_1_Initialise XR7_SPI_CTL --- = 0x"));
		string=my_itoa(rd_data);
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));
#endif
	}
}

void HW_SPI_Tx_Char_Polled(volatile uint8 **buf, volatile uint32 *length, uint8 flag)
{
    uint32 count = *length;
    volatile uint8 *temp_buf;
	
	temp_buf = *buf;

	while(!(HW_SPI_GET_REG(XR7_SPI_STATUS) & XR7_LSR_THRE));    /* Wait until the UART transmitter is empty */

	/*
	 * Once the transmitter is empty, fill the FIFO with characters
	 */

	if(count >= HW_SPI_TX_FIFO_LENGTH)
		count = HW_SPI_TX_FIFO_LENGTH;

	(*length)-= count;

	while(count > 0)
	{
		HW_SPI_SET_REG(XR7_SPI_FIFO,*(temp_buf++));
		count--;
	}

	*buf = temp_buf;
}






void HW_SPI_Tx(HAL_SPI_ID_T id,uint8 *pData, uint16 DataLen)
{
	//int tmp=0;
	uint16 count = DataLen;	
	//uint16 rd_data;
	//uint16 temp_buf;	

	while(!((HW_SPI_GET_REG(XR7_SPI_STATUS)) & tx_fifo_empty));
	
	while(count > 0)
	{
		//HW_SPI_SET_REG(XR7_SPI_FIFO,*(temp_buf));
		//temp_buf = (pData[0]<<8) + pData[1];
		hwp_spi0->FIFODATA = *pData;
		pData ++ ;
		count--;
		
		if(((HW_SPI_GET_REG(XR7_SPI_STATUS)) & tx_fifo_full))
			break;
	}
}






void HW_SPI_Tx_DMA_32bit(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)
{
	LCD_RS_SET;	
  SPI_32bit_Transfer();
	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
						AHB_DMA_CONTROL_WORD_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);	
	dma_sram_delay(1000);		
}


void HW_SPI_Tx_DMA_16bit(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)
{
	LCD_RS_SET;	
  SPI_16bit_Transfer();
	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
						AHB_DMA_CONTROL_HWORD_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);	
	dma_sram_delay(1000);	
	
}


void HW_SPI_Tx_DMA_16bit_ColorBlock(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)
{
	LCD_RS_SET;	
  SPI_16bit_Transfer();
	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
						AHB_DMA_CONTROL_HWORD_TR, AHB_DMA_CONTROL_SRC_NOINC_DES_NOINC);	
	dma_sram_delay(1000);	
	
}



void HW_SPI_Tx_DMA(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)
{
	LCD_RS_SET;
  SPI_8bit_Transfer();
	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
						AHB_DMA_CONTROL_BYTE_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);

	dma_sram_delay(1000);
}





#if 0

__RAM_CODE__ void HW_SPI_Tx_DMA(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)
{
	LCD_RS_SET;
  SPI_8bit_Transfer();
	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
						AHB_DMA_CONTROL_BYTE_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);

	//#ifdef SPI_16bit_TRSF
	//	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
	//						AHB_DMA_CONTROL_HWORD_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);

	//#endif
		

	//#ifdef SPI_32bit_TRSF

	//Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
	//					AHB_DMA_CONTROL_WORD_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);	

	//#endif
	
	dma_sram_delay(1000);
}

#endif



extern void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end);


void lcd_dma_refresh(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color)
{
	uint32_t len = w*h;
	
	Lcd_SetRegion(xs, ys, xs+w-1, ys+h-1);	
	HW_SPI_Tx_DMA_16bit(HAL_SPI_0,color,len);
	
}



void lcd_dma_refresh_colorblock(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color)
{
	uint32_t len = w*h;
	
	Lcd_SetRegion(xs, ys, xs+w-1, ys+h-1);	
	
	HW_SPI_Tx_DMA_16bit_ColorBlock(HAL_SPI_0,color,len);
	
}



/*


void app_lcd_fill_color(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, uint16_t *color)
{
	uint32_t tx_count = 0;
  uint16_t xe=xs+w-1;
	uint16_t ye=ys+h-1;
	tx_count = w*h;//设置需要清除的区域的字节大小;
	app_lcd_set_address(xs,ys,xe,ye);
  
#ifdef CFG_LCD_CS_SOFT
  CFG_LCD_CS_ENABLE;
#endif
  HAL_SPI_Transmit_DMA(&CFG_LCD_SPI, (uint8_t*)color, tx_count*2);

  app_lcd_wait_spi_state(100);
}


*/










/*****************************************************************************
 * dest     address of destination  (byte aligned)
 * src      address of source       (byte aligned)
 * size     in bytes 
//void Gecko_DMA_Transport(volatile void *dest,volatile const void *src, uint16 size,uint8 type, uint8 direct)
 *****************************************************************************/
void HW_SPI_Tx_Block(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)
{
	int tmp=0;
	uint16 count = DataLen;	
	uint16 rd_data;
	volatile uint16 *temp_buf;	
	temp_buf = pData;

	if(count >= HW_SPI_TX_FIFO_LENGTH)
		count = HW_SPI_TX_FIFO_LENGTH;

	if(id == HAL_SPI_0)
	{

		//20240125@JF			
		//Before initiating the second DMA-SPI transfer, 
		//it is necessary to check whether SPI has completed the previous data transfer at this time
		//so we need to check spi_busy flag here.
		//The speed of DMA is much faster than that of SPI.	

		//reg_0x08(status register) check busy bit first	

		while((HW_SPI_GET_REG(XR7_SPI_STATUS)) & busy_spi_sync)
		{
			if(tmp++>500)
			{
				return;
			}
		}

#if GEK_DMA_ENABLE
		
		if(count > GECKO_DMA_MIN_SIZE)
		{
			Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, count, 
								AHB_DMA_CONTROL_HWORD_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);
		}
		else
#endif
		{
			while(count > 0)
			{
				//HW_SPI_SET_REG(XR7_SPI_FIFO,*(temp_buf));
				hwp_spi0->FIFODATA = *(temp_buf);
				temp_buf ++ ;
				count--;
			}
		}

		//HW_SPI_Empty_FIFO();
		//while((HW_SPI_GET_REG(XR7_SPI_STATUS)) & XR7_SPI_STATUS_RX_FIFO_INT_FLAG) /* Read all characters out of the fifo */
		//{
		//		;
		//}	
		//HW_SPI_GET_REG(XR7_SPI_STATUS)
		
		rd_data = hwp_spi0->STATS;
		rd_data = rd_data &	XR7_SPI_STATUS_RX_FIFO_INT_FLAG;
		
		while(rd_data) /* Read all characters out of the fifo */
		{
			;
		}
	}
	else if(id==HAL_SPI_1)
	{
			
		//20240125@JF			
		//Before initiating the second DMA-SPI transfer, 
		//it is necessary to check whether SPI has completed the previous data transfer at this time
		//so we need to check spi_busy flag here.
		//The speed of DMA is much faster than that of SPI.	

		//reg_0x08(status register) check busy bit first	
		while((HW_SPI_GET_REG(XR7_SPI_STATUS)) & busy_spi_sync)
		{
			if(tmp++>500)
			{
				return;
			}
		}			

#if GEK_DMA_ENABLE
		
		if(count > GECKO_DMA_MIN_SIZE)
		{
			Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), 
			pData, count, AHB_DMA_CONTROL_BYTE_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);
		}
		else
#endif
		{
			while(count > 0)
			{
				//HW_SPI_SET_REG(XR7_SPI_FIFO,*(temp_buf));
				hwp_spi1->FIFODATA = *(temp_buf);
				temp_buf ++ ;
				count--;
			}
		}
		
		//HW_SPI_Empty_FIFO();
		//while((HW_SPI_GET_REG(XR7_SPI_STATUS)) & XR7_SPI_STATUS_RX_FIFO_INT_FLAG) /* Read all characters out of the fifo */
		//{
		//		;
		//}	
		//HW_SPI_GET_REG(XR7_SPI_STATUS)
		
		rd_data = hwp_spi1->STATS;
		rd_data = rd_data &	XR7_SPI_STATUS_RX_FIFO_INT_FLAG;
		
		while(rd_data) /* Read all characters out of the fifo */
		{
			;
		}
	}			
}

void HW_SPI_Interrupt_Handler(void)
{
	uint16 length = 0;
	while(((HW_SPI_GET_REG(XR7_SPI_STATUS)) & XR7_SPI_STATUS_RX_FIFO_INT_FLAG) && length < hw_spi_rx_length)
	{
		*hw_spi_rx_buffer=HW_SPI_GET_REG(XR7_SPI_FIFO) & 0xff;
		hw_spi_rx_buffer++;
		length ++;
	}
	//TRAhcit_UART_Rx_Data(length);
}

void HW_SPI_Setup(volatile uint8 *rx_buffer, uint16 rx_length, uint8 flag)
{
    if(rx_buffer!=NULL)
        hw_spi_rx_buffer = rx_buffer;
    hw_spi_rx_length = rx_length;
    HW_SPI_SET_REG(XR7_SPI_RCV_INT_EN,rx_length);
}

void HW_SPI_Empty_FIFO(void)
{
    while((HW_SPI_GET_REG(XR7_SPI_STATUS)) & XR7_SPI_STATUS_RX_FIFO_INT_FLAG) /* Read all characters out of the fifo */
    {
        ;
    }
}


//====================================================================================================
//
// SPI Slave Write/Read command format
//
//====================================================================================================










/**********************************************************
*
*#define GECKO_AON_BASE_ADDR     0x4001A000
*
*Oscillator Calibration:
*0x20 default 0x808E7885:
*05 40 01 A0 24 00 00 00 10
*05 40 01 A0 20 80 8E 78 85
*
*
***********************************************************/


extern const unsigned int fire_eye_1[12800];
extern const unsigned int fire_eye_2[12800];
extern const unsigned int fire_eye_3[12800];
extern const unsigned int fire_eye_4[12800];

extern const unsigned int fire_eye_5[12800];
extern const unsigned int fire_eye_6[12800];
extern const unsigned int fire_eye_7[12800];
extern const unsigned int fire_eye_8[12800];


extern void delay_1us(unsigned int delay_val);


void BSR1901_FireEye_Demo(void)
{
	#if 1

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_1, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_2, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_3, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_4, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_5, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_6, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_7, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_8, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_7, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_6, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_5, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_4, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_3, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_2, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);

	SPI_8bit_Transfer();
  Lcd_SetRegion(0, 30, 159, 159);		
	SPI_32bit_Transfer();
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_1, 12800);	
	dma_sram_delay(1000);
	delay_1us(9500);




#endif
	
}



