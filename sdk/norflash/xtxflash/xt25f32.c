//######################################################################
//  Description : Gecko SoC Project
//                Firmware TestCase of QSPI Flash Controller
//######################################################################

#include "dpi_common.h"
#include "bsr1901.h"
#include "timer.h"
#include "uart.h"

#include "norflash.h"
#include "aon.h"

extern uint32 pEnterCriticalSection (void);
extern void pExitCriticalSection (uint32 status);

extern void NVIC_DisableIRQ(IRQn_Type IRQn);
extern void NVIC_EnableIRQ(IRQn_Type IRQn);


//extern void Gecko_Timer_Setting(uint8 TIM_IDX);
//extern void Gecko_Timer_Init(void);




#define TB_CPU_BEHAVIOR_CTRL            0
#define TB_ARM_CORTEXM_MACRO            1

//------------------------------------------------------------------
//  QSPI Flash control macro definition
//------------------------------------------------------------------
#define ADDR_BASE_GECKO_QSPI_ACCESS     0x10000000
#define ADDR_BASE_GECKO_QSPI_CONFIG     0x40002000

#define ADDR_BASE_GECKO_CONFIG          0x40020000
#define ADDR_BASE_GECKO_AON_CONFIG      0x40020000
#define ADDR_BASE_GECKO_TB_CONTROL      0x40020100

#define ADDR_QSPI_CFG_VER_ID            ADDR_BASE_GECKO_QSPI_CONFIG + 0x00
#define ADDR_QSPI_CFG_TRANS_FORMAT      ADDR_BASE_GECKO_QSPI_CONFIG + 0x10
#define ADDR_QSPI_CFG_DIRECT_IO         ADDR_BASE_GECKO_QSPI_CONFIG + 0x14
#define ADDR_QSPI_CFG_TRANS_CTRL        ADDR_BASE_GECKO_QSPI_CONFIG + 0x20
#define ADDR_QSPI_CFG_SPI_CMD           ADDR_BASE_GECKO_QSPI_CONFIG + 0x24
#define ADDR_QSPI_CFG_SPI_ADDR          ADDR_BASE_GECKO_QSPI_CONFIG + 0x28
#define ADDR_QSPI_CFG_SPI_DATA          ADDR_BASE_GECKO_QSPI_CONFIG + 0x2C
#define ADDR_QSPI_CFG_SPI_CTRL          ADDR_BASE_GECKO_QSPI_CONFIG + 0x30
#define ADDR_QSPI_CFG_SPI_STATUS        ADDR_BASE_GECKO_QSPI_CONFIG + 0x34
#define ADDR_QSPI_CFG_INT_ENABLE        ADDR_BASE_GECKO_QSPI_CONFIG + 0x38
#define ADDR_QSPI_CFG_INT_STATUS        ADDR_BASE_GECKO_QSPI_CONFIG + 0x3C
#define ADDR_QSPI_CFG_SPIIF_TIMING      ADDR_BASE_GECKO_QSPI_CONFIG + 0x40
#define ADDR_QSPI_CFG_MEM_ACCESS        ADDR_BASE_GECKO_QSPI_CONFIG + 0x50

//  ADDR_QSPI_CFG_SPI_CMD bit offset
#define OFFSET_TRANS_FORMAT_ADDR_LEN    16
#define OFFSET_TRANS_FORMAT_DATA_LEN    8

//  ADDR_QSPI_CFG_TRANS_CTRL bit offset
#define OFFSET_SPI_CTRL_CMD_EN          30
#define OFFSET_SPI_CTRL_ADDR_EN         29
#define OFFSET_SPI_CTRL_ADDR_FMT        28
#define OFFSET_SPI_CTRL_TRANS_MODE      24
#define OFFSET_SPI_CTRL_DUAL_QUAD       22
#define OFFSET_SPI_CTRL_TOKEN_EN        21
#define OFFSET_SPI_CTRL_WRTRAN_CNT      12
#define OFFSET_SPI_CTRL_TOKEN_VALUE     11
#define OFFSET_SPI_CTRL_DUMMY_CNT       9
#define OFFSET_SPI_CTRL_RDTRAN_CNT      0

//  ADDR_QSPI_CFG_SPI_CTRL bit offset
#define OFFSET_SPI_CTRL_TX_THRES        16
#define OFFSET_SPI_CTRL_RX_THRES        8
#define OFFSET_SPI_CTRL_TX_DMA_EN       4
#define OFFSET_SPI_CTRL_RX_DMA_EN       3
#define OFFSET_SPI_CTRL_TX_FIFO_RST     2
#define OFFSET_SPI_CTRL_RX_FIFO_RST     1
#define OFFSET_SPI_CTRL_SPI_RST         0

//  ADDR_QSPI_CFG_SPI_STATUS bit offset
#define OFFSET_SPI_STATUS_TX_FULL       23
#define OFFSET_SPI_STATUS_TX_EMPTY      22
#define OFFSET_SPI_STATUS_TX_NUM        16
#define OFFSET_SPI_STATUS_RX_FULL       15
#define OFFSET_SPI_STATUS_RX_EMPTY      14
#define OFFSET_SPI_STATUS_RX_NUM        8
#define OFFSET_SPI_STATUS_SPI_ACTIVE    0

//  ADDR_QSPI_CFG_SPIIF_TIMING bit offset
//  `define ATCAHB2SPI200_CS2CLK_DEFAULT   3'h0
//  `define ATCAHB2SPI200_CSHT_DEFAULT     3'h2
//  `define ATCAHB2SPI200_SCLKDIV_DEFAULT  8'h1


//  ADDR_QSPI_CFG_INT_ENABLE / ADDR_QSPI_CFG_INT_STATUS interrupt bit assignment
#define OFFSET_QSPI_SLV_CMD             5
#define OFFSET_QSPI_END_INT             4
#define OFFSET_QSPI_TX_FIFO_INT         3
#define OFFSET_QSPI_RX_FIFO_INT         2
#define OFFSET_QSPI_TX_FIFOUR_INT       1
#define OFFSET_QSPI_RX_FIFOOR_INT       0

//  manufacturerID = 0xE
//  did1 = { electronic_signature, manufacturerID } = 0x140E
#define XTX_XT25F16_DEVICE_ID           0xE140E

//  XTX QSPI Flash Command
#define XTX_QSPI_CMD_WR_STATUS          0x01
#define XTX_QSPI_CMD_PAGE_PROG          0x02
#define XTX_QSPI_CMD_RD_DATA            0x03
#define XTX_QSPI_CMD_WR_DIS             0x04
#define XTX_QSPI_CMD_RD_STATUS          0x05
#define XTX_QSPI_CMD_WR_ENA             0x06
#define XTX_QSPI_CMD_RD_STATUS_1        0x35
#define XTX_QSPI_CMD_DEVICE_ID          0x90
#define XTX_QSPI_CMD_SE                 0x20
#define XTX_QSPI_CMD_CE                 0x60
#define XTX_QSPI_CMD_QUAD_FAST_RD       0x6B

//  ATC SPI transaction type
#define ATCSPI_SPI_TRANS_WR_ONLY        0x1
#define ATCSPI_SPI_TRANS_RD_ONLY        0x2

//#define ATCSPI_SPI_TRANS_WR_DUMMY_RD    0x5

#define ATCSPI_SPI_TRANS_NONE_DATA      0x7

//#define ATCSPI_SPI_TRANS_DUMMY_WR       0x8
//------------------------------------------------------------------


//------------------------------------------------------------------
//  Analog control macro definition
//------------------------------------------------------------------
#define OFFSET_REG_AON_AON_FSM_CTRL                 0x000
#define OFFSET_REG_AON_AON_STATUS                   0x004
#define OFFSET_REG_AON_AON_SLEEP_CNT                0x008
#define OFFSET_REG_AON_AON_SLEEP_CNT_RBK            0x00C
#define OFFSET_REG_AON_ANA_CTRL_1                   0x020
#define OFFSET_REG_AON_ANA_CTRL_2                   0x024

//#define ADDR_AON_CFG_AON_FSM_CTRL                   ADDR_BASE_GECKO_AON_CONFIG + OFFSET_REG_AON_AON_FSM_CTRL
//#define ADDR_AON_CFG_AON_STATUS                     ADDR_BASE_GECKO_AON_CONFIG + OFFSET_REG_AON_AON_STATUS
//#define ADDR_AON_CFG_AON_SLEEP_CNT                  ADDR_BASE_GECKO_AON_CONFIG + OFFSET_REG_AON_AON_SLEEP_CNT
//#define ADDR_AON_CFG_AON_SLEEP_CNT_RBK              ADDR_BASE_GECKO_AON_CONFIG + OFFSET_REG_AON_AON_SLEEP_CNT_RBK
//#define ADDR_AON_CFG_ANA_CTRL_1                     ADDR_BASE_GECKO_AON_CONFIG + OFFSET_REG_AON_ANA_CTRL_1
//#define ADDR_AON_CFG_ANA_CTRL_2                     ADDR_BASE_GECKO_AON_CONFIG + OFFSET_REG_AON_ANA_CTRL_2

//  ADDR_AON_CFG_AON_FSM_CTRL bit offset
#define OFFSET_AON_FSM_CTRL_SLEEP_CNT_RUN           8
#define OFFSET_AON_FSM_CTRL_SLEEP_CNT_WKUP          7
#define OFFSET_AON_FSM_CTRL_ADC_WAKEUP              6
#define OFFSET_AON_FSM_CTRL_PIN_WAKEUP              5
#define OFFSET_AON_FSM_CTRL_ENA_WAKEUP_INT          4
#define OFFSET_AON_FSM_CTRL_ENA_LITESLEEP           3
#define OFFSET_AON_FSM_CTRL_ENA_DEEPSLEEP           2
#define OFFSET_AON_FSM_CTRL_WAIT_PU_CNTTO           0

//  Gofee Gecko specific config registers (CMU)
#define ADDR_BASE_GECKO_CMU                         0x40010000
#define ADDR_REG_GECKO_CMU_EX_CONFIG                ADDR_BASE_GECKO_CMU + 0x1C
#define ADDR_REG_GECKO_CMU2AHB_WR_ADDR              ADDR_BASE_GECKO_CMU + 0x20
#define ADDR_REG_GECKO_CMU2AHB_WR_DATA              ADDR_BASE_GECKO_CMU + 0x24

#define OFFSET_BIT_GECKO_CMU2AHB_WR_REQ             0


//  Gofee Gecko TestCase ONLY
#define ADDR_TEST_TB_CONTROL_REG_CMD                ADDR_BASE_GECKO_TB_CONTROL + 0x10
#define ADDR_TEST_TB_CONTROL_SLEEP_CMD              ADDR_BASE_GECKO_TB_CONTROL + 0x14
#define ADDR_TEST_TB_CONTROL_DEBUG_CMD              ADDR_BASE_GECKO_TB_CONTROL + 0x00
#define ADDR_TEST_TB_CONTROL_DEBUG_ADDR             ADDR_BASE_GECKO_TB_CONTROL + 0x04
#define ADDR_TEST_TB_CONTROL_DEBUG_DATA             ADDR_BASE_GECKO_TB_CONTROL + 0x08

