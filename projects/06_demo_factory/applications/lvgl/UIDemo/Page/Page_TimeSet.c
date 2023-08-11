
#include "../DisplayPrivate.h"

/*导出页面*/
PAGE_EXPORT(TimeSet);
static lv_anim_timeline_t* anim_timeline = NULL;
static lv_obj_t* Cont;
static lv_obj_t* Title;

static lv_style_t btn_style;
static lv_style_t btn_focus_style;
static lv_style_t label_style;
static uint8_t is_setting;
static uint8_t index;
static lv_obj_t* obj_focused;
typedef struct
{
    lv_obj_t* btn;
    lv_obj_t* label;
    const char* name;
    uint16_t min;
    uint16_t val;
    uint16_t max;
}SetterBtn_t;
static SetterBtn_t setter_btns[] =
{
    {NULL,NULL,"Hour",0,16,24},
    {NULL,NULL,"Min",0,16,60},
    {NULL,NULL,"Sec",0,16,60},
    {NULL,NULL,"Year",2020,2023,2400},
    {NULL,NULL,"Mon",1,6,12},
    {NULL,NULL,"Day",1,6,31},
};
static void btn_style_create()
{
    lv_style_init(&btn_style);
    lv_style_set_radius(&btn_style, 5);
    lv_style_set_border_width(&btn_style, 0);
    lv_style_set_border_color(&btn_style, LV_COLOR_WHITE);
    lv_style_set_bg_color(&btn_style, lv_color_hex(0x0D8EFF));
    lv_style_set_text_color(&btn_style, LV_COLOR_WHITE);
    lv_style_set_pad_all(&btn_style, 0);
}

static void btn_focus_style_create()
{
    lv_style_init(&btn_focus_style);
    lv_style_set_radius(&btn_focus_style, 5);
    lv_style_set_border_width(&btn_focus_style, 3);
    lv_style_set_border_color(&btn_focus_style, LV_COLOR_WHITE);
    lv_style_set_bg_color(&btn_focus_style, lv_color_hex(0x0D8EFF));
    lv_style_set_text_color(&btn_focus_style, LV_COLOR_WHITE);
    lv_style_set_pad_all(&btn_focus_style, 0);
}

static void label_style_create()
{
    lv_style_init(&label_style);
    lv_style_set_text_color(&label_style, LV_COLOR_WHITE);
    lv_style_set_text_font(&label_style, &lv_font_montserrat_28);
}

static void time_set_create(lv_obj_t* par)
{
	struct tm *Time;
	time_t now;
	now = time(RT_NULL) + 28800;
	Time = gmtime(&now);
	setter_btns[3].val = Time->tm_year+1900;
	setter_btns[4].val = Time->tm_mon+1;
	setter_btns[5].val = Time->tm_mday;
	setter_btns[0].val = Time->tm_hour;
	setter_btns[1].val = Time->tm_min;
	setter_btns[2].val = Time->tm_sec;
	
    lv_obj_t* obj = lv_obj_create(par);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(obj, WINDOW_WIDTH, PAGE_HEIGHT);
    lv_obj_set_style_bg_color(obj, LV_COLOR_BLACK, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);

    for (int i = 0; i < __Sizeof(setter_btns); i++)
    {
        lv_obj_t* btn = lv_btn_create(obj);
        lv_obj_set_size(btn, 100, 35);
        lv_obj_add_style(btn, &btn_style, LV_PART_MAIN);
        lv_obj_add_style(btn, &btn_focus_style, LV_PART_MAIN | LV_STATE_FOCUSED);
        if (i < 1)
            lv_obj_align(btn, LV_ALIGN_TOP_MID, 30, 10);
        else
            lv_obj_align_to(btn, setter_btns[i - 1].btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
        setter_btns[i].btn = btn;
        lv_obj_t* btn_label = lv_label_create(btn);
        lv_obj_add_style(btn_label, &label_style, LV_PART_MAIN);
        lv_label_set_text_fmt(btn_label, "%d", setter_btns[i].val);
        lv_obj_center(btn_label);
        setter_btns[i].label = btn_label;
        lv_obj_t* label = lv_label_create(obj);
        lv_obj_add_style(label, &label_style, LV_PART_MAIN);
        lv_obj_set_width(label, 90);
        lv_label_set_text(label, setter_btns[i].name);
        lv_obj_get_style_text_align(label, LV_TEXT_ALIGN_RIGHT);
        lv_obj_align_to(label, btn, LV_ALIGN_OUT_LEFT_MID, 0, 0);
    }
}

static void btn_change_val(SetterBtn_t* setter, uint8_t dir)
{
	if(setter==RT_NULL)return;
	int value = setter->val;
	if(dir)
		value++;
	else
		value--;
	if(value > setter->max)
		value = setter->min;
	if(value < setter->min)
		value = setter->max;
	
	setter->val = value;
	lv_label_set_text_fmt(setter->label, "%d", setter->val);
}

static void Setup()
{
    is_setting = 0;
    lv_obj_move_foreground(appWindow);
	Cont = Cont_Create(appWindow);
    Title = Title_Create(appWindow, "TimeSet");
    btn_style_create();
	btn_focus_style_create();
    label_style_create();
    time_set_create(Cont);
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
			if(is_setting)
			{
				is_setting = 0;
				lv_obj_set_style_bg_color(obj_focused, lv_color_hex(0x0D8EFF), LV_PART_MAIN | LV_STATE_FOCUSED);
				set_time(setter_btns[0].val, setter_btns[1].val, setter_btns[2].val);
				set_date(setter_btns[3].val, setter_btns[4].val, setter_btns[5].val);
			}
			else
			{
				LV_LOG("Page->Pop()\n");
				PM_Pop(Page);
			}

		}
		if(key == PAGE_KEY_UP)
		{
			if(is_setting)
			{
				btn_change_val(&(setter_btns[index]), 1);
			}
			else
			{
				lv_group_set_editing(appWindow_g, false);
				lv_group_focus_prev(appWindow_g);
			}
		}
		
		if(key == PAGE_KEY_DOWN)
		{
			if(is_setting)
			{
				btn_change_val(&(setter_btns[index]), 0);
			}
			else
			{
				lv_group_set_editing(appWindow_g, false);
				lv_group_focus_next(appWindow_g);
			}
		}
		
		if(key == PAGE_KEY_RIGHT)
		{
			if(!is_setting)
			{
				obj_focused = lv_group_get_focused(appWindow_g);
				index = (lv_obj_get_index(obj_focused));
				if(index!=0)index = index / 2;
				LV_LOG_USER("index: %d\n",index);
				lv_obj_set_style_bg_color(obj_focused, lv_color_hex(0x7AC4FF), LV_PART_MAIN | LV_STATE_FOCUSED);
				is_setting = 1;
			}
		}
		
	}
}
