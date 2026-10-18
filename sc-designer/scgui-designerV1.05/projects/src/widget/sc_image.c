#include "../sc_draw.h"
#include "sc_image.h"

#if SC_WIDGET_IMAGE_ENABLE

/* Copy an in-memory RGB565 image row by row into the clipped pfb area. */
static void sc_blit_mem_rgb565(sc_pfb_t *pfb, const sc_image_t *props,
                               area_t inter, rect_t full)
{
    uint32_t row_pixels = (uint32_t)(inter.xe - inter.xs);
    color_t *dst =
        sc_pfb_pixel_ptr(pfb, inter.xs, inter.ys);
    const color_t *src = (const color_t *)props->data.pixels +
                         (uint32_t)(inter.ys - full.y) * props->width +
                         (uint32_t)(inter.xs - full.x);
    for (int16_t yy = inter.ys; yy < inter.ye; ++yy)
    {
        for (uint32_t i = 0U; i < row_pixels; ++i)
        {
            dst[i] = src[i];
        }
        dst += pfb->width;
        src += props->width;
    }
}

/*
 * Alpha-blend an in-memory planar RGB565A8 image: data.pixels points at the
 * RGB565 color plane, the A8 mask plane follows it (see SC_IMAGE_A8_MASK_OFFSET).
 */
static void sc_blit_mem_rgb565a8(sc_pfb_t *pfb, const sc_image_t *props,
                                 area_t inter, rect_t full)
{
    uint32_t row_pixels = (uint32_t)(inter.xe - inter.xs);

    const color_t *color_base = (const color_t *)props->data.pixels;
    const uint8_t *mask_base = (const uint8_t *)props->data.pixels +
                               SC_IMAGE_A8_MASK_OFFSET(props->width, props->height);
    uint32_t off = (uint32_t)(inter.ys - full.y) * props->width +
                   (uint32_t)(inter.xs - full.x);
    const uint8_t *msk = mask_base + off;
    const color_t *src = color_base + off;

    color_t *dst = sc_pfb_pixel_ptr(pfb, inter.xs, inter.ys);
    for (int16_t yy = inter.ys; yy < inter.ye; ++yy)
    {
        for (uint32_t i = 0U; i < row_pixels; ++i)
        {
            uint8_t alpha = msk[i];
            if (alpha < 4U)
            {
                continue;
            }
            dst[i] = sc_blend_rgb565_fast(dst[i], src[i], sc_rgb565_pack_alpha(src[i]), alpha);
        }
        dst += pfb->width;
        src += props->width;
        msk += props->width;
    }
}

#if SC_WIDGET_IMAGE_EX_ENABLE
/* Read an external-flash RGB565 image directly into the clipped pfb area, one row per read. */
static void sc_blit_flash_rgb565(sc_pfb_t *pfb, const sc_image_t *props,
                                 area_t inter, rect_t full)
{
    uint32_t row_bytes = (uint32_t)(inter.xe - inter.xs) * sizeof(color_t);
    color_t *dst =
        sc_pfb_pixel_ptr(pfb, inter.xs, inter.ys);
    uint32_t addr = props->data.flash_addr +
                    ((uint32_t)(inter.ys - full.y) * props->width +
                     (uint32_t)(inter.xs - full.x)) *
                        sizeof(color_t);
    for (int16_t yy = inter.ys; yy < inter.ye; ++yy)
    {
        spiflash_read((uint8_t *)dst, addr, row_bytes);
        dst += pfb->width;
        addr += (uint32_t)props->width * sizeof(color_t);
    }
}

#define SC_IMAGE_FLASH_BLEND_CHUNK 16U

/*
 * Read and alpha-blend a planar RGB565A8 image from external flash. flash_addr
 * points at the RGB565 color plane, the A8 mask plane follows it (see
 * SC_IMAGE_A8_MASK_OFFSET). Mask and color are read in matching chunks.
 */
