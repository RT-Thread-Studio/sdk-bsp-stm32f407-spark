# AP3216C 接近与光强传感器例程

## 简介

本例程主要功能是利用 RT-Thread 的 AP3216C 软件包读取传感器 ap3216c 测量的接近感应（ps，proximity sensor）与光照强度（als， ambient light sensor）。

## AP3216C 软件包简介

AP3216C 软件包提供了使用接近感应（ps）与光照强度（als）传感器 ap3216c 基本功能，并且提供了硬件中断的可选功能，如需详细了解该软件包，请参考 AP3216C 软件包中的 README（路径位于`./package/ap3216-vxx`)。

## 硬件说明

ap3216c 硬件原理图如下所示：

![接近与光强传感器模块原理图](figures/ap3216c.png)

![接近与光强传感器模块与芯片连接图](figures/ap3216c_circuit.png)

如上图所示，单片机通过 I2C2(soft) scl(PF1)、 I2C2(soft) sda(PF0) 对传感器 ap3216c 发送命令、读取数据等， AP_INT(PG13) 为硬件中断引脚。

接近感应与光照强度传感器在开发板中的位置如下图所示：

![接近与光强传感器位置](figures/board.png)

该传感器能够实现如下功能：

* 光照强度：支持 4 个量程
* 接近感应：支持 4 种增益
* 中断触发：光照强度及接近感应同时支持 高于阈值 或 低于阈值 的两种硬件中断触发方式

## 软件说明

本例程的源码位于 `/projects/03_driver_als_ps`。

接近感应与光照强度传感器 ap3216c 的示例代码位于 applications/main.c 中。
主要流程：
1. 初始化传感器 ap3216c，传入参数 i2c1 为该传感器挂载的 i2c 总线的名称；初始化若失败，则返回空，程序不会被执行，若成功，则返回传感器设备对象；
2. 将返回设备对象分别传入获取 als 与 ps 函数，获取测量的 als 与 ps 值（详细的 API 介绍参考 ap3216c 软件包读取接近感应与光照强度章节，源码参考 ap3216c.c ）。

示例代码如下：

```c
int main(void)
{
    ap3216c_device_t dev;
    const char *i2c_bus_name = "i2c2";
    int count = 0;

    /* 初始化 ap3216c */
    dev = ap3216c_init(i2c_bus_name);
    if (dev == RT_NULL)
    {
        LOG_E("The sensor initializes failure.");
        return 0;
    }

    while (count++ < 100)
    {
        rt_uint16_t ps_data;
        float brightness;

        /* 读接近感应值 */
        ps_data = ap3216c_read_ps_data(dev);
        if (ps_data == 0)
        {
            LOG_D("object is not proximity of sensor.");
        }
        else
        {
            LOG_D("current ps data   : %d.", ps_data);
        }

        /* 读光照强度值 */
        brightness = ap3216c_read_ambient_light(dev);
        LOG_D("current brightness: %d.%d(lux).", (int)brightness, ((int)(10 * brightness) % 10));

        rt_thread_mdelay(1000);
    }
    return 0;
}
```

### 编译 & 下载

- RT-Thread Studio：在 RT-Thread Studio 的包管理器中下载 `STM32F407-RT-SPARK` 资源包，然后创建新工程，执行编译。
- MDK：首先双击 mklinks.bat，生成 rt-thread 与 libraries 文件夹链接；再使用 Env 生成 MDK5 工程；最后双击 project.uvprojx 打开 MDK5 工程，执行编译。

编译完成后，将开发板的 ST-Link USB 口与 PC 机连接，然后将固件下载至开发板。

### 运行效果

烧录完成后，此时可以在 PC 端使用终端工具打开开发板的 ST-Link 提供的虚拟串口，设置串口波特率为 115200，数据位 8 位，停止位 1 位，无流控，开发板的运行日志信息即可实时输出出来，显示如下所示：

```shell
 \ | /
- RT -     Thread Operating System
 / | \     4.1.1 build Jun  9 2023 13:18:36
 2006 - 2022 Copyright by RT-Thread team
msh >[D/main] object is not proximity of sensor.
[D/main] current brightness: 32.9(lux).
[D/main] object is not proximity of sensor.
[D/main] current brightness: 33.6(lux).
[D/main] current ps data   : 7.
[D/main] current brightness: 33.9(lux).
[D/main] object is not proximity of sensor.
[D/main] current brightness: 33.9(lux).
[D/main] current ps data   : 7.
[D/main] current brightness: 33.9(lux).
[D/main] current ps data   : 11.
[D/main] current brightness: 33.9(lux).
[D/main] current ps data   : 1.
[D/main] current brightness: 33.9(lux).
[D/main] object is not proximity of sensor.
[D/main] current brightness: 33.9(lux).
[D/main] object is not proximity of sensor.
[D/main] current brightness: 33.9(lux).
[D/main] current ps data   : 1023.
[D/main] current brightness: 0.0(lux).
[D/main] current ps data   : 1023.
[D/main] current brightness: 0.0(lux).
[D/main] object is not proximity of sensor.
[D/main] current brightness: 32.9(lux).
[D/main] object is not proximity of sensor.
[D/main] current brightness: 33.6(lux).
[D/main] current ps data   : 1.
[D/main] current brightness: 33.6(lux).
[D/main] current ps data   : 8.
[D/main] current brightness: 33.6(lux).
[D/main] object is not proximity of sensor.
[D/main] current brightness: 33.6(lux).

```

## 注意事项

暂无。

## 引用参考

- 设备与驱动：[I2C 设备](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/i2c/i2c)
- ap3216c 软件包：[https://github.com/RT-Thread-packages/ap3216c](https://github.com/RT-Thread-packages/ap3216c)

