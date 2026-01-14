
/*********************************************************************************************
 *   $Id:: watchdog.c 2020-07-15 
 *   Project: Gecko watchdog driver
 *	 Author: Moana
 *   Description: 
 *	 1,AWT --- Auto Wakeup Timer
 *   2,IWDG --- independent watchdog, VDD comes from VBAT, CLOCK comes from always-on clock
 *   3,WWDG --- window watchdog
*********************************************************************************************/
#include "bsr1901.h"
#include "watchdog.h"
#include "uart.h"

/****************************************************************************/
/* Program watchdog: */
/* type = 0 : No action */
/* type = 1 : Interrupt */
/* type = 2 : Reset */
/****************************************************************************/
void watchdog_init(unsigned int cycle, int type);


/****************************************************************************/
/* update watchdog counter */
/****************************************************************************/
void watchdog_set(unsigned int cycle);


/****************************************************************************/
/* unlock watchdog */
/****************************************************************************/
void watchdog_unlock(void);

/****************************************************************************/
/* lock watchdog */
/****************************************************************************/
void watchdog_lock(void);

/****************************************************************************/
/* clear watchdog interrupt */
/****************************************************************************/
void watchdog_irq_clear(void);         


/* ----------------------------------------------------------------- */
/* Unlock watchdog access */
void watchdog_unlock(void)
{
  GECKO1108_WATCHDOG->LOCK = 0x1ACCE551;
}
/* ----------------------------------------------------------------- */
/* Lock watchdog access */
void watchdog_lock(void)
{
  GECKO1108_WATCHDOG->LOCK = 0;
}


/* ----------------------------------------------------------------- */
/* Watchdog initialization */
/* type = 0 : No action */
/* type = 1 : Interrupt */
/* type = 2 : Reset */
/* ----------------------------------------------------------------- */
void watchdog_init(unsigned int cycle, int type)
{
	volatile int TmpData;
  watchdog_unlock();
  GECKO1108_WATCHDOG->WdogLoad = cycle;
	TmpData=GECKO1108_WATCHDOG->WdogLoad;
  if (type==0) 
	{
		//printf("Set to no action");
    GECKO1108_WATCHDOG->CTRL = 0;
  } 
	else if (type==1) 
	{
		//printf("Set to NMI generation");
    GECKO1108_WATCHDOG->CTRL = Gecko_Watchdog_CTRL_INTEN_Msk;

		hal_nvic_clear_pending_irq(WDT_IRQn);
		hal_nvic_set_priority(WDT_IRQn, 0);
		hal_nvic_enable_irq(WDT_IRQn);			
		
  } 
	else 
	{
		//printf("Set to reset generation");
    GECKO1108_WATCHDOG->CTRL = Gecko_Watchdog_CTRL_RESEN_Msk|Gecko_Watchdog_CTRL_INTEN_Msk;
  }
  watchdog_lock();
}



/*
//iWDT register can not be access this way, it protected by WDT200_WP_NUM,so "iWDT_REG_RW" should not be called here.otherwise "IWDT_INT_TIME" cann't be accessed.
void iWDT_REG_RW(void)
{
	volatile int Val;
	Val=iWATCHDOG->IDREV;
	Val=iWATCHDOG->CTRL;
	Val=iWATCHDOG->RESTART;
	Val=iWATCHDOG->WREN;
	Val=iWATCHDOG->ST;	
	
	iWATCHDOG->CTRL=0;
	iWATCHDOG->CTRL=0xFFFF;	
	
	iWATCHDOG->RESTART=0;
	iWATCHDOG->RESTART=0xFFFF;		

	iWATCHDOG->WREN=0;
	iWATCHDOG->WREN=0xFFFF;
	
	iWATCHDOG->ST=0;
	iWATCHDOG->ST=0xFFFF;	
}
*/


//void iWDT_Timer_Init(unsigned int cycle, int type)
void iWDT_Timer_Init(void)
{
	volatile int ID,Val;
  ID=iWATCHDOG->IDREV;
	
	iWATCHDOG->WREN=WDT200_WP_NUM;//Write the magic number,0x5aa5

	Val|=IWDT_INT_TIME(0);//The timer interval of the interrupt stage
	Val|=IWDT_RST_TIME(2);//The time interval of the reset stage
	
	Val|=IWDT_CLK_SEL;//Clock source of timer:[0:EXTCLK;1:PCLK]
	Val|=IWDT_INT_EN;
	Val|=IWDT_RST_EN;
	Val|=IWDT_EN;	

	iWATCHDOG->CTRL=Val;	
	
}


