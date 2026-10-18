#ifndef SC_WAVEFORM_H
#define SC_WAVEFORM_H

#include "../sc_common.h"
#include "../sc_obj.h"
#include "../sc_style.h"

#ifndef SC_WAVEFORM_MAX_CH
#define SC_WAVEFORM_MAX_CH 3U
#endif
#ifndef SC_WAVEFORM_STROKE_WIDTH
#define SC_WAVEFORM_STROKE_WIDTH 2
#endif
#ifndef SC_WAVEFORM_MAX_CHUNKS
#define SC_WAVEFORM_MAX_CHUNKS 8U
#endif

typedef enum
{
    SC_WAVEFORM_SCALE_0_0625X = 0,
    SC_WAVEFORM_SCALE_0_125X = 1,
    SC_WAVEFORM_SCALE_0_25X = 2,
    SC_WAVEFORM_SCALE_0_5X = 3,
    SC_WAVEFORM_SCALE_1X = 4,
    SC_WAVEFORM_SCALE_2X = 5,
    SC_WAVEFORM_SCALE_4X = 6,
    SC_WAVEFORM_SCALE_8X = 7,
    SC_WAVEFORM_SCALE_16X = 8,
    SC_WAVEFORM_SCALE_MAX,
} sc_waveform_scale_t;

typedef struct
{
    int16_t min_y;
    int16_t max_y;
} sc_waveform_y_range_t;

typedef struct
{
    int16_t *buf;
    uint16_t size;
    uint16_t write_ptr;
    uint16_t logic_start;
    color_t color;
} sc_waveform_data_t;

typedef struct sc_waveform_t
{
    rect_t view;
    int16_t x_index_offset;
    int16_t y_center_offset;
    uint8_t x_scale;
    uint8_t y_scale;
    uint8_t opa;
    uint8_t scale_y_axis; /* 0 = edit X with left/right; 1 = edit Y */
    sc_waveform_data_t data[SC_WAVEFORM_MAX_CH];
    sc_waveform_y_range_t last_ranges[SC_WAVEFORM_MAX_CHUNKS];
} sc_waveform_t;

#define SC_WAVEFORM_NEW()                                              \
    {                                                                  \
        .view = {0, 0, 0, 0}, .x_index_offset = 0, .y_center_offset = 0, \
        .x_scale = (uint8_t)SC_WAVEFORM_SCALE_4X,                      \
        .y_scale = (uint8_t)SC_WAVEFORM_SCALE_1X, .opa = 255U,         \
        .scale_y_axis = 0U                                             \
    }

void sc_waveform_init(sc_waveform_t *props, int16_t x, int16_t y, uint16_t w, uint16_t h);
void sc_waveform_set_scale_x(sc_waveform_t *props, int scale_x);
void sc_waveform_set_scale_y(sc_waveform_t *props, int scale_y);
void sc_waveform_data_init(sc_waveform_t *props, uint8_t ch, int16_t *buf, uint32_t size, color_t color);
void sc_waveform_data_put(sc_waveform_t *props, uint8_t ch, int16_t val);
void sc_waveform_get_chunk_dirty(sc_waveform_t *props, area_t *dirty_buf, uint8_t chunk_size);
void sc_waveform_draw(sc_pfb_t *dest, sc_waveform_t *props, area_t *dirty);
void sc_lcd_draw_waveform(sc_pfb_t *pfb, sc_waveform_t *props);

#if SC_USER_OBJ && SC_WIDGET_WAVEFORM_ENABLE
sc_obj_t *sc_waveform_create(sc_obj_t *parent, sc_waveform_t *props);
void sc_waveform_set_geometry(sc_obj_t *obj, int16_t x, int16_t y, uint16_t w, uint16_t h,
                              sc_align_t parent_align);
bool sc_waveform_set_channel(sc_obj_t *obj, uint8_t ch, int16_t *buf, uint32_t size, color_t color);
void sc_waveform_put(sc_obj_t *obj, uint8_t ch, int16_t val);
void sc_waveform_refresh(sc_obj_t *obj);
void sc_waveform_obj_set_scale_x(sc_obj_t *obj, int scale_x);
void sc_waveform_obj_set_scale_y(sc_obj_t *obj, int scale_y);
bool sc_waveform_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_waveform_event(sc_obj_t *obj, const sc_event_t *event);
#endif

#endif
