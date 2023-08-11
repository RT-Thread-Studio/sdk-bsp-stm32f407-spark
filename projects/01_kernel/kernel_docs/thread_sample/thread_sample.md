# 线程的使用

线程，即任务的载体。一般被设计成 while(1) 的循环模式，但在循环中一定要有让出 CPU 使用权的动作。如果是可以执行完毕的线程，则系统会自动将执行完毕的线程进行删除 / 脱离。

参考：[文档中心——线程管理](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/thread/thread)

## 代码设计

本例程源码为：thread_sample.c

为了体现线程的创建、初始化与脱离，本例程设计了 thread1、thread2 两个线程。thread1 是创建的动态线程，优先级为 25；Thread2 初始化的静态线程，优先级为 24。

优先级较高的 Thread2 抢占低优先级的 thread1，执行完毕一段程序后自动被系统脱离。

优先级较低的 thread1 被设计成死循环，循环中有让出 CPU 使用权的动作 -- 使用了 delay 函数。该线程在 thread2 退出运行之后开始运行，每隔一段时间运行一次，并一直循环运行下去。

用户可以清晰地了解到线程在本例程中的状态变迁情况。

整个运行过程如下图所示，描述如下：

![运行过程](figures/process.png)

（1）在 tshell 线程 (优先级 20) 中创建线程 thread1 和初始化 thread2，thread1 优先级为 25，thread2 优先级为 24；

（2） 启动线程 thread1 和 thread2，使 thread1 和 thread2 处于就绪状态；

（3）随后 tshell 线程挂起，在操作系统的调度下，优先级较高的 thread2 首先被投入运行；

（4） thread2 是可执行完毕线程，运行完毕打印之后，系统自动删除 thread2；

（5） thread1 得以运行，打印信息之后执行延时将自己挂起；

（6） 系统中没有优先级更高的就绪队列，开始执行空闲线程；

（7） 延时时间到，执行 thread1；

（8） 循环（5）~（7）。

## 线程使用示例

示例代码通过 MSH_CMD_EXPORT 将示例初始函数导出到 msh 命令，可以在系统运行过程中，通过在控制台输入命令来启动。

定义了待创建线程需要用到的优先级，栈空间，时间片的宏，定义线程 thread1 的线程句柄:

```c
# include <rtthread.h>
# define THREAD_PRIORITY 25
# define THREAD_STACK_SIZE 512
# define THREAD_TIMESLICE 5
static rt_thread_t tid1 = RT_NULL;
```

线程 thread1 入口函数，每 500ms 打印一次计数值

```c
/* 线程 1 的入口函数 */
static void thread1_entry(void *parameter)
{
    rt_uint32_t count = 0;

    while (1)
    {
        /* 线程 1 采用低优先级运行，一直打印计数值 */
        rt_kprintf("thread1 count: %d\n", count ++);
        rt_thread_mdelay(500);
    }
}
```

线程 thread2 线程栈、控制块以及线程 2 入口函数的定义，线程 2 打印计数，10 次后退出。

```c
ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;

/* 线程 2 入口 */
static void thread2_entry(void *param)
{
    rt_uint32_t count = 0;

    /* 线程 2 拥有较高的优先级，以抢占线程 1 而获得执行 */
    for (count = 0; count < 10 ; count++)
    {
        /* 线程 2 打印计数值 */
        rt_kprintf("thread2 count: %d\n", count);
    }
    rt_kprintf("thread2 exit\n");

    /* 线程 2 运行结束后也将自动被系统脱离 */
}
```

例程代码，其中创建了线程 thread1，初始化了线程 thread2，并将函数使用 MSH_CMD_EXPORT 导出命令。

```c
/* 线程示例 */
int thread_sample(void)
{
    /* 创建线程 1，名称是 thread1，入口是 thread1_entry*/
    tid1 = rt_thread_create("thread1",
                            thread1_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    /* 初始化线程 2，名称是 thread2，入口是 thread2_entry */
    rt_thread_init(&thread2,
                   "thread2",
                   thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    rt_thread_startup(&thread2);

    return 0;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(thread_sample, thread sample);
```

## 编译运行

编译工程，然后下载运行。使用终端工具打开相应的 COM 口（波特率 115200），可以看到系统的启动日志，输入 thread_sample 命令启动示例应用，示例输出结果如下：

```shell
msh >
msh >thread_sample
msh >thread2 count: 0
thread2 count: 1
thread2 count: 2
thread2 count: 3
thread2 count: 4
thread2 count: 5
thread2 count: 6
thread2 count: 7
thread2 count: 8
thread2 count: 9
thread2 exit
thread1 count: 0
thread1 count: 1
thread1 count: 2
thread1 count: 3
...
```