void iWDT_Timer_Restart(void)
{
		iWATCHDOG->WREN=WDT200_WP_NUM;//Write the magic number,0x5aa5
		iWATCHDOG->RESTART=WDT200_RESTART_NUM;//Write the magic number,0xcafe	
}


void iWDT_Timer_Disable(void)
{
	iWATCHDOG->WREN=WDT200_WP_NUM;//Write the magic number,0x5aa5	
	iWATCHDOG->CTRL=(~IWDT_EN);//En=0	
}



/* ----------------------------------------------------------------- */
/* Update watchdog counter */
void watchdog_set(unsigned int cycle)
{
  watchdog_unlock();
#ifdef _FPGA_

#else
  GECKO1108_WATCHDOG->WdogLoad = cycle;
#endif
  watchdog_lock();
}






//feed watchdog
void feed_dog(unsigned int cycle)
{
	unsigned int read_data;
	
#if 0	
  uint8 i = 0;
  while (i<10) {
    read_data = GECKO1108_WATCHDOG->WdogValue;//CountRead, current value
    if (read_data < 1600) //the threshold must be large enough so that the NMI interrupt
                           //won't be triggered before the watchdog value is set again
    {
      watchdog_set(2000);
      i++;
      //printf("  Watchdog value set again");
    }
  }
	
#endif
	
	
	read_data = GECKO1108_WATCHDOG->WdogValue;//CountRead, current value
	
	if (read_data<100)
  watchdog_set(cycle);
	
	
	
}



/* ----------------------------------------------------------------- */
/* Clear watchdog interrupt request */
void watchdog_irq_clear(void)
{
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
}




/* Software variables for testing */
volatile int nmi_occurred;
volatile int nmi_expected;
volatile unsigned char reset_test;  /* set to 1 during watchdog reset test so that NMI
                             handler will not clear the watchdog */
volatile int integration_test=0;  /* set to 1 during watchdog integration test so that NMI
                             handler will clear integration test output */



#if 0

/* ----------------------------------------------------------------- */
void NMI_Handler(void)
{
  //puts ("NMI Handler Entered! \n");
  if (reset_test==1){  
		/* When testing watchdog reset, need to stay in NMI handler until the watchdog overflows again */
    while (1) {
     // wait for reset...
    }
  }
	
  watchdog_irq_clear(); /* Deassert Watchdog interrupt */
	
  nmi_occurred++; /* Update software flag */
  if (nmi_expected==0) { /* error check */
    //puts ("ERROR : NMI occurred unexpectedly\n");
  }
  if (integration_test!=0) {
    watchdog_unlock();
    GECKO1108_WATCHDOG->ITOP = 0;  // Set NMI output to 0
  }
}


#endif



#if 0
struct HAL_WDT_CTX hal_wdt_ctx[HAL_WDT_ID_NUM];


int hal_wdt_set_timeout(enum HAL_WDT_ID_T id,  unsigned int timeout)
{
    unsigned long long load;
		struct HAL_WDT_CTX *wdt = &hal_wdt_ctx[id];

    /*
     * sp805 runs counter with given value twice, after the end of first
     * counter it gives an interrupt and then starts counter again. If
     * interrupt already occurred then it resets the system. This is why
     * load is half of what should be required.
     */
    load = (WDT_RATE/2) * timeout - 1;

    load = (load > WDTLOAD_LOAD_MAX) ? WDTLOAD_LOAD_MAX: load;
    load = (load < WDTLOAD_LOAD_MIN) ? WDTLOAD_LOAD_MIN: load;

    wdt->load_val = load;
    /* roundup timeout to closest positive integer value */
    wdt->timeout = ((load + 1) * 2 + (WDT_RATE / 2))/WDT_RATE;

    return 0;
}

#endif


#if 0


/* This routine finds load value that will reset system in required timout */
static int wdt_setload(struct watchdog_device *wdd, unsigned int timeout)
{
	struct sp805_wdt *wdt = watchdog_get_drvdata(wdd);
	u64 load, rate;

	rate = wdt->rate;

	/*
	 * sp805 runs counter with given value twice, after the end of first
	 * counter it gives an interrupt and then starts counter again. If
	 * interrupt already occurred then it resets the system. This is why
	 * load is half of what should be required.
	 */
	load = div_u64(rate, 2) * timeout - 1;

	load = (load > LOAD_MAX) ? LOAD_MAX : load;
	load = (load < LOAD_MIN) ? LOAD_MIN : load;

	spin_lock(&wdt->lock);
	wdt->load_val = load;
	/* roundup timeout to closest positive integer value */
	wdd->timeout = div_u64((load + 1) * 2 + (rate / 2), rate);
	spin_unlock(&wdt->lock);

	return 0;
}



#endif
