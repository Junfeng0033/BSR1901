/****************************************************************************
 *   $Id:: timer.c  2020-07-30 02:25:13
 *   Project:Gecko Timer example
 *
 *   Description:
  *  Author:
 *   	 JF Zhou
****************************************************************************/
#include "platform_config.h"
#include "bsr1901.h"
#include "timer.h"
#include "iomux.h"
#include "aon.h"

#include "uart.h"


extern void hal_nvic_disable_irq(IRQn_Type irq_number);
//extern void tc_qspi_to_xtx_sanity(void);
static void  Gecko_Timer_Disable_One(uint8 num);

void Gecko_Timer_Unmask(uint8 num);
static void Gecko_Timer_Enable_One(uint8 num);
static void Gecko_Timer_Disable_One(uint8 num);


void Gecko_Timer_Init(void)
{
		//configure periodic timer
    //100ms timer, auto reload when time expired
		uint8  num;
		volatile uint32 clear;
	
		Gecko_Timer_Disable_One(Gecko_Timer0_Int_Index);
		Gecko_Timer_Disable_One(Gecko_Timer1_Int_Index);	
	
		for(num = 0; num < SYS_HW_TIMER_NUMS; num++)
		{
			HWreg_Assign_Register(Gecko_Timer_LoadCount_REG(num), 0x00);
			HWreg_Assign_Register(Gecko_Timer_Control_REG(num), 0x00);		
		}		
		clear = HWreg_Get_Register(Gecko_All_Timer_EOI_REG);
}








void Gecko_Timer_Setting(uint8 TIM_IDX)
{

		switch (TIM_IDX)
		{				
		case TIMER_INDEX0:
			
				//HWreg_Assign_Register(Gecko_Timer_LoadCount_REG(Gecko_Timer0_Int_Index), SYS_TIMER_DELAY_MS);//#define SYS_TIMER_DELAY_MS	20000	
				//HWreg_Assign_Register(Gecko_Timer_LoadCount_REG(Gecko_Timer0_Int_Index), 800000);//2.5ms@8Mhz
				HWreg_Assign_Register(Gecko_Timer_LoadCount_REG(Gecko_Timer0_Int_Index), 100);//
				HWreg_Assign_Register(Gecko_Timer_Control_REG(Gecko_Timer0_Int_Index), GECKO_BIT(Gecko_Timer_Control_MODE_BIT));
				Gecko_Timer_Unmask(Gecko_Timer0_Int_Index);		
				Gecko_Timer_Enable_One(Gecko_Timer0_Int_Index);	
#if 1
				/******* configure the Timer NVIC ********************************/
		    hal_nvic_disable_irq(TIMER0_IRQn);
				hal_nvic_clear_pending_irq(TIMER0_IRQn);
				hal_nvic_set_priority(TIMER0_IRQn, 0);
				hal_nvic_enable_irq(TIMER0_IRQn);	
#endif		
				break;
		
		case TIMER_INDEX1:
			
				//HWreg_Assign_Register(Gecko_Timer_LoadCount_REG(Gecko_Timer0_Int_Index), SYS_TIMER_DELAY_MS);//#define SYS_TIMER_DELAY_MS	20000	
				HWreg_Assign_Register(Gecko_Timer_LoadCount_REG(Gecko_Timer1_Int_Index), 8000);//1ms@8Mhz
				HWreg_Assign_Register(Gecko_Timer_Control_REG(Gecko_Timer1_Int_Index), GECKO_BIT(Gecko_Timer_Control_MODE_BIT));
				Gecko_Timer_Unmask(Gecko_Timer1_Int_Index);
				Gecko_Timer_Enable_One(Gecko_Timer1_Int_Index);	
#if 0
				hal_nvic_disable_irq(TIMER1_IRQn);
				hal_nvic_clear_pending_irq(TIMER1_IRQn);
				hal_nvic_set_priority(TIMER1_IRQn, 0);
				hal_nvic_enable_irq(TIMER1_IRQn);		
#endif		
				break;
		default:
			  break;
	   }

		//SYSirq_Request_IRQ(GECKO_IRQ_TIMER_INDEX, Timer0_Interrupt_Handler);		
		//SYSirq_Enable_IRQ(GECKO_IRQ_TIMER_INDEX);

}


/****************************************************************************
*
*
*
****************************************************************************/

static void  Gecko_Timer_Enable_One(uint8 num)
{
	uint32  temp_val;
	temp_val = HWreg_Get_Register(Gecko_Timer_Control_REG(num));	
	temp_val |= GECKO_BIT(Gecko_Timer_Control_EN_BIT);	
	HWreg_Assign_Register(Gecko_Timer_Control_REG(num), temp_val);	
}


static void  Gecko_Timer_Disable_One(uint8 num)
{
	uint32  temp_val;	
	temp_val = HWreg_Get_Register(Gecko_Timer_Control_REG(num));	
	temp_val &= ~ GECKO_BIT(GECKO_REG_BIT00);	
	HWreg_Assign_Register(Gecko_Timer_Control_REG(num), temp_val);	
}

#if 0
void Gecko_Timer_Enable(void)
{
	/*we only use Timer0 */
	Gecko_Timer_Enable_One(Gecko_Timer0_Int_Index);	
}

void Gecko_Timer_Disable(void)
{
	/*we only use Timer0 */
	Gecko_Timer_Disable_One(Gecko_Timer0_Int_Index);	
}
#endif


#if 1
void Gecko_Timer_ClearInt(void)
{
	volatile uint32 temp_value;
	/* For now, we only use Timer0 */
	temp_value = HWreg_Get_Register(Gecko_Timer_EOI_REG(Gecko_Timer0_Int_Index));
}
#endif


void Gecko_Timer_Mask(uint8 num)
{
	uint32  temp_val;
	temp_val = HWreg_Get_Register(Gecko_Timer_Control_REG(num));	
	temp_val |= GECKO_BIT(GECKO_REG_BIT02);	
	HWreg_Assign_Register(Gecko_Timer_Control_REG(num), temp_val);	
}


void Gecko_Timer_Unmask(uint8 num)
{
	uint32  temp_val;	
	temp_val = HWreg_Get_Register(Gecko_Timer_Control_REG(num));	
	temp_val &= ~ GECKO_BIT(GECKO_REG_BIT02);	
	HWreg_Assign_Register(Gecko_Timer_Control_REG(num), temp_val);	
}


//extern void Timer0_Toggle_GPIO0(void);
//extern void Timer1_Toggle_GPIO7(void);
/****************************************************************************
 *
 *
 *
 *
****************************************************************************/




void Timer0_Interrupt_Handler(void)
{
	volatile uint32 temp_value;


	
	temp_value = HWreg_Get_Register(Gecko_Timer_EOI_REG(Gecko_Timer0_Int_Index));
	
}





int timer1_tmp_cnt1=0;

//timer1 interrupt service routine
void gecko_timer1_irq_handler(void)
{
	//2.5ms timer
	 volatile uint32 temp_value;
	 if(timer1_tmp_cnt1++>200)//5s
	 {
		 timer1_tmp_cnt1=0;
		 //tc_qspi_to_xtx_sanity();

	 }
	 temp_value = HWreg_Get_Register(Gecko_Timer_EOI_REG(Gecko_Timer1_Int_Index));

}



