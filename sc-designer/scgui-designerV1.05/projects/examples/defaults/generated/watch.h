#ifndef WATCH_H
#define WATCH_H

#include "sc_gui.h"

#define WATCH_WIDTH 215U
#define WATCH_HEIGHT 200U
#define WATCH_QOI_INDEX_N 1U
#define WATCH_QOI_PIXEL_STRIDE 215U
#ifndef WATCH_QOI_BLOB_ADDR
#define WATCH_QOI_BLOB_ADDR 0x00000000UL
#endif
#define WATCH_QOI_BLOB_SIZE 42039UL
#define WATCH_QOI_HEAD_INDEX_BYTES 409U
#define WATCH_QOI_PAYLOAD_SIZE 41630UL

/* Full indexQOI blob lives in external Flash at BLOB_ADDR (head+index+payload).
 * u16/u24/u32_size mirror blob header bytes 3-8 so the decoder skips the
 * per-slice flash header read (SC_LCD_DRAW slices one row at a time). */
static const sc_image_qoi_flash_t watch_qoi_source = {
  .blob_addr = WATCH_QOI_BLOB_ADDR,
  .blob_size = WATCH_QOI_BLOB_SIZE,
  .u16_size = 400U,
  .u24_size = 0U,
  .u32_size = 0U,
};

const sc_image_t watch =
  SC_IMAGE_QOI_FLASH_NEW(&watch_qoi_source, WATCH_WIDTH, WATCH_HEIGHT);

#endif /* WATCH_H */
