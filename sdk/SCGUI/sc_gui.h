#ifndef SC_GUI_H
#define SC_GUI_H

#include "sc_common.h"
#include "sc_lvgl_font.h"
#include "sc_arc.h"
#include "sc_transform.h"
#include "sc_compose.h"

 void spiflash_read(uint8_t *buf, uint32_t offset, uint32_t size);

extern void lcd_dma_refresh(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color);

typedef void (*lcd_refresh_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, color_t *color); // 点绘制函数指针

/* SC_GUI结构体 */
typedef struct sc_gui_t
{
    sc_rect_t lcd_rect;        // 屏幕矩形
    sc_rect_t *g_mask;         // 屏幕矩形
    lcd_refresh_cb refresh_cb; // 底层刷新函数
    void *font;                // 主字体
    uint16_t bkc;              // 主题底色
    uint16_t fc;               // 前影色
    uint16_t bc;               // 背影色
    uint16_t alpha;            // 透明度
} sc_gui_t;
extern sc_gui_t *gui;

/* 图片结构体 */
// typedef  sc_image_t  SC_image_t;            //上位机命名不规范，这里统一命名
typedef struct sc_image_t
{
    const uint8_t *map;
    const uint8_t *mask;
    uint32_t w : 12;
    uint32_t h : 12;
    uint32_t bpp : 8;
} sc_image_t;

typedef struct
{
    const uint8_t *map;
    uint32_t   len;
    uint16_t w;
    uint16_t h;
} SC_img_zip;

typedef struct
{
    uint32_t n;
    int16_t  x;
    int16_t  y;
    uint16_t rep_cnt;
    uint16_t out;
    uint16_t unzip;
}sc_dec_zip;

/*初始化gui*/
void sc_gui_init(lcd_refresh_cb refresh_cb, color_t bkc, color_t fc, color_t bc, void *font); // 初始化

/*清屏*/
void sc_clear(int x, int y, int w, int h, color_t color); // 清屏

/*绘制矩形*/
void sc_draw_Fill(sc_pfb_t *dest, int xs, int ys, int w, int h, color_t color, uint16_t alpha);

/*空心矩形*/
void sc_draw_Frame(sc_pfb_t *dest, int xs, int ys, int w, int h, int lw, color_t color, uint16_t alpha);

// 绘制圆角矩形
bool sc_draw_Rounded_rect(sc_pfb_t *dest, int xs, int ys, int w, int h, int r, int ir, color_t color, color_t bcolor, uint16_t alpha);

// 绘制圆形进度条
void sc_draw_Bar(sc_pfb_t *dest, int xs, int ys, int w, int h, int r, int ir, color_t ac, color_t fill, int vol, int max);

// 绘制一个icon字符
void sc_draw_Icon(sc_pfb_t *dest, int xs, int ys, int w, int h, const uint8_t *src, uint8_t bpp, color_t fc, uint16_t alpha);

// 图片显示
void sc_draw_Image(sc_pfb_t *dest, int xs, int ys, const sc_image_t *src, uint32_t spi_addr);

void sc_drwa_Image_zip(sc_pfb_t *dest, int xs, int ys, const SC_img_zip *zip, sc_dec_zip *dec,uint32_t spi_addr);
// 绘制线
void sc_draw_Line_AA(sc_pfb_t *dest, int x1, int y1, int x2, int y2, color_t colour);

#endif