static void sc_blit_flash_rgb565a8(sc_pfb_t *pfb, const sc_image_t *props,
                                   area_t inter, rect_t full)
{
    uint32_t row_pixels = (uint32_t)(inter.xe - inter.xs);

    uint32_t color_base = props->data.flash_addr;
    uint32_t mask_base = color_base + SC_IMAGE_A8_MASK_OFFSET(props->width, props->height);
    uint32_t off = (uint32_t)(inter.ys - full.y) * props->width +
                   (uint32_t)(inter.xs - full.x);
    uint32_t mask_row = mask_base + off;
    uint32_t color_row = color_base + off * sizeof(color_t);
    uint8_t mbuf[SC_IMAGE_FLASH_BLEND_CHUNK];
    color_t cbuf[SC_IMAGE_FLASH_BLEND_CHUNK];

    color_t *dst = sc_pfb_pixel_ptr(pfb, inter.xs, inter.ys);
    for (int16_t yy = inter.ys; yy < inter.ye; ++yy)
    {
        uint32_t maddr = mask_row;
        uint32_t caddr = color_row;
        uint32_t remain = row_pixels;
        color_t *out = dst;
        while (remain != 0U)
        {
            uint32_t count = SC_MIN(remain, SC_IMAGE_FLASH_BLEND_CHUNK);
            spiflash_read(mbuf, maddr, count);
            spiflash_read((uint8_t *)cbuf, caddr, count * sizeof(color_t));
            for (uint32_t i = 0U; i < count; ++i)
            {
                uint8_t alpha = mbuf[i];
                if (alpha < 4U)
                {
                    continue;
                }
                out[i] = sc_blend_rgb565_fast(out[i], cbuf[i], sc_rgb565_pack_alpha(cbuf[i]), alpha);
            }
            out += count;
            maddr += count;
            caddr += count * sizeof(color_t);
            remain -= count;
        }
        dst += pfb->width;
        mask_row += props->width;
        color_row += (uint32_t)props->width * sizeof(color_t);
    }
}

#endif /* SC_WIDGET_IMAGE_EX_ENABLE */

#if SC_WIDGET_IMAGE_RGB888_ENABLE
/* Fast /255 with rounding for a value already known to fit in 16 bits. */
#define SC_DIV255(v) ((uint8_t)(((v) + ((v) >> 8) + 0x80U) >> 8))

/*
 * Blend one RGBA8888 quad (R,G,B,A) over the destination RGB565 pixel.
 * Mixing happens in full 8-bit channels (background expanded 5/6->8 bit) so the
 * source color is never truncated to RGB565 before blending; only the final
 * result is packed back to RGB565.
 */
static inline void sc_image_blend_rgba(color_t *dst, const uint8_t *rgba)
{
    uint8_t alpha = rgba[3];
    if (alpha < 4U)
    {
        return;
    }
    if (alpha > 251U)
    {
        *dst = C_RGB(rgba[0], rgba[1], rgba[2]);
        return;
    }
    uint16_t bg = *dst;
    uint8_t br = (uint8_t)((bg >> 11) & 0x1FU);
    uint8_t bgc = (uint8_t)((bg >> 5) & 0x3FU);
    uint8_t bb = (uint8_t)(bg & 0x1FU);
    br = (uint8_t)((br << 3) | (br >> 2));
    bgc = (uint8_t)((bgc << 2) | (bgc >> 4));
    bb = (uint8_t)((bb << 3) | (bb >> 2));
    uint8_t inv = (uint8_t)(255U - alpha);
    uint8_t r = SC_DIV255((uint32_t)rgba[0] * alpha + (uint32_t)br * inv);
    uint8_t g = SC_DIV255((uint32_t)rgba[1] * alpha + (uint32_t)bgc * inv);
    uint8_t b = SC_DIV255((uint32_t)rgba[2] * alpha + (uint32_t)bb * inv);
    *dst = C_RGB(r, g, b);
}

/* Alpha-blend an in-memory RGBA8888 image row by row into the clipped pfb area. */
static void sc_blit_mem_rgba(sc_pfb_t *pfb, const sc_image_t *props,
                             area_t inter, rect_t full)
{
    uint32_t row_pixels = (uint32_t)(inter.xe - inter.xs);

    const uint8_t *src = (const uint8_t *)props->data.pixels +
                         ((uint32_t)(inter.ys - full.y) * props->width +
                          (uint32_t)(inter.xs - full.x)) *
                             4U;
    color_t *dst = sc_pfb_pixel_ptr(pfb, inter.xs, inter.ys);
    for (int16_t yy = inter.ys; yy < inter.ye; ++yy)
    {
        for (uint32_t i = 0U; i < row_pixels; ++i)
        {
            sc_image_blend_rgba(&dst[i], &src[i * 4U]);
        }
        dst += pfb->width;
        src += (uint32_t)props->width * 4U;
    }
}