#define ADDR_TEST_REGISTER_00                       ADDR_BASE_GECKO_AON_CONFIG + 0x100
#define ADDR_TEST_REGISTER_01                       ADDR_BASE_GECKO_AON_CONFIG + 0x104
#define ADDR_TEST_REGISTER_02                       ADDR_BASE_GECKO_AON_CONFIG + 0x108
#define ADDR_TEST_REGISTER_03                       ADDR_BASE_GECKO_AON_CONFIG + 0x10C
//------------------------------------------------------------------


//------------------------------------------------------------------
//  TestCase macro definition
//------------------------------------------------------------------
#define FW_TEST_MACRO_AHB_RD_CMD                    0x00A1
#define FW_TEST_MACRO_AHB_WR_CMD                    0x00A2
#define FW_TEST_MACRO_COMPARE_FAIL                  0x00F1

#define FW_TEST_MACRO_CHECK_PASS_01                 0xC0A1
#define FW_TEST_MACRO_CHECK_PASS_02                 0xC0A2
#define FW_TEST_MACRO_CHECK_PASS_03                 0xC0A3
#define FW_TEST_MACRO_CHECK_PASS_04                 0xC0A4

#define FW_TEST_MACRO_CHECK_INT_01                  0xC0C1
#define FW_TEST_MACRO_CHECK_INT_02                  0xC0C2
#define FW_TEST_MACRO_CHECK_INT_03                  0xC0C3
#define FW_TEST_MACRO_CHECK_INT_04                  0xC0C4
#define FW_TEST_MACRO_CHECK_INT_0F                  0xC0CF

#define FW_TEST_MACRO_TESTCASE_DONE                 0xCC00
#define FW_TEST_MACRO_TESTCASE_PASS                 0xCCCC
#define FW_TEST_MACRO_TESTCASE_FAIL                 0xCCFF
//------------------------------------------------------------------

void gecko_ahb2qspi_ctrl_irq_handler(void);
void gecko_aon_wakeup_irq_handler(void);

//const static unsigned char raw_data1[] = {
static unsigned char raw_data1[] = {
    0x11,0x22,0x33,0x44,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};

/*
static unsigned char raw_data2[] = {
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
    0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00
};
*/

void manba_task_sys_debug (unsigned int debug_msg_cmd, unsigned int debug_msg_addr, unsigned int debug_msg_data)
{
    reg_write(ADDR_TEST_TB_CONTROL_DEBUG_CMD,  debug_msg_cmd);
    reg_write(ADDR_TEST_TB_CONTROL_DEBUG_ADDR, debug_msg_addr);
    reg_write(ADDR_TEST_TB_CONTROL_DEBUG_DATA, debug_msg_data);
}

void manba_task_cpu_goto_sleep ()
{
    #if TB_CPU_BEHAVIOR_CTRL //  Use TestBench behavioral control
    reg_write(ADDR_TEST_TB_CONTROL_SLEEP_CMD, 0x1);
    #endif
    #if TB_ARM_CORTEXM_MACRO //  Use TestBench behavioral control
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // Select deep sleep mode
      __wfi();
    #endif
}

void manba_task_clr_aon_wakeup_int ()
{
    unsigned int ahb_rd_data;
    ahb_rd_data = reg_read(ADDR_AON_CFG_AON_STATUS) & 0x1;
    reg_write(ADDR_AON_CFG_AON_STATUS, 0x1); // Clear AON wake-up interrupt
    if (ahb_rd_data)
    {
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_CHECK_PASS_01, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: [tc_gecko_aon_sanity] [Good] AON wake-up interrupt generated.\n");
    #endif
    }
}
//------------------------------------------------------------------


//  Gecko SoC IRQ Handle
void gecko_aon_wakeup_irq_handler(void)
{
	
#if 0//1108
		//char *string;				
	
    reg_write(ADDR_AON_CFG_AON_STATUS, 0x1); // Write 1 to clear AON wake-up Interrupt
    reg_write(ADDR_AON_CFG_AON_FSM_CTRL, 0x3); // Restore the default value
	
    reg_write(ADDR_AON_CFG_AON_GPIO_WAKEUP, 0x0);//Clear GPIO WAKEUP	
	
	  //delay_1us(1000);
		//UATR0_PRINT_LOG((unsigned char *)("gecko_aon_wakeup_irq_handler --- Done"));
		//UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
	
#else//1109
	
	  reg_write(GECKO_AON_BASE_ADDR+0x008, 0x1);//0x008, reg_aon_clr_wakeup_int
	
#endif
}

void gecko_ahb2qspi_ctrl_irq_handler(void)
{
    unsigned int  ahb_rd_data;
    unsigned int  ahb_wr_data;
    unsigned int  status_end_int;
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_INT_STATUS);
    status_end_int = ahb_rd_data & (0x1<<OFFSET_QSPI_END_INT);
    ahb_wr_data    = ahb_rd_data | (0x1<<OFFSET_QSPI_END_INT);
    if (status_end_int==(0x1<<OFFSET_QSPI_END_INT))
    {
        reg_write(ADDR_QSPI_CFG_INT_STATUS, ahb_wr_data); // Write 1 to clear SPI EndInt
    }
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(0xCAFE00A1, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
		//UATR0_PRINT_LOG((unsigned char *)("\r\n"));
		//UATR0_PRINT_LOG((unsigned char *)("gecko_ahb2qspi_ctrl_irq_handler --- Done\n"));
}
//------------------------------------------------------------------



//------------------------------------------------------------------
//  ZGF TBD:
//  1. TokenEn, TokenValue in ADDR_QSPI_CFG_TRANS_CTRL
//  2. WrTranCnt in ADDR_QSPI_CFG_TRANS_CTRL
//------------------------------------------------------------------


//void qspi_task_wait_endint(unsigned char wait_period, unsigned char timeout)
void qspi_task_wait_endint(unsigned char wait_period, unsigned char timeout)
{
    unsigned int  ahb_rd_data;
    unsigned char timeout_cnt=0;
    unsigned char wait_done=0;
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_INT_STATUS) & (0x1<<OFFSET_QSPI_END_INT);
    while (wait_done==0)
    {
        if (wait_period)
        {
            delay_1us(5);
        }
        else
        {
            delay_1us(10); // default
        }
        timeout_cnt++;
        if (timeout_cnt>=timeout)
        {
            #if FW_TC_DEBUG_USE_SYS_REG
                manba_task_sys_debug(FW_TEST_MACRO_CHECK_INT_0F, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
            #endif
            #if FW_TC_DEBUG_USE_PRINTF
                printf("Gecko Test: [Warning] [Timeout] wait EndInt\n");
            #endif
            wait_done=1;
        }
        else if (ahb_rd_data==(0x1<<OFFSET_QSPI_END_INT))
        {
            #if FW_TC_DEBUG_USE_SYS_REG
                manba_task_sys_debug(FW_TEST_MACRO_CHECK_INT_01, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
            #endif
            #if FW_TC_DEBUG_USE_PRINTF
                printf("Gecko Test: [Done] EndInt status: %x .\n", ahb_rd_data);
            #endif
            reg_write(ADDR_QSPI_CFG_INT_STATUS, (0x1<<OFFSET_QSPI_END_INT)); // Clear EndInt status
            wait_done=1;
        }
        else
        {
            ahb_rd_data = reg_read(ADDR_QSPI_CFG_INT_STATUS) & (0x1<<OFFSET_QSPI_END_INT);
        }
    }
}

void qspi_task_config_trans(unsigned char trans_type, unsigned char cmd_en, unsigned char addr_en, unsigned int addr_value, unsigned char wr_cnt, unsigned char rd_cnt)
{
    unsigned int ahb_wr_data;
    unsigned int ahb_rd_data;
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_TRANS_CTRL);
    if (cmd_en)   ahb_wr_data = ahb_rd_data | (0x1<<OFFSET_SPI_CTRL_CMD_EN);
    else          ahb_wr_data = ahb_rd_data & (~(0x1<<OFFSET_SPI_CTRL_CMD_EN));
    //reg_write(ADDR_BASE_GECKO_TB_CONTROL, ahb_wr_data); // Gofee Debug Only for Firmware Simulation Without Print
    if (addr_en)
    {
        ahb_wr_data = ahb_wr_data | (0x1<<OFFSET_SPI_CTRL_ADDR_EN);
        //reg_write(ADDR_BASE_GECKO_TB_CONTROL, ahb_wr_data); // Gofee Debug Only for Firmware Simulation Without Print
    }
    else
    {
        ahb_wr_data = ahb_wr_data & (~(0x1<<OFFSET_SPI_CTRL_ADDR_EN));
    }

    ahb_wr_data = ahb_wr_data & (~((0xF       <<OFFSET_SPI_CTRL_TRANS_MODE) | (0x1FF <<OFFSET_SPI_CTRL_WRTRAN_CNT) | (0x1FF <<OFFSET_SPI_CTRL_RDTRAN_CNT)));
    ahb_wr_data = ahb_wr_data |    (trans_type<<OFFSET_SPI_CTRL_TRANS_MODE) | (wr_cnt<<OFFSET_SPI_CTRL_WRTRAN_CNT) | (rd_cnt<<OFFSET_SPI_CTRL_RDTRAN_CNT);
    reg_write(ADDR_QSPI_CFG_TRANS_CTRL, ahb_wr_data);
    //reg_write(ADDR_BASE_GECKO_TB_CONTROL, ahb_wr_data); // Gofee Debug Only for Firmware Simulation Without Print

    reg_write(ADDR_QSPI_CFG_SPI_ADDR, addr_value); // DUMMY address for XTX Flash Device ID
}

void qspi_task_reset_rxfifo()
{
    unsigned int ahb_wr_data;
    unsigned int ahb_rd_data;
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_SPI_CTRL);
    ahb_wr_data = ahb_rd_data | (0x1<<OFFSET_SPI_CTRL_RX_FIFO_RST);
    reg_write(ADDR_QSPI_CFG_SPI_CTRL, ahb_wr_data);
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_SPI_CTRL);
    while(ahb_rd_data & (0x1<<OFFSET_SPI_CTRL_RX_FIFO_RST))
    {
        ahb_rd_data = reg_read(ADDR_QSPI_CFG_SPI_CTRL);
    }
}

void qspi_task_reset_spi()
{
    unsigned int ahb_wr_data;
    unsigned int ahb_rd_data;
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_SPI_CTRL);
    ahb_wr_data = ahb_rd_data | (0x1<<OFFSET_SPI_CTRL_SPI_RST);
    reg_write(ADDR_QSPI_CFG_SPI_CTRL, ahb_wr_data);
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_SPI_CTRL);
    while(ahb_rd_data & (0x1<<OFFSET_SPI_CTRL_SPI_RST))
    {
        ahb_rd_data = reg_read(ADDR_QSPI_CFG_SPI_CTRL);
    }
}
//------------------------------------------------------------------


