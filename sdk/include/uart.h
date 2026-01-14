
#include "platform_config.h"

/****************************************************************************************************

//UART PART REGISTER

*****************************************************************************************************/

#define GECKO_16550_RECV_FIFO_TRIGGER   1

//#define GECKO_16550_RECV_FIFO_TRIGGER 	4
#define GECKO_16550_TX_FIFO_LENGTH 			1
		
#define UART_RX_FIFO_TRIGGER(n)             (((n) & 0x1FF) << 0)
#define UART_TX_FIFO_TRIGGER(n)             (((n) & 0x1FF) << 9)


//`define ADDR_BASE_AHB_UART0      32'h4000_1000
//`define ADDR_BASE_AHB_UART1      32'h4000_3000
//`define ADDR_BASE_AHB_UART2      32'h4000_4000

//define AHB uart base address
#define 	 GECKO_AHB_UART_0_BASE  								0x40001000

#define	   GECKO_AHB_UART_1_BASE			        		0x40003000 //New UART --->2022-09-04

#define	   GECKO_AHB_UART_2_BASE			        		0x40004000

#define REG_UART0_BASE               0x40001000
#define REG_UART1_BASE               0x40003000//New UART
#define REG_UART2_BASE               0x40004000

typedef volatile struct
{
    REG32                          RBRTHR;                  //0x00000000
    REG32                          IER;                     //0x00000004
    REG32                          FCR;                  		//0x00000008
    REG32                          LCR;                     //0x0000000C
    REG32                          MCR;                     //0x00000010
    REG32                          LSR;                     //0x00000014
    REG32                          MSR;                     //0x00000018
} HWP_UART_T;

#define hwp_uart0                    ((HWP_UART_T*) REG_UART0_BASE)
#define hwp_uart1                    ((HWP_UART_T*) REG_UART1_BASE)
#define hwp_uart2                   ((HWP_UART_T*) REG_UART2_BASE)

// Array of pointers towards the UART HW modules
//HWP_UART_T* g_halUartHwpArray[3] = {hwp_uart0,hwp_uart1,  hwp_uart2};


// Simple send byte / get byte Macros
//#define hal_SendByte(hwp_uart0, byte_to_send) hwp_uart0->rxtx_buffer = byte_to_send;
//#define hal_GetByte(hwp_uart0) hwp_uart0->rxtx_buffer

#define hal_SendByte(hwp_uart0, byte_to_send) hwp_uart0->RBRTHR = byte_to_send;
#define hal_GetByte(hwp_uart0) hwp_uart0->RBRTHR

//#define hal_SendByte(hwp_uart1, byte_to_send) hwp_uart1->RBRTHR = byte_to_send;
//#define hal_GetByte(hwp_uart1) hwp_uart1->RBRTHR

//#define hal_SendByte(hwp_uart2, byte_to_send) hwp_uart2->RBRTHR = byte_to_send;
//#define hal_GetByte(hwp_uart2) hwp_uart2->RBRTHR


/// Uart 0 is the trace uart and is unavailable for this driver
/// The numbering starts at 1 for consistency.
/// The HAL_UART_QTY value is defined as the number of UARTS
/// avalaible for the chip on which the driver is running, and
/// can therefore be used for consistency checks
typedef enum {
    HAL_UART_0                                  = 0x00000000,
    HAL_UART_1                                  = 0x00000001,//New UART--->2022-09-04	
    HAL_UART_2                                  = 0x00000002,
    HAL_UART_QTY
} HAL_UART_ID_T;   






void Gecko_Uart_Tx_Data(HAL_UART_ID_T id,uint8 *pData, uint16 DataLen);


//uart register write

#define HWreg_UART_0_SET_REG(offset,x)          						(*(volatile uint32*)(GECKO_AHB_UART_0_BASE + (offset))) = (uint32)(x)
#define HWreg_UART_0_GET_REG(x)            									(*(volatile uint32*)(GECKO_AHB_UART_0_BASE + (x)))

#define HWreg_UART_0_GETBYTE_REG(x)        									(*(volatile uint8*)(GECKO_AHB_UART_0_BASE + (x)))
#define HWreg_UART_0_SETBYTE_REG(r,x)      									(*(volatile uint8*)(GECKO_AHB_UART_0_BASE + (r))) = (uint8)(x)


#define HWreg_UART_1_SET_REG(offset,x)          						(*(volatile uint32*)(GECKO_AHB_UART_1_BASE + (offset))) = (uint32)(x)
#define HWreg_UART_1_GET_REG(x)            									(*(volatile uint32*)(GECKO_AHB_UART_1_BASE + (x)))

