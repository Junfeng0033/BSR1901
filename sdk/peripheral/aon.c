#include "platform_config.h"
#include "bsr1901.h"

#include "aon.h"
#include "../norflash/xtxflash/dpi_common.h"

#include "uart.h"
extern void h2l_wr_busy(void);

extern void gecko_aon_wakeup_irq_handler(void);

extern void manba_task_clr_aon_wakeup_int (void);

extern void manba_task_cpu_goto_sleep(void);
extern void manba_task_cpu_goto_sleep (void);

extern void tc_gecko_aon_sanity_cm0(void);//deep sleep
extern void tc_gecko_cm0_aon_sleep(void);

//switch to quad spi nor flash mode
extern void gecko_task_cm0_sw_flash_quad(void);

//SPI-to-AHB config to Dual-wire access NOR Flash
extern void gecko_task_cm0_sw_flash(void);
extern void delay_1us(unsigned int delay_val);

//#define MREAD_32(addr) *((volatile UINT32 *)(addr))
//#define MWRITE_32(addr,data)  { *((volatile UINT32 *)(addr)) = data; }
#define ANA_READ_CTLREG(REG)      				(*(volatile unsigned int*)(REG))
#define ANA_WRITE_CTLREG(REG,VAL)    			(*(volatile unsigned int*)(REG)) = (unsigned int)(VAL) 


uint32 AON_CFG_ANA_CTRL_1_0x20_Default=0x808E7885;//
uint32 AON_CFG_ANA_CTRL_2_0x24_Default=0;
uint32 AON_CFG_GPIO_WAKEUP_CTRL_0x10_Default=0;//



//There is a bug in AON register reading	
void Analog_Ctrl_1_Init(void)
{
	unsigned int wr_data;
	//char *string;	

	//wr_data = reg_read(ADDR_AON_CFG_ANA_CTRL_1);
	wr_data = ANA_READ_CTLREG(ADDR_AON_CFG_ANA_CTRL_1);//0x4001A000+0x20
	wr_data =(*(volatile unsigned int*)(0x4001A020));
	wr_data = AON_CFG_ANA_CTRL_1_0x20_Default;////There is a bug in AON register reading	
	
	//enable ldo
	//wr_data |= (LDO18_EN |EN_OSC32M |LDO1_VSET(3)| LDO1_EN | BG_EN);
	//wr_data |= LDO_FLASH_VSET(3);//VSET<1:0> 00-->3.3V;01-->3.2V;10-->3.4V;11-->3.5V
	//#define LDO_FLASH_VSET(n)        (((n)&0x3)<<3)  //2 bits,(bit3 and bit4)
	wr_data  = wr_data | 0x00000018;//(bit3 and bit4)
  reg_write(ADDR_AON_CFG_ANA_CTRL_1, wr_data);	
  //reg_write(ADDR_AON_CFG_ANA_CTRL_1, (LDO18_EN |EN_OSC32M |LDO1_VSET(3)| LDO1_EN | BG_EN));
  AON_CFG_ANA_CTRL_1_0x20_Default=wr_data;
	
}




//There is a bug in AON register reading
void Analog_Ctrl_2_Init(void)
{
	unsigned int wr_data;
	char *string;	

  //wr_data=reg_read(ADDR_AON_CFG_ANA_CTRL_2);	
	wr_data = ANA_READ_CTLREG(ADDR_AON_CFG_ANA_CTRL_2);
	wr_data =(*(volatile unsigned int*)(0x4001A024));
	wr_data=AON_CFG_ANA_CTRL_2_0x24_Default;////There is a bug in AON register reading	

	#if DEBUG_UATR0_PRINT_LOG		
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("ADDR_AON_CFG_ANA_CTRL_2 --- = 0x"));
	string=my_itoa(wr_data);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));			
  #endif		
//	wr_data |= (SW_FORCE_BG_EN |SW_FORCE_LDO18_EN | SW_FORCE_LDO1_EN | SW_FORCE_OSC32M_EN);
//  reg_write(ADDR_AON_CFG_ANA_CTRL_2, wr_data);
  AON_CFG_ANA_CTRL_2_0x24_Default=wr_data;	
	
}



