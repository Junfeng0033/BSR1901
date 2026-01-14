#include "platform_config.h"


#define GPIO_GROUP0_BASE   	0x40018000
#define GPIO_GROUP1_BASE    0x40019000


typedef enum {
    GPIO_GROUP_0 = 0x0,
    GPIO_GROUP_1 = 0x1,
    GPIO_GROUP_QTY
} GPIO_GROUP_ID_T;   


#define GPIOA								GPIO_GROUP_0
#define GPIOB								GPIO_GROUP_1


/* GPIO bits defines */

#define   GPIO_A0      		BIT(0)
#define   GPIO_A1      		BIT(1)
#define   GPIO_A2      		BIT(2)
#define   GPIO_A3      		BIT(3)
#define   GPIO_A4      		BIT(4)
#define   GPIO_A5      		BIT(5)
#define   GPIO_A6      		BIT(6)
#define   GPIO_A7      		BIT(7)

#define   GPIO_B0      		BIT(0)
#define   GPIO_B1      		BIT(1)
#define   GPIO_B2      		BIT(2)
#define   GPIO_B3      		BIT(3)
#define   GPIO_B4      		BIT(4)
#define   GPIO_B5      		BIT(5)
#define   GPIO_B6      		BIT(6)
#define   GPIO_B7      		BIT(7)



#if 1

#define PL061_GPIODIR_0 	0x400
#define PL061_GPIOIS_0  	0x404
#define PL061_GPIOIBE_0 	0x408
#define PL061_GPIOIEV_0 	0x40C
#define PL061_GPIOIE_0  	0x410
#define PL061_GPIORIS_0 	0x414
#define PL061_GPIOMIS_0 	0x418
#define PL061_GPIOIC_0  	0x41C

#define GPIODATA_0        	(*((volatile unsigned long *)(GPIO_GROUP0_BASE+0x0)))

#define GPIODIR_0         	(*((volatile unsigned long *)(GPIO_GROUP0_BASE+0x400)))
#define GPIOIS_0          	(*((volatile unsigned long *)(GPIO_GROUP0_BASE+0x404)))
#define GPIOIBE_0         	(*((volatile unsigned long *)(GPIO_GROUP0_BASE+0x408)))
#define GPIOIEV_0         	(*((volatile unsigned long *)(GPIO_GROUP0_BASE+0x40C)))
#define GPIOIE_0          	(*((volatile unsigned long *)(GPIO_GROUP0_BASE+0x410)))
#define GPIORIS_0         	(*((volatile unsigned long *)(GPIO_GROUP0_BASE+0x414)))
#define GPIOMIS_0         	(*((volatile unsigned long *)(GPIO_GROUP0_BASE+0x418)))
#define GPIOIC_0          	(*((volatile unsigned long *)(GPIO_GROUP0_BASE+0x41C)))

//init the second group gpio

#define PL061_GPIODIR_1 	0x400
#define PL061_GPIOIS_1  	0x404
#define PL061_GPIOIBE_1 	0x408
#define PL061_GPIOIEV_1 	0x40C
#define PL061_GPIOIE_1  	0x410
#define PL061_GPIORIS_1 	0x414
#define PL061_GPIOMIS_1 	0x418
#define PL061_GPIOIC_1  	0x41C

#define GPIODATA_1        	(*((volatile unsigned long *)(GPIO_GROUP1_BASE+0x0)))

#define GPIODIR_1         	(*((volatile unsigned long *)(GPIO_GROUP1_BASE+0x400)))
#define GPIOIS_1          	(*((volatile unsigned long *)(GPIO_GROUP1_BASE+0x404)))
#define GPIOIBE_1         	(*((volatile unsigned long *)(GPIO_GROUP1_BASE+0x408)))
#define GPIOIEV_1         	(*((volatile unsigned long *)(GPIO_GROUP1_BASE+0x40C)))
#define GPIOIE_1          	(*((volatile unsigned long *)(GPIO_GROUP1_BASE+0x410)))
#define GPIORIS_1         	(*((volatile unsigned long *)(GPIO_GROUP1_BASE+0x414)))
#define GPIOMIS_1         	(*((volatile unsigned long *)(GPIO_GROUP1_BASE+0x418)))
#define GPIOIC_1          	(*((volatile unsigned long *)(GPIO_GROUP1_BASE+0x41C)))

