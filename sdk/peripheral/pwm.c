/****************************************************************************
 *   $Id:: pwm.c 2020-07-15 
 *   Project: Gecko pwm driver
 *
 *   Description: PWM Tone
 *   Author:
 *   	 JF Zhou
****************************************************************************/

#include "platform_config.h"
#include "bsr1901.h"
#include "pwm.h"

#include "uart.h"


// r_pwm_cnt      __________________________________________________________________________________________
// ---------------|x |x+1 |x+2 |x+3 |x+4 |.....................              .............|65535|x|x+1 |-----
//                -------------------------------------------------------------------------------------------
//             r_pwm0_initial                           r_pwm0_toggle                          r_pwm0_initial
// r_pwm_0_out                                               |----------------------------------|
// __________________________________________________________|                                  |___________________


/* Number of PWM Channels */
//#define NUM_CHANNELS						6

#define GPIO_Pin_5							6
#define PwmOut    						GPIO_Pin_5           // PB0 pin output PWM wave
#define BEEP_PIN      				GPIO_Pin_5


typedef enum
{
  	HW_PWM_CHAN_0=0,
    HW_PWM_CHAN_1,
    HW_PWM_CHAN_2,
    HW_PWM_CHAN_3,
    HW_PWM_CHAN_4,
    HW_PWM_CHAN_5,
#if 1//def GEK1109	
    HW_PWM_CHAN_6,
    HW_PWM_CHAN_7,	
#endif
    HW_PWM_CHAN_QTY,
	
} HW_PWM_CHAN_T;

 

//#define GECKO_PWM_BASE_NEW                0x40013000



//define AHB base address
//AHB Address
#define GECKO_PWM_BASE_NEW								0x40005000



typedef volatile struct
{
    REG32                          pwm_select_en;                   //0x00000000
    REG32                          r_pwm01_initial;                 //0x00000004
    REG32                          r_pwm23_initial;                 //0x00000008
    REG32                          r_pwm01_toggle;                  //0x0000000C
    REG32                          r_pwm23_toggle;                  //0x00000010
    REG32                          r_pwm01_phase;                   //0x00000014	
    REG32                          r_pwm23_phase;                   //0x00000018
	
    REG32                          r_pwm45_initial;                 //0x0000001C
    REG32                          r_pwm45_toggle;                  //0x00000020
    REG32                          r_pwm45_phase;                   //0x00000024
	
    REG32                          r_pwm03_Td_cycle;                //0x00000028
    REG32                          r_pwm47_Td_cycle;                //0x0000002C	
#if 1//def GEK1109		
    REG32                          r_pwm67_initial;                 //0x00000030
    REG32                          r_pwm67_toggle;                  //0x00000034
    REG32                          r_pwm67_phase;                   //0x00000038
	
    REG32                          pwm_reg0x3c;                     //0x0000003C
    REG32                          pwm_reg0x40;                     //0x00000040
    REG32                          pwm_reg0x44;                     //0x00000044
    REG32                          pwm_reg0x48;                     //0x00000048	
    REG32                          pwm_reg0x4c;                     //0x0000004C
    REG32                          pwm_reg0x50;                     //0x00000050		

#endif	
	
} Gecko_PWM_T;

#define Gecko_PWM                   ((Gecko_PWM_T*) GECKO_PWM_BASE_NEW)



void hw_pwm_invert(HW_PWM_CHAN_T channel);


static int gecko_pwm_start(void)
{
	Gecko_PWM->pwm_select_en =0xff;
	return 0;
}

static int gecko_pwm_stop(void)
{
	Gecko_PWM->pwm_select_en =0x00;
	return 0;
}



uint8 gecko_pwm_init(void)
{
	Gecko_PWM->r_pwm01_initial=0xfff0ff7f;
	Gecko_PWM->r_pwm23_initial=0xff00ff3f;	
	Gecko_PWM->r_pwm45_initial=0xff00ff3f;	
	//Gecko_PWM->r_pwm67_initial=0xff00ff3f;
	Gecko_PWM->r_pwm67_initial=0xfff0ff7f;
	
	Gecko_PWM->r_pwm01_toggle=0xfff7fff0;	
	Gecko_PWM->r_pwm23_toggle=0xff6fffaf;		
	Gecko_PWM->r_pwm45_toggle=0xff6fffaf;	
	//Gecko_PWM->r_pwm67_toggle=0xff6fffaf;
	Gecko_PWM->r_pwm67_toggle=0xfff7fff0;		

	Gecko_PWM->r_pwm01_phase=0x0010000d;	
	Gecko_PWM->r_pwm23_phase=0x0020000a;	
	Gecko_PWM->r_pwm45_phase=0x0010000d;	
	//Gecko_PWM->r_pwm67_phase=0x0020000a;
	Gecko_PWM->r_pwm67_phase=0x0010000d;
	
	//Gecko_PWM->pwm_select_en =0xff;
	
	gecko_pwm_stop();
	
	return 0;
}





