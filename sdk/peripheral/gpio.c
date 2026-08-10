/****************************************************************************
 *   $Id:: gpio.c  2020-07-30 02:25:13
 *   Project:Gecko GPIO example
 *
 *   Description:
 *     This file contains GPIO code example which include GPIO 
 *     initialization, GPIO interrupt handler, and related APIs for 
 *     GPIO access.
 *
 ****************************************************************************
*		 PL061 peripheral interface
****************************************************************************/

#include "platform_config.h"
#include "bsr1901.h"			/* Gecko Peripheral Registers */
#include "gpio.h"
#include "uart.h"

#include "iomux.h"
#include "aon.h"



uint8 GPIODIR_0_Shadow=0;
uint8 GPIOIS_0_Shadow=0;
uint8 GPIOIBE_0_Shadow=0;
uint8 GPIOIEV_0_Shadow=0;
uint8 GPIOIE_0_Shadow=0;
uint8 GPIORIS_0_Shadow=0;
uint8 GPIOMIS_0_Shadow=0;
uint8 GPIOIC_0_Shadow=0;

uint8 GPIODIR_1_Shadow=0;
uint8 GPIOIS_1_Shadow=0;
uint8 GPIOIBE_1_Shadow=0;
uint8 GPIOIEV_1_Shadow=0;
uint8 GPIOIE_1_Shadow=0;
uint8 GPIORIS_1_Shadow=0;
uint8 GPIOMIS_1_Shadow=0;
uint8 GPIOIC_1_Shadow=0;


void gpio_delay_us(float dly1us)
{
	volatile uint32 jj=0;
	//for(ii=0;ii<units_of_u_secs;ii++)
	while(dly1us--)
	{
		jj++;
	}
	
	jj=0;
}

int gpio_set_input(GPIO_GROUP_ID_T group_id, uint8 offset)
{
	uint8 gpiodir;

	if(group_id==GPIO_GROUP_0)
		gpiodir=HWreg_GPIO_0_GET_REG(PL061_GPIODIR_0);
	else if(group_id==GPIO_GROUP_1)
		gpiodir=HWreg_GPIO_1_GET_REG(PL061_GPIODIR_1);		

	gpiodir &= ~(BIT(offset)); 

	if(group_id==GPIO_GROUP_0)
		HWreg_GPIO_0_SET_REG(gpiodir, PL061_GPIODIR_0);
	else if (group_id==GPIO_GROUP_1)
		HWreg_GPIO_1_SET_REG(gpiodir, PL061_GPIODIR_1);

	return 0;
}

int gpio_set_output(GPIO_GROUP_ID_T group_id, uint8 offset)
{
	uint8 gpiodir,gpio_idx;
	
	

	if(group_id==GPIO_GROUP_0)		
		gpiodir=HWreg_GPIO_0_GET_REG(PL061_GPIODIR_0);
	else if(group_id==GPIO_GROUP_1)
		gpiodir=HWreg_GPIO_1_GET_REG(PL061_GPIODIR_1);

	gpio_idx = BIT(offset);
	gpiodir  = gpiodir | gpio_idx;

	//writeb(gpiodir, chip->base + GPIODIR);  
	if(group_id==GPIO_GROUP_0)
		HWreg_GPIO_0_SET_REG(gpiodir, PL061_GPIODIR_0);
	else if (group_id==GPIO_GROUP_1)
		HWreg_GPIO_1_SET_REG(gpiodir, PL061_GPIODIR_1);	

	return 0;
}

/* gpio value */
uint8 gpio_get_value(GPIO_GROUP_ID_T group_id,uint8 gpio_offset)
{
	uint8 gpio_status;

	if(group_id==GPIO_GROUP_0)
		return (*((volatile unsigned long *)(GPIO_GROUP0_BASE+(1 << (gpio_offset + 2)))));
	
	else if(group_id==GPIO_GROUP_1)
	{
		gpio_status = (*((volatile unsigned long *)(GPIO_GROUP1_BASE+(1 << (gpio_offset + 2)))));

		return gpio_status;
	}	
	else
		return 0xff;		
}


//BOOL gpio_val, 0 or 1
void gpio_set_value(GPIO_GROUP_ID_T group_id, BOOL gpio_val, uint8 gpio_offset)
{
	if(group_id==GPIO_GROUP_0)
		HWreg_GPIO_0_SET_REG(gpio_val << gpio_offset, (1 << (gpio_offset + 2)));
	
	else if(group_id==GPIO_GROUP_1)
		HWreg_GPIO_1_SET_REG(gpio_val << gpio_offset, (1 << (gpio_offset + 2)));
}








//IRQ_TRIGGER_RISING_EDGE

