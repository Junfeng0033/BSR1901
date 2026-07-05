#include "eye_blink_m0.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// ===================== 全局配置，完全对齐Python =====================
#define SCREEN_WIDTH        240U
#define SCREEN_HEIGHT       240U
#define EYE_CENTER_X        (SCREEN_WIDTH / 2)
#define EYE_CENTER_Y        (SCREEN_HEIGHT / 2)
#define EYE_W               220U
#define EYE_H               130U
#define IRIS_RADIUS         62U
#define PUPIL_RADIUS        42U
#define UPPER_MOVE_RANGE    ((EYE_H/2) + 20U)
#define LOWER_MOVE_RANGE    ((EYE_H/2) + 15U)

// Q12定点正弦表 替代sin/cos，128点，Q12=4096
#define Q12_SCALE           4096
#define SIN_LUT_LEN         128
static const int16_t sin_q12[SIN_LUT_LEN] = {
0,201,401,600,796,988,1176,1358,1533,1701,1860,2010,2151,2281,2401,2509,
2605,2688,2759,2816,2860,2890,2906,2908,2896,2870,2830,2777,2710,2631,
2540,2437,2323,2199,2065,1923,1773,1616,1453,1284,1112,936,758,579,
399,220,41,-136,-314,-493,-670,-845,-1017,-1185,-1348,-1505,-1655,
-1798,-1932,-2057,-2173,-2279,-2374,-2458,-2530,-2590,-2637,-2671,
-2692,-2699,-2693,-2673,-2640,-2594,-2535,-2464,-2381,-2286,-2181,
-2066,-1942,-1809,-1669,-1522,-1369,-1210,-1047,-880,-710,-538,-366,
-194,-23,147,316,484,649,812,971,1126,1276,1420,1557,1687,1809,1922,
2026,2121,2206,2280,2344,2396,2437,2466,2483,2488,2481,2462,2431,
2389,2335,2270,2195,2110,2016,1913,1801,1682,1556,1424,1286,1144,997,
848,696,543,389,236,83,-69,-220,-370,-518,-663,-806,-944,-1079,-1208,
-1331,-1448,-1558,-1660,-1754,-1840,-1917,-1985,-2044,-2094,-2134,
-2164,-2184,-2195,-2195,-2185,-2165,-2136,-2097,-2049,-1991,-1925,
-1850,-1767,-1676,-1579,-1475,-1365,-1250,-1130,-1006,-878,-748,-615,
-481,-346,-211,-77,57,191,324,454,582,707,829,947,1061,1170,1274,1372,
1464,1549,1627,1698,1761,1817,1864,1904,1936,1959,1974,1981,1979,
1969,1951,1925,1891,1849,1800,1744,1681,1611,1535,1453,1366,1274,
1177,1076,971,863,753,640,526,411,296,181,66,-47,-160,-272,-381,
-488,-592,-693,-790,-884,-974,-1060,-1141,-1218,-1289,-1355,-1415,
-1469,-1516,-1557,-1591,-1618,-1638,-1651,-1657,-1656,-1648,-1633,
-1611,-1583,-1548,-1507,-1460,-1407,-1349,-1286,-1218,-1146,-1070,
-990,-908,-823,-736,-647,-557,-466,-374,-283,-192,-101,-11,78,167,
254,340,424,506,586,663,737,808,876,940,1001,1058,1111,1160,1204,
1244,1279,1309,1334,1354,1368,1377,1380,1378,1370,1357,1338,1314,
1285,1251,1213,1170,1123,1072,1017,959,898,834,768,699,629,557,484,
410,335,260,185,110,36,-37,-110,-182,-253,-322,-390,-456,-520,-582,
-641,-698,-752,-803,-851,-895,-936,-973,-1006,-1036,-1061,-1083,-1100,
-1113,-1122,-1126,-1126,-1121,-1112,-1099,-1081,-1060,-1034,-1005,
-972,-935,-896,-853,-808,-760,-710,-658,-604,-549,-492,-435,-376,
-317,-257,-197,-137,-77,-18,40,97,153,208,262,314,365,414,461,507,
550,591,630,666,700,731,760,785,808,828,845,859,870,878,882,883,
881,875,866,854,839,821,800,777,751,723,693,661,627,592,555,518,
479,440,400,359,318,277,236,196,155,116,77,39,2,-34,-69,-104,-137,
-169,-199,-228,-255,-281,-304,-326,-345,-362,-378,-391,-402,-411,
-417,-421,-423,-422,-419,-413,-405,-395,-383,-369,-353,-336,-317,
-297,-275,-253,-229,-205,-180,-155,-129,-104,-79,-54,-30,-6,17,39,
61,82,102,121,139,156,171,186,199,210,220,229,235,241,244,246,246,
244,240,234,227,218,208,196,184,170,156,140,124,107,90,72,54,36,
18,1,-15,-31,-47,-61,-75,-88,-100,-111,-121,-129,-137,-143,-148,
-152,-154,-155,-154,-152,-148,-143,-137,-130,-122,-112,-102,-90,
-78,-65,-51,-37,-23,-9,4,18,32,45,58,70,82,93,103,112,120,127,133,
138,142,144,146,146,144,142,138,133,127,120,113,104,94,84,73,61,
49,37,24,12,0,-12,-24,-36,-48,-60,-71,-82,-92,-102,-110,-118,-124,
-130,-135,-139,-142,-144,-145,-145,-144,-141,-138,-133,-128,-121,
-114,-105,-96,-86,-75,-64,-52,-40,-28,-16,-4,7,19,31,43,54,65,76,
86,96,105,113,120,126,132,136,140,142,144,145,144,143,140,136,131,
126,119,112,103,94,84,74,63,51,39,27,15,3,-8,-20,-32,-44,-55,-67,
-78,-88,-98,-107,-115,-122,-128,-134,-138,-142,-144,-146,-146,-145,
-143,-139,-135,-129,-123,-115,-107,-98,-88,-77,-66,-54,-42,-30,-17,
-5,7,19,31,43,54,66,77,87,97,106,114,121,128,133,138,141,144,145,
146,145,143,140,136,131,125,118,111,102,93,83,72,61,49,37,25,13,1,
-11,-23,-35,-47,-58,-70,-81,-91,-101,-110,-118,-125,-131,-136,-140,
-143,-145,-146,-145,-144,-141,-137,-132,-126,-119,-111,-102,-92,-82,
-71,-59,-47,-35,-22,-10,1,13,25,37,48,60,71,82,92,102,111,119,126,
132,137,141,144,145,146,145,143,139,135,129,123,115,107,98,88,78,
67,55,43,31,19,7,-4,-16,-28,-40,-52,-63,-75,-86,-96,-106,-114,-122,
-129,-135,-139,-143,-145,-146,-146,-144,-141,-137,-132,-126,-119,-112,
-103,-94,-84,-73,-62,-50,-38,-26,-14,-2,9,21,33,45,56,68,79,90,100,
109,117,124,130,135,140,143,145,146,146,144,141,137,132,126,119,111,
102,92,82,71,60,48,36,24,12,0,-11,-23,-35,-46,-58,-69,-80,-90,-100,
-109,-117,-124,-130,-136,-140,-143,-145,-146,-146,-144,-141,-137,-132,
-126,-119,-111,-102,-92,-82,-71,-59,-47,-35,-22,-10,1,13,25,37,48,
60,71,82,92,102,111,119,126,132,137,141,144,145,146,145,143,139,135,
129,123,115,107,98,88,78,67,55,43,31,19,7,-4,-16,-28,-40,-52,-63,-75,
-86,-96,-106,-114,-122,-129,-135,-139,-143,-145,-146,-146,-144,-141,-137,
-132,-126,-119,-112,-103,-94,-84,-73,-62,-50,-38,-26,-14,-2,9,21,33,
45,56,68,79,90,100,109,117,124,130,135,140,143,145,146,146,144,141,
137,132,126,119,111,102,92,82,71,60,48,36,24,12,0,-11,-23,-35,-46,
-58,-69,-80,-90,-100,-109,-117,-124,-130,-136,-140,-143,-145,-146,-146,
-144,-141,-137,-132,-126,-119,-111,-102,-92,-82,-71,-59,-47,-35,-22,-10
};

