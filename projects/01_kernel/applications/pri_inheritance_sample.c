/*
 * Copyright (c) 2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-07-06                  first version
 */

#include <rtthread.h>

static rt_mutex_t mutex = RT_NULL;
static rt_thread_t tida = RT_NULL;
static rt_thread_t tidc = RT_NULL;

#define THREAD_PRIORITY       9
#define THREAD_STACK_SIZE     512
#define THREAD_TIMESLICE      5

static void thread_c_entry(void *parameter)
{
    rt_tick_t tick;

    /* 预留创建并启动线程 A 的时间 */
    rt_thread_delay(100);
    rt_kprintf("thread C priority is: %d\n", tidc->current_priority);

    /* 获取互斥量 */
    rt_mutex_take(mutex, RT_WAITING_FOREVER);
    rt_kprintf("thread C priority is: %d, take the mutex\n", tidc->current_priority);

    /* 持续持有互斥量 3000ms, 线程  A 优先级高会主动抢占 CPU */
    tick = rt_tick_get();
    while (rt_tick_get() - tick < 3000) ;

    rt_kprintf("thread C priority is: %d, running...\n", tidc->current_priority);

    /* 释放互斥量 */
    rt_kprintf("thread C release the mutex\n\n");
    rt_mutex_release(mutex);
    rt_kprintf("thread C priority is: %d\n", tidc->current_priority);
    rt_kprintf("thread C exit\n");
}

static void thread_a_entry(void *parameter)
{
    rt_tick_t tick;

    /* 让出更多的时间让线程 C先运行 */
    rt_thread_delay(200);

    /* 线程 A 尝试获取互斥量 */
    rt_kprintf("thread A priority is: %d, try to take the mutex\n", tida->current_priority);
    rt_mutex_take(mutex, RT_WAITING_FOREVER);
    rt_kprintf("thread A take the mutex\n");

    rt_kprintf("thread A running...\n");

    /* 持续持有互斥量 500ms, 线程 A 优先级高会一直运行 */
    tick = rt_tick_get();
    while (rt_tick_get() - tick < 500) ;

    rt_kprintf("thread A release the mutex\n");
    rt_mutex_release(mutex);
    rt_kprintf("thread A exit\n\n");
}

int pri_inheritance(void)
{
    /* 创建互斥量 mutex */
    mutex = rt_mutex_create("mutex", RT_IPC_FLAG_PRIO);
    if (mutex == RT_NULL)
    {
        rt_kprintf("create dynamic mutex failed.\n");
        return -1;
    }

    /* 创建线程 C 并启动 */
    tidc = rt_thread_create("threadc",
                            thread_c_entry,
                            RT_NULL,
                            2048,
                            THREAD_PRIORITY,
                            5);
    RT_ASSERT(tidc != RT_NULL);
    rt_thread_startup(tidc);

    /* 创建线程 A 并启动 */
    tida = rt_thread_create("threada",
                            thread_a_entry,
                            RT_NULL,
                            2048,
                            THREAD_PRIORITY - 1,
                            5);
    RT_ASSERT(tida != RT_NULL);
    rt_thread_startup(tida);

    return RT_EOK;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(pri_inheritance, pri inheritance);
