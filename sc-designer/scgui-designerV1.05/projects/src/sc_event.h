#ifndef SC_EVENT_H
#define SC_EVENT_H

/* 直接引根头：自包含（stdbool/stdint/stddef 与 sc_config 配置都经它）。 */
#include "sc_common.h"

#ifndef SC_TOUCH_ENABLE
#define SC_TOUCH_ENABLE 1
#endif

#ifndef SC_MAX_TIMER_COUNT
#define SC_MAX_TIMER_COUNT 8
#endif
#define SC_DEFAULT_EVENT_QUEUE_COUNT 4

typedef uint8_t sc_event_type_t;
/* 0x01-0x0f: system, 0x10-0x1f: touch, 0x20-0x2f: key, >=0x80: user */
enum
{
    SC_EVENT_NONE = 0,
    SC_EVENT_SYS_DRAW = 0x01,
    SC_EVENT_TOUCH_DOWN = 0x10,
    SC_EVENT_TOUCH_UP,
    SC_EVENT_TOUCH_MOVE,
    SC_EVENT_KEY_DOWN = 0x20,
    SC_EVENT_KEY_UP,
    SC_EVENT_USER = 0x80,
    SC_EVENT_KEYBOARD_INPUT
};

#define CMD_NONE 0
#define CMD_A 4  /* A-Z: 0x04-0x1d */
#define CMD_Z 29
#define CMD_1 30 /* 1-9,0: 0x1e-0x27 */
#define CMD_0 39

#define CMD_ENTER 40
#define CMD_BREAK 41   /* 边界返回信号：左上角 / 格子最左上继续向外 */
#define CMD_BACK 42
#define CMD_TAB 43     /* 边界翻页信号：右下角 / 格子最右下继续向外 */
#define CMD_DEL 76
#define CMD_RIGHT 79
#define CMD_LEFT 80
#define CMD_DOWN 81
#define CMD_UP 82

typedef struct sc_event_t
{
    uint8_t type;        /* event type */
    uint8_t user_type;   
    union
    {
        int16_t pos[2]; /* touch: x,y */
        int cmd;        /* system/user command */
        int key;        /* key code */
        void *arg;      /* draw pfb or user pointer */
    } dat;
} sc_event_t;

typedef struct sc_event_queue_t
{
    sc_event_t items[SC_DEFAULT_EVENT_QUEUE_COUNT];
    uint8_t head; /* read index */
    uint8_t tail; /* write index */
} sc_event_queue_t;

bool sc_event_post(const sc_event_t *event);
bool sc_event_pop(sc_event_t *out);
void sc_event_touch(uint8_t type, int16_t x, int16_t y);
void sc_systick_inc(void);

uint32_t sc_systick_get(void);

typedef struct sc_delay_t
{
    void *arg;
    uint32_t time_out;
    void (*cb)(struct sc_delay_t *);
} sc_delay_t;

int sc_add_delay(void (*cb)(sc_delay_t *), uint32_t ms, void *arg);

void sc_reload_delay(sc_delay_t *self, uint32_t ms);

void sc_stop_delay(uint8_t id);

void sc_timer_loop(void);

#endif