//There is a bug in AON register reading
void LDO18_Ctrl(void)
{
	unsigned int wr_data;
	//char *string;	

	
	wr_data=reg_read(ADDR_AON_CFG_ANA_CTRL_2);
  wr_data = AON_CFG_ANA_CTRL_2_0x24_Default;////There is a bug in AON register reading
	
	//Force bit[1]
	wr_data |= SW_FORCE_LDO18_EN;
	
  reg_write(ADDR_AON_CFG_ANA_CTRL_2, wr_data);	
	AON_CFG_ANA_CTRL_2_0x24_Default=wr_data;		
	
	
	
	//wr_data = reg_read(ADDR_AON_CFG_ANA_CTRL_1);
	wr_data = ANA_READ_CTLREG(ADDR_AON_CFG_ANA_CTRL_1);//0x4001A000+0x20
	wr_data =(*(volatile unsigned int*)(0x4001A020));
	wr_data = AON_CFG_ANA_CTRL_1_0x20_Default;////There is a bug in AON register reading	
	
	//enable ldo
	//wr_data |= (LDO18_EN |EN_OSC32M |LDO1_VSET(3)| LDO1_EN | BG_EN);
	//wr_data |= LDO_FLASH_VSET(3);//VSET<1:0> 00-->3.3V;01-->3.2V;10-->3.4V;11-->3.5V
	//#define LDO_FLASH_VSET(n)        (((n)&0x3)<<3)  //2 bits,(bit3 and bit4)
	//wr_data  = wr_data | 0x00000018;//(bit3 and bit4)
	wr_data  = wr_data & 0xFFFFFFFE;//(bit0=0,LDO18_EN=0)
  reg_write(ADDR_AON_CFG_ANA_CTRL_1, wr_data);	
  //reg_write(ADDR_AON_CFG_ANA_CTRL_1, (LDO18_EN |EN_OSC32M |LDO1_VSET(3)| LDO1_EN | BG_EN));
  AON_CFG_ANA_CTRL_1_0x20_Default=wr_data;
	
}

//LDO1
void LDO33_OUT_Ctrl(void)
{
	unsigned int wr_data;
	//char *string;	

	
	wr_data=reg_read(ADDR_AON_CFG_ANA_CTRL_2);
  wr_data = AON_CFG_ANA_CTRL_2_0x24_Default;////There is a bug in AON register reading
	
	//Force bit[1]
	wr_data |= SW_FORCE_LDO18_EN;
	
  reg_write(ADDR_AON_CFG_ANA_CTRL_2, wr_data);	
	AON_CFG_ANA_CTRL_2_0x24_Default=wr_data;		
	
	
	
	//wr_data = reg_read(ADDR_AON_CFG_ANA_CTRL_1);
	wr_data = ANA_READ_CTLREG(ADDR_AON_CFG_ANA_CTRL_1);//0x4001A000+0x20
	wr_data =(*(volatile unsigned int*)(0x4001A020));
	wr_data = AON_CFG_ANA_CTRL_1_0x20_Default;////There is a bug in AON register reading	
	
	//enable ldo
	//wr_data |= (LDO18_EN |EN_OSC32M |LDO1_VSET(3)| LDO1_EN | BG_EN);
	//wr_data |= LDO_FLASH_VSET(3);//VSET<1:0> 00-->3.3V;01-->3.2V;10-->3.4V;11-->3.5V
	//#define LDO_FLASH_VSET(n)        (((n)&0x3)<<3)  //2 bits,(bit3 and bit4)
	//wr_data  = wr_data | 0x00000018;//(bit3 and bit4)
	wr_data  = wr_data & 0xFFFFFFFE;//(bit0=0,LDO18_EN=0)
  reg_write(ADDR_AON_CFG_ANA_CTRL_1, wr_data);	
  //reg_write(ADDR_AON_CFG_ANA_CTRL_1, (LDO18_EN |EN_OSC32M |LDO1_VSET(3)| LDO1_EN | BG_EN));
  AON_CFG_ANA_CTRL_1_0x20_Default=wr_data;	
}



//wr_data = 0x848E7885;//0x808E7885