#define HWreg_UART_1_GETBYTE_REG(x)        									(*(volatile uint8*)(GECKO_AHB_UART_1_BASE + (x)))
#define HWreg_UART_1_SETBYTE_REG(r,x)      									(*(volatile uint8*)(GECKO_AHB_UART_1_BASE + (r))) = (uint8)(x)


#define HWreg_UART_2_SET_REG(offset,x)          						(*(volatile uint32*)(GECKO_AHB_UART_2_BASE + (offset))) = (uint32)(x)
#define HWreg_UART_2_GET_REG(x)            									(*(volatile uint32*)(GECKO_AHB_UART_2_BASE + (x)))

#define HWreg_UART_2_GETBYTE_REG(x)        									(*(volatile uint8*)(GECKO_AHB_UART_2_BASE + (x)))
#define HWreg_UART_2_SETBYTE_REG(r,x)      									(*(volatile uint8*)(GECKO_AHB_UART_2_BASE + (r))) = (uint8)(x)



/*---------------------------------------------------------------------------------------------------------*/
/* Define UART Buffer Managerment Stucture                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct _BT_UART_ 
{ 
	uint8_t *st;
	uint8_t *in;
	uint8_t *out;
	uint8_t *end;
} bt_uart_ptr;

static volatile uint8 *uart_rx_buffer;
static uint16 uart_rx_length;

#define RXBUFSIZE 64





#define XR7_HCIT_DLL            	0x00
#define XR7_HCIT_DLH            	0x04 /* Divisor latch high register */
#define XR7_HCIT_DL2            	0x28 /* Baud Rate adjust register */




#define XR7_DEC_CNT             	0x4c
/*
XR7_DEC_CNT
register NOT existence
2022-01-21 Add Comments by Zhou
*/



// Offsets of registers
#define GECKO_UART_RBRTHR         0x00
#define GECKO_UART_IER            0x04
#define GECKO_UART_IIR            0x08
#define GECKO_UART_FCR            0x08
#define GECKO_UART_LCR            0x0C
#define GECKO_UART_MCR            0x10
#define GECKO_UART_LSR            0x14
#define GECKO_UART_MSR            0x18

#define XR7_UART_DLL            0x00
/* FIFO trigger register */
#define GECKO_UART_FRR            0x24



/*
 * NCO is common to all three UARTs MSB of divisor latch is common, LSB is individual to each UART
 */

#define XR7_COMMON_DLM  0x04
#define XR7_COMMON_LCR  0x0C




/////////////////////////////////////////////////
/////////////////////////////////////////////////




/********************************************************************/

//0x12<<2
//6bits

#define GECKO_UART_SOUT_CFG     0x48

//bit5 and bit4
#define GECKO_UART_SOUT_CFG_1	  0x20 //set cfg[1]=1
#define GECKO_UART_SOUT_CFG_0	  0x10 //set cfg[0]=1


/*********************************************************************/




/*
 * Interrupt Enable Register Bits
 */

#define GECKO_IER_ERBFI           0x01
#define GECKO_IER_ETBEI           0x02
#define GECKO_IER_ELSI            0x04
#define GECKO_IER_EDSSI           0x08
#define GECKO_IER_ETOUT           0x10
#define GECKO_IER_PTIME           0x80



/*
 * Interrupt ID Register bits
 */

#define GECKO_IIR_INT_NOT_PENDING 0x01
#define GECKO_IIR_MODEM           0x00
#define GECKO_IIR_THRE            0x02
#define GECKO_IIR_RECVBUF         0x04
#define GECKO_IIR_RLS             0x06
#define GECKO_IIR_TIMEOUT         0x0c
#define GECKO_IIR_ID_MASK         0x0F


/*
 * FIFO Control Register Bits
 */

#define GECKO_FCR_ENABLE              0x1
#define GECKO_FCR_RX_FIFO_RESET       0x2
#define GECKO_FCR_TX_FIFO_RESET       0x4

#define GECKO_FCR_RECV_TRIGL_14       0x40
#define GECKO_FCR_RECV_TRIGM_14       0x80

#define GECKO_FCR_RECV_TRIGL_8        0x40
#define GECKO_FCR_RECV_TRIGM_8        0x00

#define GECKO_FCR_RECV_TRIGL_4        0x00
#define GECKO_FCR_RECV_TRIGM_4        0x80

#define GECKO_FCR_RECV_TRIGL_1        0x00
#define GECKO_FCR_RECV_TRIGM_1        0x00
////////////////////////////////////////////////////////
#define XR7_FCR_ENABLE              0x1
#define XR7_FCR_RX_FIFO_RESET       0x2
#define XR7_FCR_TX_FIFO_RESET       0x4

#define XR7_FCR_RECV_TRIGL_14       0x40
#define XR7_FCR_RECV_TRIGM_14       0x80

#define XR7_FCR_RECV_TRIGL_8        0x40
#define XR7_FCR_RECV_TRIGM_8        0x00