/* gpio edge interrupt */
void gpio_set_eint_rising(GPIO_GROUP_ID_T group_id,uint8 gpio_offset)
{
	
    gpio_set_input(group_id,gpio_offset);
	if(group_id==GPIO_GROUP_0)
	{
		gpio_offset = BIT(gpio_offset);
			
		GPIOIS_0_Shadow &= (~gpio_offset);//
		GPIOIS_0=	GPIOIS_0_Shadow;//set Corresponding bit to "0"
		GPIOIS_0	&= (~gpio_offset);
     
		GPIOIBE_0_Shadow &=(~gpio_offset);
		GPIOIBE_0 =	GPIOIBE_0_Shadow;//set Corresponding bit to "0"
		GPIOIBE_0	&=(~gpio_offset);
    
		GPIOIEV_0_Shadow |= gpio_offset;
		GPIOIEV_0 = GPIOIEV_0_Shadow;//set Corresponding bit to "1"
		GPIOIEV_0 	 |= gpio_offset;
	}
	else if(group_id==GPIO_GROUP_1)
	{
		gpio_offset = BIT(gpio_offset);
			
		GPIOIS_1_Shadow &= (~gpio_offset);//
		GPIOIS_1=	GPIOIS_1_Shadow;//set Corresponding bit to "0"
		GPIOIS_1	&= (~gpio_offset);
     
		GPIOIBE_1_Shadow &=(~gpio_offset);
		GPIOIBE_1 =	GPIOIBE_1_Shadow;//set Corresponding bit to "0"
		GPIOIBE_1	&=(~gpio_offset);
    
		GPIOIEV_1_Shadow |= gpio_offset;
		GPIOIEV_1 = GPIOIEV_1_Shadow;//set Corresponding bit to "1"	
		GPIOIEV_1	 |= gpio_offset;
	}
}


//IRQ_TRIGGER_FALLING_EDGE
void gpio_set_eint_falling(GPIO_GROUP_ID_T group_id,uint8 gpio_offset)
{
	gpio_set_input(group_id,gpio_offset);
	
	if(group_id==GPIO_GROUP_0)
	{
		gpio_offset = BIT(gpio_offset);

		GPIOIS_0_Shadow &= (~gpio_offset);//
		GPIOIS_0=	GPIOIS_0_Shadow;//set Corresponding bit to "0"
		GPIOIS_0	&= (~gpio_offset);

		GPIOIBE_0_Shadow &=(~gpio_offset);
		GPIOIBE_0 =	GPIOIBE_0_Shadow;//set Corresponding bit to "0"
		GPIOIBE_0	&=(~gpio_offset);

		GPIOIEV_0_Shadow &= gpio_offset;
		GPIOIEV_0 = GPIOIEV_0_Shadow;//set Corresponding bit to "0"
		GPIOIEV_0&= gpio_offset;	
	}
	else if(group_id==GPIO_GROUP_1)
	{
		gpio_offset = BIT(gpio_offset);

		GPIOIS_1_Shadow &= (~gpio_offset);//
		GPIOIS_1=	GPIOIS_1_Shadow;//set Corresponding bit to "0"
		GPIOIS_1	&= (~gpio_offset);

		GPIOIBE_1_Shadow &=(~gpio_offset);
		GPIOIBE_1 =	GPIOIBE_1_Shadow;//set Corresponding bit to "0"
		GPIOIBE_1	&=(~gpio_offset);

		GPIOIEV_1_Shadow &= gpio_offset;
		GPIOIEV_1 = GPIOIEV_1_Shadow;//set Corresponding bit to "0"	
		GPIOIEV_1&= gpio_offset;			
	}	
}

//IRQ_TRIGGER_BOTH_EDGES
void gpio_set_eint_both(GPIO_GROUP_ID_T group_id,uint8 gpio_offset)
{
    gpio_set_input(group_id,gpio_offset);
	
	if(group_id==GPIO_GROUP_0)
	{
		gpio_offset = BIT(gpio_offset);
			
		GPIOIS_0_Shadow &= (~gpio_offset);//
		GPIOIS_0=	GPIOIS_0_Shadow;//set Corresponding bit to "0"
		GPIOIS_0	&= (~gpio_offset);
	 
		GPIOIBE_0_Shadow |= gpio_offset;
		GPIOIBE_0 =	GPIOIBE_0_Shadow;//set Corresponding bit to "1"
			
		GPIOIBE_0	|= gpio_offset;
	}
	else if(group_id==GPIO_GROUP_1)
	{
		gpio_offset = BIT(gpio_offset);
		
//		GPIOIS_1_Shadow &= (~gpio_offset);//
//		GPIOIS_1=	GPIOIS_1_Shadow;//set Corresponding bit to "0"			
		GPIOIS_1&= (~gpio_offset);
 
//		GPIOIBE_1_Shadow |= gpio_offset;
//		GPIOIBE_1 =	GPIOIBE_1_Shadow;//set Corresponding bit to "1"
		GPIOIBE_1 |= gpio_offset;
	}		
}




