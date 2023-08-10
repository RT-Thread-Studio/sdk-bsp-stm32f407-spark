/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-07-04     Joyce       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

int lcd_init(void) /* 串口初始化函数(伪代码) */
{
    // 此处将初始化的代码放入，如初始化 lcd
    rt_kprintf("test auto init: lcd init success!\n");
    return 0;
}
//INIT_APP_EXPORT(lcd_init);    /* 使用自动初始化机制 */
