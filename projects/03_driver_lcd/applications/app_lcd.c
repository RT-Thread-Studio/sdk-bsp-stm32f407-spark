/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-08     Nino       the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <drv_lcd.h>

void app_lcd_string(void){
    lcd_clear(WHITE);

    /* set the background color and foreground color */
    lcd_set_color(WHITE, BLACK);

    /* show some string on lcd */
    lcd_show_string(10, 69, 16, "Hello, RT-Thread!");
    lcd_show_string(10, 69 + 16, 24, "RT-Thread");
    lcd_show_string(10, 69 + 16 + 24, 32, "RT-Thread");

    /* draw a line on lcd */
    lcd_draw_line(0, 69 + 16 + 24 + 32, 240, 69 + 16 + 24 + 32);
}

void app_lcd_circle(void){
    /* draw a concentric circles */
    lcd_draw_point(120, 194);
    for (int i = 0; i < 46; i += 4)
    {
        lcd_draw_circle(120, 194, i);
    }
}

void app_lcd_clean(void){
    lcd_clear(WHITE);
}

MSH_CMD_EXPORT(app_lcd_string, show string on LCD);
MSH_CMD_EXPORT(app_lcd_circle, show circle on LCD);
MSH_CMD_EXPORT(app_lcd_clean, clean LCD);