/* gpio level interrupt */
//void gpio_set_lint_high(uint32 gpio_bn,void (*lint_func)(void))
void gpio_set_lint_high(GPIO_GROUP_ID_T group_id,uint8 gpio_offset)
{

	gpio_set_input(group_id,gpio_offset);
	if(group_id==GPIO_GROUP_0)
	{
		gpio_offset = BIT(gpio_offset);

//		GPIOIS_Shadow |= gpio_offset;//
//		GPIOIS_0=	GPIOIS_Shadow;//set Corresponding bit to "1"
		GPIOIS_0	|= gpio_offset;

//		GPIOIBE_Shadow &= (~gpio_offset);
//		GPIOIBE_0 =	GPIOIBE_Shadow;//set Corresponding bit to "0"
		GPIOIBE_0	&= (~gpio_offset);

//		GPIOIEV_Shadow |= gpio_offset;
//    	GPIOIEV_0 = GPIOIEV_Shadow;//set Corresponding bit to "1"	
		GPIOIEV_0|= gpio_offset;			
	}
	else if(group_id==GPIO_GROUP_1)
	{
		gpio_offset = BIT(gpio_offset);

//		GPIOIS_1_Shadow |= gpio_offset;//
//		GPIOIS_1=	GPIOIS_1_Shadow;//set Corresponding bit to "1"
		GPIOIS_1	|= gpio_offset;

//		GPIOIBE_1_Shadow &= (~gpio_offset);
//		GPIOIBE_1 =	GPIOIBE_1_Shadow;//set Corresponding bit to "0"
		GPIOIBE_1	&= (~gpio_offset);

//		GPIOIEV_1_Shadow |= gpio_offset;
//    	GPIOIEV_1 = GPIOIEV_1_Shadow;//set Corresponding bit to "1"	
		GPIOIEV_1|= gpio_offset;			
	}
}


//void gpio_set_lint_low(uint32 gpio_bn,void (*lint_func)(void))
void gpio_set_lint_low(GPIO_GROUP_ID_T group_id,uint8 gpio_offset)
{
	gpio_set_input(group_id,gpio_offset);
	
	if(group_id==GPIO_GROUP_0)
	{
		gpio_offset = BIT(gpio_offset);

//		GPIOIS_Shadow |= gpio_offset;//
//		GPIOIS_0=	GPIOIS_Shadow;//set Corresponding bit to "1"
		GPIOIS_0	|= gpio_offset;

//		GPIOIBE_Shadow &= (~gpio_offset);
//		GPIOIBE_0 =	GPIOIBE_Shadow;//set Corresponding bit to "0"
		GPIOIBE_0	&= (~gpio_offset);

//		GPIOIEV_Shadow &= (~gpio_offset);
//    GPIOIEV_0 = GPIOIEV_Shadow;//set Corresponding bit to "0"	
		GPIOIEV_0 &= (~gpio_offset);  
	}
	else if(group_id==GPIO_GROUP_1)
	{
		gpio_offset = BIT(gpio_offset);

//		GPIOIS_1_Shadow |= gpio_offset;//
//		GPIOIS_1=	GPIOIS_1_Shadow;//set Corresponding bit to "1"
		GPIOIS_1	|= gpio_offset;

//		GPIOIBE_1_Shadow &= (~gpio_offset);
//		GPIOIBE_1 =	GPIOIBE_1_Shadow;//set Corresponding bit to "0"
		GPIOIBE_1	&= (~gpio_offset);

//		GPIOIEV_1_Shadow &= (~gpio_offset);
//    GPIOIEV_1 = GPIOIEV_1_Shadow;//set Corresponding bit to "0"	
		GPIOIEV_1&= (~gpio_offset);
	}		
}


/******************************************************************
*
* gpio test
*
*******************************************************************/
#define ARMSOC_LED0         GPIO_B0
#define ARMSOC_LED1         GPIO_B1
#define ARMSOC_LED2         GPIO_B2
#define ARMSOC_LED3         GPIO_B3
#define ARMSOC_LEDALL       ( ARMSOC_LED0|ARMSOC_LED1|ARMSOC_LED2|ARMSOC_LED3 )

#define LED0                0
#define LED1                1
#define LED2                2
#define LED3                3

#define LED_ALL             ARMSOC_LEDALL


void set_led_on(uint32 led_num)
{
    switch (led_num)
    {
        case LED0:
            gpio_set_value(GPIO_GROUP_0,1,ARMSOC_LED0);
            break;
        case LED1:
            gpio_set_value(GPIO_GROUP_0,1,ARMSOC_LED1);
            break;
        case LED2:
            gpio_set_value(GPIO_GROUP_0,1,ARMSOC_LED2);
            break;
        case LED3:
            gpio_set_value(GPIO_GROUP_0,1,ARMSOC_LED3);
            break;
//        case LED_ALL:
//            gpio_set_value(GPIO_GROUP_0,1,ARMSOC_LEDALL);
//            break;
        default:
            break;
    }
		
		__asm(" NOP");
		__asm(" NOP");
		__asm(" NOP");
		__asm(" NOP");
		__asm(" NOP");
		__asm(" NOP");
		__asm(" NOP");
		__asm(" NOP");
		__asm(" NOP");
		__asm(" NOP");
}


