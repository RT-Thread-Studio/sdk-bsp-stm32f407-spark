# 临界区

多个线程操作 / 访问同一块区域（代码），这块代码就称为临界区，上述例子中的共享内存块就是临界区。线程互斥是指对于临界区资源访问的排它性。当多个线程都要使用临界区资源时，任何时刻最多只允许一个线程去使用，其它要使用该资源的线程必须等待，直到占用资源者释放该资源。线程互斥可以看成是一种特殊的线程同步。

线程的同步方式有很多种，其核心思想都是：**在访问临界区的时候只允许一个 (或一类) 线程运行** 。进入 / 退出临界区的方式有很多种：

1）调用 rt_hw_interrupt_disable() 进入临界区，调用 rt_hw_interrupt_enable() 退出临界区。

2）调用 rt_enter_critical() 进入临界区，调用 rt_exit_critical() 退出临界区。

## 全局开关中断

**全局中断开关** 也称为中断锁，是禁止多线程访问临界区最简单的一种方式，即通过关闭中断的方式，来保证当前线程不会被其他事件打断（因为整个系统已经不再响应那些可以触发线程重新调度的外部事件），也就是当前线程不会被抢占，除非这个线程主动放弃了处理器控制权。当需要关闭整个系统的中断时，可调用下面的函数接口：

```c
rt_base_t rt_hw_interrupt_disable(void);
```

下表描述了此函数的返回值：

  rt_hw_interrupt_disable() 的返回值

| 返回| 描述                                   |
|----------|---------------------------------------------|
| 中断状态 | rt_hw_interrupt_disable 函数运行前的中断状态 |

恢复中断也称开中断。rt_hw_interrupt_enable()这个函数用于 “使能” 中断，它恢复了调用 rt_hw_interrupt_disable()函数前的中断状态。如果调用 rt_hw_interrupt_disable()函数前是关中断状态，那么调用此函数后依然是关中断状态。恢复中断往往是和关闭中断成对使用的，调用的函数接口如下：

```c
void rt_hw_interrupt_enable(rt_base_t level);
```

下表描述了此函数的输入参数：

  rt_hw_interrupt_enable() 的输入参数

|  参数  | 描述                                   |
|----------|---------------------------------------------|
| level    | 前一次 rt_hw_interrupt_disable 返回的中断状态 |

1）使用中断锁来操作临界区的方法可以应用于任何场合，且其他几类同步方式都是依赖于中断锁而实现的，可以说中断锁是最强大的和最高效的同步方法。只是使用中断锁最主要的问题在于，在中断关闭期间系统将不再响应任何中断，也就不能响应外部的事件。所以中断锁对系统的实时性影响非常巨大，当使用不当的时候会导致系统完全无实时性可言（可能导致系统完全偏离要求的时间需求）；而使用得当，则会变成一种快速、高效的同步方式。

例如，为了保证一行代码（例如赋值）的互斥运行，最快速的方法是使用中断锁而不是信号量或互斥量：

```c
    /* 关闭中断 */
    level = rt_hw_interrupt_disable();
    a = a + value;
    /* 恢复中断 */
    rt_hw_interrupt_enable(level);
```

在使用中断锁时，需要确保关闭中断的时间非常短，例如上面代码中的 a = a + value; 也可换成另外一种方式，例如使用信号量：

```c
    /* 获得信号量锁 */
    rt_sem_take(sem_lock, RT_WAITING_FOREVER);
    a = a + value;
    /* 释放信号量锁 */
    rt_sem_release(sem_lock);
```

这段代码在 rt_sem_take 、rt_sem_release 的实现中，已经存在使用中断锁保护信号量内部变量的行为，所以对于简单如 a = a + value; 的操作，使用中断锁将更为简洁快速。

2）函数 rt_base_t rt_hw_interrupt_disable(void) 和函数 void rt_hw_interrupt_enable(rt_base_t level) 一般需要配对使用，从而保证正确的中断状态。

