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
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* 配置 KEY 输入引脚  */
#define PIN_KEY1        GET_PIN(C, 1)      // PC1:  KEY1         --> KEY
#define PIN_WK_UP       GET_PIN(C, 5)      // PC5:  WK_UP        --> KEY

/* 配置蜂鸣器引脚 */
#define PIN_BEEP        GET_PIN(B, 0)      // PA1:  BEEP         --> BEEP (PB1)

/* 中断回调 */
void irq_callback(void *args)
{
    rt_uint32_t sign = (rt_uint32_t) args;
    switch (sign)
    {
    case PIN_WK_UP :
        rt_pin_write(PIN_BEEP,PIN_HIGH);
        LOG_D("WK_UP interrupt. beep on.");
        break;
    case PIN_KEY1 :
        rt_pin_write(PIN_BEEP,PIN_LOW);
        LOG_D("KEY1 interrupt. beep off.");
        break;
    default:
        LOG_E("error sign= %d !", sign);
        break;
    }
}

int main(void)
{
    /* 设置按键引脚为输入模式 */
    rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_WK_UP, PIN_MODE_INPUT_PULLUP);

    /* 设置蜂鸣器引脚为输出模式 */
    rt_pin_mode(PIN_BEEP, PIN_MODE_OUTPUT);

    /* 设置按键中断模式与中断回调函数 */
    rt_pin_attach_irq(PIN_KEY1, PIN_IRQ_MODE_FALLING, irq_callback, (void *) PIN_KEY1);
    rt_pin_attach_irq(PIN_WK_UP, PIN_IRQ_MODE_FALLING, irq_callback, (void *) PIN_WK_UP);

    /* 使能中断 */
    rt_pin_irq_enable(PIN_KEY1, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(PIN_WK_UP, PIN_IRQ_ENABLE);

    while (1)
    {

    }
    return 0;
}