#if 0

// =============================================================================
// SysSetupClkPwm
// -----------------------------------------------------------------------------
/// Setup the PWM clock
/// The PWM clock is got by dividing the system clock    
/// @param divider The divider used
// =============================================================================
void SetupSysClkPwm(uint8 divider)
{
		//update divider to regiser
    HW_SysClkUnit->CLK_DIV = PWM_CLK_DIV(divider);
}


#endif


#define APB_PWM_CLOCK                  	10000000

#ifdef BSR1901_SYS_8MHZ_MODE

//#define APB_PWM_CLOCK                  	8000000

#else
#define APB_PWM_CLOCK                  	16000000
//#define APB_PWM_CLOCK                  	4000000
#endif



#define PWM_MAX_VALUE                   0xFFFF



struct HAL_PWM_CFG_T {
    uint32_t freq; //expected frequency
    uint8_t ratio; //expected duty
	  uint8   Tdead_cycle_count;
};


//PWM_CLK  4Mhz, 4 000 000hz,4000000/137000=29(29.197)
void PWM0_137KHz_CRTL_PMOS()
{
	//hwp_pwm->PWL0_Config = 0x1234|PWM_PWL0_EN_H;
	Gecko_PWM->r_pwm01_initial=0xfff0ff7f;
	Gecko_PWM->r_pwm01_toggle=0xfff7fff0;	
	
}


void PWM1_137KHz_CRTL_NMOS()
{
	//pwl_1_out
	gecko_pwm_start();
	gecko_pwm_stop();
}


void PWM_Config_DeadZone_Td(HW_PWM_CHAN_T channel)
{
	
	
}


void Config_PWM(HW_PWM_CHAN_T channel,const struct HAL_PWM_CFG_T *cfg)
{
    uint32 load;
    uint32 toggle;
	  //uint32 freq;
	  volatile uint32 regval;
	  volatile uint32 tempval;
	  volatile uint32 tempval_d;
		//char *string;	
	
    uint8  ratio;//max 100,min 0
    ratio = cfg->ratio;
		
	  uint8 Tdead_Cycle;
	  Tdead_Cycle = cfg->Tdead_cycle_count;
	
    if (Tdead_Cycle > 10) 
		{
        return;
    }		
		
    if (ratio == 0) 
		{
			  //freq=cfg->freq;
			
        load = PWM_MAX_VALUE;
        toggle = 0;      
			  //return;
			
    }	
		
    else if (ratio == 100) 
		{
        load = PWM_MAX_VALUE;
        toggle = PWM_MAX_VALUE;
    } 
		else 
		{
        load = APB_PWM_CLOCK / cfg->freq;
        toggle = load * ratio / 100;
        if (toggle == 0) 
				{
            toggle = 1;
        }
        load = PWM_MAX_VALUE + 1 - load;
        toggle = PWM_MAX_VALUE - toggle;
    }	
		
		switch (channel)
		{
			case HW_PWM_CHAN_0:
					Gecko_PWM->r_pwm01_initial = SET_BITFIELD(Gecko_PWM->r_pwm01_initial, PWM_LOAD01_0, load);
			
#if 0//DEBUG_UATR0_PRINT_LOG
	tempval_d=Gecko_PWM->r_pwm01_initial;	
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("PWM_CHAN_0 Gecko_PWM->r_pwm01_initial --- = 0x"));
	string=my_itoa(tempval_d);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));		

#endif
		
					Gecko_PWM->r_pwm01_toggle = SET_BITFIELD(Gecko_PWM->r_pwm01_toggle, PWM_TOGGLE01_0, toggle);
					//PWM DeadZone config register
					regval=Gecko_PWM->r_pwm01_toggle;
			
				  tempval=Gecko_PWM->r_pwm01_toggle;
			    tempval=tempval & 0xffff0000;
			
					regval=regval>>0;
			    regval=regval+Tdead_Cycle;//toggle changed for Td (deadzone)
			
					Gecko_PWM->r_pwm01_toggle = tempval+PWM_TOGGLE01_0(regval);	//inclue toggle0 and toggle1			 			

