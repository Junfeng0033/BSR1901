# API.md

BSR1901外设基础API函数，AI-IDE在编写代码前告诉AI先阅读和学习这个API.md


## 1. GPIO

1901一共两组GPIO，每组8个GPIO口，分别是GPIO0和GPIO1。
GPIO模块基地址：0x40018000(GPIO0)和0x40019000(GPIO1)
#define GPIO_GROUP_A_BASE      (0x40018000)
#define GPIO_GROUP_B_BASE      (0x40019000)
#define GPIO_GROUP0_BASE       0x40018000
#define GPIO_GROUP1_BASE       0x40019000

void GpiopinMode(uint8 pin,uint8 mode)和VOID GPIO_InitIO(UINT8 direction, UINT8 port)
设置gpio的输入输出模式，参数mode
enum {
    LOW = 0x00,HIGH,TOGG,
    INPUT = 0x00,OUTPUT,s
    PA0 = 0x00,PA1,PA2,PA3,PA4,PA5,PA6,PA7,
    PB0 = 0x08,PB1,PB2,PB3,PB4,PB5,PB6,PB7,
};

用法：
GpiopinMode(PB3,OUTPUT);//输出模式
GpiopinWrite(PB3,HIGH);//输出高电平

GPIO_InitIO(1,SDA_PIN);//设置SDA_PIN为输出
GPIO_InitIO(0,SDA_PIN);//设置SDA_PIN为输入


void GpiopinWrite(uint8 pin,uint8 val)和VOID GPIO_WriteIO(UINT8 data, UINT8 port)
GpiopinWrite设置gpio输出高或者低电平，参数1，pin；参数2，val
GPIO_WriteIO设置gpio输出高或者低电平，参数1,data;参数2，port
用法：
GpiopinWrite(PB3,HIGH);//输出高电平
GPIO_WriteIO(0, SCL_PIN);//设置SCL_PIN为低电平


uint8 GpiopinRead(uint8 pin)和UINT8 GPIO_ReadIO(UINT8 port)
GpioPinRead读取gpio的状态，参数，pin
GPIO_ReadIO读取gpio的状态，参数，port
用法：
#define KEY1  (PB5)
//GpioPinRead(KEY1);

if(GpiopinRead(KEY1) != 0x20)
{
	GpiopinMode(PB3,OUTPUT);
	GpiopinWrite(PB3,HIGH);
}
//PAD11(GPIOA7)
gpio_status=K27_KEY_Detect();//default gpio_status=0x80 
key_status=gpio_status & 0x80;

#define SDA_PIN		PA4
uint8 temp_value = 0;
temp_value = GPIO_ReadIO(SDA_PIN);//读取SDA_PIN的状态


GPIO中断的配置

typedef enum {
    GPIO_GROUP_0 = 0x0,
    GPIO_GROUP_1 = 0x1,
    GPIO_GROUP_QTY
} GPIO_GROUP_ID_T; 

GPIO口中断设置常用的几个函数：
void gpio_set_eint_rising(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);
void gpio_set_eint_falling(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);
void gpio_set_eint_both(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);
void gpio_set_lint_high(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);
void gpio_set_lint_low(GPIO_GROUP_ID_T group_id,uint8 gpio_offset);

比如配置GPIOA5为中断功能范例代码：
//Hall Key ---> GPIO_0_5
//set GPIO_A_5 Input 
void GPIO_Hall_IRQ_Init(void)
{
      uint8 gpio_offset=5;//GPIOA5
      //#define BIT(x)    (1<<(x))	
	  gpio_offset = BIT(gpio_offset);	
	  GPIOIE_Shadow=gpio_offset;
	  GPIOIE=0;//disable gpio group0 interrupt      
      //gpio input	
	  GPIODIR_Shadow &= (~gpio_offset);
	  GPIODIR = GPIODIR_Shadow;//input,set Corresponding bit to "0" ,input mode	
	  GPIOIE_Shadow |= gpio_offset;
	  GPIOIE = GPIOIE_Shadow;//set Corresponding bit to "1" ,interrupt enable
	  //Disable level detection 
	  GPIOIS_Shadow &= (~gpio_offset);
      GPIOIS = GPIOIS_Shadow;			
      //Clear detection on both edges
	  GPIOIBE_Shadow &= (~gpio_offset);
      GPIOIBE = GPIOIBE_Shadow;
	  GPIOIEV_Shadow &= (~gpio_offset);
	  GPIOIEV = GPIOIEV_Shadow;//falling edge trigger
	  /******* configure the GPIO NVIC ********************************/
	  hal_nvic_clear_pending_irq(GPIO_05_MISC_IRQn);//GPIO_0_5(GPIO05)
	  hal_nvic_set_priority(GPIO_05_MISC_IRQn, 0);
	  hal_nvic_enable_irq(GPIO_05_MISC_IRQn);	
}


