#include "sc_anim.h"

#if SC_USER_OBJ

/* 静态动画池：obj=NULL 表示空闲槽，无需 init。 */
static sc_anim_t g_anim_pool[SC_ANIM_POOL_SIZE];

/* 正弦近似：sin(π/2 * t)，t∈[0,255]→[0,255]，多项式 1.5t-0.5t³，误差<2%。 */
static uint16_t sc_anim_sin_q8(uint16_t t_q8)
{
    uint16_t t2 = (uint16_t)((uint32_t)t_q8 * t_q8 >> 8);
    uint16_t t3 = (uint16_t)((uint32_t)t2 * t_q8 >> 8);
    return (uint16_t)((3 * t_q8 - t3) >> 1);
}

/* 写入 xy 组当前值到 obj->view。MOVE_XY 模式下同时遍历移动子控件（扁平树 level 比较）。 */
static void sc_anim_set_cur(sc_anim_t *anim, int16_t x, int16_t y)
{
    if ((anim->type & SC_ANIM_TYPE_MASK) == SC_ANIM_MOVE_XY)
    {
        int16_t dx = (int16_t)(x - anim->obj->view.x);
        int16_t dy = (int16_t)(y - anim->obj->view.y);
        anim->obj->view.x = x;
        anim->obj->view.y = y;
        if (dx != 0 || dy != 0)
        {
            uint8_t level = sc_obj_level(anim->obj);
            uint8_t cnt = 0;
            for (sc_obj_t *cur = anim->obj->next; cur && sc_obj_level(cur) > level; cur = cur->next)
            {
                cur->view.x = (int16_t)(cur->view.x + dx);
                cur->view.y = (int16_t)(cur->view.y + dy);
                cnt++;
            }
        }
    }
    else
    {
        anim->obj->view.w = (uint16_t)x;
        anim->obj->view.h = (uint16_t)y;
    }
}

/* 吸收外部对 view 的移动（容器滚动 / sc_obj_move 等）：
 * 上一帧动画写后记录 last_x/y，本帧 view 与 last 的差值即外部偏移，
 * 加到 from/target 上使动画轨迹跟随外部移动，避免被动画拉回旧轨迹。
 * 仅 MOVE_XY 类型需要；SCALE_WH/ALPHA 不受位置移动影响。 */
static void sc_anim_absorb_external_move(sc_anim_t *anim)
{
    if ((anim->type & SC_ANIM_TYPE_MASK) != SC_ANIM_MOVE_XY)
        return;
    int16_t dx = (int16_t)(anim->obj->view.x - anim->last_x);
    int16_t dy = (int16_t)(anim->obj->view.y - anim->last_y);
    if (dx != 0 || dy != 0)
    {
        anim->data.xy.from_x = (int16_t)(anim->data.xy.from_x + dx);
        anim->data.xy.from_y = (int16_t)(anim->data.xy.from_y + dy);
        anim->data.xy.target_x = (int16_t)(anim->data.xy.target_x + dx);
        anim->data.xy.target_y = (int16_t)(anim->data.xy.target_y + dy);
    }
}

/* repeat 消耗：返回 true=动画结束；false=还有下一轮循环。 */
static bool sc_anim_repeat_done(sc_anim_t *anim)
{
    if (anim->repeat == 0)
        return true;
    if (anim->repeat != 0xFF)
        anim->repeat--;
    return (anim->repeat == 0);
}

/* 循环：交换 alpha from↔target（往复）。 */
static void sc_anim_swap_alpha(sc_anim_t *anim)
{
    int16_t tmp = anim->data.alpha.target;
    anim->data.alpha.target = anim->data.alpha.from;
    anim->data.alpha.from = tmp;
}

/* 循环：交换 xy from↔target（往复），当前位置设为新的 from。 */
static void sc_anim_swap_xy(sc_anim_t *anim)
{
    int16_t tmp = anim->data.xy.from_x;
    anim->data.xy.from_x = anim->data.xy.target_x;
    anim->data.xy.target_x = tmp;
    tmp = anim->data.xy.from_y;
    anim->data.xy.from_y = anim->data.xy.target_y;
    anim->data.xy.target_y = tmp;
    sc_anim_set_cur(anim, anim->data.xy.from_x, anim->data.xy.from_y);
}


/* ===== 时间模式核心：所有曲线共用，state=当前帧数，duration=总帧数。
 * 内部自包含 invalidate（旧位置开头刷，新位置 set_cur 后刷；ALPHA 位置不变只刷一次）。 */
