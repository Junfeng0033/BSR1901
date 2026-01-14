/*

SC_GUI for V2. 0 一个小内存(1-3k)的开源GUI,支持LVGL抗锯齿字体,动态文字,实用波形图,支持双DMA,简易图层叠加


https://gitee.com/li_yucheng/scgui

*/


#include "platform_config.h"
#include "bsr1901.h"

#include <stdarg.h>
#include <stdio.h>

#include "gpio.h"
#include "watchdog.h"
#include "adc.h"
#include "cmu.h"
#include "iomux.h"
#include "dma.h"
//#include "led.h"
#include "uart.h"
#include "hci.h"
#include "norflash.h"
#include "aon.h"
#include "spi.h"
#include "pwm.h"
#include "i2c.h"
#include "timer.h"
#include "bms_ip2366.h"
#include "lcd_driver.h"
#include "gui.h"
#include "ui.h"

#include "sc_demo_test.h"
#include "sc_common.h"


#include "key_handle.h"
//#include "bms_global.h"


#define KEY_PIN  (PA7)

#define POINT_NUM (1024/2)


//#define __RAM_CODE__ 		__attribute__((section("ram_code")))


//int Trim[] __attribute__ ((section(".ARM.__at_0x0001F000"))) = {0x12345678,0x22334455};
//volatile int *myVariable = (volatile int *)0x0001F000;

const uint8 gSysDate[12] = __DATE__;
const uint8 gSysTime[16] = __TIME__;

//#define VERSION "(\"BraveStarr 1900+ GPU\" - GPU MCU Firware  - "__DATE__" - "__TIME__")"



volatile uint16 ram_color=0x1234;
//uint16 ram_color=0;
uint8 flag_key1=0;
uint8 flag_key2=0;

extern int gecko_efuse_read(void);

extern uint32_t TimeTick;

extern void gecko_branchnode_pclkout_cfg(void);

extern void * hw_memcpy8(void * dest, const void * src, uint16 size);

extern unsigned char bFlag_init_ipxs;
extern unsigned char bFlag_p_i2c_int_is_high;
extern unsigned char timer_test;

extern void get_ipxs_state(void);
extern void get_chg_vbus_voltage(void);
extern void get_voltage_battery(void);
extern void get_voltage_vsys(void);

extern uint8 K27_KEY_Detect(void);
	
extern void ui_paint_color_circle(void);

extern void ip2366_reg_write(void);
extern void ip2366_reg_read(void);

extern void bsr1901_prepare_sleep_for_pin_wakeup(void);

extern void delay_1us(unsigned int delay_val);

extern uint32 bsr1901_sram_test(void);
//extern void AON_Analog_Digital_Interface_Reading_Writing(void);
extern void BSR1901_MOS_Gate_CTRL(void);

extern void gecko_task_cm0_sw_flash(void);
extern void gecko_task_cm0_sw_flash_quad(void);


extern void OP_Amp_Config(void);
extern int reg_read(int addr);
extern void reg_write(int addr, int data);

extern void sram_gecko_task_cm0_sw_flash(void);

extern void ui_paint_bat_percent(uint8_t percent);


extern unsigned int gImage_128x128_charging_32bit[8192];
extern const unsigned int gImage_128x128_battery_32b[8192];

extern unsigned int gImage_128x128_star_32bit[8192];
extern unsigned int gImage_128x128_cake_32bit[8192];
extern  unsigned char gImage_128x128_cake[32768];
extern  unsigned char gImage_128x128_star[32768];
extern  unsigned char gImage_128x128_battery[32768];

extern unsigned char gImage_bat_90x49[8820];
extern const unsigned char gImage_circle_100x100[20000];
extern const unsigned char gImage_black_128x128[32768];
extern const unsigned char gImage_charge_10[20000];
extern const unsigned char gImage_charge_20[20000];
extern const unsigned char gImage_charge_30[20000];

extern const unsigned char gImage_charging[32768];

extern uint16 Get_Vbat_Voltage(void);

