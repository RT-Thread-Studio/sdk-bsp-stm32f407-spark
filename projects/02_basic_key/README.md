# 按键输入例程

## 简介

本例程主要功能是通过板载的按键 KEY0(LEFT) 控制 RGB-LED 中的红色 LED 的亮灭。

## 硬件说明

![连接单片机引脚](figures/key_pin.png)

![电路原理图](figures/key_circuit.png)

如上图所示， KEY0(LEFT) 引脚连接单片机 PC0(LEFT) 引脚, KEY0(LEFT) 按键按下为低电平，松开为高电平。

KEY 在开发板中的位置如下图所示：

![按键位置](figures/board.png)

## 软件说明

本例程的源码位于 `/projects/02_basic_key`。

KEY0(LEFT) 对应的单片机引脚定义。

```c
#define PIN_KEY0        GET_PIN(C, 0)     // PC0:  KEY0         --> KEY
```

按键输入的源代码位于 /projects/02_basic_key/applications/main.c 中。在 main 函数中，首先为了实验效果清晰可见，板载 RGB 红色 LED 作为 KEY0(LEFT) 的状态指示灯，设置 RGB 红灯引脚的模式为输出模式，然后设置 PIN_KEY0 引脚为输入模式，最后在 while 循环中通过 rt_pin_read(PIN_KEY0) 判断 KEY0(LEFT) 的电平状态，并作 50ms 的消抖处理，如果成功判断 KEY0(LEFT) 为低电平状态（即按键按下）则打印输出 “KEY0 pressed!” 并且指示灯亮，否则指示灯熄灭。

```c
int main(void)
{
    unsigned int count = 1;

    /* 设置 RGB 红灯引脚的模式为输出模式 */
    rt_pin_mode(PIN_LED_R, PIN_MODE_OUTPUT);
    /* 设置 KEY0 引脚的模式为输入上拉模式 */
    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);

    while (count > 0)
    {
        /* 读取按键 KEY0 的引脚状态 */
        if (rt_pin_read(PIN_KEY0) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_KEY0) == PIN_LOW)
            {
                /* 按键已被按下，输出 log，点亮 LED 灯 */
                LOG_D("KEY0 pressed!");
                rt_pin_write(PIN_LED_R, PIN_LOW);
            }
        }
        else
        {
            /* 按键没被按下，熄灭 LED 灯 */
            rt_pin_write(PIN_LED_R, PIN_HIGH);
        }
        rt_thread_mdelay(10);
        count++;
    }
    return 0;
}
```

## 运行

### 编译 & 下载

- RT-Thread Studio：在 RT-Thread Studio 的包管理器中下载 `STM32F407-RT-SPARK` 资源包，然后创建新工程，执行编译。
- MDK：首先双击 mklinks.bat，生成 rt-thread 与 libraries 文件夹链接；再使用 Env 生成 MDK5 工程；最后双击 project.uvprojx 打开 MDK5 工程，执行编译。

编译完成后，将开发板的 ST-Link USB 口与 PC 机连接，然后将固件下载至开发板。

### 运行效果

按下复位按键重启开发板，按住 KEY0(LEFT) 可以观察到开发板上 RBG 红色 LED 指示灯的亮起，松开 KEY0(LEFT) 可以观察到开发板上的 RBG 红色 LED 指示灯熄灭。按住 KEY0(LEFT) 按键后如下图所示：

![按住 KEY0(LEFT) 红灯亮起](figures/red_light.jpg)

此时也可以在 PC 端使用终端工具打开开发板的 ST-Link 提供的虚拟串口，设置 115200 8 1 N 。开发板的运行日志信息即可实时输出出来。

```
[D/main] KEY0 pressed!
[D/main] KEY0 pressed!
[D/main] KEY0 pressed!
```

## 注意事项

暂无。

## 引用参考

- 设备与驱动：[PIN 设备](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/pin/pin)

