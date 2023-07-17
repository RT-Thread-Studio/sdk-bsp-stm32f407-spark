# HTTP 协议固件升级例程

## 例程说明

HTTP 是一种超文本传输协议，采用请求应答的通讯模式，可以基于通用 socket 实现极简客户端程序，广泛应用在互联网上。 HTTP 请求应答的方式，及其很小的客户端代码量，也使其很方便地应用在物联网设备中，用于与服务器进行数据交互，以及 OTA 固件下载。

本例程基于 HTTP 客户端实现 HTTP OTA 固件下载器，通过 HTTP 协议从 HTTP 服务器下载升级固件到设备。 HTTP 客户端代码参考 RT-Thread WebClient 软件包 。

在该例程中用到的 bootloader 程序以 bin 文件的形式提供，文件位于 bin/bootloader.bin。

## 背景知识

参考 Ymodem 固件升级例程。

## 硬件说明

本例程使用到的硬件资源如下所示：

- UART1
- 片内 FLASH (1024KBytes)
- 片外 Nor Flash (8MBytes)

本例程中， bootloader 程序和 app 程序存放在 STM32F4 MCU 的内部 FLASH 中， download 下载区域存放在外部扩展的 Nor FLASH 中。

## 分区表

| 名称        | 设备              | 起始地址           | 大小              | 结束地址          | 说明                |
|-------------|------------------|---------------------|-------------------|-------------------|---------------------|
| bootloader  | onchip_flash_128k | 0                   | 128 * 1024        | 0x20000           | 引导加载程序分区     |
| app         | onchip_flash_128k | 128 * 1024          | 384 * 1024        | 0x80000           | 应用程序分区         |
| easyflash   | W25Q64           | 0                   | 512 * 1024        | 0x80000           | Easyflash 分区       |
| download    | W25Q64           | 512 * 1024          | 1024 * 1024       | 0x180000          | 下载分区             |
| wifi_image  | W25Q64           | (512 + 1024) * 1024 | 512 * 1024        | 0x180000          | WiFi 镜像分区       |
| font        | W25Q64           | (512 + 1024 + 512) * 1024 | 3 * 1024 * 1024 | 0x480000          | 字体分区             |
| filesystem  | W25Q64           | (512 + 1024 + 512 + 3 * 1024) * 1024 | 3 * 1024 * 1024 | 0x780000          | 文件系统分区         |

bootloader 程序的分区表定义在 bootloader 程序 中，如果需要修改 bootloader 的分区表，则需要修改 bootloader 程序。目前不支持用户自定义 bootloader，如果有商用需求，请联系 RT-Thread 获取支持。

## 软件说明

本例程的源码位于 `/projects/05_iot_ota_http`。

| 文件路径                            | 说明                                 |
|-----------------------------------|------------------------------------|
| application/main.c                  | app 程序入口 |
| bin/bootloader.bin                  | 需要烧录到 0x08000000 地址的二进制文件完成 |
| bin/v1.0.0.bin                    | 第一个版本的应用程序，代码段被链接到了 0x08020000 处 |
| bin/v2.0.0.bin                    | 第二个版本的应用程序，代码段被链接到了 0x08020000 处 |
| libraries/Board_Drivers/fal       | Flash 抽象层组件（fal）的移植文件        |
| rt-thread/components/fal          | FAL 组件                           |
| packages/ota_downloader           | 包含 ymodem 和 http 下载功能的 otadownloader 的软件包                         |

HTTP 固件升级流程如下所示：

1. 打开 tools/MyWebServer 软件，并配置本机 IP 地址和端口号，选择存放升级固件的目录
2. 在 MSH 中使用 http_ota 命令下载固件到 download 分区
3. bootloader 对 OTA 升级固件进行校验、解密和搬运（搬运到 app 分区）
4. 程序从 bootloader 跳转到 app 分区执行新的固件

### 程序说明

HTTP OTA 固件下载器程序在 packages/ota_downloader/src 目录下：

```c
void http_ota(uint8_t argc, char **argv)
{
    if (argc < 2)
    {
        rt_kprintf("using uri:" HTTP_OTA_URL "\n");
        http_ota_fw_download(HTTP_OTA_URL);
    }
    else
    {
        http_ota_fw_download(argv[1]);
    }
}
/**
 * msh />http_ota [url]
*/
MSH_CMD_EXPORT(http_ota, Use HTTP to download the firmware);
```