static bool sc_anim_tick_timed_core(sc_anim_t *anim, uint8_t frames)
{
    uint8_t curve = anim->type & SC_ANIM_CURVE_MASK;
    /* tick 前 invalidate 旧位置（ALPHA 位置不变，只需刷这一次） */
    sc_obj_invalidate_area(anim->obj, NULL);
    anim->state = (uint8_t)(anim->state + frames);
    if (anim->state > anim->duration)
        anim->state = anim->duration;
    /* t = 当前帧 / 总帧，Q8 格式 [0,255] */
    uint16_t t_q8 = (uint16_t)((uint32_t)anim->state * 255U / anim->duration);
    switch (curve)
    {
    case SC_ANIM_CURVE_EASE:
        t_q8 = (uint16_t)(255U - ((uint32_t)(255U - t_q8) * (255U - t_q8) >> 8)); /* ease-out quad：先快后慢 */
        break;
    case SC_ANIM_CURVE_SINE:
        t_q8 = sc_anim_sin_q8(t_q8); /* 先慢后快再慢 */
        break;
    case SC_ANIM_CURVE_EASE_IN:
        t_q8 = (uint16_t)((uint32_t)t_q8 * t_q8 >> 8); /* t^2 二次方缓入：先慢后快 */
        break;
    /* SC_ANIM_CURVE_LINEAR: t_q8 不变 */
    }
    if ((anim->type & SC_ANIM_TYPE_MASK) == SC_ANIM_ALPHA)
    {
        int16_t from = anim->data.alpha.from;
        int16_t target = anim->data.alpha.target;
        anim->data.alpha.anim = from + (int16_t)((int32_t)(target - from) * t_q8 / 255);
        sc_obj_set_opa(anim->obj, (uint8_t)anim->data.alpha.anim);
    }
    else
    {
        /* 吸收外部移动（容器滚动等）到 from/target，使轨迹跟随 */
        sc_anim_absorb_external_move(anim);
        int16_t new_x = anim->data.xy.from_x +
                        (int16_t)((int32_t)(anim->data.xy.target_x - anim->data.xy.from_x) * t_q8 / 255);
        int16_t new_y = anim->data.xy.from_y +
                        (int16_t)((int32_t)(anim->data.xy.target_y - anim->data.xy.from_y) * t_q8 / 255);
        sc_anim_set_cur(anim, new_x, new_y);
        /* MOVE_XY 记录本帧写后位置，供下一帧吸收外部移动（容器滚动等） */
        if ((anim->type & SC_ANIM_TYPE_MASK) == SC_ANIM_MOVE_XY)
        {
            anim->last_x = anim->obj->view.x;
            anim->last_y = anim->obj->view.y;
        }
        /* tick 后 invalidate 新位置（ALPHA 位置不变，开头已刷一次） */
        sc_obj_invalidate_area(anim->obj, NULL);
    }

    if (anim->state < anim->duration)
        return false;
    if (sc_anim_repeat_done(anim))
        return true;
    /* 循环：交换 from↔target，重置帧数 */
    if ((anim->type & SC_ANIM_TYPE_MASK) == SC_ANIM_ALPHA)
        sc_anim_swap_alpha(anim);
    else
        sc_anim_swap_xy(anim);
    anim->state = 0;
    return false;
}

/* 从池中分配空闲槽，绑定 obj，返回指针（NULL=池满）。 */
sc_anim_t *sc_anim_create(sc_obj_t *obj)
{
    for (uint8_t i = 0; i < SC_ANIM_POOL_SIZE; i++)
    {
        if (g_anim_pool[i].obj == NULL)
        {
            sc_anim_t *anim = &g_anim_pool[i];
            anim->obj = obj;
            anim->next = NULL;
            anim->type = SC_ANIM_MOVE_XY | SC_ANIM_CURVE_SINE; /* 默认 SINE 曲线 */
            anim->repeat = 0;
            anim->state = 0;            /* 当前帧数从 0 开始 */
            anim->duration = 12;        /* 默认约 200ms（12帧 × 16ms） */
            anim->last_x = obj->view.x; /* 记录起始 view，供外部移动吸收 */
            anim->last_y = obj->view.y;
            anim->data.xy.target_x = 0;
            anim->data.xy.target_y = 0;
            anim->data.xy.from_x = 0;
            anim->data.xy.from_y = 0;
            return anim;
        }
    }
    return NULL;
}

/* 配置平移：target 传 SC_ANIM_KEEP 表示该轴保持当前值。不重置曲线/时长配置。 */
void sc_anim_set_move_xy(sc_anim_t *anim, int16_t target_x, int16_t target_y)
{
    if (!anim || !anim->obj)
        return;
    anim->type = (anim->type & SC_ANIM_CURVE_MASK) | SC_ANIM_MOVE_XY;
    anim->data.xy.from_x = anim->obj->view.x;
    anim->data.xy.from_y = anim->obj->view.y;
    anim->data.xy.target_x = (target_x == SC_ANIM_KEEP) ? anim->obj->view.x : target_x;
    anim->data.xy.target_y = (target_y == SC_ANIM_KEEP) ? anim->obj->view.y : target_y;
    anim->last_x = anim->obj->view.x; /* 重新配置时同步 last，避免首帧误判外部偏移 */
    anim->last_y = anim->obj->view.y;
}