extern void sc_demo_text(void);

extern uint8 gpio_i2c_initialize(VOID);

extern KeyHandle keyHandle;
extern void Key_Process(void);


extern lv_font_t lv_font_16; 

extern void lcd_dma_refresh_colorblock(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color);




/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemFrequency variable.
 */
void SystemInit(void)
{

	//bsr1901_sram_test();

	
	//2024-11-16
	Osc32M_Freq_Trimming();//trimming 32MHz	
	
//	gecko_rootnode_osc32mclk_cfg();
//	gecko_rootnode_clk32k_cfg();


	//2024-11-16
	gecko_cmu_init();	


	
#if 1	
#if 1
	hal_nvic_clear_pending_irq(CMU2AHB_IRQ);
	hal_nvic_set_priority(CMU2AHB_IRQ, 0);
	hal_nvic_enable_irq(CMU2AHB_IRQ);	
	gecko_task_cm0_sw_flash(); // changed @ 2021.03.31 added SPI-to-AHB ctrl the QSPI controller
	// SPI command 0x3B is fast quad read for XTX NOR Flash 
	// SPI command 0x3B is given by MemRdCmd=0x2 for QSPI control,
	// Address 3 bytes in Regular mode + Dummy 1 byte in Regular mode + Dual-wire mode
	// reg_write((0x50000000+0x50), 0x2);
	//----------------------------------
#else
    //manba_task_sys_debug (0xCAFE00A1, 0x0, 0x0);
    hal_nvic_clear_pending_irq(CMU2AHB_IRQ);
    hal_nvic_set_priority(CMU2AHB_IRQ, 0);
    hal_nvic_enable_irq(CMU2AHB_IRQ);   
    //  gecko_task_cm0_sw_flash(); //
		gecko_task_cm0_sw_flash_quad();
    //----------------------------------
#endif
#endif

}








int main (void) 
{
	//int wr_data;
	//uint8_t gpio_status;
	//uint8_t key_status;
	//uint16_t count = 90;

	SystemInit();


	//gecko_efuse_read();
	
	//LDO33_AUX enable, power supply for LCD module
	LDO33_AUX_Enable();
	
	
	gecko_pinmux_default_config();

	gecko_pinmux_config(PAD15,SPI_CSN1);
	gecko_pinmux_config(PAD16,SPI_MOSI);
	gecko_pinmux_config(PAD17,SPI_CLK);
	
	
	gecko_pinmux_config(PAD18,SPI_MISO);
	//gecko_pinmux_config(PAD18,SPI_MISO);
	gecko_pinmux_config(PAD18,GPIO_B_6);


	HW_SPI_Initialise(HAL_SPI_0);

	Lcd_Init();
	bsr1901_pullup_pulldown_config(PAD_14,PAD_PULLUP);	//BL control	
	//LCD_BL_SET;//turn on backlight
	Lcd_SetRegion(0, 0, 127, 127);						//坐标设置
	//Lcd_Clear(BLACK);
	//delay_1us(8000);	
	//Lcd_Clear(RED);		
	
	DMA_Configuration();	
	
	
	//RefreshColorBlockDynamic(BLUE32);
	
	
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)gImage_128x128_star_32bit, 8192);	

	delay_1us(10000);	
//	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)gImage_128x128_cake_32bit, 8192);	

	
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)gImage_128x128_charging_32bit, 8192);		
  //dma_sram_delay(1000);	
	//while(1);
	delay_1us(8000);
	
	HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)gImage_128x128_battery_32b, 8192);
	
  //dma_sram_delay(1000);	
	//while(1);
	delay_1us(8000);

  //Lcd_Clear(BLACK);
  uint16_t blue_color = C_RED;//C_TOMATO;//C_BLACK;//C_BLUE;
  lcd_dma_refresh_colorblock(0, 0, X_MAX_PIXEL, Y_MAX_PIXEL,&blue_color);
	delay_1us(8000);
	
  //while(1);

	//Lcd_SetRegion(20, 39, 109, 87);						//坐标设置
	//HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_bat_90x49, 8820);
	//delay_1us(8000);
	
	Lcd_SetRegion(10, 10, 109, 109);						//坐标设置
	HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_circle_100x100, 20000);
	delay_1us(8000);
  

	/***************************************************************/
	//config ADC port
	//Channel 5 DO NOT NEEDED TO CONFIG
