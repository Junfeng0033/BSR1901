#include "platform_config.h"
#include "bsr1901.h"

#include "aon.h"
#include "../norflash/xtxflash/dpi_common.h"

#include "uart.h"


//D2A_AON_RSVD_7						pin_wakeup选择			



uint32 AON_CFG_ANA_CTRL_1_0x20_Default=0x808E7885;
uint32 AON_CFG_ANA_CTRL_2_0x24_Default=0;







void Analog_Ctrl_1_Init(void)
{
	unsigned int wr_data;
	//char *string;	

	//wr_data = reg_read(ADDR_AON_CFG_ANA_CTRL_1);
	wr_data = ANA_READ_CTLREG(ADDR_AON_CFG_ANA_CTRL_1);//0x4001A000+0x20
	wr_data =(*(volatile unsigned int*)(0x4001A020));
	wr_data = AON_CFG_ANA_CTRL_1_0x20_Default;	
	
	//enable ldo
	//wr_data |= (LDO18_EN |EN_OSC32M |LDO1_VSET(3)| LDO1_EN | BG_EN);
	//wr_data |= LDO_FLASH_VSET(3);//VSET<1:0> 00-->3.3V;01-->3.2V;10-->3.4V;11-->3.5V
	//#define LDO_FLASH_VSET(n)        (((n)&0x3)<<3)  //2 bits,(bit3 and bit4)
	wr_data  = wr_data | 0x00000018;//(bit3 and bit4)
  reg_write(ADDR_AON_CFG_ANA_CTRL_1, wr_data);	
  //reg_write(ADDR_AON_CFG_ANA_CTRL_1, (LDO18_EN |EN_OSC32M |LDO1_VSET(3)| LDO1_EN | BG_EN));
  AON_CFG_ANA_CTRL_1_0x20_Default=wr_data;
	
}





