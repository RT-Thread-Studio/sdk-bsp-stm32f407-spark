# HTTP Client 功能实现例程

## 简介

本例程介绍如何使用 WebClient 软件包发送 HTTP 协议 GET 和 POST 请求，并且接收响应的数据。

### HTTP 协议

HTTP （Hypertext Transfer Protocol）协议，即超文本传输协议，是互联网上应用最为广泛的一种网络协议，由于其简捷、快速的使用方式，适用于分布式和合作式超媒体信息系统。 HTTP 协议是基于 TCP/IP 协议的网络应用层协议。默认端口为 80 端口。协议最新版本是 HTTP 2.0，目前是用最广泛的是 HTTP 1.1。

HTTP 协议是一种请求 / 响应式的协议。一个客户端与服务器建立连接之后，发送一个请求给服务器。服务器接收到请求之后，通过接收到的信息判断响应方式，并且给予客户端相应的响应，完成整个 HTTP 数据交互流程。

### WebClient 软件包

WebClient 软件包是 RT-Thread 自主研发的，基于 HTTP 协议的客户端实现，它提供设备与 HTTP 服务器的通讯的基本功能。

WebClient 软件包功能特点：

- **支持 IPv4/IPv6 地址**

  WebClient 软件包会自动根据传入的 URI 地址的格式判断是 IPv4 地址或 IPv6 地址，并且从中解析出连接服务器需要的信息，提高代码兼容性。

- **支持 GET/POST 请求方法**

  目前 WebClient 软件包支持 HTTP 协议 GET 和 POST 请求方法，这也是嵌入式设备最常用到的两个命令类型，满足设备开发需求。

- **支持文件的上传和下载功能**

  WebClient 软件包提供文件上传和下载的接口函数，方便用户直接通过 GET/POST 请求方法上传本地文件到服务器或者下载服务器文件到本地。

- **支持 HTTPS 加密传输**

  WebClient 软件包可以采用 TLS 加密方式传输数据，保证数据的安全性和完整性 。

- **完善的头部数据添加和处理方式**

  WebClient 软件包中提供简单的添加发送请求头部信息的方式，方便用于快速准确的拼接头部信息。

## 硬件说明

本例程需要依赖 星火 1 号 板卡上的 WiFi 模块完成网络通信，因此请确保硬件平台上的 WiFi 模组可以正常工作。

## 软件说明

本例程的源码位于 `/projects/05_iot_http_client`。

HTTP Client 例程重要文件说明：

| 文件路径                          | 说明                               |
|-----------------------------------|----------------------------------|
| applications/main.c               | app 入口（WebClient 例程程序）       |
| packages/webclient-v2.2.0        | webclient 软件包                     |
| packages/webclient-v2.2.0/inc | webclient 软件包头文件                |
| packages/webclient-v2.2.0/src | webclient 软件包源码文件               |

本例程主要实现设备通过 GET 请求方式从指定服务器中获取数据，之后通过 POST 请求方式上传一段数据到指定服务器，并接收服务器响应数据。例程的源代码位于 applications/main.c 中。

其中 main 函数主要完成 wlan 网络初始化配置，并等待设备联网成功，程序如下所示：

```c
int main(void)
{
    int result = RT_EOK;

    result = rt_sem_init(&net_ready, "net_ready", 0, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -RT_ERROR;
    }

    /* 注册 wlan 连接网络成功的回调，wlan 连接网络成功后释放'net_ready' 信号量 */
    rt_wlan_register_event_handler(RT_WLAN_EVT_READY, wlan_ready_handler, RT_NULL);
    /* 注册 wlan 网络断开连接的回调 */
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_DISCONNECTED, wlan_station_disconnect_handler, RT_NULL);

    /* 等待 wlan 连接网络成功 */
    result = rt_sem_take(&net_ready, RT_WAITING_FOREVER);
    if (result != RT_EOK)
    {
        LOG_E("Wait net ready failed!");
        rt_sem_delete(&net_ready);
        return -RT_ERROR;
    }

    /* HTTP GET 请求发送 */
    webclient_get_smpl(HTTP_GET_URL);
    /* HTTP POST 请求发送 */
    webclient_post_smpl(HTTP_POST_URL,post_data,rt_strlen(post_data));
}
```

设备成功接入网络之后，会自动顺序的执行 webclient_get_smpl() 和 webclient_post_smpl() 函数，通过 HTTP 协议发送 GET 和 POST 请求。

1. GET 请求发送, 会根据传入的 URL 发送 get 请求然后返回结果存储在 response 里面