//	gecko_pinmux_config(PAD20,GPIO_A_2);
//	gecko_pinmux_config(PAD21,GPIO_A_3);
//	bsr1901_adc_8_9_analog_port();
//	ADC_Init();

	//initial Check_ON GPIO for ADC function
	//gecko_pinmux_config(PAD9,GPIO_A_5);
	//gpio_set_output(GPIOA, 5);
	//gpio_set_value(GPIOA, 1, 5);//default ouptut HIGH

	/***************************************************************/

	#if  0//CHRG_INSET_DET_EN //charger insert detect

		//config PAD19(GPIOB7) as GPIO input
		gecko_pinmux_config(PAD19,GPIO_B_7);
		extern void Set_GPIO_B7_Input(void);
		Set_GPIO_B7_Input();

		aon_wakeup_irq_cfg();

	#endif

	#if 0//MIC_DET_EN  //air flow sensor
		//config PAD10(GPIOA6) as GPIO input
		gecko_pinmux_config(PAD10,GPIO_A_6);
		extern void Set_GPIOA6_Input(void);
		Set_GPIOA6_Input();
		//aon_wakeup_irq_cfg();		
	#endif

	/**********************************************************************************	
		//Gecko_Timer_Init();
		//Gecko_Timer_Setting(TIMER_INDEX0);	
	*************************************************************************************/

//		gecko_pinmux_config(PAD0,UART0_OUT);
//		gecko_pinmux_config(PAD1,UART0_IN);

//		gecko_pinmux_config(PAD13,UART1_OUT);
//		gecko_pinmux_config(PAD14,UART1_IN);
		
		Uart_16550_Initialise(HAL_UART_0,115200,0x3);	
		Uart_16550_Initialise(HAL_UART_1,115200,0x3);	
		
	//	gecko_pinmux_config(PAD12,PCLK_OUT);//2024-09-14 Shanghai	

	//----SysTick Init-----

	SysTick_Config(20000);//SysTick Test === 1ms tick for KEY detect

	aon_wakeup_irq_cfg();	


/************************i2c configure***************************************/

	gecko_pinmux_config(PAD8,GPIO_A_4);//I2C/SDA
	gecko_pinmux_config(PAD9,GPIO_A_5);//I2C/SCL
	gecko_pinmux_config(PAD10,GPIO_A_6);//I2C/INT

  //gpio_set_input(GPIOA,6);
	//GPIODIR_0|= BIT(6);
	//gpio_set_value(GPIOA, 1, 6);
	//IP2366_INT_SET;
	//delay_1us(500);

	//Set_GPIOA6_Input();//Hardware pulldown(510K to ground)
	//bsr1901_pullup_pulldown_config(PAD_10,PAD_HIZ);
	//bsr1901_pullup_pulldown_config(PAD_10,PAD_PULLDOWN);

	//gpio_i2c_initialize();
	
  bFlag_init_ipxs = 1;

	
