# 邮箱的使用

邮箱是一种简单的线程间消息传递方式，特点是开销比较低，效率较高。在 RT-Thread 操作系统的实现中能够一次传递一个 4 字节大小的邮件，并且邮箱具备一定的存储功能，能够缓存一定数量的邮件数 (邮件数由创建、初始化邮箱时指定的容量决定)。邮箱中一封邮件的最大长度是 4 字节，所以邮箱能够用于不超过 4 字节的消息传递。

参考：[文档中心——IPC 管理之邮箱](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/ipc2/ipc2?id=%e9%82%ae%e7%ae%b1)

## 代码设计

本例程源码为：mailbox_sample.c

为了体现使用邮箱来达到线程间的通信，本例程设计了 thread1、thread2 两个线程，优先级同为 10，设计了一个邮箱 mbt。

线程 thread1 每 100ms 尝试接收一次邮件，如果接收到邮件就将邮件内容打印出来。在接收到结束邮件时，打印邮件信息，线程结束。

线程 thread2 每 200ms 发送一次邮件，发送 10 次之后，发送结束邮件（线程 2 共发送 11 封邮件），线程运行结束。

通过本例程，用户可以清晰地了解到，线程在使用邮箱时候的线程调度。

整个运行过程如下图所示，下面以 thread2 开始运行时为开始时间，过程描述如下：

![运行过程](figures/process70.png)

（1）在 tshell 线程中初始化一个邮箱 mbt，采用 FIFO 方式进行线程等待；初始化并启动线程 thread1、thread2，优先级同为 10；

（2）在操作系统的调度下，thread1 首先被投入运行；

（3）thread1 开始运行，首先打印一段信息，然后尝试获取邮件，邮箱暂时没有邮件，thread1 挂起；

（4）随后操作系统调度 thread2 投入运行，发送一封邮件，随后进入 200ms 延时；

（5）此时线程 thread1 被唤醒，接收到邮件，继续打印一段信息，然后进入 100ms 延时；

（6）thread2 在发送 10 次邮件后，发送一封结束内容的邮件，线程结束。

（7）thread1 一直接收邮件，当接收到来自 thread2 的结束邮件后，脱离邮箱，线程结束。

###  源程序说明

示例代码通过 MSH_CMD_EXPORT 将示例初始函数导出到 msh 命令，可以在系统运行过程中，通过在控制台输入命令来启动。

以下定义了线程需要用到的优先级，栈空间，时间片的宏，邮箱控制块，存放邮件的内存池、3 份邮件内容。

```c
#include <rtthread.h>

#define THREAD_PRIORITY      10
#define THREAD_TIMESLICE     5

/* 邮箱控制块 */
static struct rt_mailbox mb;

/* 用于放邮件的内存池 */
static char mb_pool[128];

static char mb_str1[] = "I'm a mail!";
static char mb_str2[] = "this is another mail!";
static char mb_str3[] = "over";
```


线程 thread1 使用的栈空间、线程控制块，以及线程 thread1 的入口函数，每 100ms 收取一次邮件并打印邮件内容，当收取到结束邮件的时候，脱离邮箱，结束运行。

```c
ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
static struct rt_thread thread1;

/* 线程 1 入口 */
static void thread1_entry(void *parameter)
{
    char *str;

    while (1)
    {
        rt_kprintf("thread1: try to recv a mail\n");

        /* 从邮箱中收取邮件 */
        if (rt_mb_recv(&mb, (rt_uint32_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("thread1: get a mail from mailbox, the content:%s\n", str);
            if (str == mb_str3)
                break;

            /* 延时 100ms */
            rt_thread_mdelay(100);
        }
    }
    /* 执行邮箱对象脱离 */
    rt_mb_detach(&mb);
}
```

线程 thread2 使用的栈空间、线程控制块，以及线程 thread2 的入口函数，每 200ms 发送一封邮件，10 次后发送结束邮件，结束运行

```c
ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;

/* 线程 2 入口 */
static void thread2_entry(void *parameter)
{
    rt_uint8_t count;

    count = 0;
    while (count < 10)
    {
        count ++;
        if (count & 0x1)
        {
            /* 发送 mb_str1 地址到邮箱中 */
            rt_mb_send(&mb, (rt_uint32_t)&mb_str1);
        }
        else
        {
            /* 发送 mb_str2 地址到邮箱中 */
            rt_mb_send(&mb, (rt_uint32_t)&mb_str2);
        }

        /* 延时 200ms */
        rt_thread_mdelay(200);
    }

    /* 发送邮件告诉线程 1，线程 2 已经运行结束 */
    rt_mb_send(&mb, (rt_uint32_t)&mb_str3);
}
```

邮箱的示例代码，初始化了邮箱，初始化并启动了线程 thread1 与 thread2。并将函数使用 MSH_CMD_EXPORT 导出命令

```c
int mailbox_sample(void)
{
    rt_err_t result;

    /* 初始化一个 mailbox */
    result = rt_mb_init(&mb,
                        "mbt",                      /* 名称是 mbt */
                        &mb_pool[0],                /* 邮箱用到的内存池是 mb_pool */
                        sizeof(mb_pool) / 4,        /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
                        RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox failed.\n");
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
MSH_CMD_EXPORT(mailbox_sample, mailbox sample);
```

## 编译运行

编译工程，然后下载运行。使用终端工具打开相应的 COM 口（波特率 115200），可以看到系统的启动日志，输入 mailbox_sample 命令启动示例应用，示例输出结果如下：

```shell
msh >
msh >mailbox_sample
thread1: try to recv a mail
thread1: get a mail from mailbox, the content:I'm a mail!
msh >thread1: try to recv a mail
thread1: get a mail from mailbox, the content:this is another mail!
…
thread1: try to recv a mail
thread1: get a mail from mailbox, the content:this is another mail!
thread1: try to recv a mail
thread1: get a mail from mailbox, the content:over
```

