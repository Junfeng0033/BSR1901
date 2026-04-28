
/****************************************************************************
 *   $Id:: dma.c 2020-07-15 
 *   Project: Gecko DMA driver
 *	 Author: Moana/JF
 *   Description: 
****************************************************************************/


#include "dma.h"
#include "bsr1901.h"
#include "lcd_driver.h"
#include "gpio.h"
#include "ui.h"
#include "spi.h"


extern void NVIC_EnableIRQ(IRQn_Type IRQn);
extern void NVIC_DisableIRQ(IRQn_Type IRQn);


extern void HW_SPI_Tx_DMA(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen);
extern void HW_SPI_Tx_DMA_8bit(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen);

uint16 location = 0;




void DMA_Configuration(void)
{
	volatile uint32 dma_ctrl;
	
	dma_ctrl=0;

//===========================================================================
//new function in chip BSR0035	
	dma_ctrl = DMA_READ_REG(AHB_DMA_CONTROL_REG);
	
	dma_ctrl |= dma_buslock_req_en;

	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_CONTROL_REG, dma_ctrl);

//===========================================================================
	
	
	dma_ctrl = DMA_READ_REG(AHB_DMA_STEP_CTRL_REG);
	
	dma_ctrl |= dma_for_dac_en;

	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_STEP_CTRL_REG, dma_ctrl);

	dma_ctrl |= dma_int_mask;

	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_STEP_CTRL_REG, dma_ctrl);

	hal_nvic_clear_pending_irq(DMA_IRQn);
	
	hal_nvic_set_priority(DMA_IRQn, 0);
	
	NVIC_EnableIRQ(DMA_IRQn);
}



void DMA_Req_Buslock_Enable(void)
{
	volatile uint32 dma_ctrl;
	
	dma_ctrl=0;
//===========================================================================
//new function in chip BSR0035	
	dma_ctrl = DMA_READ_REG(AHB_DMA_CONTROL_REG);
	
	dma_ctrl |= dma_buslock_req_en;

	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_CONTROL_REG, dma_ctrl);

//===========================================================================
}




void DMA_Req_Buslock_Disable(void)
{
	volatile uint32 dma_ctrl;
	
	dma_ctrl=0;
//===========================================================================
//new function in chip BSR0035	
	dma_ctrl = DMA_READ_REG(AHB_DMA_CONTROL_REG);
	
	dma_ctrl &= (~dma_buslock_req_en);

	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_CONTROL_REG, dma_ctrl);

//===========================================================================
}





__RAM_CODE__ void Clear_DMA_Interrupt(void)
	
{
	volatile uint32 dma_ctrl_tmp0=0;
	
	dma_ctrl_tmp0 |=dma_int_clear;
	
	dma_ctrl_tmp0 |=dma_int_mask;

	dma_ctrl_tmp0 |=dma_for_dac_en;
	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_STEP_CTRL_REG,dma_ctrl_tmp0);
	

//===============================================================================
//new function in chip BSR0035	
	dma_ctrl_tmp0 = DMA_READ_REG(AHB_DMA_CONTROL_REG);
	
	dma_ctrl_tmp0 &= (~dma_buslock_req_en);

	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_CONTROL_REG, dma_ctrl_tmp0);

//===============================================================================
	
}






void Gecko_DMA_Transport(volatile void *dest,volatile const void *src, uint16 size,uint8 type, uint8 direct)
{
	volatile uint32 dma_statu,dma_ctrl;//cpu_flags;

	uint32 status = pEnterCriticalSection();

	dma_statu=DMA_READ_REG(AHB_DMA_DONE_STS_REG);
	dma_statu=DMA_READ_REG(AHB_DMA_STEP_CTRL_REG);
	
    //SYSirq_Disable_Interrupts_Save_Flags(&cpu_flags);

    DMA_WRITE_REG((volatile uint32 *)AHB_DMA_SRCADDR_REG, (uint32) src);
    DMA_WRITE_REG((volatile uint32 *)AHB_DMA_DESTADDR_REG, (uint32) dest);
    DMA_WRITE_REG((volatile uint32 *)AHB_DMA_DATALENGTH_REG, size);
    DMA_WRITE_REG((volatile uint32 *)AHB_DMA_CONTROL_REG,(type|direct));
    //SYSirq_Interrupts_Restore_Flags(cpu_flags);
	pExitCriticalSection(status);
	
	
}






void DMA_ISR_Routine(void)
{
	volatile uint32 dma_ctrl_tmp1=0;
	
	dma_ctrl_tmp1 |= dma_int_clear;//clear interrupt
	
	dma_ctrl_tmp1 |= dma_int_mask; //maintain dma_int_mask bit "1"

	dma_ctrl_tmp1 |= dma_for_dac_en;
	
	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_STEP_CTRL_REG, dma_ctrl_tmp1);		


	//void Gecko_DMA_Transport(volatile void *dest,volatile const void *src, uint16 size,uint8 type, uint8 direct)
	//Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, count, AHB_DMA_CONTROL_HWORD_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);	

