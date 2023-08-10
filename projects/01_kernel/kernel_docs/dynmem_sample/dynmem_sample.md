# 动态内存堆的使用

动态堆管理根据具体内存设备划分为以下三种情况，本例程针对于第一种情况，前提是要开启系统 heap 功能。

第一种是针对小内存块的分配管理（小堆内存管理算法），小内存管理算法主要针对系统资源比较少，一般用于小于 2MB 内存空间的系统。

第二种是针对大内存块的分配管理（slab 管理算法），slab 内存管理算法则主要是在系统资源比较丰富时，提供了一种近似多内存池管理算法的快速算法。

第三种是针对多内存块的分配情况（memheap 管理算法），memheap 方法适用于系统存在多个内存堆的情况，它可以将多个内存 “粘贴” 在一起，形成一个大的内存堆，用户使用起来会感到格外便捷。

## 代码设计

本例程源码为：dynmem_sample.c

例程设计一个动态的线程，这个线程会动态申请内存并释放，每次申请更大的内存，当申请不到的时候就结束。

## 动态内存堆示例

示例代码通过 MSH_CMD_EXPORT 将示例初始函数导出到 msh 命令，可以在系统运行过程中，通过在控制台输入命令来启动。

以下定义了线程所用的优先级、栈大小以及时间片的宏。

```c
#include <rtthread.h>

#define THREAD_PRIORITY      25
#define THREAD_STACK_SIZE    512
#define THREAD_TIMESLICE     5
```

线程入口函数，一直申请内存，申请到之后就释放内存，每次会申请更大的内存，申请不到时，将结束，申请的内存大小信息也会打印出来。

```c
/* 线程入口 */
void thread1_entry(void *parameter)
{
    int i;
    char *ptr = RT_NULL; /* 内存块的指针 */

    for (i = 0; ; i++)
    {
        /* 每次分配 (1 << i) 大小字节数的内存空间 */
        ptr = rt_malloc(1 << i);

        /* 如果分配成功 */
        if (ptr != RT_NULL)
        {
            rt_kprintf("get memory :%d byte\n", (1 << i));
            /* 释放内存块 */
            rt_free(ptr);
            rt_kprintf("free memory :%d byte\n", (1 << i));
            ptr = RT_NULL;
        }
        else
        {
            rt_kprintf("try to get %d byte memory failed!\n", (1 << i));
            return;
        }
    }
}
```

动态内存管理的示例代码，创建 thread1 并启动。并将函数使用 MSH_CMD_EXPORT 导出命令。

```c
int dynmem_sample(void)
{
    rt_thread_t tid = RT_NULL;

    /* 创建线程 1 */
    tid = rt_thread_create("thread1",
                           thread1_entry, RT_NULL,
                           THREAD_STACK_SIZE,
                           THREAD_PRIORITY,
                           THREAD_TIMESLICE);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(dynmem_sample, dynmem sample);
```

## 编译运行

编译工程，然后下载运行。使用终端工具打开相应的 COM 口（波特率 115200），可以看到系统的启动日志，输入 dynmem_sample 命令启动示例应用，示例输出结果如下：

```shell
msh >
msh >dynmem_sample
msh >get memory :1 byte
free memory :1 byte
get memory :2 byte
free memory :2 byte
get memory :4 byte
free memory :4 byte
get memory :8 byte
free memory :8 byte
get memory :16 byte
free memory :16 byte
get memory :32 byte
free memory :32 byte
get memory :64 byte
free memory :64 byte
get memory :128 byte
free memory :128 byte
get memory :256 byte
...
get memory :16384 byte
free memory :16384 byte
get memory :32768 byte
free memory :32768 byte
get memory :65536 byte
free memory :65536 byte
try to get 131072 byte memory failed!

```

例程中分配内存成功并打印信息；当试图申请 131072 byte 即 128KB 内存时，由于 RAM 总大小只有 128K，而可用 RAM 小于 128K，所以分配失败。
