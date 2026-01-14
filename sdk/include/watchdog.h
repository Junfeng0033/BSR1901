

#if 0
void watchdog_init(unsigned int cycle, int type);
                                       /* Program watchdog: */
                                       /* type = 0 : No action */
                                       /* type = 1 : Interrupt */
                                       /* type = 2 : Reset */
void watchdog_set(unsigned int cycle); /* update watchdog counter */
void watchdog_unlock(void);            /* unlock watchdog */
void watchdog_lock(void);              /* lock watchdog */
void watchdog_irq_clear(void);         /* clear watchdog interrupt */


#endif





#include "platform_config.h"





#define    GECKO_APB_WATCHDOG_BASE                0x40014000
//define APB watchdog base address
#define	 	 GECKO_APB_WATCHDOG_BASE_ADDR		    		0x40014000

//#define    GECKO_iWATCHDOG_BASE                   0x40002000 //0x40022000

#define    GECKO_iWATCHDOG_BASE                   0x40022000


//output WDOGINT 			FRC(free running control) interrupt
//output WDOGRES			FRC	reset
//output [31:0]				Read data output

/*------------------- Watchdog ----------------------------------------------*/
typedef struct
{

  __IO    uint32_t  WdogLoad;               /* Offset: 0x000 (R/W) Watchdog Load Register */
  __I     uint32_t  WdogValue;//CountRead   /* Offset: 0x004 (R/ ) Watchdog Value Register */
  __IO    uint32_t  CTRL;                   /* Offset: 0x008 (R/W) Watchdog Control Register */
  __O     uint32_t  INTCLR;                 /* Offset: 0x00C ( /W) Watchdog Clear Interrupt Register */
  __I     uint32_t  RAWINTSTAT;             /* Offset: 0x010 (R/ ) Watchdog Raw Interrupt Status Register */
  __I     uint32_t  MASKINTSTAT;            /* Offset: 0x014 (R/ ) Watchdog Interrupt Status Register */
					uint32_t  RESERVED0[762];
  __IO    uint32_t  LOCK;                   /* Offset: 0xC00 (R/W) Watchdog Lock Register */
					uint32_t  RESERVED1[191];
  __IO    uint32_t  ITCR;                   /* Offset: 0xF00 (R/W) Watchdog Integration Test Control Register */
  __O     uint32_t  ITOP;                   /* Offset: 0xF04 ( /W) Watchdog Integration Test Output Set Register */
}GECKO_WATCHDOG_TypeDef;


#define GECKO1108_WATCHDOG          ((GECKO_WATCHDOG_TypeDef  *) GECKO_APB_WATCHDOG_BASE   )



#define Gecko_Watchdog_LOAD_Pos               0                                              /* Watchdog LOAD: LOAD Position */
#define Gecko_Watchdog_LOAD_Msk              (0xFFFFFFFFul << Gecko_Watchdog_LOAD_Pos)       /* Watchdog LOAD: LOAD Mask */

#define Gecko_Watchdog_VALUE_Pos              0                                              /* Watchdog VALUE: VALUE Position */
#define Gecko_Watchdog_VALUE_Msk             (0xFFFFFFFFul << Gecko_Watchdog_VALUE_Pos)      /* Watchdog VALUE: VALUE Mask */



//correct

#define Gecko_Watchdog_CTRL_RESEN_Pos         1                                              /* Watchdog CTRL_RESEN: Enable Reset Output Position */
#define Gecko_Watchdog_CTRL_RESEN_Msk        (0x1ul << Gecko_Watchdog_CTRL_RESEN_Pos)        /* Watchdog CTRL_RESEN: Enable Reset Output Mask */

#define Gecko_Watchdog_CTRL_INTEN_Pos         0                                              /* Watchdog CTRL_INTEN: Int Enable Position */
#define Gecko_Watchdog_CTRL_INTEN_Msk        (0x1ul << Gecko_Watchdog_CTRL_INTEN_Pos)        /* Watchdog CTRL_INTEN: Int Enable Mask */







