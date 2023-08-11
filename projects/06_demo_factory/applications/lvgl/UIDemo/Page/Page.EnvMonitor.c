
#include "../DisplayPrivate.h"
#include "aht10.h"
#include "ap3216c.h"
/*导出页面*/
PAGE_EXPORT(EnvMonitor);
static lv_anim_timeline_t* anim_timeline = NULL;
static lv_obj_t* Cont;
static lv_obj_t* Title;
static lv_obj_t* temp_label;
static lv_obj_t* hum_label;
static lv_obj_t* lux_label;
static uint8_t running;
static aht10_device_t aht10_dev;
static ap3216c_device_t ap_dev;
static void sensor_read(void)
{
	float humidity, temperature;
	humidity = aht10_read_humidity(aht10_dev);
	//rt_kprintf("humidity   : %d.%d %%", (int)humidity, (int)(humidity * 10) % 10);
	lv_label_set_text_fmt(hum_label, "hum   : %d.%d %%", (int)humidity, (int)(humidity * 10) % 10);
	temperature = aht10_read_temperature(aht10_dev);
	//rt_kprintf("temperature: %d.%d", (int)temperature, (int)(temperature * 10) % 10);
	lv_label_set_text_fmt(temp_label, "temp: %d.%d", (int)temperature, (int)(temperature * 10) % 10);

	float light = ap3216c_read_ambient_light(ap_dev);
	//rt_kprintf("ap3216c_read_ambient_light %f\n",light);
	lv_label_set_text_fmt(lux_label, "lux: %d.%d", (int)light, (int)(light * 10) % 10);

}

static void InfoLabel_Create(lv_obj_t* par)
{
	lv_obj_t* rect = lv_obj_create(par);
    lv_obj_set_size(rect, 200, 50);
	lv_obj_align(rect, LV_ALIGN_TOP_MID, 0, 20);
	lv_obj_set_style_border_width(rect, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(rect, lv_color_make(0xFF, 0xB4, 0x28), LV_PART_MAIN);
    lv_obj_clear_flag(rect, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(rect, 10, LV_PART_MAIN);
	lv_obj_set_style_pad_all(rect, 0, LV_PART_MAIN);
    lv_obj_t* label = lv_label_create(rect);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(label, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_label_set_text_fmt(label, "temp: 123");
    temp_label = label;
	
	lv_obj_t* rect2 = lv_obj_create(par);
    lv_obj_set_size(rect2, 200, 50);
	lv_obj_align_to(rect2, rect, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	lv_obj_set_style_border_width(rect2, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(rect2, lv_color_make(0x3F, 0xA9, 0xF5), LV_PART_MAIN);
    lv_obj_clear_flag(rect2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(rect2, 10, LV_PART_MAIN);
	lv_obj_set_style_pad_all(rect2, 0, LV_PART_MAIN);
    label = lv_label_create(rect2);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(label, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_label_set_text_fmt(label, "hum: 123");
    hum_label = label;
	
	lv_obj_t* rect3 = lv_obj_create(par);
    lv_obj_set_size(rect3, 200, 50);
    lv_obj_align_to(rect3, rect2, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	lv_obj_set_style_border_width(rect3, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(rect3, lv_color_make(0xEE, 0x4C, 0x84), LV_PART_MAIN);
    lv_obj_clear_flag(rect3, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(rect3, 10, LV_PART_MAIN);
	lv_obj_set_style_pad_all(rect3, 0, LV_PART_MAIN);
    label = lv_label_create(rect3);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(label, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_label_set_text_fmt(label, "lux: 123");
    lux_label = label;
}


static void Setup()
{
    /*将此页面移到前台*/
	running = 1;
    lv_obj_move_foreground(appWindow);
	Cont = Cont_Create(appWindow);
    Title = Title_Create(appWindow, "EnvMonitor");
	InfoLabel_Create(Cont);
    anim_timeline = PageAnim_Create(Cont, Title);
    PageAnim_Play(anim_timeline, false);
	PageDelay(100);
	aht10_dev = aht10_init("i2c3");
	ap_dev = ap3216c_init("i2c2");

	while(running)
    {
		sensor_read();
        Page_Loop_running();
    }
	PM_Pop(Page);
}

static void Exit()
{
	aht10_deinit(aht10_dev);
	ap3216c_deinit(ap_dev);
	PageAnim_Play(anim_timeline, true);
	lv_anim_timeline_del(anim_timeline);
    lv_obj_clean(appWindow);
}

static void Event(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_KEY)
	{
		uint8_t key = *(uint8_t*)lv_event_get_user_data(e);
		rt_kprintf("lv_indev_get_key %d \n", key);
		if(key == PAGE_KEY_LEFT)
		{
			running = 0;
            LV_LOG("Page->Pop()\n");
            PM_Pop(Page);
		}
		
		if(key == PAGE_KEY_UP)
		{

		}
		
		if(key == PAGE_KEY_DOWN)
		{

		}
		
		if(key == PAGE_KEY_RIGHT)
		{
			running = 0;
			LV_LOG("Page->Pop()\n");
            PM_Pop(Page);
		}
		
	}
}