void tc_qspi_to_xtx_sanity()
{
    unsigned int ahb_wr_data;
    unsigned int ahb_rd_data;
	  char *string=0;

    volatile unsigned int check_data[64];
    //unsigned char i,page_addr;

	
    ahb_rd_data =  reg_read(ADDR_BASE_GECKO_QSPI_CONFIG);
    ahb_rd_data =  reg_read(ADDR_BASE_GECKO_AON_CONFIG);
    ahb_rd_data = (reg_read(ADDR_QSPI_CFG_SPI_STATUS) & 0x1); // Bit0: SPI active
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_CHECK_PASS_01, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        if (ahb_rd_data==0x1)   printf("Gecko Test: QSPI control is BUSY writing.\n");
        else                    printf("Gecko Test: QSPI control is available for writing.\n");
    #endif

    ahb_wr_data = (reg_read(ADDR_QSPI_CFG_TRANS_FORMAT) & (~(0x1F<<OFFSET_TRANS_FORMAT_ADDR_LEN))) | (0x2<<OFFSET_TRANS_FORMAT_ADDR_LEN); // Set transfer address width to 3x byte
    reg_write(ADDR_QSPI_CFG_TRANS_FORMAT, ahb_wr_data);
    reg_write(ADDR_QSPI_CFG_INT_ENABLE, (0x1<<OFFSET_QSPI_END_INT)); // Enable the EndInt interrupt

    //----------------------------------
    //  XTX Flash read device ID
    //----------------------------------
    //  - NO change DualQuad; use default single mode
    //  - Set CmdEn=1; write command and address, then read data
    //  - Set AddrEn=1; QSPI controller default disable the address phase
    //  - Set TransMode=0x2, read with command phase and address phase
    //  - Set RdTranCnt=0x2, for 24bit device ID
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_TRANS_CTRL);
    ahb_wr_data = ahb_rd_data | (0x1<<OFFSET_SPI_CTRL_CMD_EN) | (0x1<<OFFSET_SPI_CTRL_ADDR_EN);
    ahb_wr_data = ahb_wr_data & (~((0xF<<OFFSET_SPI_CTRL_TRANS_MODE) | (0x1FF<<OFFSET_SPI_CTRL_WRTRAN_CNT) | (0x1FF<<OFFSET_SPI_CTRL_RDTRAN_CNT)));
    ahb_wr_data = ahb_wr_data |    (0x2<<OFFSET_SPI_CTRL_TRANS_MODE) | (0x0  <<OFFSET_SPI_CTRL_WRTRAN_CNT) | (0x2  <<OFFSET_SPI_CTRL_RDTRAN_CNT);
    reg_write(ADDR_QSPI_CFG_TRANS_CTRL, ahb_wr_data);
    reg_write(ADDR_QSPI_CFG_SPI_ADDR, 0x0); // DUMMY address for XTX Flash Device ID
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_DEVICE_ID); // Write QSPI command to trigger
    qspi_task_wait_endint(10, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_SPI_DATA);
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ADDR_QSPI_CFG_SPI_DATA, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read data: %x .\n", ahb_rd_data);
    #endif

	
		#if DEBUG_UATR0_PRINT_LOG
		

		//unsigned char id[3];
		//id[0]=(uint8_t)ahb_rd_data;
		//id[1]=(uint8_t)ahb_rd_data>>8;
		//id[2]=(uint8_t)ahb_rd_data>>16;
		
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));	

		//UATR0_PRINT_LOG((unsigned char *)("ADDR_QSPI_CFG_TRANS_CTRL **** ahb_wr_data:"));
		//string=my_itoa(ahb_wr_data);
		//UATR0_PRINT_LOG((unsigned char *)(string));
		
		UATR0_PRINT_LOG((unsigned char *)("flash chip id:"));
		string=my_itoa(ahb_rd_data);
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
		
    #endif


#if 0
//===========================================================================================
    //  Read-back check
    page_addr = 0x8;
    qspi_task_config_trans(ATCSPI_SPI_TRANS_RD_ONLY, 0x1, 0x1, page_addr, 0, 32); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_DATA); // Write QSPI command to trigger
    qspi_task_wait_endint(20, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    for (i=0; i<32; i++)
    {
        check_data[i] = reg_read(ADDR_QSPI_CFG_SPI_DATA);
			
				#if 0//DEBUG_UATR0_PRINT_LOG
				UATR0_PRINT_LOG((unsigned char *)("\r\n****i = 0x"));
				string=my_itoa(i);
				UATR0_PRINT_LOG((unsigned char *)(string));	
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));			
				UATR0_PRINT_LOG((unsigned char *)("Read-back check --- check_data[i] = 0x"));
				string=my_itoa(check_data[i]);
				UATR0_PRINT_LOG((unsigned char *)(string));
				UATR0_PRINT_LOG((unsigned char *)("###\r\n"));				
        #endif				
			
    }
		
				#if 0//DEBUG_UATR0_PRINT_LOG
				UATR0_PRINT_LOG((unsigned char *)("\r\n****i = 0x"));
				string=my_itoa(i);
				UATR0_PRINT_LOG((unsigned char *)(string));	
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));			
				UATR0_PRINT_LOG((unsigned char *)("Read-back check --- check_data[i] = 0x"));
				string=my_itoa(check_data[i]);
				UATR0_PRINT_LOG((unsigned char *)(string));
				UATR0_PRINT_LOG((unsigned char *)("###\r\n"));				
        #endif
//===========================================================================================
#endif


#if 0
//test lite-sleep function
		__wfi(); //add by JF

#endif


		
/*

		delay_1us(1000);
		
		int tst_dt=0x123456;
		UATR0_PRINT_LOG("TEST---print 0x123456 string =");	
		string=my_itoa(tst_dt);
		UATR0_PRINT_LOG(string);
		UATR0_PRINT_LOG("\r\n");
		
*/


    //----------------------------------

    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_TESTCASE_DONE, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: tc_qspi_to_xtx_sanity finished.\n");
    #endif
}
//------------------------------------------------------------------


void tc_qspi_to_xtx_single_wr()
{
    //unsigned int ahb_wr_data;
	  char * string;
    volatile unsigned int ahb_rd_data;
    //  unsigned int ahb_tmp_data, ahb_tmp_addr;
    unsigned int test_data[64];
    volatile unsigned int check_data[64];
    unsigned char i, j, page_addr;
    unsigned int qspi_tx_fifo_empty;

    for (i=0; i<64; i++)
    {
        test_data[i] = (((i*4)+0x3)<<24) | (((i*4)+0x2)<<16) | (((i*4)+0x1)<<8) | (((i*4)+0x0)<<0);
    }
    reg_write(ADDR_QSPI_CFG_INT_ENABLE, (0x1<<OFFSET_QSPI_END_INT)); // Enable the EndInt interrupt

    //  Write Enable
    qspi_task_config_trans(ATCSPI_SPI_TRANS_NONE_DATA, 0x1, 0x0, 0x0, 0x0, 0x0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_WR_ENA); // Write QSPI command to trigger
    qspi_task_wait_endint(1, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout

    qspi_task_config_trans(ATCSPI_SPI_TRANS_RD_ONLY, 0x1, 0x0, 0x0, 0x0, 0x0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_STATUS); // Write QSPI command to trigger
    qspi_task_wait_endint(1, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_SPI_DATA);
		
		
			  #if 1//DEBUG_UATR0_PRINT_LOG
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));		
        UATR0_PRINT_LOG((unsigned char *)("tc_qspi_to_xtx_single_wr ---READ---####111"));		
				//UATR0_PRINT_LOG((unsigned char *)("tc_qspi_to_xtx_single_wr --- qspi_tx_fifo_empty = 0x"));
				//string=my_itoa(qspi_tx_fifo_empty);
				//UATR0_PRINT_LOG((unsigned char *)(string));
				//UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
			
				//UATR0_PRINT_LOG((unsigned char *)("tc_qspi_to_xtx_single_wr --- i = 0x"));
				//string=my_itoa(i);
				//UATR0_PRINT_LOG((unsigned char *)(string));
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));				
        #endif		
		
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ADDR_QSPI_CFG_SPI_DATA, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read status register: %x .\n", ahb_rd_data);
    #endif
    //----------------------------------

    //  Prepare Write Data. Total 32bit*16 = 64 bytes; hence set QSPI ctrl wr_cnt = 64-1 = 63
    for (i=0; i<4; i++)
    {
        reg_write(ADDR_QSPI_CFG_SPI_DATA, test_data[i]);
        #if FW_TC_DEBUG_USE_PRINTF
        //  Debug Only: printf("Gecko Test: ---- test_data[%d] = %x .\n", i, test_data[i]);
        #endif
    }
    j=4;
    qspi_task_config_trans(ATCSPI_SPI_TRANS_WR_ONLY, 0x1, 0x1, 0x0, 63, 0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_PAGE_PROG); // Write QSPI command to trigger
    delay_1us(100);
    while (j<20)
    {
        delay_1us(10);
        qspi_tx_fifo_empty = reg_read(ADDR_QSPI_CFG_SPI_STATUS) & (0x1<<OFFSET_SPI_STATUS_TX_EMPTY);
			
			
			  #if 1//DEBUG_UATR0_PRINT_LOG
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));		
        UATR0_PRINT_LOG((unsigned char *)("tc_qspi_to_xtx_single_wr ---XTX_QSPI_CMD_PAGE_PROG"));		
				UATR0_PRINT_LOG((unsigned char *)("tc_qspi_to_xtx_single_wr --- qspi_tx_fifo_empty = 0x"));
				string=my_itoa(qspi_tx_fifo_empty);
				UATR0_PRINT_LOG((unsigned char *)(string));
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
			
				UATR0_PRINT_LOG((unsigned char *)("tc_qspi_to_xtx_single_wr --- j = 0x"));
				string=my_itoa(j);
				UATR0_PRINT_LOG((unsigned char *)(string));
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));				
        #endif				
			
        if (qspi_tx_fifo_empty)
        {
            #if FW_TC_DEBUG_USE_PRINTF
            //  printf("Gecko Test: Tx FIFO Empty.\n");
            #endif
            for (i=j; i<j+4; i++)
            {
                reg_write(ADDR_QSPI_CFG_SPI_DATA, test_data[i]);
                #if FW_TC_DEBUG_USE_PRINTF
                //  Debug Only: printf("Gecko Test: ---- test_data[%d] = %x .\n", i, test_data[i]);
                #endif
            }
            j=j+4;
        }
    }
    delay_1us(100);
    qspi_task_wait_endint(0, 20); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    //----------------------------------
		
		
		
			  #if 1//DEBUG_UATR0_PRINT_LOG
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));		
        UATR0_PRINT_LOG((unsigned char *)("tc_qspi_to_xtx_single_wr ---PAGE_PROG---####2222"));		
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));				
        #endif
		
		
    //  Wait for Page Programming end
    delay_1us(800); // wait 800us (> 0.7ms) for Page Programming
    qspi_task_config_trans(ATCSPI_SPI_TRANS_RD_ONLY, 0x1, 0x0, 0x0, 0, 0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_STATUS); // Write QSPI command to trigger
    qspi_task_wait_endint(1, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    ahb_rd_data = reg_read(ADDR_QSPI_CFG_SPI_DATA);
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ADDR_QSPI_CFG_SPI_DATA, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read status register: %x .\n", ahb_rd_data);
    #endif

    //  Read-back check
    page_addr = 0x8;
    qspi_task_config_trans(ATCSPI_SPI_TRANS_RD_ONLY, 0x1, 0x1, page_addr, 0, 15); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_DATA); // Write QSPI command to trigger
    qspi_task_wait_endint(20, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    for (i=0; i<4; i++)
    {
        check_data[i] = reg_read(ADDR_QSPI_CFG_SPI_DATA);
    }
		
		
			  #if 1//DEBUG_UATR0_PRINT_LOG
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));		
        UATR0_PRINT_LOG((unsigned char *)("tc_qspi_to_xtx_single_wr ---Read-back check---END####99999"));		
				UATR0_PRINT_LOG((unsigned char *)("\r\n"));				
        #endif		
		
		
    for (i=0; i<4; i++)
    {
        j = i+(page_addr/4);
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_COMPARE_FAIL, check_data[i], test_data[j]); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: ---- Index: %d; Read data: %x; Expect data: %x .\n", j, check_data[i], test_data[j]);
    #endif
    }
    //----------------------------------

    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_TESTCASE_DONE, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: tc_qspi_to_xtx_single_wr finished.\n");
    #endif
}
//------------------------------------------------------------------

