#include "eye_blink_m0.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>




// Lightweight Xorshift32 PRNG for Cortex-M0
// Avoids the overhead of standard rand() and provides sufficient randomness
static uint32_t xorshift_state = 123456789; 
uint32_t fast_rand(void) {
    xorshift_state ^= xorshift_state << 13;
    xorshift_state ^= xorshift_state >> 17;
    xorshift_state ^= xorshift_state << 5;
    return xorshift_state;
}

// Helper for range-based random: [min, max]
uint32_t random_range(uint32_t min, uint32_t max) {
    if (min == max) return min;
    return min + (fast_rand() % (max - min + 1));
}

void blink_init(EyeBlink_t *eye) {
    eye->state = NOBLINK;
    eye->startTime = 0;
    eye->duration = 0;
    eye->lastBlinkTime = 0;
    eye->nextBlinkInterval = random_range(2000000, 5000000); // 2-5 seconds in microseconds
}

void blink_update(EyeBlink_t *eye, uint32_t currentTimeUs) {
    switch (eye->state) {
        case NOBLINK:
            if (currentTimeUs - eye->lastBlinkTime >= eye->nextBlinkInterval) {
                eye->state = ENBLINK;
                eye->startTime = currentTimeUs;
                // Blink duration: 100ms to 200ms
                eye->duration = random_range(100000, 200000);
            }
            break;

        case ENBLINK: // Closing
            if (currentTimeUs - eye->startTime >= eye->duration) {
                eye->state = DEBLINK;
                eye->startTime = currentTimeUs;
                // Opening typically takes slightly longer than closing
                eye->duration = random_range(120000, 250000);
            }
            break;

        case DEBLINK: // Opening
            if (currentTimeUs - eye->startTime >= eye->duration) {
                eye->state = NOBLINK;
                eye->lastBlinkTime = currentTimeUs;
                eye->nextBlinkInterval = random_range(2000000, 6000000);
            }
            break;
    }
}

/**
 * Optimized threshold calculation for Cortex-M0
 * Original formula: n = (uThreshold * s + 254 * (257 - s)) / 256
 * Optimized: 
 * 1. Uses only integer arithmetic.
 * 2. Division by 256 is replaced by compiler with >> 8.
 * 3. Linear interpolation for s (0-255).
 */
uint8_t blink_get_upper_threshold(EyeBlink_t *eye, uint32_t currentTimeUs) {
    if (eye->state == NOBLINK) return 0;

    // Calculate progress s (0 to 255)
    uint32_t elapsed = currentTimeUs - eye->startTime;
    if (elapsed >= eye->duration) return (eye->state == ENBLINK) ? 254 : 0;

    // s = (elapsed * 255) / duration
    uint32_t s = (elapsed * 255) / eye->duration;

    if (eye->state == ENBLINK) {
        // Closing: Threshold increases from 0 to 254
        // Formula: n = (254 * s) / 255
        return (uint8_t)((254 * s) / 255);
    } else {
        // Opening: Threshold decreases from 254 to 0
        // Formula: n = (254 * (255 - s)) / 255
        return (uint8_t)((254 * (255 - s)) / 255);
    }
}

uint8_t blink_get_lower_threshold(EyeBlink_t *eye, uint32_t currentTimeUs) {
    // Lower eyelid often moves less or mirrors the upper
    // Here we implement a simplified mirror effect: lower eyelid moves 30% as much
    uint8_t uT = blink_get_upper_threshold(eye, currentTimeUs);
    return (uint8_t)(uT * 30 / 100); 
}

bool is_pixel_covered(uint8_t upper_val, uint8_t lower_val, uint8_t uT, uint8_t lT) {
    // CPU-efficient check: just two comparisons
    // If the pixel value in the threshold map is <= current threshold, it's covered
    return (upper_val <= uT) || (lower_val <= lT);
}




#if 0

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "eye_blink_m0.h"

// 全局时间计数器 (毫秒)
uint32_t TimeTick = 0;

// 系统 SysTick 函数 (1ms 计时)
uint32_t Get_SysTick(void)
{
	return TimeTick;//1ms tick
}


#endif




extern uint32_t TimeTick;



extern uint32_t Get_SysTick(void);