```c
static int http_ota_fw_download(const char* uri)
{
    int ret = RT_EOK;
    struct webclient_session* session = RT_NULL;

    /* create webclient session and set header response size */
    session = webclient_session_create(GET_HEADER_BUFSZ);
    if (!session)
    {
        LOG_E("open uri failed.");
        ret = -RT_ERROR;
        goto __exit;
    }

    /* get the real data length */
    webclient_shard_head_function(session, uri, &file_size);

    if (file_size == 0)
    {
        LOG_E("Request file size is 0!");
        ret = -RT_ERROR;
        goto __exit;
    }
    else if (file_size < 0)
    {
        LOG_E("webclient GET request type is chunked.");
        ret = -RT_ERROR;
        goto __exit;
    }
    LOG_I("OTA file size is (%d)", file_size);
    LOG_I("\033[1A");

    /* Get download partition information and erase download partition data */
    if ((dl_part = fal_partition_find("download")) == RT_NULL)
    {
        LOG_E("Firmware download failed! Partition (%s) find error!", "download");
        ret = -RT_ERROR;
        goto __exit;
    }

    LOG_I("Start erase flash (%s) partition!", dl_part->name);

    if (fal_partition_erase(dl_part, 0, file_size) < 0)
    {
        LOG_E("Firmware download failed! Partition (%s) erase error!", dl_part->name);
        ret = -RT_ERROR;
        goto __exit;
    }
    LOG_I("Erase flash (%s) partition success!", dl_part->name);

    /* register the handle function, you can handle data in the function */
    webclient_register_shard_position_function(session, http_ota_shard_download_handle);

    /* the "memory size" that you can provide in the project and uri */
    ret = webclient_shard_position_function(session, uri, begin_offset, file_size, HTTP_OTA_BUFF_LEN);

    /* clear the handle function */
    webclient_register_shard_position_function(session, RT_NULL);

    if (ret == RT_EOK)
    {
        if (session != RT_NULL)
        {
            webclient_close(session);
            session = RT_NULL;
        }

        LOG_I("\033[0B");
        LOG_I("Download firmware to flash success.");
        LOG_I("System now will restart...");

        rt_thread_delay(rt_tick_from_millisecond(5));

        /* Reset the device, Start new firmware */
        extern void rt_hw_cpu_reset(void);
        rt_hw_cpu_reset();
    }
    else
    {
        LOG_E("Download firmware failed.");
    }

__exit:
    if (session != RT_NULL)
        webclient_close(session);
    begin_offset = 0;

    return ret;
}
```

使用 MSH_CMD_EXPORT 命令将 http_ota 命令导入 msh，使用此命令即可调用 http 下载功能。

## 运行

###  烧录旧版本程序

首先将 bootloader.bin 和 v1.0.0.bin 程序烧录进单片机，烧录过程参考 Ymodem 。

### 烧录新版本程序

1. 打包新版本程序, 参考 Ymodem 固件升级例程。
2. 启动 HTTP OTA 升级
    1. 解压 /tools/MyWebServer.zip 到当前目录（解压后有 /tools/MyWebServer 目录）
    2. 打开 /tools/MyWebServer 目录下的 MyWebServer.exe 软件配置 MyWebServer 软件，选择 OTA 固件（rbl 文件）的路径，设置本机 IP 和端口号，并启动服务器，如下图所示：
    ![MyWebServer 说明](figures/aaa.png)
3. 连接开发板串口，复位开发板，进入 MSH 命令行
4. 在设备的命令行里输入 http_ota http://192.168.1.10:80/rtthread.rbl 命令启动 HTTP OTA 升级根据您的 MyWebServer 软件的 IP 和端口号配置修改 http_ota 命令。
5. 设备升级过程

输入命令后，会擦除 download 分区，下载升级固件。下载过程中会打印下载进度条。

```shell
http_ota http://192.168.181.150:80/rtthread.rbl
[I/http_ota] OTA file size is (379976)
[I/http_ota] Start erase flash (download) partition!
[I/http_ota] Erase flash (download) partition success!
[I/http_ota] Download: [====================================================================================================] 100%
[I/http_ota] Download firmware to flash success.
[I/http_ota] System now will restart...

```

