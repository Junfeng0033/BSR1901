#ifndef SC_COMMON_H
#define SC_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <sc_config.h>
#include "sc_mem.h"

/* 屏幕尺寸：工程根 sc_config.h（工具生成）存在时已定义；此处 #ifndef 兜底回落默认。 */
#ifndef SC_SCREEN_WIDTH
#define SC_SCREEN_WIDTH (320)
#endif
#ifndef SC_SCREEN_HEIGHT
#define SC_SCREEN_HEIGHT (240)
#endif

/* 板级与运行时调优宏（不进 sc_config.h）。 */
#ifndef PY32_MCU
#define PY32_MCU (0)
#endif

/* 这里只管 MCU 按行切分以省 RAM。 */
#ifndef SC_PFB_BUF_SIZE
#if PY32_MCU
#define SC_PFB_BUF_SIZE (SC_SCREEN_WIDTH * 4)
#else
#define SC_PFB_BUF_SIZE (SC_SCREEN_WIDTH * 10) // 10 行缓冲区
#endif
#endif

#ifndef SC_LCD_DMA_2BUF
#define SC_LCD_DMA_2BUF (0) // 双缓冲 DMA，减少屏闪
#endif

#ifndef SC_LCD_DMA_WAP
#define SC_LCD_DMA_WAP (0) // 是否交换 DMA 缓冲区
#endif

#ifndef SC_LCD_QSPI_ALIGN
#define SC_LCD_QSPI_ALIGN (0) // 是否对齐 QSPI数据
#endif

#ifndef SC_DIRTY_BUCKET_N
#define SC_DIRTY_BUCKET_N (6) /* 脏桶数量，用满强制合并到第一个 */
#endif
#if SC_DIRTY_BUCKET_N < 4
#error "SC_DIRTY_BUCKET_N must be >= 4 (sc_invalidate_all splits screen into 4 buckets)"
#endif
#ifndef SC_DIRTY_AREA_THRESHOLD
#define SC_DIRTY_AREA_THRESHOLD (15 * 15) /* 相交面积大于阈值则合并 */
#endif

#ifndef SC_DIRTY_FPS_DEBUG
#define SC_DIRTY_FPS_DEBUG (0)
#endif

/* SC_USER_OBJ=0: bare-draw only, do not compile obj/widget/page framework. */
#ifndef SC_USER_OBJ
#define SC_USER_OBJ 1
#endif

#ifndef SC_TOUCH_ENABLE
#define SC_TOUCH_ENABLE 1U
#endif

#ifndef SC_OBJ_MAX
#define SC_OBJ_MAX 32
#endif

#ifndef SC_DRAW_CACHE_N
#define SC_DRAW_CACHE_N 16U
#endif

#ifndef SC_PAGE_CNT
#define SC_PAGE_CNT 2U
#endif

#ifndef SC_FONT_MAX
#define SC_FONT_MAX 4
#endif

/* 常用宏定义 */
#define SC_POW2(x) ((x) * (x))
#define SC_Q15 (1 << 15)
#define SC_INT16_MAX (0x7FFF)
#define SC_INT16_MIN (-SC_INT16_MAX)

#define SC_INT32_MAX (0x7FFFFFFF)
#define SC_INT32_MIN (-SC_INT32_MAX)

