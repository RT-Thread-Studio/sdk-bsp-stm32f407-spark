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
#include <rtdevice.h>
#include <board.h>
#include <drv_matrix_led.h>

/* define LED  */
enum{
    EXTERN_LED_0,
    EXTERN_LED_1,
    EXTERN_LED_2,
    EXTERN_LED_3,
    EXTERN_LED_4,
    EXTERN_LED_5,
    EXTERN_LED_6,
    EXTERN_LED_7,
    EXTERN_LED_8,
    EXTERN_LED_9,
    EXTERN_LED_10,
    EXTERN_LED_11,
    EXTERN_LED_12,
    EXTERN_LED_13,
    EXTERN_LED_14,
    EXTERN_LED_15,
    EXTERN_LED_16,
    EXTERN_LED_17,
    EXTERN_LED_18,
};

rt_thread_t led_matrix_thread;

static void led_matrix_example_entry()
{
    int count = 0;
    while (1)
    {
        for (int i = EXTERN_LED_0; i <= EXTERN_LED_18; i++)
        {
            switch (count)
            {
            case 0:
                led_matrix_set_color(i, RED);
                break;
            case 1:
                led_matrix_set_color(i, GREEN);
                break;
            case 2:
                led_matrix_set_color(i, BLUE);
                break;
            default:
                return;
                break;
            }
            led_matrix_reflash();
            rt_thread_delay(20);
        }
        count = (count + 1) % 3;
    }
}

int main(void)
{
    led_matrix_thread = rt_thread_create("led matrix demo", led_matrix_example_entry, RT_NULL, 1024, 20, 20);
    if(led_matrix_thread == RT_NULL)
    {
        rt_kprintf("led matrix demo thread creat failed!\n");
        return 0;
    }
    rt_thread_mdelay(200); // avoid multi-thread on LED matrix transmit.
    rt_thread_startup(led_matrix_thread);

    return 0;
}

