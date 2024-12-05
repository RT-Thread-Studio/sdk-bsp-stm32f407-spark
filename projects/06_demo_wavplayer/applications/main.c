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
#include "drv_lcd.h"

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

#define GPIO_LED_B    GET_PIN(F, 11)
#define GPIO_LED_R    GET_PIN(F, 12)
int main(void)
{
    lcd_show_string(43,100,32,"Wav Player");
    while (1)
    {
        rt_pin_write(GPIO_LED_R, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(GPIO_LED_R, PIN_LOW);
        rt_thread_mdelay(500);
    }
    return 0;
}

