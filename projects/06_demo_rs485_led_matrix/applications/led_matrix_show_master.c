/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-6-6      ShiHao        first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <drv_rs485.h>
#include <led_matrix_config.h>
/* define 内部 LED  */
enum{
    INTERN_LED_0,
    INTERN_LED_1,
    INTERN_LED_2,
    INTERN_LED_3,
    INTERN_LED_4,
    INTERN_LED_5,
    INTERN_LED_6,
    INTERN_LED_7,
    INTERN_LED_8,
    INTERN_LED_9,
    INTERN_LED_10,
    INTERN_LED_11,
    INTERN_LED_12,
    INTERN_LED_13,
    INTERN_LED_14,
    INTERN_LED_15,
    INTERN_LED_16,
    INTERN_LED_17,
    INTERN_LED_18,
};

extern rt_device_t rs485_serial;

/* 发送缓冲区 */
static rt_uint8_t rs485_send_buf[8];

static rt_led_node_t led_matrix[] = {
    {INTERN_LED_0, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_1, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_2, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_3, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_4, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_5, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_6, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_7, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_8, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_9, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_10, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_11, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_12, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_13, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_14, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_15, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_16, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_17, {0, 0, 0}, INTERNAL, RT_NULL},\
    {INTERN_LED_18, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_18, {0, 0, 0}, EXTERNAL, RT_NULL},
    {EXTERN_LED_17, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_16, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_15, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_14, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_13, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_12, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_11, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_10, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_9, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_8, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_7, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_6, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_5, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_4, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_3, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_2, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_1, {0, 0, 0}, EXTERNAL, RT_NULL},\
    {EXTERN_LED_0, {0, 0, 0}, EXTERNAL, RT_NULL},\
};

static void intern_led_control(rt_led_node_t *node, pixel_rgb_t color)
{
    led_matrix_set_color(node->pin, color);
    led_matrix_reflash();
    node->status = color;
}

static void extern_led_control(rt_led_node_t *node, pixel_rgb_t color)
{
    rs485_send_buf[0] = 0xA5;
    rs485_send_buf[1] = (node->pin)&0xff;
    *(pixel_rgb_t*)(&(rs485_send_buf[2])) = color;
    rs485_send_buf[5] = 0;
    rs485_send_buf[6] = 0;
    rs485_send_buf[7] = 0xA6;
    rs485_send_data((char *)rs485_send_buf, 8);
    
    node->status = color;
}

static void led_matrix_flowing_water_entry()
{   
    while(1)
    {
        int base_time = 150;
        /* 分别变速转三圈 */
        for(int time = 1; time<=3; time++)
        {
            for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]);i++)
            {
                led_matrix[i].io_ctl(&led_matrix[i],RED);
                rt_thread_delay(base_time / time);
            }
            for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]);i++)
            {
                led_matrix[i].io_ctl(&led_matrix[i],GREEN);
                rt_thread_delay(base_time / time);
            }
            for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]);i++)
            {
                led_matrix[i].io_ctl(&led_matrix[i],BLUE);
                rt_thread_delay(base_time / time);
            }
            for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]);i++)
            {
                led_matrix[i].io_ctl(&led_matrix[i],WHITE);
                rt_thread_delay(base_time / time);
            }
            for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]);i++)
            {
                led_matrix[i].io_ctl(&led_matrix[i],DARK);
                rt_thread_delay(base_time / time);
            }
        }

        /* 同步转五圈 */
        int extern_led_index_start;
        for(extern_led_index_start = 0; (extern_led_index_start < sizeof(led_matrix))\
        && (led_matrix[extern_led_index_start].location == INTERNAL); extern_led_index_start++);
        if(extern_led_index_start < sizeof(led_matrix))
        {
            for(int time = 1; time<=3; time++)
            {
                for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]); i++)
                {
                    if(i < extern_led_index_start)
                    {
                        led_matrix[i].io_ctl(&led_matrix[i],RED);
                    }
                    if(sizeof(led_matrix)-1-i >= extern_led_index_start)
                    {
                        led_matrix[sizeof(led_matrix)/sizeof(led_matrix[0])-1-i].io_ctl(&led_matrix[sizeof(led_matrix)/sizeof(led_matrix[0])-1-i],RED);
                    }
                    rt_thread_delay(base_time / time);
                }
                for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]); i++)
                {
                    if(i < extern_led_index_start)
                    {
                        led_matrix[i].io_ctl(&led_matrix[i],GREEN);
                    }
                    if(sizeof(led_matrix)-1-i >= extern_led_index_start)
                    {
                        led_matrix[sizeof(led_matrix)/sizeof(led_matrix[0])-1-i].io_ctl(&led_matrix[sizeof(led_matrix)/sizeof(led_matrix[0])-1-i],GREEN);
                    }
                    rt_thread_delay(base_time / time);
                }
                for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]); i++)
                {
                    if(i < extern_led_index_start)
                    {
                        led_matrix[i].io_ctl(&led_matrix[i],BLUE);
                    }
                    if(sizeof(led_matrix)-1-i >= extern_led_index_start)
                    {
                        led_matrix[sizeof(led_matrix)/sizeof(led_matrix[0])-1-i].io_ctl(&led_matrix[sizeof(led_matrix)/sizeof(led_matrix[0])-1-i],BLUE);
                    }
                    rt_thread_delay(base_time / time);
                }
                for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]); i++)
                {
                    if(i < extern_led_index_start)
                    {
                        led_matrix[i].io_ctl(&led_matrix[i],WHITE);
                    }
                    if(sizeof(led_matrix)-1-i >= extern_led_index_start)
                    {
                        led_matrix[sizeof(led_matrix)/sizeof(led_matrix[0])-1-i].io_ctl(&led_matrix[sizeof(led_matrix)/sizeof(led_matrix[0])-1-i],WHITE);
                    }
                    rt_thread_delay(base_time / time);
                }
                for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]); i++)
                {
                    if(i < extern_led_index_start)
                    {
                        led_matrix[i].io_ctl(&led_matrix[i],DARK);
                    }
                    if(sizeof(led_matrix)-1-i >= extern_led_index_start)
                    {
                        led_matrix[sizeof(led_matrix)/sizeof(led_matrix[0])-1-i].io_ctl(&led_matrix[sizeof(led_matrix)/sizeof(led_matrix[0])-1-i],DARK);
                    }
                    rt_thread_delay(base_time / time);
                }
            }
        }

    }
}

static void led_matrix_flowing_water_example()
{
    rs485_init();

    /* LED 初始化 */
    for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]);i++)
    {
        if(led_matrix[i].location == INTERNAL)
        {
            led_matrix[i].io_ctl = intern_led_control;

        }else if(led_matrix[i].location == EXTERNAL)
        {
            led_matrix[i].io_ctl = extern_led_control;
            led_matrix[i].io_ctl(&led_matrix[i],DARK);
            rt_thread_delay(20);
        }
    }
    rt_thread_t thread = rt_thread_create("led matrix flowing water",led_matrix_flowing_water_entry,RT_NULL,512,26,10);
    if(thread != RT_NULL)
    {
        rt_kprintf("led matrix flowing water start success\n");
    }else
    {
        rt_kprintf("led matrix flowing water start failed\n");
    }
    rt_thread_startup(thread);
}
MSH_CMD_EXPORT(led_matrix_flowing_water_example,"flowing water");