#endif



#define HWreg_GPIO_0_SET_REG(x,offset)          					(*(volatile uint32*)(GPIO_GROUP0_BASE + (offset))) = (uint32)(x)
#define HWreg_GPIO_0_GET_REG(offset)            					(*(volatile uint32*)(GPIO_GROUP0_BASE + (offset)))

#define HWreg_GPIO_1_SET_REG(x,offset)          					(*(volatile uint32*)(GPIO_GROUP1_BASE + (offset))) = (uint32)(x)
#define HWreg_GPIO_1_GET_REG(offset)            					(*(volatile uint32*)(GPIO_GROUP1_BASE + (offset)))



enum {
    LOW = 0x00,HIGH,TOGG,
    INPUT = 0x00,OUTPUT,
    PA0 = 0x00,PA1,PA2,PA3,PA4,PA5,PA6,PA7,
    PB0 = 0x08,PB1,PB2,PB3,PB4,PB5,PB6,PB7,
};


typedef enum {
    PIN_INPUT=0,
    PIN_OUTPUT=1
} PinDirection;


typedef enum
{ 
  GPIO_Mode_IN   = 0x00, /*!< GPIO Input Mode */
  GPIO_Mode_OUT  = 0x01, /*!< GPIO Output Mode */
  GPIO_Mode_AF   = 0x02, /*!< GPIO Alternate function Mode */
  GPIO_Mode_AN   = 0x03  /*!< GPIO Analog Mode */
}GPIOMode_TypeDef;




typedef enum
{ 
  GPIO_PuPd_NOPULL = 0x00,
  GPIO_PuPd_UP     = 0x01,
  GPIO_PuPd_DOWN   = 0x02
}GPIOPuPd_TypeDef;






/*
 * GPIO interrupt trigger
 */
typedef enum {
    IRQ_TRIGGER_RISING_EDGE  = 0x1, /* Interrupt triggered at input signal's rising edge  */
    IRQ_TRIGGER_FALLING_EDGE = 0x2, /* Interrupt triggered at input signal's falling edge */
    IRQ_TRIGGER_BOTH_EDGES   = IRQ_TRIGGER_RISING_EDGE | IRQ_TRIGGER_FALLING_EDGE,
} gecko_gpio_irq_trigger_t;





#define GPIO_Pin_0                 ((uint16_t)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /* Pin 15 selected */
#define GPIO_Pin_All               ((uint16_t)0xFFFF)  /* All pins selected */



int gpio_set_input(GPIO_GROUP_ID_T group_id, uint8 offset);
int gpio_set_output(GPIO_GROUP_ID_T group_id, uint8 offset);
void gpio_set_value(GPIO_GROUP_ID_T group_id,BOOL gpio_val,uint8 gpio_offset);
uint8 gpio_get_value(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);

void GpiopinMode(uint8 pin,uint8 mode);
void GpiopinWrite(uint8 pin,uint8 val);
uint8 GpiopinRead(uint8 pin);

void gpio_set_eint_rising(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);
void gpio_set_eint_falling(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);
void gpio_set_eint_both(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);
void gpio_set_lint_high(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);
void gpio_set_lint_low(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);

void gpio_hall_int_callback_func(void);
void GPIO_Hall_IRQ_Init(void);
void GPIO_Hall_LowLevel_Interrupt_Init(void);
void GPIO_Hall_HighLevel_Interrupt_Init(void);

uint8 KP85_KEY2_Detect(void);
void Set_GPIO_B7_Input(void);
void Set_GPIOA6_Input(void);
void Set_GPIOA6_output(void);



