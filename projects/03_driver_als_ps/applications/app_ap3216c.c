#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <ap3216c.h>

#define DBG_TAG "app_ap3216c_entry"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

ap3216c_device_t dev;

void app_ap3216c_entry(void* argument){
    rt_uint16_t ps_data;
    float brightness;

    for(;;)
    {
        ps_data = ap3216c_read_ps_data(dev);

        LOG_D("current ps data: %d.", ps_data);

        brightness = ap3216c_read_ambient_light(dev);
        LOG_D("current brightness: %d.%d(lux).", (int)brightness, ((int)(10 * brightness) % 10));

        rt_thread_delay(1000);
    }
}

int app_ap3216c_init(void){
    const char* i2c_bus_name = "i2c2";
    rt_thread_t tid;

    dev = ap3216c_init(i2c_bus_name);

    tid = rt_thread_create("AP3216C", app_ap3216c_entry, NULL, 1024, 20, 1);
    if(tid != RT_NULL) rt_thread_startup(tid);

    return 0;
}

INIT_DEVICE_EXPORT(app_ap3216c_init);
