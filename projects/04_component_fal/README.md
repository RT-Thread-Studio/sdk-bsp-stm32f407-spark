# Flash 分区管理例程

本例程演示如何通过 RT-Thread 提供的 FAL 组件对 Flash 进行分区管理操作。例程中，通过调用 FAL 接口完成了对指定分区的测试工作，完成了对 Flash 读、写、擦的测试，同时也通过该例程完成了对 Flash 驱动的基本测试。

## FAL 简介

FAL (Flash Abstraction Layer) Flash 抽象层，是 RT-Thread 的一个组件，是对 Flash 及基于 Flash 的分区进行管理、操作的抽象层，对上层统一了 Flash 及分区操作的 API ，并具有以下特性：

- 支持静态可配置的分区表，并可关联多个 Flash 设备；
- 分区表支持 **自动装载** 。避免在多固件项目，分区表被多次定义的问题；
- 代码精简，对操作系统 **无依赖** ，可运行于 **裸机平台**，比如对资源有一定要求的 bootloader；
- 统一的操作接口。保证了文件系统、OTA、NVM 等对 Flash 有一定依赖的组件，底层 Flash 驱动的可重用性；
- 自带基于 Finsh/MSH 的测试命令，可以通过 Shell 按字节寻址的方式操作（读写擦） Flash 或分区，方便开发者进行调试、测试；

本例程旨在演示如何使用 fal 管理多个 Flash 设备，指导用户通过 fal 分区表操作 Flash 设备。该例程将是后续 OTA、 easyflash 等例程的基础。
通过本例程指导用户学习使用 fal 处理以下问题：

- 使用 fal 管理多个 Flash 设备
- 创建分区表
- 使用 fal 操作分区表

## 硬件说明

本例程使用到的硬件资源如下所示：

* UART1
* 片内 FLASH (1MBytes)
* 片外 Nor Flash (8MBytes)

## 软件说明

本例程的源码位于 `/projects/04_component_fal`。

| 文件路径                                              | 文件描述                                                     |
| ----------------------------------------------------- | ------------------------------------------------------------ |
| applications/main.c                                   | app 入口（fal 例程程序）                                     |
| libraries/Board_Drivers/fal/fal_cfg.h                 | fal 配置文件（Flash 设备配置和分区表配置）                   |
| libraries/Board_Drivers/fal/fal_spi_flash_sfud_port.c | fal 操作片外 Nor Flash 的移植文件（将 Flash 读写擦除接口注册到 fal） |
| libraries/HAL_Drivers/drv_flash/drv_flash_f4.c        | fal 操作片内 Flash 的移植文件（将 Flash 读写擦除接口注册到 fal） |
| rt-thread/components/fal                              | FAL 组件（fal 源码实现）                                     |
| rt-thread/components/fal/inc/fal.h                    | FAL 组件对外提供的操作接口                                   |

从上表中可以看到，如果要使用 FAL 组件需要进行必要的移植工作，移植文件存放在 ports 目录下。本 stm32F4 平台已经完成相关的移植工作。

### fal 配置说明

fal 配置存放在 `libraries/Board_Drivers/fal/fal_cfg.h` 文件中，主要包括 Flash 设备的配置、分区表的配置。

**Flash 设备配置**

fal 中使用 Flash 设备列表管理多个 Flash 设备。本例程中涉及到两个 Flash 设备， STM32F4 芯片内的 Flash 和片外的 QSPI Flash（Nor Flash）。

```c
/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &stm32_onchip_flash_128k,                                        \
    &w25q64,                                                        \
}
```

- stm32_onchip_flash_128k 是 STM32F4 片内 Flash 设备，定义在 `libraries/Board_Drivers/fal/fal_spi_flash_sfud_port.c` 文件中，参考 `Flash 设备对接说明` 章节。
- w25q64 是外部扩展的 Nor FLASH 设备，定义在 `libraries/HAL_Drivers/drv_flash/drv_flash_f4.c` 文件中，参考 `Flash 设备对接说明` 章节。

### 分区表配置

分区表存放在 libraries/Board_Drivers/fal/fal_cfg.h

