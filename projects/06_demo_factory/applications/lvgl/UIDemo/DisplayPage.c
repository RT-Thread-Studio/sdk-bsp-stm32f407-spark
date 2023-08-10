#include "DisplayPrivate.h"

/*实例化页面调度器*/
PageManager_t page;

/*页面导入*/
#define PAGE_IMPORT(name)\
do{\
    extern void PageRegister_##name(PageManager_t* page_p, uint8_t pageID);\
    PageRegister_##name(&page, PAGE_##name);\
}while(0)

static void page_gestute_event_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event.code == LV_EVENT_GESTURE)
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        PM_EventTransmit(&page, obj, dir);
    }
}

#define PIN_KEY_LEFT        GET_PIN(C, 0)     // PC0:  KEY0         --> KEY
#define PIN_KEY_DOWN        GET_PIN(C, 1)      // PC1 :  KEY1         --> KEY
#define PIN_KEY_RIGHT       GET_PIN(C, 4)      // PC4 :  KEY2         --> KEY
#define PIN_KEY_UP        	GET_PIN(C, 5)     // PC5:  WK_UP        --> KEY
uint8_t globel_key_val;

#define BUZZER_PIN      GET_PIN(B, 0)
static rt_thread_t tid = RT_NULL;
static struct rt_semaphore beep_sem_lock = {0};
rt_err_t Beeper_Thread_init(void);

/*	PAGE_KEY_NONE,
	PAGE_KEY_LEFT,
	PAGE_KEY_RIGHT,
	PAGE_KEY_UP,
	PAGE_KEY_DOWN,*/

static uint8_t Key_Read(void)
{
	if(rt_pin_read(PIN_KEY_LEFT) == PIN_LOW)
	{
		return PAGE_KEY_LEFT;
	}
	if(rt_pin_read(PIN_KEY_RIGHT) == PIN_LOW)
	{
		return PAGE_KEY_RIGHT;
	}
	if(rt_pin_read(PIN_KEY_UP) == PIN_LOW)
	{
		return PAGE_KEY_UP;
	}
	if(rt_pin_read(PIN_KEY_DOWN) == PIN_LOW)
	{
		return PAGE_KEY_DOWN;
	}
	return PAGE_KEY_NONE;
}

static void KeyScan_ThreadEntry(void)
{
	static uint8_t last_key = PAGE_KEY_NONE;
	static uint32_t last_tick = 0;
	{	
		uint8_t key = Key_Read();
		if (key != PAGE_KEY_NONE)
        {
            rt_thread_mdelay(50);
            if (key == Key_Read() && last_key != key)
            {
                rt_kprintf("%d pressed!",key);
				last_tick = rt_tick_get();
				globel_key_val = key;
				lv_event_send(AppWindow_GetObj(page.NowPage), LV_EVENT_KEY, &globel_key_val);
                rt_sem_release(&beep_sem_lock);
				// lv_event_send(AppWindow_GetObj(page.NowPage), LV_EVENT_KEY, &globel_key_val);
				last_key = key;
            }
			else if(last_key == key && rt_tick_get() - last_tick > 1000)
			{
				globel_key_val = key;
				lv_event_send(AppWindow_GetObj(page.NowPage), LV_EVENT_KEY, &globel_key_val);
				last_key = key;
			}
        }
        else if(last_key != PAGE_KEY_NONE)
        {
			last_key = PAGE_KEY_NONE;
        }
	}
}

static void Btns_Init(void)
{
	globel_key_val = 0;
	rt_pin_mode(PIN_KEY_LEFT, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(PIN_KEY_RIGHT, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(PIN_KEY_DOWN, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(PIN_KEY_UP, PIN_MODE_INPUT_PULLUP);
}

/**
  * @brief  页面初始化
  * @param  无
  * @retval 无
  */
void DisplayPage_Init()
{
    PM_PageManagerInit(&page, PAGE_MAX, 10);
    PAGE_IMPORT(MainMenu);        //主菜单
    PAGE_IMPORT(SDCard);
	PAGE_IMPORT(Dial);
    PAGE_IMPORT(SysInfo);
    PAGE_IMPORT(Attitude);
    PAGE_IMPORT(BackLight);
    PAGE_IMPORT(HWTest);
    PAGE_IMPORT(RGBLed);
    PAGE_IMPORT(Setting);
    PAGE_IMPORT(TimeSet);
    PAGE_IMPORT(EnvMonitor);
    PAGE_IMPORT(WiFi);
    //page.Push(PAGE_LEDAndBtn);
    PM_Push(&page, PAGE_Dial);
	Btns_Init();
    Beeper_Thread_init();
    lv_obj_add_event_cb(lv_scr_act(), (lv_event_cb_t)page_gestute_event_cb, LV_EVENT_ALL, NULL);

}

void Page_Loop_running(void)
{
	KeyScan_ThreadEntry();
	lv_task_handler();
	rt_thread_mdelay(LV_DISP_DEF_REFR_PERIOD);
}

/**
  * @brief  显示更新
  * @param  无
  * @retval 无
  */
void Display_Update()
{
    PM_Running(&page);
	KeyScan_ThreadEntry();
    //lv_task_handler();
}

/**
  * @brief  页面阻塞延时，保持lvgl更新
  * @param  无
  * @retval 无
  */
void PageDelay(uint32_t ms)
{
    uint32_t lastTime = lv_tick_get();
    
    while(lv_tick_elaps(lastTime) <= ms)
    {
        lv_task_handler();
		rt_thread_mdelay(LV_DISP_DEF_REFR_PERIOD);
    }
}

void Beeper_thread_entry()
{
    while (RT_TRUE)
    {
        rt_sem_take(&beep_sem_lock, RT_WAITING_FOREVER);
        rt_pin_write(BUZZER_PIN, PIN_HIGH);
        rt_thread_mdelay(100);
        rt_pin_write(BUZZER_PIN, PIN_LOW);
    }
    
}

rt_err_t Beeper_Thread_init()
{
    rt_pin_mode(BUZZER_PIN, PIN_MODE_OUTPUT);

    rt_sem_init(&beep_sem_lock, "beep_sem", 0 , RT_IPC_FLAG_PRIO);

    tid = rt_thread_create("Beeper",
                                    Beeper_thread_entry, RT_NULL,
                                    256,
                                    25,
                                    10);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
    else
    {
        rt_kprintf("create thread Beeper failed");
        return -1;
    }
}
