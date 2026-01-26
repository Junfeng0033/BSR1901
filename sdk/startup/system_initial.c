/******************************************************************************
 * @file:    system_initial.c
 * @purpose: CMSIS Cortex-M0 Device Peripheral Access Layer Source File
 *           for the BSR Ltd. Device Series 
 * @version: V1.0
 * @date:    26. Oct. 2020
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2008 ARM Limited. All rights reserved.
 *
 * ARM Limited (ARM) is supplying this software for use with Cortex-M0 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#include <platform_config.h>
#include <bsr1901.h>
#include <hci.h>
#include <uart.h>
#include "watchdog.h"
#include "timer.h"
#include "pwm.h"

extern void charger_control_init(void);
extern void HW_lightning_Setup(void);
extern void ADC_Init(void);


uint32 bsr1901_sram_test(void)
{
    //uint32 errors = 0;
    uint32 base = GECKO_SRAM_BASE;

    uint32 size = 0x00000400;  
    //char *string;	
	
    /* Initialize SRAM */
    //sram_init( ); //sram
 
    /* Data tests */
    //memory_fill_print( base, size, 0xFFFAAAAA );

    uint32 start=base;
	  uint32 len=size;
	  uint32 val=0xFFFAAAAA;
	
    uint32 i;
    uint32 end = start + len;
    uint32 errorcount = 0;
	
	   /* Write Pattern */
    for ( i = start; i < end; i += 4 )
    {
        *( volatile uint32* )i = val; 
    }

    /* Read Pattern */
    for ( i = start; i < end; i += 4 )
    {
        if ( *( volatile uint32* )i != val )
        {
            errorcount++;
					
//						UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
//						UATR0_PRINT_LOG((unsigned char *)("bsr1901_sram_test --- errorcount= 0x"));
//						string=my_itoa(errorcount);
//						UATR0_PRINT_LOG((unsigned char *)(string));
//						UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
					
            break;        
				}
        //printf( "%x\n",*( volatile uint32* )i );
    }
		return errorcount;	
}





//extern uint32_t __boot_sram_start_flash__[];
//extern uint32_t __boot_sram_end_flash__[];
//extern uint32_t __boot_sram_start__[];
//extern uint32_t __boot_bss_sram_start__[];
//extern uint32_t __boot_bss_sram_end__[];

extern void SystemInit(void);



