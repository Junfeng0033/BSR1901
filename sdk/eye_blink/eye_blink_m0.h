#ifndef EYE_BLINK_M0_H
#define EYE_BLINK_M0_H

#include <stdint.h>
#include <stdbool.h>

#define NOBLINK 0U
#define ENBLINK 1U
#define DEBLINK 2U

typedef struct {
    uint8_t     state;
    uint32_t    startTime;
    uint32_t    duration;
    uint32_t    lastBlinkTime;
    uint32_t    nextBlinkInterval;
} EyeBlink_t;

// 随机数
uint32_t fast_rand(void);
uint32_t random_range(uint32_t min, uint32_t max);

// 眨眼状态机
void blink_init(EyeBlink_t *eye);
void blink_update(EyeBlink_t *eye, uint32_t currentTimeUs);
uint8_t blink_get_upper_threshold(EyeBlink_t *eye, uint32_t currentTimeUs);
uint8_t blink_get_lower_threshold(EyeBlink_t *eye, uint32_t currentTimeUs);
bool is_pixel_covered(uint8_t upper_val, uint8_t lower_val, uint8_t uT, uint8_t lT);

// 几何初始化（上电调用一次）
void eye_geom_init(void);

// 绘制&更新
void drawEye(int eyeIndex, uint16_t iScale, int16_t eyeX, int16_t eyeY,
             uint8_t upperThreshold, uint8_t lowerThreshold, uint32_t current_us);
void updateEye(EyeBlink_t *eye, uint32_t currentTimeUs);
void frame(EyeBlink_t *eye, uint32_t currentTimeUs);
void loop(EyeBlink_t *eye, uint32_t *simulated_time, uint32_t step_us);

// 模拟入口
int eye_blink_main(void);

#endif
