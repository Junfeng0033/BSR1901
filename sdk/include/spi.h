
#include "platform_config.h"

#include "sc_common.h"


#define    GECKO_APB_SPI_BASE						0X4001B000


typedef enum {
    HAL_SPI_0                                  = 0x00000000,
    HAL_SPI_1                                  = 0x00000001,//New SPI--->2022-09-04	
    HAL_SPI_QTY
} HAL_SPI_ID_T;   

//`define ADDR_BASE_APB_I2C0      32'h4001_A000
//`define ADDR_BASE_APB_SPI0      32'h4001_B000
//`define ADDR_BASE_APB_I2C1      32'h4001_C000
//`define ADDR_BASE_APB_SPI1      32'h4001_D000

#define 	 GECKO_APB_SPI_0_BASE  				0x4001B000
#define	   GECKO_APB_SPI_1_BASE			    0x4001D000 //New SPI --->2022-09-21


#define XR7_SPI_FIFO                    0x00
#define XR7_SPI_CTL                     0x04
#define XR7_SPI_STATUS                  0x08
#define XR7_SPI_RCV_INT_EN              0x0C
#define XR7_SPI_TRANS_INT_EN            0x10

typedef volatile struct
{
    REG32                          FIFODATA;              //0x00000000//fifo data
    REG32                          CTROL;                 //0x00000004//control
    REG32                          STATS;                 //0x00000008//status
    REG32                          RCVINT;                //0x0000000C//interrupt control
    REG32                          TRANSINT;              //0x00000010//interrupt control	
} HWP_SPI_T;


#define hwp_spi0                   ((HWP_SPI_T*) GECKO_APB_SPI_0_BASE)
#define hwp_spi1                   ((HWP_SPI_T*) GECKO_APB_SPI_1_BASE)


/***********************************************************************
 * APB SPI module
 **********************************************************************/
#define GECKO_APB_SPI_DATAACCESS_REG			(GECKO_APB_SPI_BASE + 0X00)
#define GECKO_APB_SPI_CONTROL_REG				  (GECKO_APB_SPI_BASE + 0X04)
#define GECKO_APB_SPI_STATUS_REG			    (GECKO_APB_SPI_BASE + 0X08)

#define GECKO_APB_SPI_BUSBUSY_BIT				    3
#define GECKO_APB_SPI_RXBUFFERFULL_BIT	    2	
#define GECKO_APB_SPI_TXFIFOFULL_BIT		    1
#define GECKO_APB_SPI_TXFIFOEMPTY_BIT			  0

#define GECKO_APB_SPI_INTCNT_REG					(GECKO_APB_SPI_BASE + 0X0C)

/***********************************************************************/



#define XR7_SPI_BASE    0X4001B000 /* SPI Base Address */



#define HW_SPI_GET_REG(x)      (*(volatile uint32*)(XR7_SPI_BASE + (x)))
#define HW_SPI_SET_REG(r,x)    (*(volatile uint32*)(XR7_SPI_BASE + (r))) = (uint32)(x)



//register 0x04
#define TIMOD(n)						(((n)&0x3)<<0) //timod
#define SZ									(1<<2) 
#define GM									(1<<3)
//{size,lsbf}
#define SPI_SIZE(n)					(((n)&0x3)<<4)
#define LSBF								(1<<6)

#define CPHA								(1<<7)
#define CPOL								(1<<8)
#define MSTR								(1<<9)
#define SPE								  (1<<10)
#define GUARD_TIME				  (1<<11)

#define SPI_BAUD(n)         (((n)&0xFFFF)<<16) //spi baud


/*

在reg32_04[0:11]这12个bit中, size有两个bit，分别是bit4和bit5.
input [1:0] size; //2'b00:8bits; 2'b01:16bits(bit4=1,bit5=0); 2'b10:32bits(bit4=0,bit5=1);2'b11:24bits
bit4和bit5 是00，表示8bit; 

*/
//#define 	SPI_WriteData(data) 				{hwp_spi0->CTROL = 0x10f8b;hwp_spi0->FIFODATA = data;}

#define SPI_8bit_TRSF					{hwp_spi0->CTROL|= SPI_SIZE(0);}
#define SPI_16bit_TRSF				{hwp_spi0->CTROL|= SPI_SIZE(1);}
#define SPI_32bit_TRSF				{hwp_spi0->CTROL|= SPI_SIZE(2);}
#define SPI_24bit_TRSF				{hwp_spi0->CTROL|= SPI_SIZE(3);}





//register 0x08
//status
//wire [3:0] status
#define busy_spi_sync									(1<<3)
#define rx_fifo_full									(1<<2)
#define tx_fifo_full									(1<<1)
#define tx_fifo_empty									(1<<0)


#define XR7_SPI_CTL_SPI_ENABLE          0x01
#define XR7_SPI_CTL_SPI_DISABLE         0x00
#define XR7_SPI_CTL_LSB_ENABLE          0x02
#define XR7_SPI_CTL_MSB_ENABLE          0x00
#define XR7_SPI_CTL_MISO_ENABLE         0x04
#define XR7_SPI_CTL_FLUSH               0x08

#define XR7_SPI_STATUS_RX_FIFO_INT_FLAG 0x400

#define XR7_SPI_TRANS_TRIG_0               0
#define XR7_SPI_RECV_TRIG_0                0
#define XR7_SPI_RECV_TRIG_1                1
#define XR7_SPI_RECV_TRIG_2                2
#define XR7_SPI_RECV_TRIG_4                4
#define XR7_SPI_RECV_TRIG_8                8
#define XR7_SPI_RECV_TRIG_16              16
#define XR7_SPI_RECV_TRIG_32              32
#define XR7_SPI_RECV_TRIG_100            100
#define XR7_SPI_RECV_TRIG_254            254
#define XR7_SPI_RECV_TRIG_255            255
#define XR7_SPI_RECV_TRIG_400            400
#define XR7_SPI_RECV_TRIG_480            480


#define HW_SPI_TX_FIFO_LENGTH 512
#define HW_SPI_RX_FIFO_LENGTH 512


uint16 HW_SPI_Read_Data(uint16 address);
void HW_SPI_Write_Data(uint16 address, uint16 data);


void HW_SPI_Empty_FIFO(void);
	

/*
 * Line status LSR
 */

#define XR7_LSR_TEMT                0x40
#define XR7_LSR_THRE                0x20
#define XR7_LSR_BI                  0x10
#define XR7_LSR_FE                  0x08
#define XR7_LSR_PE                  0x04
#define XR7_LSR_OE                  0x02
#define XR7_LSR_DR                  0x01

#define XR7_LSR_intDR               0x100



extern void dma_sram_delay(unsigned long delay);

void HW_SPI_Initialise(HAL_SPI_ID_T id);
void HW_SPI_Tx_Block(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen);
void HW_SPI_Write_Data(uint16 address, uint16 data);

void HW_SPI_Tx_DMA(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen);

void HW_SPI_Tx_DMA_32bit(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen);

void lcd_dma_refresh(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color);