void set_led_off(uint32 led_num)
{
    switch (led_num)
    {
        case LED0:
            gpio_set_value(GPIO_GROUP_0,0,ARMSOC_LED0);
            break;
        case LED1:
            gpio_set_value(GPIO_GROUP_0,0,ARMSOC_LED1);
            break;
        case LED2:
            gpio_set_value(GPIO_GROUP_0,0,ARMSOC_LED2);
            break;
        case LED3:
            gpio_set_value(GPIO_GROUP_0,0,ARMSOC_LED3);
            break;
//        case LED_ALL:
//            gpio_set_value(GPIO_GROUP_0,0,ARMSOC_LEDALL);
//            break;
        default:
            break;
    }

	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
}




void gpio_hall_int_callback_func(void)
{
  uint8 gpio_offset=5;
	
	gpio_offset = BIT(gpio_offset);
	GPIOIC_0_Shadow |= gpio_offset;
  
	//GPIOIC (interrupt-clear) register
	GPIOIC_0=GPIOIC_0_Shadow;//set Corresponding bit to "1",clear gpio_0_5 interrupt
	GPIOIC_0=BIT(gpio_offset);
		
}





//Hall Key ---> GPIO_0_5
//set GPIO_A_5 Input 
void GPIO_Hall_IRQ_Init(void)
{
	uint8 gpio_offset=5;

	gpio_offset = BIT(gpio_offset);

	GPIOIE_0_Shadow=gpio_offset;
	GPIOIE_0=0;//disable gpio group0 interrupt

//gpio input	
	GPIODIR_0_Shadow &= (~gpio_offset);
	GPIODIR_0 = GPIODIR_0_Shadow;//input,set Corresponding bit to "0" ,input mode
	GPIODIR_0 &= (~gpio_offset);

	GPIOIE_0_Shadow |= gpio_offset;
	GPIOIE_0 = GPIOIE_0_Shadow;//set Corresponding bit to "1" ,interrupt enable
	GPIOIE_0 |= gpio_offset;

	//Disable level detection 
	GPIOIS_0_Shadow &= (~gpio_offset);
	GPIOIS_0 = GPIOIS_0_Shadow;
	GPIOIS_0 &= (~gpio_offset);

	//Clear detection on both edges */
	GPIOIBE_0_Shadow &= (~gpio_offset);
	GPIOIBE_0 = GPIOIBE_0_Shadow;
	GPIOIBE_0&= (~gpio_offset);

	//if (rising)
	//GPIOIEV |= bit;
	//else
	//GPIOIEV &= ~bit;		
	GPIOIEV_0_Shadow &= (~gpio_offset);
	GPIOIEV_0 = GPIOIEV_0_Shadow;//falling edge trigger
	GPIOIEV_0 &= (~gpio_offset);

#if 1
	/******* configure the GPIO NVIC ********************************/
	hal_nvic_clear_pending_irq(GPIO_A5_IRQn);//GPIO_0_5(GPIO05)
	hal_nvic_set_priority(GPIO_A5_IRQn, 0);
	hal_nvic_enable_irq(GPIO_A5_IRQn);	
	//hal_nvic_disable_irq(GPIO_05_MISC_IRQn);
#endif	
	
}




/*************************************Modify for JiangJun*****************************************************/
//
/*************************************High Level Interrupt****************************************************/

//Hall Key ---> GPIO_0_5
//set GPIO_A_5 Input 
void GPIO_Hall_HighLevel_Interrupt_Init(void)
{
    uint8 gpio_offset=5;
	gpio_set_lint_high(GPIO_GROUP_0,gpio_offset);
#if 1
	/******* configure the GPIO NVIC ********************************/
	hal_nvic_clear_pending_irq(GPIO_A5_IRQn);//GPIO_0_5(GPIO05)
	hal_nvic_set_priority(GPIO_A5_IRQn, 0);
	hal_nvic_enable_irq(GPIO_A5_IRQn);	
#endif
}




/*************************************Modify for JiangJun*****************************************************/
//
/*************************************Low Level Interrupt****************************************************/

//Hall Key ---> GPIO_0_5
//set GPIO_A_5 Input 
void GPIO_Hall_LowLevel_Interrupt_Init(void)
{
    uint8 gpio_offset=5;
		gpio_set_lint_low(GPIO_GROUP_0,gpio_offset);
#if 1
		/******* configure the GPIO NVIC ********************************/
		hal_nvic_clear_pending_irq(GPIO_A5_IRQn);//GPIO_0_5(GPIO05)
		hal_nvic_set_priority(GPIO_A5_IRQn, 0);
		hal_nvic_enable_irq(GPIO_A5_IRQn);	
#endif	
	
}



void GPIOB4_init(void)
{
	GPIODIR_1_Shadow |= BIT(4);  //set GPIOB4 for output
  GPIODIR_1 = GPIODIR_1_Shadow;	
	HWreg_GPIO_1_SET_REG(0 << 4, (1 << (4 + 2)));  //GPIOB4 = 0
}


