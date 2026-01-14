//=================================================
// BSR1901 PMOS Control
//
//RSVD<0> =1, MOS enable.  这一个控制位是在ECO的时候增加的。
//==================================================

#include "platform_config.h"

#include "aon.h"

extern int reg_read(int addr);
extern void reg_write(int addr, int data);



/*
aon_regfile.v
ADDR_REG_0X090 : rdata_comb=reg_0x090
*/

#define GEK1109_OPA2_CMP_SEL          (1<<0)
#define GEK1109_OPA1_CMP_SEL          (1<<1)
#define GEK1109_LDO2_EN          			(1<<2)
#define GEK1109_LDO2_VSET(n)      		(((n)&0x3)<<3)  //2 bits
#define GEK1109_LDO_FLS_EN            (1<<5)
#define GEK1109_LDO_FLS_BYPASS        (1<<6)


/*
aon_regfile.v
ADDR_REG_0X028 : rdata_comb={17'h0,reg_aon_pwrmos_g,2h'0,reg_aon_ldo_aux_vset,1'h0,reg_aon_ldo_aux_en,reg_aon_op2_en,reg_aon_op1_en,reg_aon_opa_pga2,reg_aon_opa_pga1}
*/
#define GEK1109_OPA1_PGA(n)      (((n)&0x3)<<0)  //2 bits
#define GEK1109_OPA2_PGA(n)      (((n)&0x3)<<2)  //2 bits
#define GEK1109_OPA1_EN          (1<<4)
#define GEK1109_OPA2_EN          (1<<5)
#define GEK1109_LDO_AUX_EN       (1<<6)
//bit7 reserved
#define GEK1109_LDO_AUX_VSET(n)  (((n)&0x3)<<8)  //2 bits
//bit10 reserved 
//bit11 reserved 
#define GEK1109_MOS_GATE(n)      (((n)&0x7)<<12)  //GEK1109 3-bit MOS Gate [14:12]




//#define PAD23_PULL				  (1<<23)      //PAD23
//#define PAD24_PULL				  (1<<24)      //PAD24

/*
//ZJF @20240321
//assign reg_aon_reserved_bit_ctrl   = reg_0x090[31:24];//reserved bits for analog metal fix control


//------------------------------------------------------------------
//RESERVED SIGNAL for Future Metal Fix
//ZJF @20240321
assign D2A_AON_RSVD_0       = reg_aon_reserved_bit_ctrl[0]      ;
assign D2A_AON_RSVD_1       = reg_aon_reserved_bit_ctrl[1]      ;
assign D2A_AON_RSVD_2       = reg_aon_reserved_bit_ctrl[2]      ;
assign D2A_AON_RSVD_3       = reg_aon_reserved_bit_ctrl[3]      ;

assign D2A_AON_RSVD_4       = reg_aon_reserved_bit_ctrl[4]      ;
assign D2A_AON_RSVD_5       = reg_aon_reserved_bit_ctrl[5]      ;
assign D2A_AON_RSVD_6       = reg_aon_reserved_bit_ctrl[6]      ;
assign D2A_AON_RSVD_7       = reg_aon_reserved_bit_ctrl[7]      ;

*/


//assign reg_aon_reserved_bit_ctrl   = reg_0x090[31:24];
#define MOS_EN				  (1<<24)      //bit24 assign to MOS gate controll


void BSR1901_MOS_Enable(void)
{
	unsigned int wr_data;
	wr_data = reg_read(0x40020000+0x90);
	wr_data |= MOS_EN;
	reg_write(0x40020000+0x90,wr_data);

}



void BSR1901_MOS_Disable(void)
{
	unsigned int wr_data;
	wr_data = reg_read(0x40020000+0x90);
	
	wr_data &= (~MOS_EN);
	reg_write(0x40020000+0x90,wr_data);

}



//There is a bug in AON register reading
//#define PWRMOS_G(n)              (((n)&0x3)<<13)  //2 bits Power PMOS Gate



//MOSFET switch Test
//PWR_MOS_Gate_CTRL(0);
//PWR_MOS_Gate_CTRL(3);



#if 0

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

#endif








volatile uint8 ctrl_flag=0;

void BSR1901_MOS_Gate_CTRL(void)
{
	  unsigned int wr_data,wr_data_0x090,wr_data_0x080,wr_data_0x08c,wr_data_0x088;
		wr_data=reg_read(GECKO_AON_BASE_ADDR+0x028);
	
	  wr_data_0x080=reg_read(GECKO_AON_BASE_ADDR+0x080);//ana_sel
	  wr_data_0x088=reg_read(GECKO_AON_BASE_ADDR+0x088);//pull up
	  wr_data_0x08c=reg_read(GECKO_AON_BASE_ADDR+0x08C);//pull down
	
		wr_data_0x090=reg_read(GECKO_AON_BASE_ADDR+0x090);
	
		if(ctrl_flag)
		{	
			ctrl_flag=0;
			wr_data=GEK1109_MOS_GATE(0);
			
			wr_data &=(~GEK1109_OPA1_EN);
			wr_data &=(~GEK1109_OPA2_EN);
			wr_data &=(~GEK1109_LDO_AUX_EN);
			
			wr_data_0x080 &=0x000000;//ana_sel
			
			wr_data_0x088 &=0x000000;
			wr_data_0x08c &=0x000000;
			
			wr_data_0x090 &=(~GEK1109_OPA2_CMP_SEL);
			wr_data_0x090 &=(~GEK1109_OPA1_CMP_SEL);
			wr_data_0x090 &=(~GEK1109_LDO2_EN);			
			wr_data_0x090 &=(~GEK1109_LDO_FLS_EN);		
		}
		else
		{
			ctrl_flag=1;
			wr_data=GEK1109_MOS_GATE(7);

			wr_data |=(GEK1109_OPA1_EN);
			wr_data |=(GEK1109_OPA2_EN);
			wr_data |=(GEK1109_LDO_AUX_EN);
			
			wr_data_0x080 |=0xFFFFFF;//ana_sel
			wr_data_0x088 |=0xFFFFFF;
			wr_data_0x08c |=0xFFFFFF;	

			wr_data_0x090 |=(GEK1109_OPA2_CMP_SEL);
			wr_data_0x090 |=(GEK1109_OPA1_CMP_SEL);
			wr_data_0x090 |=(GEK1109_LDO2_EN);			
			wr_data_0x090 |=(GEK1109_LDO_FLS_EN);			
			
		}
	  reg_write(GECKO_AON_BASE_ADDR+0x028, wr_data);
		
		reg_write(GECKO_AON_BASE_ADDR+0x080, wr_data_0x080);//ana_sel
	  reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data_0x088);
	  reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data_0x08c);
	  reg_write(GECKO_AON_BASE_ADDR+0x090, wr_data_0x090);			
}












