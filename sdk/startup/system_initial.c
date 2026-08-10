/******************************************************************************
 * @file:    system_initial.c
 * @purpose: CMSIS Cortex-M0 Device Peripheral Access Layer Source File
 *           for the BSR Ltd. Device Series 
 * @version: V1.0
 * @date:    26. Oct. 2020
 ******************************************************************************/


#include <platform_config.h>
#include <bsr1901.h>
#include <hci.h>
#include <uart.h>
#include "watchdog.h"
#include "timer.h"
#include "pwm.h"

extern void SystemInit(void);







uint32 bsr1901_sram_test(void)
{
    //uint32 errors = 0;
    uint32 base = GECKO_SRAM_BASE;

    uint32 size = 0x00000400;  
    //char *string;	
	
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




/** \brief  System Reset

    The function initiates a system reset request to reset the MCU.
 */
void NVIC_SystemReset(void)
{
  __DSB();                                                     /* Ensure all outstanding memory accesses included
                                                                  buffered write are completed before reset */
  SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos)      |
                 SCB_AIRCR_SYSRESETREQ_Msk);
  __DSB();                                                     /* Ensure completion of memory access */
  while(1);                                                    /* wait until reset */
}




#if 0
void NVIC_SystemReset(void)
{
    __asm("DSB");
	
		SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);	

    __asm("DSB");	
	
//	  for(;;)
//		{
//			//__NOP();
//			__asm("nop");
//		}
			
	
}
#endif



void __set_FAULTMASK(uint32_t faultMask)
{
  //register uint32_t __regFaultMask;
	
	//__asm("faultmask");
	
	__disable_irq();
	
  //__regFaultMask = (faultMask & (uint32_t)1U);
}




void soft_reset(void)
{

    //__set_FAULTMASK(1); 

    NVIC_SystemReset(); 
}

