/**
 * ST7789 LCD驱动头文件
 * 支持240x240像素的SPI接口LCD
 */

#ifndef __ST7789_H__
#define __ST7789_H__

#include <stdint.h>
#include <stdbool.h>

/* 屏幕尺寸定义 */
#define ST7789_WIDTH        240
#define ST7789_HEIGHT       240
#define ST7789_XSTART       0
#define ST7789_YSTART       0

/* ST7789命令定义 */
#define ST7789_NOP          0x00
#define ST7789_SWRESET      0x01
#define ST7789_RDDID        0x04
#define ST7789_RDDST        0x09

#define ST7789_SLPIN        0x10
#define ST7789_SLPOUT       0x11
#define ST7789_PTLON        0x12
#define ST7789_NORON        0x13

#define ST7789_INVOFF       0x20
#define ST7789_INVON        0x21
#define ST7789_DISPOFF      0x28
#define ST7789_DISPON       0x29
#define ST7789_CASET        0x2A
#define ST7789_RASET        0x2B
#define ST7789_RAMWR        0x2C
#define ST7789_RAMRD        0x2E

#define ST7789_PTLAR        0x30
#define ST7789_VSCRDEF      0x33
#define ST7789_TEOFF        0x34
#define ST7789_TEON         0x35
#define ST7789_MADCTL       0x36
#define ST7789_VSCRSADD     0x37
#define ST7789_IDMOFF       0x38
#define ST7789_IDMON        0x39
#define ST7789_COLMOD       0x3A
#define ST7789_RDID1        0xDA
#define ST7789_RDID2        0xDB
#define ST7789_RDID3        0xDC
#define ST7789_RDID4        0xDD

/* MADCTL位定义 */
#define ST7789_MADCTL_MY    0x80
#define ST7789_MADCTL_MX    0x40
#define ST7789_MADCTL_MV    0x20
#define ST7789_MADCTL_ML    0x10
#define ST7789_MADCTL_RGB   0x00
#define ST7789_MADCTL_BGR   0x08
#define ST7789_MADCTL_MH    0x04

/* 颜色定义 */
#define ST7789_BLACK        0x0000
#define ST7789_BLUE         0x001F
#define ST7789_RED          0xF800
#define ST7789_GREEN        0x07E0
#define ST7789_CYAN         0x07FF
#define ST7789_MAGENTA      0xF81F
#define ST7789_YELLOW       0xFFE0
#define ST7789_WHITE        0xFFFF

/* 旋转方向定义 */
typedef enum {
    ST7789_ROTATION_0 = 0,    // 0度
    ST7789_ROTATION_90 = 1,   // 90度
    ST7789_ROTATION_180 = 2,  // 180度
    ST7789_ROTATION_270 = 3   // 270度
} st7789_rotation_t;

/* 初始化ST7789 */
int st7789_init(void);

/* 清屏 */
void st7789_clear(uint16_t color);

/* 填充矩形区域 */
void st7789_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/* 绘制像素点 */
void st7789_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

/* 绘制水平线 */
void st7789_draw_hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color);

/* 绘制垂直线 */
void st7789_draw_vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color);

/* 绘制圆形 */
void st7789_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

/* 填充圆形 */
void st7789_fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

/* 绘制位图 */
void st7789_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *bitmap);

/* 设置显示方向 */
void st7789_set_rotation(st7789_rotation_t rotation);

/* 显示开/关 */
void st7789_display_on(bool on);

/* 反显开/关 */
void st7789_invert_display(bool invert);

#endif /* __ST7789_H__ */