#define XR7_FCR_RECV_TRIGL_4        0x00
#define XR7_FCR_RECV_TRIGM_4        0x80

#define XR7_FCR_RECV_TRIGL_1        0x00
#define XR7_FCR_RECV_TRIGM_1        0x00






/*
 * Line Control LCR
 */
#define GECKO_LCR_8DIV          0x100
#define GECKO_LCR_16DIV         0x000

#define GECKO_LCR_DLAB          0x80
#define GECKO_LCR_SB            0x40
#define GECKO_LCR_PAR_MASK      0x30
#define GECKO_LCR_PEN           0x08
#define GECKO_LCR_STB           0x04
#define GECKO_LCR_CHAR_LEN_MASK 0x03

#define GECKO_LCR_5BITS         0x0
#define GECKO_LCR_6BITS         0x2
#define GECKO_LCR_7BITS         0x1
#define GECKO_LCR_8BITS         0x3

#define GECKO_LCR_1STOP         0x0
#define GECKO_LCR_2STOP         0x4

#define GECKO_LCR_PODD          0x0
#define GECKO_LCR_PEVEN         0x10
#define GECKO_LCR_PFORCE1       0x20
#define GECKO_LCR_PFORCE0       0x30


/*
 * Modem Control MCR
 */
#define XR7_MCR_PLSEL               0x100    /* 1:48M   0:24M */
/*
XR7_MCR_PLSEL
clk_p  floating bit,  Not Connect wire, ignore this bit  
2022-01-21 Add Comments
*/

#define XR7_MCR_AUTODETECTBAUDRATE  0x80
#define XR7_MCR_MTK_CONV_EN         0x04
#define XR7_MCR_SIRE                0x40
#define XR7_MCR_AFCE                0x20
#define XR7_MCR_LOOP                0x10
#define XR7_MCR_OUT2                0x08
#define XR7_MCR_OUT1                0x04
#define XR7_MCR_RTS                 0x02
#define XR7_MCR_DTR                 0x01


/*
 * Line Status LSR
 */

#define GECKO_LSR_TEMT                0x40
#define GECKO_LSR_THRE                0x20
#define GECKO_LSR_BI                  0x10
#define GECKO_LSR_FE                  0x08
#define GECKO_LSR_PE                  0x04
#define GECKO_LSR_OE                  0x02
#define GECKO_LSR_DR                  0x01


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
 * NCO is common to all three UARTs MSB of divisor latch is common, LSB is individual to each UART
 */

#define GECKO_COMMON_DLM          0x04
#define GECKO_COMMON_LCR          0x0C

#define GECKO_FRR_TX_EMPTY_TRIGGER_BIT    9


/****************************************************************************************************
//
//UART PART REGISTER END
//
//
*****************************************************************************************************/


#if 1//XR7_HCIT_UART == 1
    #define UART_RECV_FIFO_TRIGGER    1
    #define UART_TX_FIFO_LENGTH       1
    #define UART_USE_CTSRTS                 0
#elif XR7_HCIT_UART == 2
    #define UART_RECV_FIFO_TRIGGER    1
    #define UART_TX_FIFO_LENGTH       480
    #define UART_USE_CTSRTS                 0
#else
    #error Please define XR7_HCIT_UART to be either 1 or 2
#endif




#define GEK_16550_RECV_FIFO_TRIGGER 4
#define GEK_16550_TX_FIFO_LENGTH 4
#define GEK_USE_CTSRTS 1

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



//ctrl
#define UART_ENABLE                 (1<<0)
#define UART_ENABLE_DISABLE         (0<<0)
#define UART_ENABLE_ENABLE          (1<<0)
#define UART_DATA_BITS              (1<<1)
#define UART_DATA_BITS_7_BITS       (0<<1)
#define UART_DATA_BITS_8_BITS       (1<<1)
#define UART_TX_STOP_BITS           (1<<2)
#define UART_TX_STOP_BITS_1_BIT     (0<<2)
#define UART_TX_STOP_BITS_2_BITS    (1<<2)
#define UART_PARITY_ENABLE          (1<<3)
#define UART_PARITY_ENABLE_NO       (0<<3)
#define UART_PARITY_ENABLE_YES      (1<<3)
#define UART_PARITY_SELECT(n)       (((n)&3)<<4)
#define UART_PARITY_SELECT_ODD      (0<<4)
#define UART_PARITY_SELECT_EVEN     (1<<4)
#define UART_PARITY_SELECT_SPACE    (2<<4)
#define UART_PARITY_SELECT_MARK     (3<<4)
#define UART_DIVISOR_MODE           (1<<20)
#define UART_IRDA_ENABLE            (1<<21)
#define UART_DMA_MODE               (1<<22)
#define UART_DMA_MODE_DISABLE       (0<<22)
#define UART_DMA_MODE_ENABLE        (1<<22)
#define UART_AUTO_FLOW_CONTROL      (1<<23)
#define UART_AUTO_FLOW_CONTROL_ENABLE (1<<23)
#define UART_AUTO_FLOW_CONTROL_DISABLE (0<<23)
#define UART_LOOP_BACK_MODE         (1<<24)
#define UART_RX_LOCK_ERR            (1<<25)
#define UART_RX_BREAK_LENGTH(n)     (((n)&15)<<28)