#define SC_MIN(a, b) ((a) < (b) ? (a) : (b))
#define SC_MAX(a, b) ((a) > (b) ? (a) : (b))
#define SC_INV(a, b, c) ((a) > (b) ? (c) / ((a) - (b)) : 0)
#define SC_ABS(a) ((a) < 0 ? -(a) : (a))
#define SC_CLAMP(v, lo, hi) (SC_MIN(SC_MAX((v), (lo)), (hi)))
#define SC_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* Portable compile-time assertion (C99): fails to compile when cond is false. */
#define SC_STATIC_ASSERT_CONCAT_(a, b) a##b
#define SC_STATIC_ASSERT_CONCAT(a, b) SC_STATIC_ASSERT_CONCAT_(a, b)
#define SC_STATIC_ASSERT(cond, tag) \
    typedef char SC_STATIC_ASSERT_CONCAT(sc_static_assert_##tag##_, __LINE__)[(cond) ? 1 : -1]

/*
 * Portable weak-symbol marker for overridable platform hooks
 * (e.g. spiflash_read, demo_lcd_dma_refresh). Strong definitions in the
 * application / board layer override these defaults at link time.
 */
#if defined(__GNUC__) || defined(__clang__)
#define _SC_WEAK __attribute__((weak))
#define _SC_WEAK_SUPPORT 1
#elif defined(__ICCARM__) || defined(__IAR_SYSTEMS_ICC__)
#define _SC_WEAK __weak
#define _SC_WEAK_SUPPORT 1
#elif defined(__CC_ARM)
#define _SC_WEAK __weak
#define _SC_WEAK_SUPPORT 1
#elif defined(__TASKING__)
#define _SC_WEAK __attribute__((weak))
#define _SC_WEAK_SUPPORT 1
#else
#define _SC_WEAK
#define _SC_WEAK_SUPPORT 0
#endif

/* 公共对齐宏  ALIGN_ARRAY(type, name, size, boundary) */
#if   defined(__ICCARM__)
#  define ALIGN_ARRAY(type, name, size, boundary) \
        _Pragma("data_alignment=" #boundary) \
        type name[size]
#elif defined(__CC_ARM)
#  define ALIGN_ARRAY(type, name, size, boundary) \
        __align(boundary) type name[size]
#elif defined(__GNUC__) || defined(__clang__)
#  define ALIGN_ARRAY(type, name, size, boundary) \
        type name[size] __attribute__((aligned(boundary)))
#else
#  define ALIGN_ARRAY(type, name, size, boundary) \
        type name[size]
#endif
typedef uint16_t color_t;

#define sc_rgb565_pack_alpha(color) (((color) | ((uint32_t)(color) << 16)) & 0x07E0F81FU)
static inline uint16_t sc_blend_rgb565_fast(uint16_t bg, uint16_t color,
                                            uint32_t fg_pack, uint8_t alpha)
{
    uint32_t bg_pack;
    uint32_t result;
    if (alpha < 4U)
    {
        return bg;
    }
    if (alpha > 251U)
    {
        return color;
    }
    bg_pack = sc_rgb565_pack_alpha(bg);
    result = (bg_pack + ((fg_pack - bg_pack) * (alpha >> 3) >> 5)) & 0x07E0F81FU;
    return (uint16_t)((result & 0xFFFFU) | (result >> 16));
}

static inline uint16_t alphaBlend(uint16_t fc, uint16_t bc, uint8_t alpha)
{
    uint32_t fg_pack = sc_rgb565_pack_alpha(fc);
    return sc_blend_rgb565_fast(bc, fc, fg_pack, alpha);
}

static inline void sc_blend_ptr_fast(uint16_t *buf, uint16_t color, uint32_t fc_pack, uint8_t alpha)
{
    *buf = sc_blend_rgb565_fast(*buf, color, fc_pack, alpha);
}
/* Normalize degrees to [0, 360) without division/modulo (loop-based, MCU friendly). */
static inline int sc_norm_deg(int deg)
{
    while (deg >= 360)
    {
        deg -= 360;
    }
    while (deg < 0)
    {
        deg += 360;
    }
    return deg;
}

int32_t sc_sin(int16_t angle);
int32_t sc_cos(int16_t angle);
uint32_t sc_hypot_fast(int32_t dx, int32_t dy);
uint32_t sc_isqrt_u32(uint32_t n);

typedef struct rect_t
{
    int16_t x;
    int16_t y;
    uint16_t w;
    uint16_t h;
} rect_t;
typedef char sc_rect_size_must_be_8[(sizeof(rect_t) == 8U) ? 1 : -1];

typedef struct area_t
{
    int16_t xs;
    int16_t ys;
    int16_t xe;
    int16_t ye;
} area_t;

typedef struct sc_pfb_t
{
    color_t *buffer;
    area_t area;
    color_t bg_color;
    uint16_t width;
    uint16_t height;
    int16_t y_end;
} sc_pfb_t;

static inline color_t *sc_pfb_pixel_ptr(sc_pfb_t *pfb, int32_t x, int32_t y)
{
    return pfb->buffer + (y - pfb->area.ys) * pfb->width + x - pfb->area.xs;
}

static inline area_t rect_to_area(rect_t rect)
{
    area_t out;
    out.xs = rect.x;
    out.ys = rect.y;
    out.xe = (int16_t)(rect.x + rect.w);
    out.ye = (int16_t)(rect.y + rect.h);
    return out;
}

static inline rect_t area_to_rect(area_t area)
{
    rect_t out;
    out.x = area.xs;
    out.y = area.ys;
    out.w = (uint16_t)(area.xe - area.xs);
    out.h = (uint16_t)(area.ye - area.ys);
    return out;
}
static inline area_t area_offset(area_t area, int16_t dx, int16_t dy)
{
    area.xs = (int16_t)(area.xs + dx);
    area.ys = (int16_t)(area.ys + dy);
    area.xe = (int16_t)(area.xe + dx);
    area.ye = (int16_t)(area.ye + dy);
    return area;
}

static inline rect_t rect_offset(rect_t rect, int16_t dx, int16_t dy)
{
    rect.x = (int16_t)(rect.x + dx);
    rect.y = (int16_t)(rect.y + dy);
    return rect;
}

/* Expand (dx/dy > 0) or shrink (dx/dy < 0) from center by |d| on each side.
 * Axis that would go non-positive collapses to 0 and origin moves to mid-edge. */
static inline rect_t rect_grow(rect_t rect, int16_t dx, int16_t dy)
{
    int32_t w = (int32_t)rect.w + ((int32_t)dx << 1);
    int32_t h = (int32_t)rect.h + ((int32_t)dy << 1);
    if (w > 0)
    {
        rect.x = (int16_t)(rect.x - dx);
        rect.w = (uint16_t)w;
    }
    else
    {
        rect.x = (int16_t)(rect.x + (int16_t)(rect.w / 2U));
        rect.w = 0U;
    }
    if (h > 0)
    {
        rect.y = (int16_t)(rect.y - dy);
        rect.h = (uint16_t)h;
    }
    else
    {
        rect.y = (int16_t)(rect.y + (int16_t)(rect.h / 2U));
        rect.h = 0U;
    }
    return rect;
}
static inline bool area_contains_point(area_t area, int x, int y)
{
    return x >= area.xs && y >= area.ys && x < area.xe && y < area.ye;
}

static inline bool area_is_empty(area_t area)
{
    return area.xs >= area.xe || area.ys >= area.ye;
}

static inline bool area_nor_intersect(const area_t *a, const area_t *b)
{
    return (a->xe <= b->xs || a->xs >= b->xe || a->ye <= b->ys || a->ys >= b->ye);
}

static inline bool area_intersect(area_t a, area_t b, area_t *out)
{
    area_t clip;
    clip.ys = SC_MAX(a.ys, b.ys);
    clip.ye = SC_MIN(a.ye, b.ye);
    if (clip.ys >= clip.ye)
    {
        return false;
    }
    clip.xs = SC_MAX(a.xs, b.xs);
    clip.xe = SC_MIN(a.xe, b.xe);
    if (clip.xs >= clip.xe)
    {
        return false;
    }
    if (out)
    {
        *out = clip;
    }
    return true;
}

static inline bool rect_area_intersect(rect_t a, area_t b, area_t *out)
{
    return area_intersect(rect_to_area(a), b, out);
}

static inline area_t area_merged(area_t a, area_t b)
{
    area_t out;
    out.xs = SC_MIN(a.xs, b.xs);
    out.ys = SC_MIN(a.ys, b.ys);
    out.xe = SC_MAX(a.xe, b.xe);
    out.ye = SC_MAX(a.ye, b.ye);
    return out;
}

bool pfb_init_slices(sc_pfb_t *pfb, area_t dirty, color_t bg_color);
bool pfb_next_slice(sc_pfb_t *pfb);
bool sc_pfb_intersection(sc_pfb_t *pfb, area_t *rect, const area_t *clip);

typedef uint8_t sc_align_t;
enum
{
    ALIGN_NONE = 0x00,                      // 无对齐
    ALIGN_LEFT = (1 << 0),                  // 左对齐
    ALIGN_RIGHT = (1 << 1),                 // 右对齐
    ALIGN_TOP = (1 << 2),                   // 上对齐
    ALIGN_BOTTOM = (1 << 3),                // 下对齐
    ALIGN_HOR = (ALIGN_LEFT | ALIGN_RIGHT), // 水平对齐
    ALIGN_VER = (ALIGN_TOP | ALIGN_BOTTOM), // 垂直对齐
    ALIGN_CENTER = (ALIGN_HOR | ALIGN_VER), // 居中对齐
    ALIGN_AUTO_BREAK = (1 << 4),            // 自动换行
    ALIGN_REVERSE_MODE = (1 << 5),          // 反色模式
    ALIGN_BORDER_VIS = (1 << 6),            // 边框可见
};

int16_t sc_align_offset_x(int32_t outer_w, int32_t inner_w, sc_align_t align);
int16_t sc_align_offset_y(int32_t outer_h, int32_t inner_h, sc_align_t align);

///---------------------色彩定义--------------------------------
#define C_RGB(r, g, b) ((r >> 3) << 11 | (g >> 2) << 5 | b >> 3)
#define C_MAROON 0x8000
#define C_DARK_RED 0x8800
#define C_BROWN 0xA145
#define C_FIREBRICK 0xB104
#define C_CRIMSON 0xD8A7
#define C_RED 0xF800
#define C_TOMATO 0xFB09
#define C_CORAL 0xFBEA
#define C_INDIAN_RED 0xCAEB
#define C_LIGHT_CORAL 0xEC10
#define C_DARK_SALMON 0xE4AF
#define C_SALMON 0xF40E
#define C_LIGHT_SALMON 0xFD0F
#define C_ORANGE_RED 0xFA20
#define C_DARK_ORANGE 0xFC60
#define C_ORANGE 0xFD20
#define C_GOLD 0xFEA0
#define C_DARK_GOLDEN_ROD 0xB421
#define C_GOLDEN_ROD 0xDD24
#define C_PALE_GOLDEN_ROD 0xEF35
#define C_DARK_KHAKI 0xBDAD
#define C_KHAKI 0xEF31
#define C_OLIVE 0x8400
#define C_YELLOW 0xFFE0
#define C_YELLOW_GREEN 0x9E66
#define C_DARK_OLIVE_GREEN 0x5346
#define C_OLIVE_DRAB 0x6C64
#define C_LAWN_GREEN 0x7FC0
#define C_CHART_REUSE 0x7FE0
#define C_GREEN_YELLOW 0xAFE6
#define C_DARK_GREEN 0x0320
#define C_GREEN 0x07E0
#define C_FOREST_GREEN 0x2444

#define C_LIME 0x07E0
#define C_LIME_GREEN 0x3666
#define C_LIGHT_GREEN 0x9772
#define C_PALE_GREEN 0x97D2
#define C_DARK_SEA_GREEN 0x8DD1
#define C_MEDIUM_SPRING_GREEN 0x07D3
#define C_SPRING_GREEN 0x07EF
#define C_SEA_GREEN 0x344B
#define C_MEDIUM_AQUA_MARINE 0x6675
#define C_MEDIUM_SEA_GREEN 0x3D8E
#define C_LIGHT_SEA_GREEN 0x2595
#define C_DARK_SLATE_GRAY 0x328A
#define C_TEAL 0x0410
#define C_DARK_CYAN 0x0451
#define C_AQUA 0x07FF
#define C_CYAN 0x07FF
#define C_LIGHT_CYAN 0xDFFF
#define C_DARK_TURQUOISE 0x0679
#define C_TURQUOISE 0x46F9
#define C_MEDIUM_TURQUOISE 0x4E99
#define C_PALE_TURQUOISE 0xAF7D
#define C_AQUA_MARINE 0x7FFA

#define C_POWDER_BLUE 0xAEFC
#define C_CADET_BLUE 0x64F3
#define C_STEEL_BLUE 0x4C16
#define C_CORN_FLOWER_BLUE 0x64BD
#define C_DEEP_SKY_BLUE 0x05FF
#define C_DODGER_BLUE 0x249F
#define C_LIGHT_BLUE 0xAEBC
#define C_SKY_BLUE 0x867D
#define C_LIGHT_SKY_BLUE 0x867E
#define C_MIDNIGHT_BLUE 0x18CE
#define C_NAVY 0x0010
#define C_DARK_BLUE 0x0011
#define C_MEDIUM_BLUE 0x0019
#define C_BLUE 0x001F
#define C_ROYAL_BLUE 0x435B
#define C_BLUE_VIOLET 0x897B
#define C_INDIGO 0x4810
#define C_DARK_SLATE_BLUE 0x49F1
#define C_SLATE_BLUE 0x6AD9
#define C_MEDIUM_SLATE_BLUE 0x7B5D
#define C_MEDIUM_PURPLE 0x939B
#define C_DARK_MAGENTA 0x8811
#define C_DARK_VIOLET 0x901A
#define C_DARK_ORCHID 0x9999
#define C_MEDIUM_ORCHID 0xBABA
#define C_PURPLE 0x8010
#define C_THISTLE 0xD5FA
#define C_PLUM 0xDD1B
#define C_VIOLET 0xEC1D
#define C_MAGENTA 0xF81F
#define C_ORCHID 0xDB9A
#define C_MEDIUM_VIOLET_RED 0xC0B0
#define C_PALE_VIOLET_RED 0xDB92
#define C_DEEP_PINK 0xF8B2
#define C_HOT_PINK 0xFB56
#define C_LIGHT_PINK 0xFDB7
#define C_PINK 0xFDF9
#define C_ANTIQUE_WHITE 0xF75A
#define C_BEIGE 0xF7BB
#define C_BISQUE 0xFF18
#define C_BLANCHED_ALMOND 0xFF59
#define C_WHEAT 0xF6F6
#define C_CORN_SILK 0xFFBB
#define C_LEMON_CHIFFON 0xFFD9
#define C_LIGHT_GOLDEN_ROD_YELLOW 0xF7DA
#define C_LIGHT_YELLOW 0xFFFB
#define C_SADDLE_BROWN 0x8A22
#define C_SIENNA 0x9A85
#define C_CHOCOLATE 0xD344
#define C_PERU 0xCC28
#define C_SANDY_BROWN 0xF52C
#define C_BURLY_WOOD 0xDDB0
#define C_TAN 0xD591
#define C_ROSY_BROWN 0xBC71
#define C_MOCCASIN 0xFF16
#define C_NAVAJO_WHITE 0xFEF5
#define C_PEACH_PUFF 0xFED6
#define C_MISTY_ROSE 0xFF1B
#define C_LAVENDER_BLUSH 0xFF7E
#define C_LINEN 0xF77C
#define C_OLD_LACE 0xFFBC
#define C_PAPAYA_WHIP 0xFF7A
#define C_SEA_SHELL 0xFFBD
#define C_MINT_CREAM 0xF7FE
#define C_SLATE_GRAY 0x7412
#define C_LIGHT_SLATE_GRAY 0x7453
#define C_LIGHT_STEEL_BLUE 0xAE1B
#define C_LAVENDER 0xE73E
#define C_FLORAL_WHITE 0xFFDD
#define C_ALICE_BLUE 0xEFBF
#define C_GHOST_WHITE 0xF7BF
#define C_HONEYDEW 0xEFFD
#define C_IVORY 0xFFFD
#define C_AZURE 0xEFFF
#define C_SNOW 0xFFDE
#define C_DIM_GRAY 0x6B4D
#define C_GRAY 0x8410
#define C_DARK_GRAY 0xAD55
#define C_SILVER 0xBDF7
#define C_LIGHT_GRAY 0xD69A
#define C_GAINSBORO 0xDEDB
#define C_WHITE_SMOKE 0xF7BE
#define C_WHITE 0xFFFF
#define C_BLACK 0x0000

#endif // SC_COMMON_H
