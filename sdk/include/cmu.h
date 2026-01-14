
/****************************************************************************
 *   $Id:: cmu.c 2020-09-25 
 *   Project: Gecko Clock & Reset Management Unit
 *
 *   Description:System Clock Unit
 * 
****************************************************************************/

#include "platform_config.h"


#define    GECKO_APB_CMU_BASE                0X40010000
#define    GECKO_APB_SCU_BASE                0X40010000

      
typedef enum
{
    SYS_FREQ_32K     			= 32768,
    SYS_FREQ_4M      			= 4000000,
    SYS_FREQ_8M     			= 8000000,
    SYS_FREQ_16M     			= 16000000,
    SYS_FREQ_32M     			= 32000000,
    SYS_FREQ_48M     			= 48000000,
    SYS_FREQ_UNKNOWN = 0
} SYS_FREQ_T;  

/*--------------------- Clock Configuration ----------------------------------*/
/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/

#define IRC_OSC     (8000000UL)        /* Internal RC oscillator frequency   */


/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
//#define IRC_OSC     (16000000UL)        /* Internal RC oscillator frequency   */


#define OSC_CLK       (  IRC_OSC)        /* Main oscillator frequency          */



/*
assign  reg32_1c[31:24] = cali32m_result[7:0];
assign  reg32_1c[23:16] = cali32m_golden_cycle[7:0];
assign  reg32_1c[15:10] = 6'h0;
assign  reg32_1c[9]     = cali32m_busy;
assign  reg32_1c[8]     = 1'b0;
assign  reg32_1c[7:3]   = 5'h0;
assign  reg32_1c[2]     = cali32m_direct;
assign  reg32_1c[1]     = cali32m_enable;
assign  reg32_1c[0]     = cmu2ahb_wr_req;
*/

#define cmu2ahb_wr_req						(1<<0)
#define cali32m_enable            (1<<1)
#define cali32m_direct					  (1<<2)
#define Scali32m_busy             (1<<9)

#define golden_cycle(n)					(((n)&0xFF)<<0)



typedef volatile struct
{

    REG32                          CLK_DIV;                      //0x00000000
    REG32                          SYS_CLK_EN;                   //0x00000004
		REG32                          Sys_Soft_Rst_N;               //0x00000008
    REG32                          CLK_DIV_2;                    //0x0000000C	
    REG32                          CLK_DIV_3;                    //0x00000010		

	  REG32                          REG32_0x14;                    //0x00000014		
	  REG32                          REG32_0x18;                    //0x00000018	

//	  REG32                          REG32_0x1C;                    //0x0000001C
    REG32                          cali32m;                    	 //0x0000001C		
	  REG32                          REG32_0x20;                    //0x00000020

	  REG32                          REG32_0x24;                    //0x00000024	
	  REG32                          REG32_0x28;                    //0x00000028
	
	  REG32                          REG32_0x2C;                    //0x0000002C	
	  REG32                          REG32_0x30;                    //0x00000030
	
} HW_SCU_CTRL_T;


#define HW_SysClkUnit                 ((HW_SCU_CTRL_T*) GECKO_APB_CMU_BASE)
#define cmu               					  ((HW_SCU_CTRL_T*) GECKO_APB_CMU_BASE)



//static struct CMU_T * const cmu = (struct CMU_T *)0x40000000;
//static struct HW_SCU_CTRL_T * const cmu = (struct HW_SCU_CTRL_T *)0x40000000;



/*
* reg32_00 Register bits
*
assign hclk_free_div                    = reg32_00[7:0];
assign uart_clk_div                     = reg32_00[15:8];
assign pwm_clk_div                      = reg32_00[23:16];
assign spinor_clk_div                   = reg32_00[31:24];
*
*/

#define HCLK_FREE_DIV(n)					(((n)&0xFF)<<0)
#define UART_CLK_DIV(n)           (((n)&0xFF)<<8)
#define PWM_CLK_DIV(n)					  (((n)&0xFF)<<16)
#define SPINOR_CLK_DIV(n)         (((n)&0xFF)<<24)

