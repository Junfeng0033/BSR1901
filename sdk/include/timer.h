/****************************************************************************
 *   $Id:: timer.c  2020-07-30 02:25:13
 *   Project:Gecko Timer example
 *
 *   Description:
****************************************************************************/
#include "platform_config.h"


//`define ADDR_BASE_APB_TIMER0    32'h4001_1000
//`define ADDR_BASE_APB_TIMER1    32'h4001_2000


#define 	 GECKO_APB_TIMER_BASE			0X40011000
//define APB timer base address
#define    GECKO_APB_TIMER0_BASE    0X40011000
#define    GECKO_APB_TIMER1_BASE    0X40012000


#define REG_TIMER0_BASE             0x40011000
#define REG_TIMER1_BASE             0x40012000

#define SYS_HW_TIMER_NUMS            2
#define GECKO_IRQ_TIMER_INDEX        13



enum{
	TIMER_INDEX0=0x0,
	TIMER_INDEX1,
};




/***********************************************************************
 * TIMER module
 **********************************************************************/
#define Gecko_Timer_LoadCount_REG(num)        (GECKO_APB_TIMER_BASE + 0x1000 * num)
#define Gecko_Timer_CurrVal_REG(num)          (GECKO_APB_TIMER_BASE + 0x1000 * num + 0x04)
#define Gecko_Timer_Control_REG(num)          (GECKO_APB_TIMER_BASE + 0x1000 * num + 0x08)
#define Gecko_Timer_EOI_REG(num)              (GECKO_APB_TIMER_BASE + 0x1000 * num + 0x0C)
#define Gecko_Timer_INTSTATUS_REG(num)        (GECKO_APB_TIMER_BASE + 0x1000 * num + 0x10)

#define Gecko_All_Timer_INTSTATUS_REG							(GECKO_APB_TIMER_BASE + 0xA0)
#define Gecko_All_Timer_EOI_REG										(GECKO_APB_TIMER_BASE + 0xA4)
#define Gecko_All_Timer_RAWINTSTATUS_REG					(GECKO_APB_TIMER_BASE + 0xA8)
#define Gecko_All_Timer_COMPVER_REG								(GECKO_APB_TIMER_BASE + 0xAC)

#define Gecko_Timer_Control_EN_BIT							0
#define Gecko_Timer_Control_MODE_BIT						1
#define Gecko_Timer_Control_MASK_BIT						2

#define Gecko_Timer0_Int_Index					0
#define Gecko_Timer0_Int_MASK					0x01
#define Gecko_Timer1_Int_Index					1
#define Gecko_Timer1_Int_MASK					0x02
#define Gecko_Timer2_Int_Index					2
#define Gecko_Timer2_Int_MASK					0x04
#define Gecko_Timer3_Int_Index					3
#define Gecko_Timer3_Int_MASK					0x08
#define Gecko_Timer4_Int_Index					4
#define Gecko_Timer4_Int_MASK					0x10
#define Gecko_Timer5_Int_Index					5
#define Gecko_Timer5_Int_MASK					0x20
#define Gecko_Timer6_Int_Index					6
#define Gecko_Timer6_Int_MASK					0x40
#define Gecko_Timer7_Int_Index					7
#define Gecko_Timer7_Int_MASK					0x80

#define GECKO_BIT(num)											(1u << num)


/***********************************************************************
 * BIT Define
 **********************************************************************/
#define     GECKO_REG_BIT00			0
#define     GECKO_REG_BIT01			1
#define     GECKO_REG_BIT02			2
#define     GECKO_REG_BIT03			3
#define     GECKO_REG_BIT04			4
#define     GECKO_REG_BIT05			5
#define     GECKO_REG_BIT06			6
#define     GECKO_REG_BIT07			7
#define     GECKO_REG_BIT08			8
#define     GECKO_REG_BIT09			9
#define     GECKO_REG_BIT10			10
#define     GECKO_REG_BIT11			11
#define     GECKO_REG_BIT12			12
#define     GECKO_REG_BIT13			13
#define     GECKO_REG_BIT14			14
#define     GECKO_REG_BIT15			15
#define     GECKO_REG_BIT16			16
#define     GECKO_REG_BIT17			17
#define     GECKO_REG_BIT18			18
#define     GECKO_REG_BIT19			19
#define     GECKO_REG_BIT20			20
#define     GECKO_REG_BIT21			21
#define     GECKO_REG_BIT22			22
#define     GECKO_REG_BIT23			23
#define     GECKO_REG_BIT24			24
#define     GECKO_REG_BIT25			25
#define     GECKO_REG_BIT26			26
#define     GECKO_REG_BIT27			27
#define     GECKO_REG_BIT28			28
#define     GECKO_REG_BIT29			29
#define     GECKO_REG_BIT30			30
#define     GECKO_REG_BIT31			31







#if 0//BUILD_MEMORY_MAP == 0
/* build for FPGA, clk is 40M */
#define  SYS_TIMER_DELAY_US						40
#define  SYS_TIMER_DELAY_10US					400
#define  SYS_TIMER_DELAY_100US					4000
#define  SYS_TIMER_DELAY_MS						40000
#define  SYS_TIMER_DELAY_10MS					400000
#define  SYS_TIMER_DELAY_100MS					4000000
#else
/* build for ASIC, clk is 20M */
#define  SYS_TIMER_DELAY_US						20
#define  SYS_TIMER_DELAY_10US					200
#define  SYS_TIMER_DELAY_100US					2000
#define  SYS_TIMER_DELAY_MS						20000
#define  SYS_TIMER_DELAY_10MS					200000
#define  SYS_TIMER_DELAY_100MS					2000000
#endif




