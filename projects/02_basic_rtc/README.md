# RTC 和 RTC 闹钟的使用例程

## 简介

本例程主要介绍了如何在星火 1 号上使用 RTC（Real-Time Clock）实时时钟，RTC 可以提供精确的实时时间，它可以用于产生年、月、日、时、分、秒等信息。目前实时时钟芯片大多采用精度较高的晶体振荡器作为时钟源。有些时钟芯片为了在主电源掉电时还可以工作，会外加电池供电，使时间信息一直保持有效。

RT-Thread 的 RTC 设备为操作系统的时间系统提供了基础服务。面对越来越多的 IoT 场景，RTC 已经成为产品的标配，甚至在诸如 SSL 的安全传输过程中，RTC 已经成为不可或缺的部分。

## 硬件说明

本例程使用的 RTC 设备依赖于 LSE 时钟，此外无需过多连接。

## 软件说明

本例程的源码位于 `/projects/02_basic_rtc`。

在 main 函数中，获取到了 RTC 设备，然后设置一次系统时间，随后获取一次系统时间以便检测时间是否设置成功，最后延时 1s 后再次获取系统时间。

```c
int main()
{
    rt_err_t ret = RT_EOK;
    time_t now;
    rt_device_t device = RT_NULL;

    /* 寻找设备 */
    device = rt_device_find(RTC_NAME);
    if (!device)
    {
        rt_kprintf("find %s failed!", RTC_NAME);
        return RT_ERROR;
    }

    /* 初始化 RTC 设备 */
    if(rt_device_open(device, 0) != RT_EOK)
    {
        rt_kprintf("open %s failed!", RTC_NAME);
        return RT_ERROR;
    }

    /* 设置日期 */
    ret = set_date(2018, 12, 3);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC date failed\n");
        return ret;
    }

    /* 设置时间 */
    ret = set_time(11, 15, 50);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC time failed\n");
        return ret;
    }

    /* 获取时间 */
    now = time(RT_NULL);
    rt_kprintf("%s\n", ctime(&now));

    /* 延时 1 秒 */
    rt_thread_mdelay(1000);

    /* 获取时间 */
    now = time(RT_NULL);
    rt_kprintf("%s\n", ctime(&now));

    return ret;
}
```

下面代码创建可一个 RTC 闹钟，然后设置 5 秒后唤醒，最后把该函数导入 msh 命令行中。

```c
void user_alarm_callback(rt_alarm_t alarm, time_t timestamp)
{
    rt_kprintf("user alarm callback function.\n");
}

void alarm_sample(void)
{  
    struct rt_alarm_setup setup;
    struct rt_alarm * alarm = RT_NULL;
    static time_t now;
    struct tm p_tm;

    if (alarm != RT_NULL)
        return;

    /* 获取当前时间戳，并把下 5 秒时间设置为闹钟时间 */
    now = time(NULL) + 5;
    gmtime_r(&now,&p_tm);

    setup.flag = RT_ALARM_ONESHOT;            
    setup.wktime.tm_year = p_tm.tm_year;
    setup.wktime.tm_mon = p_tm.tm_mon;
    setup.wktime.tm_mday = p_tm.tm_mday;
    setup.wktime.tm_wday = p_tm.tm_wday;
    setup.wktime.tm_hour = p_tm.tm_hour;
    setup.wktime.tm_min = p_tm.tm_min;
    setup.wktime.tm_sec = p_tm.tm_sec;   

    alarm = rt_alarm_create(user_alarm_callback, &setup);    
    if(RT_NULL != alarm)
    {
        rt_alarm_start(alarm);
    }
}
/* export msh cmd */
MSH_CMD_EXPORT(alarm_sample,alarm sample);
```

##  运行

### 编译 & 下载

- RT-Thread Studio：在 RT-Thread Studio 的包管理器中下载 `STM32F407-RT-SPARK` 资源包，然后创建新工程，执行编译。
- MDK：首先双击 mklinks.bat，生成 rt-thread 与 libraries 文件夹链接；再使用 Env 生成 MDK5 工程；最后双击 project.uvprojx 打开 MDK5 工程，执行编译。

### 运行效果

按下复位按键重启开发板，可以看到板子上会打印如下信息：
```sheel
 \ | /
- RT -     Thread Operating System
 / | \     4.1.1 build Jul 10 2023 09:37:14
 2006 - 2022 Copyright by RT-Thread team
Mon Dec  3 11:15:50 2018

msh >Mon Dec  3 11:15:51 2018
```

如上所示，两次系统时间相差 1s , 和设置的延时时间相对应。之后我们启动闹钟功能测试：
```sheel
msh >alarm_sample
msh >user alarm callback function.
```
在启动 `alarm_sample` 函数后，5s 后，闹钟被叫醒并且进入提前设置好的回调函数。

## 注意事项

暂无。

## 引用参考

- 设备与驱动：[RTC 设备](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/rtc/rtc)
