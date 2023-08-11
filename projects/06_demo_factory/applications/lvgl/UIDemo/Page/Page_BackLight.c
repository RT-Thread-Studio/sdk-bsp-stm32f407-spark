
#include "../DisplayPrivate.h"

PAGE_EXPORT(BackLight);
static lv_anim_timeline_t* anim_timeline = NULL;
static lv_obj_t* Cont;
static lv_obj_t* Title;

static void Setup()
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);
	Cont = Cont_Create(appWindow);
    Title = Title_Create(appWindow, "BackLight");

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
