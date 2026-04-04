
#include "platform_config.h"

//define APB base address
//APB Address
//#define GECKO_PWM_BASE                0x40013000

//define AHB base address
//AHB Address
#define GECKO_PWM_BASE								0x40005000


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

 


//#define PWM_SLOW_CLOCK                  32000
//#define PWM_FAST_CLOCK                  16000000
#define PWM_MAX_VALUE                   0xFFFF




#define PWM_LOAD01_0(n)             (((n) & 0xFFFF) << 0)
#define PWM_LOAD01_0_MASK           (0xFFFF << 0)
#define PWM_LOAD01_0_SHIFT          (0)
#define PWM_LOAD01_1(n)             (((n) & 0xFFFF) << 16)
#define PWM_LOAD01_1_MASK           ((uint32) 0xFFFF << 16)
#define PWM_LOAD01_1_SHIFT          (16)

#define PWM_LOAD23_2(n)             (((n) & 0xFFFF) << 0)
#define PWM_LOAD23_2_MASK           (0xFFFF << 0)
#define PWM_LOAD23_2_SHIFT          (0)
#define PWM_LOAD23_3(n)             (((n) & 0xFFFF) << 16)
#define PWM_LOAD23_3_MASK           ((uint32)0xFFFF << 16)
#define PWM_LOAD23_3_SHIFT          (16)

//by JF
#define PWM_LOAD45_4(n)             (((n) & 0xFFFF) << 0)
#define PWM_LOAD45_4_MASK           (0xFFFF << 0)
#define PWM_LOAD45_4_SHIFT          (0)
#define PWM_LOAD45_5(n)             (((n) & 0xFFFF) << 16)
#define PWM_LOAD45_5_MASK           ((uint32)0xFFFF << 16)
#define PWM_LOAD45_5_SHIFT          (16)

//by JF ---2022-09-02
#define PWM_LOAD67_6(n)             (((n) & 0xFFFF) << 0)
#define PWM_LOAD67_6_MASK           (0xFFFF << 0)
#define PWM_LOAD67_6_SHIFT          (0)
#define PWM_LOAD67_7(n)             (((n) & 0xFFFF) << 16)
#define PWM_LOAD67_7_MASK           ((uint32)0xFFFF << 16)
#define PWM_LOAD67_7_SHIFT          (16)


#define PWM_TOGGLE01_0(n)           (((n) & 0xFFFF) << 0)
#define PWM_TOGGLE01_0_MASK         (0xFFFF << 0)
#define PWM_TOGGLE01_0_SHIFT        (0)
#define PWM_TOGGLE01_1(n)           (((n) & 0xFFFF) << 16)
#define PWM_TOGGLE01_1_MASK         ((uint32)0xFFFF << 16)
#define PWM_TOGGLE01_1_SHIFT        (16)

#define PWM_TOGGLE23_2(n)           (((n) & 0xFFFF) << 0)
#define PWM_TOGGLE23_2_MASK         (0xFFFF << 0)
#define PWM_TOGGLE23_2_SHIFT        (0)
#define PWM_TOGGLE23_3(n)           (((n) & 0xFFFF) << 16)
#define PWM_TOGGLE23_3_MASK         ((uint32)0xFFFF << 16)
#define PWM_TOGGLE23_3_SHIFT        (16)

#define PWM_TOGGLE45_4(n)           (((n) & 0xFFFF) << 0)
#define PWM_TOGGLE45_4_MASK         (0xFFFF << 0)
#define PWM_TOGGLE45_4_SHIFT        (0)
#define PWM_TOGGLE45_5(n)           (((n) & 0xFFFF) << 16)
#define PWM_TOGGLE45_5_MASK         ((uint32)0xFFFF << 16)
#define PWM_TOGGLE45_5_SHIFT        (16)

