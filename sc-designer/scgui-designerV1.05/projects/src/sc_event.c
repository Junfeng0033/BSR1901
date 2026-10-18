#include "sc_event.h"

static volatile uint32_t g_systick_ms = 0;
static sc_event_queue_t g_event_queue;
static sc_delay_t g_delay_buf[SC_MAX_TIMER_COUNT];

/* Increment the millisecond system tick counter; call from the SysTick interrupt. */
void sc_systick_inc(void)
{
    g_systick_ms += 1U;
}

/* Return the current millisecond system tick count. */
uint32_t sc_systick_get(void)
{
    return g_systick_ms;
}

/* Push an event onto the queue, coalescing consecutive touch-move events; false if full. */
bool sc_event_post(const sc_event_t *event)
{
#if SC_TOUCH_ENABLE
    uint8_t last;
#endif
    if (!event || event->type == SC_EVENT_NONE)
    {
        return false;
    }
#if SC_TOUCH_ENABLE
    /* Preserve DOWN/UP order; only replace the newest pending MOVE. */
    if (event->type == SC_EVENT_TOUCH_MOVE && g_event_queue.head != g_event_queue.tail)
    {
        last = g_event_queue.tail == 0U ? (SC_DEFAULT_EVENT_QUEUE_COUNT - 1U) : (uint8_t)(g_event_queue.tail - 1U);
        if (g_event_queue.items[last].type == SC_EVENT_TOUCH_MOVE)
        {
            g_event_queue.items[last] = *event;
            return true;
        }
    }
#endif
    uint8_t next = (uint8_t)(g_event_queue.tail + 1U);
    if (next >= SC_DEFAULT_EVENT_QUEUE_COUNT)
    {
        next = 0;
    }
    if (next == g_event_queue.head)
    {
        return false; /* full */
    }
    g_event_queue.items[g_event_queue.tail] = *event;
    g_event_queue.tail = next;
    return true;
}

/* Pop the oldest queued event into out; false if the queue is empty. */
bool sc_event_pop(sc_event_t *out)
{
    if (!out)
    {
        return false;
    }
    if (g_event_queue.head == g_event_queue.tail)
    {
        return false; /* empty */
    }
    *out = g_event_queue.items[g_event_queue.head];
    g_event_queue.head++;
    if (g_event_queue.head >= SC_DEFAULT_EVENT_QUEUE_COUNT)
    {
        g_event_queue.head = 0;
    }
    return true;
}
void sc_event_touch(uint8_t type, int16_t x, int16_t y )
{
    sc_event_t event;
    event.type= type;
    event.dat.pos[0]=x;
    event.dat.pos[1]=y;
    sc_event_post(&event); 
}
/* Schedule (or reschedule) a timer callback after ms; returns its id or -1 on failure.
 * Reuse key = (cb, arg): same cb + same arg resets the same slot (e.g. repeat timers on
 * one obj); same cb + different arg gets an independent slot (independent anim chains). */
int sc_add_delay(void (*cb)(sc_delay_t *), uint32_t ms, void *arg)
{
    int id;
    sc_delay_t *delay = NULL;
    if (cb == NULL)
    {
        return -1;
    }
    /* Use int index: uint8_t would wrap when SC_MAX_TIMER_COUNT == 255. */
    for (id = 0; id < (int)SC_MAX_TIMER_COUNT; id++)
    {
        if (g_delay_buf[id].cb == cb && g_delay_buf[id].arg == arg)
        {
            delay = &g_delay_buf[id];
            break;
        }
    }
    if (delay == NULL)
    {
        for (id = 0; id < (int)SC_MAX_TIMER_COUNT; id++)
        {
            if (g_delay_buf[id].cb == NULL)
            {
                delay = &g_delay_buf[id];
                break;
            }
        }
    }
    if (delay == NULL)
    {
        return -1;
    }
    delay->cb = cb;
    delay->arg = arg;
    delay->time_out = g_systick_ms + ms;
    return id;
}

/* Restart a timer's timeout to ms from now, keeping it active (repeating). */
void sc_reload_delay(sc_delay_t *self, uint32_t ms)
{
    if (!self)
    {
        return;
    }
    self->time_out = g_systick_ms + ms;
}

/* Cancel and clear the timer slot with the given id. */
void sc_stop_delay(uint8_t id)
{
    if (id >= SC_MAX_TIMER_COUNT)
    {
        return;
    }
    g_delay_buf[id].cb = NULL;
    g_delay_buf[id].arg = NULL;
    g_delay_buf[id].time_out = 0;
}

/* Fire all expired timers once per tick; auto-clears one-shot timers that were not reloaded. */
void sc_timer_loop(void)
{
    static uint32_t last_tick;
    uint32_t now = g_systick_ms;
    uint32_t time_out;
    sc_delay_t *delay;
    if (now == last_tick)
    {
        return;
    }
    last_tick = now;
    for (int id = 0; id < (int)SC_MAX_TIMER_COUNT; id++)
    {
        delay = &g_delay_buf[id];
        if (!delay->cb)
        {
            continue;
        }
        time_out = delay->time_out;
        if ((int32_t)(now - time_out) < 0)
        {
            continue;
        }
        delay->cb(delay);
        if (time_out == delay->time_out)
        {
            delay->cb = NULL;
            delay->arg = NULL;
            delay->time_out = 0;
        }
    }
}