/* 配置缩放：target 传 SC_ANIM_KEEP 表示该轴保持当前值，左上角固定。不重置曲线/时长配置。 */
void sc_anim_set_scale_wh(sc_anim_t *anim, uint16_t target_w, uint16_t target_h)
{
    if (!anim || !anim->obj)
        return;
    anim->type = (anim->type & SC_ANIM_CURVE_MASK) | SC_ANIM_SCALE_WH;
    anim->data.xy.from_x = (int16_t)anim->obj->view.w;
    anim->data.xy.from_y = (int16_t)anim->obj->view.h;
    anim->data.xy.target_x = (target_w == SC_ANIM_KEEP) ? (int16_t)anim->obj->view.w : (int16_t)target_w;
    anim->data.xy.target_y = (target_h == SC_ANIM_KEEP) ? (int16_t)anim->obj->view.h : (int16_t)target_h;
}

/* 配置 alpha 渐变：通过 sc_obj_set_opa 统一设置，repeat=0单次 / 1~254重复N次 / 0xFF无限。不重置曲线/时长配置。 */
void sc_anim_set_alpha(sc_anim_t *anim, uint8_t from, uint8_t to, uint8_t repeat)
{
    if (!anim || !anim->obj)
        return;
    anim->type = (anim->type & SC_ANIM_CURVE_MASK) | SC_ANIM_ALPHA;
    anim->data.alpha.anim = from;
    anim->data.alpha.from = from;
    anim->data.alpha.target = to;
    anim->repeat = repeat;
    sc_obj_set_opa(anim->obj, from);
}

/* 通用动画定时器回调：遍历当前链（next 串联），通过虚表调用对应曲线的 tick 函数。
 * 每条链（sc_anim_start 传入的链头）独立定时器：串联动画同节拍共用一个定时器，
 * 独立动画各自独立定时器，互不拖累。done 的槽逐槽摘除回收，链空后清定时器。 */
static void sc_anim_translate_tick(sc_delay_t *timer)
{
    sc_anim_t *head = (sc_anim_t *)timer->arg;
    if (!head)
        return;
    /* 按真实时间推进：本次 tick 实际经过 ms = now - (上次到期时刻 - TICK_MS)。
     * 主循环帧率低于 60fps 时一次 tick 推进多帧，动画总时长始终等于 duration_ms，与帧率解耦。 */
    uint32_t now = sc_systick_get();
    uint32_t elapsed = now + SC_ANIM_TICK_MS - timer->time_out;
    uint8_t frames = (uint8_t)(elapsed / SC_ANIM_TICK_MS);
    if (frames == 0)
        frames = 1;
    /* 单次遍历：absorb 外部移动 + timed_core（内部自包含 invalidate），完成的摘除回收。 */
    sc_anim_t *prev = NULL;
    sc_anim_t *anim = head;
    bool any_active = false;
    while (anim)
    {
        sc_anim_t *next = anim->next;
        /* 所有曲线均走 timed_core（内部 absorb + invalidate 旧/新位置 + 更新 last） */
        bool done = sc_anim_tick_timed_core(anim, frames);
        if (done)
        {
            if (prev)
                prev->next = next;
            else
                head = next;
            anim->obj = NULL;
            anim->next = NULL;
        }
        else
        {
            prev = anim;
            any_active = true;
        }
        anim = next;
    }
    if (any_active)
    {
        /* 链头可能被摘除，更新定时器 arg 指向新链头；重新计时 */
        timer->arg = head;
        sc_reload_delay(timer, SC_ANIM_TICK_MS);
    }
    /* else: 链空，定时器自动清除（sc_timer_loop 检测到未 reload 会清槽） */
}

/* 停止指定对象的所有动画：把目标值设为当前值，下一帧自动结束回收。 */
void sc_anim_stop(sc_obj_t *obj)
{
    if (!obj)
        return;
    for (uint8_t i = 0; i < SC_ANIM_POOL_SIZE; i++)
    {
        sc_anim_t *anim = &g_anim_pool[i];
        if (anim->obj != obj)
            continue;
        if ((anim->type & SC_ANIM_TYPE_MASK) == SC_ANIM_ALPHA)
        {
            anim->data.alpha.target = anim->data.alpha.anim;
        }
        else if ((anim->type & SC_ANIM_TYPE_MASK) == SC_ANIM_MOVE_XY)
        {
            anim->data.xy.target_x = obj->view.x;
            anim->data.xy.target_y = obj->view.y;
        }
        else /* SC_ANIM_SCALE_WH */
        {
            anim->data.xy.target_x = (int16_t)obj->view.w;
            anim->data.xy.target_y = (int16_t)obj->view.h;
        }
    }
}

/* 启动动画链：为传入的链头 anim 开一个独立定时器（arg=链头）。
 * 串联动画（anim->next 链）共用一个定时器，回调中通过 next 遍历；独立动画各自独立定时器。
 * 同 cb+同 arg（同链头）再次调用会复用同一定时器槽（重置计时），不会叠加。 */
void sc_anim_start(sc_anim_t *anim)
{
    if (!anim)
        return;
    sc_add_delay(sc_anim_translate_tick, SC_ANIM_TICK_MS, anim);
}

/* 串联：直接把 second 接到 first 的 next（first->next = second），返回 first。
 * 串联动画共用同一定时器（start 链头即可，回调用 next 遍历）。 */
sc_anim_t *sc_anim_chain(sc_anim_t *first, sc_anim_t *second)
{
    if (!first)
        return second;
    first->next = second;
    return first;
}

#endif