
#include "../DisplayPrivate.h"

/*导出页面*/
PAGE_EXPORT(SysInfo);
static lv_anim_timeline_t* anim_timeline = NULL;
static lv_obj_t* Cont;
static lv_obj_t* Title;
static lv_style_t label_style;

static void label_style_create()
{
    lv_style_init(&label_style);
	lv_style_set_text_align(&label_style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_color(&label_style, LV_COLOR_WHITE);
    lv_style_set_text_font(&label_style, &lv_font_montserrat_20);
}


static void Info_Create(lv_obj_t* par)
{

    lv_obj_t* obj = lv_obj_create(par);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_size(obj, 170, 170);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_bg_color(obj, LV_COLOR_BLACK, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t* label1 = lv_label_create(par);
    //lv_obj_set_width(label1, 150);
    lv_label_set_text(label1, "RT-Spark 1");
    lv_obj_add_style(label1, &label_style, LV_PART_MAIN);
    lv_obj_set_style_text_font(label1, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_align_to(label1, obj, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t* label2 = lv_label_create(par);
    //lv_obj_set_width(label2, 150);
    lv_label_set_text(label2, "RT-Thread 5.0.0");
    lv_obj_add_style(label2, &label_style, LV_PART_MAIN);
    lv_obj_align_to(label2, label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t* label3 = lv_label_create(par);
    //lv_obj_set_width(label3, 150);
    lv_label_set_text(label3, "LVGL 8.3");
    lv_obj_add_style(label3, &label_style, LV_PART_MAIN);
    lv_obj_align_to(label3, label2, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t* label4 = lv_label_create(par);
    //lv_obj_set_width(label3, 150);
    lv_label_set_text(label4, "QQ Group ID: ");
    lv_obj_add_style(label4, &label_style, LV_PART_MAIN);
    lv_obj_align_to(label4, label3, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_t* label5 = lv_label_create(par);
    lv_label_set_text(label5, "546631962");
    lv_obj_add_style(label5, &label_style, LV_PART_MAIN);
    lv_obj_align_to(label5, label4, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t* label6 = lv_label_create(par);
    lv_label_set_text(label6, "Welcome to join it");
    lv_obj_add_style(label6, &label_style, LV_PART_MAIN);
    lv_obj_align_to(label6, label5, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}


static void Setup()
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);
	Cont = Cont_Create(appWindow);
    Title = Title_Create(appWindow, "SysInfo");
	label_style_create();
	Info_Create(Cont);
    anim_timeline = PageAnim_Create(Cont, Title);
    PageAnim_Play(anim_timeline, false);
}

static void Exit()
{
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
			LV_LOG("Page->Pop()\n");
            PM_Pop(Page);
		}
		
	}
}