#if 0//DEBUG_UATR0_PRINT_LOG
	tempval_d=Gecko_PWM->r_pwm01_toggle;
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("PWM_CHAN_0 Gecko_PWM->r_pwm01_toggle --- = 0x"));
	string=my_itoa(tempval_d);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
#endif	


					Gecko_PWM->r_pwm03_Td_cycle |= PWM0_Td_Cycle(Tdead_Cycle);
					
#if 0//DEBUG_UATR0_PRINT_LOG
	tempval=Gecko_PWM->r_pwm03_Td_cycle;
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("PWM_CHAN_0 Gecko_PWM->r_pwm03_Td_cycle --- = 0x"));
	string=my_itoa(tempval);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
#endif					

#if 0//DEBUG_UATR0_PRINT_LOG
	tempval=Gecko_PWM->r_pwm03_Td_cycle;
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("PWM_CHAN_0 Gecko_PWM->r_pwm03_Td_cycle --- = 0x"));
	string=my_itoa(tempval);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
#endif	
					break;

			case HW_PWM_CHAN_1:
					Gecko_PWM->r_pwm01_initial = SET_BITFIELD(Gecko_PWM->r_pwm01_initial, PWM_LOAD01_1, load);
					Gecko_PWM->r_pwm01_toggle = SET_BITFIELD(Gecko_PWM->r_pwm01_toggle, PWM_TOGGLE01_1, toggle);
			
					regval=Gecko_PWM->r_pwm01_toggle;
			
				  tempval=Gecko_PWM->r_pwm01_toggle;
			    tempval=tempval & 0x0000ffff;		
			
					regval=regval>>16;//pwm1 toggle value
			    regval=regval+Tdead_Cycle;//toggle changed for Td (deadzone)
					Gecko_PWM->r_pwm01_toggle = tempval+PWM_TOGGLE01_1(regval);			
			
			
					Gecko_PWM->r_pwm03_Td_cycle |= PWM1_Td_Cycle(Tdead_Cycle);
					break;

			case HW_PWM_CHAN_2:
					Gecko_PWM->r_pwm23_initial = SET_BITFIELD(Gecko_PWM->r_pwm23_initial, PWM_LOAD23_2, load);
					Gecko_PWM->r_pwm23_toggle = SET_BITFIELD(Gecko_PWM->r_pwm23_toggle, PWM_TOGGLE23_2, toggle);	
			
					regval=Gecko_PWM->r_pwm23_toggle;
			
				  tempval=Gecko_PWM->r_pwm23_toggle;
			    tempval=tempval & 0xffff0000; //reserve high 16bit
			
					regval=regval>>0;//pwm2 toggle value
			    regval=regval+Tdead_Cycle;//toggle changed for Td (deadzone)
					Gecko_PWM->r_pwm23_toggle = tempval+PWM_TOGGLE23_2(regval);	
			
					Gecko_PWM->r_pwm03_Td_cycle |= PWM2_Td_Cycle(Tdead_Cycle);	
			
					break;

			case HW_PWM_CHAN_3:
					Gecko_PWM->r_pwm23_initial = SET_BITFIELD(Gecko_PWM->r_pwm23_initial, PWM_LOAD23_3, load);
					Gecko_PWM->r_pwm23_toggle =  SET_BITFIELD(Gecko_PWM->r_pwm23_toggle, PWM_TOGGLE23_3, toggle);
			
					regval=Gecko_PWM->r_pwm23_toggle;
			
					tempval=Gecko_PWM->r_pwm23_toggle;
			    tempval=tempval & 0x0000ffff;	//reserve low 16bit			
			
          regval=regval>>16;//pwm3 toggle value
			    regval=regval+Tdead_Cycle;//toggle changed for Td (deadzone)
					Gecko_PWM->r_pwm23_toggle = tempval+PWM_TOGGLE23_3(regval);
			
					Gecko_PWM->r_pwm03_Td_cycle |= PWM3_Td_Cycle(Tdead_Cycle);			
					break;
			
			case HW_PWM_CHAN_4:
					Gecko_PWM->r_pwm45_initial = SET_BITFIELD(Gecko_PWM->r_pwm45_initial, PWM_LOAD45_4, load);
					Gecko_PWM->r_pwm45_toggle  = SET_BITFIELD(Gecko_PWM->r_pwm45_toggle, PWM_TOGGLE45_4, toggle);	
			
					regval=Gecko_PWM->r_pwm45_toggle;
				  tempval=Gecko_PWM->r_pwm45_toggle;
			    tempval=tempval & 0xffff0000; //reserve high 16bit			
			
				  regval=regval>>0;
			    regval=regval+Tdead_Cycle;//toggle changed for Td (deadzone)
					Gecko_PWM->r_pwm45_toggle = tempval+PWM_TOGGLE45_4(regval);			
			
					Gecko_PWM->r_pwm47_Td_cycle |= PWM4_Td_Cycle(Tdead_Cycle);	
					break;

			case HW_PWM_CHAN_5:
					Gecko_PWM->r_pwm45_initial = SET_BITFIELD(Gecko_PWM->r_pwm45_initial, PWM_LOAD45_5, load);
					Gecko_PWM->r_pwm45_toggle  = SET_BITFIELD(Gecko_PWM->r_pwm45_toggle, PWM_TOGGLE45_5, toggle);
			
					regval=Gecko_PWM->r_pwm45_toggle;
			
					tempval=Gecko_PWM->r_pwm45_toggle;
			    tempval=tempval & 0x0000ffff;	//reserve low 16bit		
			
          regval=regval>>16;//pwm5 toggle value
			    regval=regval+Tdead_Cycle;//toggle changed for Td (deadzone)
					Gecko_PWM->r_pwm45_toggle = tempval+PWM_TOGGLE45_5(regval);
			
					Gecko_PWM->r_pwm47_Td_cycle |= PWM5_Td_Cycle(Tdead_Cycle);				
					break;