//There is a bug in AON register reading
//#define PWRMOS_G(n)              (((n)&0x3)<<13)  //2 bits Power PMOS Gate
void PWR_MOS_Gate_CTRL(uint8 gate)
{
	unsigned int wr_data,wr_data_tmp;
	wr_data=reg_read(ADDR_AON_CFG_ANA_CTRL_1);
  wr_data = AON_CFG_ANA_CTRL_1_0x20_Default;////There is a bug in AON register reading

	#if DEBUG_UATR0_PRINT_LOG	
  char *string;		

	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("111-PWR_MOS_Gate_CTRL"));
	
  wr_data_tmp=wr_data>>16;
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("AON_CFG_ANA_CTRL_1_0x20_Default High 16bit --- = 0x"));
	string=my_itoa(wr_data_tmp);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));


  wr_data_tmp=wr_data & 0x0000ffff;
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("AON_CFG_ANA_CTRL_1_0x20_Default Low 16bit --- = 0x"));
	string=my_itoa(wr_data_tmp);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));
	
  #endif
	
	//bit13,bit14
	wr_data |= PWRMOS_G(gate);
	//3:GATE0 and GATE1 equal to "1" at the same time
	//2:GATE0 equal to "1" and GATE1 equal to "0"
	//1:GATE0 equal to "0" and GATE1 equal to "1"
	//0:GATE0 and GATE1 equal to "0" at the same time
	if(gate==3)//MOS-S0 connected to MOS -D0
	{
		wr_data |= 0x00006000;//bit13,bit14 =1
		UATR0_PRINT_LOG((unsigned char *)("(gate==3)-PWR_MOS_Gate_CTRL"));
	}
	else if(gate==0)//MOS-S0 disconnected to MOS -D0
	{
		wr_data &= 0xffff9fff;//bit13,bit14 =0
    UATR0_PRINT_LOG((unsigned char *)("(gate==0)-PWR_MOS_Gate_CTRL"));		
	}
	
  reg_write(ADDR_AON_CFG_ANA_CTRL_1, wr_data);	
	AON_CFG_ANA_CTRL_1_0x20_Default=wr_data;
	
	#if DEBUG_UATR0_PRINT_LOG	
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("222-PWR_MOS_Gate_CTRL"));
	
  wr_data_tmp=wr_data>>16;
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("AON_CFG_ANA_CTRL_1_0x20_Default High 16bit --- = 0x"));
	string=my_itoa(wr_data_tmp);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));


  wr_data_tmp=wr_data & 0x0000ffff;
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("AON_CFG_ANA_CTRL_1_0x20_Default Low 16bit --- = 0x"));
	string=my_itoa(wr_data_tmp);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));		
  #endif
	
}







/**********************************************************************************************
*	
* Always-On Pin Wakeup
*
* always on gpio(default input) as wakeup pin, for example:
* PAD_08		spi_slv_clk(gpio_0_4)
* PAD_09    spi_slv_di(gpio_0_5)
* PAD_11		spi_slv_csn(gpio_0_7)
*
* PAD_15    efuse_clk(gpio_1_3)
* PAD_16		efuse_di(gpio_1_4)
* PAD_18    efuse_sel(gpio_1_6)
*
***********************************************************************************************/

/*
//GPIO_WAKEUP
//register 0x10
#define GPIO_WAKEUP_EN_00                 (1<<0)						//PAD08
#define GPIO_WAKEUP_EN_01                 (1<<1)						//PAD09
#define GPIO_WAKEUP_EN_02                 (1<<2)						//PAD10
#define GPIO_WAKEUP_EN_03                 (1<<3)						//PAD11
#define GPIO_WAKEUP_EN_04                 (1<<4)						//PAD12
#define GPIO_WAKEUP_EN_05                 (1<<5)						//PAD13
#define GPIO_WAKEUP_EN_06                 (1<<6)						//PAD14
#define GPIO_WAKEUP_EN_07                 (1<<7)						//PAD15
#define GPIO_WAKEUP_EN_08                 (1<<8)						//PAD16
#define GPIO_WAKEUP_EN_09                 (1<<9)						//PAD17
#define GPIO_WAKEUP_EN_10                 (1<<10)						//PAD18
#define GPIO_WAKEUP_EN_11                 (1<<11)						//PAD19
*/



/*
`ifdef GECKO_MACRO_TC_PIN_WAKEUP_DS
initial begin
    wait (`AON_TOP.u_aon_ctrl.ena_deepsleep_lpclk==1'b1);
    force `AON_TOP.u_aon_status_sync.reg_aon_gpio_wakeup_ena_03 = 1'b1;
    wait (`AON_TOP.u_aon_ctrl.sys_is_in_sleep==1'b1);
    $display("[TC Message] [GECKO_MACRO_TC_PIN_WAKEUP_DS] Use PAD_11 (GPIO, default SPI CSN input) to wake-up DeepSleep.");
    #200us;
    force tbench.PAD_11 = 1'b0;
    #1us;
    force tbench.PAD_11 = 1'b1;
    #1us;
    release tbench.PAD_11;
    wait (`DIG_CORE.sys_hwrite_cortexm0==1'b1);
    #10us;
    $display("[TC Message] [GECKO_MACRO_TC_PIN_WAKEUP_DS] GPIO wake-up DeepSleep is test done.");
    $finish;
end
`endif
*/




