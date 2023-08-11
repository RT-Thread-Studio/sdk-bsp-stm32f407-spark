# 消息队列的使用

消息队列能够接收来自线程或中断服务例程中不固定长度的消息，并把消息缓存在自己的内存空间中。其他线程也能够从消息队列中读取相应的消息，而当消息队列是空的时候，可以挂起读取线程。当有新的消息到达时，挂起的线程将被唤醒以接收并处理消息。消息队列是一种异步的通信方式。

参考：[文档中心——IPC 管理之消息队列](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/ipc2/ipc2?id=%e6%b6%88%e6%81%af%e9%98%9f%e5%88%97)

## 代码设计

本例程源码为：msgq_sample.c

为了体现使用消息队列来达到线程间的通信，本例程设计了 thread1、thread2 两个线程，优先级同为 25，设计了一个消息队列 mqt。

线程 thread1 每 50ms 从消息队列接收一次消息，并打印接收到的消息内容，在接收 20 次消息之后，将消息队列脱离、结束线程。

线程 thread2 每 10ms 向 mqt 消息队列依次发送 20 次消息，分别是消息 “A”-“T”，第 9 次发送的是一个紧急消息 “I”，发送 20 次后线程运行结束。（注，虽然设置的是 5ms，但是该工程设置的一个 OS Tock 是 10ms，是最小精度）。

通过本例程，用户可以清晰地了解到，线程在使用消息队列时候的线程调度。

整个运行过程如下图所示，OS Tick 为系统滴答时钟，下面以例程开始后第一个到来的 OS Tick 为第 1 个 OS Tick，过程描述如下：

![运行过程](figures/process80.png)

（1）在 tshell 线程中初始化一个消息队列 mqt，采用 FIFO 方式进行线程等待；初始化并启动线程 thread1、thread2，优先级同为 25；

（2）在操作系统的调度下，thread1 首先被投入运行，尝试从消息队列获取消息，消息队列暂时没有消息，线程挂起；

（3）随后操作系统调度 thread2 投入运行，thread2 发送一个消息 “A”，并打印发送消息内容，随后每 10ms 发送一条消息；

（4）此时线程 thread1 接收到消息，打印消息内容 “A”，然后每 50ms 接收一次消息；

（5）在第 100ms 时，thread1 本应接收消息 “C”，但由于队列中有紧急消息，所以 thread1 先接收紧急消息 “I”，之后再顺序接收其他消息。

（6）thread2 发送 20 条消息后，结束线程。

（7）thread1 接收 20 条消息后，结束线程。

## 消息队列使用示例

示例代码通过 MSH_CMD_EXPORT 将示例初始函数导出到 msh 命令，可以在系统运行过程中，通过在控制台输入命令来启动。

以下定义了待创建线程需要用到的优先级、时间片的宏，消息队列控制块以及存放消息用到的内存池。

```C
#include <rtthread.h>

#define THREAD_PRIORITY      25
#define THREAD_TIMESLICE     5

/* 消息队列控制块 */
static struct rt_messagequeue mq;
/* 消息队列中用到的放置消息的内存池 */
static rt_uint8_t msg_pool[2048];
```

线程 thread1 使用的栈空间、线程控制块，以及线程 thread1 的入口函数，每 50ms 从消息队列中收取消息，并打印消息内容，20 次后结束。

```c
ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
static struct rt_thread thread1;

/* 线程 1 入口函数 */
static void thread1_entry(void *parameter)
{
    char buf = 0;
    rt_uint8_t cnt = 0;

    while (1)
    {
        /* 从消息队列中接收消息 */
        if (rt_mq_recv(&mq, &buf, sizeof(buf), RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("thread1: recv msg from msg queue, the content:%c\n", buf);
            if (cnt == 19)
            {
                break;
            }
        }
        /* 延时 50ms */
        cnt++;
        rt_thread_mdelay(50);
    }
    rt_kprintf("thread1: detach mq \n");
    rt_mq_detach(&mq);
}
```

线程 thread2 使用的栈空间、线程控制块，以及线程 thread2 的入口函数，每 5ms 向消息队列中发送消息，并打印消息内容，20 次后结束

```c
ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;

/* 线程 2 入口 */
static void thread2_entry(void *parameter)
{
    int result;
    char buf = 'A';
    rt_uint8_t cnt = 0;

    while (1)
    {
        if (cnt == 8)
        {
            /* 发送紧急消息到消息队列中 */
            result = rt_mq_urgent(&mq, &buf, 1);
            if (result != RT_EOK)
            {
                rt_kprintf("rt_mq_urgent ERR\n");
            }
            else
            {
                rt_kprintf("thread2: send urgent message - %c\n", buf);
            }
        }
        else if (cnt>= 20)/* 发送 20 次消息之后退出 */
        {
            rt_kprintf("message queue stop send, thread2 quit\n");
            break;
        }
        else
        {
            /* 发送消息到消息队列中 */
            result = rt_mq_send(&mq, &buf, 1);
            if (result != RT_EOK)
            {
                rt_kprintf("rt_mq_send ERR\n");
            }

            rt_kprintf("thread2: send message - %c\n", buf);
        }
        buf++;
        cnt++;
        /* 延时 5ms */
        rt_thread_mdelay(5);
    }
}
```

消息队列的示例代码，初始化了一个消息队列，初始化并启动了 thread1 与 thread2. 并将函数使用 MSH_CMD_EXPORT 导出命令。

```c
/* 消息队列示例的初始化 */
int msgq_sample(void)
{
    rt_err_t result;

    /* 初始化消息队列 */
    result = rt_mq_init(&mq,
                        "mqt",
                        &msg_pool[0],               /* 内存池指向 msg_pool */
                        1,                          /* 每个消息的大小是 1 字节 */
                        sizeof(msg_pool),           /* 内存池的大小是 msg_pool 的大小 */
                        RT_IPC_FLAG_FIFO);          /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    if (result != RT_EOK)
    {
        rt_kprintf("init message queue failed.\n");
        return -1;
    }

    rt_thread_init(&thread1,
                   "thread1",
                   thread1_entry,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),
                   THREAD_PRIORITY, THREAD_TIMESLICE);
    rt_thread_startup(&thread1);

    rt_thread_init(&thread2,
                   "thread2",
                   thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY, THREAD_TIMESLICE);
    rt_thread_startup(&thread2);

    return 0;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(msgq_sample, msgq sample);
```

## 编译运行

编译工程，然后下载运行。使用终端工具打开相应的 COM 口（波特率 115200），可以看到系统的启动日志，输入 msgq_sample 命令启动示例应用，示例输出结果如下：

```shell
msh >
msh > msgq_sample
msh >thread2: send message - A
thread1: recv msg from msg queue, the content:A
thread2: send message - B
thread2: send message - C
thread2: send message - D
thread2: send message - E
thread1: recv msg from msg queue, the content:B
thread2: send message - F
thread2: send message - G
thread2: send message - H
thread2: send urgent message - I
thread2: send message - J
thread1: recv msg from msg queue, the content:I
thread2: send message - K
thread2: send message - L
thread2: send message - M
thread2: send message - N
thread2: send message - O
thread1: recv msg from msg queue, the content:C
thread2: send message - P
thread2: send message - Q
thread2: send message - R
thread2: send message - S
thread2: send message - T
thread1: recv msg from msg queue, the content:D
message queue stop send, thread2 quit
thread1: recv msg from msg queue, the content:E
thread1: recv msg from msg queue, the content:F
thread1: recv msg from msg queue, the content:G
…
thread1: recv msg from msg queue, the content:T
thread1: detach mq
```


