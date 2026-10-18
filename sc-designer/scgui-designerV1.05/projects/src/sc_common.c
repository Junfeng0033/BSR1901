#include "sc_gui.h"

ALIGN_ARRAY(uint16_t, g_common_pfb_buffer, SC_PFB_BUF_SIZE, 4);
/* Push the current pfb slice to the display via the GUI flush callback. */
static void pfb_flush(sc_pfb_t *pfb)
{
    uint16_t w = (uint16_t)(pfb->area.xe - pfb->area.xs);
    uint16_t h = (uint16_t)(pfb->area.ye - pfb->area.ys);
#if SC_LCD_DMA_WAP
    uint32_t len = w * h;
    uint32_t n32 = len >> 1; // 对半数量
    uint32_t *p32 = (uint32_t *)pfb->buffer;
    while (n32--) // 4-byte swap
    {
        uint32_t v = *p32;
        v = ((v & 0x00FF00FF) << 8) | ((v & 0xFF00FF00) >> 8);
        *p32++ = v;
    }
    /* 2. 奇数长度？剩最后一个 u16 */
    if (len & 1)
    {
        // 最低位 1 表示奇数
        uint16_t v = *(uint16_t *)p32;
        *(uint16_t *)p32 = (v << 8) | (v >> 8);
    }
#endif
    g_gui->flush((uint16_t)pfb->area.xs, (uint16_t)pfb->area.ys, w, h, pfb->buffer);
}
/* Select the working buffer (alternating for double buffering) and clear it to the background color. */
static void pfb_memset(sc_pfb_t *pfb)
{

#if SC_LCD_DMA_2BUF
    static uint8_t addr;
    pfb->buffer = g_common_pfb_buffer + (addr ? (SC_PFB_BUF_SIZE >> 1) : 0);
    addr ^= 1U;
#else
    pfb->buffer = g_common_pfb_buffer;
#endif
    color_t color = pfb->bg_color;
    color_t *dst = pfb->buffer;
    uint32_t pixels = (uint32_t)pfb->width * pfb->height;
    uint32_t packed = ((uint32_t)color << 16) | color;
    uint32_t words = pixels >> 1;
    uint32_t *dst32 = (uint32_t *)dst;
    for (uint32_t i = 0; i < words; ++i)
    {
        *dst32++ = packed;
    }
    dst = (color_t *)dst32;
    if ((pixels & 1U) != 0U)
    {
        *dst = color;
    }
}

/* Set up the first horizontal render slice covering the dirty area, sized to fit the pfb buffer; returns false if invalid. */
bool pfb_init_slices(sc_pfb_t *pfb, area_t dirty, color_t bg_color)
{
    if (dirty.xs >= dirty.xe || dirty.ys >= dirty.ye)
    {
        return false;
    }
    uint16_t w = (uint16_t)(dirty.xe - dirty.xs);
    uint32_t max_h = (uint32_t)(SC_PFB_BUF_SIZE / w);
#if SC_LCD_DMA_2BUF
    max_h >>= 1; // If double buffer enabled, halve the slice height
#endif
    if (max_h == 0U)
    {
        return false;
    }
    int32_t h = (int32_t)max_h;
    if (dirty.ys + h > dirty.ye)
    {
        h = (int32_t)(dirty.ye - dirty.ys);
    }
    pfb->bg_color = bg_color;
    pfb->width = w;
    pfb->height = (uint16_t)h;
    pfb->area.xs = dirty.xs;
    pfb->area.ys = dirty.ys;
    pfb->area.xe = dirty.xe;
    pfb->area.ye = (int16_t)(dirty.ys + h);
    pfb->y_end = dirty.ye;
    pfb_memset(pfb);
    return true;
}
/* Flush the current slice and advance to the next one; returns false when the last slice has been flushed. */
bool pfb_next_slice(sc_pfb_t *pfb)
{
    pfb_flush(pfb);
    if (pfb->area.ye >= pfb->y_end)
    {
        return false;
    }
    pfb->area.ys = pfb->area.ye;
    if (pfb->area.ys + (int16_t)pfb->height > pfb->y_end)
    {
        pfb->height = (uint16_t)(pfb->y_end - pfb->area.ys);
    }
    pfb->area.ye = (int16_t)(pfb->area.ys + (int16_t)pfb->height);
    pfb_memset(pfb);
    return true;
}

