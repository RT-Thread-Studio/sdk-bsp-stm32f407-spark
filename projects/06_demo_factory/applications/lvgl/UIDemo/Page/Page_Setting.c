
#include "../DisplayPrivate.h"

/*导出页面*/
PAGE_EXPORT(Setting);

typedef struct
{
	const char* name;
    const uint8_t pageID;
    lv_obj_t* obj;
}SettingsBTN_t;

static lv_anim_timeline_t* anim_timeline = NULL;
static lv_anim_timeline_t* MsgAnimTimeLine = NULL;
static lv_obj_t* list1;
static lv_obj_t* Cont;
static lv_obj_t* Title;

static SettingsBTN_t Setttings[]=
{
	{"TimeSet", PAGE_TimeSet},
	{"SysInfo", PAGE_SysInfo},
};

static void List_Creat(lv_obj_t* par)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, LV_COLOR_BLACK);
    lv_style_set_radius(&style, 5);
    lv_style_set_border_color(&style, LV_COLOR_WHITE);
    lv_style_set_text_color(&style, LV_COLOR_WHITE);
    lv_style_set_border_width(&style, 0);
    lv_style_set_pad_all(&style, 0);

    /*Create a list*/
    lv_obj_t* list = lv_list_create(par);
    lv_obj_add_style(list, &style, LV_PART_MAIN);
    lv_obj_set_size(list, PAGE_WIDTH, PAGE_HEIGHT);
    lv_obj_align(list, LV_ALIGN_CENTER, 0,0);
    for (int i = 0; i < __Sizeof(Setttings); i++)
    {
        lv_obj_t* btn;
        btn = lv_list_add_btn(list, LV_SYMBOL_RIGHT, Setttings[i].name);
        lv_obj_add_style(btn, &style, LV_PART_MAIN);
        lv_obj_set_style_border_width(btn, 2, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_height(btn, 40);
        lv_obj_set_style_bg_color(btn, lv_color_make(0, 0x40, 0xDD), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_text_font(btn, &lv_font_montserrat_28, LV_PART_MAIN);
        Setttings[i].obj = btn;
    }
	list1 = list;
}

static void Setup(void)
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);
	Cont = Cont_Create(appWindow);
	Title = Title_Create(appWindow,"Settings");
	List_Creat(Cont);
	
	anim_timeline = PageAnim_Create(Cont, Title);
    PageAnim_Play(anim_timeline, false);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit(void)
{
	PageAnim_Play(anim_timeline, true);
	lv_anim_timeline_del(anim_timeline);
    lv_obj_clean(appWindow);
}

/**
  * @brief  页面事件
  * @param  obj:发生事件的对象
  * @param  event:事件编号
  * @retval 无
  */
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
            lv_group_set_editing(appWindow_g, false);
			lv_group_focus_prev(appWindow_g);
		}
		
		if(key == PAGE_KEY_DOWN)
		{
			lv_group_set_editing(appWindow_g, false);
			lv_group_focus_next(appWindow_g);
		}
		
		if(key == PAGE_KEY_RIGHT)
		{

			lv_obj_t* obj = lv_group_get_focused(appWindow_g);
			int index = lv_obj_get_index(obj);
			LV_LOG_USER("PM_Push TO %s",Setttings[index].name);
			PM_Push(Page, Setttings[index].pageID);

		}
		
	}
}
