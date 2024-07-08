#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <icm20608.h>

#define DBG_TAG "app_icm"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_mutex_t thread_mutex;

void app_icm20608_entry(void* argument){
    icm20608_device_t dev = argument;
    rt_int16_t accel_x, accel_y, accel_z;
    rt_int16_t gyros_x, gyros_y, gyros_z;
    rt_err_t result;

    for(;;){
        rt_mutex_take(thread_mutex, RT_WAITING_FOREVER);
        /* 读取三轴加速度 */
        result = icm20608_get_accel(dev, &accel_x, &accel_y, &accel_z);
        if (result == RT_EOK)
        {
            LOG_D("current accelerometer: accel_x%6d, accel_y%6d, accel_z%6d", accel_x, accel_y, accel_z);
        }
        else
        {
            LOG_E("The sensor does not work");
            break;
        }

        /* 读取三轴陀螺仪 */
        result = icm20608_get_gyro(dev, &gyros_x, &gyros_y, &gyros_z);
        if (result == RT_EOK)
        {
            LOG_D("current gyroscope    : gyros_x%6d, gyros_y%6d, gyros_z%6d", gyros_x, gyros_y, gyros_z);
        }
        else
        {
            LOG_E("The sensor does not work");
            break;
        }

        rt_mutex_release(thread_mutex);

        rt_thread_mdelay(1000);
    }
}

int app_icm20608_init(void){
    icm20608_device_t dev = rt_malloc(sizeof(struct icm20608_device));
    const char* i2c_bus_name = "i2c2";
    rt_err_t result;
    rt_thread_t tid;

    dev = icm20608_init(i2c_bus_name);

    /* 对 icm20608 进行零值校准：采样 10 次，求取平均值作为零值*/
    result = icm20608_calib_level(dev, 10);
    if (result == RT_EOK)
    {
        LOG_D("The sensor calibrates success");
        LOG_D("accel_offset: X%6d  Y%6d  Z%6d", dev->accel_offset.x, dev->accel_offset.y, dev->accel_offset.z);
        LOG_D("gyro_offset : X%6d  Y%6d  Z%6d", dev->gyro_offset.x, dev->gyro_offset.y, dev->gyro_offset.z);
    }
    else
    {
        LOG_E("The sensor calibrates failure");
        icm20608_deinit(dev);

        return 0;
    }

    thread_mutex = rt_mutex_create("thread_mutex", RT_IPC_FLAG_FIFO);

    tid = rt_thread_create("icm20608", app_icm20608_entry, dev, 1024, 22, 10);
    if(tid != RT_NULL) rt_thread_startup(tid);

    return 0;
}

void app_icm_start(){
    rt_mutex_release(thread_mutex);
}

void app_icm_stop(){
    rt_mutex_take(thread_mutex, RT_WAITING_FOREVER);
}

INIT_DEVICE_EXPORT(app_icm20608_init);

MSH_CMD_EXPORT(app_icm_start, icm start);
MSH_CMD_EXPORT(app_icm_stop, icm stop);
