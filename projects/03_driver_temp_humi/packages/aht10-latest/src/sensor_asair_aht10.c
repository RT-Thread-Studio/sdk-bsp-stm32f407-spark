/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-08     yangjie      the first version
 */

#include "sensor_asair_aht10.h"

#define DBG_TAG "sensor.asair.aht10"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static rt_ssize_t _aht10_polling_get_data(rt_sensor_t sensor, rt_sensor_data_t data)
{
    struct aht10_device *aht10_dev = (struct aht10_device *)sensor->parent.user_data;

    if (sensor->info.type == RT_SENSOR_TYPE_TEMP)
    {
        data->data.temp = aht10_read_temperature(aht10_dev);
        data->timestamp = rt_sensor_get_ts();
        return 1;
    }
    else if (sensor->info.type == RT_SENSOR_TYPE_HUMI)
    {
        data->data.humi = aht10_read_humidity(aht10_dev);
        data->timestamp = rt_sensor_get_ts();
        return 1;
    }
    else
    {
        return -RT_EINVAL;
    }
}

static rt_ssize_t aht10_fetch_data(rt_sensor_t sensor, rt_sensor_data_t buf, rt_size_t len)
{
    RT_ASSERT(buf);

    if (RT_SENSOR_MODE_GET_FETCH(sensor->info.mode) == RT_SENSOR_MODE_FETCH_POLLING)
    {
        return _aht10_polling_get_data(sensor, buf);
    }
    else
    {
        return -RT_EINVAL;
    }
}

static rt_err_t aht10_control(rt_sensor_t sensor, int cmd, void *args)
{
    rt_err_t result = -RT_EINVAL;
    return result;
}

static struct rt_sensor_ops sensor_ops =
{
    aht10_fetch_data,
    aht10_control
};

static const char *sensor_name = "aht10";

int rt_hw_aht10_init(const char *name, struct rt_sensor_config *cfg)
{
    rt_int8_t result;
    rt_sensor_t sensor_temp = RT_NULL, sensor_humi = RT_NULL;
    struct aht10_device *aht10_dev = aht10_init(cfg->intf.dev_name);
    if (aht10_dev == RT_NULL)
    {
        LOG_E("aht10 init failure!");
        return -1;
    }

     /* temperature sensor register */
    sensor_temp = rt_calloc(1, sizeof(struct rt_sensor_device));
    if (sensor_temp == RT_NULL)
        return -1;

    sensor_temp->info.type       = RT_SENSOR_TYPE_TEMP;
    sensor_temp->info.vendor     = RT_SENSOR_VENDOR_ASAIR;
    sensor_temp->info.name       = sensor_name;
    sensor_temp->info.unit       = RT_SENSOR_UNIT_CELSIUS;
    sensor_temp->info.intf_type  = RT_SENSOR_INTF_I2C;

    sensor_temp->info.acquire_min = 1000;
    sensor_temp->info.accuracy.resolution = 0.01;
    sensor_temp->info.accuracy.error = 0.3;
    sensor_temp->info.scale.range_min = -40.0;
    sensor_temp->info.scale.range_max = 85.0;

    rt_memcpy(&sensor_temp->config, cfg, sizeof(struct rt_sensor_config));
    sensor_temp->ops = &sensor_ops;

    result = rt_hw_sensor_register(sensor_temp, name, RT_DEVICE_FLAG_RDONLY, aht10_dev);
    if (result != RT_EOK)
    {
        LOG_E("device register err code: %d", result);
        goto __exit;
    }

    /* humidity sensor register */
    sensor_humi = rt_calloc(1, sizeof(struct rt_sensor_device));
    if (sensor_humi == RT_NULL)
    {
        return -1;
    }

    sensor_humi->info.type       = RT_SENSOR_TYPE_HUMI;
    sensor_humi->info.vendor     = RT_SENSOR_VENDOR_ASAIR;
    sensor_humi->info.name       = sensor_name;
    sensor_humi->info.unit       = RT_SENSOR_UNIT_PERCENTAGE;
    sensor_humi->info.intf_type  = RT_SENSOR_INTF_I2C;

    sensor_humi->info.acquire_min = 1000;
    sensor_humi->info.accuracy.resolution = 0.024;
    sensor_humi->info.accuracy.error = 2.0;
    sensor_humi->info.scale.range_min = 0.0;
    sensor_humi->info.scale.range_max = 100.0;

    rt_memcpy(&sensor_humi->config, cfg, sizeof(struct rt_sensor_config));
    sensor_humi->ops = &sensor_ops;

    result = rt_hw_sensor_register(sensor_humi, name, RT_DEVICE_FLAG_RDONLY, aht10_dev);
    if (result != RT_EOK)
    {
        LOG_E("device register err code: %d", result);
        goto __exit;
    }

    return RT_EOK;

__exit:
    if (sensor_temp)
        rt_free(sensor_temp);
    if (sensor_humi)
        rt_free(sensor_humi);
    if (aht10_dev)
        aht10_deinit(aht10_dev);
    return -RT_ERROR;
}