#if SC_WIDGET_IMAGE_EX_ENABLE
/* Read and alpha-blend an external-flash RGBA8888 image without a full scanline buffer. */
static void sc_blit_flash_rgba(sc_pfb_t *pfb, const sc_image_t *props,
                               area_t inter, rect_t full)
{
    uint32_t row_pixels = (uint32_t)(inter.xe - inter.xs);

    uint32_t row_addr = props->data.flash_addr +
                        ((uint32_t)(inter.ys - full.y) * props->width +
                         (uint32_t)(inter.xs - full.x)) *
                            4U;
    uint8_t rgba[SC_IMAGE_FLASH_BLEND_CHUNK * 4U];

    color_t *dst = sc_pfb_pixel_ptr(pfb, inter.xs, inter.ys);
    for (int16_t yy = inter.ys; yy < inter.ye; ++yy)
    {
        uint32_t addr = row_addr;
        uint32_t remain = row_pixels;
        color_t *out = dst;
        while (remain != 0U)
        {
            uint32_t count = SC_MIN(remain, SC_IMAGE_FLASH_BLEND_CHUNK);
            spiflash_read(rgba, addr, count * 4U);
            for (uint32_t i = 0U; i < count; ++i)
            {
                sc_image_blend_rgba(&out[i], &rgba[i * 4U]);
            }
            out += count;
            addr += count * 4U;
            remain -= count;
        }
        dst += pfb->width;
        row_addr += (uint32_t)props->width * 4U;
    }
}
#endif /* SC_WIDGET_IMAGE_EX_ENABLE */
#endif /* SC_WIDGET_IMAGE_RGB888_ENABLE */

/* Row-index table: big-endian offsets into the compressed pixel stream. */
static uint32_t sc_qoi_row_offset(const uint8_t *index_base, uint16_t u16_size, uint16_t u24_size,
                                  uint32_t interval_idx)
{
    uint32_t u16_cnt = (uint32_t)u16_size / 2U;
    uint32_t u24_cnt = (uint32_t)u24_size / 3U;
    if (interval_idx < u16_cnt)
    {
        const uint8_t *p = index_base + interval_idx * 2U;
        return ((uint32_t)p[0] << 8) | p[1];
    }
    if (interval_idx < u16_cnt + u24_cnt)
    {
        uint32_t idx = interval_idx - u16_cnt;
        const uint8_t *p = index_base + u16_size + idx * 3U;
        return ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | p[2];
    }
    uint32_t idx = interval_idx - u16_cnt - u24_cnt;
    const uint8_t *p = index_base + u16_size + u24_size + idx * 4U;
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}

#define SC_QOI_TAG_MASK 0xC0U
#define SC_QOI_A4_OPAQUE 0x0FU

/* Lossy first byte is tag00|a4 (0x00..0x0F); lossless uses 0xFE/0xFF. One compare covers it. */
static uint8_t sc_qoi_payload_a4_raw(const uint8_t *payload)
{
    return (uint8_t)(payload[0] < 0x10U);
}

