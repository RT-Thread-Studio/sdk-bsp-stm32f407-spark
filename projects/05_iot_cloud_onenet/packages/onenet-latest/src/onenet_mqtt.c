/*
 * File      : onenet_mqtt.c
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-04-24     chenyong     first version
 */
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include <cJSON_util.h>

#include <paho_mqtt.h>

#include <onenet.h>

#define DBG_ENABLE
#define DBG_COLOR
#define DBG_SECTION_NAME    "onenet.mqtt"
#if ONENET_DEBUG
#define DBG_LEVEL           DBG_LOG
#else
#define DBG_LEVEL           DBG_INFO
#endif /* ONENET_DEBUG */

#include <rtdbg.h>

#if RTTHREAD_VERSION < 40100
#ifdef RT_USING_DFS
#include <dfs_posix.h>
#endif
#else
#ifdef RT_USING_DFS
#include <dfs_file.h>
#include <unistd.h>
#include <stdio.h>      /* rename() */
#include <sys/stat.h>
#include <sys/statfs.h> /* statfs() */
#endif
#endif

#define ONENET_TOPIC_DP "$sys/" ONENET_INFO_PROID "/" ONENET_INFO_DEVID "/thing/property/post"

static rt_bool_t init_ok = RT_FALSE;
static MQTTClient mq_client;
struct rt_onenet_info onenet_info;

struct onenet_device
{
    struct rt_onenet_info *onenet_info;

    void(*cmd_rsp_cb)(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size);

} onenet_mqtt;

static void mqtt_callback(MQTTClient *c, MessageData *msg_data)
{
    size_t res_len = 0;
    uint8_t *response_buf = RT_NULL;
    char topicname[45] = { "$crsp/" };

    RT_ASSERT(c);
    RT_ASSERT(msg_data);

    LOG_D("topic %.*s receive a message", msg_data->topicName->lenstring.len, msg_data->topicName->lenstring.data);

    LOG_D("message length is %d", msg_data->message->payloadlen);

    if (onenet_mqtt.cmd_rsp_cb != RT_NULL)
    {
        onenet_mqtt.cmd_rsp_cb((uint8_t *) msg_data->message->payload, msg_data->message->payloadlen, &response_buf,
                &res_len);

        if (response_buf != RT_NULL || res_len != 0)
        {
            strncat(topicname, &(msg_data->topicName->lenstring.data[6]), msg_data->topicName->lenstring.len - 6);

            onenet_mqtt_publish(topicname, response_buf, strlen((const char *)response_buf));

            ONENET_FREE(response_buf);

        }

    }

}

static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_D("Enter mqtt_connect_callback!");
}

static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("Enter mqtt_online_callback!");
}

static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_D("Enter mqtt_offline_callback!");
}

static void mqtt_usr_callback(MQTTClient *c)
{
    LOG_D("Enter mqtt_usr_callback!");
}

static rt_err_t onenet_mqtt_entry(void)
{
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;

    mq_client.uri = onenet_info.server_uri;
    memcpy(&(mq_client.condata), &condata, sizeof(condata));
    mq_client.condata.clientID.cstring = onenet_info.device_id;
    mq_client.condata.keepAliveInterval = 30;
    mq_client.condata.cleansession = 1;
    mq_client.condata.username.cstring = onenet_info.pro_id;
    mq_client.condata.password.cstring = onenet_info.auth_info;

    mq_client.buf_size = mq_client.readbuf_size = 1024 * 2;
    mq_client.buf = (unsigned char *) ONENET_CALLOC(1, mq_client.buf_size);
    mq_client.readbuf = (unsigned char *) ONENET_CALLOC(1, mq_client.readbuf_size);
    if (!(mq_client.buf && mq_client.readbuf))
    {
        LOG_E("No memory for MQTT client buffer!");
        return -RT_ENOMEM;
    }

    /* registered callback */
    mq_client.connect_callback = mqtt_connect_callback;
    mq_client.online_callback = mqtt_online_callback;
    mq_client.offline_callback = mqtt_offline_callback;

    mq_client.defaultMessageHandler = mqtt_callback;

    mq_client.messageHandlers[0].topicFilter = rt_strdup(ONENET_MQTT_SUBTOPIC);
    mq_client.messageHandlers[0].callback = mqtt_usr_callback;
    mq_client.messageHandlers[0].qos = QOS1;

    paho_mqtt_start(&mq_client);

    return RT_EOK;
}