void GPIO_WakeUp_Setting(void)
{
	unsigned int wr_data;
  wr_data=reg_read(ADDR_AON_CFG_GPIO_WAKEUP_CTRL);
  wr_data=AON_CFG_GPIO_WAKEUP_CTRL_0x10_Default;	
//	wr_data |= (GPIO_WAKEUP_EN_00 |GPIO_WAKEUP_EN_01 |GPIO_WAKEUP_EN_02| GPIO_WAKEUP_EN_03);
	wr_data |= (GPIO_WAKEUP_EN_01);
  reg_write(ADDR_AON_CFG_GPIO_WAKEUP_CTRL, wr_data);
  AON_CFG_GPIO_WAKEUP_CTRL_0x10_Default=wr_data;	
}

/*
assign reg_aon_ena_wakeup_int				=reg_0x004[0];

ADDR_REG_0x008:rdata_comb = {15'h0,h2l_wr_busy,7'h0,reg_aon_roi_por_status,7'h0,reg_aon_roi_wakeup_int};

assign reg_aon_gpio_wakeup_ena			=reg_0x010[11:0];

assign reg_aon_gpio_wakeup_eact			=reg_0x014[11:0];

assign reg_aon_gpio_wakeup_edge			=reg_0x018[11:0];

assign reg_aon_gpio_wakeup_lvl			=reg_0x01C[11:0];

assign reg_aon_sleep_cnt_val			  =reg_0x060[23:0];
*/

//kevin@20230725
void BSR1901_GPIO_WakeUp_From_DeepSleep(void)
{
	unsigned int wr_data;
	
	wr_data=reg_read(GECKO_AON_BASE_ADDR+0x00C);//test reg_0x00C read and write
	//wr_data = 0xC08;
	
	wr_data |= 0x1;
	h2l_wr_busy();
  reg_write(GECKO_AON_BASE_ADDR+0x00C, wr_data);//
	
		
	
	
  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x004);
	wr_data |= 0x1;//aon_ena_wakeup_int
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x004, wr_data);

  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x010);
	
//	wr_data |= GPIOA4_WAKEUP_EN_00;
//	wr_data |= GPIOA5_WAKEUP_EN_01;
//	wr_data |= GPIOA6_WAKEUP_EN_02;
//	wr_data |= GPIOA7_WAKEUP_EN_03;

//	wr_data |= GPIOB0_WAKEUP_EN_04;
//	wr_data |= GPIOB1_WAKEUP_EN_05;
//	wr_data |= GPIOB2_WAKEUP_EN_06;
//	wr_data |= GPIOB3_WAKEUP_EN_07;

//	wr_data |= GPIOB4_WAKEUP_EN_08;
//	wr_data |= GPIOB5_WAKEUP_EN_09;
	wr_data |= GPIOB6_WAKEUP_EN_10;
//	wr_data |= GPIOB7_WAKEUP_EN_11;		
	
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x010, wr_data);	


  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x018);
	//wr_data |= 0xFFF;
	wr_data |= 0x400;
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x018, wr_data);	
	
}




//h2l_wr_busy
void aon_reg_wr_busy(void)
{
	unsigned int wr_data;

	
  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x004);
	wr_data |= 0x1;//aon_ena_wakeup_int
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x004, wr_data);
	
	
	wr_data=reg_read(GECKO_AON_BASE_ADDR+0x00C);//test reg_0x00C read and write
	h2l_wr_busy();
	wr_data |= 0x3;
  reg_write(GECKO_AON_BASE_ADDR+0x00C, wr_data);//	


  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x010);
	wr_data |= 0xF;
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x010, wr_data);

	
  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x018);
	wr_data |= 0x400;
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x018, wr_data);
	
  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x01C);
	wr_data |= 0x0FC;
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x01C, wr_data);	

  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x060);
	wr_data |= 0x4F0;
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x060, wr_data);	
	
}





//h2l_wr_busy
void aon_reg_wr_no_busy_ctrl(void)
{
	unsigned int wr_data;

	
  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x004);
	wr_data |= 0x1;//aon_ena_wakeup_int
	//h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x004, wr_data);
	
	
	wr_data=reg_read(GECKO_AON_BASE_ADDR+0x00C);//test reg_0x00C read and write

	//wr_data |= 0x3;
	h2l_wr_busy();
  reg_write(GECKO_AON_BASE_ADDR+0x00C, wr_data);//	


  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x010);
	wr_data |= 0xF;
	//h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x010, wr_data);

	
  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x018);
	wr_data |= 0x400;
	//h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x018, wr_data);
	
  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x01C);
	wr_data |= 0x0FC;
	//h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x01C, wr_data);	

  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x060);
	wr_data |= 0x4F0;
	//h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x060, wr_data);	
	
}





