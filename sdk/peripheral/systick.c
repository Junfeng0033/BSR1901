/* ##################################    SysTick function  ############################################ */
/** \ingroup  CMSIS_Core_FunctionInterface
    \defgroup CMSIS_Core_SysTickFunctions SysTick Functions
    \brief      Functions that configure the System.
  @{
**/

/** \brief  System Tick Configuration

    The function initializes the System Timer and its interrupt, and starts the System Tick Timer.
    Counter is in free running mode to generate periodic interrupts.

    \param [in]  ticks  Number of ticks between two interrupts.

    \return          0  Function succeeded.
    \return          1  Function failed.

    \note     When the variable <b>__Vendor_SysTickConfig</b> is set to 1, then the
    function <b>SysTick_Config</b> is not included. In this case, the file <b><i>device</i>.h</b>
    must contain a vendor-specific implementation of this function.

**/
 


#include "bsr1901.h"

extern void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);


#if 0
//SysTick DEF
typedef struct{
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VALUE;
    volatile uint32_t CALIB;
}SysTickType;
#endif

//#define SysTick_BASE 0xe000e010
//#define SysTick ((SysTickType *)SysTick_BASE)


void Set_SysTick_CTRL(uint32_t ctrl)
{
	SysTick->CTRL = ctrl;
}

void Set_SysTick_LOAD(uint32_t load)
{
	SysTick->LOAD = load;
}

uint32_t Read_SysTick_VALUE(void)
{
	//return(SysTick->VALUE);
	return(SysTick->VAL);
}

void Set_SysTick_CALIB(uint32_t calib)
{
	SysTick->CALIB = calib;
}

void Set_SysTick_VALUE(uint32_t value)
{
	//SysTick->VALUE = value;
		SysTick->VAL = value;
}



uint32_t get_value(void)
{
return SysTick->VAL;
}


void SysTickHandler(void)
{
    Set_SysTick_CTRL(0);
    SCB->ICSR = SCB->ICSR | (1 << 25);
}


/*

void timer_start(TIMER *timer,int interval)
{
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	timer->start = get_current_time();
	timer->interval = interval;
}

void timer_stop(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

*/


void systick_timer_stop(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}


uint8_t SysTick_Timer_Stop(uint32_t *duration_t,uint32_t start_t)
{
	uint32_t stop_t;
	stop_t = SysTick->VAL;
	if((SysTick->CTRL & 0x10000) == 0)
	{
		*duration_t = start_t - stop_t;
		return(1);
	}
	else
	{
		return(0);
	}
}

uint32_t SysTick_Timer_Init(void)
{
	SysTick->CTRL = 0;
	SysTick->LOAD = 0xffffff;
	SysTick->VAL = 0;
	SysTick->CTRL = 0x5;
	while(SysTick->VAL == 0);
	return(SysTick->VAL);
}

/*

static uint32_t cnts_us = 0;

void SysTick_Init(uint32_t SysTick_CLKSource)
{

	if (SysTick_CLKSource == SysTick_CLKSource_HCLK)
	{
	SysTick->CTRL |= SysTick_CLKSource_HCLK;
	cnts_us = SystemCoreClock/1000000;
	}
	else
	{
	SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8;
	cnts_us = SystemCoreClock/(8*1000000);
	}
}


void delay_us(uint32_t cnts)
{
	uint32_t flag = 0;
	uint32_t ticks = cnts_us*cnts;
	SysTick->LOAD = ticks - 1;
	SysTick->VAL = 0;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	do
	{
	flag = SysTick->CTRL;
	}while(!(flag&(0x01<<16)));
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0; 
}

*/


#if 0
uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk) { return (1UL); }

  SysTick->LOAD  = (uint32_t)(ticks - 1UL);
  //NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
	NVIC_SetPriority (SysTick_IRQn, 0);
  SysTick->VAL   = 0UL;
	#if 1
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
  return (0UL);                                                     /* Function successful */
	#endif
}
#endif

uint32_t g_SysTick_Counter = 0;
uint32_t g_SysInt_Counter = 0;

static unsigned int g_Next = 1;

//------------------------------------------------------------------------------
/// Initialize the seed for rand generator.
/// param seed rand initiation seed -- g_SysTick_Counter ?
//------------------------------------------------------------------------------
void sys_srand( unsigned int seed )
{
  g_Next = seed;
}

//------------------------------------------------------------------------------
/// Return a random number, maxinum assumed to be 65535
//------------------------------------------------------------------------------
int sys_rand( void )
{
  g_Next = g_Next * 1103515245 + 12345;
  return (unsigned int) ( g_Next / 131072 ) % 65536;
}


//uint32_t SysTick_Get( void )
//{
//  return g_SysTick_Counter;
//}

/*************************************
//GEK1108 8MHZ
//1/fosc =1/8000000 =125ns
//1ms = 8000 * 125ns
//10ms =80000 * 125ns
**************************************/


/*************************************
//GEK1109 20MHZ
//1/fosc =1/20_000_000 =50ns
//5us  =100 *50ns
//100us=2000*50ns
//1ms  = 20_000 * 50ns
//10ms =200_000 * 50ns
**************************************/


uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk) { return (1UL); }    /* Reload value impossible */

  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
  //NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
	NVIC_SetPriority (SysTick_IRQn, 0);
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  
	#if 1
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
  return (0UL);                                                     /* Function successful */
	#else
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   //SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
  return (0UL);                                                     /* Function successful */
	#endif
}

#if 0
static volatile uint32_t TimeTick = 0;
          
void SysTick_Handler(void)  //interrupt routine
{
    TimeTick++;
}


void delay_ms(uint32_t ms)
{
    SysTick->LOAD  =    25000*ms-1;     
    SysTick->VAL   =     0;                  
    SysTick->CTRL  |=  ((1<<1)|(1<<0));   //turn on tick timer interrupt
    while(!TimeTick);
    TimeTick = 0;
    SysTick->CTRL =0;   //turn off tick timer interrupt
}
#endif

#define 			SystemCoreClock  			8000000
#define 			SYS_DIV					 			8

void SysTick_Init(void)
{
	if(SysTick_Config(SystemCoreClock / SYS_DIV) )
	{
		while(1)
		{
			;
		}
	}
}

#if 0
uint32_t ticktime=0;
void SysTick_Time_Test(void)
{

		ticktime=SysTick->VAL;

		#if DEBUG_UATR0_PRINT_LOG
		char *string=0;		
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));			
		UATR0_PRINT_LOG((unsigned char *)("SysTick->VAL:---ticktime=0x"));
		string=my_itoa(ticktime);
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
		
    #endif		 

	
}
#endif
