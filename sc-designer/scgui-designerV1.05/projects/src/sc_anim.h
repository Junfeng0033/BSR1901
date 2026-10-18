#ifndef SC_ANIM_H
#define SC_ANIM_H

#include "sc_common.h"
#include "sc_obj.h"
#include "sc_event.h"

/* 动画池大小（静态数组槽位数）。 */
#ifndef SC_ANIM_POOL_SIZE
#define SC_ANIM_POOL_SIZE 8
#endif

/* 动画帧间隔（ms），约 60fps。 */
#define SC_ANIM_TICK_MS 16U

/* 魔数：set_move_xy / set_scale_wh 中传入此值表示该轴保持当前值，不参与动画。 */
#define SC_ANIM_KEEP 32767

/* type 字段位域：低 4 位动画类型，高 4 位曲线类型。 */
#define SC_ANIM_TYPE_MASK 0x0F
#define SC_ANIM_CURVE_MASK 0xF0

/* 动画类型（低 4 位）。 */
typedef enum
{
    SC_ANIM_MOVE_XY = 0, /* x+y 同时平移，通过 obj->view 访问 */
    SC_ANIM_SCALE_WH,    /* w+h 同时缩放，左上角固定 */
    SC_ANIM_ALPHA,       /* 透明度渐变，通过 sc_obj_set_opa 设置 */
    SC_ANIM_TYPE_MAX,
} sc_anim_type_t;

/* 曲线类型（高 4 位）。 */
typedef enum
{
    SC_ANIM_CURVE_EASE = 0x00,   /* 指数缓动（默认，用 gain 系数，基于误差，先快后慢） */
    SC_ANIM_CURVE_LINEAR = 0x10, /* 线性匀速（基于时间，需 duration） */
    SC_ANIM_CURVE_SINE = 0x20,   /* 正弦曲线（先慢后快再慢，alpha 渐变最自然，需 duration） */
    SC_ANIM_CURVE_EASE_IN = 0x30,/* 二次方缓入（先慢后快，EASE 的反曲线，需 duration） */
} sc_anim_curve_t;

/* 动画槽：静态数组元素，next 串联成单链表。
 * union 共享内存：xy 组（MOVE_XY/SCALE_WH）与 alpha 组（ALPHA）。
 * 空闲槽：obj==NULL。动画结束自动回收（obj=NULL 归还池）。 */
typedef struct sc_anim_t
{
    sc_obj_t *obj;          /* 被动画对象（NULL=空闲槽） */
    struct sc_anim_t *next; /* 串联下一个槽（NULL=结束） */
    uint8_t type;           /* 低 4 位：动画类型；高 4 位：曲线类型 */
    uint8_t repeat;         /* 循环次数：0=单次，1~254=重复N次，0xFF=无限循环 */
    uint8_t state;          /* 模式复用：EASE=gain缓动系数；LINEAR/SINE=当前帧数 */
    uint8_t duration;       /* LINEAR/SINE=总帧数（0=EASE模式），最大255帧≈4秒 */
    int16_t last_x;         /* 上一帧动画写后的 view.x，吸收外部移动（容器滚动等） */
    int16_t last_y;         /* 上一帧动画写后的 view.y */
    union
    {
        struct
        { /* MOVE_XY / SCALE_WH当前值直接读写 obj->view；起始/目标值存这里 */
            int16_t target_x; /* x 目标值 */
            int16_t target_y; /* y 目标值 */
            int16_t from_x;   /* x 起始值（循环回退用） */
            int16_t from_y;   /* y 起始值（循环回退用） */
        } xy;                 /* 8 字节 */
        struct
        {                   /* ALPHA */
            int16_t anim;   /* 当前 alpha */
            int16_t from;   /* 起始 alpha（循环回退用） */
            int16_t target; /* 目标 alpha */
        } alpha;            /* 6 字节，对齐到 8 */
    } data;                 /* 8 字节 */
} sc_anim_t;

/* 从池中分配空闲槽，绑定 obj，返回指针（NULL=池满）。 */
sc_anim_t *sc_anim_create(sc_obj_t *obj);

/* 配置平移：target 传 SC_ANIM_KEEP 表示该轴保持当前值。from 自动取当前 view.x/view.y。 */
void sc_anim_set_move_xy(sc_anim_t *anim, int16_t target_x, int16_t target_y);

/* 配置缩放：target 传 SC_ANIM_KEEP 表示该轴保持当前值，左上角固定。 */
void sc_anim_set_scale_wh(sc_anim_t *anim, uint16_t target_w, uint16_t target_h);

/* 配置 alpha 渐变：通过 sc_obj_set_opa 统一设置，repeat=0单次 / 1~254重复N次 / 0xFF无限循环。 */
void sc_anim_set_alpha(sc_anim_t *anim, uint8_t from, uint8_t to, uint8_t repeat);

/* 设置曲线类型（EASE/LINEAR/SINE/EASE_IN），所有曲线均基于帧数插值，切换时重置帧数。 */
static inline void sc_anim_set_curve(sc_anim_t *anim, sc_anim_curve_t curve)
{
    if (!anim)
        return;
    anim->type = (anim->type & SC_ANIM_TYPE_MASK) | (curve & SC_ANIM_CURVE_MASK);
    anim->state = 0;
}

/* 设置动画总时长（ms），自动换算为帧数（最大255帧≈4秒），所有曲线通用。 */
static inline void sc_anim_set_duration(sc_anim_t *anim, uint16_t ms)
{
    if (!anim)
        return;
    uint8_t frames = (uint8_t)(ms / SC_ANIM_TICK_MS);
    if (frames == 0)
        frames = 1; /* 至少 1 帧 */
    anim->duration = frames;
    anim->state = 0; /* 当前帧数清零 */
}

/* 便捷配置：曲线类型 + 时长(ms) + 循环次数，一行搞定。
 * 所有曲线（EASE/LINEAR/SINE/EASE_IN）均基于帧数插值，duration_ms 自动换算帧数（0→至少1帧）。
 * EASE = ease-out quad（先快后慢），EASE_IN = ease-in quad（先慢后快）。
 * repeat: 0=单次, 1~254=重复N次, 0xFF=无限循环。 */
static inline void sc_anim_config(sc_anim_t *anim, sc_anim_curve_t curve, uint16_t duration_ms, uint8_t repeat)
{
    if (!anim) return;
    anim->type = (anim->type & SC_ANIM_TYPE_MASK) | (curve & SC_ANIM_CURVE_MASK);
    anim->repeat = repeat;
    uint8_t frames = (uint8_t)(duration_ms / SC_ANIM_TICK_MS);
    if (frames == 0) frames = 1;
    anim->duration = frames;
    anim->state = 0;
}

/* 停止指定对象的所有动画：把目标值设为当前值，下一帧自动结束回收。 */
void sc_anim_stop(sc_obj_t *obj);

/* 启动动画组：头槽指针 anim，定时器回调遍历链表更新所有槽。 */
void sc_anim_start(sc_anim_t *anim);

/* 把 second 接到 first 链表尾部，返回 first。串联多个动画提高可读性。 */
sc_anim_t *sc_anim_chain(sc_anim_t *first, sc_anim_t *second);

#endif
