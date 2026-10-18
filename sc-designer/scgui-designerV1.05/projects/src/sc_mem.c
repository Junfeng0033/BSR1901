#include "sc_mem.h"

#include <stdint.h>

/* ------------------------------------------------------------------ */
/*  sc_memset：按 4 字节对齐后以 u32 块填充，提升 MCU 上大块清零效率。  */
/* ------------------------------------------------------------------ */
void *sc_memset(void *dst, int val, size_t n)
{
    uint8_t *d = (uint8_t *)dst;
    uint8_t v = (uint8_t)val;

    /* 先字节填充到 4 字节对齐边界 */
    while (((uintptr_t)d & 3U) != 0U && n > 0U)
    {
        *d++ = v;
        --n;
    }

    /* 对齐后按 u32 整块填充 */
    uint32_t v32 = (uint32_t)v * 0x01010101U;
    uint32_t *d32 = (uint32_t *)d;
    while (n >= 4U)
    {
        *d32++ = v32;
        n -= 4U;
    }

    /* 尾部不足 4 字节按字节补齐 */
    d = (uint8_t *)d32;
    while (n-- > 0)
    {
        *d++ = v;
    }
    return dst;
}

/* ------------------------------------------------------------------ */
/*  sc_memmove：dst<src 正序拷贝，dst>src 倒序拷贝，重叠时也安全。      */
/* ------------------------------------------------------------------ */
void *sc_memmove(void *dst, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;

    if (d < s)
    {
        while (n-- > 0)
        {
            *d++ = *s++;
        }
    }
    else if (d > s)
    {
        d += n;
        s += n;
        while (n-- > 0)
        {
            *--d = *--s;
        }
    }
    return dst;
}

/* ------------------------------------------------------------------ */
/*  sc_memcpy：两指针均 4 字节对齐时按 u32 块拷贝，否则字节回退。        */
/* ------------------------------------------------------------------ */
void *sc_memcpy(void *dst, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    while (n >= 4U && ((uintptr_t)d & 3U) == 0U && ((uintptr_t)s & 3U) == 0U)
    {
        *(uint32_t *)d = *(const uint32_t *)s;
        d += 4U;
        s += 4U;
        n -= 4U;
    }
    while (n-- > 0)
    {
        *d++ = *s++;
    }
    return dst;
}

/* ------------------------------------------------------------------ */
/*  sc_strlen：简单字节循环。                                            */
/* ------------------------------------------------------------------ */
size_t sc_strlen(const char *s)
{
    const char *p = s;
    while (*p != '\0')
    {
        ++p;
    }
    return (size_t)(p - s);
}
