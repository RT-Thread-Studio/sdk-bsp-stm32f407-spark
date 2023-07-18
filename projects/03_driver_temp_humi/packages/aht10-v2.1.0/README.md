# AHT10

[中文页](README_ZH.md) | English

## Introduction

The AHT10 software package provides the basic functions of using the temperature and humidity sensor `aht10`, and provides the optional function of the software average filter. And the new version of this software package has been connected to the Sensor framework, through the Sensor framework, developers can quickly drive this sensor. To view the README of the **old version of the package**, please click [here](README_OLD.md).

The basic function is mainly determined by the sensor `aht10`: in the range of input voltage `1.8v-3.3v`, the measuring range and accuracy of temperature and humidity are shown in the table below

| Function | Range | Accuracy |
| ---- | ---- | ---- |
| Temperature | `-40℃-85℃` |`±0.5℃`|
| Humidity | `0%-100%` |`±3%`|

## Support

| Includes equipment | Temperature | Humidity |
| ---- | ---- | ---- |
| **Communication Interface** | | |
| IIC | √ | √ |
| **Work Mode** | | |
| Polling | √ | √ |
| Interruption | | |
| FIFO | | |

## Instructions for use

### Dependence

- RT-Thread 4.0.0+
- Sensor component
- IIC driver: aht10 device uses IIC for data communication, which requires system IIC driver support

### Get the package

To use the aht10 package, you need to select it in the package management of RT-Thread. The specific path is as follows:

```
RT-Thread online packages --->
  peripheral libraries and drivers --->
    sensors drivers --->
            aht10: digital humidity and temperature sensor aht10 driver library.
                         [] Enable average filter by software
                               Version (latest) --->
```

**Enable average filter by software**: After selection, the software average filter function for collecting temperature and humidity will be turned on.

**Version**: Software package version selection, the latest version is selected by default.

### Using packages

The initialization function of aht10 package is as follows:

```
int rt_hw_aht10_init(const char *name, struct rt_sensor_config *cfg);
```

This function needs to be called by the user. The main functions of the function are:

- Device configuration and initialization (configure the interface device according to the incoming configuration information);
- Register the corresponding sensor device and complete the registration of aht10 device;

#### Initialization example

```c
#include "sensor_asair_aht10.h"
#define AHT10_I2C_BUS "i2c4"

int rt_hw_aht10_port(void)
{
    struct rt_sensor_config cfg;

    cfg.intf.dev_name = AHT10_I2C_BUS;
    cfg.intf.user_data = (void *)AHT10_I2C_ADDR;
    
    rt_hw_aht10_init("aht10", &cfg);

    return RT_EOK;
}
INIT_ENV_EXPORT(rt_hw_aht10_port);
```

## Precautions

In the test, it is found that the sensor `aht10` is a non-standard I2C device. The data on the bus is the same as the device address. Even if there is no start signal, it will respond and cause SDA to deadlock. Therefore, it is recommended that users give AHT10 an independent I2C bus.

## contact information

Maintenance man:

- Maintenance: [Ernest](https://github.com/ErnestChen1)
- Homepage: https://github.com/RT-Thread-packages/aht10