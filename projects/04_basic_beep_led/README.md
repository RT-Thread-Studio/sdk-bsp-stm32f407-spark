# 蜂鸣器和 led 控制例程

## 简介

本例程主要功能为使用按键控制蜂鸣器和 led，当按下 KEY0 蓝色 LED 亮起，当按下 KEY1 后红色 LED 亮起，当按下 KEY2 后 LED 熄灭，当按住 WK_UP 时蜂鸣器鸣叫，松开 WK_UP 后蜂鸣器关闭。其中 KEY0 KEY1 KEY2 三个按键会触发中断，通过 pin 设备的中断回调函数控制电机， WK_UP 按键通过轮询的方式控制蜂鸣器鸣叫。

## 硬件说明

![蜂鸣器连接单片机引脚](figures/beep_pin.png)

![蜂鸣器电路原理图](figures/beep_circurt.png)

蜂鸣器在开发板位置如下图所示：

![蜂鸣器位置](figures/board.png)

## 软件说明

按键控制蜂鸣器和 LED 的源代码位于 /examples/04_basic_beep_led/applications/main.c 中。

1. 查对应原理图可知， KEY0 、 KEY1 、 KEY2 按下为低电平，松开为高电平， WK_UP 按下为高电平，松开为低电平。所以在 main 函数中，首先将 KEY0 、 KEY1 、 KEY2 三个按键引脚配置为上拉输入模式， WK_UP 按键设置为下拉输入模式，将 PIN_MOTOR_A PIN_MOTOR_B PIN_BEEP 引脚设置为输出模式。
2. 然后使用 rt_pin_attach_irq 函数分别设置 KEY0 、 KEY1 、 KEY2 按键中断为下降沿触发中断并且绑定回调函数、设置回调函数相应的入参, 使用 rt_pin_irq_enable 函数使能这三个按键中断。
3. 最后在 while 循环里轮询 WK_UP 的按键状态，当成功判断 WK_UP 按键按下时调用 beep_ctrl(1) 蜂鸣器鸣叫，否则调用 beep_ctrl(0) 蜂鸣器关闭。

```c
int main(void)
{
    unsigned int count = 1;

    /* 设置按键引脚为输入模式 */
    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_WK_UP, PIN_MODE_INPUT_PULLUP);

    /* 设置 LED 控制引脚为输出模式 */
    rt_pin_mode(PIN_LED_B, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_LED_R, PIN_MODE_OUTPUT);

    /* 设置蜂鸣器引脚为输出模式 */
    rt_pin_mode(PIN_BEEP, PIN_MODE_OUTPUT);

    /* 设置按键中断模式与中断回调函数 */
    rt_pin_attach_irq(PIN_KEY0, PIN_IRQ_MODE_FALLING, irq_callback, (void *)PIN_KEY0);
    rt_pin_attach_irq(PIN_KEY1, PIN_IRQ_MODE_FALLING, irq_callback, (void *)PIN_KEY1);
    rt_pin_attach_irq(PIN_KEY2, PIN_IRQ_MODE_FALLING, irq_callback, (void *)PIN_KEY2);

    /* 使能中断 */
    rt_pin_irq_enable(PIN_KEY0, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(PIN_KEY1, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(PIN_KEY2, PIN_IRQ_ENABLE);

    while (count > 0)
    {
        if (rt_pin_read(PIN_WK_UP) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_WK_UP) == PIN_LOW)
            {
                LOG_D("WK_UP pressed. beep on.");
                beep_ctrl(1);
            }
        }
        else
        {
            beep_ctrl(0);
        }
        rt_thread_mdelay(10);
        count++;
    }
    return 0;
}
```

在中断回调函数中判断入参，根据不同入参进行相应的操作。

```c
/* 中断回调 */
void irq_callback(void *args)
{
    rt_uint32_t sign = (rt_uint32_t)args;
    switch (sign)
    {
    case PIN_KEY0:
        motor_ctrl(LED_BLUE);
        LOG_D("KEY0 interrupt. blue light on.");
        break;
    case PIN_KEY1:
        motor_ctrl(LED_RED);
        LOG_D("KEY1 interrupt. red light on.");
        break;
    case PIN_KEY2:
        motor_ctrl(LED_STOP);
        LOG_D("KEY2 interrupt. light off.");
        break;
    default:
        LOG_E("error sign= %d !", sign);
        break;
    }
}
```
## 运行

### 编译 & 下载

- RT-Thread Studio：通过 RT-Thread Studio 导入工程，执行编译。
- MDK：首先双击 mklinks.bat，生成 rt-thread 与 libraries 文件夹链接；再使用 Env 生成 MDK5 工程；最后双击 project.uvprojx 打开 MDK5 工程，执行编译。

编译完成后，将开发板的 ST-Link USB 口与 PC 机连接，然后将固件下载至开发板。

### 运行效果

```c
[D/main] KEY0 interrupt. blue light on.
[D/main] KEY0 interrupt. blue light on.
[D/main] KEY0 interrupt. blue light on.
[D/main] KEY0 interrupt. blue light on.
[D/main] KEY0 interrupt. blue light on.
[D/main] KEY1 interrupt. red light on.
[D/main] KEY1 interrupt. red light on.
[D/main] KEY1 interrupt. red light on.
[D/main] KEY1 interrupt. red light on.
[D/main] KEY1 interrupt. red light on.
[D/main] KEY1 interrupt. red light on.
[D/main] KEY1 interrupt. red light on.
[D/main] KEY1 interrupt. red light on.
[D/main] KEY1 interrupt. red light on.
[D/main] WK_UP pressed. beep on.
[D/main] WK_UP pressed. beep on.
[D/main] WK_UP pressed. beep on.
[D/main] WK_UP pressed. beep on.
[D/main] WK_UP pressed. beep on.
[D/main] WK_UP pressed. beep on.
[D/main] KEY2 interrupt. light off.
[D/main] KEY2 interrupt. light off.
[D/main] KEY2 interrupt. light off.
[D/main] KEY2 interrupt. light off.
[D/main] KEY2 interrupt. light off.
[D/main] KEY2 interrupt. light off.

```
## 注意事项

暂无

## 引用参考

- 《通用GPIO设备应用笔记》: docs/AN0002-RT-Thread-通用 GPIO 设备应用笔记.pdf
- 《RT-Thread 编程指南》: docs/RT-Thread 编程指南.pdf