extern void lcd_dma_refresh(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, uint16_t *color);


// 模拟 TFT 显示屏绘制函数
// 参数说明:
// - eyeIndex: 眼睛索引 (0=左眼, 1=右眼, 可用于多眼系统)
// - iScale: 虹膜缩放比例 (0-1023, 0=最小, 1023=最大)
// - eyeX: 眼睛X坐标 (0-239, 屏幕宽度240像素)
// - eyeY: 眼睛Y坐标 (0-239, 屏幕高度240像素) 
// - upperThreshold: 上眼睑阈值 (0-255, 控制上眼睑位置)
// - lowerThreshold: 下眼睑阈值 (0-255, 控制下眼睑位置)
//
// 使用示例:
// drawEye(0, 512, 120, 120, 200, 100);  // 绘制左眼，虹膜中等大小，居中位置，上眼睑200，下眼睑100
// drawEye(1, 800, 180, 100, 150, 80);   // 绘制右眼，虹膜较大，右上位置，上眼睑150，下眼睑80
void drawEye(int eyeIndex, uint16_t iScale, int16_t eyeX, int16_t eyeY,
             uint8_t upperThreshold, uint8_t lowerThreshold) {
    // ------------------------------------------------------------
    // 为了在 SRAM 仅 4 KB 的 Cortex‑M0 上运行，这里不再使用
    // 大的全局像素缓冲区，而是采用行缓冲的方式逐行刷新。
    // 每行使用一个局部 uint16_t 数组（最大宽度 120 → 240 字节），
    // 生成该行的 16 bit RGB565 像素后立即调用
    // lcd_dma_refresh() 完成 DMA 传输。这样整个函数在栈上
    // 只占用约 300 字节，符合 4 KB 限制。
    // ------------------------------------------------------------

    // 固定绘制尺寸（可根据实际需求修改）
    const uint16_t EYE_W = 120;
    const uint16_t EYE_H = 120;

    // ------------------- 参数合法性检查 & 裁剪 -------------------
    if (eyeX < 0) eyeX = 0;
    if (eyeY < 0) eyeY = 0;
    if (eyeX + EYE_W > 240) eyeX = 240 - EYE_W;
    if (eyeY + EYE_H > 240) eyeY = 240 - EYE_H;

#if 0

    // 行缓冲区，最大 120 像素 → 240 字节（每像素 2 字节）
    uint16_t line_buf[120];


    // --------------------- 逐行渲染 ---------------------
    for (uint16_t row = 0; row < EYE_H; ++row) {
        uint16_t pixels = 0;
        // 以下代码摘自原来的实现，仅把写入 pbuffer 的逻辑改为写入行缓冲
        uint32_t screenY = eyeY + row;
        int32_t  irisX = 0;   // 这里在实际实现中应根据 scleraX、irisX 计算，简化示例
        int32_t  irisY = row - (SCLERA_HEIGHT - IRIS_HEIGHT) / 2;
        uint32_t p = 0, a = 0, d = 0;

        // 简化的阈值判断
        uint8_t upper_val = pgm_read_byte(upper + screenY * SCREEN_WIDTH + eyeX);
        uint8_t lower_val = pgm_read_byte(lower + screenY * SCREEN_WIDTH + eyeX);
        if ((upper_val <= upperThreshold) || (lower_val <= lowerThreshold)) {
            p = 0; // 被眼睑覆盖 → 黑色
        } else {
            // 此处使用原始的虹膜 / 巩膜渲染逻辑（已摘录）
            if ((irisY < 0) || (irisY >= IRIS_HEIGHT) ||
                (irisX < 0) || (irisX >= IRIS_WIDTH)) {
                p = pgm_read_word(sclera + screenY * SCLERA_WIDTH + eyeX);
            } else {
                p = pgm_read_word(polar + irisY * IRIS_WIDTH + irisX);
                d = (iScale * (p & 0x7F)) / 128;
                if (d < IRIS_MAP_HEIGHT) {
                    a = (IRIS_MAP_WIDTH * (p >> 7)) / 512;
                    p = pgm_read_word(iris + d * IRIS_MAP_WIDTH + a);
                } else {
                    p = pgm_read_word(sclera + screenY * SCLERA_WIDTH + eyeX);
                }
            }
        }
        // 字节序翻转并写入行缓冲
        line_buf[pixels++] = (p >> 8) | (p << 8);

        // 将本行像素通过 DMA 刷新到 LCD
        lcd_dma_refresh((uint16_t)eyeX, (uint16_t)screenY,
                        (uint16_t)EYE_W, (uint16_t)1,
                        line_buf);
    }


#endif

		
}



