
#include "platform_config.h"
#include <stdbool.h>


#define LONG_PRESS_THRESHOLD  1000 				// Long press time threshold in milliseconds


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



