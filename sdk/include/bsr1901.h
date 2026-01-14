
#include "platform_config.h"

#define	 	 GECKO_ROM_BASE		    							0x00000000
#define	 	 GECKO_SRAM_BASE		    						0x20000000

#define BIT(n)            ((unsigned int) 1 << (n))
//#define BIT(x)          (1<<(x))

/*
//AON Default Value
//uint32 AON_BASE_ADDR_0x2C_Default=0;
//uint32 AON_CFG_ANA_CTRL_1_0x20_Default=0x808E7885;//32bit
//uint32 AON_CFG_ANA_CTRL_2_0x24_Default=0;
//uint32 AON_CFG_GPIO_WAKEUP_CTRL_0x10_Default=0;//?
*/

#define  QI_TEST_EN				0
#define  LIGHTN_TEST_EN		0

#define  LED_TEST_EN			1

#define  PWM_TEST_EN			1
#define  UART_TEST_EN			1
#define  SPI_TEST_EN			1
#define  I2C_TEST_EN			1
#define  GPIO_TEST_EN			1
#define  ADC_TEST_EN			1

#define  DeepSleep_TEST_EN	1

#define  DAC_TEST_EN			0
#define  DMA_TEST_EN			1

#define  TIMER_TEST_EN		1
#define  WDT_TEST_EN		  1
#define  iWDT_TEST_EN		  1
#define  RTC_TEST_EN			1

#define  MOS_TEST_EN			1



//#define  CHRG_INSET_DET_EN	1

#define  MIC_DET_EN		1



#if 0

New GEK1109

//AHB Peripherals Base Addr.
`define ADDR_BASE_AHB_FLASH      32'h0000_0000
`define ADDR_BASE_AHB_RAM        32'h2000_0000

`define ADDR_BASE_AHB_UART0      32'h4000_1000
`define ADDR_BASE_AHB_QSPI       32'h4000_2000
`define ADDR_BASE_AHB_UART1      32'h4000_3000
`define ADDR_BASE_AHB_UART2      32'h4000_4000
`define ADDR_BASE_AHB_PWM        32'h4000_5000
//`define ADDR_BASE_AHB_UART3      32'h4000_6000
`define ADDR_BASE_AHB_DMA        32'h4000_6000
`define ADDR_BASE_AHB_APB0       32'h4001_0000
`define ADDR_BASE_AHB_APB1       32'h4002_0000


//APB0 Peripherals Base Addr.
`define ADDR_BASE_APB_CMU       32'h4001_0000
`define ADDR_BASE_APB_TIMER0    32'h4001_1000
`define ADDR_BASE_APB_TIMER1    32'h4001_2000
`define ADDR_BASE_APB_GPIO2     32'h4001_3000
`define ADDR_BASE_APB_WDT       32'h4001_4000
`define ADDR_BASE_APB_ADC       32'h4001_6000
`define ADDR_BASE_APB_LTNING    32'h4001_7000
`define ADDR_BASE_APB_GPIO0     32'h4001_8000
`define ADDR_BASE_APB_GPIO1     32'h4001_9000
`define ADDR_BASE_APB_I2C0      32'h4001_A000
`define ADDR_BASE_APB_SPI0      32'h4001_B000
`define ADDR_BASE_APB_I2C1      32'h4001_C000
`define ADDR_BASE_APB_SPI1      32'h4001_D000
`define ADDR_BASE_APB_QI        32'h4001_E000
`define ADDR_BASE_APB_LED       32'h4001_F000

//APB1 Peripherals Base Addr.
`define ADDR_BASE_APB_AON       32'h4002_0000
`define ADDR_BASE_APB_RTC       32'h4002_1000
`define ADDR_BASE_APB_IWDT      32'h4002_2000
`define ADDR_BASE_APB_DMA       32'h4003_0000
`define ADDR_BASE_APB_SYSCTRL   32'h4004_0000


`define OFFSET_SYSCTRL_PAD_FUN0 16'h0300 
`define OFFSET_SYSCTRL_PAD_FUN1 16'h0304
`define OFFSET_SYSCTRL_PAD_FUN2 16'h0308

