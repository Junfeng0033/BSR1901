
/****************************************************************************
 *   $Id:: isr.c 2020-10-19 
 *   Project: Interrupt Service Routines
 *	 Author: Moana
 *   Description: Interrupt Service Routines
****************************************************************************/
#include "platform_config.h"
#include "bsr1901.h"
#include "uart.h"

#include "gpio.h"
#include "watchdog.h"
#include "pwm.h"
#include "spi.h"
#include "lcd_driver.h"


uint8 dc5v_online_flag=0;

extern const unsigned char gImage_60X160_2[19200];
extern const unsigned char gImage_60X160_flower[19200];

extern void watchdog_irq_clear(void);

extern void gecko_timer1_irq_handler(void);

extern void Clear_DMA_Interrupt(void);
extern void DMA_ISR_Routine(void);

extern uint32_t Get_SysTick_VALUE(void);

extern void HW_SPI_Interrupt_Handler(void);


/* Cortex-M0 Processor Exception Handlers -----------------------------------*/
/**
  * @brief  Handle NMI
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  Handle hard fault
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    while (1)
    {
    }
}

/**
  * @brief  Handle SVC
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  Handle PendSV
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}





volatile uint32_t TimeTick = 0;
extern uint32_t system_tick;


//(*(volatile uint32*)(0x40019000 + (1 << (pin + 2)) ) ) = (uint32)(1 << pin);	
//(*(volatile uint32*)(0x40019000 + (1 << (pin + 2)) ) ) = (uint32)(0 << pin);	




__RAM_CODE__ void SysTick_Handler(void)  //interrupt routine
{
	

	TimeTick++;//TimeTick will increase by "1"  every 1000us
	
	//system_tick=TimeTick;
	
#if 0			
	if(TimeTick%2)
		//CST6118_Motor_PWM_Control(pwm_freq,17,100);
	  //LCD_BL_CLR;
	  //(*(volatile uint32*)(0x40019000 + (1 << (2 + 2)) ) ) = (uint32)(0 << 2);
		CST6118_Motor_PWM7_Control_Optimized(150000,20);
	else
		//CST6118_Motor_PWM_Control(pwm_freq,82,100);
	  //LCD_BL_SET;
	  //(*(volatile uint32*)(0x40019000 + (1 << (2 + 2)) ) ) = (uint32)(1 << 2);
	  CST6118_Motor_PWM7_Control_Optimized(150000,80);
#endif	
	
	
}




uint32_t Get_SysTick(void)
{
	return TimeTick;//1ms tick
}

	





void GPIO_B6_IRQHandler(void)
{

}




//DC Insert Detect
void GPIO_0_6_IRQHandler(void)
{
	 dc5v_online_flag=1;//DC 5V insert
}



void GPIO_A5_IRQHandler(void)
{
  #if 0//DEBUG_UATR0_PRINT_LOG
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("GPIO_A5_IRQHandler"));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));
  #endif		
}


#if 1
extern void Timer0_Interrupt_Handler(void);

void TIMER0_IRQHandler(void)
{
	Timer0_Interrupt_Handler();
	
}



void TIMER1_IRQHandler(void)
{
	//timer1 interrupt service routine
	gecko_timer1_irq_handler();
	
}
#endif




extern void UART0_Interrupt_Handler(void);

void UART0_IRQHandler(void)
{
	//the first UART interrupt service routine
	UART0_Interrupt_Handler();
	
}


extern void UART1_Interrupt_Handler(void);

void UART1_IRQHandler(void)
{
	//the second UART interrupt service routine
	UART1_Interrupt_Handler();
}









volatile unsigned char dma_int_flag=0;

__RAM_CODE__  void DMA_IRQHandler(void)
{

	dma_int_flag =1;	
	Clear_DMA_Interrupt();
	
	//DMA_ISR_Routine();
	
}


//wait DMA transfer complete
__RAM_CODE__ void dma_sram_wait(unsigned long delay)
{
    unsigned int i;
    unsigned long count;
    count = (delay<<4);
    for (i=0; i<count; i++)
    {
			  if(dma_int_flag==1)
				{
						dma_int_flag=0;
					  return;					
				}
        __nop();
			  __nop();
			  __nop();
    }
}














volatile uint8_t flag_250us=0;

//void WDT_IRQHandler(void)
void WDT_IRQHandler(void)
{
#if 0	
	if(flag_250us)
	{
		flag_250us=0;
		set_gpiob4_l();
	}
	else
	{
		flag_250us=1;
		set_gpiob4_h();
	}	
#endif


//**********************************************************	
  //watchdog_irq_clear();
	
  //watchdog_unlock();
	GECKO1108_WATCHDOG->LOCK = 0x1ACCE551;
	
  #if 0//DEBUG_UATR0_PRINT_LOG
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
	UATR0_PRINT_LOG((unsigned char *)("watchdog_irq_clear"));
	UATR0_PRINT_LOG((unsigned char *)("\r\n"));
  #endif	
	
  GECKO1108_WATCHDOG->INTCLR = Gecko_Watchdog_INTCLR_Msk;
  //watchdog_lock();
	GECKO1108_WATCHDOG->LOCK = 0;	
//*************************************************************	
}








//RC 频率的漂移可能导致同一芯片在不同温度下读出的 ID 变化超过 ±5%。此时可以：

//在生产时记录每个芯片在室温下的“黄金 ID”并写入 Flash。

//运行时读取当前 ID，与黄金 ID 比较，允许 ±5% 的误差窗口。

uint32 RC_Unique_ID=0;

void iWDT_IRQHandler(void)
{
	
//	  RC_Unique_ID=Get_SysTick_VALUE();
//	  iWDT_Timer_Disable();
	
}




#if 0

void iWDT_IRQHandler(void)
{
    // 1. 立即读取 SysTick 值
    RC_Unique_ID = SysTick->VAL;
    
    // 2. 禁用 iWDT 并清除中断标志
    iWATCHDOG->WREN = WDT200_WP_NUM;      // 解锁
    iWATCHDOG->CTRL &= ~IWDT_EN;          // 只清除使能位
    iWATCHDOG->ST = WDT_ST_INTEXPIRED_CLR; // 清中断标志
    iWATCHDOG->WREN = 0;                  // 重新锁定（可选）
}

// 初始化测量函数
uint32_t Get_RC_Unique_ID(void)
{
    // 配置 SysTick
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL  = 0;
    SysTick->CTRL = 0x5;   // 使能，使用系统时钟，无中断
    
    // 配置 iWDT：1ms 定时（假设 32kHz 时钟，32 个周期 = 1ms）
    iWATCHDOG->WREN = WDT200_WP_NUM;
    iWATCHDOG->CTRL = 0;                     // 先完全禁用
    iWATCHDOG->ST = WDT_ST_INTEXPIRED_CLR;   // 清残留标志
    iWATCHDOG->LOAD = 32;                    // 重装载值
    iWATCHDOG->CTRL = (IWDT_EN | IWDT_IRQ_EN); // 使能定时器和中断
    iWATCHDOG->WREN = 0;
    
    // 等待中断完成（RC_Unique_ID 是 volatile 全局变量）
    while (RC_Unique_ID == 0);
    
    return RC_Unique_ID;
}


//RC 频率的漂移可能导致同一芯片在不同温度下读出的 ID 变化超过 ±5%。此时可以：

//在生产时记录每个芯片在室温下的“黄金 ID”并写入 Flash。

//运行时读取当前 ID，与黄金 ID 比较，允许 ±5% 的误差窗口。

//RC_Unique_ID
#endif








//****************************************************************************
//  IRQ handler of : 1. QSPI controller; 2. AON Wakeup
//  Gofee added @ 2021.02.11
//****************************************************************************
extern void gecko_ahb2qspi_ctrl_irq_handler(void);

void QSPI_CTRL_IRQHandler(void)
{
  
  gecko_ahb2qspi_ctrl_irq_handler();
  
}




extern void gecko_aon_wakeup_irq_handler(void);

void AON_WKUP_IRQHandler(void)
{
  
  gecko_aon_wakeup_irq_handler();
  
}

void APB_SPI0_IRQnHandler(void)
{
	HW_SPI_Interrupt_Handler();
}

void APB_SPI1_IRQnHandler(void)
{
	
}


void CMU2AHB_IRQHandler(void)
{
    // TBD
}

//****************************************************************************

