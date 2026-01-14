#ifndef COMPOSE_H
#define COMPOSE_H

#include "sc_common.h"
#include "sc_lvgl_font.h"

typedef struct sc_textbox_t
{
    sc_label_t label;
    int32_t xoffs:12;
    int32_t yoffs:12;
    int32_t bar_w:8;
} sc_textbox_t;

typedef struct sc_button_t
{
    sc_label_t label; // label
    uint8_t state;
} sc_button_t;

typedef struct
{
    int16_t   dat_buf[220];
    int16_t   last;
    uint16_t  wp;      //队列
    uint32_t  indx;    //当前显示位置
    color_t    color;
} sc_chart_t;


void sc_draw_Led(sc_pfb_t *dest,int cx,int cy,int r, color_t color,uint16_t alpha);

void sc_init_Button(sc_button_t *button, lv_font_t *font, char *text, color_t txtcolor);

void sc_draw_Button(sc_pfb_t *dest,int xs,int ys,int w,int h,uint8_t r,uint8_t ir,color_t ac,color_t fill,sc_button_t *button);

void sc_draw_Switch(sc_pfb_t *dest,int xs,int ys,int w,int h, color_t ac,color_t fill,uint8_t state);

void sc_init_Textbox(sc_textbox_t *textbox, lv_font_t *font, char *text, color_t txtcolor);

void sc_draw_Textbox(sc_pfb_t *dest, int xs, int ys, int w, int h, color_t boxcolor, sc_textbox_t *textbox);

void sc_draw_Slider(sc_pfb_t *dest, int xs, int ys, int w, int h, color_t ac, color_t fill, int vol, int max);

void sc_draw_Chart(sc_pfb_t *dest, int xs, int ys, int w, int h,uint16_t gc, int gx, int gy, sc_chart_t *p, int size);

void sc_put_Chart_ch(sc_chart_t *p, int16_t vol, uint16_t scaleX,color_t color);
#endif 