/*
GEK1109 register 

assign reg_aon_pad_pullup		=reg_0x88[23:0]
assign reg_aon_pad_pulldn		=reg_0x8C[23:0]

*/

void GEK1109_PAD_pullup_pulldown_Config(void)
{
	  unsigned int wr_data;
		wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);//pull up
	  wr_data |=0xFFFFFF;
	  reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
	
		wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
	  wr_data &=0x000000;
	  reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
	
///////////////////////////////////////////////////////////////////
	
		wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);//pull down
	  wr_data &=0x000000;
	  reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);
	

		wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
	  wr_data |=0xFFFFFF;
	  reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);
	
}





/*
uint32 AON_CFG_ANA_CTRL_1_0x20_Default=0x808E7885;//
//ANA_CTRL_1
//register 0x20

#define LDO18_EN                 (1<<0)
#define LDO18_VSET(n)            (((n)&0x3)<<1)  //2 bits
#define LDO_FLASH_VSET(n)        (((n)&0x3)<<3)  //2 bits
#define LPBG_VTRIM(n)            (((n)&0x7)<<5)  //3 bits
#define RCO32K_FC(n)						 (((n)&0xF)<<8)  //4 bits

#define EN_OSC32M                (1<<12)

#define PWRMOS_G(n)              (((n)&0x3)<<13)  //2 bits Power PMOS Gate
#define LDO1_VSET(n)             (((n)&0x3)<<15)  //2 bits
#define LDO1_EN                  (1<<17)
#define BG_EN                    (1<<18)
#define RCO32M_EN_16M            (1<<19)
#define RCO32M_TC(n)						 (((n)&0xF)<<20)  //4 bits
#define RCO32M_FREQ(n)					 (((n)&0xFF)<<24) //8 bits
*/

//uint32 AON_CFG_ANA_CTRL_1_0x20_Default=0x808E7885;//
//0xA0,4.065MHz
//0x90,3.7736MHz(uart ok)



void Osc32M_Freq_Trimming(void)
{
	unsigned int wr_data;//wr_data_tmp;//hj_ahb_rd_data;
	volatile unsigned long adr;
	//char *string;	
	wr_data=reg_read(OFFSET_REG_AON_AON_FSM_CTRL);
	wr_data=reg_read(OFFSET_REG_AON_AON_STATUS);	
	wr_data=reg_read(OFFSET_REG_AON_AON_SLEEP_CNT);

	wr_data=reg_read(OFFSET_REG_AON_AON_SLEEP_CNT_RBK);
	wr_data=reg_read(OFFSET_REG_AON_GPIO_WAKEUP);	
	
	wr_data=reg_read(OFFSET_REG_AON_ANA_CTRL_1);
	wr_data=reg_read(OFFSET_REG_AON_ANA_CTRL_2);	
	
	wr_data=reg_read(OFFSET_REG_AON_RSV_ANA_CTRL);	
	wr_data=reg_read(OFFSET_REG_AON_RSV_DIG_CTRL_1);
	wr_data=reg_read(OFFSET_REG_AON_RSV_DIG_CTRL_2);		
	
//	adr=0x3F000;
//	status=pEnterCriticalSection(); 
//	hj_ahb_rd_data=QSPI_NorFlash_ReadWord(adr);
//	pExitCriticalSection(status);	
	
	wr_data=reg_read(ADDR_AON_CFG_ANA_CTRL_2);
  wr_data = AON_CFG_ANA_CTRL_2_0x24_Default;////There is a bug in AON register reading
	
	//Force bit[4] to use SW config OSC frequency
//	wr_data |= SW_FORCE_CALI32M_TUNE | SW_FORCE_LDO1_EN;//change LDO33_OUT and LDO33_FLASH_OUT to 3.5V//2022-08-19 ZJF
	wr_data |= SW_FORCE_CALI32M_TUNE;	
	
  reg_write(ADDR_AON_CFG_ANA_CTRL_2, wr_data);	
	AON_CFG_ANA_CTRL_2_0x24_Default=wr_data;	

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
	
	wr_data=reg_read(ADDR_AON_CFG_ANA_CTRL_1);
  wr_data = AON_CFG_ANA_CTRL_1_0x20_Default;////There is a bug in AON register reading
	
	//wr_data |= RCO32M_FREQ(0xff);//max
//1@1 board
  wr_data = 0x8A8E7885;//0x808E7885
  wr_data = 0x938E7885;//0x808E7885 chip-1
  wr_data = 0x8B8E7885;//0x808E7885 chip-2	

//#define LDO1_VSET(n)             (((n)&0x3)<<15)  //2 bits,2b'00,3.3V;2b'01,3.2V;2b'10,3.4V;2b'11,3.5V;
//#define LDO_FLASH_VSET(n)        (((n)&0x3)<<3)  //2 bits,//VSET<1:0> 00-->3.3V;01-->3.2V;10-->3.4V;11-->3.5V

//  wr_data = 0x8B8FF89D;////change LDO33_OUT and LDO33_FLASH_OUT to 3.5V	
//  wr_data = 0x8B8FF885;////change LDO33_OUT(LDO1) to 3.5V
  wr_data = 0x8B8E789D;////change LDO33_FLASH_OUT to 3.5V

/*
volatile unsigned int ahb_rd_data;// global variable
volatile unsigned long adr;
adr=0x3F000;
ahb_rd_data=QSPI_NorFlash_ReadWord(adr);


//写入高8位calibration值
wr_data= wr_data&0x00FFFFFF;
ahb_rd_data= ahb_rd_data&0xFF000000;
wr_data= wr_data| ahb_rd_data;

*/


//0xA0,4.065MHz
//0x90,3.7736MHz(uart ok)	
//	wr_data = 0xcc86789D;	//2024-09-14 @shanghai change bit19=0 24.2MH2@3.3V


  wr_data = 0x9D86789D;//20MHZ
  //wr_data = 0x6F86789D;//16MHZ

  //wr_data = 0x7B8E789D;//2024-09-14 @Shanghai  8.5MHZ
	
	//wr_data = 0x708E789D;//2024-09-14 @Shanghai  8MHZ
	reg_write(ADDR_AON_CFG_ANA_CTRL_1, wr_data);

	AON_CFG_ANA_CTRL_1_0x20_Default=wr_data;	
	wr_data=AON_CFG_ANA_CTRL_1_0x20_Default;
	
}


