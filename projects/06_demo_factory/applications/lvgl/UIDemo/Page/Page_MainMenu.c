
#include "../DisplayPrivate.h"

PAGE_EXPORT(MainMenu);

static lv_obj_t* contApps;
static lv_anim_timeline_t* anim_timeline = NULL;

LV_IMG_DECLARE(IMG_SDCard);
LV_IMG_DECLARE(IMG_RGBLed);
LV_IMG_DECLARE(IMG_WiFi);
LV_IMG_DECLARE(IMG_EnvMonitor);
LV_IMG_DECLARE(IMG_Attitude);
LV_IMG_DECLARE(IMG_Setting);
typedef struct
{
    const void* src_img;
    const char* name;
    const uint8_t pageID;
    int bg_color;
    lv_obj_t* obj;
} AppICON_TypeDef;

#define APP_DEF(name, color) {&IMG_##name, #name, PAGE_##name, color, NULL}
#define APP_ICON_SIZE 90
#define APP_ICON_ANIM_TIME 100
#define MENU_ROW 2
#define MENU_COL 3
#define APP_ICON_GAP 20

static uint8_t ICON_NowSelIndex = 0;


static AppICON_TypeDef AppICON_Grp[] =
{
    APP_DEF(SDCard, 0xFFB428),
	APP_DEF(RGBLed, 0x0400A5),
	APP_DEF(EnvMonitor, 0xFF7FC6),
	APP_DEF(Attitude, 0x4EB9AB),
	APP_DEF(Setting, 0x0040DD),
};

static lv_style_t* cont_style;
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

static void AppClickAnim(lv_obj_t* img, bool ispress)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, img);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
    lv_anim_set_values(&a, lv_img_get_zoom(img), ispress ? 190 : LV_IMG_ZOOM_NONE);
    lv_anim_set_time(&a, APP_ICON_ANIM_TIME);

    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

    lv_anim_start(&a);
}

static void AppICON_EventHandler(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_current_target(e);
    lv_obj_t* img = lv_obj_get_child(obj, 0);

    lv_event_code_t code = lv_event_get_code(e);
    AppICON_TypeDef* appIcon = (AppICON_TypeDef*)lv_obj_get_user_data(obj);

    if(code == LV_EVENT_PRESSED)
    {
        AppClickAnim(img, true);
    }
    else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
    {
        AppClickAnim(img, false);
    }
    else if(code == LV_EVENT_SHORT_CLICKED)
    {
        uint8_t pageID = appIcon->pageID;

        if (pageID == PAGE_NONE)
            return;

        if (pageID == PAGE_Setting)
        {
            PM_Push(Page, PAGE_Setting);
        }
        else if (pageID == PAGE_SDCard)
        {
            PM_Push(Page, PAGE_SDCard);
        }
//        else if (pageID == PAGE_WiFi)
//        {
//            PM_Push(Page, PAGE_WiFi);
//        }
    }
}