void set_gpiob4_h(void)
{
		HWreg_GPIO_1_SET_REG(1 << 4, (1 << (4 + 2)));  //GPIOB4 = 0
}

void set_gpiob4_l(void)
{
		HWreg_GPIO_1_SET_REG(0 << 4, (1 << (4 + 2)));  //GPIOB4 = 0
}







void set_uart1_for_gpio_h(void)
{
//	 gecko_pinmux_config(PAD21,GPIO_0_3);	//uart2_dat As GPIO_0_3	
//	
//   GPIODIR_0_Shadow|= BIT(3);//#define BIT(x)  (1<<(x))
//   //GPIODIR_Shadow|= (1<<(gpio_offset));
//   GPIODIR_0 = GPIODIR_0_Shadow;
//	 gpio_set_value(GPIO_GROUP_0,1,3);//set GPIO_P0_3 =1	
}

void set_uart1_for_gpio_l(void)
{
//	 gecko_pinmux_config(PAD21,GPIO_0_3);	//uart2_dat As GPIO_0_3	
//	
//   GPIODIR_0_Shadow|= BIT(3);//#define BIT(x)  (1<<(x))
//   //GPIODIR_Shadow|= (1<<(gpio_offset));
//   GPIODIR_0 = GPIODIR_0_Shadow;
//	 gpio_set_value(GPIO_GROUP_0,0,3);//set GPIO_P0_3 =0		
}




void GpiopinMode(uint8 pin,uint8 mode) {
		
 if(mode==INPUT)
 {	 
		//if((pin>=0) &&  (pin<=7))
	  if(pin<=7)
		{
			//GPIODIR_Shadow&= (~BIT(pin));
			//GPIODIR_0 = GPIODIR_Shadow;
			GPIODIR_0&= (~BIT(pin));
		}
		else if((pin>=8) &&  (pin<=15))
		{
			pin=pin-8;
			//GPIODIR_1_Shadow&= (~BIT(pin));
			//GPIODIR_1 = GPIODIR_1_Shadow;
			GPIODIR_1&= (~BIT(pin));	
		}
		
 }
 else if(mode==OUTPUT)
 {
		//if((pin>=0) &&  (pin<=7))
	  if(pin<=7)
		{
			//GPIODIR_Shadow|= BIT(pin);
			//GPIODIR_0 = GPIODIR_Shadow;
			GPIODIR_0|= BIT(pin);
		}
		else if((pin>=8) &&  (pin<=15))
		{
			pin=pin-8;
			//GPIODIR_1_Shadow|= BIT(pin);
			//GPIODIR_1 = GPIODIR_1_Shadow;
			GPIODIR_1|= BIT(pin);
		}
 }
	
}





void GpiopinWrite(uint8 pin,uint8 val) 
//void GpiopinWrite(uint8 pin,uint8 val)
{

//set gpio port output
//		GPIODIR_0 =0xff;
//		GPIODIR_1 =0xff;			

		//if((pin>=0) &&  (pin<=7))
	  if(pin<=7)
		{
			//#define HWreg_GPIO_0_SET_REG(val,pinoffset)          (*(volatile uint32*)(GPIO_GROUP0_BASE + (pinoffset))) = (uint32)(val)
			//HWreg_GPIO_0_SET_REG(gpio_val << gpio_offset, (1 << (gpio_offset + 2)));
			
			//#define GPIO_GROUP_A_BASE       (0x40018000)
			(*(volatile uint32*)(0x40018000 + (1 << (pin + 2)) ) ) = (uint32)(val << pin);
   
		}
		else if((pin>=8) &&  (pin<=15))
		{
			//#define GPIO_GROUP_B_BASE       (0x40019000)
			pin=pin-8;
			(*(volatile uint32*)(0x40019000 + (1 << (pin + 2)) ) ) = (uint32)(val << pin);
		}

#if 0		
		else if((pin>=16) &&  (pin<=23))
		{
			//#define GPIO_GROUP_C_BASE       (0x40013000)
			pin=pin-16;
			(*(volatile uint32*)(0x40013000 + (1 << (pin + 2)) ) ) = (uint32)(val << pin);
		}		
#endif
		
}

//KEY Port(GPIOB6)
uint8 GpiopinRead(uint8 pin) 
{

	uint8 gpio_status;

#if 0	
	gecko_pinmux_config(PAD18,GPIO_B_6);//config PAD18 as gpio	

	GPIODIR_1 &=0xBF;//set GPIOB6 direction to "0"
#endif	
	
	if(pin<=7)
	{
		return (*((volatile unsigned long *)(GPIO_GROUP0_BASE+(1 << (pin + 2)))));
	}
	else if((pin>=8) && (pin<=15))
	{
		pin=pin-8;
		gpio_status = (*((volatile unsigned long *)(GPIO_GROUP1_BASE+(1 << (pin + 2)))));
		return gpio_status;
	}	
	

	return 0xff;
	
}





