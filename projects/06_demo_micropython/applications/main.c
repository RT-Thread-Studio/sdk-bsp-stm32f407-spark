/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <stdio.h>
#include <rtthread.h>
#include <string.h>
#include <rthw.h>

#define LOG_TAG               "main"


#define FS_PARTITION_NAME     "filesystem"

int main(void)
{

    /* 打开 MicroPython 命令交互界面 */
    extern void mpy_main(const char *filename);
    mpy_main(NULL);
}