`define OFFSET_AON_RET_RAM_0  16'h0040
`define OFFSET_AON_RET_RAM_1  16'h0044
`define OFFSET_AON_RET_RAM_2  16'h0048
`define OFFSET_AON_RET_RAM_3  16'h004C
`define OFFSET_AON_RET_RAM_4  16'h0050
`define OFFSET_AON_RET_RAM_5  16'h0054
`define OFFSET_AON_RET_RAM_6  16'h0058
`define OFFSET_AON_RET_RAM_7  16'h005C


`define  PWM_CH0 tbench.PAD_GPIO18

//DAC check registers
#define ADDR_BASE_APB_DAC                     	0x40013000
#define GEK_DAC_BASE_ADDR												0x40013000
//`define ADDR_BASE_APB_DAC   32'h4003_0000

`define OFFSET_DAC_CTRL     16'h0004

//GPIO Check Registers
`define GPIO_GFIN_ST_OFFSET    16'h0614
`define GPIO_AFSEL_OFFSET      16'h0420

//UART Check Registers
`define OFFSET_UART_MCR_TEST   16'h0010
`define OFFSET_UART_FFR_TEST   16'h0024

//SPI Check Registers
`define OFFSET_SPI_0X04_TEST   16'h0004
`define OFFSET_SPI_0X0C_TEST   16'h000C

//I2C Check Registers
`define OFFSET_I2C_INT_EN_TEST   16'h0014
`define OFFSET_I2C_SETUP_TEST    16'h002C

//Timer Check Registers
`define OFFSET_TIMER_LOAD_CNT  16'h0000
`define OFFSET_TIMER_CTRL_REG  16'h0008

//ADC Check Registers
`define OFFSET_ADC_IRQ_MASK   16'h002C
`define OFFSET_ADC_THRESHOD9  16'h005C

//LGT Check Registers
`define OFFSET_LGT_0X04_TEST   16'h0004
`define OFFSET_LGT_BUF00_TEST  16'h0060

//QI Check Registers
`define OFFSET_QI_INTEN_TEST   16'h0008
`define OFFSET_QI_US_CTRL_TEST  16'h0030

//LCD Check Registers
`define OFFSET_LCD_REG0C_TEST  16'h000C
`define OFFSET_LCD_REG28_TEST  16'h0028






/******************************************************************************/
/*                         Peripheral Module Address Map                      */
/******************************************************************************/
#define	 	 GECKO_ROM_BASE		    							0x00000000
#define	 	 GECKO_SRAM_BASE		    						0x20000000
//define APB lightning interface base address
#define	 	 GECKO_APB_APPLE_LIGHTNING_BASE		  0x40017000
////define APB GPIO interface base address
#define 	 GPIO0_BASE       									0x40018000
#define 	 GPIO1_BASE       									0x40019000
//define AHB I2C base address
#define    GECKO_AHB_I2C_BASE                 0X40000000
#define    GECKO_APB_I2C_BASE                 0X40000000
//define AHB uart base address
#define    GECKO_AHB_UART0_BASE               0x40001000
#define    GECKO_AHB_UART2_BASE               0x40003000
#define 	 GECKO_UART_BASE  									0x40001000
#define	   GECKO_AHB_UART_2_BASE			        0x40003000
#define 	 REG_UART_BASE               				0x40001000
#define 	 REG_UART2_BASE              				0x40003000
//define APB ADC base address
#define	 	 GECKO_APB_ADC_BASE		    					0x40016000
#define 	 REG_GPADC_BASE              				0x40016000
//define APB timer base address
#define    GECKO_APB_TIMER0_BASE    					0X40011000
#define    GECKO_APB_TIMER1_BASE    					0X40012000
#define 	 REG_TIMER0_BASE             				0x40011000
#define 	 REG_TIMER1_BASE             				0x40012000

//define APB PWM base address
#define 	 GECKO_PWM_BASE                			0x40013000
//define APB EFUSE base address
#define  	 GECKO_APB_EFFUSE_ADDR							0X40015000
//define APB watchdog base address
#define	 	 GECKO_APB_WATCHDOG_BASE_ADDR		    0X40014000
//define APB CMU base address
#define    GECKO_APB_CMU_BASE                 0x40010000
#define    GECKO_APB_SCU_BASE                 0x40010000

//define APB SPI Master base address
#define    GECKO_APB_SPI_MASTER_BASE          0x4001B000

//define APB AON base address
#define    GECKO_APB_AON_BASE                 0x4001A000

//define APB LCD base address
#define    GECKO_APB_LCD_BASE                 0x4001C000

//define APB PMU base address
#define    GECKO_APB_PMU_BASE                 0x4001D000

//define AHB flash config register base address
#define    GECKO_AHB_FLASHCTRL_REG_BASE       0x40002000 //
#define    ADDR_BASE_GECKO_QSPI_CONFIG        0x40002000

#define ADDR_BASE_GECKO_QSPI_ACCESS     0x10000000
#define ADDR_BASE_GECKO_QSPI_CONFIG     0x40002000

#endif



#define HCI_clock_frequency                                  (10000000)

#ifdef BSR1901_SYS_8MHZ_MODE

//#define HCI_clock_frequency                                  (8000000)

#else
//uart clock source, 16Mhz/26Mhz
#define HCI_clock_frequency                                  (16000000)
#endif


#define    BSR1901_SPI_MASTER_TEST      0







#define     __I     volatile                  /*!< defines 'read only' permissions      */
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */


#if 0

#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000L
#define BIT24 0x01000000L
#define BIT25 0x02000000L
#define BIT26 0x04000000L
#define BIT27 0x08000000L
#define BIT28 0x10000000L
#define BIT29 0x20000000L
#define BIT30 0x40000000L
#define BIT31 0x80000000UL

#endif



#if 1


/* -------------------------  Interrupt Number Definition  ------------------------ */
/* -------------------------  Interrupt Number Definition  ------------------------ */
/* -------------------------  Interrupt Number Definition  ------------------------ */
typedef enum IRQn
{
	
/* -------------------  Cortex-M4 Processor Exceptions Numbers  ------------------- */
  NonMaskableInt_IRQn           = -14,      /*!<  2 Non Maskable Interrupt          */
  HardFault_IRQn                = -13,      /*!<  3 HardFault Interrupt             */
  MemoryManagement_IRQn         = -12,      /*!<  4 Memory Management Interrupt     */
  BusFault_IRQn                 = -11,      /*!<  5 Bus Fault Interrupt             */
  UsageFault_IRQn               = -10,      /*!<  6 Usage Fault Interrupt           */
  SVCall_IRQn                   =  -5,      /*!< 11 SV Call Interrupt               */
  DebugMonitor_IRQn             =  -4,      /*!< 12 Debug Monitor Interrupt         */
  PendSV_IRQn                   =  -2,      /*!< 14 Pend SV Interrupt               */
  SysTick_IRQn                  =  -1,      /*!< 15 System Tick Interrupt           */


/* ----------------------  B Specific Interrupt Numbers  --------------------- */
  CMP1_IRQn                     =   0,      /*!< Compare1 Interrupt                 */
  CMP2_IRQn                     =   1,      /*!< Compare2 Interrupt                 */

  UART0_IRQn                    =   2,      /*!< UART0 Interrupt                    */
  UART1_IRQn                    =   3,      /*!< UART1 Interrupt                    */
  DMA_IRQn                      =   4,      /*!< DMA Interrupt                      */
	
  TIMER0_IRQn                   =   5,      /*!< TIMER0 Interrupt                   */
  RSV1_IRQn                     =   6,      /*!< RESERVED Interrupt                 */
  RSV0_IRQn                     =   7,      /*!< RESERVED Interrupt                 */
	
  GPIO_0_INTR_IRQn              =   8,      /*!< GPIO Group 0 INTR Interrupt        */
  GPIO_1_INTR_IRQn              =   9,      /*!< GPIO Group 1 INTR Interrupt        */
	
  WDT_IRQn                      =   10,     /*!< WatchDog Interrupt                 */
  GPIO_A2_IRQn             			=   11,     /*!< GPIOA[2] Interrupt                	*/

  GPIO_A0_IRQn             			=   12,     /*!< GPIOA[0] Interrupt                	*/
	
  APB_SPI0_IRQn                  =  13,     /*!< SPI Master Interrupt               */
  GPIO_A1_IRQn             			=   14,     /*!< GPIOA[1] Interrupt                	*/

  GPIO_A3_IRQn             			=   15,     /*!< GPIOA[3] Interrupt                	*/
  QSPI_IRQn                     =   16,     /*!< QSPI controller interrupt          */


  IWDT_IRQ											=   17,
	
  GPIO_A4_IRQn             			=   18,      /*!< GPIOA[4] interrupt   								*/	
  GPIO_A5_IRQn             			=   19,      /*!< GPIOA[5] interrupt   								*/	
  GPIO_A6_IRQn             			=   20,      /*!< GPIOA[6] interrupt   								*/	
  GPIO_A7_IRQn             			=   21,      /*!< GPIOA[7] interrupt   								*/	

  GPIO_B0_IRQn             			=   22,      /*!< GPIOB[0] interrupt   								*/	
  GPIO_B1_IRQn             			=   23,      /*!< GPIOB[1] interrupt   								*/	
  GPIO_B2_IRQn             			=   24,      /*!< GPIOB[2] interrupt   								*/	
  GPIO_B3_IRQn             			=   25,      /*!< GPIOB[3] interrupt   								*/	
  GPIO_B4_IRQn             			=   26,      /*!< GPIOB[4] interrupt   								*/	
  GPIO_B5_IRQn             			=   27,      /*!< GPIOB[5] interrupt   								*/	
  GPIO_B6_IRQn             			=   28,      /*!< GPIOB[6] interrupt   								*/	
  GPIO_B7_IRQn             			=   29,      /*!< GPIOB[7] interrupt   								*/
  AON_IRQn                      =   30,      /*!< Always-on module interrupt          */
  CMU2AHB_IRQ                   =   31,      /*!< Reserved Interrupt                  */
  INVALID_IRQn
} IRQn_Type;


/*
assign irq_raw = {
        cmu2ahb_wr_ack,     //31
        //aon2core_wakeup_irq,//30
        aon2core_wakeup_irq,//30

        //GPIO0 MIS[3:0] use as a group, GPIO0 MIS[7:4] and GPIO1 MIS[7:0] use as seperate wakeup signal. 
        gpio1mis[7:0],      //29:22  //Roy@2022-09-06. ORed as 1-bit for gpio1_mis for Gek1109.
        gpio0mis[7:4],      //21:18  //Roy@2022-09-06. ORed as 1-bit for gpio0_mis for Gek1109.

        //|gpio0mis[3:0],     //17     //Roy@2022-09-06. ORed as 1-bit for gpio0_mis for Gek1109.
        aon2core_iwdt_irq,     //17    //kevin@20240117 modify it to iwdt_irq Roy@2023-02-06. use rtc intr for Gek1109.

        irq_qspi_ctrl,      //16

        //int_qi,           //15  //Roy@2022-09-06. new added for Gek1109.
        gpio0mis[3],        //15   //GPIOA3 kevin@20230718, new added for 1109 final version

        gpio0mis[1],        //14  //Roy@2022-09-06. new added for Gek1109.
        irq_apb_spi0,       //13
        gpio0mis[0],        //12   //Roy@2022-12-02. new added for Gek1109.

        //int_lightning,    //11
        gpio0mis[2],        //11   //GPIOA2 kevin@20230718, new added for 1109 final version
            
        int_wdt,            //10
        gpio1intr,          //9
        gpio0intr,          //8
        1'b0,//intr_gpadc,         //7
        1'b0,//int_opi,            //6
        int_timer0,         //5

        int_dma,            //4  @20240120 //Roy@2022-09-06. new added for Gek1109.

        int_uart1,          //3
        int_uart0,          //2
        cmp2_int,//1'b0,               //1  //Roy@2022-09-06. new added for Gek1109.
        cmp1_int //1'b0                //0
        }; 
*/



/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the Cortex-M0 Processor and Core Peripherals  ------- */

#ifndef __CM0_REV
	#define __CM0_REV               0x0000
	//#warning "__CM0_REV not defined in device header file; using default!"
#endif

#ifndef __NVIC_PRIO_BITS
//	#define __NVIC_PRIO_BITS          2
	#define __NVIC_PRIO_BITS          4	
	//#warning "__NVIC_PRIO_BITS not defined in device header file; using default!"
#endif

#ifndef __Vendor_SysTickConfig
	#define __Vendor_SysTickConfig    0
	//#warning "__Vendor_SysTickConfig not defined in device header file; using default!"
#endif





#endif






#define GECKO_READ_REGISTER(_register_)                      		(*(volatile uint32*)(_register_))
#define HWreg_Get_Register(_register_)                      		(*(volatile uint32*)(_register_))
	
#define GECKO_WRITE_REGISTER(_register_,_value_)                (*(volatile uint32*)(_register_)) = (uint32)(_value_)
#define HWreg_Assign_Register(_register_,_value_)               (*(volatile uint32*)(_register_)) = (uint32)(_value_)








/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/










/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_SysTick     System Tick Timer (SysTick)
  \brief    Type definitions for the System Timer Registers.
  @{
 */

/**
  \brief  Structure type to access the System Timer (SysTick).
 */
typedef struct
{
  __IO uint32_t CTRL;                   /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  __IO uint32_t LOAD;                   /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
  __IO uint32_t VAL;                    /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
  __I  uint32_t CALIB;                  /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTick_Type;





/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_NVIC  Nested Vectored Interrupt Controller (NVIC)
  \brief      Type definitions for the NVIC Registers
  @{
 */

/**
  \brief  Structure type to access the Nested Vectored Interrupt Controller (NVIC).
 */
typedef struct
{
  __IO uint32_t ISER[1U];               /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register */
        uint32_t RESERVED0[31U];
  __IO uint32_t ICER[1U];               /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register */
        uint32_t RSERVED1[31U];
  __IO uint32_t ISPR[1U];               /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register */
        uint32_t RESERVED2[31U];
  __IO uint32_t ICPR[1U];               /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register */
        uint32_t RESERVED3[31U];
        uint32_t RESERVED4[64U];
  __IO uint32_t IP[8U];                 /*!< Offset: 0x300 (R/W)  Interrupt Priority Register */
}  NVIC_Type;




/*@} end of group CMSIS_NVIC */


/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_SCB     System Control Block (SCB)
  \brief    Type definitions for the System Control Block Registers
  @{
 */

/**
  \brief  Structure type to access the System Control Block (SCB).
 */
typedef struct
{
  __I  uint32_t CPUID;                  /*!< Offset: 0x000 (R/ )  CPUID Base Register */
  __IO uint32_t ICSR;                   /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
        uint32_t RESERVED0;
  __IO uint32_t AIRCR;                  /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
  __IO uint32_t SCR;                    /*!< Offset: 0x010 (R/W)  System Control Register */
  __IO uint32_t CCR;                    /*!< Offset: 0x014 (R/W)  Configuration Control Register */
        uint32_t RESERVED1;
  __IO uint32_t SHP[2U];                /*!< Offset: 0x01C (R/W)  System Handlers Priority Registers. [0] is RESERVED */
  __IO uint32_t SHCSR;                  /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
} SCB_Type;




/* SCB CPUID Register Definitions */
#define SCB_CPUID_IMPLEMENTER_Pos          24U                                            /*!< SCB CPUID: IMPLEMENTER Position */
#define SCB_CPUID_IMPLEMENTER_Msk          (0xFFUL << SCB_CPUID_IMPLEMENTER_Pos)          /*!< SCB CPUID: IMPLEMENTER Mask */

#define SCB_CPUID_VARIANT_Pos              20U                                            /*!< SCB CPUID: VARIANT Position */
#define SCB_CPUID_VARIANT_Msk              (0xFUL << SCB_CPUID_VARIANT_Pos)               /*!< SCB CPUID: VARIANT Mask */

#define SCB_CPUID_ARCHITECTURE_Pos         16U                                            /*!< SCB CPUID: ARCHITECTURE Position */
#define SCB_CPUID_ARCHITECTURE_Msk         (0xFUL << SCB_CPUID_ARCHITECTURE_Pos)          /*!< SCB CPUID: ARCHITECTURE Mask */

#define SCB_CPUID_PARTNO_Pos                4U                                            /*!< SCB CPUID: PARTNO Position */
#define SCB_CPUID_PARTNO_Msk               (0xFFFUL << SCB_CPUID_PARTNO_Pos)              /*!< SCB CPUID: PARTNO Mask */

#define SCB_CPUID_REVISION_Pos              0U                                            /*!< SCB CPUID: REVISION Position */
#define SCB_CPUID_REVISION_Msk             (0xFUL /*<< SCB_CPUID_REVISION_Pos*/)          /*!< SCB CPUID: REVISION Mask */

/* SCB Interrupt Control State Register Definitions */
#define SCB_ICSR_NMIPENDSET_Pos            31U                                            /*!< SCB ICSR: NMIPENDSET Position */
#define SCB_ICSR_NMIPENDSET_Msk            (1UL << SCB_ICSR_NMIPENDSET_Pos)               /*!< SCB ICSR: NMIPENDSET Mask */

#define SCB_ICSR_PENDSVSET_Pos             28U                                            /*!< SCB ICSR: PENDSVSET Position */
#define SCB_ICSR_PENDSVSET_Msk             (1UL << SCB_ICSR_PENDSVSET_Pos)                /*!< SCB ICSR: PENDSVSET Mask */

#define SCB_ICSR_PENDSVCLR_Pos             27U                                            /*!< SCB ICSR: PENDSVCLR Position */
#define SCB_ICSR_PENDSVCLR_Msk             (1UL << SCB_ICSR_PENDSVCLR_Pos)                /*!< SCB ICSR: PENDSVCLR Mask */

#define SCB_ICSR_PENDSTSET_Pos             26U                                            /*!< SCB ICSR: PENDSTSET Position */
#define SCB_ICSR_PENDSTSET_Msk             (1UL << SCB_ICSR_PENDSTSET_Pos)                /*!< SCB ICSR: PENDSTSET Mask */

#define SCB_ICSR_PENDSTCLR_Pos             25U                                            /*!< SCB ICSR: PENDSTCLR Position */
#define SCB_ICSR_PENDSTCLR_Msk             (1UL << SCB_ICSR_PENDSTCLR_Pos)                /*!< SCB ICSR: PENDSTCLR Mask */

#define SCB_ICSR_ISRPREEMPT_Pos            23U                                            /*!< SCB ICSR: ISRPREEMPT Position */
#define SCB_ICSR_ISRPREEMPT_Msk            (1UL << SCB_ICSR_ISRPREEMPT_Pos)               /*!< SCB ICSR: ISRPREEMPT Mask */

#define SCB_ICSR_ISRPENDING_Pos            22U                                            /*!< SCB ICSR: ISRPENDING Position */
#define SCB_ICSR_ISRPENDING_Msk            (1UL << SCB_ICSR_ISRPENDING_Pos)               /*!< SCB ICSR: ISRPENDING Mask */

#define SCB_ICSR_VECTPENDING_Pos           12U                                            /*!< SCB ICSR: VECTPENDING Position */
#define SCB_ICSR_VECTPENDING_Msk           (0x1FFUL << SCB_ICSR_VECTPENDING_Pos)          /*!< SCB ICSR: VECTPENDING Mask */

#define SCB_ICSR_VECTACTIVE_Pos             0U                                            /*!< SCB ICSR: VECTACTIVE Position */
#define SCB_ICSR_VECTACTIVE_Msk            (0x1FFUL /*<< SCB_ICSR_VECTACTIVE_Pos*/)       /*!< SCB ICSR: VECTACTIVE Mask */

/* SCB Application Interrupt and Reset Control Register Definitions */
#define SCB_AIRCR_VECTKEY_Pos              16U                                            /*!< SCB AIRCR: VECTKEY Position */
#define SCB_AIRCR_VECTKEY_Msk              (0xFFFFUL << SCB_AIRCR_VECTKEY_Pos)            /*!< SCB AIRCR: VECTKEY Mask */

#define SCB_AIRCR_VECTKEYSTAT_Pos          16U                                            /*!< SCB AIRCR: VECTKEYSTAT Position */
#define SCB_AIRCR_VECTKEYSTAT_Msk          (0xFFFFUL << SCB_AIRCR_VECTKEYSTAT_Pos)        /*!< SCB AIRCR: VECTKEYSTAT Mask */

#define SCB_AIRCR_ENDIANESS_Pos            15U                                            /*!< SCB AIRCR: ENDIANESS Position */
#define SCB_AIRCR_ENDIANESS_Msk            (1UL << SCB_AIRCR_ENDIANESS_Pos)               /*!< SCB AIRCR: ENDIANESS Mask */

#define SCB_AIRCR_SYSRESETREQ_Pos           2U                                            /*!< SCB AIRCR: SYSRESETREQ Position */
#define SCB_AIRCR_SYSRESETREQ_Msk          (1UL << SCB_AIRCR_SYSRESETREQ_Pos)             /*!< SCB AIRCR: SYSRESETREQ Mask */

#define SCB_AIRCR_VECTCLRACTIVE_Pos         1U                                            /*!< SCB AIRCR: VECTCLRACTIVE Position */
#define SCB_AIRCR_VECTCLRACTIVE_Msk        (1UL << SCB_AIRCR_VECTCLRACTIVE_Pos)           /*!< SCB AIRCR: VECTCLRACTIVE Mask */

/* SCB System Control Register Definitions */
#define SCB_SCR_SEVONPEND_Pos               4U                                            /*!< SCB SCR: SEVONPEND Position */
#define SCB_SCR_SEVONPEND_Msk              (1UL << SCB_SCR_SEVONPEND_Pos)                 /*!< SCB SCR: SEVONPEND Mask */

#define SCB_SCR_SLEEPDEEP_Pos               2U                                            /*!< SCB SCR: SLEEPDEEP Position */
#define SCB_SCR_SLEEPDEEP_Msk              (1UL << SCB_SCR_SLEEPDEEP_Pos)                 /*!< SCB SCR: SLEEPDEEP Mask */

#define SCB_SCR_SLEEPONEXIT_Pos             1U                                            /*!< SCB SCR: SLEEPONEXIT Position */
#define SCB_SCR_SLEEPONEXIT_Msk            (1UL << SCB_SCR_SLEEPONEXIT_Pos)               /*!< SCB SCR: SLEEPONEXIT Mask */

/* SCB Configuration Control Register Definitions */
#define SCB_CCR_STKALIGN_Pos                9U                                            /*!< SCB CCR: STKALIGN Position */
#define SCB_CCR_STKALIGN_Msk               (1UL << SCB_CCR_STKALIGN_Pos)                  /*!< SCB CCR: STKALIGN Mask */

#define SCB_CCR_UNALIGN_TRP_Pos             3U                                            /*!< SCB CCR: UNALIGN_TRP Position */
#define SCB_CCR_UNALIGN_TRP_Msk            (1UL << SCB_CCR_UNALIGN_TRP_Pos)               /*!< SCB CCR: UNALIGN_TRP Mask */

/* SCB System Handler Control and State Register Definitions */
#define SCB_SHCSR_SVCALLPENDED_Pos         15U                                            /*!< SCB SHCSR: SVCALLPENDED Position */
#define SCB_SHCSR_SVCALLPENDED_Msk         (1UL << SCB_SHCSR_SVCALLPENDED_Pos)            /*!< SCB SHCSR: SVCALLPENDED Mask */



/**
  * @brief Cortex-M0 processor and core peripherals configuration
  */


#if 0
/* Memory mapping of Cortex-M0 Hardware */
#define SCS_BASE            (0xE000E000UL)                            /*!< System Control Space Base Address */
#define NVIC_BASE           (SCS_BASE +  0x0100UL)                    /*!< NVIC Base Address */
#define SCB_BASE            (SCS_BASE +  0x0D00UL)   

#define SCB                 ((SCB_Type       *)     SCB_BASE      )   /*!< SCB configuration struct */
#define NVIC                ((NVIC_Type      *)     NVIC_BASE     )   /*!< NVIC configuration struct */
#endif




/** \ingroup  CMSIS_core_register
    \defgroup CMSIS_SysTick     System Tick Timer (SysTick)
    \brief      Type definitions for the System Timer Registers.
  @{
 */

#if 0
/** \brief  Structure type to access the System Timer (SysTick).
 */
typedef struct
{
  __IO uint32_t CTRL;                    /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  __IO uint32_t LOAD;                    /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register       */
  __IO uint32_t VAL;                     /*!< Offset: 0x008 (R/W)  SysTick Current Value Register      */
  __I  uint32_t CALIB;                   /*!< Offset: 0x00C (R/ )  SysTick Calibration Register        */
} SysTick_Type;
#endif

/* SysTick Control / Status Register Definitions */
#define SysTick_CTRL_COUNTFLAG_Pos         16                                             /*!< SysTick CTRL: COUNTFLAG Position */
#define SysTick_CTRL_COUNTFLAG_Msk         (1UL << SysTick_CTRL_COUNTFLAG_Pos)            /*!< SysTick CTRL: COUNTFLAG Mask */

#define SysTick_CTRL_CLKSOURCE_Pos          2                                             /*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk         (1UL << SysTick_CTRL_CLKSOURCE_Pos)            /*!< SysTick CTRL: CLKSOURCE Mask */

#define SysTick_CTRL_TICKINT_Pos            1                                             /*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk           (1UL << SysTick_CTRL_TICKINT_Pos)              /*!< SysTick CTRL: TICKINT Mask */

#define SysTick_CTRL_ENABLE_Pos             0                                             /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk            (1UL /*<< SysTick_CTRL_ENABLE_Pos*/)           /*!< SysTick CTRL: ENABLE Mask */

/* SysTick Reload Register Definitions */
#define SysTick_LOAD_RELOAD_Pos             0                                             /*!< SysTick LOAD: RELOAD Position */
#define SysTick_LOAD_RELOAD_Msk            (0xFFFFFFUL /*<< SysTick_LOAD_RELOAD_Pos*/)    /*!< SysTick LOAD: RELOAD Mask */

/* SysTick Current Register Definitions */
#define SysTick_VAL_CURRENT_Pos             0                                             /*!< SysTick VAL: CURRENT Position */
#define SysTick_VAL_CURRENT_Msk            (0xFFFFFFUL /*<< SysTick_VAL_CURRENT_Pos*/)    /*!< SysTick VAL: CURRENT Mask */

/* SysTick Calibration Register Definitions */
#define SysTick_CALIB_NOREF_Pos            31                                             /*!< SysTick CALIB: NOREF Position */
#define SysTick_CALIB_NOREF_Msk            (1UL << SysTick_CALIB_NOREF_Pos)               /*!< SysTick CALIB: NOREF Mask */

#define SysTick_CALIB_SKEW_Pos             30                                             /*!< SysTick CALIB: SKEW Position */
#define SysTick_CALIB_SKEW_Msk             (1UL << SysTick_CALIB_SKEW_Pos)                /*!< SysTick CALIB: SKEW Mask */

#define SysTick_CALIB_TENMS_Pos             0                                             /*!< SysTick CALIB: TENMS Position */
#define SysTick_CALIB_TENMS_Msk            (0xFFFFFFUL /*<< SysTick_CALIB_TENMS_Pos*/)    /*!< SysTick CALIB: TENMS Mask */

/*@} end of group CMSIS_SysTick */




/* Memory mapping of Core Hardware */
#define SCS_BASE            (0xE000E000UL)                            /*!< System Control Space Base Address */
#define SysTick_BASE        (SCS_BASE +  0x0010UL)                    /*!< SysTick Base Address */
#define NVIC_BASE           (SCS_BASE +  0x0100UL)                    /*!< NVIC Base Address */
#define SCB_BASE            (SCS_BASE +  0x0D00UL)                    /*!< System Control Block Base Address */

#define SCB                 ((SCB_Type       *)     SCB_BASE      )   /*!< SCB configuration struct */
#define SysTick             ((SysTick_Type   *)     SysTick_BASE  )   /*!< SysTick configuration struct */
#define NVIC                ((NVIC_Type      *)     NVIC_BASE     )   /*!< NVIC configuration struct */






extern void hal_nvic_set_priority(IRQn_Type irq_number, uint32 priority);
extern void hal_nvic_enable_irq(IRQn_Type irq_number);
extern void hal_nvic_disable_irq(IRQn_Type irq_number);

extern uint32_t hal_nvic_get_pending_irq(IRQn_Type irq_number);
extern void hal_nvic_set_pending_irq(IRQn_Type irq_number);
extern void hal_nvic_clear_pending_irq(IRQn_Type irq_number);

extern uint32_t SysTick_Config(uint32_t ticks);
extern uint32_t Get_SysTick(void);

