# MQTT 协议通信例程

## 简介

本例程基于 Paho-MQTT 软件包，展示了向服务器订阅主题和向指定主题发布消息的功能。

## 硬件说明

本例程需要依赖 星火 1 号 板卡上的 WiFi 模块完成网络通信，因此请确保硬件平台上的 WiFi 模组可以正常工作。

## 软件说明

本例程的源码位于 `/projects/05_iot_mqtt`。

### MQTT

MQTT（Message Queuing Telemetry Transport，消息队列遥测传输协议），是一种基于发布 / 订阅（publish/subscribe）模式的 “轻量级” 通讯协议，该协议构建于 TCP/IP 协议上，由 IBM 在 1999 年发布。MQTT 最大优点在于，可以以极少的代码和有限的带宽，为连接远程设备提供实时可靠的消息服务。作为一种低开销、低带宽占用的即时通讯协议，使其在物联网、小型设备、移动应用等方面有较广泛的应用。

MQTT 是一个基于客户端 - 服务器的消息发布 / 订阅传输协议。 MQTT 协议是轻量、简单、开放和易于实现的，这些特点使它适用范围非常广泛。在很多情况下，包括受限的环境中，如：机器与机器（M2M）通信和物联网（IoT）。其在，通过卫星链路通信传感器、偶尔拨号的医疗设备、智能家居、及一些小型化设备中已广泛使用。

### Paho MQTT 包