#if SC_WIDGET_IMAGE_EX_ENABLE
/* Read consecutive mixed-width big-endian row offsets and expand them in place. */
static void sc_qoi_flash_offsets(uint32_t index_addr, uint16_t u16_size, uint16_t u24_size,
                                 uint32_t first, uint32_t count, uint32_t *out)
{
    uint32_t u16_cnt = (uint32_t)u16_size / 2U;
    uint32_t u24_cnt = (uint32_t)u24_size / 3U;
    while (count > 0U)
    {
        uint32_t width;
        uint32_t span;
        uint32_t addr;
        if (first < u16_cnt)
        {
            width = 2U;
            span = SC_MIN(count, u16_cnt - first);
            addr = index_addr + first * 2U;
        }
        else if (first < u16_cnt + u24_cnt)
        {
            width = 3U;
            span = SC_MIN(count, u16_cnt + u24_cnt - first);
            addr = index_addr + u16_size + (first - u16_cnt) * 3U;
        }
        else
        {
            width = 4U;
            span = count;
            addr = index_addr + u16_size + u24_size +
                   (first - u16_cnt - u24_cnt) * 4U;
        }
        uint8_t *raw = (uint8_t *)out;
        spiflash_read(raw, addr, span * width);
        for (uint32_t i = span; i-- > 0U;)
        {
            uint32_t p = i * width;
            if (width == 2U)
            {
                out[i] = ((uint32_t)raw[p] << 8) | raw[p + 1U];
            }
            else if (width == 3U)
            {
                out[i] = ((uint32_t)raw[p] << 16) |
                         ((uint32_t)raw[p + 1U] << 8) | raw[p + 2U];
            }
            else
            {
                out[i] = ((uint32_t)raw[p] << 24) |
                         ((uint32_t)raw[p + 1U] << 16) |
                         ((uint32_t)raw[p + 2U] << 8) | raw[p + 3U];
            }
        }
        first += span;
        count -= span;
        out += span;
    }
}

/*
 * Full blob in Flash (indexQOI n=1: one index entry per image row).
 * Header bytes 3-8 (u16/u24/u32 size) are mirrored in sc_image_qoi_flash_t,
 * so no per-slice flash header read is needed (SC_LCD_DRAW slices one row at a time).
 * Row offsets for the visible span are read in one batch, then payload is streamed
 * through an 18-byte temporary buffer.
 */