```c
#define FAL_PART_TABLE                                                                                          \
{                                                                                                               \
    {FAL_PART_MAGIC_WROD,        "app", "onchip_flash_128k",                            0,       384 * 1024, 0}, \
    {FAL_PART_MAGIC_WROD,      "param", "onchip_flash_128k",                   384 * 1024,       640 * 1024, 0}, \
    {FAL_PART_MAGIC_WROD,  "easyflash",    "W25Q64",                                    0,       512 * 1024, 0}, \
    {FAL_PART_MAGIC_WROD,   "download",    "W25Q64",                           512 * 1024,      1024 * 1024, 0}, \
    {FAL_PART_MAGIC_WROD, "wifi_image",    "W25Q64",                  (512 + 1024) * 1024,       512 * 1024, 0}, \
    {FAL_PART_MAGIC_WROD,       "font",    "W25Q64",            (512 + 1024 + 512) * 1024,  3 * 1024 * 1024, 0}, \
    {FAL_PART_MAGIC_WROD, "filesystem",    "W25Q64", (512 + 1024 + 512 + 3 * 1024) * 1024,  3 * 1024 * 1024, 0}, \
}
```

这里有一个宏定义 FAL_PART_HAS_TABLE_CFG，如果定义，则表示应用程序使用 fal_cfg.h 文件中定义的分区表。是否使用 fal_cfg.h 文件中定义的分区表，有这样一个准则:

- 如果使用 bootloader 则不定义 FAL_PART_HAS_TABLE_CFG 宏，而使用 bootloader 中定义的分区表
- 如果不使用 bootloader 则需要用户定义 FAL_PART_HAS_TABLE_CFG 宏，从而使用 fal_cfg.h 文件中定义的分区表 fal_cfg.h 文件中定义的分区表最终会注册到 struct fal_partition 结构体数组中。

`fal_partition` 结构体定义如下所示：

```c
struct fal_partition
{
    uint32_t magic_word;
    /* FLASH 分 区 名 称 */
    char name[FAL_DEV_NAME_MAX];
    /* FLASH 分 区 所 在 的 FLASH 设 备 名 称 */
    char flash_name[FAL_DEV_NAME_MAX];
    /* FLASH 分 区 在 FLASH 设 备 的 偏 移 地 址 */
    long offset;
    size_t len;
    uint8_t reserved;`
};
```

`fal_partition` 结构体成员简要介绍如下所示：

| 成员变量   | 说明                                             |
| ---------- | ------------------------------------------------ |
| magic_word | 魔法数，系统使用，用户无需关心                   |
| name       | 分区名字，最大 23 个 ASCII 字符                  |
| flash_name | 分区所属的 Flash 设备名字，最大 23 个 ASCII 字符 |
| offset     | 分区起始地址相对 Flash 设备起始地址的偏移量      |
| len        | 分区大小，单位字节                               |
| reserved   | 保留项                                           |

### Flash 设备对接说明

fal 是 Flash 抽象层，要操作 Flash 设备必然要将 Flash 的读、写、擦接口对接到 fal 抽象层中。在 fal 中，使用 `struct fal_flash_dev` 结构体来让用户注册该 Flash 设备的操作接口。

`fal_flash_dev` 结构体定义如下所示：

```c
struct fal_flash_dev
{
    char name[FAL_DEV_NAME_MAX];
    /* FLASH 设备的起始地址 */
    uint32_t addr;
    size_t len;
    /* FLASH 设备最小擦除的块大小 */
    size_t blk_size;

    struct
    {
        int (*init)(void);
        int (*read)(long offset, uint8_t *buf, size_t size);
        int (*write)(long offset, const uint8_t *buf, size_t size);
        int (*erase)(long offset, size_t size);
    } ops;

    /* write minimum granularity, unit: bit.
    1(nor flash)/ 8(stm32f2/f4)/ 32(stm32f1)/ 64(stm32l4)
    0 will not take effect. */
    size_t write_gran;
};