static void AppICON_Create(lv_obj_t* par)
{

    int label_h = 16;

    lv_obj_t* obj = lv_obj_create(par);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(obj, WINDOW_WIDTH, (__Sizeof(AppICON_Grp) + 1 )* (APP_ICON_SIZE + APP_ICON_GAP));
    lv_obj_add_style(obj, cont_style, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);//Propagate the events to the parent too

    contApps = obj;

    for (int i = 0; i < __Sizeof(AppICON_Grp); i++)
    {
        lv_obj_t* obj_vir = lv_obj_create(contApps);
        lv_obj_set_style_border_width(obj_vir, 0, LV_PART_MAIN);
        lv_obj_set_size(obj_vir, APP_ICON_SIZE, APP_ICON_SIZE);
        //lv_obj_set_drag_parent(obj_vir, true);
        //lv_obj_add_event_cb(obj_vir, (lv_event_cb_t)AppICON_EventHandler, LV_EVENT_ALL, NULL);
        lv_obj_set_user_data(obj_vir, &(AppICON_Grp[i]));
        lv_obj_clear_flag(obj_vir, LV_OBJ_FLAG_SCROLLABLE);
        int col = MENU_COL;
        lv_coord_t interval_pixel = APP_ICON_SIZE + APP_ICON_GAP;
        lv_obj_align(
            obj_vir,
            LV_ALIGN_TOP_MID,
            -40,
            i* interval_pixel
        );

        lv_obj_set_style_bg_color(obj_vir, lv_color_hex(AppICON_Grp[i].bg_color), LV_PART_MAIN);
        lv_obj_add_event_cb(obj_vir, (lv_event_cb_t)AppICON_EventHandler, LV_EVENT_ALL, NULL);

        lv_obj_t* img = lv_img_create(obj_vir);
        lv_img_set_src(img, AppICON_Grp[i].src_img);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t* label = lv_label_create(contApps);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
        lv_obj_set_width(label, 150);
        lv_label_set_text(label, AppICON_Grp[i].name);
		lv_obj_set_style_text_color(label, LV_COLOR_WHITE, LV_PART_MAIN);
        lv_obj_align_to(label, obj_vir, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
		lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);
		lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        AppICON_Grp[i].obj = obj_vir;
    }

}

static void ICON_Grp_MoveFouce(uint8_t iconIndex)
{
    int target_y = - iconIndex * (APP_ICON_SIZE + APP_ICON_GAP) + (WINDOW_HEIGHT - APP_ICON_SIZE)/2;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, contApps);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, lv_obj_get_y(contApps), target_y);
    lv_anim_set_time(&a, APP_ICON_ANIM_TIME);

    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

    lv_anim_start(&a);
    //PageDelay(APP_ICON_ANIM_TIME);
}

static void ICON_Grp_Move(int8_t dir)
{
    __ValuePlus(ICON_NowSelIndex, dir, 0, __Sizeof(AppICON_Grp)-1);
	rt_kprintf("appIcon move to %d\n",ICON_NowSelIndex);
    ICON_Grp_MoveFouce(ICON_NowSelIndex);
}

static void MainPageAnim_Create()
{
    int32_t cont_rey = lv_obj_get_y(contApps);

    lv_obj_set_y(contApps, WINDOW_HEIGHT);

    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, contApps);
    lv_anim_set_values(&a1, WINDOW_HEIGHT, 	((APP_ICON_SIZE + APP_ICON_GAP) + (WINDOW_HEIGHT - APP_ICON_SIZE)/2));
    lv_anim_set_early_apply(&a1, false);
    lv_anim_set_exec_cb(&a1, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a1, lv_anim_path_ease_out);
    lv_anim_set_time(&a1, 300);

    anim_timeline = lv_anim_timeline_create();
    lv_anim_timeline_add(anim_timeline, 0, &a1);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup(void)
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);
	ContStyle_Setup();
    //ImgAppShadow_Create(appWindow);
    AppICON_Create(appWindow);
//    MainPageAnim_Create();
//    PageAnim_Play(anim_timeline, false);
	lv_obj_set_y(contApps, WINDOW_HEIGHT);
	PageDelay(10);
	ICON_Grp_MoveFouce(ICON_NowSelIndex);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit(void)
{
//    PageAnim_Play(anim_timeline, true);
//    lv_anim_timeline_del(anim_timeline);
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
            ICON_Grp_Move(-1);
		}
		
		if(key == PAGE_KEY_DOWN)
		{
            ICON_Grp_Move(+1);
		}
		
		if(key == PAGE_KEY_RIGHT)
		{

			PM_Push(Page, AppICON_Grp[ICON_NowSelIndex].pageID);

		}
		
	}
    if(code == LV_EVENT_PRESSING)
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if (dir == LV_DIR_RIGHT)
        {
            LV_LOG("Page->Pop()\n");
            PM_Pop(Page);
        }
    }
}