#if 1			
			case HW_PWM_CHAN_6:
					Gecko_PWM->r_pwm67_initial = SET_BITFIELD(Gecko_PWM->r_pwm67_initial, PWM_LOAD67_6, load);
					Gecko_PWM->r_pwm67_toggle  = SET_BITFIELD(Gecko_PWM->r_pwm67_toggle, PWM_TOGGLE67_6, toggle);	
			
					regval=Gecko_PWM->r_pwm67_toggle;
				  tempval=Gecko_PWM->r_pwm67_toggle;
			    tempval=tempval & 0xffff0000; //reserve high 16bit			
			
				  regval=regval>>0;
			    regval=regval+Tdead_Cycle;//toggle changed for Td (deadzone)
					Gecko_PWM->r_pwm67_toggle = tempval+PWM_TOGGLE67_6(regval);			
			
					Gecko_PWM->r_pwm47_Td_cycle |= PWM6_Td_Cycle(Tdead_Cycle);	
					break;
			
			
			case HW_PWM_CHAN_7:
					Gecko_PWM->r_pwm67_initial = SET_BITFIELD(Gecko_PWM->r_pwm67_initial, PWM_LOAD67_7, load);
					Gecko_PWM->r_pwm67_toggle  = SET_BITFIELD(Gecko_PWM->r_pwm67_toggle, PWM_TOGGLE67_7, toggle);
			
					regval=Gecko_PWM->r_pwm67_toggle;
			
					tempval=Gecko_PWM->r_pwm67_toggle;
			    tempval=tempval & 0x0000ffff;	//reserve low 16bit		
			
          regval=regval>>16;//pwm5 toggle value
			    regval=regval+Tdead_Cycle;//toggle changed for Td (deadzone)
					Gecko_PWM->r_pwm67_toggle = tempval+PWM_TOGGLE67_7(regval);
			
					Gecko_PWM->r_pwm47_Td_cycle |= PWM7_Td_Cycle(Tdead_Cycle);				
					break;			
#endif					
			default:
					break;
		}	
		 
}




