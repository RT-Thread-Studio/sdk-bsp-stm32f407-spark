/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-07     Tanek        first implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <drv_wakeup.h>
#include <board.h>

#define PIN_LED_R                           GET_PIN(F,12)
#define PIN_LED_B                           GET_PIN(F,11)

#define WAKEUP_APP_THREAD_STACK_SIZE        1024
#define WAKEUP_APP__THREAD_PRIORITY         RT_THREAD_PRIORITY_MAX / 3
#define WAKEUP_EVENT_BUTTON                 (1 << 0)

static rt_event_t wakeup_event;

static void led_app(void)
{
    rt_pm_request(PM_SLEEP_MODE_NONE);
    rt_pin_mode(PIN_LED_R, PIN_MODE_OUTPUT);
    rt_pin_write(PIN_LED_R, 0);
    rt_thread_mdelay(2000);
    rt_pin_write(PIN_LED_R, 1);
    rt_pm_release(PM_SLEEP_MODE_NONE);
}

static void led_cycle()
{
    rt_pin_mode(PIN_LED_B, PIN_MODE_OUTPUT);
    while(1)
    {
        rt_pin_write(PIN_LED_B,1);
        rt_thread_mdelay(500);
        rt_pin_write(PIN_LED_B,0);
        rt_thread_mdelay(500);
    }
}

static void wakeup_callback(void)
{
    if(rt_pm_get_sleep_mode() == PM_SLEEP_MODE_DEEP)
    {
        rt_event_send(wakeup_event, WAKEUP_EVENT_BUTTON);
    }
}

static void wakeup_init(void)
{
    wakeup_event = rt_event_create("wakup", RT_IPC_FLAG_FIFO);
    RT_ASSERT(wakeup_event != RT_NULL);

    bsp_register_wakeup(wakeup_callback);
}

static void pm_mode_init(void)
{
    /* 上电之后默认高功耗模式，在这里退出高功耗模式 */
    rt_pm_release_all(PM_SLEEP_MODE_NONE);
    rt_pm_request(PM_SLEEP_MODE_DEEP);
}

int main(void)
{
    /* 创建一个线程检测CPU是否还在继续运行 */
    rt_thread_t led_cycle_tid = rt_thread_create("led_cycle",led_cycle,RT_NULL,512,21,10);
    if(led_cycle_tid == RT_NULL)
    {
        rt_kprintf("led_cycle_tid is null\n");
    }
    rt_thread_startup(led_cycle_tid);
    
    /* 唤醒回调函数初始化 */
    wakeup_init();

    /* 电源管理初始化 */
    pm_mode_init();
    rt_kprintf("pm_mode_init!\n");
    while (1)
    {
        rt_kprintf("wait sem!\n");
        /* 等待唤醒事件 */
        if (rt_event_recv(wakeup_event,
                        WAKEUP_EVENT_BUTTON,
                        RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
                        RT_WAITING_FOREVER, RT_NULL) == RT_EOK)
        {
            led_app();
        }
    }
    return 0;
}
