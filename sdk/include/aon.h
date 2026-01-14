
#include "platform_config.h"
//`define ADDR_BASE_APB_AON       32'h4002_0000
#define GECKO_AON_BASE_ADDR             0x40020000//New for 1109
#define ADDR_BASE_GECKO_AON_CONFIG      0x40020000//New for 1109


//------------------------------------------------------------------
//  Analog control macro definition
//------------------------------------------------------------------
#define OFFSET_REG_AON_AON_FSM_CTRL                 0x000
#define OFFSET_REG_AON_AON_STATUS                   0x004
#define OFFSET_REG_AON_AON_SLEEP_CNT                0x008
#define OFFSET_REG_AON_AON_SLEEP_CNT_RBK            0x00C

//add by JF 20210822
#define OFFSET_REG_AON_GPIO_WAKEUP                  0x010

#define OFFSET_REG_AON_ANA_CTRL_1                   0x020
#define OFFSET_REG_AON_ANA_CTRL_2                   0x024

//add by JF 20210702
#define OFFSET_REG_AON_RSV_ANA_CTRL                    0x028
#define OFFSET_REG_AON_RSV_DIG_CTRL_1                  0x02C
#define OFFSET_REG_AON_RSV_DIG_CTRL_2                  0x030

/*
"GEK1109 added a AON register
Default 2'b01: 1/2 of OSC - 16MHz"
*/
#define OFFSET_REG_AON_CTRL_EXT0										0x70 //Add for 1109,New
#define aon2cmu_clk_div(n)													(((n)&0x3)<<0) //bit0-bit1,2bits,default value=0x01
#define ADDR_AON_CFG_CTRL_EXT0                      GECKO_AON_BASE_ADDR + OFFSET_REG_AON_CTRL_EXT0




#define ADDR_AON_CFG_AON_FSM_CTRL                   GECKO_AON_BASE_ADDR + OFFSET_REG_AON_AON_FSM_CTRL
#define ADDR_AON_CFG_AON_STATUS                     GECKO_AON_BASE_ADDR + OFFSET_REG_AON_AON_STATUS
#define ADDR_AON_CFG_AON_SLEEP_CNT                  GECKO_AON_BASE_ADDR + OFFSET_REG_AON_AON_SLEEP_CNT
#define ADDR_AON_CFG_AON_SLEEP_CNT_RBK              GECKO_AON_BASE_ADDR + OFFSET_REG_AON_AON_SLEEP_CNT_RBK

#define ADDR_AON_CFG_AON_GPIO_WAKEUP                ADDR_BASE_GECKO_AON_CONFIG + 0x010

#define ADDR_AON_CFG_ANA_CTRL_1                     GECKO_AON_BASE_ADDR + OFFSET_REG_AON_ANA_CTRL_1
#define ADDR_AON_CFG_ANA_CTRL_2                     GECKO_AON_BASE_ADDR + OFFSET_REG_AON_ANA_CTRL_2

//add by JF 20210702
#define ADDR_AON_CFG_GPIO_WAKEUP_CTRL                   GECKO_AON_BASE_ADDR + OFFSET_REG_AON_GPIO_WAKEUP

#define ADDR_AON_CFG_RSV_ANA_CTRL                       GECKO_AON_BASE_ADDR + OFFSET_REG_AON_RSV_ANA_CTRL
#define ADDR_AON_CFG_RSV_DIG_CTRL_1                     GECKO_AON_BASE_ADDR + OFFSET_REG_AON_RSV_DIG_CTRL_1
#define ADDR_AON_CFG_RSV_DIG_CTRL_2                     GECKO_AON_BASE_ADDR + OFFSET_REG_AON_RSV_DIG_CTRL_2




//`define OFFSET_AON_RET_RAM_0  16'h0040
//`define OFFSET_AON_RET_RAM_1  16'h0044
//`define OFFSET_AON_RET_RAM_2  16'h0048
//`define OFFSET_AON_RET_RAM_3  16'h004C
//`define OFFSET_AON_RET_RAM_4  16'h0050
//`define OFFSET_AON_RET_RAM_5  16'h0054
//`define OFFSET_AON_RET_RAM_6  16'h0058
//`define OFFSET_AON_RET_RAM_7  16'h005C