static void sc_blit_flash_qoi(sc_pfb_t *pfb, const sc_image_t *props,
                              area_t inter, rect_t full)
{
    const sc_image_qoi_flash_t *source = (const sc_image_qoi_flash_t *)props->data.pixels;
    uint32_t height = props->height;
    uint32_t row0 = (uint32_t)(inter.ys - full.y);
    uint32_t row1 = (uint32_t)(inter.ye - full.y);
    uint32_t row_last = (row1 < height) ? row1 : (height - 1U);
    uint32_t index_slots = row_last - row0 + 1U;
    if (row_last + 1U < height)
    {
        index_slots += 1U;
    }
    uint8_t qoi_buffer[18U];
    uint16_t u16_size = source->u16_size;
    uint16_t u24_size = source->u24_size;
    uint16_t u32_size = source->u32_size;
    uint32_t index_addr = source->blob_addr + 9U;
    uint32_t index_bytes = (uint32_t)u16_size + (uint32_t)u24_size + (uint32_t)u32_size;
    uint32_t payload_addr = index_addr + index_bytes;
    uint32_t payload_size = source->blob_size - 9U - index_bytes;
    uint32_t row_offs[index_slots];
    uint32_t clipped_left = (uint32_t)(inter.xs - full.x);
    uint32_t decode_count = clipped_left + (uint32_t)(inter.xe - inter.xs);
    color_t *dest_row = sc_pfb_pixel_ptr(pfb, inter.xs, inter.ys);
    uint32_t row = row0;
    sc_qoi_flash_offsets(index_addr, u16_size, u24_size, row0, index_slots, row_offs);
    for (int16_t y = inter.ys; y < inter.ye; ++y)
    {
        uint32_t row_offset = row_offs[row - row0];
        uint32_t next_offset = (row + 1U < height) ? row_offs[row + 1U - row0] : payload_size;
        uint32_t qoi_addr = payload_addr + row_offset;
        uint32_t qoi_left = next_offset - row_offset;
        uint8_t *qoi_p = qoi_buffer;
        uint8_t *qoi_buffer_end = qoi_buffer;
        uint8_t r = 0U, g = 0U, b = 0U, a4 = SC_QOI_A4_OPAQUE;
        color_t cur = 0U;
        uint8_t run = 0U;
        color_t *out = dest_row;
        for (uint32_t i = 0U; i < decode_count; ++i)
        {
            if (run == 0U)
            {
                uint8_t remain = (uint8_t)(qoi_buffer_end - qoi_p);
                if (remain < 3U && qoi_left != 0U)
                {
                    for (uint8_t j = 0U; j < remain; ++j)
                    {
                        qoi_buffer[j] = qoi_p[j];
                    }
                    uint32_t read_size = SC_MIN(qoi_left, 16U);
                    qoi_buffer_end = qoi_buffer + remain;
                    spiflash_read(qoi_buffer_end, qoi_addr, read_size);
                    qoi_addr += read_size;
                    qoi_left -= read_size;
                    qoi_p = qoi_buffer;
                    qoi_buffer_end += read_size;
                }
                uint8_t flag = *qoi_p++;
                switch (flag & SC_QOI_TAG_MASK)
                {
                case 0x40U:
                    r = (uint8_t)((r + ((flag >> 4) & 0x03U) - 2U) & 0x1FU);
                    g = (uint8_t)((g + ((flag >> 2) & 0x03U) - 2U) & 0x3FU);
                    b = (uint8_t)((b + (flag & 0x03U) - 2U) & 0x1FU);
                    cur = (color_t)(((uint16_t)r << 11) | ((uint16_t)g << 5) | b);
                    break;
                case 0x80U:
                {
                    int8_t vg = (int8_t)((flag & 0x3FU) - 32);
                    uint8_t next_byte = *qoi_p++;
                    r = (uint8_t)((r + vg - 8 + ((next_byte >> 4) & 0x0FU)) & 0x1FU);
                    g = (uint8_t)((g + vg) & 0x3FU);
                    b = (uint8_t)((b + vg - 8 + (next_byte & 0x0FU)) & 0x1FU);
                    cur = (color_t)(((uint16_t)r << 11) | ((uint16_t)g << 5) | b);
                    break;
                }
                case 0xC0U:
                {
                    uint8_t temp = (uint8_t)(flag & 0x3FU);
                    /* 统一 RUN_MAX=62：0xC0~0xFD是RUN，0xFE/0xFF落入default绝对色 */
                    if (temp < 62U)
                    {
                        run = temp;
                        break;
                    }
                }
                /* fallthrough */
                default:
                {
                    uint8_t h_val = *qoi_p++;
                    uint8_t l_val = *qoi_p++;
                    cur = (color_t)(((uint16_t)h_val << 8) | l_val);
                    a4 = (uint8_t)(flag & 0x0FU);
                    r = (uint8_t)((cur >> 11) & 0x1FU);
                    g = (uint8_t)((cur >> 5) & 0x3FU);
                    b = (uint8_t)(cur & 0x1FU);
                    break;
                }
                }
            }
            else
            {
                --run;
            }
            if (i >= clipped_left)
            {
                a4 = a4 & 0x0FU;
                if (a4 == 0x0f)
                {
                    *out = cur;
                }
                else
                {
                    uint8_t alpha = (uint8_t)(a4 * 17U);
                    if (alpha >= 4U)
                    {
                        *out = sc_blend_rgb565_fast(*out, cur, sc_rgb565_pack_alpha(cur), alpha);
                    }
                }
                ++out;
            }
        }
        ++row;
        dest_row += pfb->width;
    }
}
#endif /* SC_WIDGET_IMAGE_EX_ENABLE */

/*
 * Decode indexQOI (RGB565 + per-row payload offsets) into the clipped pfb.
 * Compact header: src[0]==9; bytes 1-2 are interval (must equal image width, n=1).
 */
