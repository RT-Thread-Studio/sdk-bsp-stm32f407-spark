/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-09-20     yuanjie      port Adafruit neopixel.
 */

#ifndef __DRV_NEO_PIXEL_H__
#define __DRV_NEO_PIXEL_H__
#include <rtthread.h>
#include <drv_matrix_led.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct neo_pixel_tools
{
    uint32_t (*ColorHSV)(uint16_t hue, uint8_t sat, uint8_t val);
    uint32_t (*gamma32)(uint32_t x);
    void (*rainbow)(uint16_t first_hue, int8_t reps, uint8_t saturation, uint8_t brightness, uint8_t gammify);
} neo_pixel_tools_t;

typedef struct neo_pixel_ops
{
    void (*setPixelColor)(uint16_t n, uint32_t c);
    void (*setPixelColorRGB)(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    void (*setPixelColorRGBW)(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void (*fill)(uint32_t c, uint16_t first, uint16_t count);
    void (*setBrightness)(uint8_t b);
    void (*clear)(void);
    void (*show)(void);
    void (*updateLength)(uint16_t n);
    neo_pixel_tools_t *tool;
} neo_pixel_ops_t;


void neo_pixel_init(neo_pixel_ops_t **ops, uint16_t n);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_MATRIX_LED__ */