/*
void tc_qspi_to_xtx_s_ahb_rd_bkup ()
{
    volatile unsigned int ahb_rd_data;
    unsigned int ahb_tmp_addr;
    unsigned int test_data[8];
    unsigned char i;

    for (i=0; i<8; i++)
    {
        test_data[i] = (((i*4)+0x3)<<24) | (((i*4)+0x2)<<16) | (((i*4)+0x1)<<8) | (((i*4)+0x0)<<0);
    }
    reg_write(ADDR_QSPI_CFG_INT_ENABLE, (0x1<<OFFSET_QSPI_END_INT)); // Enable the EndInt interrupt

    //  Write Enable
    qspi_task_config_trans(ATCSPI_SPI_TRANS_NONE_DATA, 0x1, 0x0, 0x0, 0x0, 0x0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_WR_ENA); // Write QSPI command to trigger
    qspi_task_wait_endint(1, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout

    qspi_task_config_trans(ATCSPI_SPI_TRANS_RD_ONLY, 0x1, 0x0, 0x0, 0x0, 0x0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_STATUS); // Write QSPI command to trigger
    qspi_task_wait_endint(1, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    //----------------------------------

    //  Prepare Write Data. Total 32bit*4 = 16 bytes; hence set QSPI ctrl wr_cnt = 16-1 =15
    for (i=0; i<4; i++)
    {
        reg_write(ADDR_QSPI_CFG_SPI_DATA, test_data[i]);
    }
    qspi_task_config_trans(ATCSPI_SPI_TRANS_WR_ONLY, 0x1, 0x1, 0x0, 15, 0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_PAGE_PROG); // Write QSPI command to trigger
    delay_1us(100);
    qspi_task_wait_endint(0, 20); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    //----------------------------------

    //  Wait for Page Programming end
    delay_1us(800); // wait 800us (> 0.7ms) for Page Programming
    qspi_task_config_trans(ATCSPI_SPI_TRANS_RD_ONLY, 0x1, 0x0, 0x0, 0, 0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_STATUS); // Write QSPI command to trigger
    qspi_task_wait_endint(1, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    //----------------------------------

    //  AHB read check
    qspi_task_reset_rxfifo();
    qspi_task_reset_spi();
    delay_1us(5);
    for (i=0; i<4; i++)
    {
        ahb_tmp_addr= ADDR_BASE_GECKO_QSPI_ACCESS + (i*4);
        ahb_rd_data = reg_read(ahb_tmp_addr);
        if (i==0) delay_1us(20); // first read will fill the QSPI Rx FIFO
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ahb_tmp_addr, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read addr: %x; Read data: %x .\n", ahb_tmp_addr, ahb_rd_data);
    #endif
    }
    delay_1us(100);
    //----------------------------------

    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_TESTCASE_DONE, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: tc_qspi_to_xtx_sanity finished.\n");
    #endif
}
*/

//------------------------------------------------------------------

void tc_qspi_to_xtx_dual_fast_rd ()
{
    volatile unsigned int ahb_rd_data;
    unsigned int ahb_wr_data;
    unsigned int ahb_tmp_addr;
    unsigned char i;

    //  AHB read check
    qspi_task_reset_rxfifo();
    qspi_task_reset_spi();
    delay_1us(5);

    //  SPI clock divided by 2
    //  CS2CLK_DEFAULT   3'h0
    //  CSHT_DEFAULT     3'h2
    //  SCLKDIV_DEFAULT  8'h1
    ahb_wr_data = (0x2<<8) | 0x0;
    reg_write(ADDR_QSPI_CFG_SPIIF_TIMING, ahb_wr_data);

    //  Regular read (single-wire)
    for (i=0; i<20; i++)
    {
        ahb_tmp_addr= ADDR_BASE_GECKO_QSPI_ACCESS + (i*4);
        ahb_rd_data = reg_read(ahb_tmp_addr);
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ahb_tmp_addr, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read addr: %x; Read data: %x .\n", ahb_tmp_addr, ahb_rd_data);
    #endif
    }
    delay_1us(200);
    //----------------------------------

    // SPI command 0x3B is fast dual read for XTX NOR Flash 
    // SPI command 0x3B is given by MemRdCmd=0x2 for Andes QSPI control,
    // Address 3 bytes in Regular mode + Dummy 1 byte in Regular mode + Dual-wire mode
    reg_write(ADDR_QSPI_CFG_MEM_ACCESS, 0x2);
    //----------------------------------

    //  Dual-wire read
    for (i=0; i<20; i++)
    {
        ahb_tmp_addr= ADDR_BASE_GECKO_QSPI_ACCESS + (i*4);
        ahb_rd_data = reg_read(ahb_tmp_addr);
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ahb_tmp_addr, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read addr: %x; Read data: %x .\n", ahb_tmp_addr, ahb_rd_data);
    #endif
    }
    delay_1us(100);
    //----------------------------------

    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_TESTCASE_DONE, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: tc_qspi_to_xtx_dual_fast_rd finished.\n");
    #endif
}