//status
#define UART_RX_FIFO_LEVEL(n)       (((n)&0x3F)<<0)
#define UART_RX_FIFO_LEVEL_MASK     (0x3F<<0)
#define UART_RX_FIFO_LEVEL_SHIFT    (0)
#define UART_TX_FIFO_SPACE(n)       (((n)&31)<<8)
#define UART_TX_FIFO_SPACE_MASK     (31<<8)
#define UART_TX_FIFO_SPACE_SHIFT    (8)
#define UART_TX_ACTIVE              (1<<14)
#define UART_RX_ACTIVE              (1<<15)
#define UART_RX_OVERFLOW_ERR        (1<<16)
#define UART_TX_OVERFLOW_ERR        (1<<17)
#define UART_RX_PARITY_ERR          (1<<18)
#define UART_RX_FRAMING_ERR         (1<<19)
#define UART_RX_BREAK_INT           (1<<20)
#define UART_DCTS                   (1<<24)
#define UART_CTS                    (1<<25)
#define UART_DTR                    (1<<28)
#define UART_CLK_ENABLED            (1<<31)

//rxtx_buffer
#define UART_RX_DATA(n)             (((n)&0xFF)<<0)
#define UART_TX_DATA(n)             (((n)&0xFF)<<0)

//irq_mask
#define UART_TX_MODEM_STATUS        (1<<0)
#define UART_RX_DATA_AVAILABLE      (1<<1)
#define UART_TX_DATA_NEEDED         (1<<2)
#define UART_RX_TIMEOUT             (1<<3)
#define UART_RX_LINE_ERR            (1<<4)
#define UART_TX_DMA_DONE            (1<<5)
#define UART_RX_DMA_DONE            (1<<6)
#define UART_RX_DMA_TIMEOUT         (1<<7)
#define UART_DTR_RISE               (1<<8)
#define UART_DTR_FALL               (1<<9)


/******************************************************************************/
/*                         Peripheral UART API                                */
/******************************************************************************/
char *my_itoa(int n);
//char *my_itoa(int n);
//void UATR0_PRINT_LOG(unsigned char *s);
//extern void UATR0_PRINT_LOG(char *s);
//extern void UATR0_PRINT_LOG(char *str);
void pDebugPrintfEX(char *format, ...);
void UART_PRINT_LOG(char *s);
void UATR0_PRINT_LOG(unsigned char *s);
void UATR1_PRINT_LOG(unsigned char *s);
void Uart_16550_Initialise(HAL_UART_ID_T id,uint32 baud_rate, uint16 uart_setting);
void Gecko_Uart_Tx_Data(HAL_UART_ID_T id,uint8 *pData, uint16 DataLen);
uint8 Switch_UartTxRx_Mode(HAL_UART_ID_T id,uint8 mode);

void Gecko_Set_BaudRate(HAL_UART_ID_T id,uint32 baud_rate);

void gecko_uart0_init(uint16 uart_setting);
void Gecko_Uart0_Send_Char(char s);

void gecko_uart1_init(uint16 uart_setting);
void Gecko_Uart1_Send_Char(char s);

void gecko_uart2_init(uint16 uart_setting);
void Gecko_Uart2_Send_Char(char s);


//#define hal_SendByte(hwp_uart0, byte_to_send) hwp_uart0->RBRTHR = byte_to_send;
//#define hal_GetByte(hwp_uart0) hwp_uart0->RBRTHR

//#define hal_SendByte(hwp_uart1, byte_to_send) hwp_uart1->RBRTHR = byte_to_send;
//#define hal_GetByte(hwp_uart1) hwp_uart1->RBRTHR

//#define hal_SendByte(hwp_uart2, byte_to_send) hwp_uart2->RBRTHR = byte_to_send;
//#define hal_GetByte(hwp_uart2) hwp_uart2->RBRTHR




void uart_tx_test(void);

void Gecko_Uart_Restart(void);
void Gecko_Uart_Shutdown(void);

void bsr1901_product_information(void);
//void Gecko_16550_Tx_Char_Polled(volatile uint8 **buf, volatile uint32 *length, uint8 flag);
void Gecko_16550_DMA_Tx_Char(uint16 *pData, uint16 DataLen);