#define Gecko_Watchdog_INTCLR_Pos             0                                              /* Watchdog INTCLR: Int Clear Position */
#define Gecko_Watchdog_INTCLR_Msk            (0x1ul << Gecko_Watchdog_INTCLR_Pos)            /* Watchdog INTCLR: Int Clear Mask */

#define Gecko_Watchdog_RAWINTSTAT_Pos         0                                              /* Watchdog RAWINTSTAT: Raw Int Status Position */
#define Gecko_Watchdog_RAWINTSTAT_Msk        (0x1ul << Gecko_Watchdog_RAWINTSTAT_Pos)        /* MPS2_Watchdog RAWINTSTAT: Raw Int Status Mask */

#define Gecko_Watchdog_MASKINTSTAT_Pos        0                                              /* Watchdog MASKINTSTAT: Mask Int Status Position */
#define Gecko_Watchdog_MASKINTSTAT_Msk       (0x1ul << Gecko_Watchdog_MASKINTSTAT_Pos)       /* Watchdog MASKINTSTAT: Mask Int Status Mask */

#define Gecko_Watchdog_LOCK_Pos               0                                              /* Watchdog LOCK: LOCK Position */
#define Gecko_Watchdog_LOCK_Msk              (0x1ul << Gecko_Watchdog_LOCK_Pos)              /* Watchdog LOCK: LOCK Mask */

#define Gecko_Watchdog_INTEGTESTEN_Pos        0                                              /* Watchdog INTEGTESTEN: Integration Test Enable Position */
#define Gecko_Watchdog_INTEGTESTEN_Msk       (0x1ul << Gecko_Watchdog_INTEGTESTEN_Pos)       /* Watchdog INTEGTESTEN: Integration Test Enable Mask */

#define Gecko_Watchdog_INTEGTESTOUTSET_Pos    1                                              /* Watchdog INTEGTESTOUTSET: Integration Test Output Set Position */
#define Gecko_Watchdog_INTEGTESTOUTSET_Msk   (0x1ul << Gecko_Watchdog_INTEGTESTOUTSET_Pos)   /* Watchdog INTEGTESTOUTSET: Integration Test Output Set Mask */



//#define    GECKO_iWATCHDOG_BASE                   0x40022000
#define    GECKO_iWATCHDOG_BASE                   0x40022000
typedef struct
{
  __IO    uint32_t  IDREV;               /* Offset: 0x000 (RO) ID and Revision Register */
	        uint32_t  RESERVED04;
	        uint32_t  RESERVED08;
		      uint32_t  RESERVED0C;	
  __I     uint32_t  CTRL;                /* Offset: 0x010 (R/W) Control Register */
  __IO    uint32_t  RESTART;             /* Offset: 0x014 (R/W) Restart Register */
  __O     uint32_t  WREN;                /* Offset: 0x018 ( /W) Write Enable Register */
  __I     uint32_t  ST;                  /* Offset: 0x01C (R/ ) iWatchdog Status Register */

}iWatchDog_T;


#define iWATCHDOG          ((iWatchDog_T  *) GECKO_iWATCHDOG_BASE )

//0x18 register
#define WDT200_WP_NUM 							0x5aa5
//0x14 register
#define WDT200_RESTART_NUM 					0xcafe




//0x10 control register
#define IWDT_EN        			(1<<0)
#define IWDT_CLK_SEL        (1<<1)
#define IWDT_INT_EN        	(1<<2)
#define IWDT_RST_EN         (1<<3)
#define IWDT_INT_TIME(n)    (((n)&0xF)<<4)//IntTime[7:4]////The timer interval of the interrupt stage
#define IWDT_RST_TIME(n)    (((n)&0x7)<<8)//RstTime[10:8]