/* Intersect *rect with the pfb slice area and optional clip; returns false if the result is empty. */
bool sc_pfb_intersection(sc_pfb_t *pfb, area_t *rect, const area_t *clip)
{
    if (!area_intersect(*rect, pfb->area, rect))
    {
        return false;
    }
    return !clip || area_intersect(*rect, *clip, rect);
}

/* Compute the horizontal offset to position an inner box inside an outer box per the alignment flags. */
int16_t sc_align_offset_x(int32_t outer_w, int32_t inner_w, sc_align_t align)
{
    if ((align & ALIGN_HOR) == ALIGN_HOR)
    {
        return (int16_t)((outer_w - inner_w) >> 1);
    }
    if ((align & ALIGN_RIGHT) != 0U)
    {
        return (int16_t)(outer_w - inner_w);
    }
    return 0;
}

/* Compute the vertical offset to position an inner box inside an outer box per the alignment flags. */
int16_t sc_align_offset_y(int32_t outer_h, int32_t inner_h, sc_align_t align)
{
    if ((align & ALIGN_VER) == ALIGN_VER)
    {
        return (int16_t)((outer_h - inner_h) >> 1);
    }
    if ((align & ALIGN_BOTTOM) != 0U)
    {
        return (int16_t)(outer_h - inner_h);
    }
    return 0;
}

static const int16_t sin0_90_table[] = {
    0,     572,   1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
    9032,  9580,  10126, 10668, 11207, 11743, 12275, 12803, 13328, 13848, 14364, 14876, 15383, 15886, 16383, 16876,
    17364, 17846, 18323, 18794, 19260, 19720, 20173, 20621, 21062, 21497, 21925, 22347, 22762, 23170, 23571, 23964,
    24351, 24730, 25101, 25465, 25821, 26169, 26509, 26841, 27165, 27481, 27788, 28087, 28377, 28659, 28932, 29196,
    29451, 29697, 29934, 30162, 30381, 30591, 30791, 30982, 31163, 31335, 31498, 31650, 31794, 31927, 32051, 32165,
    32269, 32364, 32448, 32523, 32587, 32642, 32687, 32722, 32747, 32762, 32767};

int32_t sc_sin(int16_t angle)
{
    angle = (int16_t)sc_norm_deg(angle);
    int32_t sign = (angle < 180) ? 1 : -1;
    if (angle >= 180)
    {
        angle = (int16_t)(angle - 180);
    }
    if (angle >= 90)
    {
        angle = (int16_t)(180 - angle);
    }
    return sign * sin0_90_table[angle];
}

int32_t sc_cos(int16_t angle)
{
    return sc_sin((int16_t)(angle + 90));
}

uint32_t sc_hypot_fast(int32_t dx, int32_t dy)
{
    uint32_t abs_dx = (dx < 0) ? (uint32_t)(-dx) : (uint32_t)dx;
    uint32_t abs_dy = (dy < 0) ? (uint32_t)(-dy) : (uint32_t)dy;
    uint32_t max = (abs_dx > abs_dy) ? abs_dx : abs_dy;
    uint32_t min = (abs_dx < abs_dy) ? abs_dx : abs_dy;
    return max + ((min * 424U) >> 10);
}

uint32_t sc_isqrt_u32(uint32_t n)
{
    uint32_t res = 0U;
    uint32_t bit = 1U << 30;
    /* bit > n loop also handles n == 0: bit shifts down to 0, second loop is skipped. */
    while (bit > n)
    {
        bit >>= 2;
    }
    while (bit != 0U)
    {
        if (n >= res + bit)
        {
            n -= res + bit;
            res = (res >> 1) + bit;
        }
        else
        {
            res >>= 1;
        }
        bit >>= 2;
    }
    return res;
}