void tc_qspi_to_xtx_quad_fast_rd ()
{
    volatile unsigned int ahb_rd_data;
    unsigned int ahb_wr_data;
    unsigned int ahb_tmp_addr;
    unsigned char i;

    ahb_wr_data = 0x0;

    //  AHB read check
    qspi_task_reset_rxfifo();
    qspi_task_reset_spi();
    delay_1us(5);

    //  XTX status reigster
    //  | ----- | --------- | ------------- |
    //  | S15   | Reserved  | 0             |
    //  | S14   | CMP       | 0 (default)   |
    //  | S13   | Reserved  | 0             |
    //  | S12   | Reserved  | 0             |
    //  | S11   | Reserved  | 0             |
    //  | S10   | LB        | 0 (default)   |
    //  | S9    | QE        | 1 for Quad    |
    //  | S8    | Reserved  | 0             |
    //  | S7    | SRP       | 0             |
    //  | S6    | BP4       | 0 (default)   |
    //  | S5    | BP3       | 0 (default)   |
    //  | S4    | BP2       | 0 (default)   |
    //  | S3    | BP1       | 0 (default)   |
    //  | S2    | BP0       | 0 (default)   |
    //  | S1    | WEL       | 1 for Write   |
    //  | S0    | WIP       | Write status  |

    //  Write Enable
    qspi_task_config_trans(ATCSPI_SPI_TRANS_NONE_DATA, 0x1, 0x0, 0x0, 0x0, 0x0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_WR_ENA); // Write QSPI command to trigger
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: XTX NOR Flash Write Enable\n");
    #endif
    qspi_task_wait_endint(1, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout

    //  Regular read (single-wire)
    for (i=0; i<20; i++)
    {
        ahb_tmp_addr= ADDR_BASE_GECKO_QSPI_ACCESS + (i*4);
        ahb_rd_data = reg_read(ahb_tmp_addr);
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ahb_tmp_addr, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read addr: %x; Read data: %x .\n", ahb_tmp_addr, ahb_rd_data);
    #endif
    }
    delay_1us(200);
    //----------------------------------

    //  Write Status Register
    ahb_wr_data = (reg_read(ADDR_QSPI_CFG_TRANS_FORMAT) & (~(0x1F<<OFFSET_TRANS_FORMAT_ADDR_LEN))) | (0x1<<OFFSET_TRANS_FORMAT_ADDR_LEN); // Set transfer address width to 2x byte
    reg_write(ADDR_QSPI_CFG_TRANS_FORMAT, ahb_wr_data);
    ahb_wr_data = (0x1<<1); // [15:8] - [S7:S0]; [7:0] - [S15:S8]
    qspi_task_config_trans(ATCSPI_SPI_TRANS_NONE_DATA, 0x1, 0x1, ahb_wr_data, 0x0, 0x0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_WR_STATUS); // Write QSPI command to trigger
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: XTX NOR Flash Write Status Register\n");
    #endif
    qspi_task_wait_endint(1, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    ahb_wr_data = (reg_read(ADDR_QSPI_CFG_TRANS_FORMAT) & (~(0x1F<<OFFSET_TRANS_FORMAT_ADDR_LEN))) | (0x2<<OFFSET_TRANS_FORMAT_ADDR_LEN); // Set transfer address width to 3x byte
    reg_write(ADDR_QSPI_CFG_TRANS_FORMAT, ahb_wr_data);
    delay_1us(200); // Wait extra time for "write status register cycle time" in NOR Flash model

    // SPI command 0x6B is fast quad read for XTX NOR Flash 
    // SPI command 0x6B is given by MemRdCmd=0x3 for Andes QSPI control,
    // Address 3 bytes in Regular mode + Dummy 1 byte in Regular mode + Quad mode
    reg_write(ADDR_QSPI_CFG_MEM_ACCESS, 0x3);
    //----------------------------------

    //  Quad read
    for (i=0; i<20; i++)
    {
        ahb_tmp_addr= ADDR_BASE_GECKO_QSPI_ACCESS + (i*4);
        ahb_rd_data = reg_read(ahb_tmp_addr);
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ahb_tmp_addr, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read addr: %x; Read data: %x .\n", ahb_tmp_addr, ahb_rd_data);
    #endif
    }
    delay_1us(100);
    //----------------------------------

    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_TESTCASE_DONE, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: tc_qspi_to_xtx_quad_fast_rd finished.\n");
    #endif
}


void tc_qspi_to_xtx_s_ahb_rd ()
{
    volatile unsigned int ahb_rd_data;
    unsigned int ahb_tmp_addr;
    unsigned char i;

    //  AHB read check
    qspi_task_reset_rxfifo();
    qspi_task_reset_spi();
    delay_1us(5);
    for (i=0; i<10; i++)
    {
        ahb_tmp_addr= ADDR_BASE_GECKO_QSPI_ACCESS + (i*4);
        ahb_rd_data = reg_read(ahb_tmp_addr);
        //  if (i==0) delay_1us(30); // first read will fill the QSPI Rx FIFO
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ahb_tmp_addr, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read addr: %x; Read data: %x .\n", ahb_tmp_addr, ahb_rd_data);
    #endif
    }
    delay_1us(20);
    for (i=0; i<20; i++)
    {
        ahb_tmp_addr= ADDR_BASE_GECKO_QSPI_ACCESS + 0x100 + (i*4);
        ahb_rd_data = reg_read(ahb_tmp_addr);
        //  if (i==0) delay_1us(30); // first read will fill the QSPI Rx FIFO
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ahb_tmp_addr, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read addr: %x; Read data: %x .\n", ahb_tmp_addr, ahb_rd_data);
    #endif
			//UATR0_PRINT_LOG("read flash data:\n");
			//UATR0_PRINT_LOG(ahb_rd_data);
    }
    delay_1us(20);
    //----------------------------------

    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_TESTCASE_DONE, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: tc_qspi_to_xtx_sanity finished.\n");
    #endif
}
//------------------------------------------------------------------


void tc_gecko_aon_sanity ()
{
    //  unsigned int ahb_wr_data;
    //  unsigned int ahb_rd_data;

    //  delay_1us(300); // Wait for Fast-Simulation with 1MHz LP clock boot-up

        /*
    //  LightSleep with sleep counter wakeup. No wakeup interrupt.
    ahb_wr_data =   (0x1 << OFFSET_AON_FSM_CTRL_SLEEP_CNT_RUN)  |
                    (0x1 << OFFSET_AON_FSM_CTRL_SLEEP_CNT_WKUP) |
                    (0x1 << OFFSET_AON_FSM_CTRL_ENA_WAKEUP_INT) |
                    (0x1 << OFFSET_AON_FSM_CTRL_ENA_LITESLEEP)  ;
    reg_write(ADDR_AON_CFG_AON_FSM_CTRL, ahb_wr_data);
    reg_write(ADDR_AON_CFG_AON_SLEEP_CNT, 20);
    manba_task_cpu_goto_sleep();
    //----------------------------------

    //  Wait for wake-up and clear interrupt
    delay_1us(1000);
    manba_task_clr_aon_wakeup_int();
    //----------------------------------
    */

        /*
    //  Wait for wake-up and clear interrupt
    delay_1us(1000);
    manba_task_clr_aon_wakeup_int();
    //----------------------------------

    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_TESTCASE_DONE, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: tc_gecko_aon_sanity finished.\n");
    #endif
        */
}













/*

sleep_cnt_val	

"Unit: 32 x RCO32K clock cycles Max sleep counter time: (1/32kHz) * 32 * (2^24) = 4.66 hours"

*/



//reg_0x008 bit16
#define BIT16_MASK (1 << 16)
void h2l_wr_busy(void)
{
	volatile uint32 tmp=0;
	
	while (reg_read(GECKO_AON_BASE_ADDR+0x008) & BIT16_MASK){
		
		 //reg_write(ADDR_AON_RETENTION_REG_0, reg_read(GECKO_AON_BASE_ADDR+0x008));
		 //reg_write(ADDR_AON_RETENTION_REG_1, reg_read(GECKO_AON_BASE_ADDR+0x008) & BIT16_MASK);	
		
		//__asm__("nop");//if bit16 =1,wait
		  tmp++;
		  //reg_write(ADDR_AON_RETENTION_REG_3, tmp);
			if(tmp>100)
			{				
			break;
			}		
		
	}
	tmp=0;
	//bit=0, continue execute
	
	//reg_write(ADDR_AON_RETENTION_REG_0, 0x1234567);
	//reg_write(ADDR_AON_RETENTION_REG_1, 0x33557799);		
}



uint8_t OW_GetDQ(void)
{
#if 0
   if (gpio_get_value(GPIOA,2) == 0x04)
      return 1;
   else
      return 0;
#endif
      return 0;
}




void tc_gecko_cm0_aon_sleep()
{
    unsigned int ahb_wr_data;
    //unsigned int ahb_rd_data;
		//char *string;	
	
	  //delay_1us(1000);
	  //reg_write(ADDR_AON_CFG_ANA_CTRL_2, 0x1f);	
	
/***************************************************************************************/		

//		ahb_wr_data=reg_read(GECKO_AON_BASE_ADDR+0x090);
//		ahb_wr_data |= LOWPOWER_MODE_OPTM_EN;
//		reg_write(GECKO_AON_BASE_ADDR+0x090, ahb_wr_data);

//		ahb_wr_data=reg_read(GECKO_AON_BASE_ADDR+0x090);
//		ahb_wr_data &= (~LOWPOWER_MODE_OPTM_EN);
//		reg_write(GECKO_AON_BASE_ADDR+0x090, ahb_wr_data);	

		ahb_wr_data=reg_read(GECKO_AON_BASE_ADDR+0x090);
		ahb_wr_data |= LOWPOWER_MODE_OPTM_EN;
		reg_write(GECKO_AON_BASE_ADDR+0x090, ahb_wr_data);

/***************************************************************************************/	
	

    //assign reg_aon_ena_wakeup_int	=reg_0x004[0];
		ahb_wr_data=reg_read(GECKO_AON_BASE_ADDR+0x004);
		ahb_wr_data |= 0x1;
		h2l_wr_busy();
		reg_write(GECKO_AON_BASE_ADDR+0x004, ahb_wr_data);

    #if 0
    ahb_wr_data =   (0x1 << GEK1109_SLEEP_CNT_RUN_Enable)  |
                    (0x1 << GEK1109_SLEEP_CNT_WKUP_Enable) |
                    //(0x1 << OFFSET_AON_FSM_CTRL_ENA_WAKEUP_INT) |
                    (0x1 << GEK1109_DEEP_SLEEP_Enable)  ;
	                  //(0x1 << OFFSET_AON_FSM_CTRL_ENA_LITESLEEP)  ;
		#endif


		ahb_wr_data=reg_read(GECKO_AON_BASE_ADDR+0x000);
		
		ahb_wr_data = GEK1109_PIN_WAKEUP_Enable  |
								  GEK1109_DEEP_SLEEP_Enable ;

    
    //reg_write(GECKO_AON_BASE_ADDR+0x000, ahb_wr_data);
		h2l_wr_busy();
    reg_write(GECKO_AON_BASE_ADDR+0x000, 0x34);//2024-9-13 @ Wuxi	
		h2l_wr_busy();		
    reg_write(GECKO_AON_BASE_ADDR+0x060, 10000);//aon_sleep_cnt_val  2024-9-13 @ wuxi			
		
		#if 0
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
		UATR0_PRINT_LOG((unsigned char *)("tc_gecko_cm0_aon_sleep---ADDR_AON_CFG_AON_FSM_CTRL = 0x"));
		string=my_itoa(ahb_wr_data);
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
	
    //reg_write(ADDR_AON_CFG_AON_SLEEP_CNT, 30000);
		reg_write(GECKO_AON_BASE_ADDR+0x060, 100);//config "reg_aon_sleep_cnt_val" register
		UATR0_PRINT_LOG((unsigned char *)("tc_gecko_cm0_aon_sleep---ADDR_AON_CFG_AON_SLEEP_CNT = 50000"));
		#endif
	  //delay_1us(100);
    manba_task_cpu_goto_sleep();
		while(1);
}



void tc_gecko_aon_sanity_cm0()
{
    unsigned int ahb_wr_data;
    // unsigned int ahb_rd_data;

    //	delay_1us(300); // Wait for Fast-Simulation with 1MHz LP clock boot-up

		/*
    //  LightSleep with sleep counter wakeup. No wakeup interrupt.
    ahb_wr_data =   (0x1 << OFFSET_AON_FSM_CTRL_SLEEP_CNT_RUN)  |
                    (0x1 << OFFSET_AON_FSM_CTRL_SLEEP_CNT_WKUP) |
                    (0x1 << OFFSET_AON_FSM_CTRL_ENA_WAKEUP_INT) |
                    (0x1 << OFFSET_AON_FSM_CTRL_ENA_LITESLEEP)  ;
    reg_write(ADDR_AON_CFG_AON_FSM_CTRL, ahb_wr_data);
    reg_write(ADDR_AON_CFG_AON_SLEEP_CNT, 20);
    manba_task_cpu_goto_sleep();
    //----------------------------------

    //  Wait for wake-up and clear interrupt
    delay_1us(1000);
    manba_task_clr_aon_wakeup_int();
    //----------------------------------
    */

    //  LightSleep with sleep counter wakeup. No wakeup interrupt.
    ahb_wr_data =   (0x1 << OFFSET_AON_FSM_CTRL_SLEEP_CNT_RUN)  |
                    (0x1 << OFFSET_AON_FSM_CTRL_SLEEP_CNT_WKUP) |
                    (0x1 << OFFSET_AON_FSM_CTRL_ENA_WAKEUP_INT) | 
                    (0x1 << OFFSET_AON_FSM_CTRL_ENA_DEEPSLEEP)  ;
    reg_write(ADDR_AON_CFG_AON_FSM_CTRL, ahb_wr_data);
    reg_write(ADDR_AON_CFG_AON_SLEEP_CNT, 20);
    manba_task_cpu_goto_sleep();
    //----------------------------------

		/*
    //  Wait for wake-up and clear interrupt
    delay_1us(1000);
    manba_task_clr_aon_wakeup_int();
    //----------------------------------

    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_TESTCASE_DONE, 0x0, 0x0); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: tc_gecko_aon_sanity finished.\n");
    #endif
		*/
}

void gecko_task_cm0_sw_flash_quad()
{
    unsigned int ahb_wr_data;
    //  unsigned int ahb_rd_data;

    manba_task_sys_debug (0xCAFE00BB, 0x0, 0x0); // Temp Debug Only
    
    //  SPI clock divided by 2
    //  CS2CLK_DEFAULT   3'h0
    //  CSHT_DEFAULT     3'h2
    //  SCLKDIV_DEFAULT  8'h1
    ahb_wr_data = (0x2<<8) | 0x0;
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_ADDR, ADDR_QSPI_CFG_SPIIF_TIMING);
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_DATA, ahb_wr_data);
    reg_write(ADDR_REG_GECKO_CMU_EX_CONFIG, 0x1); // ahb_wr_data = reg_read(ADDR_REG_GECKO_CMU_EX_CONFIG) | 0x1;
    #if TB_ARM_CORTEXM_MACRO //  Use TestBench behavioral control
        __wfi();
    #endif
    //----------------------------------

    //  Write Enable
    qspi_task_config_trans(ATCSPI_SPI_TRANS_NONE_DATA, 0x1, 0x0, 0x0, 0x0, 0x0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_ADDR, ADDR_QSPI_CFG_SPI_CMD);
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_DATA, XTX_QSPI_CMD_WR_ENA); // Write QSPI command to trigger
    reg_write(ADDR_REG_GECKO_CMU_EX_CONFIG, 0x1); // ahb_wr_data = reg_read(ADDR_REG_GECKO_CMU_EX_CONFIG) | 0x1;
    #if TB_ARM_CORTEXM_MACRO //  Use TestBench behavioral control
        __wfi();
    #endif
    delay_1us(1);
    manba_task_sys_debug (0xCAFE00B0, 0x0, 0x0); // Temp Debug Only
    //----------------------------------

    ahb_wr_data = (reg_read(ADDR_QSPI_CFG_TRANS_FORMAT) & (~(0x1F<<OFFSET_TRANS_FORMAT_ADDR_LEN))) | (0x1<<OFFSET_TRANS_FORMAT_ADDR_LEN); // Set transfer address width to 2x byte
    reg_write(ADDR_QSPI_CFG_TRANS_FORMAT, ahb_wr_data);

    //  Write Status Register
    ahb_wr_data = (0x1<<1); // [15:8] - [S7:S0]; [7:0] - [S15:S8]
    qspi_task_config_trans(ATCSPI_SPI_TRANS_NONE_DATA, 0x1, 0x1, ahb_wr_data, 0x0, 0x0); // 1. trans_type, 2. cmd_en, 3. addr_en, 4. addr_value, 5. wr_cnt, 6. rd_cnt
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_ADDR, ADDR_QSPI_CFG_SPI_CMD);
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_DATA, XTX_QSPI_CMD_WR_STATUS); // Write QSPI command to trigger

		//  use timer-wakeup to hold the CPU
    Gecko_Timer_Init();
	  Gecko_Timer_Setting(TIMER_INDEX0);
		NVIC_DisableIRQ(CMU2AHB_IRQ);

    reg_write(ADDR_REG_GECKO_CMU_EX_CONFIG, 0x1); // ahb_wr_data = reg_read(ADDR_REG_GECKO_CMU_EX_CONFIG) | 0x1;
    
		//  Gofee TBD @ 2021.04.02 : change start here
    //  Gofee TBD @ 2021.04.02 : need use timer-wakeup to hold the CPU
    #if TB_ARM_CORTEXM_MACRO //  Use TestBench behavioral control
        __wfi();
    #endif
    manba_task_sys_debug (0xCAFE00B1, 0x0, 0x0); // Temp Debug Only
		
		NVIC_DisableIRQ(TIMER0_IRQn);
		NVIC_EnableIRQ(CMU2AHB_IRQ);
    //----------------------------------

    ahb_wr_data = (reg_read(ADDR_QSPI_CFG_TRANS_FORMAT) & (~(0x1F<<OFFSET_TRANS_FORMAT_ADDR_LEN))) | (0x2<<OFFSET_TRANS_FORMAT_ADDR_LEN); // Set transfer address width to 3x byte
    reg_write(ADDR_QSPI_CFG_TRANS_FORMAT, ahb_wr_data);
    manba_task_sys_debug (0xCAFE00B2, 0x0, 0x0); // Temp Debug Only

    //  Use SPI-to-AHB master to Quad-wire access NOR Flash
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_ADDR, ADDR_QSPI_CFG_MEM_ACCESS);
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_DATA, 0x3); // XTX QSPI command 0x6B is given by MemRdCmd=0x3 for Andes QSPI control
    reg_write(ADDR_REG_GECKO_CMU_EX_CONFIG, 0x1); // ahb_wr_data = reg_read(ADDR_REG_GECKO_CMU_EX_CONFIG) | 0x1;
    #if TB_ARM_CORTEXM_MACRO //  Use TestBench behavioral control
        __wfi();
    #endif
    //----------------------------------
}
//------------------------------------------------------------------

