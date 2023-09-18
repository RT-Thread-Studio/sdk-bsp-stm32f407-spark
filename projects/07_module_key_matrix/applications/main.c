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

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define ROWS 4
#define COLS 4
/* 配置 KEY 输入引脚  选择PMOD1  */
#define PIN_COL1        GET_PIN(E,2)      // PE2:  COL1
#define PIN_COL2        GET_PIN(E,5)      // PE5 : COL2
#define PIN_COL3        GET_PIN(E,3)      // PE3 : COL3
#define PIN_COL4        GET_PIN(E,4)      // PE4:  COL4
#define PIN_ROW1        GET_PIN(A,4)      // PA4:  ROW1
#define PIN_ROW2        GET_PIN(A,7)      // PA7 : ROW2
#define PIN_ROW3        GET_PIN(A,6)      // PA6 : ROW3
#define PIN_ROW4        GET_PIN(A,5)      // PA5:  ROW4

/* 定义列的四个IO在一个数组*/
unsigned int ROW_PINS[ROWS] = {PIN_ROW1, PIN_ROW2, PIN_ROW3, PIN_ROW4};
unsigned int COL_PINS[COLS] = {PIN_COL1, PIN_COL2, PIN_COL3, PIN_COL4};

/* 八个IO口模式设置  */
void key_scan_init()
{
    for (int i = 0; i < ROWS; i++)
    {
        /* 配置行IO为输出模式*/
        rt_pin_mode(ROW_PINS[i], PIN_MODE_OUTPUT);
        /* 初始化行IO为高电平 */
        rt_pin_write(ROW_PINS[i], PIN_HIGH);
    }
    for (int j = 0; j < COLS; j++)
    {
        /* 配置列IO为上拉输入模式*/
        rt_pin_mode(COL_PINS[j], PIN_MODE_INPUT_PULLUP);
    }
}
/* 按键扫描处理函数 */
int key_scan()
{
    /* 依次扫描行的四个IO */
    for (int row = 0 ; row < ROWS ; row++)
    {
        /* 扫描的行置低电平，其余为高电平 */
        rt_pin_write(ROW_PINS[row], PIN_LOW);
        /* 依次扫描列的四个IO */
        for (int COL = 0 ; COL < COLS ; COL++)
        {
            if (rt_pin_read(COL_PINS[COL]) == PIN_LOW)
            {
                rt_thread_mdelay(15);

                while (rt_pin_read(COL_PINS[COL]) == PIN_LOW);
                /* 返回扫描到的键值，键值和按键数值对应如下 */
                /* 1 2 3 A 4 5 6 B 7 8  9  C  *  0  #  D  */
                /* 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 */
                return (row * COLS + COL) + 1;
            }
        }
        /*  扫描的行置高电平  */
        rt_pin_write(ROW_PINS[row], PIN_HIGH);
    }
    return -1;
}
/*  按键处理事件  */
int main(void)
{
    unsigned int count = 1;
    /* 按键初始化 */
    key_scan_init();

    while (count > 0)
    {
        int key = key_scan();
        switch (key)
        {
        case 1:
            LOG_D("1");
            break;
        case 2:
            LOG_D("2");
            break;
        case 3:
            LOG_D("3");
            break;
        case 4:
            LOG_D("A");
            break;
        case 5:
            LOG_D("4");
            break;
        case 6:
            LOG_D("5");
            break;
        case 7:
            LOG_D("6");
            break;
        case 8:
            LOG_D("B");
            break;
        case 9:
            LOG_D("7");
            break;
        case 10:
            LOG_D("8");
            break;
        case 11:
            LOG_D("9");
            break;
        case 12:
            LOG_D("C");
            break;
        case 13:
            LOG_D("*");
            break;
        case 14:
            LOG_D("0");
            break;
        case 15:
            LOG_D("#");
            break;
        case 16:
            LOG_D("D");
            break;
        default:
            break;
        }
        count++;
        rt_thread_mdelay(15);
    }
    return 0;
}

