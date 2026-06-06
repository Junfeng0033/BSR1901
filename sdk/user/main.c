
#include "main.h"
#include "SEGGER_RTT.h"

//#define __RAM_CODE__ 		__attribute__((section("ram_code")))
//int Trim[] __attribute__ ((section(".ARM.__at_0x0001F000"))) = {0x12345678,0x22334455};
//volatile int *myVariable = (volatile int *)0x0001F000;

const uint8 gSysDate[12] = __DATE__;
const uint8 gSysTime[16] = __TIME__;

//#define VERSION "(\"BraveStarr 1901+ GPU\" - GPU MCU Firware  - "__DATE__" - "__TIME__")"





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




void EnterDeepSleepMode(void)
{
	
		#if 0
		//LCD_BL_CLR;
		//bsr1901_pullup_pulldown_config(PAD_14,PAD_PULLDOWN);	
		#endif

		//LDO33_AUX disable, power down LCD module			
		LDO33_LCD_Disable();

//	wr_data = 0x608e7885;
//	reg_write(0x40020000+0x020, wr_data);
//				
//	wr_data=reg_read(0x40020000+0x000);
//	wr_data |= 0x200;//(set bit10=1)
//	reg_write(0x40020000+0x000, wr_data);

		bsr1901_prepare_sleep_for_pin_wakeup();
		//sleep-wakeup setting
		tc_gecko_cm0_aon_sleep();//deep sleep test for low power design
	
}