```

`fal_flash_dev` 结构体成员简要介绍如下所示：

| 成员变量   | 说明                                                         |
| ---------- | ------------------------------------------------------------ |
| name       | Flash 设备名字，最大 23 个 ASCII 字符                        |
| addr       | Flash 设备的起始地址（片内 Flash 为 0x08000000，片外 Flash 为 0x00） |
| len        | Flash 设备容量，单位字节                                     |
| blk_size   | Flash 设备最小擦除单元的大小，单位字节                       |
| ops.init   | Flash 设备的初始化函数，会在 fal_init 接口中调用             |
| ops.read   | Flash 设备数据读取接口                                       |
| ops.write  | Flash 设备数据写入接口                                       |
| ops.erase  | Flash 设备数据擦除接口                                       |
| write_gran | Flash 设备写入最小粒度，单位为位。                           |

#### 片内 Flash 对接说明

片内 Flash 设备实例定义在 libraries/HAL_Drivers/drv_flash/drv_flash_f4.c 文件中，如下所示：

```c
const struct fal_flash_dev stm32_onchip_flash_128k =
{
    "onchip_flash_128k",
    STM32_FLASH_START_ADRESS_128K,
    FLASH_SIZE_GRANULARITY_128K,
    (128 * 1024),
    {
        NULL,
        fal_flash_read_128k,
        fal_flash_write_128k,
        fal_flash_erase_128k,
    },
    8,
};
```

Flash 设备名称为 onchip_flash_128k，最小擦除单元为 128K，无初始化接口。

#### 片外 Nor Flash 对接说明

片外 Nor Flash 设备实例定义在 libraries/Board_Drivers/fal/fal_spi_flash_sfud_port.c 文件中，使用了 RT-Thread 内置的 SFUD 框架。

> SFUD 是一款开源的串行 SPI Flash 通用驱动库，覆盖了市面上绝大多数串行 Flash 型号，无需软件开发就能驱动。

Nor Flash 设备实例如下所示：

```c
struct fal_flash_dev w25q64 =
{
    .name       = "W25Q64",
    .addr       = 0,
    .len        = 8 * 1024 * 1024,
    .blk_size   = 4096,
    .ops        = {init, read, write, erase},
    .write_gran = 1
};
```

Flash 设备名称为 W25Q64，设备容量为 8M，最小擦除单元为 4K。这里使用的 read、 write、 erase 接口最终调用 SFUD 框架中的接口，无需用户进行驱动开发。

### 例程使用说明

fal 例程代码位于 application/main.c 文件中。例程中封装了一个分区测试函数 fal_test，如下所示：

```c
static int fal_test(const char *partiton_name);
```

fal_test 函数输入参数为 Flash 分区的名字，功能是对输入分区进行完整的擦、读、写测试，覆盖整个分区。

注意， fal 在使用前，务必使用 fal_init 函数完成 fal 功能组件的初始化。

以擦除为例，对代码进行简要说明：

#### 1. 擦除整个分区

```c
/* 擦除 `partition` 分区上的全部数据 */
ret = fal_partition_erase_all(partition);
```

使用 `fal_partition_erase_all` API 接口将 `download` 分区完整擦除，擦除后 `download` 分区内的数据全为 **0xFF**。

#### 2. 校验擦除操作是否成功

```c
/* 循环读取整个分区的数据，并对内容进行检验 */
for (i = 0; i < partition->len;)
{
    rt_memset(buf, 0x00, BUF_SIZE);
    len = (partition->len - i) > BUF_SIZE ? BUF_SIZE : (partition->len - i);

    /* 从 Flash 读取 len 长度的数据到 buf 缓冲区 */
    ret = fal_partition_read(partition, i, buf, len);
    if (ret < 0)
    {
        LOG_E("Partition (%s) read failed!", partition->name);
        ret = -1;
        return ret;
    }
    for(j = 0; j < len; j++)
    {
        /* 校验数据内容是否为 0xFF */
        if (buf[j] != 0xFF)
        {
            LOG_E("The erase operation did not really succeed!");
            ret = -1;
            return ret;
        }
    }
    i += len;
}
```

通过上面的代码，循环读取整个分区的数据，并对数据内容进行校验，判断是否为 0xFF，校验通过则说明擦除操作正常。

#### 3. 写整个分区

```c
    /* 把 0 写入指定分区 */
    for (i = 0; i < partition->len;)
    {
        /* 设置写入的数据 0x00 */
        rt_memset(buf, 0x00, BUF_SIZE);
        len = (partition->len - i) > BUF_SIZE ? BUF_SIZE : (partition->len - i);

        /* 写入数据 */
        ret = fal_partition_write(partition, i, buf, len);
        if (ret < 0)
        {
            LOG_E("Partition (%s) write failed!", partition->name);
            ret = -1;
            return ret;
        }
        i += len;
    }
    LOG_I("Write (%s) partition finish! Write size %d(%dK).", partiton_name, i, i / 1024);
