/*******************************************************************************
*
*
*
*
********************************************************************************/
//#include "bsr1901.h"

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
1109 Cost Down IOMUX Table
//////////////////////////////////////////////////////////////////////////////////////////////////////////
pad name	location	default function						function0	 		function1						function2			ANA func
PAD_00		3p3				swclk												uart0_out			gpio_a_0						pwm_ch4				ADC2
PAD_01		3p3				swdio												uart0_in			gpio_a_1						pwm_ch5				ADC3
PAD_02		3p3_f			spi_fls_scsn			　					                       	 	 	 	 	           	 	
PAD_03		3p3_f			spi_fls_sclk								                        					          		
PAD_04		3p3_f			spi_fls_mosi								                        					          		
PAD_05		3p3_f			spi_fls_miso								                        					          		
PAD_06		3p3_f			spi_fls_hold								gpio_a_7			uart2_in						pwm_ch0				---
PAD_07		3p3_f			spi_fls_wp									gpio_b_7			uart2_out						pwm_ch1				---
PAD_08		vbat			spi2ahb_clk									uart2_out			gpio_a_4						spi0_mst_clk	ADC0
PAD_09		vbat			spi2ahb_mosi	　			      uart2_in			gpio_a_5						spi0_mst_do		ADC1
PAD_10		vbat			spi2ahb_miso	　						i2c0_sda			gpio_a_6						spi0_mst_di		ADC6
PAD_11		vbat			spi2ahb_csn	　							i2c0_scl			gpio_a_7						pwm_ch6				CK32_OUT
PAD_12		vbat			pwm_cap_1										pwm_ch5				gpio_b_0						spi0_mst_csn	DAC
PAD_13		vbat			pwm_ch0											uart1_in			gpio_b_1						led_io1				OPA0_VIN
PAD_14		vbat			pwm_ch1											uart1_out			gpio_b_2						led_io2				OPA0_VIP
PAD_15		vbat			clk_ref(OSC校正参考时钟)		pwm_ch4				gpio_b_3						led_io3				OPA0_VO
PAD_16		vbat			pwm_ch3										  spi1_mst_di		gpio_b_4						led_io4				OPA1_VIN
PAD_17		vbat			pwm_ch6											spi1_mst_do		gpio_b_5						led_io5				OPA1_VIP
PAD_18		vbat			pwm_ch7											spi1_mst_clk	gpio_b_6						led_io6				OPA1_VO
PAD_19		vbat			ftest											  spi1_mst_csn	gpio_b_7						pwm_cap_0			ADC4
PAD_20		1p8				uart0_dat(pogo_pin1)				i2c1_scl			gpio_a_2						pwm_ch2				ADC9
PAD_21		1p8				uart1_dat(pogo_pin2)				i2c1_sda			gpio_a_3						pwm_ch7				ADC7
PAD_22		1p8				uart2_dat(pogo_pin3)				gpio_b_0			PWM_BRK（紧急刹车）	pclk_out			ADC8
*/





//SYS CTRL REG
#define GECKO_SYSCTRL_EXT_BASE_ADDR       0x40040000

//#define    GECKO_APB_CMU_BASE                0X40010000
//#define    GECKO_APB_SCU_BASE                0X40010000

#define GECKO_AON_BASE_ADDR             0x40020000 //GEK1109

//#define GECKO_AON_BASE_ADDR             0x4001A000 //GEK1108
//#define ADDR_BASE_GECKO_AON_CONFIG      0x4001A000



#define SYS_HW32_REG_RD(reg)  						(*(volatile uint32_t  *)(GECKO_SYSCTRL_EXT_BASE_ADDR+reg))
#define SYS_HW32_REG_WR(reg,val)  				((*(volatile uint32_t *)(GECKO_SYSCTRL_EXT_BASE_ADDR+reg)) = val)








#define    GECKO_APB_CMU_BASE                			0X40010000
#define    GECKO_APB_SCU_BASE                			0X40010000



#define NULL 0



typedef enum
{
    PAD_PULLUP=0,
    PAD_PULLDOWN,
    PAD_HIZ	
}Pad_PULL_CFG;


typedef enum
{
    Domain_AON,  //aon io ring
    Domain_3P3,  //3.3v io ring
	  Domain_1P8   //1.8v io ring
}Pad_Ring_Domain;

