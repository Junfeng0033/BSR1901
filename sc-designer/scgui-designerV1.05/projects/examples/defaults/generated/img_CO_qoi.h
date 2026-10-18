#ifndef IMG_CO_QOI_H
#define IMG_CO_QOI_H

#include <stdint.h>

#define IMG_CO_WIDTH 43U
#define IMG_CO_HEIGHT 41U
#define IMG_CO_QOI_INDEX_N 1U
#define IMG_CO_QOI_PIXEL_STRIDE 43U /* n×width; same value as blob header u16 */
#define IMG_CO_QOI_BYTES 2251U

extern const uint8_t img_CO_43x41[IMG_CO_QOI_BYTES];

#endif