//#define LED_PIN  (PB3)
//GPIO_Toggle(LED_PIN);

__RAM_CODE__ void GPIO_Toggle(uint8 pin) 
{
			if((pin>=8) &&  (pin<=15))		
			pin=pin-8;	
//		GpiopinMode(LED_PIN,OUTPUT);	
//		GpiopinWrite(LED_PIN,HIGH);
//		GpiopinWrite(LED_PIN,LOW);	
			while(1)
			{
				(*(volatile uint32*)(0x40019000 + (1 << (pin + 2)) ) ) = (uint32)(1 << pin);	
				(*(volatile uint32*)(0x40019000 + (1 << (pin + 2)) ) ) = (uint32)(0 << pin);	
			}	
			
}






/******************************************************************************
**                            End Of File
******************************************************************************/



//PAD16
//GPIOB4
uint8 K27_KEY_Detect(void)
{
	
	
	uint32 iomux_val;
  //GPIOB4	
	uint8 pin=4;//GPIOB4(PAD16)


	gecko_pinmux_config(PAD16,GPIO_B_4);

/*

// pinmux in comments
assign GPIOB_4_iin =
                    (pad_fun0[24]==1'b0 && pad_fun1[24]==1'b0) ? pad_24_pin :
                    pad_fun1[16] ? pad_16_pin :
                                   1'b1;

*/	

	
	iomux_val = SYS_HW32_REG_RD(0x300);
	iomux_val|=FUNC1_SPI_CS2;
	SYS_HW32_REG_WR(0x300,iomux_val);
	
	iomux_val = SYS_HW32_REG_RD(0x304);
	iomux_val|=FUNC1_SPI_CS2;
	SYS_HW32_REG_WR(0x304,iomux_val);


  GPIODIR_1 &=~BIT(pin);
	
	return (*((volatile unsigned long *)(GPIO_GROUP1_BASE+(1 << (pin + 2)))));
	
}









#if 0
//PAD11
//GPIOA7
uint8 K27_KEY_Detect(void)
{
		uint32 iomux_val;
//GPIOA7	
	uint8 pin=7;//GPIOA7(PAD11)


	gecko_pinmux_config(PAD11,GPIO_A_7);

/*

	assign GPIOA_7_iin =
                    pad_fun0[6] ? pad_06_pin :
                    pad_fun1[11] ? pad_11_pin :
                                   1'b1;

*/	

	//clear func0
	iomux_val = SYS_HW32_REG_RD(0x300);
	iomux_val&=(~FUNC1_UART0_SOUT);
	SYS_HW32_REG_WR(0x300,iomux_val);

	//clear func1
	iomux_val = SYS_HW32_REG_RD(0x304);
	iomux_val&=(~FUNC0_GPIO_A_7);
	SYS_HW32_REG_WR(0x304,iomux_val);
	


  GPIODIR_0 &=~BIT(7);
	
	return (*((volatile unsigned long *)(GPIO_GROUP0_BASE+(1 << (pin + 2)))));
	
}






//KEY Port(GPIOA6)


//GPIOA6
uint8 K27_KEY_Detect(void)
{
	
//GPIOA6	
	uint8 pin=6;//GPIOA6(PAD10)

	gecko_pinmux_config(PAD10,GPIO_A_6);//config PAD10 as gpio

  GPIODIR_0 &=~BIT(6);
	
	return (*((volatile unsigned long *)(GPIO_GROUP0_BASE+(1 << (pin + 2)))));
	
}




//GPIOA5
uint8 K27_KEY_Detect(void)
{
	
//GPIOA5	
	uint8 pin=5;//GPIOA5(PAD9)

	gecko_pinmux_config(PAD9,GPIO_A_5);//config PAD9 as gpio

  GPIODIR_0 &=~BIT(5);
	
	return (*((volatile unsigned long *)(GPIO_GROUP0_BASE+(1 << (pin + 2)))));
	
}


#endif 








//GPIOB6(KEY2)
uint8 KP85_KEY2_Detect(void)
{
	//uint8 gpio_status;
	uint8 pin=6;//GPIOA6

	//gecko_pinmux_config(PAD6,GPIOA_7);//config PAD06 as gpio	
	gecko_pinmux_config(PAD18,GPIO_B_6);//config PAD11 as gpio

	//GPIODIR_1 &=0xBF;//set GPIOA6 direction to input (config it to "0")
  GPIODIR_1 &=~BIT(6);
	pin=6;
	
	return (*((volatile unsigned long *)(GPIO_GROUP1_BASE+(1 << (pin + 2)))));

}





//GPIOB4
uint8 BSR1901_KEY1_Detect(void)
{
	//uint8 gpio_status;
	uint8 pin=4;
	
	bsr1901_pullup_pulldown_config(PAD_16,PAD_PULLUP);
	

	gecko_pinmux_config(PAD16,GPIO_B_4);//config PAD16 as gpio

  GPIODIR_1 &=~BIT(4);//set GPIOB4 direction to input (config it to "0")

	return (*((volatile unsigned long *)(GPIO_GROUP1_BASE+(1 << (pin + 2)))));
}




