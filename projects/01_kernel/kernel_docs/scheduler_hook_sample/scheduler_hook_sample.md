# 调度器

RT-Thread 的线程调度器是抢占式的，主要的工作就是从就绪线程列表中查找最高优先级线程，保证最高优先级的线程能够被运行，最高优先级的任务一旦就绪，总能得到 CPU 的使用权。

当一个运行着的线程使一个比它优先级高的线程满足运行条件，当前线程的 CPU 使用权就被剥夺了，或者说被让出了，高优先级的线程立刻得到了 CPU 的使用权。

如果是中断服务程序使一个高优先级的线程满足运行条件，中断完成时，被中断的线程挂起，优先级高的线程开始运行。

当调度器调度线程切换时，先将当前线程上下文保存起来，当再切回到这个线程时，线程调度器将该线程的上下文信息恢复。

## 设置调度器钩子

在整个系统的运行时，系统都处于线程运行、中断触发 - 响应中断、切换到其他线程，甚至是线程间的切换过程中，或者说系统的上下文切换是系统中最普遍的事件。有时用户可能会想知道在一个时刻发生了什么样的线程切换，可以通过调用下面的函数接口设置一个相应的钩子函数。在系统线程切换时，这个钩子函数将被调用：

```c
void rt_scheduler_sethook(void (*hook)(struct rt_thread* from, struct rt_thread* to));
```

设置调度器钩子函数的输入参数如下表所示：

|   函数参数   |  描述                   |
| ------------ | -------------------------- |
| hook         | 表示用户定义的钩子函数指针 |

钩子函数 hook() 的声明如下：

```c
void hook(struct rt_thread* from, struct rt_thread* to);
```

调度器钩子函数 hook() 的输入参数如下表所示：

|   函数参数   |  描述                           |
| ------------ | ---------------------------------- |
| from         | 表示系统所要切换出的线程控制块指针 |
| to           | 表示系统所要切换到的线程控制块指针 |

注：请仔细编写你的钩子函数，稍有不慎将很可能导致整个系统运行不正常（在这个钩子函数中，基本上不允许调用系统 API，更不应该导致当前运行的上下文挂起）。

## 调度器钩子示例

例程源码：scheduler_hook_sample.c

使用 rt_scheduler_sethook 设置调度器钩子函数，在钩子函数中打印线程的切换。

```c
/* 设置调度器钩子 */
rt_scheduler_sethook(hook_of_scheduler);

/* 钩子函数 */
static void hook_of_scheduler(struct rt_thread *from, struct rt_thread *to)
{
#if RT_VER_NUM >= 0x50001
    rt_kprintf("from: %s -->  to: %s \n", from->parent.name, to->parent.name);
#else
    rt_kprintf("from: %s -->  to: %s \n", from->name, to->name);
#endif
}
```

## 编译运行

编译工程，然后下载运行。使用终端工具打开相应的 COM 口（波特率 115200），可以看到系统的启动日志，输入 scheduler_hook 命令启动示例应用，示例输出结果如下，可以看到系统中的线程都在执行切换。

```shell
msh >
msh >scheduler_hook
msh >from: tshell -->  to: thread2
thread 2 is running
from: thread2 -->  to: thread1
thread 1 is running
from: thread1 -->  to: tidle0
from: tidle0 -->  to: thread2
thread 2 is running
from: thread2 -->  to: thread1
thread 1 is running
from: thread1 -->  to: tidle0
from: tidle0 -->  to: thread2
thread 2 is running
from: thread2 -->  to: thread1
thread 1 is running
from: thread1 -->  to: tidle0
from: tidle0 -->  to: thread2
thread 2 is running
from: thread2 -->  to: thread1
thread 1 is running
from: thread1 -->  to: tidle0
from: tidle0 -->  to: thread2
thread 2 is running
from: thread2 -->  to: thread1
```