/************************i2c configure***************************************/


	
	#if 0
  printf("date:%s \r\n",gSysDate);
  printf("time:%s \r\n",gSysTime);
	printf("\r\n");
	printf(VERSION);
	#endif


  //RefreshColorBlockDynamic(BLUE32);
	

  //sc_gui_init(lcd_dma_refresh, 0x777, C_ROYAL_BLUE, C_BLUE, &lv_font_16);
	//sc_clear(0, 0, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, gui->bkc);             // 清屏
	
	//sc_create_task(0, sc_demo_drity_tesk, 2);	
	//sc_create_task(0, sc_demo_menu_task, 2);	
	//sc_create_task(0, sc_demo_DrawEye_tesk, 2);
	
	//sc_create_task(0, sc_demo_trans_tesk, 2);
	
	//sc_create_task(0, sc_demo_arc, 2);

	//sc_create_task(0, sc_watch_demo_test, 5);
	
	
	
	while(1)
	{
		//sc_task_loop(NULL);
		
		//system_tick=TimeTick;
		
		//system_tick++;

		
	
		
		Get_Vbat_Voltage();
		
		//printf("\r\n  Charger Bank Solution Software, Copyright (c) 2020-2022 BraveStarr Inc.\r\n");
		
		
		
		//ipxs_writeByte(DEVICE_ADDR_IP, 0x05, 0x75);//write value 0x75 to register 0x05
		//wr_data=ipxs_readByte(DEVICE_ADDR_IP,0x05);//read register 0x05
		//printf("\r\n!!!!!!!!!!!ipxs_readByte ~~~~5678~~~######### = 0x%x \r\n",wr_data);		


		
		
/************************usb insert charging detect***************************************/
		//read 0x31 register(STATE_CTL0)
		//wr_data = ipxs_readByte(DEVICE_ADDR_IP, REG_STATE_CTL0);
	
	  //printf("\r\n !!!!!!!!get_ip2366_state!!!!!  reg_ipxs = %x \r\n",wr_data);		
		//wr_data=wr_data & 0x20;//(bit5,CHG_EN-------1,charging;0, not charging)
	 // printf("\r\n !!!!22222!!!!get_ip2366_state!!22222!!!  reg_ipxs = %x \r\n",wr_data);	
		
		#if 0
		
		if(wr_data==0x20)
		{
			//usb insert
			printf("\r\n usb insert ---- charging -----  \r\n");
			Lcd_SetRegion(0, 0, 127, 127);
			//Lcd_Clear(YELLOW);
			
			HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_charging, 32768);
			//dma_sram_delay(1000);

//			DMA_Configuration();			
//			SPI_32bit_Transfer();
//			HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)gImage_128x128_charging_32bit, 8192);		
//			dma_sram_delay(1000);	
//			SPI_8bit_Transfer();	
		}
		else if(wr_data==0x0)
		{
			#if 1//idle UI
			Lcd_SetRegion(0, 0, 127, 127);
			//Lcd_Clear(RED);
			//HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_128x128_cake, 32768);
			HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_128x128_battery, 32768);
			//dma_sram_delay(1000);
			

	//		SPI_32bit_Transfer();
	//		HW_SPI_Tx_DMA_32bit(HAL_SPI_0, (uint16*)gImage_128x128_star_32bit, 8192);	
	//		dma_sram_delay(1000);
	//		SPI_8bit_Transfer();
			#endif			
		}
		
		get_ipxs_state();
		get_chg_vbus_voltage();
		get_voltage_battery();
		get_voltage_vsys();
		
		
    #endif
/************************usb insert charging detect***************************************/


		
		