// ============================================================================
//
//
//
// =============================================================================

#if 0

typedef volatile struct
{
    REG32                          OSTimer_Ctrl;                 //0x00000000
    REG32                          OSTimer_CurVal;               //0x00000004
    REG32                          WDTimer_Ctrl;                 //0x00000008
    REG32                          WDTimer_LoadVal;              //0x0000000C
    REG32                          HWTimer_Ctrl;                 //0x00000010
    REG32                          HWTimer_CurVal;               //0x00000014
    REG32                          Timer_Irq_Mask_Set;           //0x00000018
    REG32                          Timer_Irq_Mask_Clr;           //0x0000001C
    REG32                          Timer_Irq_Clr;                //0x00000020
    REG32                          Timer_Irq_Cause;              //0x00000024
} HW_TIMER_T;

#define hwp_timer                   ((HW_TIMER_T*) REG_TIMER0_BASE)
#define hwp_timer1                   ((HW_TIMER_T*) REG_TIMER1_BASE)

// Array of pointers towards the UART HW modules
HW_TIMER_T* g_halTimerHwpArray[2] = {hwp_timer, hwp_timer1};

#endif


//OSTimer_Ctrl
#define TIMER_LOADVAL(n)            (((n)&0xFFFFFF)<<0)
#define TIMER_LOADVAL_MASK          (0xFFFFFF<<0)
#define TIMER_LOADVAL_SHIFT         (0)
#define TIMER_ENABLE                (1<<24)
#define TIMER_ENABLED               (1<<25)
#define TIMER_CLEARED               (1<<26)
#define TIMER_REPEAT                (1<<28)
#define TIMER_WRAP                  (1<<29)
#define TIMER_LOAD                  (1<<30)

//OSTimer_CurVal
#define TIMER_CURVAL(n)             (((n)&0xFFFFFFFF)<<0)

//WDTimer_Ctrl
#define TIMER_START                 (1<<0)
#define TIMER_WDENABLED             (1<<3)
#define TIMER_STOP                  (1<<4)
#define TIMER_RELOAD                (1<<16)

//WDTimer_LoadVal
//#define TIMER_LOADVAL(n)          (((n)&0xFFFFFF)<<0)
//#define TIMER_LOADVAL_MASK        (0xFFFFFF<<0)
//#define TIMER_LOADVAL_SHIFT       (0)

//HWTimer_Ctrl
#define TIMER_INTERVAL_EN           (1<<8)
#define TIMER_INTERVAL(n)           (((n)&3)<<0)

//HWTimer_CurVal
//#define TIMER_CURVAL(n)           (((n)&0xFFFFFFFF)<<0)

//Timer_Irq_Mask_Set
#define TIMER_OSTIMER_MASK          (1<<0)
#define TIMER_HWTIMER_WRAP_MASK     (1<<1)
#define TIMER_HWTIMER_ITV_MASK      (1<<2)

//Timer_Irq_Mask_Clr
//#define TIMER_OSTIMER_MASK        (1<<0)
//#define TIMER_HWTIMER_WRAP_MASK   (1<<1)
//#define TIMER_HWTIMER_ITV_MASK    (1<<2)

//Timer_Irq_Clr
#define TIMER_OSTIMER_CLR           (1<<0)
#define TIMER_HWTIMER_WRAP_CLR      (1<<1)
#define TIMER_HWTIMER_ITV_CLR       (1<<2)

//Timer_Irq_Cause
#define TIMER_OSTIMER_CAUSE         (1<<0)
#define TIMER_HWTIMER_WRAP_CAUSE    (1<<1)
#define TIMER_HWTIMER_ITV_CAUSE     (1<<2)
#define TIMER_OSTIMER_STATUS        (1<<16)
#define TIMER_HWTIMER_WRAP_STATUS   (1<<17)
#define TIMER_HWTIMER_ITV_STATUS    (1<<18)
#define TIMER_OTHER_TIMS_IRQ(n)     (((n)&3)<<1)
#define TIMER_OTHER_TIMS_IRQ_MASK   (3<<1)
#define TIMER_OTHER_TIMS_IRQ_SHIFT  (1)


enum{
	Timer0=0x0,
	Timer1,
};


//10ms timer
#define   TIMER0_CNT_100MS    											2000
#define   TIMER1_CNT_100MS    											2000


#define TIMER_CTRL_EN                   (1 << 7)
#define TIMER_CTRL_MODE_PERIODIC        (1 << 6)
#define TIMER_CTRL_INTEN                (1 << 5)
#define TIMER_CTRL_PRESCALE_DIV_1       (0 << 2)
#define TIMER_CTRL_PRESCALE_DIV_16      (1 << 2)
#define TIMER_CTRL_PRESCALE_DIV_256     (2 << 2)
#define TIMER_CTRL_PRESCALE_MASK        (3 << 2)
#define TIMER_CTRL_SIZE_32_BIT          (1 << 1)
#define TIMER_CTRL_ONESHOT              (1 << 0)
#define TIMER_ELAPSED_CTRL_EN           (1 << 0)
#define TIMER_ELAPSED_CTRL_CLR          (1 << 1)



void Gecko_Timer_Init(void);
void Gecko_Timer_Setting(uint8 TIM_IDX);

