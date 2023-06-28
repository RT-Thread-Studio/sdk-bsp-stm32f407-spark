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
#define PIN_KEY0        GET_PIN(C, 0)      // PC0:  KEY0         --> KEY
#define PIN_KEY1        GET_PIN(C, 1)      // PC1:  KEY1         --> KEY
#define PIN_KEY2        GET_PIN(C, 4)      // PC4:  KEY2         --> KEY
#define PIN_WK_UP       GET_PIN(C, 5)      // PC5:  WK_UP        --> KEY

/* 配置蜂鸣器引脚 */
#define PIN_BEEP        GET_PIN(B, 0)      // PA1:  BEEP         --> BEEP (PB1)

/* 配置 LED 灯引脚 */
#define PIN_LED_B       GET_PIN(F, 11)     // PF11: LED_B        --> LED
#define PIN_LED_R       GET_PIN(F, 12)     // PF12: LED_R        --> LED

enum
{
    LED_STOP, LED_BLUE, LED_RED
};


void led_ctrl(rt_uint8_t turn)
{
    if (turn == LED_STOP)
    {
        rt_pin_write(PIN_LED_B, PIN_HIGH);
        rt_pin_write(PIN_LED_R, PIN_HIGH);
    }
    else if (turn == LED_BLUE)
    {
        rt_pin_write(PIN_LED_B, PIN_LOW);
        rt_pin_write(PIN_LED_R, PIN_HIGH);
    }
    else if (turn == LED_RED)
    {
        rt_pin_write(PIN_LED_B, PIN_HIGH);
        rt_pin_write(PIN_LED_R, PIN_LOW);
    }
    else
    {
        LOG_D("err parameter ! Please enter 0-2.");
    }
}

void beep_ctrl(rt_uint8_t on)
{
    if (on)
    {
        rt_pin_write(PIN_BEEP, PIN_HIGH);
    }
    else
    {
        rt_pin_write(PIN_BEEP, PIN_LOW);
    }
}

/* 中断回调 */
void irq_callback(void *args)
{
    rt_uint32_t sign = (rt_uint32_t) args;
    switch (sign)
    {
    case PIN_KEY0 :
        led_ctrl(LED_BLUE);
        LOG_D("KEY0 interrupt. blue light on.");
        break;
    case PIN_KEY1 :
        led_ctrl(LED_RED);
        LOG_D("KEY1 interrupt. red light on.");
        break;
    case PIN_KEY2 :
        led_ctrl(LED_STOP);
        LOG_D("KEY2 interrupt. light off.");
        break;
    default:
        LOG_E("error sign= %d !", sign);
        break;
    }
}

int main(void)
{
    unsigned int count = 1;

    /* 设置按键引脚为输入模式 */
    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_WK_UP, PIN_MODE_INPUT_PULLUP);

    /* 设置LED控制引脚为输出模式 */
    rt_pin_mode(PIN_LED_B, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_LED_R, PIN_MODE_OUTPUT);

    /* 设置蜂鸣器引脚为输出模式 */
    rt_pin_mode(PIN_BEEP, PIN_MODE_OUTPUT);

    /* 设置按键中断模式与中断回调函数 */
    rt_pin_attach_irq(PIN_KEY0, PIN_IRQ_MODE_FALLING, irq_callback, (void *) PIN_KEY0);
    rt_pin_attach_irq(PIN_KEY1, PIN_IRQ_MODE_FALLING, irq_callback, (void *) PIN_KEY1);
    rt_pin_attach_irq(PIN_KEY2, PIN_IRQ_MODE_FALLING, irq_callback, (void *) PIN_KEY2);

    /* 使能中断 */
    rt_pin_irq_enable(PIN_KEY0, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(PIN_KEY1, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(PIN_KEY2, PIN_IRQ_ENABLE);

    while (count > 0)
    {
        if (rt_pin_read(PIN_WK_UP) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_WK_UP) == PIN_LOW)
            {
                LOG_D("WK_UP pressed. beep on.");
                beep_ctrl(1);
            }
        }
        else
        {
            beep_ctrl(0);
        }
        rt_thread_mdelay(10);
        count++;
    }
    return 0;
}

