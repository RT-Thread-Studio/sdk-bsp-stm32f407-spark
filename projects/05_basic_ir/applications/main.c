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
#include "infrared.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* 配置 KEY 输入引脚  */
#define PIN_KEY0        GET_PIN(C, 0)     // PC0:  KEY0         --> KEY
#define PIN_KEY1        GET_PIN(C, 1)      // PC1 :  KEY1         --> KEY
#define PIN_KEY2        GET_PIN(C, 4)      // PC4 :  KEY2         --> KEY
#define PIN_WK_UP       GET_PIN(C, 5)     // PC5:  WK_UP        --> KEY

/* 配置 LED 灯引脚 */
#define PIN_LED_B              GET_PIN(F, 11)      // PF11 :  LED_B        --> LED
#define PIN_LED_R              GET_PIN(F, 12)      // PF12 :  LED_R        --> LED

rt_int16_t key_scan(void)
{
    if (rt_pin_read(PIN_KEY0) == PIN_LOW)
    {
        rt_thread_mdelay(50);
        if (rt_pin_read(PIN_KEY0) == PIN_LOW)
        {
            return PIN_KEY0;
        }
    }
    return -RT_ERROR;
}

int main(void)
{
    unsigned int count = 1;
    rt_int16_t key;
    struct infrared_decoder_data infrared_data;

    /* 选择 NEC 解码器 */
    ir_select_decoder("nec");

    /* 设置按键引脚为输入模式 */
    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);

    /* 设置 RGB 引脚为输出模式*/
    rt_pin_mode(PIN_LED_R, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_LED_B, PIN_MODE_OUTPUT);

    rt_pin_write(PIN_LED_R, PIN_HIGH);
    rt_pin_write(PIN_LED_B, PIN_HIGH);

    while (count > 0)
    {
        /* 按键扫描 */
        key = key_scan();
        if(key == PIN_KEY0)
        {
            /* 有按键按下，蓝灯亮起 */
            rt_pin_write(PIN_LED_B, PIN_LOW);
            infrared_data.data.nec.repeat = 0;
            /* 发送红外数据 */
            infrared_write("nec",&infrared_data);
            rt_thread_mdelay(200);
            LOG_I("SEND    OK: addr:0x%02X key:0x%02X repeat:%d",
                infrared_data.data.nec.addr, infrared_data.data.nec.key, infrared_data.data.nec.repeat);
        }
        else if(infrared_read("nec",&infrared_data) == RT_EOK)  
        {
            /* 读取到红外数据，红灯亮起 */
            rt_pin_write(PIN_LED_R, PIN_LOW);
            LOG_I("RECEIVE OK: addr:0x%02X key:0x%02X repeat:%d",
                infrared_data.data.nec.addr, infrared_data.data.nec.key, infrared_data.data.nec.repeat);
        }
        rt_thread_mdelay(10);

        /* 熄灭蓝灯 */
        rt_pin_write(PIN_LED_B, PIN_HIGH);
        /* 熄灭红灯 */
        rt_pin_write(PIN_LED_R, PIN_HIGH);
        count++;
    }
    return 0;
}

