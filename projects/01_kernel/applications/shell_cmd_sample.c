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

int new_cmd(void)
{
    rt_kprintf("This function can be called by the new command 'new_cmd'\n");
    return 0;
}

/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(new_cmd, new cmd sample);

void hello(void)
{
    rt_kprintf("hello RT-Thread!\n");
}

//MSH_CMD_EXPORT(hello , say hello to RT-Thread);

#include <rtthread.h>

static void atcmd(int argc, char**argv)
{
    if (argc < 2)
    {
        rt_kprintf("Please input'atcmd <server|client>'\n");
        return;
    }

    if (!rt_strcmp(argv[1], "server"))
    {
        rt_kprintf("AT server!\n");
    }
    else if (!rt_strcmp(argv[1], "client"))
    {
        rt_kprintf("AT client!\n");
    }
    else
    {
        rt_kprintf("Please input'atcmd <server|client>'\n");
    }
}

//MSH_CMD_EXPORT(atcmd, atcmd sample: atcmd <server|client>);
