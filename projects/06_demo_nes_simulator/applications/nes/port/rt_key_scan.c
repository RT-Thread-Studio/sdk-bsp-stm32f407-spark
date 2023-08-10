#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <rt_key_scan.h>


/* 配置 KEY 输入引脚  */
#define PIN_KEY0        GET_PIN(C, 0)     // PC0:  KEY0         --> KEY
#define PIN_KEY1        GET_PIN(C, 1)      // PC1 :  KEY1         --> KEY
#define PIN_KEY2        GET_PIN(C, 4)      // PC4 :  KEY2         --> KEY
#define PIN_WK_UP       GET_PIN(C, 5)     // PC5:  WK_UP        --> KEY

rt_event_t key_event;

static void key_scan()
{
    uint32_t key_data;
    while(1)
    {
        rt_thread_mdelay(15); //10ms tick wait for input change.
        key_data = 0;
        if (rt_pin_read(PIN_KEY0) == PIN_LOW)
        {
#if     DEBUG_PRINTF
            rt_kprintf("KEY0 pressed!\n");
#endif  //debug
            key_data |= TP_LEFT;
        }
        if (rt_pin_read(PIN_KEY1) == PIN_LOW)
        {
#if     DEBUG_PRINTF
            rt_kprintf("KEY1 pressed!\n");
#endif  //debug
            key_data |= TP_START;
        }
        if (rt_pin_read(PIN_KEY2) == PIN_LOW)
        {
#if     DEBUG_PRINTF
            rt_kprintf("KEY2 pressed!\n");
#endif  //debug
            key_data |= TP_RIGHT;
        }
        if (rt_pin_read(PIN_WK_UP) == PIN_LOW)
        {
#if     DEBUG_PRINTF
            rt_kprintf("WK_UP pressed!\n");
#endif  //debug
            key_data |= TP_A;
        }
        if(key_data)
        {
            rt_event_send(key_event,key_data);
        }
    }
    
}
void key_scan_init()
{

    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_WK_UP, PIN_MODE_INPUT_PULLUP);

    key_event = rt_event_create("key", RT_IPC_FLAG_FIFO);

    rt_thread_t thread = rt_thread_create("key scan",key_scan,RT_NULL,1024,20,1);
    if(thread == RT_NULL) rt_kprintf("key scan thread create failed!\n");
    rt_thread_startup(thread);
}