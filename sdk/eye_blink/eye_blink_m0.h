#ifndef EYE_BLINK_M0_H
#define EYE_BLINK_M0_H

#include <stdint.h>
#include <stdbool.h>

// Configuration
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 240
#define IRIS_MIN      100
#define IRIS_MAX      500

typedef enum {
    NOBLINK = 0,
    ENBLINK, // Closing
    DEBLINK  // Opening
} BlinkState_t;

typedef struct {
    BlinkState_t state;
    uint32_t startTime;
    uint32_t duration;
    uint32_t lastBlinkTime;
    uint32_t nextBlinkInterval;
} EyeBlink_t;

// API
void blink_init(EyeBlink_t *eye);
void blink_update(EyeBlink_t *eye, uint32_t currentTimeUs);
uint8_t blink_get_upper_threshold(EyeBlink_t *eye, uint32_t currentTimeUs);
uint8_t blink_get_lower_threshold(EyeBlink_t *eye, uint32_t currentTimeUs);

// Helper to simulate the rendering check
// returns true if the pixel is covered by the eyelid
bool is_pixel_covered(uint8_t upper_val, uint8_t lower_val, uint8_t uT, uint8_t lT);

#endif // EYE_BLINK_M0_H