void hw_pwm_enable(HW_PWM_CHAN_T channel)
{
	volatile uint32 regval=Gecko_PWM->pwm_select_en;
	switch (channel)
	{
		case HW_PWM_CHAN_0:
				regval|= (1<<0); //bit0
				Gecko_PWM->pwm_select_en=regval;
				break;

		case HW_PWM_CHAN_1:
				regval|= (1<<1); //bit1
				Gecko_PWM->pwm_select_en=regval;
				break;

		case HW_PWM_CHAN_2:
				regval|= (1<<2); //bit2
				Gecko_PWM->pwm_select_en=regval;
				break;

		case HW_PWM_CHAN_3:
				regval|= (1<<3); //bit3
				Gecko_PWM->pwm_select_en=regval;
				break;
		
		
		case HW_PWM_CHAN_4:
				regval|= (1<<4); //bit4
				Gecko_PWM->pwm_select_en=regval;
				break;

		case HW_PWM_CHAN_5:
				regval|= (1<<5); //bit5
				Gecko_PWM->pwm_select_en=regval;
				break;
		
		case HW_PWM_CHAN_6:
				regval|= (1<<6); //bit6
				Gecko_PWM->pwm_select_en=regval;
				break;

		case HW_PWM_CHAN_7:
				regval|= (1<<7); //bit7
				Gecko_PWM->pwm_select_en=regval;
				break;		
		
		default:
		    break;
	}
	
	
	
}



// =============================================================================
// hw_pwm_invert
// -----------------------------------------------------------------------------
// This function not work correctly
// =============================================================================
void hw_pwm_invert(HW_PWM_CHAN_T channel)
{
	volatile uint32 regval=Gecko_PWM->pwm_select_en;
	switch (channel)
	{
		case HW_PWM_CHAN_0:
				regval|= (1<<8); //bit8
				Gecko_PWM->pwm_select_en=regval;
				break;

		case HW_PWM_CHAN_1:
				regval|= (1<<9); //bit9
				Gecko_PWM->pwm_select_en=regval;
				break;

		case HW_PWM_CHAN_2:
				regval|= (1<<10); //bit10
				Gecko_PWM->pwm_select_en=regval;
				break;

		case HW_PWM_CHAN_3:
				regval|= (1<<11); //bit11
				Gecko_PWM->pwm_select_en=regval;
				break;
		
		
		case HW_PWM_CHAN_4:
				regval|= (1<<12); //bit12
				Gecko_PWM->pwm_select_en=regval;
				break;

		case HW_PWM_CHAN_5:
				regval|= (1<<13); //bit13
				Gecko_PWM->pwm_select_en=regval;
				break;
		
		case HW_PWM_CHAN_6:
				regval|= (1<<14); //bit14
				Gecko_PWM->pwm_select_en=regval;
				break;

		case HW_PWM_CHAN_7:
				regval|= (1<<15); //bit15
				Gecko_PWM->pwm_select_en=regval;
				break;		
		default:
		    break;
	}
	
	
	
}






void SixCh_PWM5_Config(void)
{
	
	struct HAL_PWM_CFG_T SixChCfg;
	volatile uint32 pwmregval;


	//unsigned int wr_data;
	//char *string;	

#if 0	
	SixChCfg.freq= 200000;//
	SixChCfg.ratio= 50;//
	SixChCfg.Tdead_cycle_count=0;
	
	Config_PWM(HW_PWM_CHAN_0,&SixChCfg);
	
	SixChCfg.freq= 200000;//
	SixChCfg.ratio= 50;//
	SixChCfg.Tdead_cycle_count=1;//0;Tdead cycle
	Config_PWM(HW_PWM_CHAN_1,&SixChCfg);		
  hw_pwm_invert(HW_PWM_CHAN_1);



	SixChCfg.freq= 200000;//
	SixChCfg.ratio= 50;//
	SixChCfg.Tdead_cycle_count=0;
	
	Config_PWM(HW_PWM_CHAN_6,&SixChCfg);
	
	
/*************************************************************************/	
	//Pin23
	SixChCfg.freq= 150000;//
	SixChCfg.ratio= 20;//
	SixChCfg.Tdead_cycle_count=0;
	Config_PWM(HW_PWM_CHAN_7,&SixChCfg);	
  hw_pwm_invert(HW_PWM_CHAN_7);	
	
	
	
	//Pin24
	SixChCfg.freq= 150000;
	SixChCfg.ratio= 80;	
	SixChCfg.Tdead_cycle_count=0;	//
	Config_PWM(HW_PWM_CHAN_2,&SixChCfg);
	
/*************************************************************************/		
	
	
	
	
	SixChCfg.freq= 150000;//300000;
	SixChCfg.ratio= 80;//50;	
	SixChCfg.Tdead_cycle_count=0;//5;	// Tdead cycle, it maybe conflict with ratio
	Config_PWM(HW_PWM_CHAN_3,&SixChCfg);
	//hw_pwm_invert(HW_PWM_CHAN_3);	
	
#endif
	
	SixChCfg.freq= 100;//100HZ
	SixChCfg.ratio= 50;		
	SixChCfg.Tdead_cycle_count=0;

	//Config_PWM(HW_PWM_CHAN_4,&SixChCfg);	
	Config_PWM(HW_PWM_CHAN_5,&SixChCfg);	
	hw_pwm_invert(HW_PWM_CHAN_5);		

	pwmregval=Gecko_PWM->pwm_select_en;
	pwmregval=pwmregval|0x0020;//enable bit5(PWM5)
	Gecko_PWM->pwm_select_en=pwmregval;
	//gecko_pwm_start();








#if 0//DEBUG_UATR0_PRINT_LOG
	wr_data=Gecko_PWM->pwm_select_en;	
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("Gecko_PWM->pwm_select_en --- = 0x"));
	string=my_itoa(wr_data);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));		


	wr_data=Gecko_PWM->r_pwm03_Td_cycle;
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("Gecko_PWM->r_pwm03_Td_cycle --- = 0x"));
	string=my_itoa(wr_data);
	UATR0_PRINT_LOG((unsigned char *)(string));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