/************************key process debug***************************************/
		//KEY1
		#if 0	
		//PAD11(GPIOA7)
		gpio_status=K27_KEY_Detect();//default gpio_status=0x80 
		key_status=gpio_status & 0x80;

		if(key_status==0x0)
		{
			Lcd_SetRegion(0, 0, 127, 127);
			if(flag_key1==0)
			{
				//UATR1_PRINT_LOG((unsigned char *)("K27 KEY Press Down"));
        printf("\r\n K27 KEY Press Down \r\n ");
				HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_128x128_star, 32768);
				//dma_sram_delay(1000);
				//HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_128x128_star_32bit, 8192);
				//Lcd_Clear(BLUE);				
				
				flag_key1=1;
			}
			else
			{
				printf("\r\n K27 KEY Press Down Again \r\n ");				
				HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_128x128_cake, 32768);
				//dma_sram_delay(1000);
				//HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_128x128_cake_32bit, 8192);
				//Lcd_Clear(RED);					
				flag_key1=0;								
			}
		}
		#endif
		
		//KEY2
		#if 0	
		//PAD18(GPIOB6)		
		gpio_status=KP85_KEY2_Detect();//default gpio_status=0x40 
		key_status=gpio_status & 0x40;

		if(key_status==0x0)
		{
			  #if 0
				//LCD_BL_CLR;
				//bsr1901_pullup_pulldown_config(PAD_14,PAD_PULLDOWN);	
        #endif

				//LDO33_AUX disable, power down LCD module			
				LDO33_AUX_Disable();
			
//				wr_data = 0x608e7885;
//				reg_write(0x40020000+0x020, wr_data);
//				
//				//reg_aon_sel_aon_clk16k(bit10)
//				wr_data=reg_read(0x40020000+0x000);
//				wr_data |= 0x200;//(set bit10=1)
//				reg_write(0x40020000+0x000, wr_data);
			
        bsr1901_prepare_sleep_for_pin_wakeup();
			  //sleep-wakeup setting
				BSR1901_GPIO_WakeUp_From_DeepSleep();//
				tc_gecko_cm0_aon_sleep();//deep sleep test for low power design

		}
		#endif		
		
/************************key process debug***************************************/	
		
		
		
//		if(count%10 == 1){
//			if(count <= 10)
//				HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_charge_10, 20000);
//			if(count <= 20)
//				HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_charge_20, 20000);
//			if(count <= 30)
//				HW_SPI_Tx_DMA(HAL_SPI_0, (uint16*)gImage_charge_30, 20000);
//		}
		

		
//		GuiShowNumString_16(50, 18, count, 2);
//		GuiShowNumString_48(32, 35, count, 2);
//		GuiShowPersent(45+40, 45);
//		GuiShowNumString_16(50, 85, count, 2);






		#if 0
		adc_datavalue=GeckoGpadcGetRawData(GPADC_CHAN_5);//get VBAT voltage
		/**********************************************************************/	  
		gpio_set_value(GPIOA, 0, 5);//Check_ON ouptut LOW

		//ADC_VOUT1
		adc_datavalue=GeckoGpadcGetRawData(GPADC_CHAN_8);//ADC_VOUT1 (VOUT1)

		//Check_ADC(PNP_OUT)
		adc_datavalue=GeckoGpadcGetRawData(GPADC_CHAN_9);//Check_ADC (PNP_OUT)

		gpio_set_value(GPIOA, 1, 5);//Check_ON ouptut HIGH
		/**********************************************************************/		
		#endif

		#if 0
		//////////////////mi tou/////////////////////////////////////	
		//PAD10(GPIOA6)
		//PAD10 pulldown

		//bsr1901_pullup_pulldown_config(PAD_10,PAD_PULLUP);
		//bsr1901_pullup_pulldown_config(PAD_10,PAD_PULLDOWN);	

		gpio_status=mi_tou_detect();
		if(gpio_status==0x40)
		{
			mi_tou_flag=1;//smoking in progress
			
//			UATR1_PRINT_LOG((unsigned char *)("\r\n"));						
//			UATR1_PRINT_LOG((unsigned char *)("mi_tou_detect---gpio_a6 HIGH:0x="));
//			string=my_itoa(gpio_status);
//			UATR1_PRINT_LOG((unsigned char *)(string));
//			UATR1_PRINT_LOG((unsigned char *)("\r\n"));
		}
		////////////////////////////////////////////////////////////////			
		#endif		

		#if 0
			//LDO33_AUX disable, power down LCD module
			wr_data = reg_read(0x40020000+0x28);
			//wr_data &= (~0x340);
			wr_data=0x0;
			reg_write(0x40020000+0x28,wr_data);

			#if 0
			delay_1us(1000);

			//LDO33_AUX enable, power on LCD module
			wr_data |= 0x340;
			reg_write(0x40020000+0x28,wr_data);

			delay_1us(5000);
      #endif			
		#endif
		


	}
}