void Analog_Ctrl_2_Init(void)
{
	unsigned int wr_data;
	//char *string;	

  //wr_data=reg_read(ADDR_AON_CFG_ANA_CTRL_2);	
	wr_data = ANA_READ_CTLREG(ADDR_AON_CFG_ANA_CTRL_2);
	wr_data =(*(volatile unsigned int*)(0x4001A024));
	wr_data=AON_CFG_ANA_CTRL_2_0x24_Default;	

	#if 0//DEBUG_UATR0_PRINT_LOG		
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




void LDO18_Ctrl(void)
{
	unsigned int wr_data;
	//char *string;	

	
	wr_data=reg_read(ADDR_AON_CFG_ANA_CTRL_2);
  wr_data = AON_CFG_ANA_CTRL_2_0x24_Default;
	
	//Force bit[1]
	wr_data |= SW_FORCE_LDO18_EN;
	
  reg_write(ADDR_AON_CFG_ANA_CTRL_2, wr_data);	
	AON_CFG_ANA_CTRL_2_0x24_Default=wr_data;		
	
	
	
	//wr_data = reg_read(ADDR_AON_CFG_ANA_CTRL_1);
	wr_data = ANA_READ_CTLREG(ADDR_AON_CFG_ANA_CTRL_1);//0x4001A000+0x20
	wr_data =(*(volatile unsigned int*)(0x4001A020));
	wr_data = AON_CFG_ANA_CTRL_1_0x20_Default;	
	
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
  wr_data = AON_CFG_ANA_CTRL_2_0x24_Default;
	
	//Force bit[1]
	wr_data |= SW_FORCE_LDO18_EN;
	
  reg_write(ADDR_AON_CFG_ANA_CTRL_2, wr_data);	
	AON_CFG_ANA_CTRL_2_0x24_Default=wr_data;		
	
	
	
	//wr_data = reg_read(ADDR_AON_CFG_ANA_CTRL_1);
	wr_data = ANA_READ_CTLREG(ADDR_AON_CFG_ANA_CTRL_1);//0x4001A000+0x20
	wr_data =(*(volatile unsigned int*)(0x4001A020));
	wr_data = AON_CFG_ANA_CTRL_1_0x20_Default;
	
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
	
//	wr_data |= (GPIO_WAKEUP_EN_00 |GPIO_WAKEUP_EN_01 |GPIO_WAKEUP_EN_02| GPIO_WAKEUP_EN_03);
	wr_data |= (GPIO_WAKEUP_EN_01);
  reg_write(ADDR_AON_CFG_GPIO_WAKEUP_CTRL, wr_data);

}





/*


reg  dff_wr_por_status;
wire wr_por_status = wr_sel_0x008 & byte1_sel & wdata[8];
always @(posedge regfile_clk or negedge regfile_rst_n) begin
    if (~regfile_rst_n)
            dff_wr_por_status <= 1'b0;
    else
            dff_wr_por_status <= wr_por_status;
end
assign reg_aon_clr_por_status = dff_wr_por_status;

reg  dff_wr_wakeup_int;
wire wr_wakeup_int = wr_sel_0x008 & byte0_sel & wdata[0];
always @(posedge regfile_clk or negedge regfile_rst_n) begin
    if (~regfile_rst_n)
            dff_wr_wakeup_int <= 1'b0;
    else
            dff_wr_wakeup_int <= wr_wakeup_int;
end
assign reg_aon_clr_wakeup_int = dff_wr_wakeup_int;


assign r_reg_0x008_sts = {13'h0,ana2dig_cmp2_result2dig_bit_d[1],ana2dig_cmp1_result2dig_bit_d[1], h2l_wr_busy, 7'h0, reg_aon_roi_por_status, 7'h0, reg_aon_roi_wakeup_int};


Register 0x008 is special:
When read, it represents reg_aon_roi_wakeup_int,which indicates the current status of the wakeup interrupt.
When written, it represents wr_wakeup_int, which enables the wakeup interrupt.





always @(posedge regfile_clk or negedge regfile_rst_n) begin
    if (~regfile_rst_n) begin
            reg_0x00C <= 12'h0;
    end
    else if (wr_sel_0x00C) begin
        if (byte0_sel)
            reg_0x00C[ 7: 0] <= wdata[ 7: 0];
        if (byte1_sel)
            reg_0x00C[11: 8] <= wdata[11: 8];
    end
end



reg  dff_wr_gpio11_wkup;
wire wr_gpio11_wkup = wr_sel_0x00C & byte1_sel & wdata[11];
always @(posedge regfile_clk or negedge regfile_rst_n) begin
    if (~regfile_rst_n)
            dff_wr_gpio11_wkup <= 1'b0;
    else
            dff_wr_gpio11_wkup <= wr_gpio11_wkup;
end
assign reg_aon_clr_gpio11_wkup = dff_wr_gpio11_wkup;




Register 0x00C has special functions:
On read: It indicates reg_aon_roi_gpio11_wkup, which shows which GPIO currently wakes up the CPU.
On write: It acts as reg_aon_clr_gpio11_wkup, which clears the GPIO wakeup status flag bit.





assign reg_aon_ena_wakeup_int				=reg_0x004[0];

ADDR_REG_0x008:rdata_comb = {15'h0,h2l_wr_busy,7'h0,reg_aon_roi_por_status,7'h0,reg_aon_roi_wakeup_int};

assign reg_aon_gpio_wakeup_ena			=reg_0x010[11:0];

assign reg_aon_gpio_wakeup_eact			=reg_0x014[11:0];

assign reg_aon_gpio_wakeup_edge			=reg_0x018[11:0];

assign reg_aon_gpio_wakeup_lvl			=reg_0x01C[11:0];

assign reg_aon_sleep_cnt_val			  =reg_0x060[23:0];




//  Low-power clock domain (slow-clock domain)
assign gpio_wkup_edge = reg_aon_gpio_wkup_edge  ?   ((~gpio_pad_in_sync[2]) &   gpio_pad_in_sync[1]) : // GPIO Input Posedge
                                                    (  gpio_pad_in_sync[2]  & (~gpio_pad_in_sync[1])); // GPIO Input Negedge

assign gpio_wkup_levl = reg_aon_gpio_wkup_lvl   ?   (  gpio_pad_in_sync[2]  &   gpio_pad_in_sync[1]) : // GPIO Input 2x AON cycle high-level
                                                    ((~gpio_pad_in_sync[2]) & (~gpio_pad_in_sync[1])); // GPIO Input 2x AON cycle low-level

assign gpio_wkup_active = reg_aon_gpio_wkup_eact ? gpio_wkup_edge : gpio_wkup_levl;



assign reg_aon_sel_aon_clk16k   = reg_0x000[10];
assign reg_aon_wait_ana_cntto   = reg_0x000[9:8];
assign reg_aon_ena_pin_wakeup   = reg_0x000[7];
//assign reg_aon_ena_rtc_wakeup   = reg_0x000[6];
assign reg_aon_ena_sleep_cnt_wkup= reg_0x000[5];
assign reg_aon_ena_sleep_cnt_run= reg_0x000[4];
assign reg_aon_ena_litesleep    = reg_0x000[3];
assign reg_aon_ena_deepsleep    = reg_0x000[2];
assign reg_aon_wait_pu_cntto    = reg_0x000[1:0];
assign reg_aon_ena_wakeup_int   = reg_0x004[0];
assign reg_aon_gpio_wkup_ena    = reg_0x010[11:0];
assign reg_aon_gpio_wkup_eact   = reg_0x014[11:0];
assign reg_aon_gpio_wkup_edge   = reg_0x018[11:0];
assign reg_aon_gpio_wkup_lvl    = reg_0x01C[11:0];
//assign reg_aon_sleep_cnt_val    = reg_0x060[11:0];
assign reg_aon_sleep_cnt_val    = reg_0x060[23:0];

assign aon_gpio_wkup_ena[11:0] = reg_aon_gpio_wkup_ena[11:0];



*/





#if 0

//sleep-wakeup source setting,there are 12 GPIO wake-up sources

void BSR1901_Config_GPIO_WakeUp_Source_From_DeepSleep(void)
{
	unsigned int wr_data;


	/*
	
	register 0x00C has special functions:
	READ: It indicates reg_aon_roi_gpio11_wkup, which shows which GPIO currently wakes up the CPU.
	WRITE: It acts as reg_aon_clr_gpio11_wkup, which clears the GPIO wakeup status flag bit.
	
	*/

	wr_data=reg_read(GECKO_AON_BASE_ADDR+0x00C);
	//wr_data = 0xC08;
	wr_data = 0xFFF;	
	//wr_data = 0x1;//just clear GPIO08 wakeup status,ZJF @ 20240131
	
	h2l_wr_busy();
  reg_write(GECKO_AON_BASE_ADDR+0x00C, wr_data);
	
		
	
	
  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x004);
	wr_data |= 0x1;//aon_ena_wakeup_int
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x004, wr_data);
	
	
	

  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x010);


