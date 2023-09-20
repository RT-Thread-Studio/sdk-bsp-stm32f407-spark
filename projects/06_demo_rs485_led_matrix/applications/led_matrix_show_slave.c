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

static rt_led_node_t led_matrix[] = {
    {EXTERN_LED_0, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_1, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_2, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_3, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_4, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_5, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_6, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_7, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_8, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_9, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_10, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_11, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_12, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_13, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_14, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_15, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_16, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_17, {0, 0, 0}, INTERNAL, RT_NULL},\
    {EXTERN_LED_18, {0, 0, 0}, INTERNAL, RT_NULL},
};

/* 接受缓冲区 */
static rt_uint8_t rs485_receive_buf1[8];
static rt_uint8_t rs485_receive_buf2[8];
static rt_uint8_t *buff = rs485_receive_buf2;
static rt_uint8_t *last_buff = rs485_receive_buf1;

static rt_sem_t rs_485_receive_ready = RT_NULL;

static void intern_led_control(rt_led_node_t *node, pixel_rgb_t color)
{
    led_matrix_set_color(node->pin, color);
    led_matrix_reflash();
    node->status = color;
}

void led_matrix_receieve_task(void *parameter) /* 接收RS485发送过来的数据包的任务 */
{
    /* buff 指针，接收采用双buff */
    static uint8_t index;
    char ch = 0;
    while (1)
    {
        /* A byte of data is read from a rs485_serial port, and if it is not read, it waits for the received semaphore */
        while (rt_device_read(rs485_serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(&rs485_rx_sem, RT_WAITING_FOREVER);
        }
        if(ch == 0xA6)
        {
            buff[index] = ch;
            index = 0;
            if(buff == rs485_receive_buf1)
            {
                buff = rs485_receive_buf2; /* switch buffer pointers */
                last_buff = rs485_receive_buf1;
            }else if(buff == rs485_receive_buf2)
            {
                buff = rs485_receive_buf1; /* switch buffer pointers */
                last_buff = rs485_receive_buf2;
            }
        rt_kprintf("\n receive:");
        if(buff == rs485_receive_buf1)
        {
            rt_kprintf(" rs485_receive_buf1:");
        }else if(buff == rs485_receive_buf2)
        {
            rt_kprintf(" rs485_receive_buf2:");
        }
        for(int i =0;i<8;i++)
        {
            rt_kprintf("%d:%x ",i,buff[i]);
        }
        rt_uint8_t led_no = buff[1];
        rt_kprintf("\nindex:%d",led_no);
        rt_sem_release(rs_485_receive_ready);

        }else
        {
            buff[index++] = ch;
        }


    }
}

void led_matrix_slave_task_entry()
{
    while(1)
    {
        rt_sem_take(rs_485_receive_ready, RT_WAITING_FOREVER); /* Wait for the semaphore */
        // rt_kprintf("\n receive:");
        // for(int i =0;i<8;i++)
        // {
        //     rt_kprintf("%d:%x ",i,(*last_buff)[i]);
        // }
        rt_uint8_t led_no = last_buff[1];
        // rt_kprintf("\nindex:%d",led_no);
        if(led_no<0 || led_no >= sizeof(led_matrix)/sizeof(led_matrix[0]))
        {
            continue;
        }
        pixel_rgb_t color = *(pixel_rgb_t *)(&(last_buff[2]));
        led_matrix[led_no].io_ctl(&led_matrix[led_no],color);
    }
}

void led_matrix_slave_example()
{
    /* 一帧数据接受完成信号量 */
    rs_485_receive_ready = rt_sem_create("receive ready", 0, RT_IPC_FLAG_PRIO);
    if (rs_485_receive_ready == RT_NULL)
    {
        rt_kprintf("create dynamic semaphore  rs_485_receive_ready failed.\n");
    }else
    {
        rt_kprintf("create dynamic semaphore  rs_485_receive_ready success.\n");
    }
    /* INIT LED MATRIX */
    for(int i=0; i<sizeof(led_matrix)/sizeof(led_matrix[0]);i++)
    {
        led_matrix[i].io_ctl = intern_led_control;
        led_matrix[i].io_ctl(&led_matrix[i],DARK);
    }

    rt_thread_t thread = rt_thread_create("led matrix slave example",led_matrix_slave_task_entry,RT_NULL,512,21,10);
    if(thread == RT_NULL)
    {
        rt_kprintf("led matrix slave start faild\n");
    }else
    {
        rt_kprintf("led matrix slave start success\n");
    }
    rs485_init();
    rt_thread_startup(thread);
}
MSH_CMD_EXPORT(led_matrix_slave_example,"led matrix slave example");
// INIT_APP_EXPORT(led_matrix_slave_example);