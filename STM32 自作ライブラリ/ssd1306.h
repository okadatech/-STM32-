/**
 * Auther:S.Watanabe
 * Date:2019/10/30
 * 
 * SSD1306 OLED Library for STM32HAL
 **/
#ifndef _SSD1306_H_
#define _SSD1306_H_

#include <stm32f4xx_hal.h>
#include <stdio.h>
#include <stdarg.h>

#define SSD1306_ADDRESS 0x3C << 1
#define WIDTH 128
#define HEIGHT 64
#define MAX_STR_LEN 21 //per page

extern const uint8_t font_bitmat[][6];
extern I2C_HandleTypeDef *__SSD1306_HI2C__;

static const uint8_t ctrlbyte_mono = 0b10000000;
static const uint8_t ctrlbyte_multi = 0;
static const uint8_t databyte_mono = 0b11000000;
static const uint8_t databyte_multi = 0b01000000;

void oled_init(I2C_HandleTypeDef *handler);
void oled_printf(const char *format,...);
void oled_clear(void);
void oled_setarea(uint8_t x0,uint8_t x1,uint8_t y0,uint8_t y1);

#endif