//	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_SRCADDR_REG, gImage);
//	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_DESTADDR_REG, (uint32) 0x4001B000);//spi fifo base addr
//	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_DATALENGTH_REG, 3200);
//	DMA_WRITE_REG((volatile uint32 *)AHB_DMA_CONTROL_REG,(AHB_DMA_CONTROL_HWORD_TR|AHB_DMA_CONTROL_SRC_INC_DES_NOINC));	
	

//	Lcd_SetRegion(location, 0, 85+location, 49);		//×ø±êÉèÖÃ

	LCD_RS_SET;


//	if(location)
//		HW_SPI_Tx_DMA(HAL_SPI_0, gImage_libai, 25600);
//	else 
//		HW_SPI_Tx_DMA(HAL_SPI_0, gImage_taizi, 25600);
  

	
	location = ~location;
	
}




/*****************************************************************************
 * FUNCTION: hw_memcpy8
 *
 * byte (8 bit) direct copy
 *
 * dest     address of destination  (byte aligned)
 * src      address of source       (byte aligned)
 * size     in bytes   
 *****************************************************************************/
void * hw_memcpy8(void * dest, const void * src, uint16 size)
{
#if 1//GEK_DMA_ENABLE
    if(size > GECKO_DMA_MIN_SIZE)
    {
       Gecko_DMA_Transport(dest,src,size,AHB_DMA_CONTROL_HWORD_TR, AHB_DMA_CONTROL_SRC_INC_DES_INC); 
    }else
#endif
    {
        uint8 *_u8_dest = (uint8 *) dest;
        uint8 *_u8_end  = (uint8 *) dest + size;
			
        uint8 *_u8_src  = (uint8 *) src;

        while (_u8_dest < _u8_end)
        {
            *_u8_dest++ = *_u8_src++;
        }
    }

    return dest;
}



/*****************************************************************************
 * FUNCTION: hw_memset8
 *
 * DESCRIPTION: 8-bit memset where value is written size times at dest
 *****************************************************************************/
void hw_memset8(void * dest, uint8 value, uint16 size)
{
    uint8 *_u8_dest = (uint8 *)dest;
    uint8 *_u8_end  = (uint8 *)dest + size;

#if GEK_DMA_ENABLE
    if(size > GECKO_DMA_MIN_SIZE)
    {
//        size= ((size + 2) >> 1);
        Gecko_DMA_Transport(dest,&value, size,AHB_DMA_CONTROL_BYTE_TR, AHB_DMA_CONTROL_SRC_NOINC_DES_INC); 
    }else
#endif

    while (_u8_dest < _u8_end)
    {
        *_u8_dest++ = value;
    }
}


















#define USB_BASE_ADDR 		0X80000000
#define M_FIFO_EP0			USB_BASE_ADDR + 32

/****************************************
  FIFORead
****************************************/
void FIFORead (int nEP, int nBytes, volatile void * pDst)
{
    int     nCount;
    BYTE *  pby;
    int     nAddr;

    if (nBytes)
    {
        nAddr = M_FIFO_EP0+(nEP<<2);
        nCount = nBytes;
        pby = (BYTE *)pDst;

#if GEK_DMA_ENABLE
        if(nCount > GECKO_DMA_MIN_SIZE)
        {
            Gecko_DMA_Transport((volatile uint32 *)pby,
            (volatile uint32 *)(nAddr ),
            nCount, AHB_DMA_CONTROL_BYTE_TR, AHB_DMA_CONTROL_SRC_NOINC_DES_INC);
        }
        else
#endif
        {
            while (nCount) {
                *pby++ = *((BYTE *)nAddr);
                nCount--;
            }
        }
    }
    return;
}






/****************************************
  FIFOWrite
****************************************/
void FIFOWrite (int nEP, int nBytes, volatile void * pSrc)
{
    int     nCount;
    BYTE *  pby;
    int     nAddr;

    if (nBytes)
    {
        nAddr = M_FIFO_EP0+(nEP<<2);
        nCount = nBytes;
        pby = (BYTE *)pSrc;

#if GEK_DMA_ENABLE
        if(nBytes > GECKO_DMA_MIN_SIZE)
        {
            Gecko_DMA_Transport((volatile uint32 *)(nAddr ), (volatile uint32 *)pby, nCount, AHB_DMA_CONTROL_BYTE_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);
        }
        else
#endif
        {
            while (nCount) {
               *((BYTE *)nAddr) = *pby++;
               nCount--;
            }
        }
    }
    return;
}




