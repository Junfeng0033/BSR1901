#ifndef GIF_QOIA_H
#define GIF_QOIA_H

#include "sc_gui.h"

#define GIF_QOIA_WIDTH 238U
#define GIF_QOIA_HEIGHT 238U
#define GIF_QOIA_FRAME_COUNT 21U
#define GIF_QOIA_GIF_START 0U
#define GIF_QOIA_GIF_END 20U
#define GIF_QOIA_GIF_INTERVAL 1U
/* Designer may redefine after merging bins into one flash image. */
#ifndef GIF_QOIA_FLASH_BASE
#define GIF_QOIA_FLASH_BASE 0x00000000UL
#endif
#define GIF_QOIA_FLASH_SIZE 195880UL
/* Packed indexQOI lossy blobs in external Flash; frame indices (21): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 */

/* GIF frame 0 @ flash+0 */
static const sc_image_qoi_flash_t gif_qoia_f0_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 0UL,
  .blob_size = 9249U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 1 @ flash+9249 */
static const sc_image_qoi_flash_t gif_qoia_f1_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 9249UL,
  .blob_size = 9110U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 2 @ flash+18359 */
static const sc_image_qoi_flash_t gif_qoia_f2_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 18359UL,
  .blob_size = 9294U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 3 @ flash+27653 */
static const sc_image_qoi_flash_t gif_qoia_f3_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 27653UL,
  .blob_size = 9355U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 4 @ flash+37008 */
static const sc_image_qoi_flash_t gif_qoia_f4_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 37008UL,
  .blob_size = 9279U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 5 @ flash+46287 */
static const sc_image_qoi_flash_t gif_qoia_f5_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 46287UL,
  .blob_size = 9492U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 6 @ flash+55779 */
static const sc_image_qoi_flash_t gif_qoia_f6_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 55779UL,
  .blob_size = 9441U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 7 @ flash+65220 */
static const sc_image_qoi_flash_t gif_qoia_f7_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 65220UL,
  .blob_size = 9363U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 8 @ flash+74583 */
static const sc_image_qoi_flash_t gif_qoia_f8_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 74583UL,
  .blob_size = 9254U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 9 @ flash+83837 */
static const sc_image_qoi_flash_t gif_qoia_f9_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 83837UL,
  .blob_size = 9264U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 10 @ flash+93101 */
static const sc_image_qoi_flash_t gif_qoia_f10_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 93101UL,
  .blob_size = 9524U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 11 @ flash+102625 */
static const sc_image_qoi_flash_t gif_qoia_f11_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 102625UL,
  .blob_size = 9459U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 12 @ flash+112084 */
static const sc_image_qoi_flash_t gif_qoia_f12_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 112084UL,
  .blob_size = 9280U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 13 @ flash+121364 */
static const sc_image_qoi_flash_t gif_qoia_f13_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 121364UL,
  .blob_size = 9194U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 14 @ flash+130558 */
static const sc_image_qoi_flash_t gif_qoia_f14_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 130558UL,
  .blob_size = 9396U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 15 @ flash+139954 */
static const sc_image_qoi_flash_t gif_qoia_f15_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 139954UL,
  .blob_size = 9468U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 16 @ flash+149422 */
static const sc_image_qoi_flash_t gif_qoia_f16_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 149422UL,
  .blob_size = 9323U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 17 @ flash+158745 */
static const sc_image_qoi_flash_t gif_qoia_f17_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 158745UL,
  .blob_size = 9346U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 18 @ flash+168091 */
static const sc_image_qoi_flash_t gif_qoia_f18_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 168091UL,
  .blob_size = 9210U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 19 @ flash+177301 */
static const sc_image_qoi_flash_t gif_qoia_f19_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 177301UL,
  .blob_size = 9349U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

/* GIF frame 20 @ flash+186650 */
static const sc_image_qoi_flash_t gif_qoia_f20_src = {
  .blob_addr = GIF_QOIA_FLASH_BASE + 186650UL,
  .blob_size = 9230U,
  .u16_size = 476U,
  .u24_size = 0U,
  .u32_size = 0U,
};

const sc_image_t gif_qoia[GIF_QOIA_FRAME_COUNT] = {
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f0_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f1_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f2_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f3_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f4_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f5_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f6_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f7_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f8_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f9_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f10_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f11_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f12_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f13_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f14_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f15_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f16_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f17_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f18_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f19_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT),
    SC_IMAGE_QOI_FLASH_NEW(&gif_qoia_f20_src, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT)
};

#endif /* GIF_QOIA_H */