static rt_err_t onenet_get_info(void)
{
    char dev_id[ONENET_INFO_DEVID_LEN] = { 0 };
    char api_key[ONENET_INFO_APIKEY_LEN] = { 0 };
    char auth_info[ONENET_INFO_AUTH_LEN] = { 0 };

#ifdef ONENET_USING_AUTO_REGISTER
    char name[ONENET_INFO_NAME_LEN] = { 0 };

    if (!onenet_port_is_registed())
    {
        if (onenet_port_get_register_info(name, auth_info) < 0)
        {
            LOG_E("onenet get register info fail!");
            return -RT_ERROR;
        }

        if (onenet_http_register_device(name, auth_info) < 0)
        {
            LOG_E("onenet register device fail! name is %s,auth info is %s", name, auth_info);
            return -RT_ERROR;
        }
    }

    if (onenet_port_get_device_info(dev_id, api_key, auth_info))
    {
        LOG_E("onenet get device id fail,dev_id is %s,api_key is %s,auth_info is %s", dev_id, api_key, auth_info);
        return -RT_ERROR;
    }

#else
    strncpy(dev_id, ONENET_INFO_DEVID, strlen(ONENET_INFO_DEVID));
    strncpy(auth_info, ONENET_INFO_AUTH, strlen(ONENET_INFO_AUTH));
#endif

    strncpy(onenet_info.device_id, dev_id, strlen(dev_id));
    strncpy(onenet_info.api_key, api_key, strlen(api_key));
    strncpy(onenet_info.pro_id, ONENET_INFO_PROID, strlen(ONENET_INFO_PROID));
    strncpy(onenet_info.auth_info, auth_info, strlen(auth_info));
    strncpy(onenet_info.server_uri, ONENET_SERVER_URL, strlen(ONENET_SERVER_URL));

    return RT_EOK;
}

/**
 * onenet mqtt client init.
 *
 * @param   NULL
 *
 * @return  0 : init success
 *         -1 : get device info fail
 *         -2 : onenet mqtt client init fail
 */
int onenet_mqtt_init(void)
{
    int result = 0;

    if (init_ok)
    {
        LOG_D("onenet mqtt already init!");
        return 0;
    }

    if (onenet_get_info() < 0)
    {
        result = -1;
        goto __exit;
    }

    onenet_mqtt.onenet_info = &onenet_info;
    onenet_mqtt.cmd_rsp_cb = RT_NULL;

    if (onenet_mqtt_entry() < 0)
    {
        result = -2;
        goto __exit;
    }

__exit:
    if (!result)
    {
        LOG_I("RT-Thread OneNET package(V%s) initialize success.", ONENET_SW_VERSION);
        init_ok = RT_TRUE;
    }
    else
    {
        LOG_E("RT-Thread OneNET package(V%s) initialize failed(%d).", ONENET_SW_VERSION, result);
    }

    return result;
}

/**
 * mqtt publish msg to topic
 *
 * @param   topic   target topic
 * @param   msg     message to be sent
 * @param   len     message length
 *
 * @return  0 : publish success
 *         -1 : publish fail
 */
rt_err_t onenet_mqtt_publish(const char *topic, const uint8_t *msg, size_t len)
{
    MQTTMessage message;

    RT_ASSERT(topic);
    RT_ASSERT(msg);

    message.qos = QOS0;
    message.retained = 0;
    message.payload = (void *) msg;
    message.payloadlen = len;

    if (MQTTPublish(&mq_client, topic, &message) < 0)
    {
        return -1;
    }

    return 0;
}