//GPIOB5
uint8 BSR1901_KEY2_Detect(void)
{
	//uint8 gpio_status;
	uint8 pin=5;

	gecko_pinmux_config(PAD17,GPIO_B_5);//config PAD16 as gpio

  GPIODIR_1 &=~BIT(5);//set GPIOB5 direction to input (config it to "0")

	return (*((volatile unsigned long *)(GPIO_GROUP1_BASE+(1 << (pin + 2)))));
}







//GPIO_A_6,air flow sensor (mi tou ßäÍ·)
uint8 mi_tou_detect(void)
{
	
	//uint8 gpio_status;
	uint8 pin=6;//GPIOA6(PAD10)

	gecko_pinmux_config(PAD10,GPIO_A_6);//config PAD10 as gpio
	
	//GEK1109_PAD10_pulldown_Config();

	//GPIODIR_0 &=0xBF;//set GPIOA6 direction to input (config it to "0")	
	GPIODIR_0 &=~BIT(6);

	pin=6;
	
	return (*((volatile unsigned long *)(GPIO_GROUP0_BASE+(1 << (pin + 2)))));
	
}







//DC Insert Detect  ---> GPIO_0_6
//set GPIO_A_6 Input 
void GPIO_5V_DCIN_Init(void)
{
    uint8 gpio_offset=6;
	
	  gpio_offset = BIT(gpio_offset);
	
	  GPIOIE_0_Shadow=gpio_offset;
		GPIOIE_0=0;//disable gpio group0 interrupt

//gpio input	
	  GPIODIR_0_Shadow &= (~gpio_offset);
	  GPIODIR_0 = GPIODIR_0_Shadow;//input,set Corresponding bit to "0" ,input mode
	
	  GPIOIE_0_Shadow |= gpio_offset;
	  GPIOIE_0 = GPIOIE_0_Shadow;//set Corresponding bit to "1" ,interrupt enable



		//Disable level detection 
		GPIOIS_0_Shadow &= (~gpio_offset);
    GPIOIS_0 = GPIOIS_0_Shadow;	
		
    //Clear detection on both edges */
		GPIOIBE_0_Shadow &= (~gpio_offset);
    GPIOIBE_0 = GPIOIBE_0_Shadow;
		//if (rising)
		//GPIOIEV |= bit;
		//else
		//GPIOIEV &= ~bit;
		
		GPIOIEV_0_Shadow &= (~gpio_offset);
		GPIOIEV_0 = GPIOIEV_0_Shadow;//falling edge trigger

#if 1
		/******* configure the GPIO NVIC ********************************/
		hal_nvic_clear_pending_irq(GPIO_A6_IRQn);//GPIO_0_6(GPIOA6)
		hal_nvic_set_priority(GPIO_A6_IRQn, 0);
		hal_nvic_enable_irq(GPIO_A6_IRQn);	
#endif	
	
}




void Set_GPIO_B7_Input(void)
{
    uint8 gpio_offset=7;
	
	  gpio_offset = BIT(gpio_offset);
	
//gpio input	
	  GPIODIR_1_Shadow &= (~gpio_offset);
	  GPIODIR_1 = GPIODIR_1_Shadow;//input,set Corresponding bit to "0" ,input mode
	  GPIODIR_1 &= (~gpio_offset);
	
}




void Set_GPIOA6_Input(void)
{
    uint8 gpio_offset=6;
	
	  gpio_offset = BIT(gpio_offset);
	
//gpio input	
	  GPIODIR_0_Shadow &= (~gpio_offset);
	  GPIODIR_0 = GPIODIR_0_Shadow;//input,set Corresponding bit to "0" ,input mode
	  GPIODIR_0 &= (~gpio_offset);
	
}


void Set_GPIOA6_output(void)
{
    uint8 gpio_offset=6;
	
	  gpio_offset = BIT(gpio_offset);
	
//gpio output	
	  GPIODIR_0 |= gpio_offset;
	
}



#define KEY1  (PB5)
#define LED_PIN  (PB3)