设备重启后， bootloader 会对升级固件进行合法性和完整性校验，验证成功后将升级固件从 download 分区搬运到目标分区（这里是 app 分区）。

升级成功后设备状态如下所示：

```shell
 \ | /
- RT -     Thread Operating System
 / | \     4.1.1 build Jun 14 2023 13:30:35
 2006 - 2022 Copyright by RT-Thread team
lwIP-2.0.3 initialized!
[I/sal.skt] Socket Abstraction Layer initialize success.
[I/SFUD] Find a Winbond flash chip. Size is 8388608 bytes.
[I/SFUD] W25Q64 flash device is initialize success.
[I/SFUD] Probe SPI flash W25Q64 by SPI device spi20 success.
msh />[E/[RW007]] The wifi Stage 1 status 0 0 0 1
[I/WLAN.dev] wlan init success
[I/WLAN.lwip] eth device init ok name:w0
[I/WLAN.dev] wlan init success
[I/WLAN.lwip] eth device init ok name:w1

rw007  sn: [rw007c745bb22fc584aa6cecc]
rw007 ver: [RW007_2.1.0-a7a0d089-57]

[D/main] The current version of APP firmware is 2.0.0
[D/FAL] (fal_flash_init:49) Flash device |        onchip_flash_128k | addr: 0x08000000 | len: 0x00100000 | blk_size: 0x00020000 |initialized finish.
[D/FAL] (fal_flash_init:49) Flash device |                   W25Q64 | addr: 0x00000000 | len: 0x00800000 | blk_size: 0x00001000 |initialized finish.
[I/FAL] ==================== FAL partition table ====================
[I/FAL] | name       | flash_dev         |   offset   |    length  |
[I/FAL] -------------------------------------------------------------
[I/FAL] | bootloader | onchip_flash_128k | 0x00000000 | 0x00020000 |
[I/FAL] | app        | onchip_flash_128k | 0x00020000 | 0x00060000 |
[I/FAL] | easyflash  | W25Q64            | 0x00000000 | 0x00080000 |
[I/FAL] | download   | W25Q64            | 0x00080000 | 0x00100000 |
[I/FAL] | wifi_image | W25Q64            | 0x00180000 | 0x00080000 |
[I/FAL] | font       | W25Q64            | 0x00200000 | 0x00300000 |
[I/FAL] | filesystem | W25Q64            | 0x00500000 | 0x00300000 |
[I/FAL] =============================================================
[I/FAL] RT-Thread Flash Abstraction Layer initialize success.
[I/FAL] The FAL block device (filesystem) created successfully
[D/main] Create a block device on the filesystem partition of flash successful.
[I/main] Filesystem initialized!

```

设备升级完成后会自动运行新的固件，从上图中的日志上可以看到， app 固件已经从 1.0.0 版本升级到了 2.0.0 版本。

2.0.0 版本的固件同样是支持 HTTP OTA 下载功能的，因此可以一直使用 HTTP 进行 OTA 升级。用户如何需要增加自己的业务代码，可以基于该例程进行修改。

## 注意事项

在运行该例程前，请务必先将 all.bin 固件烧录到设备

- 必须使用 .rbl 格式的升级固件
- 打包 OTA 升级固件时，分区名字必须与分区表中的名字相同（升级 app 固件对应 app 分区），参考分区表章节
- MyWebServer 软件可能会被您的防火墙限制功能，使用前请检查 Windows 防火墙配置
- 串口波特率 115200，无奇偶校验，无流控
- app 固件必须从 0x08020000 地址开始链接，否则应用 bootloader 会跳转到 app 失败 app 固件存储在 app 分区内，起始地址为 0x08020000，如果用户需要升级其他 app 程序，请确保编译器从 0x08020000 地址链接 app 固件。

## 引用参考

- 文档中心：[RT-Thread 文档中心](https://www.rt-thread.org/document/site/#/)
- OTA 说明请参考 **Ymodem 固件升级**章节
- WiFi 使用说明请参考 **使用 WiFi Manager 管理、操作 WiFi 网络**章节