static void sc_blit_mem_qoi(sc_pfb_t *pfb, const sc_image_t *props, area_t inter, rect_t full)
{
    const uint8_t *src = (const uint8_t *)props->data.pixels;
    const uint8_t *index_base = src + src[0];
    const uint8_t *meta = index_base - 8U;
    uint16_t u16_size = ((uint16_t)meta[2] << 8) | meta[3];
    uint16_t u24_size = ((uint16_t)meta[4] << 8) | meta[5];
    uint16_t u32_size = ((uint16_t)meta[6] << 8) | meta[7];
    const uint8_t *qoi_data = index_base + u16_size + u24_size + u32_size;
    uint8_t a4_raw = sc_qoi_payload_a4_raw(qoi_data);
    uint32_t clipped_left = (uint32_t)(inter.xs - full.x);
    uint32_t visible_width = (uint32_t)(inter.xe - inter.xs);
    color_t *dest_row = sc_pfb_pixel_ptr(pfb, inter.xs, inter.ys);
    for (int16_t y = inter.ys; y < inter.ye; ++y)
    {
        uint32_t row_r = (uint32_t)(y - full.y);
        const uint8_t *p = qoi_data +
                           sc_qoi_row_offset(index_base, u16_size, u24_size, row_r);
        /* 解码状态直接用局部变量（不建结构体/不取地址），r/g/b/cur/run 留在寄存器 */
        uint8_t r = 0U, g = 0U, b = 0U, a4 = SC_QOI_A4_OPAQUE;
        color_t cur = 0U;
        uint8_t run = 0U;
        uint32_t decode_count = clipped_left + visible_width;
        color_t *out = dest_row;
        for (uint32_t i = 0U; i < decode_count; ++i)
        {
            if (run == 0U)
            {
                uint8_t flag = *p++;
                switch (flag & SC_QOI_TAG_MASK)
                {
                case 0x40U:
                    r = (uint8_t)((r + ((flag >> 4) & 0x03U) - 2U) & 0x1FU);
                    g = (uint8_t)((g + ((flag >> 2) & 0x03U) - 2U) & 0x3FU);
                    b = (uint8_t)((b + (flag & 0x03U) - 2U) & 0x1FU);
                    cur = (color_t)(((uint16_t)r << 11) | ((uint16_t)g << 5) | b);
                    break;
                case 0x80U:
                {
                    int8_t vg = (int8_t)((flag & 0x3FU) - 32);
                    uint8_t next_byte = *p++;
                    r = (uint8_t)((r + vg - 8 + ((next_byte >> 4) & 0x0FU)) & 0x1FU);
                    g = (uint8_t)((g + vg) & 0x3FU);
                    b = (uint8_t)((b + vg - 8 + (next_byte & 0x0FU)) & 0x1FU);
                    cur = (color_t)(((uint16_t)r << 11) | ((uint16_t)g << 5) | b);
                    break;
                }
                case 0xC0U:
                {
                    uint8_t temp = (uint8_t)(flag & 0x3FU);
                    /* 统一 RUN_MAX=62：0xC0~0xFD(temp<62)是RUN，0xFE/0xFF落入default绝对色 */
                    if (temp < 62U)
                    {
                        run = temp;
                        break;
                    }
                }
                /* fallthrough */
                default:
                {
                    uint8_t h_val = *p++;
                    uint8_t l_val = *p++;
                    cur = (color_t)(((uint16_t)h_val << 8) | l_val);
                    a4 = (uint8_t)(flag & 0x0FU); /* 无损不读此值 */
                    r = (uint8_t)((cur >> 11) & 0x1FU);
                    g = (uint8_t)((cur >> 5) & 0x3FU);
                    b = (uint8_t)(cur & 0x1FU);
                    break;
                }
                }
            }
            else
            {
                --run;
            }
            if (i >= clipped_left)
            {
                if (a4_raw != 0U)
                {
                    uint8_t alpha = (uint8_t)((a4 & 0x0FU) * 17U);
                    if (alpha >= 4U)
                    {
                        *out = sc_blend_rgb565_fast(*out, cur, sc_rgb565_pack_alpha(cur), alpha);
                    }
                }
                else
                {
                    *out = cur;
                }
                ++out;
            }
        }
        dest_row += pfb->width;
    }
}