#if 0

void drawEye( // Renders one eye.  Inputs must be pre-clipped & valid.
  // Use native 32 bit variables where possible as this is 10% faster!
  uint8_t  e,       // Eye array index; 0 or 1 for left/right
  uint32_t iScale,  // Scale factor for iris
  uint32_t  scleraX, // First pixel X offset into sclera image
  uint32_t  scleraY, // First pixel Y offset into sclera image
  uint32_t  uT,      // Upper eyelid threshold value
  uint32_t  lT) {    // Lower eyelid threshold value

  uint32_t  screenX, screenY, scleraXsave;
  int32_t  irisX, irisY;
  uint32_t p, a;
  uint32_t d;

  uint32_t pixels = 0;

  // Set up raw pixel dump to entire screen.  Although such writes can wrap
  // around automatically from end of rect back to beginning, the region is
  // reset on each frame here in case of an SPI glitch.
  digitalWrite(eye[e].tft_cs, LOW);
  tft.startWrite();
  tft.setAddrWindow(eye[e].xposition, 0, 240, 240);

  // Now just issue raw 16-bit values for every pixel...

  scleraXsave = scleraX; // Save initial X value to reset on each line
  irisY       = scleraY - (SCLERA_HEIGHT - IRIS_HEIGHT) / 2;

  // Eyelid image is left<>right swapped for two displays
  uint16_t lidX = 0;
  uint16_t dlidX = -1;
  if (e) dlidX = 1;
  for (screenY = 0; screenY < SCREEN_HEIGHT; screenY++, scleraY++, irisY++) {
    scleraX = scleraXsave;
    irisX   = scleraXsave - (SCLERA_WIDTH - IRIS_WIDTH) / 2;
    if (e) lidX = 0; else lidX = SCREEN_WIDTH - 1;
    for (screenX = 0; screenX < SCREEN_WIDTH; screenX++, scleraX++, irisX++, lidX += dlidX) {
      if ((pgm_read_byte(lower + screenY * SCREEN_WIDTH + lidX) <= lT) ||
          (pgm_read_byte(upper + screenY * SCREEN_WIDTH + lidX) <= uT)) {              // Covered by eyelid
        p = 0;
      } else if ((irisY < 0) || (irisY >= IRIS_HEIGHT) ||
                 (irisX < 0) || (irisX >= IRIS_WIDTH)) { // In sclera
        p = pgm_read_word(sclera + scleraY * SCLERA_WIDTH + scleraX);
      } else {                                          // Maybe iris...
        p = pgm_read_word(polar + irisY * IRIS_WIDTH + irisX);                        // Polar angle/dist
        d = (iScale * (p & 0x7F)) / 128;                // Distance (Y)
        if (d < IRIS_MAP_HEIGHT) {                      // Within iris area
          a = (IRIS_MAP_WIDTH * (p >> 7)) / 512;        // Angle (X)
          p = pgm_read_word(iris + d * IRIS_MAP_WIDTH + a);                           // Pixel = iris
        } else {                                        // Not in iris
          p = pgm_read_word(sclera + scleraY * SCLERA_WIDTH + scleraX);               // Pixel = sclera
        }
      }
      *(&pbuffer[dmaBuf][0] + pixels++) = p >> 8 | p << 8;

      if (pixels >= BUFFER_SIZE) {
        yield();
#ifdef USE_DMA
        tft.pushPixelsDMA(&pbuffer[dmaBuf][0], pixels);
        dmaBuf  = !dmaBuf;
#else
        tft.pushPixels(pbuffer, pixels);
#endif
        pixels = 0;
      }
    }
  }

  if (pixels) {
#ifdef USE_DMA
    tft.pushPixelsDMA(&pbuffer[dmaBuf][0], pixels);
#else
    tft.pushPixels(pbuffer, pixels);
#endif
  }
  tft.endWrite();
  digitalWrite(eye[e].tft_cs, HIGH);
}