#define ADDR_AON_RETENTION_REG_0                     GECKO_AON_BASE_ADDR + 0x40
#define ADDR_AON_RETENTION_REG_1                     GECKO_AON_BASE_ADDR + 0x44
#define ADDR_AON_RETENTION_REG_2                     GECKO_AON_BASE_ADDR + 0x48
#define ADDR_AON_RETENTION_REG_3                     GECKO_AON_BASE_ADDR + 0x4C
#define ADDR_AON_RETENTION_REG_4                     GECKO_AON_BASE_ADDR + 0x50
#define ADDR_AON_RETENTION_REG_5                     GECKO_AON_BASE_ADDR + 0x54
#define ADDR_AON_RETENTION_REG_6                     GECKO_AON_BASE_ADDR + 0x58
#define ADDR_AON_RETENTION_REG_7                     GECKO_AON_BASE_ADDR + 0x5C





//  ADDR_AON_CFG_AON_FSM_CTRL bit offset
#define OFFSET_AON_FSM_CTRL_SLEEP_CNT_RUN           8
#define OFFSET_AON_FSM_CTRL_SLEEP_CNT_WKUP          7
#define OFFSET_AON_FSM_CTRL_ADC_WAKEUP              6
#define OFFSET_AON_FSM_CTRL_PIN_WAKEUP              5
#define OFFSET_AON_FSM_CTRL_ENA_WAKEUP_INT          4
#define OFFSET_AON_FSM_CTRL_ENA_LITESLEEP           3
#define OFFSET_AON_FSM_CTRL_ENA_DEEPSLEEP           2
#define OFFSET_AON_FSM_CTRL_WAIT_PU_CNTTO           0

//AON FSM CTRL Register
//register 0x00
#define  WAIT_PU_CNTTO(n)          (((n)&0x3)<<0)  //2bits Wait OSC32M and LDO ready counter
#define  EN_DEEP_SLEEP						 (1<<2)
#define  EN_LITE_SLEEP						 (1<<3)
#define  EN_WAKEUP_INT						 (1<<4)
#define  EN_PIN_WAKEUP             (1<<5)
#define  EN_ADC_WAKEUP             (1<<6)
#define  EN_SLEEP_CNT_WAKEUP       (1<<7)
#define  EN_SLEEP_CNT_RUN          (1<<8)					//enable sleep counter running when sleep

#define  SEL_AON_CLK16K            (1<<10)				//sel_aon_clk16k

/*
gek1109 aon_regsync.v

assign reg_aon_sel_aon_clk16k  			=reg_0x000[10];
assign reg_aon_wait_ana_cntto  			=reg_0x000[9:8];
assign reg_aon_ena_pin_wakeup  			=reg_0x000[7];
assign reg_aon_ena_rtc_wakeup  			=reg_0x000[6];
assign reg_aon_ena_sleep_cnt_wkup		=reg_0x000[5];
assign reg_aon_ena_sleep_cnt_run		=reg_0x000[4];
assign reg_aon_ena_litesleep				=reg_0x000[3];
assign reg_aon_ena_deepsleep				=reg_0x000[2];
assign reg_aon_wait_pu_cntto				=reg_0x000[1:0];
*/

//GEK1109 AON FSM CTRL Register
//register 0x00
#define  GEK1109_SEL_AON_CLK16K            (1<<10)				//sel_aon_clk16k
#define  GEK1109_WAIT_ANA_CNTTO(n)         (((n)&0x3)<<8) //2bits Wait OSC32M and LDO ready counter
#define  GEK1109_PIN_WAKEUP_Enable         (1<<7)
#define  GEK1109_RTC_WAKEUP_Enable         (1<<6)
#define  GEK1109_SLEEP_CNT_WKUP_Enable     (1<<5)					//?
#define  GEK1109_SLEEP_CNT_RUN_Enable      (1<<4)					//enable sleep counter running when sleep
#define  GEK1109_LITE_SLEEP_Enable				 (1<<3)
#define  GEK1109_DEEP_SLEEP_Enable				 (1<<2)
#define  GEK1109_WAIT_PU_CNTTO(n)          (((n)&0x3)<<0) //wait power up couter ?



//AON Status Register
//register 0x04

#define WAKEUP_INT                 (1<<0)
#define POR_STATUS                 (1<<1)




//AON SLEEP_CNT Register
//register 0x08
#define SLEEP_CNT_VAL(n)					(((n)&0xFFFFFF)<<0)   //24 bits


//AON_WAKEUP_STATUS
//register 0x0c
//Sleep counter value read-back by SW