int main (void) 
{

	uint32_t current_tick;

	SystemInit();

	aon_wakeup_irq_cfg();	
	//gecko_efuse_read();


#ifdef LOG_SEGGER_RTT
    SEGGER_RTT_Init();
    SEGGER_RTT_printf(0, "LOG_SEGGER_RTT Initial !\r\n");
	  SEGGER_RTT_printf(0, "Tick Value: %d\r\n", current_tick);
#endif

	
	gecko_pinmux_default_config();
	

//本程序适配 BSR1901 推荐硬件端口
//              GND   电源地
//              VCC   接3.3v电源--LDO33_AUX_OUT

//              SCL   接SPI_CLK--------(PAD22)
//              SDA   接SPI_MOSI-------(PAD23)

//              RES   接PB7------------(PAD07)
//              DC    接PA3(MISO)------(PAD21)

//              CS    接SPI_CSN_1------(PAD20)//有些显示屏可以直接接地

//              BLK   接PB4(PWM4/CSN2)-(PAD24)


	gecko_pinmux_config(PAD22,SPICLK);
	gecko_pinmux_config(PAD23,SPIMOSI);	
	gecko_pinmux_config(PAD20,SPI_CSN_1);	


	//gecko_pinmux_config(PAD24,GPIO_B_4);//default function,do not needed to configure
	gecko_pinmux_config(PAD21,GPIO_A_3);//DC control	
	gecko_pinmux_config(PAD7,GPIOB_7);//RES(reset) control
	
	//LDO33_AUX enable, power supply for LCD module
	LDO33_LCD_Enable();//BL control(BSR1901 use MOS to control backlight)
	

	Lcd_Init();

	
	HW_SPI_Initialise(HAL_SPI_0);	
	DMA_Configuration();	


	//bsr1901_pullup_pulldown_config(PAD_14,PAD_PULLUP);	//BL control	
	//LCD_BL_SET;//turn on backlight
	//Lcd_SetRegion(0, 0, 127, 127);
	//Lcd_Clear(BLACK);
	Lcd_Clear(WHITE);
	
	//Lcd_Fill(0,0,X_MAX_PIXEL,Y_MAX_PIXEL,RED);
	
	Gui_FillCircle(64, 64, 20, C_RED);
	
	delay_1us(8000);	

	HW_SPI_Tx_DMA_32bit((uint16*)gImage_128x128_star_32bit, 8192);	

	delay_1us(10000);	
  //HW_SPI_Tx_DMA_32bit((uint16*)gImage_128x128_cake_32bit, 8192);	

	
	HW_SPI_Tx_DMA_32bit((uint16*)gImage_128x128_charging_32bit, 8192);
	delay_1us(8000);
	
	HW_SPI_Tx_DMA_32bit((uint16*)gImage_128x128_battery_32b, 8192);
	delay_1us(8000);

  uint16_t blue_color = BLACK;//C_TOMATO;//C_BLACK;//C_BLUE;
  lcd_dma_refresh_colorblock(0, 0, X_MAX_PIXEL, Y_MAX_PIXEL,&blue_color);
	delay_1us(8000);
	

	Lcd_SetRegion(20, 39, 109, 87);
	HW_SPI_Tx_DMA((uint16*)gImage_bat_90x49, 8820);

	
	Lcd_SetRegion(10, 10, 109, 109);
	//HW_SPI_Tx_DMA((uint16*)gImage_circle_100x100, 20000);
	HW_SPI_Tx_DMA_8bit((uint16*)gImage_circle_100x100, 20000);	
	delay_1us(8000);
 
	ui_paint_color_circle();
	delay_1us(8000);	
	


/*

    //PWM Charger

		//PAD10 gpio_0_6 as DC_IN detect
		gecko_pinmux_config(PAD10,GPIO_A_6);//config PAD10 as gpio_0_6(A6)	
		GPIO_5V_DCIN_Init();//set GPIO_0_6 as falling edge interrupt
		GPIO_WakeUp_Setting();//GPIO_A_6 wakeup pin	
	
	  ADC_Init();

	  pwm_io_init();
	
    hw_pwm_disable(HW_PWM_CHAN_3);	
	  Set_PWM_CH3_Duty(10);//10% duty
	  hw_pwm_enable(HW_PWM_CHAN_3);

*/




/***************************************************************/
//  config ADC port
//  Channel 5 DO NOT NEEDED TO CONFIG
//	gecko_pinmux_config(PAD20,GPIO_A_2);
//	gecko_pinmux_config(PAD21,GPIO_A_3);
//	bsr1901_adc_8_9_analog_port();
//	ConfigPort_AnalogFunction(ADC0_FUNC|ADC1_FUNC);
//	ADC_Init();

	//initial Check_ON GPIO for ADC function
	//gecko_pinmux_config(PAD9,GPIO_A_5);
	//gpio_set_output(GPIOA, 5);
	//gpio_set_value(GPIOA, 1, 5);//default ouptut HIGH
	
	//GPIO_InitIO(OUTPUT,PA5);
	//GPIO_WriteIO(HIGH, PA5);

/***************************************************************/

	#if  0////charger insert detect
		//config PAD19(GPIOB7) as GPIO input
		gecko_pinmux_config(PAD19,GPIO_B_7);
		extern void Set_GPIO_B7_Input(void);
		Set_GPIO_B7_Input();
		GPIO_InitIO(INPUT,PB7);
	#endif


	#if 0////air flow sensor
		//config PAD10(GPIOA6) as GPIO input
		gecko_pinmux_config(PAD10,GPIO_A_6);
		extern void Set_GPIOA6_Input(void);
		Set_GPIOA6_Input();
		
		GPIO_InitIO(INPUT,PA6);		
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




/************************i2c configure***************************************/

	gecko_pinmux_config(PAD8,GPIO_A_4);//I2C/SDA
	gecko_pinmux_config(PAD9,GPIO_A_5);//I2C/SCL
	gecko_pinmux_config(PAD10,GPIO_A_6);//I2C/INT

  //gpio_set_input(GPIOA,6);
	//GPIO_InitIO(OUTPUT,PA6);
	//GPIODIR_0|= BIT(6);
	//gpio_set_value(GPIOA, 1, 6);
	//GPIO_WriteIO(HIGH, PA6);
	//IP2366_INT_SET;
	//delay_1us(500);

	//Set_GPIOA6_Input();//Hardware pulldown(510K to ground)
	//GPIO_InitIO(INPUT,PA6);	
	//bsr1901_pullup_pulldown_config(PAD_10,PAD_HIZ);
	//bsr1901_pullup_pulldown_config(PAD_10,PAD_PULLDOWN);

	//gpio_i2c_initialize();
	
  //bFlag_init_ipxs = 1;

	
/************************i2c configure***************************************/


	
	#if 0
  printf("date:%s \r\n",gSysDate);
  printf("time:%s \r\n",gSysTime);
	printf("\r\n");
	printf(VERSION);
	printf("Manufacturer Name: %#04x\n\n", MANUFACTURER_NAME);
	printf("Chip Local Name:    %s\n",   CHIP_LOCAL_NAME);
	#endif
	


/************************SCGUI******SCGUI*****SCGUI****************************/

#if 0
	sc_gui_init(lcd_dma_16bit_refresh, 0, C_ROYAL_BLUE, C_BLUE, &lv_font_16);
	sc_clear(0, 0, SC_SCREEN_WIDTH,SC_SCREEN_HEIGHT,gui->bkc);
	
	
	sc_draw_Fill(NULL, 50, 50, 30, 30, C_RED, 255);	


	sc_create_task(0, sc_demo_arc, 2);
	//sc_create_task(0, sc_demo_text, 5);
	
	//sc_create_task(0, sc_demo_DrawEye_tesk, 2);
	//sc_create_task(0, sc_demo_drity_tesk, 2);	
#endif

	Gui_DrawRect(10, 10, 100, 60, RED);
	
	Gui_ProgressBar(20, 20, 100, 20, 60, BLACK, GREEN, GRAY1);
	
/************************SCGUI******SCGUI*****SCGUI****************************/



//	watchdog_init(5000,1);



//iWatchDog for FSM Control

//	  iWDT_Timer_Disable();
//	  iWDT_Timer_Restart();	
//	  iWDT_Timer_Init();

/************************Buck-Boost Control***************************************/
    
  charger_init(&my_charger);
	
/************************Buck-Boost Control***************************************/	




/************************SysTick configure***************************************/
	//----SysTick Init-----
	SysTick_Config(20000);//SysTick === 1ms tick for KEY detect
/************************SysTick configure***************************************/


	while(1)
	{
	
//		sc_task_loop(NULL);		
//		system_tick=TimeTick;//1ms tick
	
		current_tick = TimeTick;
		
		if (current_tick % 10 == 0) Task_KeyScan();
		
		if (current_tick % 100 == 0) Get_Vbat_Voltage();
		
//		if (current_tick % 150 == 0) Task_Charger_Control();// 软件PWM方案	

		if (current_tick % 200 == 0) Task_UI_Refresh();	

		if (current_tick % 250 == 0) Task_BMS_Update();// IP2366 IC方案	
		
	}
	

	
}







