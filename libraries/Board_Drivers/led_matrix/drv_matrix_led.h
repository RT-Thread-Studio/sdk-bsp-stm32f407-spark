/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-05-22     yuanjie      first version, transfer using PWM method.
 * 2023-09-13     yuanjie      twick few functions.
 */

#ifndef __DRV_MATRIX_LED_H__
#define __DRV_MATRIX_LED_H__
#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LED_ONBOARD_MATRIX_EN_PIN   GET_PIN(F, 2)

typedef struct pixel_rgb
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    // uint8_t w;  //reserved
} pixel_rgb_t; // 颜色结构体，4字节

// 常见颜色定义
extern const pixel_rgb_t DARK;
extern const pixel_rgb_t GREEN;
extern const pixel_rgb_t RED;
extern const pixel_rgb_t BLUE;
extern const pixel_rgb_t WHITE;
extern const pixel_rgb_t LT_RED;
extern const pixel_rgb_t LT_GREEN;
extern const pixel_rgb_t LT_BLUE;
extern const pixel_rgb_t LT_WHITE;

int led_matrix_init(void);
void led_matrix_clear(void);
void led_matrix_reflash(void);
void led_matrix_set_color(uint16_t n, pixel_rgb_t c);
void led_matrix_fill(pixel_rgb_t pix, uint16_t first, uint16_t count);
void led_matrix_fill_rgb(uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_MATRIX_LED__ */