```c
int webclient_get_smpl(const char *uri)
{
    char *response = RT_NULL;
    size_t resp_len = 0;
    int index;

    if (webclient_request(uri, RT_NULL, RT_NULL, 0, (void **)&response, &resp_len) < 0)
    {
        rt_kprintf("webclient send get request failed.");
        return -RT_ERROR;
    }

    rt_kprintf("webclient send get request by simplify request interface.\n");
    rt_kprintf("webclient get response data: \n");
    for (index = 0; index < rt_strlen(response); index++)
    {
        rt_kprintf("%c", response[index]);
    }
    rt_kprintf("\n");

    if (response)
    {
        web_free(response);
    }

    return 0;
}
```

2. POST 请求发送

```c
int webclient_post_smpl(const char *uri, const char *post_data, size_t data_len)
{
    char *response = RT_NULL;
    char *header = RT_NULL;
    size_t resp_len = 0;
    int index = 0;

    webclient_request_header_add(&header, "Content-Length: %d\r\n", strlen(post_data));
    webclient_request_header_add(&header, "Content-Type: application/octet-stream\r\n");

    if (webclient_request(uri, header, post_data, data_len, (void **)&response, &resp_len) < 0)
    {
        rt_kprintf("webclient send post request failed.");
        web_free(header);
        return -RT_ERROR;
    }

    rt_kprintf("webclient send post request by simplify request interface.\n");
    rt_kprintf("webclient post response data: \n");
    for (index = 0; index < resp_len; index++)
    {
        rt_kprintf("%c", response[index]);
    }
    rt_kprintf("\n");

    if (header)
    {
        web_free(header);
    }

    if (response)
    {
        web_free(response);
    }

    return 0;
}
```

本例程中使用 POST 方式请求发送一段数据到 HTTP 服务器 www.rt-thread.com ， HTTP 服务器响应并下发同样的数据到设备上。

例程中调用封装的 `webclient_post_smpl()` 函数发送 POST 请求， 该函数内部直接使 用 `webclient_request()` 完成整个 POST 请求发送头部信息和获取响应数据的流程，这里客户端发送默认 POST 请求头部信息，响应的数据存储到 response 缓冲区并打印到 FinSH 控制台。

## 运行

### 编译 & 下载

- RT-Thread Studio：在 RT-Thread Studio 的包管理器中下载 `STM32F407-RT-SPARK` 资源包，然后创建新工程，执行编译。
- MDK：首先双击 mklinks.bat，生成 rt-thread 与 libraries 文件夹链接；再使用 Env 生成 MDK5 工程；最后双击 project.uvprojx 打开 MDK5 工程，执行编译。

编译完成后，将开发板的 ST-Link USB 口与 PC 机连接，然后将固件下载至开发板。

按下复位按键重启开发板，程序运行日志如下所示：

```shell
 \ | /
- RT -     Thread Operating System
 / | \     4.1.1 build Jun  9 2023 15:23:02
 2006 - 2022 Copyright by RT-Thread team
lwIP-2.0.3 initialized!
[I/sal.skt] Socket Abstraction Layer initialize success.
msh >[E/[RW007]] The wifi Stage 1 status 67452301 efc1ab09 1 1
[I/WLAN.dev] wlan init success
[I/WLAN.lwip] eth device init ok name:w0
[I/WLAN.dev] wlan init success
[I/WLAN.lwip] eth device init ok name:w1

rw007  sn: [rw007c745bb22fc584aa6cecc]
rw007 ver: [RW007_2.1.0-a7a0d089-57]

wifi join test_ssid 12345678
[I/WLAN.mgnt] wifi connect success ssid:test_ssid
msh >[I/WLAN.lwip] Got IP address : 192.168.137.187
webclient send get request by simplify request interface.
webclient get response data:
RT-Thread is an open source IoT operating system from China, which has strong scalability: from a tiny kernel running on a tiny core, for example ARM Cortex-M0, or Cortex-M3/4/7, to a rich feature system running on MIPS32, ARM Cortex-A8, ARM Cortex-A9 DualCore etc.

webclient send post request by simplify request interface.
webclient post response data:
RT-Thread is an open source IoT operating system from China!

```

重启开发板之后，使用 wifi 命令连接 wifi，在连接成功的回调函数里面启动 http 客户端发送 get 请求和 post 请求。并且打印返回的结果。

## 注意事项

使用本例程前需要先连接 WiFi。

## 引用参考

- webclient 软件包：[https://github.com/RT-Thread-packages/webclient](https://github.com/RT-Thread-packages/webclient)
- 文档中心：[RT-Thread 文档中心](https://www.rt-thread.org/document/site/#/)

