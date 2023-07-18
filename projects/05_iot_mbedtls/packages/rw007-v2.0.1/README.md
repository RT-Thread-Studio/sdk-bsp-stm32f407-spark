# rw007

[中文页](README_ZH.md) | English

## 1. Introduction

**RW007** is a SPI/UART high-speed wifi module based on Cortex-M4 WIFI SOC developed by Shanghai Ruiside Electronic Technology Co., Ltd. The warehouse is the SPI driver of **rw007**

**RW007** The hardware design is simple. In SPI mode, users only need to reserve 1 set of SPI signals, one interrupt input, and one IO output, including a total of 8 pins for power and ground.

### 1.1. File structure

| Folder | Description |
| ---- | ---- |
| src | Core driver source code, which mainly implements communication logic |
| inc | Header file directory |
| example | Platform porting example |

### 1.2 License

The at_device package complies with the Apache 2.0 license, see the `LICENSE` file for details.

### 1.3 Dependency

- RT-Thread 3.0+
- RT-Thread LWIP component
- RT-Thread SPI driver framework
- RT-Thread PIN driver framework (sample platform code dependency)

### 1.4 Configuration Macro Description

The relevant configuration macros will be configured in env during automatic configuration. The manual configuration is as follows

Type description

- bool: definition is valid, undefined, not valid
- string: string
- int: number

|Macro|Type|Function|
|--|--|--|
|PKG_USING_RW007|bool|Enable rw007 driver, use this software package, you need to define this macro|
|RW007_USING_STM32_DRIVERS|bool|Enable STM32 platform migration example|
|RW007_USING_IMXRT_DRIVERS|bool|Enable IMXRT platform migration example|
|RW007_SPI_BUS_NAME|string|The SPI bus device name used in example|
|RW007_CS_PIN|int|The serial number of the SPI chip select pin used in the example in the pin driver|
|RW007_BOOT0_PIN|int|The serial number of the BOOT0 pin used in the example in the pin driver (the same pin is multiplexed with the CLK of SPI)|
|RW007_BOOT1_PIN|int|The serial number of the BOOT1 pin used in the example in the pin driver (the same pin is multiplexed with the CS of SPI)|
|RW007_INT_BUSY_PIN|int|The serial number of the INT/BUSY pin used in the example in the pin driver|
|RW007_RST_PIN|int|The serial number of the RST pin used in the example in the pin driver|

## 2. Matters needing attention

Because of the pin multiplexing situation, the driver of the bsp SPI needs to be configured during Config.

## 4. Contact

- Maintenance: RT-Thread development team
- Homepage: https://github.com/RT-Thread-packages/rw007
