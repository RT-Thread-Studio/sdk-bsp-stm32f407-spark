# 简介

“星火 1 号”，一款专为工程师和高校学生设计的嵌入式 RTOS 开发学习板。在这个科技飞速发展的时代，嵌入式系统已经成为了现代工业、交通、通信等众多领域的核心驱动力。而 RTOS 实时操作系统作为嵌入式领域的基石，更是工程师们必须熟练掌握的核心技术。作为业界主流的 RTOS 实时操作系统 RT-Thread，我们有义务帮助更多开发者掌握这项技术。为此，我们精心打造了一款专为工程师和高校学生设计的嵌入式开发学习板。

星火 1 号主控选用了目前行业中比较常用且学习门槛较低的 STM32F407，性能强劲、功能丰富，完全能够满足嵌入式入门的需求。此开发板不仅具有众多的板载资源（Flash 存储、WIFI 通信、多个传感器），还支持丰富的扩展接口，让您轻松实现各种复杂的应用场景。通过使用这款开发学习板，您将能够深入了解 RTOS/RT-Thread 的工作原理，提升自己的技能水平，为当前以及未来的职业生涯做好充分准备。

![board-small](docs/images/board-small.jpg)

## 目录结构

```
|-- README.md
|-- docs
|-- libraries
|   |-- Board_Drivers
|   |-- HAL_Drivers
|   `-- STM32F4xx_HAL
|-- projects
|-- rt-thread
`-- sdk-bsp-rt-spark.yaml
```

- docs：星火 1 号原理图、用户手册等
- libraries：STM32F4 固件库、通用外接驱动程序
- projects：示例项目文件夹，包括各种示例代码
- rt-thread：rt-thread 源代码
- sdk-bsp-rt-spark.yaml：描述 星火 1 号 的硬件信息

## 学习路线

项目文件夹按照不同的学习阶段添加了数字编号，如 01_kernel，02_basic_led_blink，分别表示两个不同的学习阶段，编号小则内容相对简单，建议初学者按照数字编号大小顺序进行学习，循序渐进掌握 RT-Thread。

示例项目文件夹和相应学习阶段如下所示：


| 阶段                     | 序号 | 项目实现功能                      | 项目文件夹名称             |
| ------------------------ | ---- | :-------------------------------- | -------------------------- |
| 01，入门 RT-Thread 内核  | 01   | RT-Thread 内核学习                | 01_kernel                  |
| 02，简单外设的使用       | 02   | LED 闪烁例程                      | 02_basic_led_blink         |
|                          | 03   | RGB LED 例程                      | 02_basic_rgb_led           |
|                          | 04   | 按键输入例程                      | 02_basic_key               |
|                          | 05   | 蜂鸣器和 LED 控制例程             | 02_basic_irq_beep          |
|                          | 06   | 红外遥控例程                      | 02_basic_ir                |
|                          | 07   | RTC 和 RTC 闹钟的使用例程         | 02_basic_rtc               |
| 03，板载外设模块的使用   | 08   | LCD 显示例程                      | 03_driver_lcd              |
|                          | 09   | AHT10 温湿度传感器例程            | 03_driver_temp_humi        |
|                          | 10   | AP3216C 接近与光强传感器例程      | 03_driver_als_ps           |
|                          | 11   | ICM20608 六轴传感器例程           | 03_driver_axis             |
|                          | 12   | CAN 通信例程                      | 03_driver_can              |
|                          | 13   | LED MATRIX 闪烁例程               | 03_driver_led_matrix       |
| 04，RT-Thread 组件的使用 | 14   | USB 鼠标例程                      | 04_component_usb_mouse     |
|                          | 15   | TF 卡文件系统例程                 | 04_component_fs_tf_card    |
|                          | 16   | 低功耗例程                        | 04_component_pm            |
|                          | 17   | Flash 分区管理例程                | 04_component_fal           |
|                          | 18   | KV 参数存储例程                   | 04_component_kv            |
|                          | 19   | SPI Flash 文件系统例程            | 04_component_fs_flash      |
| 05，IoT 相关组件的使用   | 20   | WiFi 管理例程                     | 05_iot_wifi_manager        |
|                          | 21   | MQTT 协议通信例程                 | 05_iot_mqtt                |
|                          | 22   | HTTP Client 功能实现例程          | 05_iot_http_client         |
|                          | 23   | MEMBEDTLS 例程                    | 05_iot_mbedtls             |
|                          | 24   | Ymodem 协议固件升级例程           | 05_iot_ota_ymodem          |
|                          | 25   | HTTP 协议固件升级例程             | 05_iot_ota_http            |
|                          | 26   | 网络小工具集使用例程              | 05_iot_netutils            |
|                          | 27   | 中国移动 OneNET 云平台接入例程    | 05_iot_cloud_onenet        |
|                          | 28   | 阿里云物联网平台接入例程          | 05_iot_cloud_ali_iotkit    |
|                          | 29   | 使用 Web 服务器组件：WebNet       | 05_iot_web_server          |
| 06，综合 Demo 学习       | 30   | LVGL 例程                         | 06_demo_lvgl               |
|                          | 31   | MicroPython 例程                  | 06_demo_micropython        |
|                          | 32   | 板载 LED matrix 和 RS485 驱动例程 | 06_demo_rs485_led_matrix   |
|                          | 33   | nes 模拟器实验                    | 06_demo_nes_simulator      |
|                          | 34   | 开发板综合 Demo（出厂 Demo）      | 06_demo_factory            |
| 07，教育套件对应模块例程 | 35   | 薄膜键盘模块例程                  | 07_module_key_matrix       |
|                          | 36   | PMOD ENC28J60 以太网模块例程      | 07_module_spi_eth_enc28j60 |

## 使用

sdk-bsp-stm32f407-spark 支持 RT-Thread Studio 和 MDK 开发。

### RT-Thread Studio 开发

1. 打开 RT-Thread Studio 的包管理器，安装 ` 星火 1 号开发板 ` 资源包
2. 安装完成后，选择基于 BSP 创建工程即可

### MDK 开发

为了避免 SDK 在持续更新中，每一个 `projects` 都创建一份 `rt-thread` 文件夹 和 `libraries` 文件夹导致的 SDK 越来越臃肿，所以这些通用文件夹被单独提取了出来。这样就会导致直接打开 `MDK` 的工程编译会提示缺少上述两个文件夹的文件，我们使用如下步骤解决这个问题：

1. 双击某个 `project` 目录下的 `mklinks.bat` 文件，或者使用 [Env](https://club.rt-thread.org/ask/question/5699.html) 工具执行 mklink 命令，分别为 `rt-thread` 及 `libraries` 文件创建符号链接。
2. 查看目录下是否有 `rt-thread` 和 `libraries` 的文件夹图标。
3. 使用 [Env](https://club.rt-thread.org/ask/question/5699.html) 工具执行 scons --target=mdk5 更新 MDK5 工程文件。

## 交流平台

对 星火 1 号 感兴趣的小伙伴可以加入 QQ 群 - RT-Thread 星火学习板 群号: 839583041、852752783。

