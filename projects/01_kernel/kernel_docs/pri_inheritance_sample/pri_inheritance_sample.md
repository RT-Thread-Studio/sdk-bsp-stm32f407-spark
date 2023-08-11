# 互斥量——优先级继承

互斥量是一种特殊的二值信号量。它和信号量不同的是：拥有互斥量的线程拥有互斥量的所有权，互斥量支持递归访问且能防止线程优先级翻转；并且互斥量只能由持有线程释放，而信号量则可以由任何线程释放。

互斥量的使用比较单一，因为它是信号量的一种，并且它是以锁的形式存在。在初始化的时候，互斥量永远都处于开锁的状态，而被线程持有的时候则立刻转为闭锁的状态。

注意：需要切记的是互斥量不能在中断服务例程中使用。

参考：[文档中心——IPC 管理之互斥量](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/ipc1/ipc1?id=%e4%ba%92%e6%96%a5%e9%87%8f)

## 代码设计

本例程源码为：priority_inheritance_sample.c。

为了体现使用互斥量来达到线程间的同步，并体现优先级继承的现象，本例程设计了 threadC、threadA 两个线程，优先级分别为 9、8，设计了一个互斥量 mutex。

线程 threadC 进入后先打印自己的优先级，然后进入 100ms 延时，延时结束后获取互斥量 mutex，获取到互斥量之后持有一段时间，再释放互斥量 mutex。

线程 threadA 进入后先打印自己的优先级，然后尝试获取互斥量 mutex，获取到互斥量之后持有一段时间，之后将互斥量释放。

其中优先级继承的情况就是：低优先级线程 C 先持有互斥量，而后高优先级线程 A 试图持有互斥量，此时线程 C 的优先级应该被提升为和线程 A 的优先级相同。

通过本例程，用户可以清晰地了解到，互斥量在线程间同步的作用、互斥量的优先级继承性以及互斥量连续获取不会造成死锁。

## 源程序说明

示例代码通过 MSH_CMD_EXPORT 将示例初始函数导出到 msh 命令，可以在系统运行过程中，通过在控制台输入命令来启动。

定义了待创建线程需要用到的优先级，栈空间，时间片的宏，以及线程控制块句柄和互斥量控制块句柄

```c
#include <rtthread.h>

/* 指向线程控制块的指针 */
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t tid3 = RT_NULL;
static rt_mutex_t mutex = RT_NULL;

#define THREAD_PRIORITY       9
#define THREAD_STACK_SIZE     512
#define THREAD_TIMESLICE      5
```

线程 thread C 的入口函数，打印优先级信息，之后获取互斥量并长时间运行，让高优先级的 thread A 主动抢占 CPU，然后打印优先级，最后释放互斥量。

```c
/* 线程 C 入口 */
static void thread_c_entry(void *parameter)
{
    rt_tick_t tick;

    /* 预留创建并启动线程 A 的时间 */
    rt_thread_delay(100);
    rt_kprintf("thread C priority is: %d\n", tidc->current_priority);

    /* 获取互斥量 */
    rt_mutex_take(mutex, RT_WAITING_FOREVER);
    rt_kprintf("thread C priority is: %d, take the mutex\n", tidc->current_priority);

    /* 持续持有互斥量 3000ms, 线程 A 优先级高会主动抢占 CPU */
    tick = rt_tick_get();
    while (rt_tick_get() - tick < 3000) ;

    rt_kprintf("thread C priority is: %d, running...\n", tidc->current_priority);

    /* 释放互斥锁 */
    rt_kprintf("thread C release the mutex\n\n");
    rt_mutex_release(mutex);
    rt_kprintf("thread C priority is: %d\n", tidc->current_priority);
    rt_kprintf("thread C exit\n");
}
```

线程 thread A 的入口函数，先打印自身优先级信息，然后获取互斥量，获取到互斥量之后进行 500ms 的长时间循环，使 thread3 运行 500ms 左右，之后释放互斥量。

```c
/* 线程 A 入口 */
static void thread_a_entry(void *parameter)
{
    rt_tick_t tick;

    /* 让出更多的时间让线程 C先运行 */
    rt_thread_delay(200);

    /* 线程 A 尝试获取互斥量 */
    rt_kprintf("thread A priority is: %d, try to take the mutex\n", tida->current_priority);
    rt_mutex_take(mutex, RT_WAITING_FOREVER);
    rt_kprintf("thread A take the mutex\n");

    rt_kprintf("thread A running...\n");

    /* 持续持有互斥量 500ms, 线程 A 优先级高会一直运行 */
    tick = rt_tick_get();
    while (rt_tick_get() - tick < 500) ;

    rt_kprintf("thread A release the mutex\n");
    rt_mutex_release(mutex);
    rt_kprintf("thread A exit\n\n");
}
```

互斥量优先级继承的例子，解决优先级翻转问题。示例函数首先创建互斥量，再创建、启动了线程 threadC、threadA。并将函数使用 MSH_CMD_EXPORT 导出命令。

```c
int pri_inheritance(void)
{
    /* 创建互斥量 mutex */
    mutex = rt_mutex_create("mutex", RT_IPC_FLAG_PRIO);
    if (mutex == RT_NULL)
    {
        rt_kprintf("create dynamic mutex failed.\n");
        return -1;
    }

    /* 创建线程 C 并启动 */
    tidc = rt_thread_create("threadc",
                            thread_c_entry,
                            RT_NULL,
                            2048,
                            THREAD_PRIORITY,
                            5);
    RT_ASSERT(tidc != RT_NULL);
    rt_thread_startup(tidc);

    /* 创建线程 A 并启动 */
    tida = rt_thread_create("threada",
                            thread_a_entry,
                            RT_NULL,
                            2048,
                            THREAD_PRIORITY - 1,
                            5);
    RT_ASSERT(tida != RT_NULL);
    rt_thread_startup(tida);

    return RT_EOK;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(pri_inheritance, pri inheritance);
```

## 编译运行

编译工程，然后下载运行。使用终端工具打开相应的 COM 口（波特率 115200），可以看到系统的启动日志，输入 pri_inversion 命令启动示例应用，示例输出结果如下：

```shell
msh >
msh >pri_inheritance
thread C priority is: 9              # 线程 C 的优先级为 9
thread C priority is: 9, take the mutex
thread A priority is: 8, try to take the mutex
thread C priority is: 8, running...  # 线程 C 继承线程 A 的优先级，变为 8
thread C release the mutex

thread A take the mutex
thread A running...
thread A release the mutex
thread A exit

thread C priority is: 9              # 线程 C 的优先级恢复为 9
thread C exit
```

例程演示了互斥量的优先级继承特性。线程 C 先持有互斥量，而后线程 A 试图持有互斥量，此时线程 C 的优先级被提升为和线程 A 的优先级相同。