#if 0
/*----- WDT Time Period -----*/
#define NDS_WDT_TIME_POW_2_6              (0x0UL)                           ///< WDT timer interval    64 clock periods.
#define NDS_WDT_TIME_POW_2_8              (0x1UL)                           ///< WDT timer interval   256 clock periods.
#define NDS_WDT_TIME_POW_2_10             (0x2UL)                           ///< WDT timer interval  1024 clock periods.
#define NDS_WDT_TIME_POW_2_11             (0x3UL)                           ///< WDT timer interval  2048 clock periods.
#define NDS_WDT_TIME_POW_2_12             (0x4UL)                           ///< WDT timer interval  4096 clock periods.
#define NDS_WDT_TIME_POW_2_13             (0x5UL)                           ///< WDT timer interval  8192 clock periods.
#define NDS_WDT_TIME_POW_2_14             (0x6UL)                           ///< WDT timer interval 16384 clock periods.
#define NDS_WDT_TIME_POW_2_15             (0x7UL)                           ///< WDT timer interval 32768 clock periods.
#endif

#if 0
/* 0x10 Control Register */

#define WDT_CTRL_RSTTIME_POW_2_7        0x000
#define WDT_CTRL_RSTTIME_POW_2_8        0x100
#define WDT_CTRL_RSTTIME_POW_2_9        0x200
#define WDT_CTRL_RSTTIME_POW_2_10       0x300
#define WDT_CTRL_RSTTIME_POW_2_11       0x400
#define WDT_CTRL_RSTTIME_POW_2_12       0x500
#define WDT_CTRL_RSTTIME_POW_2_13       0x600
#define WDT_CTRL_RSTTIME_POW_2_14       0x700

#define WDT_CTRL_INTTIME_POW_2_6        0x000
#define WDT_CTRL_INTTIME_POW_2_8        0x010
#define WDT_CTRL_INTTIME_POW_2_10       0x020
#define WDT_CTRL_INTTIME_POW_2_11       0x030
#define WDT_CTRL_INTTIME_POW_2_12       0x040
#define WDT_CTRL_INTTIME_POW_2_13       0x050
#define WDT_CTRL_INTTIME_POW_2_14       0x060
#define WDT_CTRL_INTTIME_POW_2_15       0x070

#define WDT_CTRL_INTTIME_POW_2_17       0x080
#define WDT_CTRL_INTTIME_POW_2_19       0x090
#define WDT_CTRL_INTTIME_POW_2_21       0x0A0
#define WDT_CTRL_INTTIME_POW_2_23       0x0B0
#define WDT_CTRL_INTTIME_POW_2_25       0x0C0
#define WDT_CTRL_INTTIME_POW_2_27       0x0D0
#define WDT_CTRL_INTTIME_POW_2_29       0x0E0
#define WDT_CTRL_INTTIME_POW_2_31       0x0F0

#define WDT_CTRL_RSTEN                  0x8
#define WDT_CTRL_INTEN                  0x4
#define WDT_CTRL_APBCLK                 0x2
#define WDT_CTRL_EXTCLK                 0x0
#define WDT_CTRL_EN                     0x1

/* 0x14 Restart Register */
#define WDT_RESTART_NUM                 0xcafe

/* 0x18 Write Enable Register */
#define WDT_WREN_NUM                    0x5aa5

/* 0x1C Status Register */
#define WDT_ST_INTEXPIRED               0x1
#define WDT_ST_INTEXPIRED_CLR           0x1     // w1c bit

// WDT flags
#define WDT_FLAG_INITIALIZED            (1U << 0)
#define WDT_FLAG_POWERED                (1U << 1)

#endif

//#define WDT200_32BIT_TIMER




#if 0

 /***********************************************************************
 * APB WDT module
 **********************************************************************/
