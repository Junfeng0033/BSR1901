#include "platform_config.h"

//#define AHB_DMA_BASE                     				0x4001E000
//0x4000_6000

#define AHB_DMA_BASE                     				0x40006000
//| Slv 7 | UART3         										| 0x4000_6000 - 0x4000_6fff

//#define GEK_DMA_ENABLE             0
#define GEK_DMA_ENABLE             1


#define AHB_DMA_CONTROL_REG                     (AHB_DMA_BASE + 0x00)
#define AHB_DMA_SRCADDR_REG                     (AHB_DMA_BASE + 0x04)
#define AHB_DMA_DESTADDR_REG                    (AHB_DMA_BASE + 0x08)
#define AHB_DMA_DATALENGTH_REG                  (AHB_DMA_BASE + 0x0C)
//#######################################################################
//New Register for GEK1109
#define AHB_DMA_DONE_STS_REG                  	(AHB_DMA_BASE + 0x10) //dma done status register
#define AHB_DMA_STEP_CTRL_REG                  	(AHB_DMA_BASE + 0x14) //dma carry data step control register


//register 0x14
#define dma_for_dac_en				  (1<<18)
#define dma_int_mask				    (1<<17)//set this bit "1" to enable DMA Interrupt.
#define dma_int_clear				    (1<<16)
//#######################################################################



//register 0x00
#define dma_buslock_req_en				    (1<<6)


/*
// Read/write 32-bit registers:
//
// Address  Read     		Write
// 0x00     DMA_CTRL    DMA_CTRL
// 0x04     SRC_ADDR    SRC_ADDR
// 0x08     DES_ADDR    DES_ADDR
// 0x1C     LENGTH    	LENGTH

//`define   DMA_CTRL_ADDR    5'b00000 
//`define   SRC_ADDR_ADDR    5'b00001 
//`define   DES_ADDR_ADDR    5'b00010 
//`define   LENGTH_ADDR      5'b00011 
//`define   DMA_DONE_STS     5'b00100
//`define   DMA_STEP_CTRL    5'b00101


// Internal write registers
  wire        DMA_CTRLEn;
  wire        SRC_ADDREn;
  wire        DES_ADDREn;
  wire        LENGTHEn;
  wire [7:0]  DMA_CTRLNext;
  wire [31:0] SRC_ADDRNext;
  wire [31:0] DES_ADDRNext;
  wire [09:0]	LENGTHNext;
  reg  [7:0]  DMA_CTRL;
  reg  [31:0] SRC_ADDR;
  reg  [31:0] DES_ADDR;
  reg  [09:0] LENGTH;
	
	
  wire        fix_src;
  wire        fix_des;
  wire        dma_en;
  wire [2:0]  hsizem;//hsizem=3'h0:byte;hsizem=3'h1:2-byte;hsizem=any else:4-byte;
  wire        cycle2_3;// 0: 3 cycle; 1: 2 cycle

  assign      fix_src = DMA_CTRL[0];
  assign      fix_des = DMA_CTRL[1];
  assign      hsizem  = DMA_CTRL[4:2];

  assign dma_en  = dma_en_reg!=dma_en_reg_d;
  assign cycle2_3 = DMA_CTRL[5];

  assign dma_busreq_lock_en = DMA_CTRL[6];
*/


#define AHB_DMA_CONTROL_SRC_INC_DES_INC         0
#define AHB_DMA_CONTROL_SRC_NOINC_DES_INC       1
#define AHB_DMA_CONTROL_SRC_INC_DES_NOINC       2
#define AHB_DMA_CONTROL_SRC_NOINC_DES_NOINC     3

#define AHB_DMA_CONTROL_WORD_TR                 0x08
#define AHB_DMA_CONTROL_HWORD_TR                0x04
#define AHB_DMA_CONTROL_BYTE_TR                 0x00

#define GECKO_DMA_MIN_SIZE                   	(16)



#define DMA_READ_REG(REG)      									(*(volatile uint32*)(REG))
#define DMA_WRITE_REG(REG,VAL)    							(*(volatile uint32*)(REG)) = (uint32)(VAL) 


/*
typedef struct
{
	uint32_t DMA_Channel;
	uint32_t DMA_PeripheralBaseAddr;
	uint32_t DMA_MemoryBaseAddr;
	uint32_t DMA_DIR;
	uint32_t DMA_PeripheralDataSize;
	uint32_t DMA_MemoryDataSize;	
	uint32_t DMA_Mode;	
	uint32_t DMA_Priority;	
	uint32_t DMA_FIFOMode;	
	uint32_t DMA_FIFOThreshold;
	uint32_t DMA_MemoryBurst;			
	uint32_t DMA_PeripheralBurst;		
} DMA_InitTypeDef;
*/

void Gecko_DMA_Transport(volatile void *dest,volatile const void *src, uint16 size,uint8 type, uint8 direct);
void DMA_Configuration(void);