typedef enum
{
    PAD0,
    PAD1,
    PAD2,
    PAD3,
    PAD4,
    PAD5,
    PAD6,
    PAD7,
    PAD8,
    PAD9,
    PAD10,
    PAD11,
    PAD12,
    PAD13,
    PAD14,
    PAD15,
    PAD16,
    PAD17,
    PAD18,
    PAD19,
    PAD20,
    PAD21,
    PAD22,
    PAD23,
    PAD24		
}Pad_Num_Type;

/*
typedef enum
{
    UART0_OUT,  			GPIO_A_0,			I2C0_SCL,				//PAD00
    UART0_IN, 				GPIO_A_1, 		I2C0_SDA,				//PAD01
																										//PAD02
																										//PAD03
																										//PAD04
																										//PAD05	
    GPIO_A_2,    			UART_2_IN,		PWM4,						//PAD06
    GPIO_A_3,					UART_2_OUT,		PWM5,						//PAD07		
    PWM_CH2,	 				GPIO_A_4,			SPI0_CLK,				//PAD08
    PWM_CH3,	 				GPIO_A_5,			SPI0_DO,				//PAD09	
    PWM_CH4,	 				GPIO_A_6,			SPI0_DI,				//PAD10
    PWM_CH5,	 				GPIO_A_7,			SPI0_CSN,				//PAD11
    UART2_OUT,				GPIO_B_0,			PWM6,						//PAD12	
    I2C1_SCL,	 				GPIO_B_1,			LED_IO1,				//PAD13	
    I2C1_SDA,	 				GPIO_B_2,			LED_IO2,				//PAD14
    UART2_IN,	 				GPIO_B_3,			LED_IO3,				//PAD15	
    SPI1_CLK,	 				GPIO_B_4,			LED_IO4,				//PAD16	
    SPI1_DI,	 				GPIO_B_5,			LED_IO5,				//PAD17	
    SPI1_DO,	 				GPIO_B_6,			LED_IO6,				//PAD18		
    SPI1_CSN,	 				GPIO_B_7,			PWM_CAP,				//PAD19		
    UART1_IN,	 				GPIOA_2,			PWM2,						//PAD20
    UART1_OUT,	 			GPIOA_3,			PWM7,						//PAD21
    GPIOB_0,	 			  PWM_BRK,			PCLK_OUT,				//PAD22				
	  PAD	
}Pad_Func_Type;
*/




typedef enum
{
//  function0         function1
    UART0_OUT,  			GPIO_A_0,			//PAD00
    UART0_IN, 				GPIO_A_1, 		//PAD01
																		//PAD02
																		//PAD03
																		//PAD04
																		//PAD05	
    GPIOA_7,    			UART0_SOUT,				  //PAD06
    GPIOB_7,					UART0_SIN,				  //PAD07		
    PWM_CH3,	 			  GPIO_A_4,						//PAD08
    PWM_CH6,	 				GPIO_A_5,						//PAD09	
    PWM_CH7,	 				GPIO_A_6,						//PAD10
    PWM_CH2,	 				GPIO_A_7,						//PAD11
    PCLK_OUT,					GPIO_B_0,						//PAD12	
    UART1_OUT,	 			GPIO_B_1,						//PAD13	
    UART1_IN,	 				GPIO_B_2,						//PAD14
    SPI_CSN1,	 				GPIO_B_3,						//PAD15	
    SPI_MOSI,	 				GPIO_B_4,						//PAD16	
    SPI_CLK,	 				GPIO_B_5,						//PAD17	
    SPI_MISO,	 				GPIO_B_6,						//PAD18		
    SPI_CSN2,	 				GPIO_B_7,						//PAD19
		
    GPIO_A_2,	 				SPI_CSN_1,					//PAD20
    GPIO_A_3,	 				SPIMISO,						//PAD21

    PWM_CH0,	 				SPICLK,							//PAD22
    PWM_CH1,	 				SPIMOSI,						//PAD23
    PWM_CH4,	 				SPI_CSN_2,					//PAD24

	  PAD	
}Pad_Func_Type;


/**********************************************************************************************
*	
* 1109_IOMUX_FUN_0
*
* 23bits
pad_fun0[0]
pad_fun0[1]
pad_fun0[2]
pad_fun0[3]
pad_fun0[4]
pad_fun0[5]
pad_fun0[6]
pad_fun0[7]
pad_fun0[8]
pad_fun0[9]
pad_fun0[10]
pad_fun0[11]
pad_fun0[12]
pad_fun0[13]
pad_fun0[14]
pad_fun0[15]
pad_fun0[16]
pad_fun0[17]
pad_fun0[18]
pad_fun0[19]
pad_fun0[20]
pad_fun0[21]
pad_fun0[22]
pad_fun0[23]
pad_fun0[24]
***********************************************************************************************/