[Paho MQTT](https://www.eclipse.org/paho/index.php?page=downloads.php) 是 Eclipse 实现的基于 MQTT 协议的客户端，本软件包是在 Eclipse [paho-mqtt](https://github.com/eclipse/paho.mqtt.embedded-c) 源码包的基础上设计的一套 MQTT 客户端程序。

RT-Thread MQTT 客户端功能特点如下：

- 断线自动重连

    RT-Thread MQTT 软件包实现了断线重连机制，在断网或网络不稳定导致连接断开时，会维护登陆状态，重新连接，并自动重新订阅 Topic。提高连接的可靠性，增加了软件包的易用性。

- pipe 模型，非阻塞 API

    降低编程难度，提高代码运行效率，适用于高并发数据量小的情况。

- 事件回调机制

    在建立连接、收到消息或者断开连接等事件时，可以执行自定义的回调函数。

- TLS 加密传输

    MQTT 可以采用 TLS 加密方式传输，保证数据的安全性和完整性。

### 例程使用说明

本示例的源代码位于 main.c 中。

MQTT 软件包已经实现了 MQTT 客户端的完整功能，开发者只需要设定好 MQTT 客户端的配置即可使用。本例程使用的测试服务器是 Eclipse 的测试服务器，服务器网址、用户名和密码已在 main 文件的开头定义。

在 main 函数中，首先将 MQTT 客户端启动函数（mq_start()）注册为网络连接成功的回调函数，然后执行 WiFi 自动连接初始化。当开发板连接上 WiFi 后， MQTT 客户端函数（mq_start()）会被自动调用。 mq_start() 函数主要是配置 MQTT 客户端的连接参数（客户端 ID、保持连接时间、用户名和密码等），设置事件回调函数（连接成功、在线和离线回调函数），设置订阅的主题，并为每个主题设置不同的回调函数去处理发生的事件。设置完成后，函数会启动一个 MQTT 客户端。客户端会自动连接服务器，并订阅相应的主题。

mq_publish() 函数用来向指定的主题发布消息。例程里的主题就是我们 MQTT 客户端启动时订阅的主题，这样，我们会接收到自己发布的消息，实现自发自收的功能。本例程在在线回调函数里调用了 mq_publish() 函数，发布了 Hello,RT-Thread! 消息，所以我们在 MQTT 客户端成功连上服务器，处于在线状态后，会收到 Hello,RT-Thread! 的消息。

本例程的部分示例代码如下所示：

```c
/**
 * MQTT URI farmat:
 * domain mode
 * tcp://broker.emqx.io:1883
 *
 * ipv4 mode
 * tcp://192.168.10.1:1883
 * ssl://192.168.10.1:1884
 *
 * ipv6 mode
 * tcp://[fe80::20c:29ff:fe9a:a07e]:1883
 * ssl://[fe80::20c:29ff:fe9a:a07e]:1884
 */
#define MQTT_URI "tcp://broker.emqx.io:1883"
#define MQTT_SUBTOPIC "/mqtt/test/"
#define MQTT_PUBTOPIC "/mqtt/test/"

/* define MQTT client context */
static MQTTClient client;
static void mq_start(void);
static void mq_publish(const char *send_str);

char sup_pub_topic[48] = {0};

/* 连接成功的回调函数 */
static void wlan_connect_handler(int event, struct rt_wlan_buff *buff, void *parameter)
{
    rt_kprintf("%s\n", __FUNCTION__);
    if ((buff != RT_NULL) && (buff->len == sizeof(struct rt_wlan_info)))
    {
        rt_kprintf("ssid : %s \n", ((struct rt_wlan_info *)buff->data)->ssid.val);
    }
}

/* 连接失败的回调函数 */
static void wlan_connect_fail_handler(int event, struct rt_wlan_buff *buff, void *parameter)
{
    rt_kprintf("%s\n", __FUNCTION__);
    if ((buff != RT_NULL) && (buff->len == sizeof(struct rt_wlan_info)))
    {
        rt_kprintf("ssid : %s \n", ((struct rt_wlan_info *)buff->data)->ssid.val);
    }
}

int main(void)
{
    /* 注册 wlan 回调函数 */
    rt_wlan_register_event_handler(RT_WLAN_EVT_READY, (void (*)(int, struct rt_wlan_buff *, void *))mq_start, RT_NULL);
    /* 初始化自动连接功能 */
    rt_wlan_set_mode(RT_WLAN_DEVICE_STA_NAME, RT_WLAN_STATION);
    rt_wlan_config_autoreconnect(RT_TRUE);
    /*  */
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_CONNECTED, wlan_connect_handler, RT_NULL);
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_CONNECTED_FAIL, wlan_connect_fail_handler, RT_NULL);
}

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("Topic: %.*s receive a message: %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);

    return;
}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub default callback: %.*s %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);
    return;
}

static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_I("Start to connect mqtt server");
}

static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("Connect mqtt server success");
    LOG_D("Publish message: Hello,RT-Thread! to topic: %s", sup_pub_topic);
    mq_publish("Hello,RT-Thread!");
}

static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_I("Disconnect from mqtt server");
}

/* 创建与配置 mqtt 客户端 */
static void mq_start(void)
{
    /* 初始 condata 参数 */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = {0};

    static int is_started = 0;
    if (is_started)
    {
        return;
    }
    /* 配置 MQTT 文本参数 */
    {
        client.isconnected = 0;
        client.uri = MQTT_URI;

        /* 生成随机客户端 ID */
        rt_snprintf(cid, sizeof(cid), "rtthread%d", rt_tick_get());
        rt_snprintf(sup_pub_topic, sizeof(sup_pub_topic), "%s%s", MQTT_PUBTOPIC, cid);
        /* 配置连接参数 */
        memcpy(&client.condata, &condata, sizeof(condata));
        client.condata.clientID.cstring = cid;
        client.condata.keepAliveInterval = 60;
        client.condata.cleansession = 1;
        client.condata.username.cstring = "";
        client.condata.password.cstring = "";

        /* 配置 mqtt 参数 */
        client.condata.willFlag = 0;
        client.condata.will.qos = 1;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = sup_pub_topic;

        client.buf_size = client.readbuf_size = 1024;
        client.buf = malloc(client.buf_size);
        client.readbuf = malloc(client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            LOG_E("no memory for MQTT client buffer!");
            goto _exit;
        }

        /* 设置事件回调 */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;
        /* 设置要订阅的 topic 和 topic 对应的回调函数 */
        client.messageHandlers[0].topicFilter = sup_pub_topic;
        client.messageHandlers[0].callback = mqtt_sub_callback;
        client.messageHandlers[0].qos = QOS1;

        /* 设置默认订阅回调函数 */
        client.defaultMessageHandler = mqtt_sub_default_callback;
    }

    /* 启动 MQTT 客户端 */
    LOG_D("Start mqtt client and subscribe topic:%s", sup_pub_topic);
    paho_mqtt_start(&client);
    is_started = 1;

_exit:
    return;
}

/* MQTT 消息发布函数 */
static void mq_publish(const char *send_str)
{
    MQTTMessage message;
    const char *msg_str = send_str;
    const char *topic = sup_pub_topic;
    message.qos = QOS1;
    message.retained = 0;
    message.payload = (void *)msg_str;
    message.payloadlen = strlen(message.payload);

    MQTTPublish(&client, topic, &message);

    return;
}


static void msh_mq_publish(int argc, char *argv[])
{
    char send_buff[100] = {'\0'};
    for(int i=1;i<argc;i++)
    {
        if(i> 1)
        {
            strcat(send_buff," ");
        }
        strcat(send_buff,argv[i]);
    }
    mq_publish(send_buff);  // 发布消息给主题 "test"
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(msh_mq_publish, publish messege by msh);
```

## 运行

### 编译 & 下载

- RT-Thread Studio：在 RT-Thread Studio 的包管理器中下载 `STM32F407-RT-SPARK` 资源包，然后创建新工程，执行编译。
- MDK：首先双击 mklinks.bat，生成 rt-thread 与 libraries 文件夹链接；再使用 Env 生成 MDK5 工程；最后双击 project.uvprojx 打开 MDK5 工程，执行编译。

编译完成后，将开发板的 ST-Link USB 口与 PC 机连接，然后将固件下载至开发板。

### 运行效果

按下复位按键重启开发板，开发板会自动连上 WiFi（首次使用需要输入 wifi join wifi 名称 wifi 密码进行连接），可以看到板子会打印出如下信息：

```shell
 \ | /
- RT -     Thread Operating System
 / | \     4.1.1 build Jun 13 2023 16:40:30
 2006 - 2022 Copyright by RT-Thread team
lwIP-2.0.3 initialized!
[I/sal.skt] Socket Abstraction Layer initialize success.
msh />[E/[RW007]] The wifi Stage 1 status 67452301 efcdab89 1 1
[I/WLAN.dev] wlan init success
[I/WLAN.lwip] eth device init ok name:w0
[I/WLAN.dev] wlan init success
[I/WLAN.lwip] eth device init ok name:w1

rw007  sn: [rw007c745bb22fc584aa6cecc]
rw007 ver: [RW007_2.1.0-a7a0d089-57]

wifi join test_ssid 12345678
[I/WLAN.mgnt] wifi connect success ssid:test_ssid
wlan_connect_handler
ssid : test_ssid
msh />[I/WLAN.lwip] Got IP address : 192.168.137.187
[D/main] Start mqtt client and subscribe topic:/mqtt/test/rtthread47037
[I/main] Start to connect mqtt server
[D/mqtt] ipv4 address port: 1883
[D/mqtt] HOST = 'broker.emqx.io'
[I/mqtt] MQTT server connect success.
[I/mqtt] Subscribe #0 /mqtt/test/rtthread47037 OK!
[D/main] Connect mqtt server success
[D/main] Publish message: Hello,RT-Thread! to topic: /mqtt/test/rtthread47037
[D/main] Topic: /mqtt/test/rtthread47037 receive a message: Hello,RT-Thread!

```

我们可以看到， WiFi 连接成功后， MQTT 客户端就自动连接了服务器，并订阅了我们指定的主题。连接服务器成功，处于在线状态后，发布了一条 Hello,RT-Thread! 的消息，我们很快接收到了服务器推送过来的这条消息。

## 注意事项

使用本例程前需要先连接 WiFi。

## 引用参考

- mqtt 软件包：[https://github.com/RT-Thread-packages/paho-mqtt](https://github.com/RT-Thread-packages/paho-mqtt)
- 文档中心：[RT-Thread 文档中心](https://www.rt-thread.org/document/site/#/)