```

通过上面的代码，循环写入数据 0x00 到整个分区。

#### 4. 校验写操作是否成功

```c
    /* 从指定的分区读取数据并校验数据 */
    for (i = 0; i < partition->len;)
    {
        /* 清空读缓冲区，以 0xFF 填充 */
        rt_memset(buf, 0xFF, BUF_SIZE);
        len = (partition->len - i) > BUF_SIZE ? BUF_SIZE : (partition->len - i);

        /* 读取数据到 buf 缓冲区 */
        ret = fal_partition_read(partition, i, buf, len);
        if (ret < 0)
        {
            LOG_E("Partition (%s) read failed!", partition->name);
            ret = -1;
            return ret;
        }
        for(j = 0; j < len; j++)
        {
            /* 校验读取的数据是否为步骤 3 中写入的数据 0x00 */
            if (buf[j] != 0x00)
            {
                LOG_E("The write operation did not really succeed!");
                ret = -1;
                return ret;
            }
        }
        i += len;
    }
```

通过上面的代码，循环读取整个分区的数据，并对数据内容进行校验，判断是否为步骤 3 写入的数据 0x00，校验通过则说明写操作正常。

## 运行

### 编译 & 下载

- RT-Thread Studio：在 RT-Thread Studio 的包管理器中下载 `STM32F407-RT-SPARK` 资源包，然后创建新工程，执行编译。
- MDK：首先双击 mklinks.bat，生成 rt-thread 与 libraries 文件夹链接；再使用 Env 生成 MDK5 工程；最后双击 project.uvprojx 打开 MDK5 工程，执行编译。

编译完成后，将开发板的 ST-Link USB 口与 PC 机连接，然后将固件下载至开发板。

### 运行效果

```shell
 \ | /
- RT -     Thread Operating System
 / | \     4.1.1 build Jun 13 2023 11:29:32
 2006 - 2022 Copyright by RT-Thread team
[I/SFUD] Find a Winbond flash chip. Size is 8388608 bytes.
[I/SFUD] W25Q64 flash device is initialize success.
[I/SFUD] Probe SPI flash W25Q64 by SPI device spi20 success.
[D/FAL] (fal_flash_init:49) Flash device |        onchip_flash_128k | addr: 0x08000000 | len: 0x00100000 | blk_size: 0x00020000 |initialized finish.
[D/FAL] (fal_flash_init:49) Flash device |                   W25Q64 | addr: 0x00000000 | len: 0x00800000 | blk_size: 0x00001000 |initialized finish.
[I/FAL] ==================== FAL partition table ====================
[I/FAL] | name       | flash_dev         |   offset   |    length  |
[I/FAL] -------------------------------------------------------------
[I/FAL] | app        | onchip_flash_128k | 0x00000000 | 0x00060000 |
[I/FAL] | param      | onchip_flash_128k | 0x00060000 | 0x000a0000 |
[I/FAL] | easyflash  | W25Q64            | 0x00000000 | 0x00080000 |
[I/FAL] | download   | W25Q64            | 0x00080000 | 0x00100000 |
[I/FAL] | wifi_image | W25Q64            | 0x00180000 | 0x00080000 |
[I/FAL] | font       | W25Q64            | 0x00200000 | 0x00300000 |
[I/FAL] | filesystem | W25Q64            | 0x00500000 | 0x00300000 |
[I/FAL] =============================================================
[I/FAL] RT-Thread Flash Abstraction Layer initialize success.
[I/main] Flash device : onchip_flash_128k   Flash size : 1024K   Partition : param   Partition size: 640K
[I/main] Erase (param) partition finish!
[I/main] Write (param) partition finish! Write size 655360(640K).
[I/main] Fal partition (param) test success!
[I/main] Flash device : W25Q64   Flash size : 8192K   Partition : download   Partition size: 1024K