//setting GPIO wakeup source(There are a total of 12 GPIO wake-up sources)

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



/*


edge active config

assign reg_aon_gpio_wakeup_eact			=reg_0x014[11:0];

assign reg_aon_gpio_wakeup_edge			=reg_0x018[11:0];

assign reg_aon_gpio_wakeup_lvl			=reg_0x01C[11:0];


assign gpio_wkup_active = reg_aon_gpio_wkup_eact ? gpio_wkup_edge : gpio_wkup_levl;


*/

  ////configure GPIO for rising edge triggering
  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x014);
	wr_data |= 0xFFF;
	
	//wr_data &= 0x000;
	//wr_data |= 0x400;
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x014, wr_data);



  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x018);
	wr_data |= 0xFFF;
	//wr_data |= 0x400;
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x018, wr_data);//GPIO input posedge(gpio wakeup edge config)	
	


  wr_data=reg_read(GECKO_AON_BASE_ADDR+0x01C);
	wr_data |= 0xFFF;
	//wr_data |= 0x400;
	h2l_wr_busy();
	reg_write(GECKO_AON_BASE_ADDR+0x01C, wr_data);	
	
}

#endif









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

void bsr1901_PAD_pullup_pulldown_Config(void)
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

BOR_BIT[1:0]			

2.2  2.4  2.6  2.8

assign reg_aon_bor_bit_cfg_vset =reg_0x020[14:13]

11：2.2V
00：2.8V

*/

typedef enum {
    BOR_2P2 = 3,    // 0b11
    BOR_2P4 = 2,    // 0b10
    BOR_2P6 = 1,		// 0b01
    BOR_2P8 = 0,    // 0b00
    BOR_QTY
} BOR_VSET_T;



#define BOR_VOL_SET(n)      				(((n)&0x3U)<<13U)  //2 bits

