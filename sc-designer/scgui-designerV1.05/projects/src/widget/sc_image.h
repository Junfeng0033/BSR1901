#ifndef SC_WIDGET_IMAGE_H
#define SC_WIDGET_IMAGE_H

#include "../sc_common.h"

/*
 * One image type covers both storage locations and every pixel format.
 * The low bits of type hold the pixel format; SC_IMAGE_FLASH marks the data
 * union as an external-flash byte address instead of a RAM/ROM pointer.
 * Flash data is read on demand during drawing (see SC_WIDGET_IMAGE_EX_ENABLE).
 * Pixels are row-major; RGB565 is little-endian, RGBA8888 is R,G,B,A byte order.
 *
 * SC_IMAGE_RGB565A8 is planar and needs no extra pointer: a single base
 * (data.pixels or data.flash_addr) points at the RGB565 color plane, followed
 * by the A8 alpha plane. Color first keeps the color plane 2-byte aligned and
 * needs no padding, since the trailing A8 plane is byte addressed:
 *     [ RGB565 color : w*h*2 ][ A8 mask : w*h ]
 * The trailing A8 plane is a standalone alpha mask that can be fed straight to
 * sc_draw_icon() with bpp = 8 (see SC_IMAGE_A8_MASK_PTR).
 *
 * SC_IMAGE_QOI is an indexQOI RGB565 blob (header + per-row offsets + stream).
 * MCU decoder assumes row index n=1 (header interval equals image width).
 * Lossy blobs (RGB565+A4 in stream, tag 00 raw) are still SC_IMAGE_QOI; payload
 * is auto-detected and alpha-blended on draw. Lossless uses 0xFE/0xFF raw RGB565.
 * External-flash blobs store the full blob (9-byte head + index + payload) at
 * blob_addr; drawing reads the head and only the index entries for the current
 * PFB row span, then streams payload through an 18-byte temporary buffer (no persistent per-image RAM).
 */
enum
{
    SC_IMAGE_RGB565 = 0,
    SC_IMAGE_RGBA8888 = 1,  /* enable decoder with SC_WIDGET_IMAGE_RGB888_ENABLE */
    SC_IMAGE_RGB565A8 = 2, /* RGB565 color plane followed by A8 mask plane */
    SC_IMAGE_QOI = 3,      /* indexQOI row-compressed RGB565 */
    SC_IMAGE_FMT_MASK = 0x7F,
    SC_IMAGE_FLASH = 0x80,
};



/* Byte offset of the trailing A8 mask plane inside a RGB565A8 buffer. */
#define SC_IMAGE_A8_MASK_OFFSET(w, h) ((uint32_t)(w) * (uint32_t)(h) * 2U)
/* Total byte size of a full RGB565A8 buffer: color plane (w*h*2) + mask plane (w*h). */
#define SC_IMAGE_RGB565A8_BYTES(w, h) ((uint32_t)(w) * (uint32_t)(h) * 3U)
/* Pointer to the standalone A8 mask plane of a RAM/ROM RGB565A8 image (bpp = 8 for sc_draw_icon). */
#define SC_IMAGE_A8_MASK_PTR(img) \
    ((const uint8_t *)(img)->data.pixels + SC_IMAGE_A8_MASK_OFFSET((img)->width, (img)->height))

/*
 * Compile-time guard for a RAM/ROM RGB565A8 asset: buf must be the real array
 * (not a pointer) and hold both planes (w*h*3 bytes). A mask-only export, where
 * the color/data plane is disabled and only w*h bytes remain, fails this check
 * so it cannot be handed to image drawing by mistake.
 */
#define SC_IMAGE_RGB565A8_ASSERT(buf, w, h) \
    SC_STATIC_ASSERT(sizeof(buf) >= SC_IMAGE_RGB565A8_BYTES(w, h), rgb565a8_needs_color_plane)

/* Full indexQOI blob in external Flash: [9-byte head][index][payload].
 * u16/u24/u32_size mirror the header bytes 3-8 so the decoder needs no
 * per-slice flash header read (SC_LCD_DRAW slices one row at a time). */
typedef struct sc_image_qoi_flash_t
{
    uint32_t blob_addr;
    uint32_t blob_size;
    uint16_t u16_size;
    uint16_t u24_size;
    uint16_t u32_size;
} sc_image_qoi_flash_t;

#define SC_IMAGE_NEW(pixels_, width_, height_, type_) \
    {.data.pixels = (pixels_), .width = (width_), .height = (height_), .type = (uint8_t)(type_)}

/* EX 关时引用 FLASH 资源宏 → 展开为未定义标识符 → 编译报错即提示：
 * 在 sc_config.h 里把 SC_WIDGET_IMAGE_EX_ENABLE 改为 1 后重新编译。
 * 配置必须与资源存储一致（外部 flash 图片），防止把 flash 地址当 RAM 指针的运行时崩溃。 */
#if !SC_WIDGET_IMAGE_EX_ENABLE
#define SC_IMAGE_FLASH_NEW(flash_addr_, width_, height_, type_) \
    SC_PLEASE_ENABLE_SC_WIDGET_IMAGE_EX_ENABLE_IN_CONFIG_H
#define SC_IMAGE_QOI_FLASH_NEW(source_, width_, height_) \
    SC_PLEASE_ENABLE_SC_WIDGET_IMAGE_EX_ENABLE_IN_CONFIG_H
#else
#define SC_IMAGE_FLASH_NEW(flash_addr_, width_, height_, type_) \
    {.data.flash_addr = (flash_addr_), .width = (width_), .height = (height_), .type = (uint8_t)(SC_IMAGE_FLASH | (type_))}
#define SC_IMAGE_QOI_FLASH_NEW(source_, width_, height_) \
    {.data.pixels = (source_), .width = (width_), .height = (height_), \
     .type = (uint8_t)(SC_IMAGE_FLASH | SC_IMAGE_QOI)}
#endif

#include "../sc_obj.h"

typedef struct sc_image_t
{
    union
    {
        const void *pixels;
        uint32_t flash_addr;
    } data;
    uint16_t width;
    uint16_t height;
    uint8_t type;
} sc_image_t;

/* 运行时 image 控件 props（RAM，每个 image 控件一份）：常量资源外层套一层，
 * 资源可共享（多帧数组），回调归控件。sc_img_create 内部创建，API 仍收 const 资源。 */
typedef struct sc_image_props_t
{
    const sc_image_t *image; /* 指向 const 资源/帧（资源本体只读） */
    sc_obj_cb_t cb;          /* 用户事件回调 */
} sc_image_props_t;

void sc_draw_image(sc_pfb_t *pfb, const sc_image_t *props, int16_t x, int16_t y);
void sc_lcd_draw_image(sc_pfb_t *pfb, int16_t x, int16_t y, const sc_image_t *props);

#if SC_USER_OBJ && SC_WIDGET_IMAGE_ENABLE
sc_obj_t *sc_img_create(sc_obj_t *parent, sc_image_props_t *props);
void sc_image_set_img(sc_obj_t *obj, const sc_image_t *img);
bool sc_image_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_image_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