// 获取X轴抖动 sin(t/300000) * 10
static int16_t get_jitter_x(uint32_t current_us)
{
    uint32_t idx = (current_us / 300000U) % SIN_LUT_LEN;
    return sin_q12[idx] / (Q12_SCALE / 10);
}
// 获取Y轴抖动 cos(t/250000) *6 = sin(t/250000 + 90°)
static int16_t get_jitter_y(uint32_t current_us)
{
    uint32_t idx = (current_us / 250000U) % SIN_LUT_LEN;
    idx = (idx + SIN_LUT_LEN / 4) % SIN_LUT_LEN;
    return sin_q12[idx] / (Q12_SCALE / 6);
}

// Lightweight Xorshift32 PRNG for Cortex-M0
static uint32_t xorshift_state = 123456789; 
uint32_t fast_rand(void) {
    xorshift_state ^= xorshift_state << 13;
    xorshift_state ^= xorshift_state >> 17;
    xorshift_state ^= xorshift_state << 5;
    return xorshift_state;
}
uint32_t random_range(uint32_t min, uint32_t max) {
    if (min == max) return min;
    return min + (fast_rand() % (max - min + 1));
}

// ===================== 修正眨眼初始化：首次500ms眨眼，对齐Python =====================
void blink_init(EyeBlink_t *eye) {
    eye->state = NOBLINK;
    eye->startTime = 0;
    eye->duration = 0;
    eye->lastBlinkTime = 0;
    // Python初始第一次眨眼：500000us(500ms)
    eye->nextBlinkInterval = 500000U;
}