#define BOR_VOL_SET_MASK            (0x3U << 13U)           //BOR域完整掩码，用于清除旧值
#define BSR1901_BOR_CTRL_REG_ADDR   (0x40020000U + 0x20U)



void BSR1901_SetBOR_Voltage(BOR_VSET_T bor_cfg)
{

//	wr_data = 0x608e789D;//test bor function [BOR_BT=11,-------2.35V]
//	wr_data = 0x608e189D;//test bor function [BOR_BT=00,-------2.75V]


		unsigned int wr_data;
	
	  uint8_t hw_code = (uint8_t)bor_cfg;
	
	  hw_code &= 0x3U;
	
		wr_data = reg_read(BSR1901_BOR_CTRL_REG_ADDR);
    wr_data &= ~BOR_VOL_SET_MASK;          //清除旧的bit13---bit14
    wr_data |= BOR_VOL_SET(hw_code);       //写入新2bit配置
    reg_write(BSR1901_BOR_CTRL_REG_ADDR, wr_data);
	
}


uint8_t BSR1901_GetBOR_VoltageCode(void)
{
    uint32_t rd_data = reg_read(BSR1901_BOR_CTRL_REG_ADDR);
    return (uint8_t)((rd_data & BOR_VOL_SET_MASK) >> 13U);
}








typedef enum
{
    LPBG_VTRIM_CFG0 = 0,    // 0b000
    LPBG_VTRIM_CFG1 = 1,    // 0b001
    LPBG_VTRIM_CFG2 = 2,    // 0b010
    LPBG_VTRIM_CFG3 = 3,    // 0b011
    LPBG_VTRIM_CFG4 = 4,    // 0b100
    LPBG_VTRIM_CFG5 = 5,    // 0b101
    LPBG_VTRIM_CFG6 = 6,    // 0b110
    LPBG_VTRIM_CFG7 = 7     // 0b111
}LPBG_VTRIM_CFG_T;



#define BSR1901_LPBG_VTRIM(n)            (((n)&0x7U)<<5U)  //3 bits
#define BSR1901_LPBG_VTRIM_MASK      		 (0x7U << 5U)
#define BSR1901_LPBG_CTRL_REG_ADDR   		 (0x40020000U + 0x20U)


void BSR1901_CFG_LPBG_VTRIM(LPBG_VTRIM_CFG_T lpbg_vtrim_cfg)
{
		unsigned int wr_data;
	
	  uint8_t trim_val = (uint8_t)lpbg_vtrim_cfg;
	
		trim_val &= 0x07U;
	
		wr_data = reg_read(BSR1901_LPBG_CTRL_REG_ADDR);
    wr_data &= ~BSR1901_LPBG_VTRIM_MASK; //清除旧的bit5---bit7
    wr_data |= BSR1901_LPBG_VTRIM(trim_val);
    reg_write(BSR1901_LPBG_CTRL_REG_ADDR, wr_data);

}









//#define LDO_FLASH_VSET(n)        (((n)&0x3)<<3)  //2 bits,
//ERROR config //VSET<1:0> 00-->3.3V;01-->3.2V;10-->3.4V;11-->3.5V

typedef enum {
    LDO_FLASH_3P2V = 0,    // 0b00
    LDO_FLASH_3P3V = 1,    // 0b01
    LDO_FLASH_3P4V = 2,    // 0b10
    LDO_FLASH_3P5V = 3,    // 0b11
    LDO_FLASH_QTY
} LDO_NORFLASH_VSET_T;



#define LDO_NORFLASH_VSET(n)             (((n)&0x3U)<<3U)  //2 bits bit3---bit4, VSET<1:0>
#define LDO_NORFLASH_VSET_MASK         	 (0x3U << 3U)      //NorFlash LDO VSET域完整掩码，清除旧值
#define BSR1901_NORFLASH_LDO_REG_ADDR    (0x40020000U + 0x20U)


void BSR1901_SetLdo_NorFlashVoltage(LDO_NORFLASH_VSET_T vset_cfg)
{
    uint32_t wr_data;
    uint8_t hw_code;

    /* 非法参数直接返回，不操作寄存器 */
    if ((uint32_t)vset_cfg >= LDO_FLASH_QTY)
    {
        return;
    }

    hw_code = (uint8_t)vset_cfg;
    hw_code &= 0x3U;

    wr_data = reg_read(BSR1901_NORFLASH_LDO_REG_ADDR);
    wr_data &= ~LDO_NORFLASH_VSET_MASK;        //清除旧的bit3---bit4
    wr_data |= LDO_NORFLASH_VSET(hw_code);     //写入新2bit配置
    reg_write(BSR1901_NORFLASH_LDO_REG_ADDR, wr_data);
}



