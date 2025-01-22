# OneNET

[中文页](README_ZH.md) | English

## 1. Introduction

[OneNET](https://open.iot.10086.cn/) The platform is an ecological platform built by China Mobile based on the Internet of Things industry. It has high concurrency, multi-protocol access, rich API support, secure data storage, and fast application Incubation and other features. At the same time, OneNET platform also provides all-round support to accelerate the development of user products.

The OneNET software package is an adaptation of RT-Thread to the OneNET platform connection. Through this software package, devices can be easily connected to the OneNet platform on RT-Thread to complete data transmission, reception, device registration and control, etc. Features.

The software package has the following advantages:

- Reconnect after disconnection
- Automatic registration
- Custom response function
- Customize the callback function corresponding to topic and topic
- Upload binary data

For more introduction, please see [Detailed introduction](./docs/introduction.md)

### 1.1 Directory structure

```c
OneNET
│ README.md // Software package instructions
│ SConscript // RT-Thread default build script
├───docs
│ └───figures // Documents use pictures
│ │ api.md // API instructions
│ │ introduction.md // Detailed introduction of the software package
│ │ principle.md // Implementation principle
│ │ README.md // Document structure description
│ │ samples.md // package sample
│ │ user-guide.md // Instructions
│ │ port.md // Porting instruction document
│ └───version.md // version
├───ports // transplant files
│ └───onenet_port.c // Porting file template
├───samples // sample code
│ └───onenet_sample.c // Software package application sample code
├───inc // header file
└───src // source file
```

### 1.2 License

OneNET package complies with GUN GPL license, please refer to `LICENSE` file for details.

### 1.3 Dependency

- RT_Thread 3.0+
- [paho-mqtt](https://github.com/RT-Thread-packages/paho-mqtt.git)
- [webclient](https://github.com/RT-Thread-packages/webclient.git)
- [cJSON](https://github.com/RT-Thread-packages/cJSON.git)

## 2. How to obtain

To use `OneNET package`, you need to select it in the package management of RT-Thread. The specific path is as follows:

```c
RT-Thread online packages
    IoT-internet of things --->
        IoT Cloud --->
            [*] OneNET: China Mobile OneNet cloud SDK for RT-Thread
```

Enter the configuration menu of the onenet software package to configure as shown in the figure below, and fill in the information according to the **actual situation** of your product and equipment

```c
--- OneNET: China Mobile OneNet cloud SDK for RT-Thread
    [] Enable OneNET sample
    [*] Enable support MQTT protocol
    [] Enable OneNET automatic register device (NEW)
    (35936966) device id
    (201807171718) auth info
    (H3ak5Bbl0NxpW3QVVe33InnPxOg=) api key
    (156418) product id
    (dVZ=ZjVJvGjXIUDsbropzg1a8Dw=) master/product apikey (NEW)
        version (latest) --->
```

**Enable OneNET sample**: Open OneNET sample code

**Enable support MQTT protocol**: Enable MQTT protocol to connect to OneNET support

**Enable OneNET automatic register device**: Enable OneNET automatic register device function

**device id**: Configure the `device ID` obtained when creating a device in the cloud

**auth info**: Configure `user-defined authentication information` when creating products in the cloud (unique per device for each product)

**api key**: Configure the `APIkey` obtained when creating a device in the cloud

**product id**: Configure the `product ID` obtained when creating a product in the cloud

**master/product apikey**: Configure the `product APIKey` obtained when creating a product in the cloud

After the configuration is complete, let the RT-Thread package manager automatically update, or use the pkgs --update command to update the package to the BSP.

## 3. Use OneNET software package

- For detailed sample introduction, please refer to [Sample Document](docs/samples.md).
 
- How to use it from scratch, please refer to [User Manual](docs/user-guide.md).
 
- For complete API documentation, please refer to [API Manual](docs/api.md).
 
- For the working principle of OneNET software package, please refer to [Working Principle](docs/principle.md).

- OneNET software package migration, please refer to [Porting Manual](docs/port.md).

- More **detailed introduction documents** are located in the [`/docs`](/docs) folder, **Please check before using the package for development**.

## 4. Matters needing attention

- The auto-registration function is not enabled, and the information such as `device id`, `api key`, `product id`, and `auth info` configured in the menuconfig option needs to be consistent with the information obtained when creating a new product and new device in the OneNET cloud.
- After enabling the automatic registration function, you need to read the migration manual and complete the migration work.
- The device must be **connected successfully** before initializing the OneNET package.

## 5. Contact & Thanks

- Maintenance: RT-Thread development team
- Homepage: https://github.com/RT-Thread-packages/onenet
