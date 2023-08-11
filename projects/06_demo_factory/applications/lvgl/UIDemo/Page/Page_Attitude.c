
#include "../DisplayPrivate.h"
#include "icm20608.h"
/*导出页面*/
PAGE_EXPORT(Attitude);
static lv_anim_timeline_t* anim_timeline = NULL;
static lv_obj_t* Cont;
static lv_obj_t* Title;

typedef struct {
    lv_obj_t* bar;
    lv_obj_t* label;
}StateBar_t;
static uint8_t running;
static StateBar_t StateBars[6];
static lv_style_t* cont_style;
static icm20608_device_t dev;
static rt_thread_t dev_thread;
static void ContStyle_Setup(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, LV_COLOR_BLACK);
    lv_style_set_border_width(&style, 2);
    lv_style_set_radius(&style, 0);
    lv_style_set_border_color(&style, LV_COLOR_WHITE);
    lv_style_set_text_color(&style, LV_COLOR_WHITE);
    lv_style_set_border_width(&style, 0);
    lv_style_set_pad_all(&style, 0);
    cont_style = &style;
}


static void Slider_Create(lv_obj_t* par)
{

    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, WINDOW_WIDTH, 220);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(cont, 20, LV_PART_MAIN);
    for (int i = 0; i < __Sizeof(StateBars); i++)
    {
        /*Create a slider in the center of the display*/
        lv_obj_t* bar = lv_bar_create(cont);
        lv_obj_set_size(bar, 144, 20);
        StateBars[i].bar = bar;
        if (i < 1)
            lv_obj_align(bar, LV_ALIGN_TOP_MID, -30, 10);
        else if(i==3)
            lv_obj_align_to(bar, StateBars[i - 1].bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
        else 
            lv_obj_align_to(bar, StateBars[i - 1].bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
        /*Create a label below the slider*/
        lv_obj_t* label = lv_label_create(par);
        lv_obj_set_style_text_color(label, LV_COLOR_WHITE, LV_PART_MAIN);
        lv_label_set_text(label, "65535");
        StateBars[i].label = label;
    }

    for (int i = 0; i < __Sizeof(StateBars); i++)
    {
        lv_obj_align_to(StateBars[i].label, StateBars[i].bar, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    }

    lv_obj_t* label = lv_label_create(par);
    lv_obj_set_style_text_color(label, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_label_set_text(label, "Gyro:");
    lv_obj_align_to(label, StateBars[0].bar, LV_ALIGN_OUT_TOP_LEFT, 0, -5);
    label = lv_label_create(par);
    lv_obj_set_style_text_color(label, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_label_set_text(label, "Accel:");
    lv_obj_align_to(label, StateBars[3].bar, LV_ALIGN_OUT_TOP_LEFT, 0, -5);

}

void IMU_read(void* paramenter)
{
	rt_int16_t gyro_x = 0, gyro_y = 0, gyro_z = 0;
    rt_int16_t acc_x = 0, acc_y = 0, acc_z = 0;
	rt_err_t ret = icm20608_get_gyro(dev, &gyro_x, &gyro_y, &gyro_z);
    icm20608_get_accel(dev, &acc_x, &acc_y, &acc_z);
	lv_slider_set_value(StateBars[0].bar, 50 + (50 * gyro_x / 32767),LV_ANIM_OFF);
	lv_slider_set_value(StateBars[1].bar, 50 + (50 * gyro_y / 32767), LV_ANIM_OFF);
	lv_slider_set_value(StateBars[2].bar, 50 + (50 * gyro_z / 32767), LV_ANIM_OFF);
	lv_label_set_text_fmt(StateBars[0].label, "%d", gyro_x);
	lv_label_set_text_fmt(StateBars[1].label, "%d", gyro_y);
	lv_label_set_text_fmt(StateBars[2].label, "%d", gyro_z);

    lv_slider_set_value(StateBars[3].bar, 50 + (50 * acc_x / 32767),LV_ANIM_OFF);
	lv_slider_set_value(StateBars[4].bar, 50 + (50 * acc_y / 32767), LV_ANIM_OFF);
	lv_slider_set_value(StateBars[5].bar, 50 + (50 * acc_z / 32767), LV_ANIM_OFF);
	lv_label_set_text_fmt(StateBars[3].label, "%d", acc_x);
	lv_label_set_text_fmt(StateBars[4].label, "%d", acc_y);
	lv_label_set_text_fmt(StateBars[5].label, "%d", acc_z);
	rt_thread_mdelay(30);
}

static void Setup()
{
    /*将此页面移到前台*/
	running = 1;
	ContStyle_Setup();
    lv_obj_move_foreground(appWindow);
	Cont = Cont_Create(appWindow);
    Title = Title_Create(appWindow, "Attitude");
	Slider_Create(Cont);
    anim_timeline = PageAnim_Create(Cont, Title);
    PageAnim_Play(anim_timeline, false);
	
	dev = icm20608_init("i2c2");
	while(running)
    {
		IMU_read(RT_NULL);
		Page_Loop_running();
    }
	PM_Pop(Page);
}

static void Exit()
{
	icm20608_deinit(dev);
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
