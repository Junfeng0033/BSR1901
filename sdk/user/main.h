#ifndef _M_H_
#define _M_H_




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

#include "ui_app.h"

#include "sc_demo_test.h"
#include "sc_common.h"
#include "lvgl.h"

#include "sc_gui.h"

#include "charger.h"

#include "key_handle.h"
//#include "bms_global.h"


//#define KEY_PIN  (PA7)

//#define POINT_NUM (1024/2)



volatile uint16 ram_color=0x1234;
//uint16 ram_color=0;
uint8 flag_key1=0;
uint8 flag_key2=0;

extern int gecko_efuse_read(void);

extern charger_manager_t my_charger;

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

extern void Task_BMS_Update(void);

extern uint8 K27_KEY_Detect(void);
	
extern void ui_paint_color_circle(void);

extern void ip2366_reg_write(void);
extern void ip2366_reg_read(void);

extern void bsr1901_prepare_sleep_for_pin_wakeup(void);

extern void GPIO_5V_DCIN_Init(void);

extern void delay_1us(unsigned int delay_val);

extern uint32 bsr1901_sram_test(void);

//extern void BSR1901_MOS_Gate_CTRL(void);

extern void gecko_task_cm0_sw_flash(void);
extern void gecko_task_cm0_sw_flash_quad(void);


extern void bsr1901_cm0_lite_sleep(void);
	

extern void OP_Amp_Config(void);
extern int reg_read(int addr);
extern void reg_write(int addr, int data);

extern void sram_gecko_task_cm0_sw_flash(void);

extern void ui_paint_bat_percent(uint8_t percent);


extern void BSR1901_MOS_Enable(void);
extern void BSR1901_MOS_Disable(void);

extern void Gui_Draw_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Color);

extern void Gui_Circle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc);
extern void Gui_FillCircle(uint16_t X, uint16_t Y, uint16_t R, uint16_t fc);

extern void Gui_DrawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t Color);
extern void Gui_DrawFillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);

extern void Gui_ShowString_Transparent(uint16_t x, uint16_t y, uint16_t fc, uint8_t *s);


extern unsigned int gImage_128x128_charging_32bit[8192];
extern const unsigned int gImage_128x128_battery_32b[8192];

extern unsigned int gImage_128x128_star_32bit[8192];
extern unsigned int gImage_128x128_cake_32bit[8192];
extern  unsigned char gImage_128x128_cake[32768];
extern  unsigned char gImage_128x128_star[32768];

extern  const unsigned char gImage_128x128_battery[32768];


extern const unsigned char gImage_circle_100x100[20000];
extern const unsigned char gImage_black_128x128[32768];


extern const unsigned char gImage_charging[32768];

extern const unsigned int img_watch_2_240x240[28800];

extern uint16 Get_Vbat_Voltage(void);

extern void Task_KeyScan(void);

extern void Task_UI_Refresh(void);

extern uint8 gpio_i2c_initialize(VOID);

extern KeyHandle keyHandle;
extern void Key_Process(void);

extern void charger_init(charger_manager_t *charger);
extern void charger_process(charger_manager_t *charger);

extern lv_font_t lv_font_16; 


extern void lcd_dma_refresh_colorblock(uint16_t xs, uint16_t ys, uint16_t xend,uint16_t yend, color_t *color);


extern void GPIO_Toggle(uint8 pin);


extern void pad20_as_spi_csn1(void);

extern void pad21_as_spi_miso(void);
extern void pad21_as_gpio_a3(void);

extern void pad22_as_spi_clk(void);
extern void pad23_as_spi_mosi(void);
extern void pad24_as_spi_csn2(void);


extern void EnterDeepSleepMode(void);

#endif