在 RT-Thread 中，开关全局中断的 API 支持多级嵌套使用，简单嵌套中断的代码如下代码所示：

简单嵌套中断使用

```c
#include <rthw.h>

void global_interrupt_demo(void)
{
    rt_base_t level0;
    rt_base_t level1;

    /* 第一次关闭全局中断，关闭之前的全局中断状态可能是打开的，也可能是关闭的 */
    level0 = rt_hw_interrupt_disable();
    /* 第二次关闭全局中断，关闭之前的全局中断是关闭的，关闭之后全局中断还是关闭的 */
    level1 = rt_hw_interrupt_disable();

    do_something();

    /* 恢复全局中断到第二次关闭之前的状态，所以本次 enable 之后全局中断还是关闭的 */
    rt_hw_interrupt_enable(level1);
    /* 恢复全局中断到第一次关闭之前的状态，这时候的全局中断状态可能是打开的，也可能是关闭的 */
    rt_hw_interrupt_enable(level0);
}
```

这个特性可以给代码的开发带来很大的便利。例如在某个函数里关闭了中断，然后调用某些子函数，再打开中断。这些子函数里面也可能存在开关中断的代码。由于全局中断的 API 支持嵌套使用，用户无需为这些代码做特殊处理。

## 进入退出临界区

调用 rt_enter_critical() 进入临界区，调用 rt_exit_critical() 退出临界区，函数原型如下所示：

```c
void rt_enter_critical(void);
void rt_exit_critical(void);
```

两个函数均没有参数和返回，rt_enter_critical 此函数将锁定线程调度程序。

## 代码设计

本例程源码为：interrupt_sample.c

使用开关全局中断的方式保护临界区 cnt  的安全。

## 临界区保护示例

```c
#include <rthw.h>
#include <rtthread.h>

#define THREAD_PRIORITY      20
#define THREAD_STACK_SIZE    512
#define THREAD_TIMESLICE     5

/* 同时访问的全局变量 */
static rt_uint32_t cnt;
static void thread_entry(void *parameter)
{
    rt_uint32_t no;
    rt_uint32_t level;

    no = (rt_uint32_t) parameter;
    while (1)
    {
        /* 关闭中断 */
        level = rt_hw_interrupt_disable();
        cnt += no;
        /* 恢复中断 */
        rt_hw_interrupt_enable(level);

        rt_kprintf("protect thread[%d]'s counter is %d\n", no, cnt);
        rt_thread_mdelay(no * 10);
    }
}

int interrupt_sample(void)
{
    rt_thread_t thread;

    /* 创建thread1线程 */
    thread = rt_thread_create("thread1", thread_entry, (void *)10,
                              THREAD_STACK_SIZE,
                              THREAD_PRIORITY, THREAD_TIMESLICE);

    if (thread != RT_NULL)
        rt_thread_startup(thread);

    /* 创建thread2线程 */
    thread = rt_thread_create("thread2", thread_entry, (void *)20,
                              THREAD_STACK_SIZE,
                              THREAD_PRIORITY, THREAD_TIMESLICE);

    if (thread != RT_NULL)
        rt_thread_startup(thread);

    return 0;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(interrupt_sample, interrupt sample);

```

## 编译运行

编译工程，然后下载运行。使用终端工具打开相应的 COM 口（波特率 115200），可以看到系统的启动日志，输入 interrupt_sample 命令启动示例应用，示例输出结果如下：

```shell
msh >interrupt_sample
msh >protect thread[20]'s counter is 20
protect thread[10]'s counter is 30
protect thread[10]'s counter is 40
protect thread[20]'s counter is 60
protect thread[10]'s counter is 70
protect thread[10]'s counter is 80
protect thread[20]'s counter is 100
protect thread[10]'s counter is 110
protect thread[10]'s counter is 120
protect thread[20]'s counter is 140
protect thread[10]'s counter is 150
protect thread[10]'s counter is 160
protect thread[20]'s counter is 180
protect thread[10]'s counter is 190
protect thread[10]'s counter is 200
protect thread[20]'s counter is 220
```

