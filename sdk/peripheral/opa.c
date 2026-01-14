
#include "platform_config.h"
#include "bsr1901.h"
#include "aon.h"


//=================================================
// VDD5V ---> VSS
// OPA Module Configure
// OP_Amp can be confiured as ...
// OP-Amp input signal and output voltage are within range of 0V to AVDDR-0.1V.
// x100 gain
//
//==================================================


/*

OPA Function test

D2A_AON_OP1_EN															OP1_EN			
D2A_AON_OP2_EN															OP2_EN			
D2A_AON_OPA_PGA1[1:0]												PGA1[1:0]			
D2A_AON_OPA_PGA2[1:0]												PGA2[1:0]			
D2A_AON_OPA1_PGA_CMP_SEL										DA_PGA1_CMP_SEL			
D2A_AON_OPA2_PGA_CMP_SEL										DA_PGA2_CMP_SEL			
D2A_AON_CMP1_VH_EN													CMP1_VH_EN			
D2A_AON_CMP2_VH_EN													CMP2_VH_EN			
D2A_CMP1_OUT_RESULT_TO_DIG_SEL							DA_CMP1_OUT_SEL			
D2A_CMP1_OUT_RESULT_TO_DIG_SEL							DA_CMP1_OUT_SEL			
A2D_CMP1_OUT_RESULT													CMP1_OUT			
A2D_CMP1_OUT_RESULT													CMP2_OUT			


assign reg_aon_op2_en        = reg_0x028[5];
assign reg_aon_op1_en        = reg_0x028[4];
assign reg_aon_opa_pga2      = reg_0x028[3:2];
assign reg_aon_opa_pga1      = reg_0x028[1:0];
////////////////////////////////////////////////////////////

1,enable opa1 and opa2
2,tune PGA gain 

////////////////////////////////////////////////////////////
*/


extern int reg_read(int addr);
extern void reg_write(int addr, int data);


void OP_Amp_Config(void)
{
	
		unsigned int wr_data;
	  //1,enable OPA
    wr_data=reg_read(GECKO_AON_BASE_ADDR+0x028);
	  wr_data |= 0x30;//set bit4 and bit5 "11"
	  reg_write(GECKO_AON_BASE_ADDR+0x028, wr_data);
	
	  //2,set PGA(or not set)

	
}




/*

assign reg_aon_cmp2_hys_res_en    = reg_0x028[11];//comparator2 internal hysteresis resistance enable
assign reg_aon_cmp1_hys_res_en    = reg_0x028[10];//comparator1 internal hysteresis resistance enable

assign reg_aon_ldo_aux_vset    = reg_0x028[9:8];

assign reg_aon_ldo_aux_en    = reg_0x028[6];
assign reg_aon_op2_en        = reg_0x028[5];
assign reg_aon_op1_en        = reg_0x028[4];
assign reg_aon_opa_pga2      = reg_0x028[3:2];
assign reg_aon_opa_pga1      = reg_0x028[1:0];




//ZJF @20240321
assign reg_aon_reserved_bit_ctrl   = reg_0x090[31:24];//reserved bits for analog metal fix control

assign reg_aon_cmp1_result2dig_sel  = reg_0x090[9]  ;//enabel analog result to digital 
assign reg_aon_cmp2_result2dig_sel  = reg_0x090[8]  ;//enabel analog result to digital 

assign reg_aon_optm_lowpower_mode_en  = reg_0x090[7]  ; 

                
assign reg_aon_ldo_fls_en       = reg_0x090[5]  ;  //output wire ////kevin@20230718,this bit used for "optimize low power consumption" 

assign reg_aon_ldo2_vset        = reg_0x090[4:3];  //output wire [1:0]            
assign reg_aon_ldo2_en          = reg_0x090[2]  ;  //output wire                  
assign reg_aon_opa1_pga_cmp_sel = reg_0x090[1]  ;  //output wire                  
assign reg_aon_opa2_pga_cmp_sel = reg_0x090[0]  ;  //output wire   

*/



void opa_cmp_mode (void)
{
	unsigned int wr_data;
	
	wr_data =reg_read(0x40020000+0x28);
	wr_data |= 0x30;//set bit4 and bit5 "11"
	reg_write(GECKO_AON_BASE_ADDR+0x028, wr_data);
	
	wr_data = reg_read (GECKO_AON_BASE_ADDR+0x090);
	wr_data |=0x3;//opa1 and opa2,set bit0 and bit1 "1"
	reg_write(GECKO_AON_BASE_ADDR+0x090,wr_data);
	
}






/****************************************************************************/

/****************************************************************************/
uint8_t OPx[50]={   0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                    0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
                    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
                    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,};
//Corresponding Gain:
//								1,2,4,8,16,32,64,
//                2,4,8,16,32,64,128
//                4,8,16,32,64,128,256
//                8,16,32,64,128,256,512
//                16,32,64,128,256,512,1024
//                32,64,128,256,512,1024,2048
//                64,128,256,512,1024,2048,4096
 
//LTC6912-2 PGA gain setting
void LTC6912_SetGain(uint16_t xGain)
{
	volatile uint8_t gtemp=0;
 
	switch(xGain)
	{
		case 1:
			gtemp=0;
			break;
		case 2:
			gtemp=1;
			break;
		case 4:
			gtemp=2;
			break;
		case 8:
			gtemp=3;
			break;
		case 16:
			gtemp=4;
			break;
		case 32:
			gtemp=5;
			break;
		case 64:
			gtemp=6;
			break;
		case 128:
			gtemp=13;
			break;
		case 256:
			gtemp=20;
			break;
		case 512:
			gtemp=27;
			break;
		case 1024:
			gtemp=34;
			break;
		case 2048:
			gtemp=41;
			break;
		case 4096:
			gtemp=48;
			break;
    default:
      break;
	}
	

}