static rt_err_t onenet_mqtt_get_digit_data(const char *ds_name, const double digit, char **out_buff, size_t *length)
{
    rt_err_t result = RT_EOK;
    cJSON *root = RT_NULL;
    cJSON *params = RT_NULL;
    cJSON *temp = RT_NULL;
    char *msg_str = RT_NULL;

    RT_ASSERT(ds_name);
    RT_ASSERT(out_buff);
    RT_ASSERT(length);

    root = cJSON_CreateObject();
    params = cJSON_CreateObject();
    temp = cJSON_CreateObject();

    if (!root)
    {
        LOG_E("MQTT publish digit data failed! cJSON create object error return NULL!");
        return -RT_ENOMEM;
    }

    /*  add a key-value pair with the key "id" and a string value to the root object */
    cJSON_AddItemToObject(root, "id", cJSON_CreateString("123"));

    /*  add the params object to the root object with the key "params" */
    cJSON_AddItemToObject(root, "params", params);

    /*  add the temp object to the params object with the key ds_name */
    cJSON_AddItemToObject(params, ds_name, temp);

    /*  add a key-value pair with the key "value" and a digit value to the temp object */
    cJSON_AddNumberToObject(temp, "value", digit);

    /* render a cJSON structure to buffer */
    msg_str = cJSON_PrintUnformatted(root);
    if (!msg_str)
    {
        LOG_E("MQTT publish digit data failed! cJSON print unformatted error return NULL!");
        result = -RT_ENOMEM;
        goto __exit;
    }

    *out_buff = ONENET_MALLOC(strlen(msg_str));
    if (!(*out_buff))
    {
        LOG_E("ONENET mqtt upload digit data failed! No memory for send buffer!");
        return -RT_ENOMEM;
    }

    strncpy(&(*out_buff)[0], msg_str, strlen(msg_str));
    *length = strlen(msg_str);

__exit:
    if (root)
    {
        cJSON_Delete(root);
    }
    if (msg_str)
    {
        cJSON_free(msg_str);
    }

    return result;
}

/**
 * Upload digit data to OneNET cloud.
 *
 * @param   ds_name     datastream name
 * @param   digit       digit data
 *
 * @return  0 : upload digit data success
 *         -5 : no memory
 */
rt_err_t onenet_mqtt_upload_digit(const char *ds_name, const double digit)
{
    char *send_buffer = RT_NULL;
    rt_err_t result = RT_EOK;
    size_t length = 0;

    RT_ASSERT(ds_name);

    result = onenet_mqtt_get_digit_data(ds_name, digit, &send_buffer, &length);
    if (result < 0)
    {
        goto __exit;
    }

    result = onenet_mqtt_publish(ONENET_TOPIC_DP, (uint8_t *)send_buffer, length);
    if (result < 0)
    {
        LOG_E("onenet publish failed (%d)!", result);
        goto __exit;
    }

__exit:
    if (send_buffer)
    {
        ONENET_FREE(send_buffer);
    }

    return result;
}

static rt_err_t onenet_mqtt_get_string_data(const char *ds_name, const char *str, char **out_buff, size_t *length)
{
    rt_err_t result = RT_EOK;
    cJSON *root = RT_NULL;
    cJSON *params = RT_NULL;
    cJSON *temp = RT_NULL;
    char *msg_str = RT_NULL;

    RT_ASSERT(ds_name);
    RT_ASSERT(str);
    RT_ASSERT(out_buff);
    RT_ASSERT(length);

    root = cJSON_CreateObject();
    params = cJSON_CreateObject();
    temp = cJSON_CreateObject();

    if (!root)
    {
        LOG_E("MQTT publish string data failed! cJSON create object error return NULL!");
        return -RT_ENOMEM;
    }

    /*  add a key-value pair with the key "id" and a string value to the root object */
    cJSON_AddItemToObject(root, "id", cJSON_CreateString("123"));

    /*  add the params object to the root object with the key "params" */
    cJSON_AddItemToObject(root, "params", params);

    /*  add the temp object to the params object with the key ds_name */
    cJSON_AddItemToObject(params, ds_name, temp);

    /*  add a key-value pair with the key "value" and a digit value to the temp object */
    cJSON_AddStringToObject(temp, "value", str);

    /* render a cJSON structure to buffer */
    msg_str = cJSON_PrintUnformatted(root);
    if (!msg_str)
    {
        LOG_E("MQTT publish string data failed! cJSON print unformatted error return NULL!");
        result = -RT_ENOMEM;
        goto __exit;
    }

    *out_buff = ONENET_MALLOC(strlen(msg_str));
    if (!(*out_buff))
    {
        LOG_E("ONENET mqtt upload string data failed! No memory for send buffer!");
        return -RT_ENOMEM;
    }

    strncpy(&(*out_buff)[0], msg_str, strlen(msg_str));
    *length = strlen(msg_str);

__exit:
    if (root)
    {
        cJSON_Delete(root);
    }
    if (msg_str)
    {
        cJSON_free(msg_str);
    }

    return result;
}

