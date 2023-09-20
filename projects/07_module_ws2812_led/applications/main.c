/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <rtthread.h>
#include <drv_neo_pixel.h>

#define LED_NUMS   (19 + 30) /* LED 的数目 */
#define LED_BRIGHTNESS 128   /* LED 的亮度 */
#define LED_REPS       1     /* LED 颜色的周期数 */
#define LED_SATURATION 255   /* LED 的饱和度 */

int main(void)
{
    neo_pixel_ops_t *neo_ops;
    uint16_t hue = 0; /* 第一个灯珠的颜色 */

    /* 初始化 neo_pixel 库 */
    neo_pixel_init(&neo_ops, LED_NUMS);
    /* 等待驱动初始化完成 */
    rt_thread_mdelay(10);

    while (1)
    {
        /* 生成彩虹序列 */
        neo_ops->tool->rainbow(hue, LED_REPS, LED_SATURATION, LED_BRIGHTNESS, RT_TRUE);
        /* 显示 */
        neo_ops->show();
        /* 刷新周期10ms */
        rt_thread_mdelay(10);
        hue += 100;
    }
    return 0;
}