/* Clip the image to the pfb, then blit by storage/format (no function-pointer table). */
void sc_draw_image(sc_pfb_t *pfb, const sc_image_t *props, int16_t x, int16_t y)
{
    rect_t full = (rect_t){x, y, props->width, props->height};
    area_t inter = rect_to_area(full);
    if (!sc_pfb_intersection(pfb, &inter, NULL))
    {
        return;
    }
    uint8_t fmt = props->type & SC_IMAGE_FMT_MASK;
#if SC_WIDGET_IMAGE_EX_ENABLE
    if (props->type & SC_IMAGE_FLASH)
    {
        switch (fmt)
        {
        case SC_IMAGE_RGB565:
            sc_blit_flash_rgb565(pfb, props, inter, full);
            break;
#if SC_WIDGET_IMAGE_RGB888_ENABLE
        case SC_IMAGE_RGBA8888:
            sc_blit_flash_rgba(pfb, props, inter, full);
            break;
#endif
        case SC_IMAGE_RGB565A8:
            sc_blit_flash_rgb565a8(pfb, props, inter, full);
            break;
        case SC_IMAGE_QOI:
            sc_blit_flash_qoi(pfb, props, inter, full);
            break;
        default:
            break;
        }
        return;
    }
#endif
    switch (fmt)
    {
    case SC_IMAGE_RGB565:
        sc_blit_mem_rgb565(pfb, props, inter, full);
        break;
#if SC_WIDGET_IMAGE_RGB888_ENABLE
    case SC_IMAGE_RGBA8888:
        sc_blit_mem_rgba(pfb, props, inter, full);
        break;
#endif
    case SC_IMAGE_RGB565A8:
        sc_blit_mem_rgb565a8(pfb, props, inter, full);
        break;
    case SC_IMAGE_QOI:
        sc_blit_mem_qoi(pfb, props, inter, full);
        break;
    default:
        break;
    }
}

/* Draw an image to the LCD via the given pfb or the LCD draw macro. */
void sc_lcd_draw_image(sc_pfb_t *pfb, int16_t x, int16_t y, const sc_image_t *props)
{
    if (!props || props->width == 0U || props->height == 0U)
    {
        return;
    }
#if !SC_WIDGET_IMAGE_EX_ENABLE
    if (props->type & SC_IMAGE_FLASH)
    {
        return;
    }
#endif
    rect_t view = (rect_t){x, y, props->width, props->height};
    if (pfb)
    {
        sc_draw_image(pfb, props, x, y);
        return;
    }
    SC_LCD_DRAW(rect_to_area(view), sc_draw_image(sc_lcd_draw_pfb__, props, x, y));
}

#if SC_USER_OBJ && SC_WIDGET_IMAGE_ENABLE
static const sc_obj_vt_t sc_image_vt = {
    .event_cb = sc_obj_handle_image_event,
    .set_style = sc_image_style,
};

/* Create an image object under parent using the given runtime props.
 * 调用方提供 sc_image_props_t（RAM 静态变量）：image 指向 const 资源，cb 归控件可运行时注册。 */
sc_obj_t *sc_img_create(sc_obj_t *parent, sc_image_props_t *props)
{
    return sc_obj_create(parent, SC_OBJ_IMAGE, &sc_image_vt, (void *)props);
}

/* Switch to another image (e.g. the i-th frame of an array) at the same position.
 * 只换资源指针，回调保留在包装内；新图尺寸不同时自动跟随 view（保留 x/y）。 */
void sc_image_set_img(sc_obj_t *obj, const sc_image_t *img)
{
    sc_image_props_t *props = (sc_image_props_t *)obj->props;
    if (props && img)
    {
        props->image = img;
        obj->view.w = img->width;
        obj->view.h = img->height;
        sc_obj_invalidate(obj);
    }
}

/* Image widget: only callback is stylable; callback lives in the runtime props. */
bool sc_image_style(sc_obj_t *obj, sc_style_t style, const void *value)
{
    if (style == SC_STYLE_CALLBACK)
    {
        sc_image_props_t *props = (sc_image_props_t *)obj->props;
        props->cb = *(const sc_obj_cb_t *)value;
        if (props->cb)
        {
            obj->flags |= SC_OBJ_F_FOCUSABLE;
        }
        return true;
    }
    return false;
}

/* Handle image events: draw on redraw; press/release tracking and callback via common path. */
bool sc_obj_handle_image_event(sc_obj_t *obj, const sc_event_t *event)
{
    sc_image_props_t *props = (sc_image_props_t *)obj->props;
    if (event->type == SC_EVENT_SYS_DRAW)
    {
        if (props && props->image)
        {
            sc_draw_image((sc_pfb_t *)event->dat.arg, props->image, obj->view.x, obj->view.y);
        }
        return true;
    }
    return sc_obj_common_event(obj, event, props ? props->cb : NULL);
}

#endif /* SC_USER_OBJ && SC_WIDGET_IMAGE_ENABLE */

#endif /* SC_WIDGET_IMAGE_ENABLE */