/**
 * upload string data to OneNET cloud.
 *
 * @param   ds_name     datastream name
 * @param   str         string data
 *
 * @return  0 : upload digit data success
 *         -5 : no memory
 */
rt_err_t onenet_mqtt_upload_string(const char *ds_name, const char *str)
{
    char *send_buffer = RT_NULL;
    rt_err_t result = RT_EOK;
    size_t length = 0;

    RT_ASSERT(ds_name);
    RT_ASSERT(str);

    result = onenet_mqtt_get_string_data(ds_name, str, &send_buffer, &length);
    if (result < 0)
    {
        goto __exit;
    }

    result = onenet_mqtt_publish(ONENET_TOPIC_DP, (uint8_t *)send_buffer, length);
    if (result < 0)
    {
        LOG_E("onenet mqtt publish digit data failed!");
        goto __exit;
    }

__exit:
    if (send_buffer)
    {
        ONENET_FREE(send_buffer);
    }

    return result;
}

/**
 * set the command responses call back function
 *
 * @param   cmd_rsp_cb  command responses call back function
 *
 * @return  0 : set success
 *         -1 : function is null
 */
void onenet_set_cmd_rsp_cb(void (*cmd_rsp_cb)(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size))
{

    onenet_mqtt.cmd_rsp_cb = cmd_rsp_cb;

}

