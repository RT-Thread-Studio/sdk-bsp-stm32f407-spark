/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-09     rtt       the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <aht10.h>

#define DBG_TAG "app_temp_humi"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static rt_mutex_t thread_mutex;

void app_temp_humi_entry(void* argument){
    aht10_device_t dev = argument;
    float humidity, temperature;

    for(;;)
    {
        rt_mutex_take(thread_mutex, RT_WAITING_FOREVER);

       /* 读取湿度 */
       humidity = aht10_read_humidity(dev);
       LOG_D("humidity   : %d.%d %%", (int)humidity, (int)(humidity * 10) % 10);

       /* 读取温度 */
       temperature = aht10_read_temperature(dev);
       LOG_D("temperature: %d.%d", (int)temperature, (int)(temperature * 10) % 10);

       rt_mutex_release(thread_mutex);

       rt_thread_mdelay(1000);
    }
}

int app_temp_humi_init(void){
    aht10_device_t dev;

    /* 总线名称 */
    const char *i2c_bus_name = "i2c3";

    thread_mutex = rt_mutex_create("app_temp_humi", RT_IPC_FLAG_PRIO);

    /* 等待传感器正常工作 */
    rt_thread_mdelay(2000);

    /* 初始化 aht10 */
    dev = aht10_init(i2c_bus_name);
    if (dev == RT_NULL)
    {
       LOG_E(" The sensor initializes failure");
       return 0;
    }

    rt_thread_t tid;
    tid = rt_thread_create("app_temp_humi", app_temp_humi_entry, dev, 1024, 20, 10);

    if(tid != RT_NULL) rt_thread_startup(tid);
    else return -1;

    return 0;
}

INIT_DEVICE_EXPORT(app_temp_humi_init);

void app_th_start(){
    rt_mutex_release(thread_mutex);
}

void app_th_stop(){
    rt_mutex_take(thread_mutex, RT_WAITING_FOREVER);
}

MSH_CMD_EXPORT(app_th_start, start app_temp_humi);
MSH_CMD_EXPORT(app_th_stop, stop_app_temp_humi);