#define FUNC0_UART0_OUT					((0x1)<<0)   //PAD00
#define FUNC0_UART0_IN					(1<<1)			 //PAD01
#define FUNC0_NULL_PAD02				(1<<2)			 //PAD02
#define FUNC0_NULL_PAD03				(1<<3)       //PAD03
#define FUNC0_NULL_PAD04				(1<<4)			 //PAD04
#define FUNC0_NULL_PAD05				(1<<5)			 //PAD05
#define FUNC0_GPIO_A_7				  (1<<6)			 //PAD06
#define FUNC0_GPIO_B_7				  (1<<7)			 //PAD07

#define FUNC0_PWM_CH3				    (1<<8)			 //PAD08
#define FUNC0_PWM_CH6				    (1<<9)			 //PAD09
#define FUNC0_PWM_CH7				    (1<<10)			 //PAD10
#define FUNC0_PWM_CH2				    (1<<11)			 //PAD11

#define FUNC0_PCLK_OUT					(1<<12)			 //PAD12
#define FUNC0_UART1_OUT				  (1<<13)			 //PAD13
#define FUNC0_UART1_IN				  (1<<14)			 //PAD14

#define FUNC0_SPI_CSN1				  (1<<15)			 //PAD15
#define FUNC0_SPI_MOSI					(1<<16)			 //PAD16
#define FUNC0_SPI_CLK				  	(1<<17)			 //PAD17
#define FUNC0_SPI_MISO				  (1<<18)			 //PAD18

#define FUNC0_SPI_CSN2				  (1<<19)			 //PAD19



#define FUNC0_GPIOA_2						(1<<20)			 //PAD20
#define FUNC0_GPIOA_3						(1<<21)      //PAD21
#define FUNC0_PWM_CH0						(1<<22)			 //PAD22
#define FUNC0_PWM_CH1						(1<<23)      //PAD23
#define FUNC0_PWM_CH4						(1<<24)      //PAD24

/**********************************************************************************************
*	
* 1109_IOMUX_FUN_1
*
* 23 bits
pad_fun1[0]
pad_fun1[1]
pad_fun1[2]
pad_fun1[3]
pad_fun1[4]
pad_fun1[5]
pad_fun1[6]
pad_fun1[7]
pad_fun1[8]
pad_fun1[9]
pad_fun1[10]
pad_fun1[11]
pad_fun1[12]
pad_fun1[13]
pad_fun1[14]
pad_fun1[15]
pad_fun1[16]
pad_fun1[17]
pad_fun1[18]
pad_fun1[19]
pad_fun1[20]
pad_fun1[21]
pad_fun1[22]
pad_fun1[23]
pad_fun1[24]
***********************************************************************************************/


#define FUNC1_GPIO_A_0					((0x1)<<0)   //PAD00
#define FUNC1_GPIO_A_1					(1<<1)			 //PAD01

#define FUNC1_NULL_PAD02				(1<<2)			 //PAD02
#define FUNC1_NULL_PAD03				(1<<3)       //PAD03
#define FUNC1_NULL_PAD04				(1<<4)			 //PAD04
#define FUNC1_NULL_PAD05				(1<<5)			 //PAD05
#define FUNC1_UART0_SOUT				(1<<6)			 //PAD06
#define FUNC1_UART0_SIN				  (1<<7)			 //PAD07

#define FUNC1_GPIO_A_4				  (1<<8)			 //PAD08
#define FUNC1_GPIO_A_5				  (1<<9)			 //PAD09
#define FUNC1_GPIO_A_6				  (1<<10)			 //PAD10
#define FUNC1_GPIO_A_7				  (1<<11)			 //PAD11

#define FUNC1_GPIO_B_0					(1<<12)			 //PAD12
#define FUNC1_GPIO_B_1				  (1<<13)			 //PAD13
#define FUNC1_GPIO_B_2				  (1<<14)			 //PAD14
#define FUNC1_GPIO_B_3				  (1<<15)			 //PAD15
#define FUNC1_GPIO_B_4					(1<<16)			 //PAD16
#define FUNC1_GPIO_B_5				  (1<<17)			 //PAD17
#define FUNC1_GPIO_B_6				  (1<<18)			 //PAD18
#define FUNC1_GPIO_B_7				  (1<<19)			 //PAD19