static rt_err_t onenet_mqtt_get_bin_data(const char *str, const uint8_t *bin, int binlen, uint8_t **out_buff, size_t *length)
{
    rt_err_t result = RT_EOK;
    cJSON *root = RT_NULL;
    char *msg_str = RT_NULL;

    RT_ASSERT(str);
    RT_ASSERT(bin);
    RT_ASSERT(out_buff);
    RT_ASSERT(length);

    root = cJSON_CreateObject();
    if (!root)
    {
        LOG_E("MQTT online push failed! cJSON create object error return NULL!");
        return -RT_ENOMEM;
    }

    cJSON_AddStringToObject(root, "ds_id", str);

    /* render a cJSON structure to buffer */
    msg_str = cJSON_PrintUnformatted(root);
    if (!msg_str)
    {
        LOG_E("Device online push failed! cJSON print unformatted error return NULL!");
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* size = header(3) + json + binary length(4) + binary length +'\0' */
    *out_buff = (uint8_t *) ONENET_MALLOC(strlen(msg_str) + 3 + 4 + binlen + 1);

    strncpy((char *)&(*out_buff)[3], msg_str, strlen(msg_str));
    *length = strlen((const char *)&(*out_buff)[3]);

    /* mqtt head and cjson length */
    (*out_buff)[0] = 0x02;
    (*out_buff)[1] = (*length & 0xff00) >> 8;
    (*out_buff)[2] = *length & 0xff;
    *length += 3;

    /* binary data length */
    (*out_buff)[(*length)++] = (binlen & 0xff000000) >> 24;
    (*out_buff)[(*length)++] = (binlen & 0x00ff0000) >> 16;
    (*out_buff)[(*length)++] = (binlen & 0x0000ff00) >> 8;
    (*out_buff)[(*length)++] = (binlen & 0x000000ff);

    memcpy(&((*out_buff)[*length]), bin, binlen);
    *length = *length + binlen;

__exit:
    if (root)
    {
        cJSON_Delete(root);
    }
    if (msg_str)
    {
        cJSON_free(msg_str);
    }

    return result;
}

/**
 * upload binary data to onenet cloud by path
 *
 * @param   ds_name     datastream name
 * @param   bin         binary file
 * @param   len         binary file length
 *
 * @return  0 : upload success
 *         -1 : invalid argument or open file fail
 */
rt_err_t onenet_mqtt_upload_bin(const char *ds_name, uint8_t *bin, size_t len)
{
    size_t length = 0;
    rt_err_t result = RT_EOK;
    uint8_t *send_buffer = RT_NULL;

    RT_ASSERT(ds_name);
    RT_ASSERT(bin);

    result = onenet_mqtt_get_bin_data(ds_name, bin, len, &send_buffer, &length);
    if (result < 0)
    {
        result = -RT_ERROR;
        goto __exit;
    }

    result = onenet_mqtt_publish(ONENET_TOPIC_DP, send_buffer, length);
    if (result < 0)
    {
        LOG_E("onenet publish data failed(%d)!", result);
        result = -RT_ERROR;
        goto __exit;
    }

__exit:
    if (send_buffer)
    {
        ONENET_FREE(send_buffer);
    }

    return result;
}

#ifdef RT_USING_DFS
/**
 * upload binary data to onenet cloud by path
 *
 * @param   ds_name     datastream name
 * @param   bin_path    binary file path
 *
 * @return  0 : upload success
 *         -1 : invalid argument or open file fail
 */
rt_err_t onenet_mqtt_upload_bin_by_path(const char *ds_name, const char *bin_path)
{
    int fd;
    size_t length = 0, bin_size = 0;
    size_t bin_len = 0;
    struct stat file_stat;
    rt_err_t result = RT_EOK;
    uint8_t *send_buffer = RT_NULL;
    uint8_t * bin_array = RT_NULL;

    RT_ASSERT(ds_name);
    RT_ASSERT(bin_path);

    if (stat(bin_path, &file_stat) < 0)
    {
        LOG_E("get file state fail!, bin path is %s",bin_path);
        return -RT_ERROR;
    }
    else
    {
        bin_len = file_stat.st_size;
        if (bin_len > 3 * 1024 * 1024)
        {
            LOG_E("bin length must be less than 3M, %s length is %d", bin_path, bin_len);
            return -RT_ERROR;
        }

    }

    fd = open(bin_path, O_RDONLY);
    if (fd >= 0)
    {
        bin_array = (uint8_t *) ONENET_MALLOC(bin_len);

        bin_size = read(fd, bin_array, file_stat.st_size);
        close(fd);
        if (bin_size <= 0)
        {
            LOG_E("read %s file fail!", bin_path);
            result = -RT_ERROR;
            goto __exit;
        }
    }
    else
    {
        LOG_E("open %s file fail!", bin_path);
        return -RT_ERROR;
    }

    result = onenet_mqtt_get_bin_data(ds_name, bin_array, bin_size, &send_buffer, &length);
    if (result < 0)
    {
        result = -RT_ERROR;
        goto __exit;
    }

    result = onenet_mqtt_publish(ONENET_TOPIC_DP, send_buffer, length);
    if (result < 0)
    {
        LOG_E("onenet publish %s data failed(%d)!", bin_path, result);
        result = -RT_ERROR;
        goto __exit;
    }

__exit:
    if (send_buffer)
    {
        ONENET_FREE(send_buffer);
    }
    if (bin_array)
    {
        ONENET_FREE(bin_array);
    }

    return result;
}
#endif /* RT_USING_DFS */

#ifdef FINSH_USING_MSH
#include <finsh.h>

MSH_CMD_EXPORT(onenet_mqtt_init, OneNET cloud mqtt initializate);

#endif
