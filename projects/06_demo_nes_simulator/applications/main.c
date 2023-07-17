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
#include <InfoNES.h>

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

#define PIN_LED_B              GET_PIN(F, 11)      // PE7 :  LED_R        --> LED
#define PIN_LED_R              GET_PIN(F, 12)      // PE7 :  LED_R        --> LED

void show();

int main(void)
{
    unsigned int count = 1;

    rt_pin_mode(PIN_LED_R, PIN_MODE_OUTPUT);

    if(InfoNES_Load(NULL)==0)
    {
        rt_thread_t nes_thread = rt_thread_create("nes", InfoNES_Main, RT_NULL, 40960*2, 21, 10000);
        if(nes_thread == RT_NULL) rt_kprintf("nes thread create failed!\n");
        rt_thread_startup(nes_thread);
        LOG_D("nes start");

    }else{
        LOG_E("nes error");
    }

    while (count > 0)
    {
        rt_pin_write(PIN_LED_R, PIN_LOW);
        rt_thread_mdelay(500);

        rt_pin_write(PIN_LED_R, PIN_HIGH);
        rt_thread_mdelay(500);

        count++;

    }

    return 0;
}

