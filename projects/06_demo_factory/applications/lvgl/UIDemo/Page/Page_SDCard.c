
#include "../DisplayPrivate.h"
#include "lv_file_explorer.h"
#define DBG_TAG    "page sdcard"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>

/*导出页面*/
PAGE_EXPORT(SDCard);

typedef struct
{
    const char* text;
    const uint8_t pageID;
    lv_obj_t* btn;
    lv_obj_t* line;
} ListBtn_Typedef;

static ListBtn_Typedef btnGrp[] =
{
    {"Time.txt", PAGE_NONE},
    {"Backlight.wav", PAGE_NONE},
    {"Wuhu.avi", PAGE_NONE},
    {"Lala.mp4", PAGE_NONE},
    {"Yuele.mp3", PAGE_NONE},
    {"Rio.jpg", PAGE_NONE},
    {"Wotule.png", PAGE_NONE}
};
static lv_anim_timeline_t* anim_timeline = NULL;
static lv_anim_timeline_t* MsgAnimTimeLine = NULL;
static lv_obj_t* list1;
static lv_obj_t* Cont;
static lv_obj_t* Title;
static lv_style_t style;
static lv_obj_t* MsgBg;
static lv_obj_t* MsgRect;
static lv_obj_t* MsgText;

static lv_obj_t* file_explorer_panel;

static void Msg_Create(lv_obj_t* par, const char* name);
static void MsgAnim_Create(void);
static void MsgAnim_Play(bool playback);
static void File_Explorer_Create(lv_obj_t *par);

char current_path[128];

static void return_event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked: return\n");
        PM_Pop(Page);
    }
}

static void refresh_event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked: refresh");
    }
}

static void close_event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked: Msg Close\n");
        MsgAnim_Play(true);
    }
}

int is_avi_file(const char* filename) 
{
	char* extension = strrchr(filename, '.');
	if (extension != NULL && strcmp(extension + 1, "avi") == 0) 
	{
		LV_LOG_USER("%s is a avi file",filename);
		return 1;
	} 
	else 
	{
		LV_LOG_USER("%s is not a avi file",filename);
		return 0;
	}
}

static void file_explorer_event_cb(lv_event_t* e)
{
	const char* s = lv_file_explorer_get_selected_file_name(file_explorer_panel);
	const char* dir = lv_file_explorer_get_current_path(file_explorer_panel);
	LOG_I("%s",s);
	LOG_I("%s",dir);
	LV_LOG_USER("Clicked: Msg Close %s\n", s);
//	if(is_avi_file(s))
//	{
//		sprintf(file_name,"%s%s",dir,s);
//		//PM_Push(Page, PAGE_VideoPlayer);
//	}
}
static bool is_end_with(const char * str1, const char * str2)
{
    if(str1 == NULL || str2 == NULL)
        return false;

    uint16_t len1 = strlen(str1);
    uint16_t len2 = strlen(str2);
    if((len1 < len2) || (len1 == 0 || len2 == 0))
        return false;

    while(len2 >= 1) {
        if(str2[len2 - 1] != str1[len1 - 1])
            return false;

        len2--;
        len1--;
    }

    return true;
}

static void strip_ext(char * dir)
{
    char * end = dir + strlen(dir);

    while(end >= dir && *end != '/') {
        --end;
    }

    if(end > dir) {
        *end = '\0';
    }
    else if(end == dir) {
        *(end + 1) = '\0';
    }
}