// ===================== 修正随机时长区间，完全匹配Python =====================
void blink_update(EyeBlink_t *eye, uint32_t currentTimeUs) {
    switch (eye->state) {
        case NOBLINK:
            if (currentTimeUs - eye->lastBlinkTime >= eye->nextBlinkInterval) {
                eye->state = ENBLINK;
                eye->startTime = currentTimeUs;
                // Python闭眼：100000 ~ 150000 us
                eye->duration = random_range(100000U, 150000U);
            }
            break;
        case ENBLINK: // Closing
            if (currentTimeUs - eye->startTime >= eye->duration) {
                eye->state = DEBLINK;
                eye->startTime = currentTimeUs;
                // Python睁眼：120000 ~ 200000 us
                eye->duration = random_range(120000U, 200000U);
            }
            break;
        case DEBLINK: // Opening
            if (currentTimeUs - eye->startTime >= eye->duration) {
                eye->state = NOBLINK;
                eye->lastBlinkTime = currentTimeUs;
                // Python正常间隔：2000000 ~ 4000000 us
                eye->nextBlinkInterval = random_range(2000000U, 4000000U);
            }
            break;
    }
}

uint8_t blink_get_upper_threshold(EyeBlink_t *eye, uint32_t currentTimeUs) {
    if (eye->state == NOBLINK) return 0;
    uint32_t elapsed = currentTimeUs - eye->startTime;
    if (elapsed >= eye->duration) return (eye->state == ENBLINK) ? 254U : 0U;
    uint32_t s = (elapsed * 255U) / eye->duration;
    if (eye->state == ENBLINK) {
        return (uint8_t)((254U * s) / 255U);
    } else {
        return (uint8_t)((254U * (255U - s)) / 255U);
    }
}

uint8_t blink_get_lower_threshold(EyeBlink_t *eye, uint32_t currentTimeUs) {
    uint8_t uT = blink_get_upper_threshold(eye, currentTimeUs);
    return (uint8_t)(uT * 30U / 100U); 
}

bool is_pixel_covered(uint8_t upper_val, uint8_t lower_val, uint8_t uT, uint8_t lT) {
    return (upper_val <= uT) || (lower_val <= lT);
}

extern uint32_t TimeTick;
extern uint32_t Get_SysTick(void);
extern void lcd_dma_refresh(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, uint16_t *color);

// LCD颜色定义 RGB565 匹配Python色值
#define COL_WHITE      0xFFFFU
#define COL_SCLERA     0xF7FFU  // #f5f5ff
#define COL_IRIS       0x7BDEU  // #7b68ee
#define COL_IRIS_OUT   0x483DU  // #483d8b
#define COL_PUPIL      0x0000U
#define COL_SKIN       0x0000U

