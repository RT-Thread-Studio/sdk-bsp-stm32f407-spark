# 空闲线程

空闲线程（idle）是系统创建的最低优先级的线程，线程状态永远为就绪态。当系统中无其他就绪线程存在时，调度器将调度到空闲线程，它通常是一个死循环，且永远不能被挂起。另外，空闲线程在 RT-Thread 也有着它的特殊用途：

若某线程运行完毕，系统将自动删除线程：自动执行 rt_thread_exit() 函数，先将该线程从系统就绪队列中删除，再将该线程的状态更改为关闭状态，不再参与系统调度，然后挂入 rt_thread_defunct 僵尸队列（资源未回收、处于关闭状态的线程队列）中，最后空闲线程会回收被删除线程的资源。

空闲线程也提供了接口来运行用户设置的钩子函数，在空闲线程运行时会调用该钩子函数，适合处理功耗管理、看门狗喂狗等工作。空闲线程必须有得到执行的机会，即其他线程不允许一直while(1)死卡，必须调用具有阻塞性质的函数；否则例如线程删除、回收等操作将无法得到正确执行。

## 空闲线程钩子函数

空闲钩子函数是空闲线程的钩子函数，如果设置了空闲钩子函数，就可以在系统执行空闲线程时，自动执行空闲钩子函数来做一些其他事情，比如系统指示灯。设置 / 删除空闲钩子的接口如下：

```c
rt_err_t rt_thread_idle_sethook(void (*hook)(void));
rt_err_t rt_thread_idle_delhook(void (*hook)(void));
```

设置空闲钩子函数 rt_thread_idle_sethook() 的输入参数和返回值如下表所示：

|  函数参数  | 描述      |
|--------------|----------------|
| hook         | 设置的钩子函数 |
| 返回    | ——             |
| RT_EOK       | 设置成功       |
| \-RT_EFULL   | 设置失败       |

删除空闲钩子函数 rt_thread_idle_delhook() 的输入参数和返回值如下表所示：

|  函数参数  | 描述      |
|--------------|----------------|
| hook         | 删除的钩子函数 |
| 返回    | ——             |
| RT_EOK       | 删除成功       |
| \-RT_ENOSYS  | 删除失败       |

注：空闲线程是一个线程状态永远为就绪态的线程，因此设置的钩子函数必须保证空闲线程在任何时刻都不会处于挂起状态，例如 rt_thread_delay()，rt_sem_take() 等可能会导致线程挂起的函数都不能使用。并且，由于 malloc、free 等内存相关的函数内部使用了信号量作为临界区保护，因此在钩子函数内部也不允许调用此类函数！

## 代码设计

本例程源码为：idlehook_sample.c

这个例程创建一个线程，通过延时进入空闲任务钩子，用于打印进入空闲钩子的次数

## 空闲钩子函数使用示例

```c
#include <rtthread.h>
#include <rthw.h>

#define THREAD_PRIORITY      20
#define THREAD_STACK_SIZE    1024
#define THREAD_TIMESLICE     5

/* 指向线程控制块的指针 */
static rt_thread_t tid = RT_NULL;

/* 空闲线程钩子函数执行次数 */
volatile static int hook_times = 0;

/* 空闲线程钩子函数 */
static void idle_hook()
{
    if (0 == (hook_times % 10000))
    {
        rt_kprintf("enter idle hook %d times.\n", hook_times);
    }

    rt_enter_critical();
    hook_times++;
    rt_exit_critical();
}

/* 线程入口 */
static void thread_entry(void *parameter)
{
    int i = 5;
    while (i--)
    {
        rt_kprintf("enter thread1.\n");
        rt_enter_critical();
        hook_times = 0;
        rt_exit_critical();

        /* 休眠500ms */
        rt_kprintf("thread1 delay 500ms.\n");
        rt_thread_mdelay(500);
    }
    rt_kprintf("delete idle hook.\n");

    /* 删除空闲线程钩子函数 */
    rt_thread_idle_delhook(idle_hook);
    rt_kprintf("thread1 finish.\n");
}

int idle_hook_sample(void)
{
    /* 设置空闲线程钩子 */
    rt_thread_idle_sethook(idle_hook);

    /* 创建线程 */
    tid = rt_thread_create("thread1",
                           thread_entry, RT_NULL,
                           THREAD_STACK_SIZE,
                           THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(idle_hook_sample, idle hook sample);

```

## 编译运行

编译工程，然后下载运行。使用终端工具打开相应的 COM 口（波特率 115200），可以看到系统的启动日志，输入 idle_hook_sample 命令启动示例应用，示例输出结果如下：

```shell
msh >
msh >idle_hook_sample
msh >enter thread1.
thread1 delay 500ms.
enter idle hook 0 times.
enter idle hook 10000 times.
enter idle hook 20000 times.
enter idle hook 30000 times.
...
enter idle hook 200000 times.
enter thread1.
thread1 delay 500ms.
enter idle hook 0 times.
...
enter idle hook 200000 times.
...
enter thread1.
thread1 delay 500ms.
enter idle hook 0 times.
enter idle hook 10000 times.
...
enter idle hook 200000 times.
delete idle hook.
thread1 finish.
```


