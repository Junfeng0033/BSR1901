#ifndef SC_DEMO_LAYOUT_H
#define SC_DEMO_LAYOUT_H

#include "sc_demo_assets.h"

typedef struct
{
    int x;
    int y;
    int w;
    int h;
    const color_t *pixels;
} sc_demo_image_t;

#define SC_DEMO_SCREEN_WIDTH 320
#define SC_DEMO_SCREEN_HEIGHT 240
#define SC_DEMO_PFB_LINES 16
#define SC_DEMO_IMAGE_COUNT 3

extern const sc_demo_image_t g_sc_demo_images[SC_DEMO_IMAGE_COUNT];

#endif
