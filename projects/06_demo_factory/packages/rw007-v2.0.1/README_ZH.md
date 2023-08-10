# rw007

中文页 | [English](README.md)

## 1. 简介

**RW007**是由上海睿赛德电子科技有限公司开发基于Cortex-M4 WIFI SOC的SPI/UART 高速wifi模块。该仓库为**rw007**的SPI驱动

**RW007**硬件设计简单，SPI模式下用户只需要预留1组SPI信号，一个中断输入，一个IO输出即可，包含电源和地总共8个引脚。

### 1.1. 文件结构

| 文件夹 | 说明 |
| ---- | ---- |
| src  | 核心驱动源码，主要实现通信逻辑 |
| inc  | 头文件目录 |
| example | 平台移植示例 |

### 1.2 许可证

at_device package 遵循 Apache 2.0 许可，详见 `LICENSE` 文件。

### 1.3 依赖

- RT-Thread 3.0+
- RT-Thread LWIP 组件
- RT-Thread SPI 驱动框架
- RT-Thread PIN 驱动框架(示例平台代码依赖)

### 1.4 配置宏说明

自动配置时相关配置宏将在env中被配置 手动配置参考如下

类型说明

- bool: 定义生效 未定义 不生效
- string: 字符串
- int: 数值

|宏|类型|功能|
|--|--|--|
|PKG_USING_RW007|bool|开启rw007驱动，使用该软件包则需要定义该宏|
|RW007_USING_STM32_DRIVERS|bool|使能STM32平台移植示例|
|RW007_USING_IMXRT_DRIVERS|bool|使能IMXRT平台移植示例|
|RW007_SPI_BUS_NAME|string|example中使用的SPI总线设备名称|
|RW007_CS_PIN|int|example中使用的SPI 片选引脚在pin驱动中的序号|
|RW007_BOOT0_PIN|int|example中使用的BOOT0引脚在pin驱动中的序号(与SPI的CLK是同一引脚复用)|
|RW007_BOOT1_PIN|int|example中使用的BOOT1引脚在pin驱动中的序号(与SPI的CS是同一引脚复用)|
|RW007_INT_BUSY_PIN|int|example中使用的INT/BUSY引脚在pin驱动中的序号|
|RW007_RST_PIN|int|example中使用的RST引脚在pin驱动中的序号|

## 2. 注意事项

由于存在引脚复用情况，所以bsp的SPI的驱动需要引脚配置在Config时进行。

## 4. 联系方式

- 维护：RT-Thread 开发团队
- 主页：https://github.com/RT-Thread-packages/rw007
