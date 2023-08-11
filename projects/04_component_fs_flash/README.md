# SPI Flash 文件系统例程

## 简介

本例程使用板载的 SPI Flash 作为文件系统的存储设备，展示如何在 Flash 的指定分区上创建文件系统，并挂载文件系统到 rt-thread 操作系统中。文件系统挂载成功后，展示如何使用文件系统提供的功能对目录和文件进行操作。

本例程中使用的是 FAT 文件系统，也支持 Littlefs 文件系统。 Littlefs 文件系统的使用可以参考 [RT-Thread 文档中心](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/README)。

由于本例程需要使用 fal 组件对存储设备进行分区等操作，所以在进行本例程的实验前，需要先进行 fal 例程的实验，对 fal 组件的使用有一定的了解。

## 硬件说明

本次示例和存储器连接通过 SPI 接口，使用的硬件接口是 SPI2，原理图如下所示：

![SPI FLASH 原理图](figures/w25q64.png)

SPI FLASH 在开发板中的位置如下图所示：

![SPI FLASH 位置](figures/board.png)

## 软件说明

本例程的源码位于 `/projects/04_component_fs_flash`。

###  挂载操作代码说明

挂载文件系统的源代码位于 main.c 中。

在示例代码中会执行如下操作：

1. 使用 `fal_blk_device_create()` 函数在 spi flash 中名为 "filesystem" 的分区上创建一个块设备，作为文件系统的存储设备。
2. 使用 `dfs_mount()` 函数将该块设备中的文件系统挂载到根目录下的 `/fal` 目录上。

```c
#define FS_PARTITION_NAME  "filesystem"

int main(void)
{
    /* 初始化 fal 功能 */
    fal_init();

    /* 在 spi flash 中名为 "filesystem" 的分区上创建一个块设备 */
    struct rt_device *flash_dev = fal_blk_device_create(FS_PARTITION_NAME);
    if (flash_dev == NULL)
    {
        LOG_E("Can't create a block device on '%s' partition.", FS_PARTITION_NAME);
    }
    else
    {
        LOG_D("Create a block device on the %s partition of flash successful.", FS_PARTITION_NAME);
    }

    /* 挂载 spi flash 中名为 "filesystem" 的分区上的文件系统 */
    if (dfs_mount(flash_dev->parent.name, "/fal", "elm", 0, 0) == 0)
    {
        LOG_I("Filesystem initialized!");
    }
    else
    {
        LOG_E("Failed to initialize filesystem!");
        LOG_D("You should create a filesystem on the block device first!");
    }

    return 0;
}
```

## 运行

### 编译 & 下载

- RT-Thread Studio：在 RT-Thread Studio 的包管理器中下载 `STM32F407-RT-SPARK` 资源包，然后创建新工程，执行编译。
- MDK：首先双击 mklinks.bat，生成 rt-thread 与 libraries 文件夹链接；再使用 Env 生成 MDK5 工程；最后双击 project.uvprojx 打开 MDK5 工程，执行编译。

编译完成后，将开发板的 ST-Link USB 口与 PC 机连接，然后将固件下载至开发板。

###  运行效果

1. 在 PC 端使用终端工具打开开发板的 ST-Link 提供的虚拟串口，设置 115200 8 1 N 。
2. 按下复位按键重启开发板，如果看到提示 "Failed to initialize filesystem!，这是因为指定的挂载设备中还没有创建文件系统。
3. 在 msh 中使用命令 mkfs -t elm filesystem 可以在名为 “filesystem” 的块设备上创建 elm-fat 类型的文件系统。
4. 此时按下复位按键重启开发板，可以看到提示 "FileSystem initialized!"，表明文件系统挂载成功。

打印信息如下所示：

```shell
 \ | /
- RT -     Thread Operating System
 / | \     4.1.1 build Jun  9 2023 18:14:07
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
[I/FAL] The FAL block device (filesystem) created successfully
[D/main] Create a block device on the filesystem partition of flash successful.     # 在 flash 的文件系统分区上创建块设备成功
[I/main] Filesystem initialized!                                                    # 文件系统初始化成功
msh />
```

### 常用功能展示
### ls: 查看当前目录信息
```shell
msh />ls                          # 使用 ls 命令查看文件系统目录信息
Directory /:                      # 可以看到已经存在根目录 /
fal                 <DIR>         # ROMFS 内置的fal目录
```

### cd：切换目录到FAL

```shell
msh />cd fal                      # 使用 cd 命令切换当前目录
msh /fal>
```

### mkdir: 创建文件夹

```shell
msh /fal>mkdir rt-thread             # 创建 rt-thread 文件夹
msh /fal>ls                          # 查看目录信息如下
Directory /fal:
rt-thread           <DIR>
```

### echo: 将输入的字符串输出到指定输出位置
```shell
msh /fal>echo "hello rt-thread!!!"             # 将字符串输出到标准输出
hello rt-thread!!!
msh /fal>echo "hello rt-thread!!!" hello.txt   # 将字符串出输出到 hello.txt
msh /fal>ls
Directory /fal:
rt-thread           <DIR>
hello.txt           18
msh /fal>

### cat: 查看文件内容
​```shell
msh /fal>cat hello.txt                     # 查看 hello.txt 文件的内容并输出
hello rt-thread!!!
```

### rm: 删除文件夹或文件
```shell
msh /fal>ls                                # 查看当前目录信息
Directory /fal:
rt-thread           <DIR>
hello.txt           18
msh /fal>rm rt-thread                      # 删除 rt-thread 文件夹
msh /fal>ls
Directory /fal:
hello.txt           18
msh /fal>rm hello.txt                      # 删除 hello.txt 文件
msh /fal>ls
Directory /:
msh /fal>
```
## 注意事项

挂载文件系统之前一定要先在存储设备中创建相应类型的文件系统，否则会挂载失败。

## 引用参考

- 组件：[虚拟文件系统](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/filesystem/filesystem)
- 设备与驱动：[SPI 设备](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/spi/spi)
- 文档中心：[RT-Thread 文档中心](https://www.rt-thread.org/document/site/#/)