__RAM_CODE__ void gecko_task_cm0_sw_flash()
{
    unsigned int ahb_wr_data;
    //  unsigned int ahb_rd_data;

    //manba_task_sys_debug (0xCAFE00BB, 0x0, 0x0); // Temp Debug Only

    //  SPI clock divided by 2
    //  [13:12] CS2CLK_DEFAULT   3'h0
    //  [11: 8] CSHT_DEFAULT     3'h2 -- default
    //  [ 7: 0] SCLKDIV_DEFAULT  8'h2 -- Divided by ((SCLKDIV + 1) x 2), such as 8/2 = 4MHz
	
    //  SPI clock divided by 1
    //  CS2CLK_DEFAULT   3'h0
    //  CSHT_DEFAULT     3'h2
    //  SCLKDIV_DEFAULT  8'h1
    ahb_wr_data = (0x2<<8) | 0xff;
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_ADDR, ADDR_QSPI_CFG_SPIIF_TIMING);
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_DATA, ahb_wr_data);
    reg_write(ADDR_REG_GECKO_CMU_EX_CONFIG, 0x1); // ahb_wr_data = reg_read(ADDR_REG_GECKO_CMU_EX_CONFIG) | 0x1;
    #if TB_ARM_CORTEXM_MACRO //  Use TestBench behavioral control
        __wfi();
    #endif
    //----------------------------------

    //  SPI-to-AHB config to Dual-wire access NOR Flash
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_ADDR, ADDR_QSPI_CFG_MEM_ACCESS);
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_DATA, 0x2);
    reg_write(ADDR_REG_GECKO_CMU_EX_CONFIG, 0x1); // ahb_wr_data = reg_read(ADDR_REG_GECKO_CMU_EX_CONFIG) | 0x1;
    #if TB_ARM_CORTEXM_MACRO //  Use TestBench behavioral control
        __wfi();
    #endif
    //----------------------------------
}
//------------------------------------------------------------------



void bsr1901_prepare_sleep_for_pin_wakeup(void)
{
	unsigned int ahb_wr_data;
/*
#define GECKO_AON_BASE_ADDR             0x40020000//New for 1109
#define OFFSET_REG_AON_ANA_CTRL_1                   0x020
#define OFFSET_REG_AON_ANA_CTRL_2                   0x024	
#define ADDR_AON_CFG_ANA_CTRL_1                     GECKO_AON_BASE_ADDR + OFFSET_REG_AON_ANA_CTRL_1
#define ADDR_AON_CFG_ANA_CTRL_2                     GECKO_AON_BASE_ADDR + OFFSET_REG_AON_ANA_CTRL_2	
*/

//Recovery Frequency to default value
	reg_write(0x40020000+0x20, 0x608e7885);
	reg_write(0x40020000+0x24, 0);	


//Recovery qspi flash Frequency to default value
	
    ahb_wr_data = (0x2<<8) | 0x01;
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_ADDR, ADDR_QSPI_CFG_SPIIF_TIMING);
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_DATA, ahb_wr_data);
    reg_write(ADDR_REG_GECKO_CMU_EX_CONFIG, 0x1); 	
		__wfi();

    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_ADDR, ADDR_QSPI_CFG_MEM_ACCESS);
    reg_write(ADDR_REG_GECKO_CMU2AHB_WR_DATA, 0x0);
    reg_write(ADDR_REG_GECKO_CMU_EX_CONFIG, 0x1);		
		__wfi();		
}






//------------------------------------------------------------------
//  The new added QSPI NOR Flash operation function
//  Added by Kevin Zhou @ 2021.03.13
//------------------------------------------------------------------
__RAM_CODE__ void spi_nor_write_en(void)
{
	  unsigned int  ahb_rd_data;
	  unsigned int  ahb_wr_data;
    reg_write(ADDR_QSPI_CFG_INT_ENABLE, (0x1<<OFFSET_QSPI_END_INT));
//=============================================================================================
    //qspi_task_config_trans(ATCSPI_SPI_TRANS_NONE_DATA, 0x1, 0x0, 0x0, 0x0, 0x0);
	  ahb_wr_data = ahb_wr_data & (~((0xF<<OFFSET_SPI_CTRL_TRANS_MODE) | (0x1FF <<OFFSET_SPI_CTRL_WRTRAN_CNT) | (0x1FF <<OFFSET_SPI_CTRL_RDTRAN_CNT)));
    ahb_wr_data = (0x1<<OFFSET_SPI_CTRL_CMD_EN) | (ATCSPI_SPI_TRANS_NONE_DATA<<OFFSET_SPI_CTRL_TRANS_MODE);    
	  reg_write(ADDR_QSPI_CFG_TRANS_CTRL, ahb_wr_data);
//=============================================================================================	
		//delay_1us(20);
		//#define XTX_QSPI_CMD_WR_ENA             0x06	
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_WR_ENA);
//=============================================================================================	
    //qspi_task_wait_endint(1, 200);
	  while(1)
		{
				ahb_rd_data = reg_read(ADDR_QSPI_CFG_INT_STATUS) & (0x1<<OFFSET_QSPI_END_INT);
				if (ahb_rd_data==(0x1<<OFFSET_QSPI_END_INT))
				{
						 reg_write(ADDR_QSPI_CFG_INT_STATUS, (0x1<<OFFSET_QSPI_END_INT)); // Clear EndInt status
						 break;
				}				
		}
//=============================================================================================
}

unsigned char spi_nor_read_SR1(void)
{
    unsigned char sr;
	  char *string;
	
    qspi_task_config_trans(ATCSPI_SPI_TRANS_RD_ONLY, 0x1, 0x0, 0x0, 0x0, 0x1);
	
    UATR0_PRINT_LOG((unsigned char *)("spi_nor_read_SR1 --- 1111111"));	
	  //Gecko_Timer_Setting(TIMER_INDEX0);//80 0000, 100ms
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_STATUS);
	  //__wfi();
    UATR0_PRINT_LOG((unsigned char *)("spi_nor_read_SR1 --- 222222"));		
    qspi_task_wait_endint(1, 100);
    sr =  reg_read(ADDR_QSPI_CFG_SPI_DATA);
	
		#if 1//DEBUG_UATR0_PRINT_LOG
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
		UATR0_PRINT_LOG((unsigned char *)("spi_nor_read_SR1 --- sr = 0x"));
		string=my_itoa(sr);
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));				
		#endif	
	
	
    return sr;
}

/*Mandatory Flash Programming Functions (Called by FlashOS):*/
// Initialize Flash
int Init (unsigned long adr,  unsigned long clk,unsigned long fnc)
{
    return 0;
}

// De-initialize Flash
int UnInit(unsigned long fnc)
{
    return 0;
}


//  Erase complete Device
int EraseChip(void)
{
    // write enable: Send 06H
    spi_nor_write_en();
    // send 60H
    reg_write(ADDR_QSPI_CFG_INT_ENABLE, (0x1<<OFFSET_QSPI_END_INT));
    qspi_task_config_trans(ATCSPI_SPI_TRANS_NONE_DATA, 0x1, 0x0, 0x0, 0x0, 0x0);
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_CE);
    #if FW_TC_DEBUG_USE_SYS_REG
        reg_write(ADDR_TEST_TB_CONTROL_DEBUG_CMD, 0xCAFE0001); // Gofee Debug Only for Firmware Simulation Without Print
    #endif
    qspi_task_wait_endint(1, 10);
    #if FW_TC_DEBUG_USE_SYS_REG
        reg_write(ADDR_TEST_TB_CONTROL_DEBUG_CMD, 0xCAFE0002); // Gofee Debug Only for Firmware Simulation Without Print
    #endif

    // polling WIP bit ->0
    while(1)
    {
        if((spi_nor_read_SR1()&0x01)==0){
            break;
        }
    }
    #if FW_TC_DEBUG_USE_SYS_REG
        reg_write(ADDR_TEST_TB_CONTROL_DEBUG_CMD, 0xCAFE0003); // Gofee Debug Only for Firmware Simulation Without Print
    #endif
    return 0;
}




