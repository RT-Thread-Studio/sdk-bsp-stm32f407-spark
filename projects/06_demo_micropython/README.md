# MicroPython 例程

## 简介

MicroPython 是 Python 3 编程语言的一种精简而高效的实现，它包含 Python 标准库的一个子集，并被优化为在微控制器和受限环境中运行。它具有交互式提示、任意精度整数、闭包函数、列表解析、生成器、异常处理等高级特性，具有很强的可移植性。它能够帮助开发者快速控制各种硬件外设，不用再去研究底层硬件模块的使用方法，翻看寄存器手册。降低了开发难度，而且减少了重复开发工作，可以加快开发速度，提高开发效率。

本例程作将介绍如何在 rt-thread 上使用 MicroPython ，并通过命令行展示 MicroPython 代码的输入与运行，最后使用一个示例，演示使用 MicroPython 控制硬件。

## 硬件说明

本例程将使用 MicroPython 控制星火 1 号上的各种硬件，请确保开发板上的相关硬件可以正常工作。

## 软件说明

本例程的源码位于 `/projects/06_demo_micropython`。main 程序代码位于 applications/main.c 中, 主要是为 MicroPython 的提供必要的运行环境。如下：

```c
#define FS_PARTITION_NAME     "filesystem"

int main(void)
{

    /* 打开 MicroPython 命令交互界面 */
    extern void mpy_main(const char *filename);
    mpy_main(NULL);
}
```

主函数内容主要是完成了 python 环境的启动。

## 运行

### 编译 & 下载

- RT-Thread Studio：在 RT-Thread Studio 的包管理器中下载 `STM32F407-RT-SPARK` 资源包，然后创建新工程，执行编译。
- MDK：首先双击 mklinks.bat，生成 rt-thread 与 libraries 文件夹链接；再使用 Env 生成 MDK5 工程；最后双击 project.uvprojx 打开 MDK5 工程，执行编译。

编译完成后，将开发板的 ST-Link USB 口与 PC 机连接，然后将固件下载至开发板。

### 运行效果

在 PC 端使用终端工具打开开发板的 uart0 串口，设置 115200 8 1 N 。正常运行后，终端输出信息如下：

```shell
 \ | /
- RT -     Thread Operating System
 / | \     4.1.1 build Jun 12 2023 10:21:47
 2006 - 2022 Copyright by RT-Thread team
lwIP-2.0.3 initialized!
[I/sal.skt] Socket Abstraction Layer initialize success.
msh >[E/[RW007]] The wifi Stage 1 status 0 0 0 1
[I/WLAN.dev] wlan init success
[I/WLAN.lwip] eth device init ok name:w0
[I/WLAN.dev] wlan init success
[I/WLAN.lwip] eth device init ok name:w1

rw007  sn: [rw007c745bb22fc584aa6cecc]
rw007 ver: [RW007_2.1.0-a7a0d089-57]

The stack (main) size for executing MicroPython must be >= 4096

MicroPython v1.13-148-ged7ddd4 on 2020-11-03; Universal python platform with RT-Thread
Type "help()" for more information.
>>>
>>> print("hello RT-Thread")
hello RT-Thread

```

此时 MicroPython 命令交互界面就已经启动，可以通过命令行与 MicroPython 进行交互。下面将使用一个示例展示如何使用 MicroPython 控制硬件。

## 更多使用

MicroPython 的更多使用方法请参考 [文档中心](https://www.rt-thread.org/document/site/#/)

## 注意事项

无

## 引用参考

- 文档中心：[RT-Thread 文档中心](https://www.rt-thread.org/document/site/#/)
- MicroPython 软件包: [https://github.com/RT-Thread-packages/micropython](https://github.com/RT-Thread-packages/micropython)