//by JF ---2022-09-02
#define PWM_TOGGLE67_6(n)           (((n) & 0xFFFF) << 0)
#define PWM_TOGGLE67_6_MASK         (0xFFFF << 0)
#define PWM_TOGGLE67_6_SHIFT        (0)
#define PWM_TOGGLE67_7(n)           (((n) & 0xFFFF) << 16)
#define PWM_TOGGLE67_7_MASK         ((uint32)0xFFFF << 16)
#define PWM_TOGGLE67_7_SHIFT        (16)


#define PWM_PHASE01_0(n)            (((n) & 0xFFFF) << 0)
#define PWM_PHASE01_0_MASK          (0xFFFF << 0)
#define PWM_PHASE01_0_SHIFT         (0)
#define PWM_PHASE01_1(n)            (((n) & 0xFFFF) << 16)
#define PWM_PHASE01_1_MASK          (0xFFFF << 16)
#define PWM_PHASE01_1_SHIFT         (16)

#define PWM_PHASE23_2(n)            (((n) & 0xFFFF) << 0)
#define PWM_PHASE23_2_MASK          (0xFFFF << 0)
#define PWM_PHASE23_2_SHIFT         (0)
#define PWM_PHASE23_3(n)            (((n) & 0xFFFF) << 16)
#define PWM_PHASE23_3_MASK          (0xFFFF << 16)
#define PWM_PHASE23_3_SHIFT         (16)


#define PWM_PHASE45_4(n)            (((n) & 0xFFFF) << 0)
#define PWM_PHASE45_4_MASK          (0xFFFF << 0)
#define PWM_PHASE45_4_SHIFT         (0)
#define PWM_PHASE45_5(n)            (((n) & 0xFFFF) << 16)
#define PWM_PHASE45_5_MASK          (0xFFFF << 16)
#define PWM_PHASE45_5_SHIFT         (16)

//by JF ---2022-09-02
#define PWM_PHASE67_6(n)            (((n) & 0xFFFF) << 0)
#define PWM_PHASE67_6_MASK          (0xFFFF << 0)
#define PWM_PHASE67_6_SHIFT         (0)
#define PWM_PHASE67_7(n)            (((n) & 0xFFFF) << 16)
#define PWM_PHASE67_7_MASK          (0xFFFF << 16)
#define PWM_PHASE67_7_SHIFT         (16)


//register 0x28
#define PWM0_Td_Cycle(n)					(((n)&0xFF)<<0)
#define PWM1_Td_Cycle(n)          (((n)&0xFF)<<8)
#define PWM2_Td_Cycle(n)					(((n)&0xFF)<<16)
#define PWM3_Td_Cycle(n)          (((n)&0xFF)<<24)


//register 0x2c
#define PWM4_Td_Cycle(n)					(((n)&0xFF)<<0)
#define PWM5_Td_Cycle(n)          (((n)&0xFF)<<8)
//by JF ---2022-09-02
#define PWM6_Td_Cycle(n)					(((n)&0xFF)<<16)
#define PWM7_Td_Cycle(n)          (((n)&0xFF)<<24)


//register 0x40
#define FREQ_AVE_DONE          		(1<<1)//bit1

//register 0x4C
//#define FREQ_AVE_NUM(n)           (((n)&0xFF)<<16) 
#define FREQ_AVE_EN               ((uint32)1<<24)//bit24



//register 0x50





uint8 gecko_pwm_init(void);


int gecko_pwm_start(void);
int gecko_pwm_stop(void);

void hw_pwm_enable(HW_PWM_CHAN_T channel);
void hw_pwm_disable(HW_PWM_CHAN_T channel);
void hw_pwm_invert(HW_PWM_CHAN_T channel);


void SixCh_PWM5_Config(void);

void PWM2_PWM7_Enable(void);
//void CST6118_Motor_PWM_Control(int duty2,int duty7);
void CST6118_Motor_PWM_Control(int freq,int duty2,int duty7);


void CST6118_Motor_PWM2_Control_Optimized(int freq,int duty);
void CST6118_Motor_PWM7_Control_Optimized(int freq,int duty);


void Set_PWM_CH3_Duty(uint8 duty_count);
	