void BootInit(void)
{
    //uint32_t *dst, *src;
#if 0
    // Init boot sections
    for (dst = __boot_sram_start__, src = __boot_sram_start_flash__;
            src < __boot_sram_end_flash__;
            dst++, src++) {
        *dst = *src;
    }

    for (dst = __boot_bss_sram_start__; dst < __boot_bss_sram_end__; dst++) {
        *dst = 0;
    }
#endif
	
#ifdef FPGA
    hal_cmu_fpga_setup();
		hal_norflash_init();
#else
    //hal_cmu_setup();
		//hal_norflash_init();
#endif

#if 0
    for (dst = __sram_text_data_start__, src = __sram_text_data_start_flash__;
            src < __sram_text_data_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
    for (dst = __sram_bss_start__; dst < __sram_bss_end__; dst++) {
        *dst = 0;
    }
    for (dst = __fast_sram_text_data_start__, src = __fast_sram_text_data_start_flash__;
            src < __fast_sram_text_data_end_flash__;
            dst++, src++) {
        *dst = *src;
    }
#endif						

}





//void UserGpioInit(void) 
//{

//    #ifdef __SWD_DISABLED__
//    LPC_IOCON->R_PIO1_0  &= ~0x07;
//    LPC_IOCON->R_PIO1_0  |=  0x01;
//    LPC_IOCON->R_PIO1_1  &= ~0x07;
//    LPC_IOCON->R_PIO1_1  |=  0x01;
//    LPC_IOCON->R_PIO1_2  &= ~0x07;
//    LPC_IOCON->R_PIO1_2 |=  0x01;	
//    LPC_IOCON->R_PIO0_11 &= ~0x07;	   
//    LPC_IOCON->R_PIO0_11 |=  0x01;
//    #endif	
//}
  


//typedef enum
//{
//    TWS_JieLi,
//    TWS_ACTS,
//    TWS_BES,
//    TWS_BLUETRUM,
//    TWS_QTY,
//} Customer_ID;







//void hal_nvic_enable_irq(IRQn_Type irq_number)
//{
//    /* Enable interrupt */
//    NVIC_EnableIRQ(irq_number);
//}


//void hal_nvic_disable_irq(IRQn_Type irq_number)
//{
//	  /* Disable interrupt */
//		NVIC_DisableIRQ(irq_number);
//}

//#define __enable_irq                              __enable_interrupt        /*!< global Interrupt enable */
//#define __disable_irq                             __disable_interrupt       /*!< global Interrupt disable */

uint32 pEnterCriticalSection (void)
{
    __disable_irq();
    return 0;
}

void pExitCriticalSection (uint32 status)
{
    __enable_irq();
}

//extern volatile uint32* g_SysCsAddress;

// =============================================================================
// SysEnterCriticalSection
// -----------------------------------------------------------------------------
//
// =============================================================================
uint32 SysEnterCriticalSection(void)
{
    //uint32  myLocalVar = *g_SysCsAddress;	
		uint32  myLocalVar=0x0;	
    // add this empty assembly line to avoid the compiler to re-order
    // the code around the critical section call
    asm volatile ("");
    return myLocalVar;
}

// =============================================================================
// SysExitCriticalSection
// -----------------------------------------------------------------------------
//
// =============================================================================
void SysExitCriticalSection(uint32 status)
{
    // add this empty assembly line to avoid the compiler to re-order
    // the code around the critical section call
    asm volatile ("");
    //*g_SysCsAddress = status;
}





#ifdef VECT_TAB_RAM
/* Set the Vector Table base location at 0x20000000 */
NVIC_SetVectorTable(NVIC_VectTab_RAM+0x2000, 0x0);
#else /* VECT_TAB_FLASH */
/* Set the Vector Table base location at 0x08000000 */
//NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); // NVIC_VectTab_FLASH
#endif





#if 1



/* Interrupt Priorities are WORD accessible only under ARMv6M                   */
/* The following MACROS handle generation of the register offset and byte masks */
#define _BIT_SHIFT(IRQn)         (  ((((uint32_t)(int32_t)(IRQn)))               &  0x03UL) * 8UL)
#define _SHP_IDX(IRQn)           ( (((((uint32_t)(int32_t)(IRQn)) & 0x0FUL)-8UL) >>    2UL)      )
#define _IP_IDX(IRQn)            (   (((uint32_t)(int32_t)(IRQn))                >>    2UL)      )


/**
  \brief   Enable External Interrupt
  \details Enables a device-specific interrupt in the NVIC interrupt controller.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void NVIC_EnableIRQ(IRQn_Type IRQn)
{
	if ((int32_t)(IRQn) >= 0)
  {
  NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
	}
}


/**
  \brief   Disable External Interrupt
  \details Disables a device-specific interrupt in the NVIC interrupt controller.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void NVIC_DisableIRQ(IRQn_Type IRQn)
{
	if ((int32_t)(IRQn) >= 0)
  {
  NVIC->ICER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
	}
}



#if 0
//clean up interrupts

//write NVIC->ISER register corresponding bit to 1, turn on interrupt

//write NVIC->ICER register corresponding bit to 1, turn off interrupt

#endif



//test interrupt
void Disable_All_IRQ(void)
{
	NVIC->ICER[0]=0xffffffff;
}




uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((NVIC->ISPR[0U] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}




void NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    NVIC->ISPR[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}




void NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    NVIC->ICPR[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}




/**
  \brief   Set Interrupt Priority
  \details Sets the priority of an interrupt.
  \note    The priority cannot be set for every core interrupt.
  \param [in]      IRQn  Interrupt number.
  \param [in]  priority  Priority to set.
 */
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) < 0)
  {
    SCB->SHP[_SHP_IDX(IRQn)] = ((uint32_t)(SCB->SHP[_SHP_IDX(IRQn)] & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
       (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
  else
  {
    NVIC->IP[_IP_IDX(IRQn)]  = ((uint32_t)(NVIC->IP[_IP_IDX(IRQn)]  & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
       (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
}


void hal_nvic_set_priority(IRQn_Type irq_number, uint32 priority)
{
    /* Set interrupt priority */
    NVIC_SetPriority(irq_number, priority);
}



/**
  * @brief  Enable device specific interrupt
  * @param  irq_number An enumerator of IRQn_Type enumeration in device file such as gecko1108.h
  * @retval None
  */
void hal_nvic_enable_irq(IRQn_Type irq_number)
{

    /* Enable interrupt */
    NVIC_EnableIRQ(irq_number);
}


void hal_nvic_disable_irq(IRQn_Type irq_number)
{
	  /* Disable interrupt */
		NVIC_DisableIRQ(irq_number);
}


/**
  * @brief  Set interrupt pending bit
  * @param  irq_number An enumerator of IRQn_Type enumeration in device file such as gmf0313x6.h
  * @retval None
  */
void hal_nvic_set_pending_irq(IRQn_Type irq_number)
{
    /* Set interrupt pending */
    NVIC_SetPendingIRQ(irq_number);
}

/**
  * @brief  Clear interrupt pending bit
  * @param  irq_number An enumerator of IRQn_Type enumeration in device file such as gmf0313x6.h
  * @retval None
  */
void hal_nvic_clear_pending_irq(IRQn_Type irq_number)
{
    /* Clear pending interrupt */
    NVIC_ClearPendingIRQ(irq_number);
}

/**
  * @brief  Get interrupt pending bit
  * @param  irq_number An enumerator of IRQn_Type enumeration in device file such as gmf0313x6.h
  * @retval Interrupt pending bit (0/1)
  */
uint32_t hal_nvic_get_pending_irq(IRQn_Type irq_number)
{
    /* Get interrupt pending bit */
    return NVIC_GetPendingIRQ(irq_number);
}





#endif