#endif


// 模拟 updateEye 函数 - 处理虹膜缩放和眼睛位置
void updateEye(EyeBlink_t *eye, uint32_t currentTimeUs) {
    // 模拟虹膜缩放 (0-1023)
    static uint16_t irisValue = 512;
    static int32_t irisDirection = 1;
    
    // 随机改变虹膜大小
    irisValue += irisDirection * 10;
    if (irisValue >= 1023 || irisValue <= 0) {
        irisDirection *= -1;
    }
    
    // 模拟眼睛位置 (0-1023)
    static int16_t eyeX = 512, eyeY = 512;
    static int32_t xDirection = 1, yDirection = 1;
    
    eyeX += xDirection * 5;
    eyeY += yDirection * 3;
    
    if (eyeX >= 1023 || eyeX <= 0) xDirection *= -1;
    if (eyeY >= 1023 || eyeY <= 0) yDirection *= -1;
    
    // 获取当前眨眼阈值
    uint8_t uT = blink_get_upper_threshold(eye, currentTimeUs);
    uint8_t lT = blink_get_lower_threshold(eye, currentTimeUs);
    
    // 映射眼睛位置到屏幕坐标
    int16_t screenX = (eyeX * 240) / 1023;
    int16_t screenY = (eyeY * 240) / 1023;
    
    // 调用 drawEye 函数 (模拟实际硬件绘制)
    drawEye(0, irisValue, screenX, screenY, uT, lT);
}

// 模拟 frame 函数 - 处理单帧动画
void frame(EyeBlink_t *eye, uint32_t currentTimeUs) {
    // 更新眨眼状态
    blink_update(eye, currentTimeUs);
    
    // 调用 updateEye 进行实际绘制
    updateEye(eye, currentTimeUs);
}

// 模拟 loop 函数 - 主循环
void loop(EyeBlink_t *eye, uint32_t *simulated_time, uint32_t step_us) {
    uint32_t currentTime = Get_SysTick() * 1000; // 将毫秒转换为微秒
    
    // 处理当前帧
    frame(eye, currentTime);
    
    // 模拟时间流逝 (使用 SysTick 计数)
    TimeTick += step_us / 1000; // 将微秒转换为毫秒计数
    
    // 更新模拟时间
    *simulated_time = Get_SysTick() * 1000; // 将毫秒转换为微秒
}







int eye_blink_main() {
    printf("--- Cortex-M0 完整眼睛动画模拟开始 ---\n");
    printf("模拟完整的调用链: loop() -> frame() -> updateEye() -> drawEye()\n\n");

    // 初始化眨眼状态结构体
    EyeBlink_t myEye;
    blink_init(&myEye);

    uint32_t simulated_time_us = 0;
    uint32_t loop_step_us = 50000; // 每次循环模拟增加 50ms
    int total_simulated_seconds = 5; // 模拟运行 5 秒
    int frame_count = 0;

    printf("初始状态: NOBLINK, 下次眨眼间隔: %u us\n", myEye.nextBlinkInterval);

    // 模拟主循环
    while (simulated_time_us < (total_simulated_seconds * 1000000)) {
        frame_count++;
        
        // 执行完整的主循环
        loop(&myEye, &simulated_time_us, loop_step_us);
        
        // 每10帧打印一次状态信息
        if (frame_count % 10 == 0) {
            const char* state_str = "NOBLINK";
            if (myEye.state == ENBLINK) state_str = "CLOSING";
            if (myEye.state == DEBLINK) state_str = "OPENING";
            
            uint8_t uT = blink_get_upper_threshold(&myEye, simulated_time_us);
            uint8_t lT = blink_get_lower_threshold(&myEye, simulated_time_us);
            
            printf("[%7u us] 状态: %-8s | 上阈值: %3u | 下阈值: %3u\n", 
                   simulated_time_us, state_str, uT, lT);
        }
    }

    printf("\n--- 模拟完成 ---\n");
    printf("总帧数: %d, 模拟时间: %d 秒\n", frame_count, total_simulated_seconds);
    return 0;
}