/*
* reg32_0C Register bits
*
assign adc_clk_div                      = reg32_0c[7:0];
assign i2c_clk_div                      = reg32_0c[15:8];
assign timer_clk_div                    = reg32_0c[23:16];
assign lightning_clk_div                = reg32_0c[31:24];
*
*/

#define ADC_CLK_DIV(n)						(((n)&0xFF)<<0)
#define I2C_CLK_DIV(n)          	(((n)&0xFF)<<8)
#define TIMER_CLK_DIV(n)					(((n)&0xFF)<<16)
#define LIGHTNING_CLK_DIV(n)      (((n)&0xFF)<<24)


/*
* reg32_10 Register bits
*
assign efuse_clk_div                    = reg32_10[7:0];
assign wdt_clk_div                      = reg32_10[15:8];
assign gpio_clk_div                     = reg32_10[23:16];
assign uart2_clk_div                    = reg32_10[31:24];
*
*/


#define EFUSE_CLK_DIV(n)						(((n)&0xFF)<<0)
#define WDT_CLK_DIV(n)          	  (((n)&0xFF)<<8)
#define GPIO_CLK_DIV(n)					    (((n)&0xFF)<<16)
#define UART2_CLK_DIV(n)					  (((n)&0xFF)<<24)



/*
 * reg32_04 Register bits
 */

#define HCLK_FREE_BYPASS_EN           0
#define UART_CLK_BYPASS_EN            1
#define PWM_CLK_BYPASS_EN             2
#define SPINOR_CLK_BYPASS_EN          3
#define EN_HCLK_ROM                 	4
#define EN_HCLK_RAM                   5
#define EN_HCLK_UART                  6

#define EN_HCLK_I2C                   7 //Disable I2C Module for Reduce running power consumption
#define EN_HCLK_GPIO                  8
#define EN_HCLK_PCLK                  9
#define EN_HCLK_FLASH                 10
#define EN_PCLK_TIMER0                11
#define EN_PCLK_TIMER1                12
#define EN_PCLK_GPIO   								13
#define EN_PCLK_WDT                 	14 //Disable WDT Module for Reduce running power consumption
#define EN_PCLK_PWM                   15
#define EN_PCLK_EFUSE                 16 //Disable EFUSE Module for Reduce running power consumption
#define EN_PCLK_ADC                   17
#define EN_PCLK_LIGHTNING             18 //Disable LIGHTNING Module for Reduce running power consumption

#define RESERVED1                    19
#define EN_UART_CLK                   20
#define EN_PWM_CLK                 		21
#define EN_SPINOR_CLK                 22
#define EN_WDT_CLK                    23
#define EN_TIMER0_CLK                 24
#define EN_TIMER1_CLK                 25
#define RESERVED2                    26
#define EN_LIGHTNING_CLK              27
#define EN_EFUSE_CLK              		28
#define EN_I2C_CLK              		 	29
#define EN_PCLK_GPIO1             		30
#define RESERVED3                    31


/*
 * reg32_08 Register bits
 */

#define SOFT_RESETN_HCLK                0
#define SOFT_RESETN_UART                1
#define SOFT_RESETN_PWM                 2
#define SOFT_RESETN_WDT                 3 
 
#define SOFT_RESETN_SPINOR              4
#define SOFT_RESETN_TIMER0              5
#define SOFT_RESETN_TIMER1              6
#define SOFT_RESETN_ADC                 7
#define SOFT_RESETN_LIGHTNING           8
#define SOFT_RESETN_I2C                 9
#define RESERVER00                    10
#define SOFT_RESETN_EFUSE           		11
#define RESERVER01                    12
#define SOFT_RESETN_GPIO           		  13
#define SOFT_RESETN_UART2           		14

#define uart2_clk_bypass_en           15
#define en_hclk_uart2                 16
#define en_uart2_clk                  17

/*
 * reg32_28 Register bits
 */
 
#define GPIO_SEL(n)						(((n)&0xFFFF)<<0)


/*
 * reg32_18 Register bits
 *assign cmu_rsv_ctrl     = reg32_18[31:0];
 */

//assign cmu_rsv_ctrl     = reg32_18[31:0];

