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

#define PIN_LED_R              GET_PIN(F, 12)      // PE7 :  LED_R        --> LED

#include "nes.h"

static void nes_thread_entry(void *parameter)
{
    nes_t* nes = (nes_t*)parameter;
    nes_run(nes);
    nes_unload_file(nes);
}
static int nes_start(int argc, char *argv[]){

    if (argc == 2)
    {
        const char *nes_file_path = argv[1];
        size_t nes_file_path_len = strlen(nes_file_path);
        if (memcmp(nes_file_path+nes_file_path_len-4,".nes",4)==0 || memcmp(nes_file_path+nes_file_path_len-4,".NES",4)==0)
        {
            nes_printf("nes_file_path:%s\n",nes_file_path);
            nes_t* nes = nes_load_file(nes_file_path);
            if (!nes)
            {
                return -1;
            }
            rt_thread_t thread = rt_thread_create("nes", nes_thread_entry, nes, 2048, 20, 10);
            if(thread == RT_NULL)
            {
                rt_kprintf("Can't create nes thread!\n");
                return -1;
            }
            rt_thread_startup(thread);
            return 0;
        }else
        {
            nes_printf("Please enter xxx.nes\n");
            return -1;
        }
    }else
    {
        nes_printf("Please enter the nes file path\n");
        return -1;
    }
}

int main()
{
    rt_pin_mode(PIN_LED_R,PIN_MODE_OUTPUT);
    while (1)
    {
        rt_pin_write(PIN_LED_R, PIN_LOW);
        rt_thread_mdelay(500);

        rt_pin_write(PIN_LED_R, PIN_HIGH);
        rt_thread_mdelay(500);
    }
    return 0;
}
MSH_CMD_EXPORT(nes_start,nes_start);