void GEK_Retention_Reg_Access(void)
{
	volatile unsigned int aon_data;
	reg_write(ADDR_AON_RETENTION_REG_0, 0x12345678);
	reg_write(ADDR_AON_RETENTION_REG_1, 0xA55ACAFE);
	reg_write(ADDR_AON_RETENTION_REG_2, 0xABCD5678);
	reg_write(ADDR_AON_RETENTION_REG_3, 0xA55AABCD);

	reg_write(ADDR_AON_RETENTION_REG_4, 0x12345678);
	reg_write(ADDR_AON_RETENTION_REG_5, 0xA55ACAFE);
	reg_write(ADDR_AON_RETENTION_REG_6, 0xABCD5678);
	reg_write(ADDR_AON_RETENTION_REG_7, 0xA55AABCD);	
	
	aon_data=reg_read(ADDR_AON_RETENTION_REG_0);
	aon_data=reg_read(ADDR_AON_RETENTION_REG_1);
	aon_data=reg_read(ADDR_AON_RETENTION_REG_2);
	aon_data=reg_read(ADDR_AON_RETENTION_REG_3);	
	
	aon_data=reg_read(ADDR_AON_RETENTION_REG_4);
	aon_data=reg_read(ADDR_AON_RETENTION_REG_5);
	aon_data=reg_read(ADDR_AON_RETENTION_REG_6);
	aon_data=reg_read(ADDR_AON_RETENTION_REG_7);		
	


}




void App_LowPowerMode(void)
{
	

}
 

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



//------------------------------------------------------------------------------


/*
"GEK1109 added a AON clock divded-by-2 MUX
0: 32kHz clock (default)
1: 16kHz clock"

*/
void gecko_rootnode_clk32k_cfg(void)
{
	//AON Register,Offset Addr 0x0
	unsigned int wr_data;
	//sel_aon_clk16k
	wr_data=reg_read(ADDR_AON_CFG_AON_FSM_CTRL);
	wr_data |= SEL_AON_CLK16K;
	reg_write(ADDR_AON_CFG_AON_FSM_CTRL, wr_data);	
	wr_data=reg_read(ADDR_AON_CFG_AON_FSM_CTRL);
	delay_1us(15);
	wr_data &= (~SEL_AON_CLK16K);	
	reg_write(ADDR_AON_CFG_AON_FSM_CTRL, wr_data);
}