static void showDir(const char* path)
{
	char fn[LV_FILE_EXPLORER_PATH_MAX_LEN];
    uint16_t index = 0;
    lv_fs_dir_t dir;
    lv_fs_res_t res;
	lv_obj_t* obj = list1;
	lv_obj_t* btn;
	
	lv_obj_clean(obj);
    /* */
//    extern struct player v_player;
//    rt_kprintf("cur path:%s  video name:%s\n\n", path, v_player.video_name);
//    if (rt_strcmp(path, v_player.video_name) == 0)
//    {
//        goto DIR_CLOSE;
//    }

    res = lv_fs_dir_open(&dir, path);
    if(res != LV_FS_RES_OK) {
        LV_LOG_USER("Open dir error %d!", res);
        return;
    }
	
	//lv_list_add_text(obj, path);
	
	btn = lv_list_add_btn(obj, LV_SYMBOL_DIRECTORY, ".");
	lv_obj_add_style(btn, &style, LV_PART_MAIN);
	lv_obj_set_style_bg_color(btn, lv_color_make(0, 0x40, 0xDD), LV_PART_MAIN | LV_STATE_FOCUSED);
	btn = lv_list_add_btn(obj, LV_SYMBOL_DIRECTORY, "..");
	lv_obj_add_style(btn, &style, LV_PART_MAIN);
	lv_obj_set_style_bg_color(btn, lv_color_make(0, 0x40, 0xDD), LV_PART_MAIN | LV_STATE_FOCUSED);

    while(1) {

        res = lv_fs_dir_read(&dir, fn);
        if(res != LV_FS_RES_OK) {
            LV_LOG_USER("Driver, file or directory is not exists %d!", res);
            break;
        }

        /*fn is empty, if not more files to read*/
        if(strlen(fn) == 0) {
            LV_LOG_USER("Not more files to read!");
            break;
        }

        if((is_end_with(fn, ".png") == true)  || (is_end_with(fn, ".PNG") == true)  || \
           (is_end_with(fn, ".jpg") == true) || (is_end_with(fn, ".JPG") == true) || \
           (is_end_with(fn, ".bmp") == true) || (is_end_with(fn, ".BMP") == true) || \
           (is_end_with(fn, ".gif") == true) || (is_end_with(fn, ".GIF") == true)) {
			btn = lv_list_add_btn(obj, LV_SYMBOL_IMAGE, fn);
        }
        else if((is_end_with(fn, ".mp3") == true) || (is_end_with(fn, ".MP3") == true)) {
			btn = lv_list_add_btn(obj, LV_SYMBOL_AUDIO, fn);
        }
        else if((is_end_with(fn, ".mp4") == true) || (is_end_with(fn, ".MP4") == true)) {
			btn = lv_list_add_btn(obj, LV_SYMBOL_VIDEO, fn);
        }
        else if((is_end_with(fn, ".avi") == true)) {
			btn = lv_list_add_btn(obj, LV_SYMBOL_VIDEO, fn);
        }
        else if((is_end_with(fn, ".") == true) || (is_end_with(fn, "..") == true)) {
            /*is dir*/
            continue;
        }
        else if(fn[0] == '/') {/*is dir*/
			btn = lv_list_add_btn(obj, LV_SYMBOL_DIRECTORY, fn + 1);
        }
        else {
			btn = lv_list_add_btn(obj, LV_SYMBOL_FILE, fn);
        }
		lv_obj_add_style(btn, &style, LV_PART_MAIN);
		lv_obj_set_style_bg_color(btn, lv_color_make(0, 0x40, 0xDD), LV_PART_MAIN | LV_STATE_FOCUSED);
		lv_obj_t* label = lv_obj_get_child(btn, 1);
		lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
		
        index++;
    }

    lv_fs_dir_close(&dir);

//DIR_CLOSE:
    lv_event_send(obj, LV_EVENT_READY, NULL);

    /*Move the table to the top*/
    lv_obj_scroll_to_y(obj, 0, LV_ANIM_OFF);
	lv_snprintf(current_path,sizeof(current_path),"%s/",path);
}
static void List_Create(lv_obj_t* par)
{
//	File_Explorer_Create(par);

    lv_style_init(&style);
    lv_style_set_bg_color(&style, LV_COLOR_BLACK);
    lv_style_set_radius(&style, 5);
    lv_style_set_border_color(&style, LV_COLOR_WHITE);
    lv_style_set_text_color(&style, LV_COLOR_WHITE);
    lv_style_set_border_width(&style, 0);
    lv_style_set_pad_all(&style, 5);
	lv_style_set_text_font(&style, &lv_font_montserrat_20);
	
    lv_obj_t* obj = lv_list_create(par);
	lv_obj_add_style(obj, &style, LV_PART_MAIN);
    lv_obj_set_size(obj, WINDOW_WIDTH, WINDOW_HEIGHT-TITLE_HEIGHT);
    lv_obj_center(obj);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);//Propagate the events to the parent too
	
	list1 = obj;
	
	const char * path = "/";
	
	showDir(path);

