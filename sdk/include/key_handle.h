
#include "platform_config.h"
#include <stdbool.h>




// Key state machine definition
typedef enum {
    KEY_STATE_IDLE,          		// Idle state
    KEY_STATE_PRESS_DETECT,  		// Press debounce detection
    KEY_STATE_PRESSED,       		// Confirmed press (waiting for release or long press trigger)
    KEY_STATE_RELEASE 	
} KeyState;




// Key handle structure
typedef struct {
    KeyState state;          // Current state
    uint32_t timestamp;      // Timestamp record
    bool valid_press;
    bool valid_long_press;
	  bool key_released_flag;	
    uint32_t press_start;    // Press start time (for long press timing)
} KeyHandle;





#define LONG_PRESS_THRESHOLD  1000 				// Long press time threshold in milliseconds


extern uint32_t TimeTick;  								// SysTick counter


extern uint8 KP85_KEY2_Detect(void);
extern uint8 K27_KEY_Detect(void);
extern uint8 mi_tou_detect(void);
extern void bsr1901_prepare_sleep_for_pin_wakeup(void);






extern unsigned int gImage_128x128_charging_32bit[8192];
extern const unsigned int gImage_128x128_battery_32b[8192];

extern unsigned int gImage_128x128_star_32bit[8192];
extern unsigned int gImage_128x128_cake_32bit[8192];
extern  unsigned char gImage_128x128_cake[32768];
extern  unsigned char gImage_128x128_star[32768];
extern const unsigned char gImage_128x128_battery[32768];

extern const unsigned char gImage_circle_100x100[20000];
extern const unsigned char gImage_black_128x128[32768];


extern const unsigned char gImage_charging[32768];