```

## SHELL 命令

为了方便用户验证 fal 功能是否正常，以及 Flash 驱动是否正确工作，分区表配置是否合理， RT-Thread 为 fal 提供了一套测试命令。

fal 测试命令如下所示：

```shell
msh >fal
Usage:
fal probe [dev_name|part_name] - probe flash device or partition by given name
fal read addr size - read 'size' bytes starting at 'addr'
fal write addr data1 ... dataN - write some bytes 'data' starting at 'addr'
fal erase addr size - erase 'size' bytes starting at 'addr'
fal bench <blk_size> - benchmark test with per block size
```

- 使用 `fal probe [dev_name|part_name]` 命令探测指定的 Flash 设备或者 Flash 分区

    当探测到指定的 Flash 设备或 分区后，会显示其属性信息，如下所示：

```shell
msh >fal probe W25Q64
Probed a flash device | W25Q64 | addr: 0 | len: 8388608 |.
msh >fal probe download
Probed a flash partition | download | flash_dev: W25Q64 | offset: 524288 | len: 1048576 |.
```
- 擦除数据

首先选择要擦除数据的分区，演示使用的是 download 分区，然后使用 fal erase 命令擦除，如下所示：

```shell
msh >fal probe download
Probed a flash partition | download | flash_dev: W25Q64 | offset: 524288 | len:
msh >fal erase 0 4096
Erase data success. Start from 0x00000000, size is 4096.
msh >
```

其中，使用擦除命令时， addr 为相应探测 Flash 分区的偏移地址， size 为不超过该分区的值，以下写入数据、读取数据与此类似。

- 写入数据

在完成擦除操作后，才能在已擦除区域写入数据，先输入 fal write ，后面跟着 N 个待写入的数据，并以空格隔开（能写入的数据数量取决与 MSH 命令行的配置）。演示从地址 0x00000008 的位置开始写入数据 1 2 3 4 5 6 7，共 7 个数据，如下所示：

```shell
msh >fal write 8 1 2 3 4 5 6 7
Write data success. Start from 0x00000008, size is 7.
Write data: 1 2 3 4 5 6 7 .
```

- 读取数据

先输入 fal read ，后面跟着待读取数据的起始地址以及长度。演示从 0 地址开始读取 64 字节数据，读取前面写入的数据，如下所示：

```shell
msh >fal read 0 64
Read data success. Start from 0x00000000, size is 64. The data is:
Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
[00000000] FF FF FF FF FF FF FF FF 01 02 03 04 05 06 07 FF
[00000010] FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
[00000020] FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
[00000030] FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
```

从日志上可以看到，在 0x00000008 地址处开始就是演示所写入的 7 个数据。
– 注： Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 作为读取数据的行号标记。

- 性能测试

    性能测试将会测试 Flash 的擦除、写入及读取速度，同时将会测试写入及读取数据的准确性，保证整个 Flash 或整个分区的 写入与读取 数据的一致性。

    先输入 `fal bench`，后面跟着待测试 Flash 的扇区大小（请查看对应的 Flash 手册，SPI Nor Flash 一般为 4096）。由于性能测试将会让整个 Flash 或者整个分区的数据丢失，所以命令最后必须跟 yes 。

```shell
msh >fal bench 4096 yes
Erasing 1048576 bytes data, waiting...
Erase benchmark success, total time: 3.744S.
Writing 1048576 bytes data, waiting...
Write benchmark success, total time: 4.096S.
Reading 1048576 bytes data, waiting...
Read benchmark success, total time: 1.961S.

```

## 注意事项

- 如果要修改分区表，请正确配置起始地址和分区大小，不要有分区重叠
- 在使用 fal 测试命令的时候，请先使用 `fal probe` 命令选择一个 Flash 分区

## 引用参考

- 文档中心：[RT-Thread 文档中心](https://www.rt-thread.org/document/site/#/)
- [FAL 组件](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/fal/fal)