//Hall Key ---> GPIO_0_5
//set GPIO_A_5 Input 
void GPIO_Hall_HighLevel_Interrupt_Init(void)
{
    uint8 gpio_offset=5;
	gpio_set_lint_high(GPIO_GROUP_0,gpio_offset);

	hal_nvic_clear_pending_irq(GPIO_A5_IRQn);//GPIO_0_5(GPIO05)
	hal_nvic_set_priority(GPIO_A5_IRQn, 0);
	hal_nvic_enable_irq(GPIO_A5_IRQn);	
}


## 2. PWM

1901包含8个16bit的 PWM模块。每个PWM 都支持反向，死区控制。

PWM模块基地址
//AHB Address
#define GECKO_PWM_BASE_NEW								0x40005000

hw_pwm_enable(HW_PWM_CHAN_T channel)
PWM通道使能，参数channel。
用法：
hw_pwm_enable(HW_PWM_CHAN_3);


void hw_pwm_disable(HW_PWM_CHAN_T channel)
PWM通道输出关闭，参数channel。
用法：
hw_pwm_disable(HW_PWM_CHAN_3);


void hw_pwm_invert(HW_PWM_CHAN_T channel)
PWM通道反向输出，参数channel。

用法：
hw_pwm_invert(HW_PWM_CHAN_7);	


void Config_PWM(HW_PWM_CHAN_T channel,const struct HAL_PWM_CFG_T *cfg)

struct HAL_PWM_CFG_T {
    uint32_t freq; //expected frequency
    uint8_t ratio; //expected duty
	uint8   Tdead_cycle_count;
};
配置PWM通道的参数，有2个参数：

参数1，channel
参数2，*cfg

用法：
SixChCfg.freq= 200000;//配置PWM频率
SixChCfg.ratio= 50;//配置PWM占空比
SixChCfg.Tdead_cycle_count=1;//配置PWM死区的cycle
Config_PWM(HW_PWM_CHAN_1,&SixChCfg);	


//配置 PWM 输出
struct HAL_PWM_CFG_T pwm_cfg = {200000, 50, 1};
Config_PWM(HW_PWM_CHAN_1, &pwm_cfg);
hw_pwm_enable(HW_PWM_CHAN_1);


## 3. SPI

BSR1901的SPI接口可以认为是专门用于 LCD 显示屏的，SPI的通信速率可以达到25Mbit/s,可以驱动65536色的LCD。这个接口有两个片选信号，支持双屏异显。

void HW_SPI_Initialise(HAL_SPI_ID_T id)
初始化SPI接口，参数id(1901芯片只保留SPI_0)
typedef enum {
    HAL_SPI_0                                  = 0x00000000,
    HAL_SPI_1                                  = 0x00000001,//New SPI--->2022-09-04	HAL_SPI_QTY
} HAL_SPI_ID_T;  

用法：
HW_SPI_Initialise(HAL_SPI_0);




void HW_SPI_Tx_DMA_16bit(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)
{
	LCD_RS_SET;	
    SPI_16bit_Transfer();
	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
	AHB_DMA_CONTROL_HWORD_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);	
	dma_sram_delay(1000);	
}

void HW_SPI_Tx_DMA_16bit_ColorBlock(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)
{
	LCD_RS_SET;	
    SPI_16bit_Transfer();
	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
	AHB_DMA_CONTROL_HWORD_TR, AHB_DMA_CONTROL_SRC_NOINC_DES_NOINC);	
	dma_sram_delay(1000);		
}

上面的两个函数在DMA配置的时候， 一个是SRC_INC， 一个是SRC_NOINC，区别比较大，请根据场景需要调用。

void HW_SPI_Tx_DMA_32bit(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)

设置DMA按照32bit读取数据，同时SPI按照32bit格式传输输出数据：

参数1，id
参数2，pData
参数3，DataLen

void HW_SPI_Tx_DMA_32bit(HAL_SPI_ID_T id,uint16 *pData, uint16 DataLen)
{
	LCD_RS_SET;	
    SPI_32bit_Transfer();
	Gecko_DMA_Transport((volatile uint32 *)(XR7_SPI_BASE + XR7_SPI_FIFO), pData, DataLen, 
	AHB_DMA_CONTROL_WORD_TR, AHB_DMA_CONTROL_SRC_INC_DES_NOINC);	
	dma_sram_delay(1000);	
}