//assign  pad_00_pren = cmu_rsv_ctrl[0];
//assign  pad_01_pren = cmu_rsv_ctrl[1];
//assign  pad_02_pren = cmu_rsv_ctrl[2];
//assign  pad_03_pren = cmu_rsv_ctrl[3];
//assign  pad_04_pren = cmu_rsv_ctrl[4];
//assign  pad_05_pren = cmu_rsv_ctrl[5];
//assign  pad_06_pren = cmu_rsv_ctrl[6];
//assign  pad_07_pren = cmu_rsv_ctrl[7];

//assign  pad_20_pren = cmu_rsv_ctrl[8];
//assign  pad_21_pren = cmu_rsv_ctrl[9];

//#define CMU_RSV_CTRL_PAD_PULL(n)						(((n)&0x3FF)<<0)   //10 bits for 3p3 demain pad "pull function" config




/*
 * reg32_30 Register bits
 */
#define DAC_CLK_DIV(n)					  (((n)&0xFF)<<16)


//================================================================================================================
//
//
//================================================================================================================





#define CMU_HCLK_MCU                (1 << 0)
#define CMU_HCLK_ROM                (1 << 1)
#define CMU_HCLK_RAM0               (1 << 2)
#define CMU_HCLK_RAM1               (1 << 3)
#define CMU_HCLK_AHB0               (1 << 4)
#define CMU_HCLK_AHB1               (1 << 5)
#define CMU_HCLK_AH2H               (1 << 6)
#define CMU_HCLK_AH2H_BT            (1 << 7)
#define CMU_HCLK_ADMA               (1 << 8)
#define CMU_HCLK_GDMA               (1 << 9)
#define CMU_HCLK_PSRAM              (1 << 10)
#define CMU_HCLK_FLASH              (1 << 11)
#define CMU_HCLK_SDIO               (1 << 12)
#define CMU_HCLK_SDMMC              (1 << 13)
#define CMU_HCLK_USBC               (1 << 14)
#define CMU_HCLK_DPDRX              (1 << 15)
#define CMU_HCLK_DPDTX              (1 << 16)

#define CMU_PCLK_CMU                (1 << 0)
#define CMU_PCLK_GPIO               (1 << 1)
#define CMU_PCLK_GPIO_INT           (1 << 2)
#define CMU_PCLK_WDT                (1 << 3)
#define CMU_PCLK_PWM                (1 << 4)
#define CMU_PCLK_TIMER              (1 << 5)
#define CMU_PCLK_I2C                (1 << 6)
#define CMU_PCLK_SPI                (1 << 7)
#define CMU_PCLK_SLCD               (1 << 8)
#define CMU_PCLK_UART0              (1 << 9)
#define CMU_PCLK_UART1              (1 << 10)
#define CMU_PCLK_CODEC              (1 << 11)
#define CMU_PCLK_PCM                (1 << 12)
#define CMU_PCLK_I2S                (1 << 13)
#define CMU_PCLK_SPI_ITN            (1 << 14)
#define CMU_PCLK_IOMUX              (1 << 15)
#define CMU_PCLK_SPDIF              (1 << 16)


#define CMU_HRST_MCU                (1 << 0)
#define CMU_HRST_ROM                (1 << 1)
#define CMU_HRST_RAM0               (1 << 2)
#define CMU_HRST_RAM1               (1 << 3)
#define CMU_HRST_AHB0               (1 << 4)
#define CMU_HRST_AHB1               (1 << 5)
#define CMU_HRST_AH2H               (1 << 6)
#define CMU_HRST_AH2H_BT            (1 << 7)
#define CMU_HRST_ADMA               (1 << 8)
#define CMU_HRST_GDMA               (1 << 9)
#define CMU_HRST_PSRAM              (1 << 10)
#define CMU_HRST_FLASH              (1 << 11)
#define CMU_HRST_SDIO               (1 << 12)
#define CMU_HRST_SDMMC              (1 << 13)
#define CMU_HRST_USBC               (1 << 14)
#define CMU_HRST_DPDRX              (1 << 15)
#define CMU_HRST_DPDTX              (1 << 16)

