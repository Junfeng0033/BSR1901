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
//#define GEK1109_MOS_GATE(n)      (((n)&0x7)<<12)  //GEK1109 3-bit MOS Gate [14:12]




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
//bit24 assign to MOS gate controll
#define MOS_EN				  (1<<24)      //RSVD<0>


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







//	BSR1901_MOS_Enable();
//	
//  delay_1us(8000);
//	
//	BSR1901_MOS_Disable();
	