用法：
HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)fire_eye_7, 12800);	

上面一共有四个与DMA相关的函数，因为我们的DMA支持三种传输：
32bit WORD 传输
#define AHB_DMA_CONTROL_WORD_TR                 0x08 
16bit HALF WORD 传输
#define AHB_DMA_CONTROL_HWORD_TR                0x04
8bit BYTE传输
#define AHB_DMA_CONTROL_BYTE_TR                   0x00
同时我们的SPI支持四种传输：
input [1:0] size;
//2'b00:8bit;
//2'b01:16bit(bit5=0，bit4=1); 
//2'b10:32bit(bit5=1，bit4=0);
//2'b11:24bit

在做LCD显示色块测试的时候，我建议调用HW_SPI_Tx_DMA_16bit_ColorBlock函数，范例如下：
#define RED  			0xf800
#define GREEN		      0x07e0
#define BLUE 			0x001f
#define WHITE		      0xffff
#define BLACK			0x0000
uint16_t blue_color = BLACK;// RED ;// GREEN;//BLUE;
lcd_dma_refresh_colorblock(0, 0, X_MAX_PIXEL, Y_MAX_PIXEL,&blue_color);

void lcd_dma_refresh_colorblock(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color)
{
	uint32_t len = w*h;	
	Lcd_SetRegion(xs, ys, xs+w-1, ys+h-1);		
	HW_SPI_Tx_DMA_16bit_ColorBlock(HAL_SPI_0,color,len);	
}


## 4. DMA


1901的DMA在设计的时候，优先考虑的应用场景就是LCD刷屏。

关于DMA寄存器的说明：
#define AHB_DMA_CONTROL_REG                     (AHB_DMA_BASE + 0x00)
#define AHB_DMA_SRCADDR_REG                     (AHB_DMA_BASE + 0x04)
#define AHB_DMA_DESTADDR_REG                    (AHB_DMA_BASE + 0x08)
#define AHB_DMA_DATALENGTH_REG                  (AHB_DMA_BASE + 0x0C)


void DMA_Configuration(void)
初始化DMA，无参数
用法：
DMA_Configuration();


__RAM_CODE__ void Clear_DMA_Interrupt(void)
清除DMA中断，无参数。　DMA每完成一次传输，都会产生一次中断。
用法：
在ISR中调用
__RAM_CODE__  void DMA_IRQHandler(void)
{
	dma_int_flag =1;	
	Clear_DMA_Interrupt();
}



## 5. UART


1901有两个UART模块。UART模块基地址：0x40001000/0x40003000


void Gecko_Uart0_Send_Char(char s)

UART_0发送一个字符，参数s；通常用于打印log信息。

用法：
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
	Gecko_Uart0_Send_Char(ch);
	return ch;
}


void Gecko_Uart1_Send_Char(char s)
UART_1发送一个字符，参数s；通常用于打印log信息。

用法：
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
	Gecko_Uart1_Send_Char(ch);
	return ch;
}



void Uart_16550_Initialise(HAL_UART_ID_T id,uint32 baud_rate, uint16 uart_setting)
UART初始化，参数1，id；参数2，baud_rate；参数3，uart_setting

用法：
Uart_16550_Initialise(HAL_UART_0,115200,0x3);	
Uart_16550_Initialise(HAL_UART_1,115200,0x3);	


## 6. ADC

1901芯片集成了一个10通道的12bit ADC

// -----------------------------------------------------------------------------
// Channel selection.
// =============================================================================
typedef enum
{
    GPADC_CHAN_0,//AC_IN
    GPADC_CHAN_1,//VBAT
    GPADC_CHAN_2,//REAR_DET
    GPADC_CHAN_3,//LEAR_DET
    GPADC_CHAN_4,//QI_DET
    GPADC_CHAN_5,//VDD5V
    GPADC_CHAN_6,//TEMP_SEN
    GPADC_CHAN_7,//ISEN_BATT	
    GPADC_CHAN_8,
    GPADC_CHAN_9,		
    GPADC_CHAN_QTY,	
} GPADC_CHAN_T;



void ADC_Init(void)

初始化ADC，配置ADC的参考电压，输入buffer等



uint16 GeckoGpadcGetRawData(GPADC_CHAN_T channel)

读取ADC通道的ADC值



## 7. SoftPUF

在 Reset_Handler 里面调用这个功能。

void sram_puf_on_power_up(void)

CPU上电，初始化之前，先获取SRAM PUF，在startup_cm0.s里面调用sram_puf_on_power_up。



---