// 外部LCD基础绘图接口（自行适配硬件）
extern void LCD_FillEllipse(int16_t cx, int16_t cy, int16_t rw, int16_t rh, uint16_t color);
extern void LCD_DrawEllipseOutline(int16_t cx, int16_t cy, int16_t rw, int16_t rh, uint16_t color, uint8_t width);
extern void LCD_FillCircle(int16_t cx, int16_t cy, int16_t r, uint16_t color);
extern void LCD_FillPolygon(int16_t *pts, uint16_t point_cnt, uint16_t fill);
extern void LCD_ClearScreen(uint16_t bg);

// 预生成眼睑基础曲线缓存
#define LID_STEPS 50
static int16_t upper_base[2 * (LID_STEPS + 1)];
static int16_t lower_base[2 * (LID_STEPS + 1)];
// 静态多边形顶点缓冲，优化栈空间占用
static int16_t lid_buf[2 * (LID_STEPS + 1) + 8];

// 上电初始化眼睑曲线（仅调用一次）
void eye_geom_init(void)
{
    for(uint8_t i = 0; i <= LID_STEPS; i++)
    {
        uint32_t lut_idx = (i * SIN_LUT_LEN) / LID_STEPS;
        int16_t sin_val = sin_q12[lut_idx] / Q12_SCALE;
        int16_t x = EYE_CENTER_X - (EYE_W/2) + ((EYE_W * i) / LID_STEPS);
        int16_t y = EYE_CENTER_Y - ((EYE_H / 2) * sin_val);
        upper_base[2*i] = x;
        upper_base[2*i + 1] = y;
    }
    for(uint8_t i = LID_STEPS; i <= LID_STEPS; i--)
    {
        uint32_t lut_idx = (i * SIN_LUT_LEN) / LID_STEPS;
        int16_t sin_val = sin_q12[lut_idx] / Q12_SCALE;
        int16_t x = EYE_CENTER_X - (EYE_W/2) + ((EYE_W * i) / LID_STEPS);
        int16_t y = EYE_CENTER_Y + ((EYE_H / 2) * sin_val);
        lower_base[2*(LID_STEPS - i)] = x;
        lower_base[2*(LID_STEPS - i) + 1] = y;
    }
}

