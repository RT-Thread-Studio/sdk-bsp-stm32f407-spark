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

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

/* 配置 LED 灯引脚 */
#define PIN_LED_B              GET_PIN(F, 11)      // PE7 :  LED_R        --> LED
#define PIN_LED_R              GET_PIN(F, 12)      // PE7 :  LED_R        --> LED


int main(void)
{
     unsigned int count = 1;

     /* 设置 LED 引脚为输出模式 */
     rt_pin_mode(PIN_LED_R, PIN_MODE_OUTPUT);
     while (count > 0)
     {
         /* LED 灯亮 */
         rt_pin_write(PIN_LED_R, PIN_LOW);
         rt_thread_mdelay(500);

         /* LED 灯灭 */
         rt_pin_write(PIN_LED_R, PIN_HIGH);
         rt_thread_mdelay(500);

         count++;
     }
    return 0;
}