/*
digital root node clock(Oscillator CLK input)

"GEK1109 added a AON register
Default 2'b01: 1/2 of OSC - 16MHz"

*/


void gecko_rootnode_osc32mclk_cfg(void)
{
	//AON Register,Offset Addr 0x70
	unsigned int wr_data;
	//aon2cmu_clk_div
	wr_data=reg_read(ADDR_AON_CFG_CTRL_EXT0);//aon2cmu_clk_div
	wr_data=aon2cmu_clk_div(1);
	reg_write(ADDR_AON_CFG_CTRL_EXT0, wr_data);	
	wr_data=reg_read(ADDR_AON_CFG_CTRL_EXT0);
	delay_1us(2);
	wr_data=aon2cmu_clk_div(2);
	reg_write(ADDR_AON_CFG_CTRL_EXT0, wr_data);	
	wr_data=reg_read(ADDR_AON_CFG_CTRL_EXT0);
	delay_1us(2);
	wr_data=aon2cmu_clk_div(3);
	reg_write(ADDR_AON_CFG_CTRL_EXT0, wr_data);	
	wr_data=reg_read(ADDR_AON_CFG_CTRL_EXT0);//aon2cmu_clk_div
	delay_1us(2);
	
	wr_data=aon2cmu_clk_div(1);
	reg_write(ADDR_AON_CFG_CTRL_EXT0, wr_data);		
}



extern void gecko_branchnode_pclkout_cfg(void);






void LDO33_AUX_Enable(void)
{
	unsigned int wr_data;
	wr_data = reg_read(0x40020000+0x28);
	wr_data |= 0x340;
	reg_write(0x40020000+0x28,wr_data);

}

void LDO33_AUX_Disable(void)
{
		unsigned int wr_data;
		wr_data = reg_read(0x40020000+0x28);
		//wr_data &= (~0x340);
		wr_data=0x0;
		reg_write(0x40020000+0x28,wr_data);	
}



void aon_wakeup_irq_cfg(void)
{
	hal_nvic_clear_pending_irq(AON_IRQn);
	hal_nvic_set_priority(AON_IRQn, 0);
	hal_nvic_enable_irq(AON_IRQn);	
}




typedef enum
{
    ADC2,//PAD_00
    ADC3,//PAD_01
    RSVD_1,
    RSVD_2,
    RSVD_3,
    RSVD_4,
    RSVD_5,
    RSVD_6,
    ADC0,//PAD_08
    ADC1,//PAD_09
    ADC6,//PAD_10
    CK32_OUT,//PAD_11
    ADC7,    //PAD_12
    OPA0_VIN,//PAD13,
    OPA0_VIP,//PAD14,
    OPA0_VO, //PAD15,
    OPA1_VIN,//PAD16,
    OPA1_VIP,//PAD17,
    OPA1_VO, //PAD18,
    ADC4,//PAD19,
    ADC8,//PAD20,
    ADC9 //PAD21

}ANA_Func_Type;

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
1109 Cost Down IOMUX Table
//////////////////////////////////////////////////////////////////////////////////////////////////////////
pad name	location	default function						function0	 		function1						function2			ANA func
PAD_00		3p3				swclk												uart0_out			gpio_a_0						pwm_ch4				ADC2
PAD_01		3p3				swdio												uart0_in			gpio_a_1						pwm_ch5				ADC3
PAD_02		3p3_f			spi_fls_scsn			　					                       	 	 	 	 	           	 	
PAD_03		3p3_f			spi_fls_sclk								                        					          		
PAD_04		3p3_f			spi_fls_mosi								                        					          		
PAD_05		3p3_f			spi_fls_miso								                        					          		
PAD_06		3p3_f			spi_fls_hold								gpio_a_7			uart2_in						pwm_ch0				---
PAD_07		3p3_f			spi_fls_wp									gpio_b_7			uart2_out						pwm_ch1				---
PAD_08		vbat			spi2ahb_clk									uart2_out			gpio_a_4						spi0_mst_clk	ADC0
PAD_09		vbat			spi2ahb_mosi	　			      uart2_in			gpio_a_5						spi0_mst_do		ADC1
PAD_10		vbat			spi2ahb_miso	　						i2c0_sda			gpio_a_6						spi0_mst_di		ADC6
PAD_11		vbat			spi2ahb_csn	　							i2c0_scl			gpio_a_7						pwm_ch6				CK32_OUT
PAD_12		vbat			pwm_cap_1										pwm_ch5				gpio_b_0						spi0_mst_csn	ADC7
PAD_13		vbat			pwm_ch0											uart1_in			gpio_b_1						led_io1				OPA0_VIN
PAD_14		vbat			pwm_ch1											uart1_out			gpio_b_2						led_io2				OPA0_VIP
PAD_15		vbat			clk_ref(OSC校正参考时钟)		pwm_ch4				gpio_b_3						led_io3				OPA0_VO
PAD_16		vbat			pwm_ch3										  spi1_mst_di		gpio_b_4						led_io4				OPA1_VIN
PAD_17		vbat			pwm_ch6											spi1_mst_do		gpio_b_5						led_io5				OPA1_VIP
PAD_18		vbat			pwm_ch7											spi1_mst_clk	gpio_b_6						led_io6				OPA1_VO
PAD_19		vbat			ftest											  spi1_mst_csn	gpio_b_7						pwm_cap_0			ADC4
PAD_20		1p8				uart0_dat(pogo_pin1)				i2c1_scl			gpio_a_2						pwm_ch2				ADC8
PAD_21		1p8				uart1_dat(pogo_pin2)				i2c1_sda			gpio_a_3						pwm_ch7				ADC9