#define	GECKO_APB_WDT_CON				(GECKO_APB_WATCHDOG_BASE + 0x08)
#define	GECKO_APB_WDT_VAL				(GECKO_APB_WATCHDOG_BASE + 0x0c)
#define GECKO_APB_WDT_TIMER_CON		(GECKO_APB_WATCHDOG_BASE + 0x10)
#define GECKO_APB_WDT_TIMER_MSK		(GECKO_APB_WATCHDOG_BASE + 0x18)
#define GECKO_APB_WDT_CLEAR			(GECKO_APB_WATCHDOG_BASE + 0x20)
#define GECKO_APB_WDT_CLEAR_VAL		0x4
#define GECKO_APB_WDT_RELOAD_VAL		0x10000



/*****************************************************************************
* WDT module
*****************************************************************************/
#define GECKO_APB_WATCHDOG_CR                                (GECKO_APB_WATCHDOG_BASE) 
#define GECKO_APB_WATCHDOG_TORR                            (GECKO_APB_WATCHDOG_BASE+0x04) 
#define GECKO_APB_WATCHDOG_CCVR                            (GECKO_APB_WATCHDOG_BASE+0x08) 
#define GECKO_APB_WATCHDOG_CRR                              (GECKO_APB_WATCHDOG_BASE+0x0c) 
#define GECKO_APB_WATCHDOG_STAT                            (GECKO_APB_WATCHDOG_BASE+0x10) 
#define GECKO_APB_WATCHDOG_EOI                              (GECKO_APB_WATCHDOG_BASE+0x14) 




#define GECKO_SCU_PCLKWDTEN_BIT                   11
#define GECKO_SCU_WDTCLKEN_BIT                    28



//error address for gecko1108
#define GECKO_APB_SCU_BASE 		    0x1a0c000
#define GECKO_APB_SCU_CLKGATE_REG                 (GECKO_APB_SCU_BASE + 0X00)




#define GECKO_WDT_RESET_PW                        7
#define GECKO_WDT_ENABLE                          1
#define GECKO_WDT_RESTART_VAL                     0x76
//0.5s watch dog
#define GECKO_WDT_INIT_VAL                        0x20000

#endif








/*****************************************************************************************************
*
*
* wdt controller 
* reg address 
**
*
*
*
******************************************************************************************************/

//#define WDT_RATE            32000

/* watchdog register offsets and masks */
//#define WDTLOAD_REG_OFFSET         0x000
//#define WDTLOAD_LOAD_MIN    0x00000001
//#define WDTLOAD_LOAD_MAX    0xFFFFFFFF

//#define WDTVALUE_REG_OFFSET        0x004

//#define WDTCONTROL_REG_OFFSET      0x008


//#define WDTCONTROL_REG_INT_ENABLE  (1 << 0)
//#define WDTCONTROL_REG_RESET_ENABLE    (1 << 1)

//#define WDTINTCLR_REG_OFFSET       0x00C

//#define WDTRIS_REG_OFFSET          0x010


//#define WDTRIS_REG_INT_MASK    (1 << 0)

//#define WDTMIS_REG_OFFSET          0x014
//#define WDTMIS_REG_INT_MASK    (1 << 0)

//#define WDTLOCK_REG_OFFSET         0xC00
//#define WDTLOCK_REG_UNLOCK      0x1ACCE551
//#define WDTLOCK_REG_LOCK        0x00000001





///* read write */
//#define wdtip_write32(v,b,a) \
//    (*((volatile unsigned int *)(b+a)) = v)
//			
//		
//#define wdtip_read32(b,a) \
//    (*((volatile unsigned int *)(b+a)))


		
//enum HAL_WDT_ID_T {
//    HAL_WDT_ID_0 = 0,
//    HAL_WDT_ID_NUM,
//};
//		
//		
//		

//struct HAL_WDT_CTX {
//    unsigned int load_val;
//    unsigned int timeout;
//};




void watchdog_init(unsigned int cycle, int type);
void watchdog_set(unsigned int cycle);
void feed_dog(unsigned int cycle);

void iWDT_REG_RW(void);
void iWDT_Timer_Init(void);
void iWDT_Timer_Disable(void);
void iWDT_Timer_Restart(void);