//GPIO_WAKEUP
//register 0x10

#define GPIO_WAKEUP_EN_00                 (1<<0)
#define GPIO_WAKEUP_EN_01                 (1<<1)
#define GPIO_WAKEUP_EN_02                 (1<<2)
#define GPIO_WAKEUP_EN_03                 (1<<3)
#define GPIO_WAKEUP_EN_04                 (1<<4)
#define GPIO_WAKEUP_EN_05                 (1<<5)
#define GPIO_WAKEUP_EN_06                 (1<<6)
#define GPIO_WAKEUP_EN_07                 (1<<7)
#define GPIO_WAKEUP_EN_08                 (1<<8)
#define GPIO_WAKEUP_EN_09                 (1<<9)
#define GPIO_WAKEUP_EN_10                 (1<<10)
#define GPIO_WAKEUP_EN_11                 (1<<11)
#define GPIO_RESERVED(n)                  (((n)&0xF)<<12) //4 bits reserved
#define GPIO_WAKEUP_STA_00                (1<<16)
#define GPIO_WAKEUP_STA_01                (1<<17)
#define GPIO_WAKEUP_STA_02                (1<<18)
#define GPIO_WAKEUP_STA_03                (1<<19)
#define GPIO_WAKEUP_STA_04                (1<<20)
#define GPIO_WAKEUP_STA_05                (1<<21)
#define GPIO_WAKEUP_STA_06                (1<<22)
#define GPIO_WAKEUP_STA_07                (1<<23)
#define GPIO_WAKEUP_STA_08                (1<<24)
#define GPIO_WAKEUP_STA_09                (1<<25)
#define GPIO_WAKEUP_STA_10                (1<<26)
#define GPIO_WAKEUP_STA_11                (1<<27)


#define GPIOA4_WAKEUP_EN_00                 (1<<0)
#define GPIOA5_WAKEUP_EN_01                 (1<<1)
#define GPIOA6_WAKEUP_EN_02                 (1<<2)
#define GPIOA7_WAKEUP_EN_03                 (1<<3)
#define GPIOB0_WAKEUP_EN_04                 (1<<4)
#define GPIOB1_WAKEUP_EN_05                 (1<<5)
#define GPIOB2_WAKEUP_EN_06                 (1<<6)
#define GPIOB3_WAKEUP_EN_07                 (1<<7)
#define GPIOB4_WAKEUP_EN_08                 (1<<8)
#define GPIOB5_WAKEUP_EN_09                 (1<<9)
#define GPIOB6_WAKEUP_EN_10                 (1<<10)
#define GPIOB7_WAKEUP_EN_11                 (1<<11)



//ANA_CTRL_1
//register 0x20
//default value 0x808E7885
#define LDO18_EN                 (1<<0)
#define LDO18_VSET(n)            (((n)&0x3)<<1)  //2 bits
#define LDO_FLASH_VSET(n)        (((n)&0x3)<<3)  //2 bits,//VSET<1:0> 00-->3.3V;01-->3.2V;10-->3.4V;11-->3.5V
#define LPBG_VTRIM(n)            (((n)&0x7)<<5)  //3 bits
#define RCO32K_FC(n)						 (((n)&0xF)<<8)  //4 bits

#define EN_OSC32M                (1<<12)

#define PWRMOS_G(n)              (((n)&0x3)<<13)  //2 bits Power PMOS Gate


#define LDO1_VSET(n)             (((n)&0x3)<<15)  //2 bits,2b'00,3.3V;2b'01,3.2V;2b'10,3.4V;2b'11,3.5V;
#define LDO1_EN                  (1<<17)
#define BG_EN                    (1<<18)
#define RCO32M_EN_16M            (1<<19)
#define RCO32M_TC(n)						 (((n)&0xF)<<20)  //4 bits
#define RCO32M_FREQ(n)					 (((n)&0xFF)<<24) //8 bits


//ANA_CTRL_2
//register 0x24

#define SW_FORCE_BG_EN            (1<<0)
#define SW_FORCE_LDO18_EN         (1<<1)
#define SW_FORCE_LDO1_EN          (1<<2)
#define SW_FORCE_OSC32M_EN        (1<<3)
#define SW_FORCE_CALI32M_TUNE     (1<<4) //Debug 2022-01-14 ZJF

//#define SW_FORCE_CALI32M_TUNE     (((n)&0xF)<<4)  //4 bits  SW force select 32Mhz calibration tune value