//    lv_memset_00(explorer->current_path, sizeof(explorer->current_path));
//    strcpy(explorer->current_path, path);
//    lv_label_set_text_fmt(Title, LV_SYMBOL_EYE_OPEN" %s", path);

//    size_t current_path_len = strlen(explorer->current_path);
//    if((*((explorer->current_path) + current_path_len) != '/') && (current_path_len < LV_FILE_EXPLORER_PATH_MAX_LEN)) {
//        *((explorer->current_path) + current_path_len) = '/';
//    }
}



static void File_Explorer_Create(lv_obj_t *par)
{

    file_explorer_panel = lv_file_explorer_create(par);
	//lv_obj_set_style_border_width(file_explorer_panel, 5, LV_PART_MAIN);
    lv_obj_set_size(file_explorer_panel, WINDOW_WIDTH, WINDOW_HEIGHT-TITLE_HEIGHT);
    lv_obj_align(file_explorer_panel, LV_ALIGN_CENTER, 0, 0);

    lv_file_explorer_set_sort(file_explorer_panel, LV_EXPLORER_SORT_NONE);
    lv_file_explorer_open_dir(file_explorer_panel, "/sdcard"); 

	lv_obj_add_event_cb(file_explorer_panel, file_explorer_event_cb, LV_EVENT_VALUE_CHANGED, par);
}