uint8_t BSR1901_GetLdo_NORFlashVoltageCode(void)
{
    uint32_t rd_data = reg_read(BSR1901_NORFLASH_LDO_REG_ADDR);
    return (uint8_t)((rd_data & LDO_NORFLASH_VSET_MASK) >> 3U);
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

#define BOR_VSET(n)              (((n)&0x3)<<13)  //2 bits:2.2  2.4  2.6  2.8

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
	
	
	wr_data = 0x608e7885;//default
	
	wr_data = 0xD08e7885;//1.2MHZ PCLK	
	
	wr_data = 0xD08e789D;//1.233MHZ PCLK	
	
	
	wr_data = 0x608e789D;//test bor function [BOR_BT=11,-------2.35V]
	wr_data = 0x608e189D;//test bor function [BOR_BT=00,-------2.75V]




//#define LDO_FLASH_VSET(n)        (((n)&0x3)<<3)  //2 bits

//assign reg_aon_ldo_flash_vset =reg_0x020[4:3]


	//wr_data = 0xD08e189D;

	wr_data = 0xD08e188D;//ldo flash vset=01 [3.2V],actual 3.6V
	
	wr_data = 0xD08e1885;//ldo flash vset=00 [3.3V],actual 3.48V


//test freq limit @20260830
	wr_data = 0xDF8e1885;//1.26MHZ@3.3V PCLK
	
	wr_data = 0xEF8e1885;//1.32MHZ@3.3V PCLK


	wr_data = 0xFF8e1885;//1.37MHZ@3.3V PCLK

//enale bit19
  wr_data = 0x5D861885;//1.44MHZ@3.3V PCLK
	

  wr_data = 0x60861885;//1.47MHZ@3.3V PCLK
	



	
	
//test pwm
//	wr_data = 0xCC8e7885;//1.2MHZ PCLK	
	
	
	reg_write(0x40020000+0x020, wr_data);		
	
	reg_write(ADDR_AON_CFG_ANA_CTRL_1, wr_data);

	AON_CFG_ANA_CTRL_1_0x20_Default=wr_data;	
	wr_data=AON_CFG_ANA_CTRL_1_0x20_Default;
	
	
#if 0
//==========================================================================		
	

//"A larger value corresponds to a higher voltage."

	BSR1901_CFG_LPBG_VTRIM(0);	
//	BSR1901_CFG_LPBG_VTRIM(1);
//	BSR1901_CFG_LPBG_VTRIM(2);	
//	BSR1901_CFG_LPBG_VTRIM(3);
//	BSR1901_CFG_LPBG_VTRIM(4);	
//	BSR1901_CFG_LPBG_VTRIM(5);
//	BSR1901_CFG_LPBG_VTRIM(6);
//	BSR1901_CFG_LPBG_VTRIM(7);	
	

//@VDD5V = 4V
//	BSR1901_SetLdo_NorFlashVoltage(LDO_FLASH_3P2V);	//3.44V
	BSR1901_SetLdo_NorFlashVoltage(LDO_FLASH_3P3V);	//3.55V

//	BSR1901_SetLdo_NorFlashVoltage(LDO_FLASH_3P4V);	//3.64V
	
	
//	BSR1901_SetLdo_NorFlashVoltage(LDO_FLASH_3P5V);		//3.74V


//==========================================================================		




//==========================================================================		
	
//@VDD5V = 4V

	BSR1901_CFG_LPBG_VTRIM(4);	

	BSR1901_SetLdo_NorFlashVoltage(LDO_FLASH_3P3V);	//3.32V



//==========================================================================	

//==========================================================================		
	
//@VDD5V = 4V

	BSR1901_CFG_LPBG_VTRIM(3);	

	BSR1901_SetLdo_NorFlashVoltage(LDO_FLASH_3P3V);	//3.36V



//==========================================================================	


//==========================================================================		
	
//@VDD5V = 4V

	BSR1901_CFG_LPBG_VTRIM(5);	

	BSR1901_SetLdo_NorFlashVoltage(LDO_FLASH_3P3V);	//3.52V



//==========================================================================	




//==========================================================================		
	
//@VDD5V = 4V

	BSR1901_CFG_LPBG_VTRIM(5);	

	BSR1901_SetLdo_NorFlashVoltage(LDO_FLASH_3P2V);	//3.44V



//==========================================================================	


//==========================================================================		
	
//@VDD5V = 4V

	BSR1901_CFG_LPBG_VTRIM(4);	

	BSR1901_SetLdo_NorFlashVoltage(LDO_FLASH_3P2V);	//3.24V



//==========================================================================	
#endif


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






//可以为用户提供1.8V /2.8V/3.0V/3.3V 四种电压。
/*
VSET<1:0>         Vout

00                1.8V
01                2.8V
10                3.0V
11                3.3V

//assign reg_aon_ldo_aux_vset = reg_0x28[9:8]
//assign reg_aon_ldo_aux_en = reg_0x28[6]
//=============================================	
	wr_data=reg_read(0x40020000+0x28);	
	wr_data |= 0x340;	
	reg_write(0x40020000+0x28, wr_data);
//=============================================	
*/



#define REG_LDO33_OUT_MASK              (0x3UL << 8) 

#define reg_ldo33_out_vset(n)           (((n) & 0x3) << 8) //2-bit [9:8]//default value n=0

#define reg_ldo33_aux_out_en            (1<<6)




void LDO33_LCD_Enable(void)
{
	unsigned int wr_data;
	wr_data = reg_read(0x40020000+0x28);
	
	wr_data &= ~REG_LDO33_OUT_MASK;
	wr_data|=reg_ldo33_out_vset(2);
	wr_data|=reg_ldo33_aux_out_en;	
	//wr_data |= 0x340;
	reg_write(0x40020000+0x28,wr_data);

	
	#if 0
	delay_1us(1000);
	//LDO33_AUX enable, power on LCD module
	wr_data |= 0x340;
	reg_write(0x40020000+0x28,wr_data);
	delay_1us(5000);
	#endif
}



void LDO33_LCD_Disable(void)
{
		unsigned int wr_data;
		wr_data = reg_read(0x40020000+0x28);
	
	  wr_data &= ~REG_LDO33_OUT_MASK;
	
		//wr_data &= (~0x340);
		wr_data=0x0;
		reg_write(0x40020000+0x28,wr_data);	
	
		#if 0
		//LDO33_AUX disable, power down LCD module
		wr_data = reg_read(0x40020000+0x28);
		//wr_data &= (~0x340);
		wr_data=0x0;
		reg_write(0x40020000+0x28,wr_data);			
		#endif	
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
PAD_15		vbat			clk_ref(OSC校正参考时钟)		  pwm_ch4				gpio_b_3						led_io3				OPA0_VO
PAD_16		vbat			pwm_ch3										  spi1_mst_di		gpio_b_4						led_io4				OPA1_VIN
PAD_17		vbat			pwm_ch6											spi1_mst_do		gpio_b_5						led_io5				OPA1_VIP
PAD_18		vbat			pwm_ch7											spi1_mst_clk	gpio_b_6						led_io6				OPA1_VO
PAD_19		vbat			ftest											  spi1_mst_csn	gpio_b_7						pwm_cap_0			ADC4
PAD_20		1p8				uart0_dat(pogo_pin1)				i2c1_scl			gpio_a_2						pwm_ch2				ADC8
PAD_21		1p8				uart1_dat(pogo_pin2)				i2c1_sda			gpio_a_3						pwm_ch7				ADC9

*/






//Note: The AON register must be set after the chip is powered off and then powered back on to take effect


void ConfigPort_AnalogFunction(uint32 func)
{

	unsigned int wr_data;
	
	wr_data=reg_read(GECKO_AON_BASE_ADDR+0x080);
	
  //wr_data |= (ADC0_FUNC|ADC1_FUNC);//set ADC0 and ADC1 port to analog fucntion
	
	wr_data |= func;
	
	
	reg_write(GECKO_AON_BASE_ADDR+0x080, wr_data);	
	
}


void bsr1901_adc_0_1_analog_port(void)
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