*/


#define ADC2_FUNC					((0x1)<<0)   //PAD00
#define ADC3_FUNC					(1<<1)			 //PAD01

#define RSVD_1_FUNC				(1<<2)			 //PAD02
#define RSVD_2_FUNC				(1<<3)       //PAD03
#define RSVD_3_FUNC				(1<<4)			 //PAD04
#define RSVD_4_FUNC				(1<<5)			 //PAD05
#define RSVD_5_FUNC				(1<<6)			 //PAD06
#define RSVD_6_FUNC				(1<<7)			 //PAD07


#define ADC0_FUNC				    (1<<8)			 //PAD08
#define ADC1_FUNC				    (1<<9)			 //PAD09
#define ADC6_FUNC				    (1<<10)			 //PAD10
#define CK32_OUT_FUNC				(1<<11)			 //PAD11
#define ADC7_FUNC					  (1<<12)			 //PAD12

#define OPA0_VIN_FUNC				  (1<<13)			 //PAD13
#define OPA0_VIP_FUNC				  (1<<14)			 //PAD14
#define OPA0_VO_FUNC				  (1<<15)			 //PAD15
#define OPA1_VIN_FUNC					(1<<16)			 //PAD16
#define OPA1_VIP_FUNC				  (1<<17)			 //PAD17
#define OPA1_VO_FUNC				  (1<<18)			 //PAD18

#define ADC4_FUNC				  	(1<<19)			 //PAD19
#define ADC8_FUNC						(1<<20)			 //PAD20
#define ADC9_FUNC						(1<<21)      //PAD21



//Note: The AON register must be set after the chip is powered off and then powered back on to take effect


void ConfigPort_AnalogFunction(void)
{

	unsigned int wr_data;
	
	wr_data=reg_read(GECKO_AON_BASE_ADDR+0x080);
	
  wr_data = (ADC0_FUNC|ADC1_FUNC);//set ADC0 and ADC1 port to analog fucntion
	
	reg_write(GECKO_AON_BASE_ADDR+0x080, wr_data);	
	
}


void gek1109_adc_0_1_analog_port(void)
{
	
		unsigned int wr_data;
		reg_write(GECKO_AON_BASE_ADDR+0x080, 0x0);//reg_aon_ana0_sel
		reg_write(GECKO_AON_BASE_ADDR+0x084, 0x0);//reg_aon_ana0_sel

	  //set PAD as analog function
	  //CK32K_OUT  <------> reg_aon_ana0_sel[11]
	  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x080);
	  wr_data |= 0x300;//bit8 and bit9 (PAD8 and PAD9 as ADC function)
	  reg_write(GECKO_AON_BASE_ADDR+0x080, wr_data);		
}



void bsr1901_adc_8_9_analog_port(void)
{
	
		unsigned int wr_data;
		reg_write(GECKO_AON_BASE_ADDR+0x080, 0x0);//reg_aon_ana0_sel
		reg_write(GECKO_AON_BASE_ADDR+0x084, 0x0);//reg_aon_ana0_sel

	  //set PAD as analog function
	  //CK32K_OUT  <------> reg_aon_ana0_sel[11]
	  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x080);
	  wr_data |= 0x300000;//bit20 and bit21 (PAD20 and PAD21 as ADC function)
	  reg_write(GECKO_AON_BASE_ADDR+0x080, wr_data);		
}


