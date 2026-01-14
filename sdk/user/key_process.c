
#include "key_handle.h"


extern uint32_t TimeTick;  								// SysTick counter
extern uint8 KP85_KEY2_Detect(void);
extern uint8 K27_KEY_Detect(void);


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





