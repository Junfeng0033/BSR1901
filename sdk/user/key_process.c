
#include "key_handle.h"


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

#include "GUI.h"


// Initialize key handle structure
KeyHandle keyHandle = {
    .state = KEY_STATE_IDLE,
    .timestamp = 0,
    .valid_press = false,
    .valid_long_press = false,	
		.key_released_flag = false,
    .press_start = 0
};





void Key_Process(void) 
{
    //uint8_t gpio_status = KP85_KEY2_Detect();
		uint8_t gpio_status = K27_KEY_Detect();
	
	  /*
		key_status=gpio_status & 0x40;
	
		idle state:							gpio_status=0x40
		key press down:					gpio_status=0x00;
		key release: 						gpio_status=0x40;
	  */
	
    bool current_status = ((gpio_status & 0x40) == 0x00); // Current physical key state	
		//bool current_status = ((gpio_status & 0x40) == 0x40); // Current physical key state

	
    switch(keyHandle.state) {
        case KEY_STATE_IDLE:
            if(current_status) // Potential press detected
						{ 
                keyHandle.state = KEY_STATE_PRESS_DETECT;
                keyHandle.timestamp = TimeTick;
            }
            break;

        case KEY_STATE_PRESS_DETECT:
            if(TimeTick - keyHandle.timestamp >= 20) // Debounce for 20ms
						{ 
                if(current_status) // Confirm press
								{ 
                    keyHandle.state = KEY_STATE_PRESSED;
                    keyHandle.press_start = TimeTick; // Record the start time of the press
                } 
								else 
								{
                    keyHandle.state = KEY_STATE_IDLE;
                }
            }
            break;

        case KEY_STATE_PRESSED:
					
					  keyHandle.valid_press = true;
				
            // Detect long - press condition (pressed continuously for more than the threshold)
            if((TimeTick - keyHandle.press_start) >= LONG_PRESS_THRESHOLD) 
						{
								// Key long - press is triggered
								keyHandle.valid_long_press = true;                
            }
						
            // Detect release
            if(!current_status) 
						{ 
                keyHandle.state = KEY_STATE_RELEASE;
                keyHandle.timestamp = TimeTick;
            }
            break;

        case KEY_STATE_RELEASE:
						if(TimeTick - keyHandle.timestamp >= 20)
						{
							if(!current_status) // Confirm release
							{ 
									keyHandle.key_released_flag = true;
								
									keyHandle.state = KEY_STATE_IDLE;
									keyHandle.press_start = 0; // Reset the timer
							} 
							else 
							{
									keyHandle.state = KEY_STATE_PRESSED;
							}
						}
            break;

    }
}





extern uint8 flag_key1;



void Task_KeyScan(void)
{
	
		uint8_t gpio_status;
	
	  uint8_t key_status;

	
/************************key process debug***************************************/
		
	
		//KEY1
		#if 1
//		//PAD11(GPIOA7)
//		gpio_status=K27_KEY_Detect();//default gpio_status=0x80 
//		key_status=gpio_status & 0x80;
	
	  //PAD16(GPIOB4)
		gpio_status=BSR1901_KEY1_Detect();
		key_status=gpio_status & 0x10;	

		if(key_status==0x0)
		{
			Lcd_SetRegion(0, 0, 127, 127);
			if(flag_key1==0)
			{
				//UATR1_PRINT_LOG((unsigned char *)("K27 KEY Press Down"));
        printf("\r\n K27 KEY Press Down \r\n ");
				HW_SPI_Tx_DMA_8bit((uint16*)gImage_128x128_star, 32768);			
				
				flag_key1=1;
			}
			else
			{
				printf("\r\n K27 KEY Press Down Again \r\n ");				
				HW_SPI_Tx_DMA_8bit((uint16*)gImage_128x128_cake, 32768);
					
				flag_key1=0;								
			}
		}
		#endif





		//KEY2
		#if 1	
//		//PAD18(GPIOB6)		
//		gpio_status=KP85_KEY2_Detect();//default gpio_status=0x40 
//		key_status=gpio_status & 0x40;

	  //PAD17(GPIOB5)
		gpio_status=BSR1901_KEY2_Detect();
		key_status=gpio_status & 0x20;			
	
		
		if(key_status==0x0)
		{
			  #if 0
				//LCD_BL_CLR;
				//bsr1901_pullup_pulldown_config(PAD_14,PAD_PULLDOWN);	
        #endif

				//LDO33_AUX disable, power down LCD module			
				LDO33_LCD_Disable();
			
//				wr_data = 0x608e7885;
//				reg_write(0x40020000+0x020, wr_data);
//				
//				wr_data=reg_read(0x40020000+0x000);
//				wr_data |= 0x200;//(set bit10=1)
//				reg_write(0x40020000+0x000, wr_data);
			
        bsr1901_prepare_sleep_for_pin_wakeup();
			  //sleep-wakeup setting
				tc_gecko_cm0_aon_sleep();//deep sleep test for low power design

		}
		#endif
		





		#if 1
		//////////////////mi tou/////////////////////////////////////	
		//PAD10(GPIOA6)
		//PAD10 pulldown

		//bsr1901_pullup_pulldown_config(PAD_10,PAD_PULLUP);
		//bsr1901_pullup_pulldown_config(PAD_10,PAD_PULLDOWN);	

		gpio_status=mi_tou_detect();
		if(gpio_status==0x40)
		{
			//mi_tou_flag=1;//smoking in progress
			printf("\r\n !!!!mi_tou_detect---gpio_a6 HIGH !!! gpio_status = %x",gpio_status);
		}
		////////////////////////////////////////////////////////////////			
		#endif	
		
		
/************************key process debug***************************************/			


}