static void Msg_Create(lv_obj_t* par, const char* name)
{
    lv_obj_t* bg = lv_obj_create(par);
    lv_obj_align(bg, LV_ALIGN_BOTTOM_MID, 0, 0);
    //lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_border_width(bg, 0, LV_PART_MAIN);
    lv_obj_set_size(bg, WINDOW_WIDTH, WINDOW_HEIGHT - TITLE_HEIGHT);
    lv_obj_set_style_bg_color(bg, LV_COLOR_GRAY, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bg, 0, LV_PART_MAIN);
    lv_obj_clear_flag(bg, LV_OBJ_FLAG_SCROLLABLE);
    MsgBg = bg;

    lv_obj_t* rect = lv_obj_create(par);
    lv_obj_align(rect, LV_ALIGN_CENTER, 0, 0);
    //lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_border_width(rect, 5, LV_PART_MAIN);
    lv_obj_set_size(rect, 200, 150);
    lv_obj_set_style_bg_color(rect, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN);
    lv_obj_clear_flag(rect, LV_OBJ_FLAG_SCROLLABLE);
    //lv_obj_set_style_border_side(rect, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT, LV_PART_MAIN);
    lv_obj_set_style_radius(rect, 10, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(rect);
    lv_label_set_text(label, name);
    lv_obj_center(label);
    MsgText = label;
    MsgRect = rect;

    lv_obj_t* rbtn = lv_btn_create(rect);
    lv_obj_align(rbtn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_border_width(rbtn, 0, LV_PART_MAIN);
    lv_obj_set_size(rbtn, 50, 30);
    lv_obj_set_style_bg_color(rbtn, lv_color_make(0x99, 0xcc, 0xff), LV_PART_MAIN);
    lv_obj_add_event_cb(rbtn, close_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_t* rlabel = lv_label_create(rbtn);
    lv_label_set_text(rlabel, LV_SYMBOL_LEFT);
    lv_obj_center(rlabel);

}

static void MsgAnim_Create()
{
    int32_t tile_rey = lv_obj_get_y(MsgRect);

    lv_obj_set_y(MsgRect, WINDOW_HEIGHT);
    lv_obj_set_y(MsgBg, WINDOW_HEIGHT);
    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, MsgBg);
    lv_anim_set_values(&a1, WINDOW_HEIGHT, 0);
    lv_anim_set_early_apply(&a1, false);
    lv_anim_set_exec_cb(&a1, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a1, lv_anim_path_ease_in_out);
    lv_anim_set_time(&a1, 300);

    lv_anim_t a2;
    lv_anim_init(&a2);
    lv_anim_set_var(&a2, MsgRect);
    lv_anim_set_values(&a2, WINDOW_HEIGHT,0);
    lv_anim_set_early_apply(&a2, false);
    lv_anim_set_exec_cb(&a2, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a2, lv_anim_path_ease_in_out);
    lv_anim_set_time(&a2, 300);

    MsgAnimTimeLine = lv_anim_timeline_create();
    lv_anim_timeline_add(MsgAnimTimeLine, 0, &a1);
    lv_anim_timeline_add(MsgAnimTimeLine, 0, &a2);
     
}

static void MsgAnim_Play(bool playback)
{
    if(playback)lv_obj_add_flag(MsgBg, LV_OBJ_FLAG_EVENT_BUBBLE);
    else lv_obj_clear_flag(MsgBg, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_anim_timeline_set_reverse(MsgAnimTimeLine, playback);
    lv_anim_timeline_start(MsgAnimTimeLine);
    //PageDelay(lv_anim_timeline_get_playtime(MsgAnimTimeLine));
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);
    Cont = Cont_Create(appWindow);
    Title = Title_Create(appWindow, "SDCard");
    List_Create(Cont);
    Msg_Create(appWindow, "");
    MsgAnim_Create();
    anim_timeline = PageAnim_Create(Cont, Title);
    PageAnim_Play(anim_timeline, false);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    PageAnim_Play(anim_timeline, true);
    lv_anim_timeline_del(MsgAnimTimeLine);
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
			lv_group_set_editing(appWindow_g, false); /*Editing is not used by KEYPAD is be sure it is disabled*/
			lv_group_focus_prev(appWindow_g);
		}
		
		if(key == PAGE_KEY_DOWN)
		{
			lv_group_set_editing(appWindow_g, false); /*Editing is not used by KEYPAD is be sure it is disabled*/
			lv_group_focus_next(appWindow_g);

		}
		
		if(key == PAGE_KEY_RIGHT)
		{
			const char * str_fn = NULL;
			char file_name[LV_FILE_EXPLORER_PATH_MAX_LEN];
			lv_obj_t* obj = lv_group_get_focused(appWindow_g);
			str_fn = lv_list_get_btn_text(list1, obj);
			LV_LOG_USER("str_fn %s\n",str_fn);
			if((strcmp(str_fn, ".") == 0))  return;

			if((strcmp(str_fn, "..") == 0) && (strlen(current_path) > 3)) {
				strip_ext(current_path);
				/*Remove the last '/' character*/
				strip_ext(current_path);
				lv_snprintf((char *)file_name, sizeof(file_name), "%s", current_path);
			}
			else {
				if(strcmp(str_fn, "..") != 0) {
					lv_snprintf((char *)file_name, sizeof(file_name), "%s%s", current_path, str_fn);
				}
			}

			lv_fs_dir_t dir;
			LV_LOG_USER("lv_fs_dir_open file_name %s\n",file_name);
			if(lv_fs_dir_open(&dir, file_name) == LV_FS_RES_OK) {
				lv_fs_dir_close(&dir);
				showDir(file_name);
			}
//			else {
//				if(strcmp(str_fn, "..") != 0) {
//					explorer->sel_fn = str_fn;
//					lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
//				}
//			}
		}
		
	}
    if (code == LV_EVENT_PRESSING)
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if (dir == LV_DIR_RIGHT)
        {
            LV_LOG("Page->Pop()\n");
            PM_Pop(Page);
        }
    }
}