/*
Erase Timing:
1,cs low
2,send 06H command
3,cs high
4,cs low
5,send 20H comand and ADDRESS
should give cs low before command and cs high after command...................
6,send 05H comand to check WIP bit
7,if WIP bit equal 0 exit loop
*/

// Erase Sector Function
__RAM_CODE__ int EraseSector (unsigned long sectornum)
//int EraseSector (unsigned long sectornum)
{
    unsigned int ahb_wr_data;
	  unsigned int ahb_rd_data;
	  uint32 adr;
    //ADDR_BASE_GECKO_QSPI_ACCESS ---> 0x10000000
	  //adr=ADDR_BASE_GECKO_QSPI_ACCESS + sectornum * 0x1000;
	  adr= sectornum * 0x1000;

   if(adr==0x3F000)//0x3_F000 Addr keep reserved,not for users
   return 1;
	
//===========================================================================
	  // write enable: Send 06H	
//************************************************************************	
    spi_nor_write_en();
//***********************************************************************
//===========================================================================
    // send 20H+adr
	  ahb_wr_data = ahb_wr_data & (~((0xF<<OFFSET_SPI_CTRL_TRANS_MODE) | (0x1FF <<OFFSET_SPI_CTRL_WRTRAN_CNT) | (0x1FF <<OFFSET_SPI_CTRL_RDTRAN_CNT)));
    ahb_wr_data = (0x1<<OFFSET_SPI_CTRL_CMD_EN) | (0x1<<OFFSET_SPI_CTRL_ADDR_EN) | (ATCSPI_SPI_TRANS_NONE_DATA<<OFFSET_SPI_CTRL_TRANS_MODE);
    
	  reg_write(ADDR_QSPI_CFG_TRANS_CTRL, ahb_wr_data);
    reg_write(ADDR_QSPI_CFG_SPI_ADDR, adr);//add Kevin Zhou 20220303	
	
    reg_write(ADDR_QSPI_CFG_INT_ENABLE, (0x1<<OFFSET_QSPI_END_INT));

//===========================================================================	
	
		//Gecko_Timer_Setting(TIMER_INDEX0);//80 0000, 100ms
    //send 20H+adr	
    //#define XTX_QSPI_CMD_SE   0x20
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_SE);
		//__wfi();
		sram_run_delay(20000);
		
//**********************************************************************
//=============================================================================================	
	  while(1)
		{
				ahb_rd_data = reg_read(ADDR_QSPI_CFG_INT_STATUS) & (0x1<<OFFSET_QSPI_END_INT);
				if (ahb_rd_data==(0x1<<OFFSET_QSPI_END_INT))
				{
						 reg_write(ADDR_QSPI_CFG_INT_STATUS, (0x1<<OFFSET_QSPI_END_INT)); // Clear EndInt status
						 break;
				}				
		}
//=============================================================================================		
//***********************************************************************
//=============================================================================================
		unsigned char sr;
    while(1)
		{
			
				delay_1us(50);
				ahb_wr_data = ahb_wr_data & (~((0xF<<OFFSET_SPI_CTRL_TRANS_MODE) | (0x1FF <<OFFSET_SPI_CTRL_WRTRAN_CNT) | (0x1FF <<OFFSET_SPI_CTRL_RDTRAN_CNT)));
				ahb_wr_data = (0x1<<OFFSET_SPI_CTRL_CMD_EN) | (ATCSPI_SPI_TRANS_RD_ONLY<<OFFSET_SPI_CTRL_TRANS_MODE) | (1<<OFFSET_SPI_CTRL_RDTRAN_CNT);
				reg_write(ADDR_QSPI_CFG_TRANS_CTRL, ahb_wr_data);

				reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_STATUS);
				sr =  reg_read(ADDR_QSPI_CFG_SPI_DATA);
				if((sr&0x01)==0)
				{
					 break;
				}
			
	  }

    return 0;

}




//  Program Page Function
__RAM_CODE__ int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf)
{
    unsigned int i = 0;
	  unsigned int ahb_wr_data;
    unsigned int ahb_rd_data;
	  //sz=256;
	  sz=16;
	  //unsigned int j=0;
	  //char *string;	
    //ADDR_BASE_GECKO_QSPI_ACCESS ---> 0x10000000
		unsigned int HW_ADDR;
	  //HW_ADDR=ADDR_BASE_GECKO_QSPI_ACCESS + adr * 0x1000;////add Kevin Zhou 20220303
	  HW_ADDR= adr * 0x1000;////add Kevin Zhou 20220313
//************************************************************************
    //write enable: Send 06H	
    spi_nor_write_en();
//************************************************************************
//************************************************************************************************************
    // send 02H+adr +data
    reg_write(ADDR_QSPI_CFG_INT_ENABLE, (0x1<<OFFSET_QSPI_END_INT));

#if 1
    // Prepare Write Data. Total 32bit*4 = 16 bytes; hence set QSPI ctrl wr_cnt = 16-1 =15
    for (i=0; i<4; i++)
    {   
			  //ahb_wr_data=(raw_data1[i*4+3]<<24)|(raw_data1[i*4+2]<<16)|(raw_data1[i*4+1]<<8)|raw_data1[i*4];
        //reg_write(ADDR_QSPI_CFG_SPI_DATA, raw_data1[i]);
			  ahb_wr_data=(buf[i*4+3]<<24)|(buf[i*4+2]<<16)|(buf[i*4+1]<<8)|buf[i*4];
			  reg_write(ADDR_QSPI_CFG_SPI_DATA, ahb_wr_data);
    }
#else
    // Prepare Write Data. Total 32bit*4 = 16 bytes; hence set QSPI ctrl wr_cnt = 16-1 =15
    for (i=0; i<4; i++)
    {
        reg_write(ADDR_QSPI_CFG_SPI_DATA, raw_data1[i]);
    }
#endif
		
	  ahb_wr_data = ahb_wr_data & (~((0xF<<OFFSET_SPI_CTRL_TRANS_MODE) | (0x1FF <<OFFSET_SPI_CTRL_WRTRAN_CNT) | (0x1FF <<OFFSET_SPI_CTRL_RDTRAN_CNT)));
    ahb_wr_data = (0x1<<OFFSET_SPI_CTRL_CMD_EN) | (0x1<<OFFSET_SPI_CTRL_ADDR_EN) | (ATCSPI_SPI_TRANS_WR_ONLY<<OFFSET_SPI_CTRL_TRANS_MODE)| ((sz-1)<<OFFSET_SPI_CTRL_WRTRAN_CNT);

	  reg_write(ADDR_QSPI_CFG_TRANS_CTRL, ahb_wr_data);
    reg_write(ADDR_QSPI_CFG_SPI_ADDR, HW_ADDR);//add Kevin Zhou 20220303			

		//Gecko_Timer_Setting(TIMER_INDEX0);//80 0000, 100ms		
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_PAGE_PROG);	
		//__wfi();
		sram_run_delay(6000);

//************************************************************************
    //qspi_task_wait_endint(1, 200);
	  while(1)
		{
				ahb_rd_data = reg_read(ADDR_QSPI_CFG_INT_STATUS) & (0x1<<OFFSET_QSPI_END_INT);
				if (ahb_rd_data==(0x1<<OFFSET_QSPI_END_INT))
				{
						 reg_write(ADDR_QSPI_CFG_INT_STATUS, (0x1<<OFFSET_QSPI_END_INT)); // Clear EndInt status
						 break;
				}				
		}
//***********************************************************************************************************			
   //reg_write(ADDR_QSPI_CFG_INT_ENABLE, (0x1<<OFFSET_QSPI_END_INT));	
		unsigned char sr;
    while(1)
		{
				delay_1us(50);			
				ahb_wr_data = ahb_wr_data & (~((0xF<<OFFSET_SPI_CTRL_TRANS_MODE) | (0x1FF <<OFFSET_SPI_CTRL_WRTRAN_CNT) | (0x1FF <<OFFSET_SPI_CTRL_RDTRAN_CNT)));
				ahb_wr_data = (0x1<<OFFSET_SPI_CTRL_CMD_EN) | (ATCSPI_SPI_TRANS_RD_ONLY<<OFFSET_SPI_CTRL_TRANS_MODE) | (1<<OFFSET_SPI_CTRL_RDTRAN_CNT);
				reg_write(ADDR_QSPI_CFG_TRANS_CTRL, ahb_wr_data);
				//qspi_task_config_trans(ATCSPI_SPI_TRANS_RD_ONLY, 0x1, 0x0, 0x0, 0x0, 0x1);
			
				reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_STATUS);
				sr =  reg_read(ADDR_QSPI_CFG_SPI_DATA);			
				if((sr&0x01)==0)
				{
					 break;
				}			
	  }
		
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!		
		ahb_rd_data = reg_read(ADDR_QSPI_CFG_INT_STATUS) & (0x1<<OFFSET_QSPI_END_INT);
		if (ahb_rd_data==(0x1<<OFFSET_QSPI_END_INT))
		{
				#if 0//DEBUG_UATR0_PRINT_LOG		
				UATR0_PRINT_LOG((unsigned char *)("Clear EndInt status......\r\n"));
				#endif
				reg_write(ADDR_QSPI_CFG_INT_STATUS, (0x1<<OFFSET_QSPI_END_INT)); // Clear EndInt status
		}	
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	
//=============================================================================================
	
    return 0;
}



__RAM_CODE__ int QSPI_NorFlash_ReadWord(unsigned int addr)
{
	  unsigned int ahb_wr_data;
    unsigned int ahb_rd_data;
	  //sz=256;
	  unsigned char sz;
	  //char *string;	
	  //read 4 bytes, 1 word
	  sz=4;
    //ADDR_BASE_GECKO_QSPI_ACCESS ---> 0x10000000
		//unsigned int HW_ADDR;
	  //HW_ADDR=ADDR_BASE_GECKO_QSPI_ACCESS + adr * 0x1000;////add Kevin Zhou 20220303
	  //HW_ADDR= adr * 0x1000;////add Kevin Zhou 20220313	

	  ahb_wr_data = ahb_wr_data & (~((0xF<<OFFSET_SPI_CTRL_TRANS_MODE) | (0x1FF <<OFFSET_SPI_CTRL_WRTRAN_CNT) | (0x1FF <<OFFSET_SPI_CTRL_RDTRAN_CNT)));
    ahb_wr_data = (0x1<<OFFSET_SPI_CTRL_CMD_EN) | (0x1<<OFFSET_SPI_CTRL_ADDR_EN) | (ATCSPI_SPI_TRANS_RD_ONLY<<OFFSET_SPI_CTRL_TRANS_MODE)| ((sz-1)<<OFFSET_SPI_CTRL_RDTRAN_CNT);

	  reg_write(ADDR_QSPI_CFG_TRANS_CTRL, ahb_wr_data);
    reg_write(ADDR_QSPI_CFG_SPI_ADDR, addr);//add Kevin Zhou 20220303	


		reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_DATA); 
		