#endif

}




#if 0
uint32 PWM_Capture(void)
{
	volatile uint32 pwmregval=0,timeout=0;
  volatile uint16 frequency,duty;
  //Initial pwm caputre
  pwmregval=0;
  pwmregval=Gecko_PWM->pwm_reg0x4c;		
	pwmregval|=FREQ_AVE_EN;//bit24 [#define FREQ_AVE_EN  ((uint32)1<<24)//bit24]
	Gecko_PWM->pwm_reg0x4c=pwmregval;	
	//Gecko_PWM->pwm_reg0x4c |= FREQ_AVE_NUM(0x10);	
	
	//check capture done status
	timeout=0;
	while (!(Gecko_PWM->pwm_reg0x40 & FREQ_AVE_DONE))//status 1,done; otherwise it's not done
	{
		if(timeout++>500)
		return 0xff;
	}	
	
	pwmregval=Gecko_PWM->pwm_reg0x50;//[31:0] frequency
	
	frequency	=	pwmregval;
	
	pwmregval=Gecko_PWM->pwm_reg0x40;//[17:2] ducy_cycle
	
	duty			=	pwmregval>>2;	
	duty      = duty & 0xffff;//reserve low 16bit	
	
	return frequency;
}
#endif



void PWM2_PWM7_Enable(void)
{
	volatile uint32 pwmregval=Gecko_PWM->pwm_select_en;


	pwmregval=Gecko_PWM->pwm_select_en;
	pwmregval=pwmregval|0x0084;//PWM2 and PWM7
	Gecko_PWM->pwm_select_en=pwmregval;	
	
	
	Gecko_PWM->pwm_select_en=pwmregval;
}




void CST6118_Motor_PWM_Control(int freq,int duty2,int duty7)
{
	struct HAL_PWM_CFG_T SixChCfg;
	//volatile uint32 pwmregval;

/*************************************************************************/	
//	
//	//Pin24
//	SixChCfg.freq= 150000;
//	SixChCfg.ratio= 80;	
//	SixChCfg.Tdead_cycle_count=0;	//
//	Config_PWM(HW_PWM_CHAN_2,&SixChCfg);

	//Pin23
//	SixChCfg.freq= 150000;//
//	SixChCfg.ratio= 20;//
//	SixChCfg.Tdead_cycle_count=0;
//	Config_PWM(HW_PWM_CHAN_7,&SixChCfg);	
//  hw_pwm_invert(HW_PWM_CHAN_7);	
//	
//	

	
/*************************************************************************/		

	
	SixChCfg.freq= freq;//150000;//freq
	SixChCfg.ratio= duty2;//duty2
	SixChCfg.Tdead_cycle_count=0;
	Config_PWM(HW_PWM_CHAN_2,&SixChCfg);	
	
	SixChCfg.freq= freq;//150000;//freq
	SixChCfg.ratio= duty7;//duty7
	SixChCfg.Tdead_cycle_count=0;
	Config_PWM(HW_PWM_CHAN_7,&SixChCfg);	
	hw_pwm_invert(HW_PWM_CHAN_7);		
}