//RSV_ANA_CTRL 
//register 0x28



//RSV_DIG_CTRL_1
//register 0x2C
#define AON_IOMUX_SEL_1(n)     (((n)&0xFFF)<<0)   //12 bits, bit0---bit11
#define AON_IOMUX_SEL_2(n)     (((n)&0xFFF)<<16)  //12 bits, bit16---bit27



//RSV_DIG_CTRL_2
//REG_AON_RSV_DIG_CTRL_2
//register 0x30
//Pad Pull function

//#define AON_PAD_PREN(n)				 (((n)&0xFFF)<<0)   //12 bits, bit0---bit11
//#define AON_RSV_DIG_CTRL(n)		 (((n)&0xFFFF)<<16)  //16 bits, bit16---bit31


/*
gek1109 aon_regsync.v

assign reg_aon_sel_aon_clk16k  			=reg_0x000[10];
assign reg_aon_wait_ana_cntto  			=reg_0x000[9:8];
assign reg_aon_ena_pin_wakeup  			=reg_0x000[7];
assign reg_aon_ena_rtc_wakeup  			=reg_0x000[6];
assign reg_aon_ena_sleep_cnt_wkup		=reg_0x000[5];
assign reg_aon_ena_sleep_cnt_run		=reg_0x000[4];
assign reg_aon_ena_litesleep				=reg_0x000[3];
assign reg_aon_ena_deepsleep				=reg_0x000[2];
assign reg_aon_wait_pu_cntto				=reg_0x000[1:0];

assign reg_aon_ena_wakeup_int				=reg_0x004[0];

assign reg_aon_gpio_wakeup_ena			=reg_0x010[11:0];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//low-power clock domain(slow-clock domain)
//assign gpio_wkup_edge = reg_aon_gpio_wkup_edge ? ((~gpio_pad_in_sync[2]) & gpio_pad_in_sync[1] :   //GPIO input posedge
//																								   gpio_pad_in_sync[2]  & (~gpio_pad_in_sync[1])); //GPIO input negedge

//assign gpio_wkup_levl = reg_aon_gpio_wkup_lvl ? (gpio_pad_in_sync[2] & gpio_pad_in_sync[1] :    	 //GPIO input 2x AON cycle high-level
//																								(~gpio_pad_in_sync[2])  & (~gpio_pad_in_sync[1])); //GPIO input 2x AON cycle low-level

//gpio_wkup_active = reg_aon_gpio_wkup_eact ? gpio_wkup_edge :gpio_wkup_level;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

assign reg_aon_gpio_wakeup_eact			=reg_0x014[11:0];
assign reg_aon_gpio_wakeup_edge			=reg_0x018[11:0];
assign reg_aon_gpio_wakeup_lvl			=reg_0x01C[11:0];

assign reg_aon_sleep_cnt_val			  =reg_0x060[23:0];

*/




//register 0x090

#define LOWPOWER_MODE_OPTM_EN   (1<<7)
#define AON_LDO_FLASH_BYPASS_EN (1<<6)
#define AON_LDO_FLASH_EN        (1<<5)




void LDO33_AUX_Enable(void);
void LDO33_AUX_Disable(void);

void Analog_Ctrl_1_Init(void);
void Analog_Ctrl_2_Init(void);

void PWR_MOS_Gate_CTRL(uint8 gate);


void manba_task_cpu_goto_sleep (void);
void tc_gecko_cm0_aon_sleep(void);
//void tc_gecko_aon_sanity(void);
void tc_gecko_aon_sanity_cm0(void);
void tc_qspi_to_xtx_sanity(void);//read chip id

void gecko_aon_wakeup_irq_handler(void);
void manba_task_clr_aon_wakeup_int (void);

void GPIO_WakeUp_Setting(void);
void BSR1901_GPIO_WakeUp_From_DeepSleep(void);
void Osc32M_Freq_Trimming(void);


//SPI-to-AHB config to Dual-wire access NOR Flash
void gecko_task_cm0_sw_flash(void);

void GEK_Retention_Reg_Access(void);


void gecko_rootnode_clk32k_cfg(void);
void gecko_rootnode_osc32mclk_cfg(void);
void gecko_branchnode_pclkout_cfg(void);

void bsr1901_adc_8_9_analog_port(void);

void aon_wakeup_irq_cfg(void);
	
