/**
 * ST7789 LCD驱动模块
 * 基于AliOS-Things的ST7789驱动库实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drv_gpio.h"
#include "drv_spi.h"
#include "st7789.h"

// 引脚定义
#define ST7789_DC_PIN       2   // 数据/命令控制引脚
#define ST7789_RST_PIN      3   // 复位引脚
#define ST7789_CS_PIN       4   // 片选引脚
#define ST7789_BL_PIN       5   // 背光控制引脚

// SPI设备句柄
static spi_dev_t st7789_spi;

// 延时函数(ms)
static void st7789_delay_ms(uint32_t ms)
{
    aos_msleep(ms);
}

// 发送命令
static void st7789_send_cmd(uint8_t cmd)
{
    // 设置DC引脚为低(命令模式)
    drv_gpio_write(ST7789_DC_PIN, 0);
    
    // 发送命令
    uint8_t buffer[1] = {cmd};
    drv_spi_send(&st7789_spi, buffer, 1);
}

// 发送数据
static void st7789_send_data(uint8_t data)
{
    // 设置DC引脚为高(数据模式)
    drv_gpio_write(ST7789_DC_PIN, 1);
    
    // 发送数据
    uint8_t buffer[1] = {data};
    drv_spi_send(&st7789_spi, buffer, 1);
}

// 发送16位数据
static void st7789_send_data16(uint16_t data)
{
    // 设置DC引脚为高(数据模式)
    drv_gpio_write(ST7789_DC_PIN, 1);
    
    // 发送数据(高字节在前)
    uint8_t buffer[2] = {data >> 8, data & 0xFF};
    drv_spi_send(&st7789_spi, buffer, 2);
}

// 发送数据块
static void st7789_send_data_block(uint8_t *data, uint32_t len)
{
    // 设置DC引脚为高(数据模式)
    drv_gpio_write(ST7789_DC_PIN, 1);
    
    // 发送数据块
    drv_spi_send(&st7789_spi, data, len);
}

// 硬复位ST7789
static void st7789_hard_reset(void)
{
    // 复位引脚高->低->高
    drv_gpio_write(ST7789_RST_PIN, 1);
    st7789_delay_ms(10);
    drv_gpio_write(ST7789_RST_PIN, 0);
    st7789_delay_ms(10);
    drv_gpio_write(ST7789_RST_PIN, 1);
    st7789_delay_ms(120);
}

// 设置显示窗口
static void st7789_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    st7789_send_cmd(ST7789_CASET); // Column address set
    st7789_send_data(x0 >> 8);
    st7789_send_data(x0 & 0xFF);
    st7789_send_data(x1 >> 8);
    st7789_send_data(x1 & 0xFF);

    st7789_send_cmd(ST7789_RASET); // Row address set
    st7789_send_data(y0 >> 8);
    st7789_send_data(y0 & 0xFF);
    st7789_send_data(y1 >> 8);
    st7789_send_data(y1 & 0xFF);

    st7789_send_cmd(ST7789_RAMWR); // Write to RAM
}

// 初始化ST7789
int st7789_init(void)
{
    int ret;
    
    // 配置SPI设备
    st7789_spi.port = 0;  // SPI端口号
    st7789_spi.config.mode = SPI_MODE_0;
    st7789_spi.config.freq = 40000000;  // 40MHz
    st7789_spi.config.data_width = 8;
    st7789_spi.config.chip_select = ST7789_CS_PIN;
    
    // 初始化SPI
    ret = drv_spi_init(&st7789_spi);
    if (ret != 0) {
        printf("SPI初始化失败\n");
        return -1;
    }
    
    // 配置控制引脚
    drv_gpio_mode(ST7789_DC_PIN, OUTPUT_PUSH_PULL);
    drv_gpio_mode(ST7789_RST_PIN, OUTPUT_PUSH_PULL);
    drv_gpio_mode(ST7789_BL_PIN, OUTPUT_PUSH_PULL);
    
    // 复位ST7789
    st7789_hard_reset();
    
    // 开始初始化序列
    st7789_send_cmd(ST7789_SWRESET);    // 软复位
    st7789_delay_ms(150);
    
    st7789_send_cmd(ST7789_SLPOUT);     // 退出睡眠模式
    st7789_delay_ms(500);
    
    st7789_send_cmd(ST7789_COLMOD);     // 设置色彩模式
    st7789_send_data(0x05);             // 16位色彩模式(RGB565)
    st7789_delay_ms(10);
    
    st7789_send_cmd(ST7789_MADCTL);     // 内存数据访问控制
    st7789_send_data(0x00);             // 竖屏模式
    st7789_delay_ms(10);
    
    st7789_send_cmd(ST7789_CASET);      // 设置列地址
    st7789_send_data(0x00);
    st7789_send_data(0x00);
    st7789_send_data(0x00);
    st7789_send_data(0xEF);
    
    st7789_send_cmd(ST7789_RASET);      // 设置行地址
    st7789_send_data(0x00);
    st7789_send_data(0x00);
    st7789_send_data(0x01);
    st7789_send_data(0x3F);
    
    st7789_send_cmd(ST7789_INVON);      // 反转显示
    st7789_delay_ms(10);
    
    st7789_send_cmd(ST7789_NORON);      // 正常显示模式
    st7789_delay_ms(10);
    
    st7789_send_cmd(ST7789_DISPON);     // 打开显示
    st7789_delay_ms(100);
    
    // 打开背光
    drv_gpio_write(ST7789_BL_PIN, 1);
    
    return 0;
}

// 清屏
void st7789_clear(uint16_t color)
{
    st7789_fill_rect(0, 0, ST7789_WIDTH, ST7789_HEIGHT, color);
}

// 填充矩形区域
void st7789_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint16_t i, j;
    uint8_t data[2];
    
    data[0] = color >> 8;
    data[1] = color & 0xFF;
    
    st7789_set_addr_window(x, y, x + w - 1, y + h - 1);
    
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            st7789_send_data_block(data, 2);
        }
    }
}

// 绘制像素点
void st7789_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    st7789_set_addr_window(x, y, x, y);
    st7789_send_data16(color);
}

// 绘制水平线
void st7789_draw_hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{
    st7789_set_addr_window(x, y, x + w - 1, y);
    
    uint8_t data[2] = {color >> 8, color & 0xFF};
    for (uint16_t i = 0; i < w; i++) {
        st7789_send_data_block(data, 2);
    }
}

// 绘制垂直线
void st7789_draw_vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color)
{
    st7789_set_addr_window(x, y, x, y + h - 1);
    
    uint8_t data[2] = {color >> 8, color & 0xFF};
    for (uint16_t i = 0; i < h; i++) {
        st7789_send_data_block(data, 2);
    }
}

// 绘制圆形(中点圆算法)
void st7789_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    int16_t x = 0;
    int16_t y = r;
    int16_t d = 3 - 2 * r;
    
    while (x <= y) {
        st7789_draw_pixel(x0 + x, y0 + y, color);
        st7789_draw_pixel(x0 - x, y0 + y, color);
        st7789_draw_pixel(x0 + x, y0 - y, color);
        st7789_draw_pixel(x0 - x, y0 - y, color);
        st7789_draw_pixel(x0 + y, y0 + x, color);
        st7789_draw_pixel(x0 - y, y0 + x, color);
        st7789_draw_pixel(x0 + y, y0 - x, color);
        st7789_draw_pixel(x0 - y, y0 - x, color);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// 填充圆形
void st7789_fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    int16_t x = 0;
    int16_t y = r;
    int16_t d = 3 - 2 * r;
    
    while (x <= y) {
        for (int16_t i = x0 - y; i <= x0 + y; i++) {
            st7789_draw_pixel(i, y0 + x, color);
            st7789_draw_pixel(i, y0 - x, color);
        }
        
        for (int16_t i = x0 - x; i <= x0 + x; i++) {
            st7789_draw_pixel(i, y0 + y, color);
            st7789_draw_pixel(i, y0 - y, color);
        }
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// 显示位图
void st7789_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *bitmap)
{
    uint32_t i;
    uint8_t buffer[2048];
    uint32_t pixels = w * h;
    uint32_t offset = 0;
    
    st7789_set_addr_window(x, y, x + w - 1, y + h - 1);
    
    // 分块传输数据以避免内存溢出
    while (pixels > 0) {
        uint32_t chunk = pixels > 1024 ? 1024 : pixels;
        
        for (i = 0; i < chunk; i++) {
            buffer[i*2] = bitmap[offset + i] >> 8;
            buffer[i*2 + 1] = bitmap[offset + i] & 0xFF;
        }
        
        st7789_send_data_block(buffer, chunk * 2);
        
        offset += chunk;
        pixels -= chunk;
    }
}

// 设置显示方向
void st7789_set_rotation(st7789_rotation_t rotation)
{
    st7789_send_cmd(ST7789_MADCTL);
    
    switch (rotation) {
        case ST7789_ROTATION_0:
            st7789_send_data(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
            break;
        case ST7789_ROTATION_90:
            st7789_send_data(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
            break;
        case ST7789_ROTATION_180:
            st7789_send_data(ST7789_MADCTL_RGB);
            break;
        case ST7789_ROTATION_270:
            st7789_send_data(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
            break;
    }
}

// 显示开/关
void st7789_display_on(bool on)
{
    if (on) {
        st7789_send_cmd(ST7789_DISPON);
    } else {
        st7789_send_cmd(ST7789_DISPOFF);
    }
}

// 反显开/关
void st7789_invert_display(bool invert)
{
    if (invert) {
        st7789_send_cmd(ST7789_INVON);
    } else {
        st7789_send_cmd(ST7789_INVOFF);
    }
}