void CST6118_Motor_PWM2_Control_Optimized(int freq,int duty)
{
	
	uint8 Tdead_Cycle=0;
  uint32 load;
  uint32 toggle;	
	volatile uint32 regval;
	volatile uint32 tempval;	

  uint8  ratio;//max 100,min 0
  ratio = duty;	
	
	  if (ratio == 0) 
		{
			  //freq=cfg->freq;
			
        load = PWM_MAX_VALUE;
        toggle = 0;      
			  //return;
			
    }	
		
    else if (ratio == 100) 
		{
        load = PWM_MAX_VALUE;
        toggle = PWM_MAX_VALUE;
    } 
		else 
			
		
		
		{
/******************************************************			
        load = APB_PWM_CLOCK / freq;
        toggle = load * ratio / 100;
*******************************************************/
        load = APB_PWM_CLOCK / freq;//10_000_000/150_000=66
        toggle = load * ratio / 100;			


        if (toggle == 0) 
				{
            toggle = 1;
        }
        load = PWM_MAX_VALUE + 1 - load;
        toggle = PWM_MAX_VALUE - toggle;
    }	

			//case HW_PWM_CHAN_2:
					Gecko_PWM->r_pwm23_initial = SET_BITFIELD(Gecko_PWM->r_pwm23_initial, PWM_LOAD23_2, load);
					Gecko_PWM->r_pwm23_toggle = SET_BITFIELD(Gecko_PWM->r_pwm23_toggle, PWM_TOGGLE23_2, toggle);	
			
					regval=Gecko_PWM->r_pwm23_toggle;
			
				  tempval=Gecko_PWM->r_pwm23_toggle;
			    tempval=tempval & 0xffff0000; //reserve high 16bit
			
					regval=regval>>0;//pwm2 toggle value
			    regval=regval+Tdead_Cycle;//toggle changed for Td (deadzone)
					Gecko_PWM->r_pwm23_toggle = tempval+PWM_TOGGLE23_2(regval);	
			
					Gecko_PWM->r_pwm03_Td_cycle |= PWM2_Td_Cycle(Tdead_Cycle);	
			
					//break;

}






void CST6118_Motor_PWM7_Control_Optimized(int freq,int duty)
{
	
	uint8 Tdead_Cycle=0;
  uint32 load;
  uint32 toggle;	
	volatile uint32 regval;
	volatile uint32 tempval;	

  uint8  ratio;//max 100,min 0
  ratio = duty;	

	
	  if (ratio == 0) 
		{
			  //freq=cfg->freq;
			
        load = PWM_MAX_VALUE;
        toggle = 0;      
			  //return;
			
    }	
		
    else if (ratio == 100) 
		{
        load = PWM_MAX_VALUE;
        toggle = PWM_MAX_VALUE;
    } 
		else 
		
		
		{
			
/******************************************************			
        load = APB_PWM_CLOCK / freq;
        toggle = load * ratio / 100;
*******************************************************/
        load = 0x43;//APB_PWM_CLOCK / freq;//10_000_000/150_000=66
        toggle = load * ratio / 100;			


			
        if (toggle == 0) 
				{
            toggle = 1;
        }
        load = PWM_MAX_VALUE + 1 - load;
        toggle = PWM_MAX_VALUE - toggle;
    }	

			//case HW_PWM_CHAN_7:
					Gecko_PWM->r_pwm67_initial = SET_BITFIELD(Gecko_PWM->r_pwm67_initial, PWM_LOAD67_7, load);
					Gecko_PWM->r_pwm67_toggle  = SET_BITFIELD(Gecko_PWM->r_pwm67_toggle, PWM_TOGGLE67_7, toggle);
			
					regval=Gecko_PWM->r_pwm67_toggle;
			
					tempval=Gecko_PWM->r_pwm67_toggle;
			    tempval=tempval & 0x0000ffff;	//reserve low 16bit		
			
          regval=regval>>16;
			    regval=regval+Tdead_Cycle;//toggle changed for Td (deadzone)
					Gecko_PWM->r_pwm67_toggle = tempval+PWM_TOGGLE67_7(regval);
			
					Gecko_PWM->r_pwm47_Td_cycle |= PWM7_Td_Cycle(Tdead_Cycle);				
					//break;

}