// 完全复刻Python绘图逻辑，移除无用irisScale往复，增加正弦眼球抖动
void drawEye(int eyeIndex, uint16_t iScale, int16_t eyeX, int16_t eyeY,
             uint8_t upperThreshold, uint8_t lowerThreshold, uint32_t current_us)
{
    (void)eyeIndex; (void)iScale; (void)eyeX; (void)eyeY;
    // 1. 计算眼球抖动
    int16_t jit_x = get_jitter_x(current_us);
    int16_t jit_y = get_jitter_y(current_us);
    int16_t iris_cx = EYE_CENTER_X + jit_x;
    int16_t iris_cy = EYE_CENTER_Y + 2 + jit_y;

    // 2. 清屏+巩膜
    LCD_ClearScreen(COL_WHITE);
    LCD_FillEllipse(EYE_CENTER_X, EYE_CENTER_Y, EYE_W/2, EYE_H/2, COL_SCLERA);

    // 3. 虹膜+瞳孔
    LCD_FillEllipse(iris_cx, iris_cy, IRIS_RADIUS, IRIS_RADIUS, COL_IRIS);
    LCD_DrawEllipseOutline(iris_cx, iris_cy, IRIS_RADIUS, IRIS_RADIUS, COL_IRIS_OUT, 1);
    LCD_FillEllipse(iris_cx, iris_cy, PUPIL_RADIUS, PUPIL_RADIUS, COL_PUPIL);

    // 4. 四处高光，坐标完全对齐Python
    LCD_FillCircle(iris_cx + 25, iris_cy - 25, 20, COL_WHITE);
    LCD_FillCircle(iris_cx + 47, iris_cy, 5, COL_WHITE);
    LCD_FillCircle(iris_cx + 38, iris_cy + 15, 3, COL_WHITE);
    LCD_FillCircle(iris_cx - 18, iris_cy + 21, 7, COL_WHITE);

    // 5. 计算眼睑偏移
    int16_t u_offset = (upperThreshold * UPPER_MOVE_RANGE) / 254U;
    int16_t l_offset = (lowerThreshold * LOWER_MOVE_RANGE) / 254U;

    // 上眼睑多边形
    uint16_t ptr = 0;
    for(uint8_t i=0; i <= LID_STEPS; i++)
    {
        lid_buf[ptr++] = upper_base[2*i];
        lid_buf[ptr++] = upper_base[2*i + 1] + u_offset;
    }
    int16_t edge_y = EYE_CENTER_Y - (EYE_H/2) + u_offset;
    lid_buf[ptr++] = EYE_CENTER_X + EYE_W/2 + 50;
    lid_buf[ptr++] = edge_y;
    lid_buf[ptr++] = EYE_CENTER_X + EYE_W/2 + 50;
    lid_buf[ptr++] = EYE_CENTER_Y - EYE_H/2 - 50;
    lid_buf[ptr++] = EYE_CENTER_X - EYE_W/2 - 50;
    lid_buf[ptr++] = EYE_CENTER_Y - EYE_H/2 - 50;
    lid_buf[ptr++] = EYE_CENTER_X - EYE_W/2 - 50;
    lid_buf[ptr++] = edge_y;
    LCD_FillPolygon(lid_buf, ptr/2, COL_SKIN);

    // 下眼睑多边形
    ptr = 0;
    for(uint8_t i=0; i <= LID_STEPS; i++)
    {
        lid_buf[ptr++] = lower_base[2*i];
        lid_buf[ptr++] = lower_base[2*i + 1] - l_offset;
    }
    edge_y = EYE_CENTER_Y + (EYE_H/2) - l_offset;
    lid_buf[ptr++] = EYE_CENTER_X - EYE_W/2 - 50;
    lid_buf[ptr++] = edge_y;
    lid_buf[ptr++] = EYE_CENTER_X - EYE_W/2 - 50;
    lid_buf[ptr++] = EYE_CENTER_Y + EYE_H/2 + 50;
    lid_buf[ptr++] = EYE_CENTER_X + EYE_W/2 + 50;
    lid_buf[ptr++] = EYE_CENTER_Y + EYE_H/2 + 50;
    lid_buf[ptr++] = EYE_CENTER_X + EYE_W/2 + 50;
    lid_buf[ptr++] = edge_y;
    LCD_FillPolygon(lid_buf, ptr/2, COL_SKIN);
}

// 修正updateEye：移除无用往复缩放，传入当前时间生成抖动
void updateEye(EyeBlink_t *eye, uint32_t currentTimeUs) {
    uint8_t uT = blink_get_upper_threshold(eye, currentTimeUs);
    uint8_t lT = blink_get_lower_threshold(eye, currentTimeUs);
    // Python固定虹膜尺寸512，屏幕居中
    drawEye(0, 512, EYE_CENTER_X, EYE_CENTER_Y, uT, lT, currentTimeUs);
}

void frame(EyeBlink_t *eye, uint32_t currentTimeUs) {
    blink_update(eye, currentTimeUs);
    updateEye(eye, currentTimeUs);
}

void loop(EyeBlink_t *eye, uint32_t *simulated_time, uint32_t step_us) {
    uint32_t currentTime = Get_SysTick() * 1000;
    frame(eye, currentTime);
    TimeTick += step_us / 1000;
    *simulated_time = Get_SysTick() * 1000;
}

int eye_blink_main() {
    printf("--- Cortex-M0 与Python完全等效眨眼仿真 ---\n");
    EyeBlink_t myEye;
    blink_init(&myEye);
    eye_geom_init(); // 初始化眼睑曲线，仅一次
    uint32_t simulated_time_us = 0;
    uint32_t loop_step_us = 16000; // Python 16ms刷新
    int total_simulated_seconds = 5;
    int frame_count = 0;
    printf("初始状态: NOBLINK, 首次眨眼500ms\n");
    while (simulated_time_us < (total_simulated_seconds * 1000000)) {
        frame_count++;
        loop(&myEye, &simulated_time_us, loop_step_us);
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
    printf("\n模拟完成，总帧数:%d\n", frame_count);
    return 0;
}