//***********************************************************************************************************
		delay_1us(200);
//***********************************************************************************************************		

		ahb_rd_data = reg_read(ADDR_QSPI_CFG_SPI_DATA);
	
		#if 0//DEBUG_UATR0_PRINT_LOG
		//UATR0_PRINT_LOG((unsigned char *)("\r\n****i = 0x"));
		//string=my_itoa(i);
		//UATR0_PRINT_LOG((unsigned char *)(string));	
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));			
		UATR0_PRINT_LOG((unsigned char *)("QSPI_NorFlash_ReadWord --- ahb_rd_data = 0x"));
		string=my_itoa(ahb_rd_data);
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));				
		#endif	
	
	  return ahb_rd_data;
}





// Read Page Function
int ReadPage (unsigned long adr, unsigned long sz, unsigned char *buf)
{
    unsigned int i;
    qspi_task_config_trans(ATCSPI_SPI_TRANS_RD_ONLY, 0x1, 0x1, adr, 0, sz-1);
    reg_write(ADDR_QSPI_CFG_SPI_CMD, XTX_QSPI_CMD_RD_DATA); // Write QSPI command to trigger
    qspi_task_wait_endint(20, 10); // Wait for EndInt; Input: 1. wait_period; 2. timeout
    for (i=0; i<sz; i++)
    {
        buf[i] = reg_read_8bit(ADDR_QSPI_CFG_SPI_DATA);
    }
    return 0;	

}


//------------------------------------------------------------------


//  Gofee @ 2021.03.13 added complex XTX QSPI Flash operation with Page Eraser and Page Program
void tc_qspi_to_xtx_page_op()
{
    volatile unsigned int ahb_rd_data;
    unsigned int ahb_tmp_addr;
    unsigned char i;
//UATR0_PRINT_LOG("GEK1\r\n");
	  //pDebugPrintfEX("GEK1\n");
    reg_write(ADDR_TEST_TB_CONTROL_DEBUG_CMD, 0x03141956); // Gofee Debug Only for Firmware Simulation Without Print | Print time
	//UATR0_PRINT_LOG("EraseSector...\r\n");
	  //pDebugPrintfEX("EraseSector\n");
    EraseSector(0x0); // Input flash address
	//UATR0_PRINT_LOG("EraseSector Success!\r\n");	
    reg_write(ADDR_TEST_TB_CONTROL_DEBUG_CMD, 0xCAFE00A1); // Gofee Debug Only for Firmware Simulation Without Print
	  //delay_1us(1000);
	//UATR0_PRINT_LOG("ProgramPage...\r\n");
		//pDebugPrintfEX("ProgramPage\n");
    ProgramPage(0x0, 256, &raw_data1[0]); // (unsigned long adr, unsigned long sz, unsigned char *buf)
    reg_write(ADDR_TEST_TB_CONTROL_DEBUG_CMD, 0xCAFE00A2); // Gofee Debug Only for Firmware Simulation Without Print
	//UATR0_PRINT_LOG("ProgramPage Success!\r\n");
    delay_1us(5);
    for (i=0; i<10; i++)
    {
        ahb_tmp_addr= ADDR_BASE_GECKO_QSPI_ACCESS + (i*4);
        ahb_rd_data = reg_read(ahb_tmp_addr);
    #if FW_TC_DEBUG_USE_SYS_REG
        manba_task_sys_debug(FW_TEST_MACRO_AHB_RD_CMD, ahb_tmp_addr, ahb_rd_data); // Input: debug_msg_cmd, debug_msg_addr, debug_msg_data
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: Read addr: %x; Read data: %x .\n", ahb_tmp_addr, ahb_rd_data);
    #endif			
		//char *string;				
		//UATR0_PRINT_LOG("ftc_qspi_to_xtx_page_op --- read ProgramPage data:");
		//string=my_itoa(ahb_rd_data);
		//UATR0_PRINT_LOG(string);
		//UATR0_PRINT_LOG("\r\n");			
    }
    //reg_write(ADDR_BASE_GECKO_TB_CONTROL, 0xCAFEFFFF); // Gofee Debug Only
		//UATR0_PRINT_LOG("GEK2\r\n");
		//pDebugPrintfEX("GEK2\n");
}


//------------------------------------------------------------------
//  The main test function
//------------------------------------------------------------------
void xtxflash_test_entry(void)
{
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: sanity test started.\n");
    #endif

    //  Main test block
    //  tc_qspi_to_xtx_sanity();
    //  tc_qspi_to_xtx_single_wr();
    //  tc_qspi_to_xtx_s_ahb_rd();
    tc_qspi_to_xtx_dual_fast_rd();
    //  tc_qspi_to_xtx_quad_fast_rd();
    //  tc_qspi_to_xtx_page_op();
    //  tc_gecko_cm0_aon_sleep();
    //----------------------------------

    //  Gofee added temp test
    //  reg_read(ADDR_BASE_GECKO_QSPI_CONFIG);
    //  reg_read(ADDR_BASE_GECKO_AON_CONFIG);
    //  delay_1us(5);
    //----------------------------------

    #if FW_TC_DEBUG_USE_SYS_REG
        reg_write(ADDR_TEST_TB_CONTROL_DEBUG_CMD, 0xCAFEFFFF); // Gofee Debug Only for Firmware Simulation Without Print
    #endif
    #if FW_TC_DEBUG_USE_PRINTF
        printf("Gecko Test: sanity test finished.\n");
        stop(0);
    #endif
}







void write_norflash(unsigned int base,unsigned int offset,unsigned int word)
{
		 volatile unsigned short * p=(volatile unsigned short *)(base+(offset<<1));
		 *p=word;
}


unsigned int read_norflash(unsigned int base,unsigned int offset)
{
		 volatile unsigned short * p=(volatile unsigned short *)(base+(offset<<1));
		 return *p;
}




//void Gek1108_NV_Save_UserData(uint32 addr,uint8_t dat)
void Gek1108_NV_Save_UserData()
{
  volatile unsigned int ahb_rd_data;
  //unsigned int ahb_tmp_addr;
  unsigned char i;
 volatile unsigned long adr;
 unsigned int sector_num=0x3F;//63//0x20=32(4KB sector)
	char *string;	
  //unsigned char raw_data22[16]={0x11,0x22,0x33,0x44,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
	//unsigned char raw_data22[16];
	uint32 status;

//  use timer-wakeup to hold the CPU
//	Gecko_Timer_Init();
//	Gecko_Timer_Setting(TIMER_INDEX0);
//	__wfi();


	
	UATR0_PRINT_LOG((unsigned char *)("EraseSector ...\r\n"));
	status= pEnterCriticalSection();
  //UATR0_PRINT_LOG((unsigned char *)("pEnterCriticalSection!\n"));	
  EraseSector(sector_num); // Input flash address	
	pExitCriticalSection(status);
	UATR0_PRINT_LOG((unsigned char *)("EraseSector Success!\r\n"));



	
  //ADDR_BASE_GECKO_QSPI_ACCESS ---> 0x10000000	
	UATR0_PRINT_LOG((unsigned char *)("ProgramPage ...\r\n"));
  status=pEnterCriticalSection();  
  //ProgramPage(sector_num, 16, &raw_data22[0]);
  ProgramPage(sector_num, 16, &raw_data1[0]);
  pExitCriticalSection(status);
	UATR0_PRINT_LOG((unsigned char *)("ProgramPage Success!\r\n"));	


//========================================================
  adr=sector_num * 0x1000;	
	//read back 4 words 
	for(i=0; i<4; i++)
	{
		ahb_rd_data=QSPI_NorFlash_ReadWord(adr);
		adr=adr+4;
		
		#if DEBUG_UATR0_PRINT_LOG
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));			
		UATR0_PRINT_LOG((unsigned char *)("QSPI_NorFlash_ReadWord --- ahb_rd_data = 0x"));
		string=my_itoa(ahb_rd_data);
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));				
		#endif
	}
//========================================================	
	
	
}



typedef struct{
	uint8_t hall_votage_level;     //0:低电平 1:高电平
	uint8_t hall_status_variable;  //霍尔状态变量
	uint8_t hall_det_time;      	 //霍尔检测时间间隔
	uint8_t key_votage_level;      //0:低电平 1：高电平
	
	uint8_t hj_batlevel;           //电量等级0-100
	uint8_t hj_batlevel_bak;
	uint8_t hj_100ms_time;         
	
	uint8_t charge_online_flag;    //充电线在线标记
	uint8_t box_full_flag;         //充饱标记
	uint8_t charge_status;         //充电线状态变化标记
	uint8_t box_full_status;       //盒子充饱复充变化标记
	uint8_t charge_det_time;       //充电线检测时间间隔
	uint8_t box_full_det_time;     //充饱检测时间间隔
	
	uint8_t red_led_mode;
	uint8_t red_led_scan_time;
	uint8_t red_led_couter;
	uint8_t red_led_times;
	uint16_t red_led_on_cnt;
	
	uint8_t green_led_mode;
	uint8_t green_led_scan_time;
	uint8_t green_led_couter;
	uint8_t green_led_times;
	uint16_t green_led_on_cnt;
	
	
	uint8_t white_led_scan_time;
	uint8_t white_led_mode;
	uint16_t white_led_on_cnt;
	
}HJ_AppTypedef;


HJ_AppTypedef hj_appstruct;


void HJ_Save_UserData(void)
{
	uint8 hj_user_sector=60;//addr=60*0x1000=0x3_C000;

	volatile unsigned long adr;
	
	//unsigned char hj_raw_data[16]={0};
	unsigned char hj_raw_data[16]={0xFF};
	uint32 status;
	
	hj_raw_data[0] = hj_appstruct.hall_status_variable;
	hj_raw_data[1] = hj_appstruct.hall_status_variable;
	hj_raw_data[2] = hj_appstruct.hall_status_variable;
	hj_raw_data[3] = hj_appstruct.hall_status_variable;


//	hj_raw_data[0] = 0xA5;
//	hj_raw_data[1] = 0x89;	
//	hj_raw_data[2] = 0xCA;
//	hj_raw_data[3] = 0xFE;
	
	status= pEnterCriticalSection();
	EraseSector(hj_user_sector); // Input flash address
	pExitCriticalSection(status);
	
	status=pEnterCriticalSection(); 
	ProgramPage(hj_user_sector, 16, &hj_raw_data[0]);
	pExitCriticalSection(status);

}



uint32 hj_flash_rd_data;
//hj_appstruct.hall_status_variable
void HJ_Read_UserData(void)
{
	volatile unsigned long adr;
	uint8 hj_user_sector=60;//addr=60*0x1000=0x3_C000;
  adr=hj_user_sector * 0x1000;
	uint32 status;
  status=pEnterCriticalSection(); 
  hj_flash_rd_data=QSPI_NorFlash_ReadWord(adr);//read a word(4-byte) data
  pExitCriticalSection(status);	
}	

	
	
	