#define CMU_PRST_CMU                (1 << 0)
#define CMU_PRST_GPIO               (1 << 1)
#define CMU_PRST_GPIO_INT           (1 << 2)
#define CMU_PRST_WDT                (1 << 3)
#define CMU_PRST_PWM                (1 << 4)
#define CMU_PRST_TIMER              (1 << 5)
#define CMU_PRST_I2C                (1 << 6)
#define CMU_PRST_SPI                (1 << 7)
#define CMU_PRST_SLCD               (1 << 8)
#define CMU_PRST_UART0              (1 << 9)
#define CMU_PRST_UART1              (1 << 10)
#define CMU_PRST_CODEC              (1 << 11)
#define CMU_PRST_PCM                (1 << 12)
#define CMU_PRST_I2S                (1 << 13)
#define CMU_PRST_SPI_ITN            (1 << 14)
#define CMU_PRST_IOMUX              (1 << 15)
#define CMU_PRST_SPDIF              (1 << 16)
#define CMU_PRST_GLOBAL             (1 << 17)


// TIMER_CLK
#define CMU_TIMER1_DIV(n)           (((n) & 0xFFFF) << 0)
#define CMU_TIMER1_DIV_MASK         (0xFFFF << 0)
#define CMU_TIMER1_DIV_SHIFT        (0)
#define CMU_TIMER2_DIV(n)           (((n) & 0xFFFF) << 16)
#define CMU_TIMER2_DIV_MASK         ((uint32)0xFFFF << 16)
#define CMU_TIMER2_DIV_SHIFT        (16)




// SYS_CLK
#define CMU_SEL_32K_FLASH           (1 << 0)
#define CMU_SEL_13M_FLASH           (1 << 1)
#define CMU_SEL_26M_FLASH           (1 << 2)
#define CMU_SEL_52M_FLASH           (1 << 3)
#define CMU_SEL_PLLUSB_FLASH        (1 << 4)
#define CMU_BYPASS_LOCK_FLASH       (1 << 5)
#define CMU_BYPASS_DIV_FLASH        (1 << 6)
#define CMU_CFG_DIV_FLASH(n)        (((n) & 3) << 7)
#define CMU_CFG_DIV_FLASH_MASK      (3 << 7)
#define CMU_CFG_DIV_FLASH_SHIFT     (7)
#define CMU_SEL_32K_MEM             (1 << 9)
#define CMU_SEL_13M_MEM             (1 << 10)
#define CMU_SEL_26M_MEM             (1 << 11)
#define CMU_SEL_52M_MEM             (1 << 12)
#define CMU_SEL_PLLUSB_MEM          (1 << 13)
#define CMU_BYPASS_LOCK_MEM         (1 << 14)
#define CMU_BYPASS_DIV_MEM          (1 << 15)
#define CMU_CFG_DIV_MEM(n)          (((n) & 3) << 16)
#define CMU_CFG_DIV_MEM_MASK        (3 << 16)
#define CMU_CFG_DIV_MEM_SHIFT       (16)
#define CMU_SEL_32K_SYS             (1 << 18)
#define CMU_SEL_13M_SYS             (1 << 19)
#define CMU_SEL_26M_SYS             (1 << 20)
#define CMU_SEL_52M_SYS             (1 << 21)
#define CMU_SEL_PLLUSB_SYS          (1 << 22)
#define CMU_BYPASS_LOCK_SYS         (1 << 23)
#define CMU_BYPASS_DIV_SYS          (1 << 24)
#define CMU_CFG_DIV_SYS(n)          (((n) & 3) << 25)
#define CMU_CFG_DIV_SYS_MASK        (3 << 25)
#define CMU_CFG_DIV_SYS_SHIFT       (25)
#define CMU_EN_PLL_AUD              (1 << 27)
#define CMU_EN_PLL_USB              (1 << 28)
#define CMU_PU_PLL_AUD              (1 << 29)
#define CMU_PU_PLL_USB              (1 << 30)
#define CMU_LOCK_CPU_EN             (1 << 31)





void gecko_cmu_init(void);
void gecko_cmu_reg_rw(void);
void cali32m_initial(void);