#define FUNC1_SPI_CS1						(1<<20)			 //PAD20
#define FUNC1_SPI_MISO					(1<<21)      //PAD21
#define FUNC1_SPI_CLK						(1<<22)			 //PAD22
#define FUNC1_SPI_MOSI					(1<<23)      //PAD23
#define FUNC1_SPI_CS2						(1<<24)      //PAD24





/**********************************************************************************************
*	
* 1109_IOMUX_FUN_2
*
* 23 bits
*
pad_fun2[0]
pad_fun2[1]
pad_fun2[2]
pad_fun2[3]
pad_fun2[4]
pad_fun2[5]
pad_fun2[6]
pad_fun2[7]
pad_fun2[8]
pad_fun2[9]
pad_fun2[10]
pad_fun2[11]
pad_fun2[12]
pad_fun2[13]
pad_fun2[14]
pad_fun2[15]
pad_fun2[16]
pad_fun2[17]
pad_fun2[18]
pad_fun2[19]
pad_fun2[20]
pad_fun2[21]
pad_fun2[22]
pad_fun2[23]
pad_fun2[24]
***********************************************************************************************/


//NULL




/**********************************************************************************************
*	
* PADAON_IOMUX_FUN_2(AON_IOMUX_SEL_2)
* 12 bits
* register 0x2C
***********************************************************************************************/
/*
#define FUNC_GPIOA_0_4				(1<<0)
#define FUNC_GPIOA_0_5				(1<<1)
#define FUNC_GPIO_0_6					(1<<2)
#define FUNC_GPIO_0_7				  (1<<3)

#define FUNC_GPIO_1_1				  (1<<5)
#define FUNC_GPIO_1_2				  (1<<6)
#define FUNC_GPIO_1_3				  (1<<7)
#define FUNC_GPIO_1_4				  (1<<8)
#define FUNC_GPIO_1_5				  (1<<9)
#define FUNC_GPIO_1_6				  (1<<10)
*/



typedef enum
{
    PAD_00,
    PAD_01,
    PAD_02,
    PAD_03,
    PAD_04,
    PAD_05,
    PAD_06,
    PAD_07,
    PAD_08,
    PAD_09,
    PAD_10,
    PAD_11,
    PAD_12,
    PAD_13,
    PAD_14,
    PAD_15,
    PAD_16,
    PAD_17,
    PAD_18,
    PAD_19,
    PAD_20,
    PAD_21,
    PAD_22,
    PAD_23,
    PAD_24,		
}Pad_Num;





//for GEK1109

#define PAD00_PULL					((0x1)<<0)   //PAD00
#define PAD01_PULL					(1<<1)			 //PAD01
#define PAD02_PULL				  (1<<2)			 //PAD02
#define PAD03_PULL				  (1<<3)       //PAD03
#define PAD04_PULL				  (1<<4)			 //PAD04
#define PAD05_PULL				  (1<<5)			 //PAD05
#define PAD06_PULL				  (1<<6)			 //PAD06
#define PAD07_PULL				  (1<<7)			 //PAD07

#define PAD08_PULL					(1<<8)   		 //PAD08
#define PAD09_PULL					(1<<9)			 //PAD09
#define PAD10_PULL				  (1<<10)			 //PAD10
#define PAD11_PULL				  (1<<11)       //PAD11
#define PAD12_PULL				  (1<<12)			 //PAD12
#define PAD13_PULL				  (1<<13)			 //PAD13
#define PAD14_PULL				  (1<<14)			 //PAD14
#define PAD15_PULL				  (1<<15)			 //PAD15
#define PAD16_PULL				  (1<<16)			 //PAD16
#define PAD17_PULL					(1<<17)			 //PAD17
#define PAD18_PULL				  (1<<18)      //PAD18
#define PAD19_PULL				  (1<<19)      //PAD19

#define PAD20_PULL					(1<<20)			 //PAD20
#define PAD21_PULL				  (1<<21)      //PAD21
#define PAD22_PULL				  (1<<22)      //PAD22
#define PAD23_PULL				  (1<<23)      //PAD23
#define PAD24_PULL				  (1<<24)      //PAD24
///////////////////////////////////////////////////////////



extern int reg_read(int addr);
extern void reg_write(int addr, int data);


void gecko_pinmux_config(Pad_Num_Type pad_num, Pad_Func_Type pad_func);
void bsr1901_pullup_pulldown_config(Pad_Num pad_num,Pad_PULL_CFG pull_type);
void gecko_pinmux_default_config(void);
void gecko_branchnode_pclkout_cfg(void);
	