void Key1_Detect(void)
{
		//GpioPinRead(KEY1);
		if(GpiopinRead(KEY1) != 0x20)
		{
			GpiopinMode(PB3,OUTPUT);
			GpiopinWrite(PB3,HIGH);
		}	
}






	
	#if 0
	//PAD0(GPIOA1)---SWCLK
	gecko_pinmux_config(PAD0,GPIO_A_0);
	//GPIO_InitIO(OUTPUT,PA0);	
	GpiopinMode(PA0,OUTPUT);		
  while(1)
	{
		GPIO_WriteIO(HIGH, PA0); 

		GPIO_WriteIO(LOW, PA0);

	}
	#endif	
	
	

	#if 0
	//PAD0(GPIOA1)---SWCLK
	gecko_pinmux_config(PAD0,GPIO_A_0);
	//GPIO_InitIO(OUTPUT,PA0);	
	GpiopinMode(PA0,OUTPUT);		
  while(1)
	{
		GPIO_WriteIO(HIGH, PA0); 

		GPIO_WriteIO(LOW, PA0);

	}
	#endif	

	
	#if 0
	//PAD1(GPIOA1)---SWDIO
	gecko_pinmux_config(PAD1,GPIO_A_1);
	//GPIO_InitIO(OUTPUT,PA1);	
	GpiopinMode(PA1,OUTPUT);		
  while(1)
	{
		GPIO_WriteIO(HIGH, PA1); 

		GPIO_WriteIO(LOW, PA1);

	}
	#endif	

	
	
	#if 0
	//PAD6
	gecko_pinmux_config(PAD6,GPIOA_7);//LCD reset
	GPIO_InitIO(OUTPUT,PA7);	
	GpiopinMode(PA7,OUTPUT);		
  while(1)
	{
		//GPIO_WriteIO(HIGH, PA7);
		LCD_RST_SET;
		
		//GPIO_WriteIO(LOW, PA7);
		LCD_RST_CLR;
	}
	#endif
	
	
	
	#if 0
	//PAD7(GPIOB7)
	gecko_pinmux_config(PAD7,GPIOB_7);//

	GPIO_InitIO(OUTPUT,PB7);	
	GpiopinMode(PB7,OUTPUT);		
  while(1)
	{
		GPIO_WriteIO(HIGH, PB7); 

		GPIO_WriteIO(LOW, PB7);
	}

	
  #else
//	//PAD19(GPIOB7)
//	gecko_pinmux_config(PAD19,GPIO_B_7);
//	
//	
//	GPIO_InitIO(OUTPUT,PB7);	
//	GpiopinMode(PB7,OUTPUT);		
//  while(1)
//	{
//		GPIO_WriteIO(HIGH, PB7); 
//		//bsr1901_pullup_pulldown_config(PAD_07,PAD_PULLUP);
//		GPIO_WriteIO(LOW, PB7);
//		//bsr1901_pullup_pulldown_config(PAD_07,PAD_PULLDOWN);
//	}
	#endif
	




	#if 0
	//PAD8
	gecko_pinmux_config(PAD8,GPIO_A_4);	
	GPIO_InitIO(OUTPUT,PA4);	
	GpiopinMode(PA4,OUTPUT);		
  while(1)
	{
		GPIO_WriteIO(HIGH, PA4);
		GPIO_WriteIO(LOW, PA4);
	}
	#endif
	
	
	
	#if 0
	//PAD9
	gecko_pinmux_config(PAD9,GPIO_A_5);	
	GPIO_InitIO(OUTPUT,PA5);	
	GpiopinMode(PA5,OUTPUT);		
  while(1)
	{
		GPIO_WriteIO(HIGH, PA5);
		GPIO_WriteIO(LOW, PA5);
	}
	#endif
	


	#if 0
	//PAD11
	gecko_pinmux_config(PAD11,GPIO_A_7);	
	GPIO_InitIO(OUTPUT,PA7);	
	GpiopinMode(PA7,OUTPUT);		
  while(1)
	{
		GPIO_WriteIO(HIGH, PA7);
		GPIO_WriteIO(LOW, PA7);
	}
	#endif




	#if 0
	//PAD16(GPIOB4)
	gecko_pinmux_config(PAD16,GPIO_B_4);	
	GPIO_InitIO(OUTPUT,PB4);	
	//GpiopinMode(PB4,OUTPUT);		
  while(1)
	{
		GPIO_WriteIO(HIGH, PB4);
		GPIO_WriteIO(LOW, PB4);
	}
	#endif
	


	#if 0
	//PAD21
	gecko_pinmux_config(PAD21,GPIO_A_3);//LCD DC
	GPIO_InitIO(OUTPUT,PA3);	
	GpiopinMode(PA3,OUTPUT);		
  while(1)
	{
		GPIO_WriteIO(HIGH, PA3);
		GPIO_WriteIO(LOW, PA3);
	}
	#endif	
	


	#if 0
  //PAD24 LCD BL Control
	GPIO_InitIO(OUTPUT,PB4);	
	GpiopinMode(PB4,OUTPUT);		
  while(1)
	{
		//GPIO_WriteIO(HIGH, PB4);
		LCD_BL_SET;
		
		//GPIO_WriteIO(LOW, PB4);
		LCD_BL_CLR;
		
	}
	#endif
	
	#if 0
	//PAD24(GPIOB4)
	//default function GPIO
	//GPIO_InitIO(OUTPUT,PB4);	
	GpiopinMode(PB4,OUTPUT);		
  while(1)
	{
		GPIO_WriteIO(HIGH, PB4); 

		GPIO_WriteIO(LOW, PB4);

	}
	#endif		
	
	



