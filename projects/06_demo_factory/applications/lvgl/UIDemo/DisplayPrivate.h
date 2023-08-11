#ifndef __DISPLAYPRIVATE_H
#define __DISPLAYPRIVATE_H

void Display_Init(void);
void Display_Update(void);

#include "CommonMacro.h"
/*Page*/
#include "PageManager.h"
/*LVGL*/
#include "lvgl.h"
//#include "lv_obj_ext_func.h"
//#include "lv_theme_conf.h"


#include <rtthread.h>
#include "rtdevice.h"
#include <rthw.h>
#include "board.h"

#include <string.h>
#include <stdio.h> 

//#include "wlan_dev.h"
//#include "wlan_cfg.h"
//#include "wlan_mgnt.h"
//#include "wlan_prot.h"

//#include <netdev_ipaddr.h>
//#include <netdev.h>

#define WINDOW_WIDTH 240
#define WINDOW_HEIGHT 240
#define TITLE_HEIGHT 20
#define TITLE_WIDTH 240
#define PAGE_HEIGHT (WINDOW_HEIGHT-TITLE_HEIGHT)
#define PAGE_WIDTH (WINDOW_WIDTH)

#define TITLE_FONT (lv_font_montserrat_20)
extern PageManager_t page;
extern lv_group_t* appWindow_g;
typedef enum
{
	PAGE_KEY_NONE,
	PAGE_KEY_LEFT,
	PAGE_KEY_RIGHT,
	PAGE_KEY_UP,
	PAGE_KEY_DOWN,
} Key_Enum;

typedef enum
{
    /*保留*/
    PAGE_NONE,
    /*用户页面*/
    PAGE_MainMenu,
    PAGE_SDCard,
	PAGE_SysInfo,
	PAGE_Dial,
    PAGE_Attitude,
    PAGE_BackLight,
    PAGE_HWTest,
    PAGE_RGBLed,
    PAGE_Setting,
    PAGE_TimeSet,
    PAGE_EnvMonitor,
    PAGE_WiFi,
    /*保留*/
    PAGE_MAX
} Page_Type;

/*Clock*/
typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t ms;
    uint16_t year;
    uint8_t  month;
    uint8_t  date;
    uint8_t  week;
} Clock_Value_t;

typedef struct
{
	uint8_t temp;
	uint8_t hum;
}Weather_Value_t;

extern uint8_t globel_key_val;
void Page_Loop_running(void);
void PageDelay(uint32_t ms);
void DisplayPage_Init(void);

#define PAGE_EXPORT(name)\
static PageManager_t* Page;\
static lv_obj_t* appWindow;\
static void Setup(void);\
static void Exit(void);\
static void Event(lv_event_t* e);\
static void Page_EventHandler(void* obj, uint8_t event)\
{\
    if(obj == Page)\
    {\
        switch (event)\
        {\
        case PM_MSG_Setup: Setup(); break; \
        case PM_MSG_Exit:  Exit();  break; \
        case PM_MSG_Loop:/*Loop();*/break; \
        default: break; \
        }\
    }\
}\
void PageRegister_##name(PageManager_t* page, uint8_t pageID)\
{\
    appWindow = AppWindow_GetObj(pageID);\
    lv_obj_add_event_cb(appWindow, (lv_event_cb_t)Event, LV_EVENT_ALL , &globel_key_val);\
    PM_Register(page, pageID, Page_EventHandler, #name);\
    Page = page; \
}

#define __Sizeof(arr) (sizeof(arr)/sizeof(arr[0]))

#define LV_COLOR_WHITE lv_color_make(0xFF, 0xFF, 0xFF)
#define LV_COLOR_SILVER lv_color_make(0xC0, 0xC0, 0xC0)
#define LV_COLOR_GRAY lv_color_make(0x80, 0x80, 0x80)
#define LV_COLOR_BLACK lv_color_make(0x00, 0x00, 0x00)
#define LV_COLOR_RED lv_color_make(0xFF, 0x00, 0x00)
#define LV_COLOR_MAROON lv_color_make(0x80, 0x00, 0x00)
#define LV_COLOR_YELLOW lv_color_make(0xFF, 0xFF, 0x00)
#define LV_COLOR_OLIVE lv_color_make(0x80, 0x80, 0x00)
#define LV_COLOR_LIME lv_color_make(0x00, 0xFF, 0x00)
#define LV_COLOR_GREEN lv_color_make(0x00, 0x80, 0x00)
#define LV_COLOR_CYAN lv_color_make(0x00, 0xFF, 0xFF)
#define LV_COLOR_AQUA LV_COLOR_CYAN
#define LV_COLOR_TEAL lv_color_make(0x00, 0x80, 0x80)
#define LV_COLOR_BLUE lv_color_make(0x00, 0x00, 0xFF)
#define LV_COLOR_NAVY lv_color_make(0x00, 0x00, 0x80)
#define LV_COLOR_MAGENTA lv_color_make(0xFF, 0x00, 0xFF)
#define LV_COLOR_PURPLE lv_color_make(0x80, 0x00, 0x80)
#define LV_COLOR_ORANGE lv_color_make(0xFF, 0xA5, 0x00)

LV_FONT_DECLARE(Font_Impact_120);
LV_FONT_DECLARE(Font_RexBold_28);
LV_FONT_DECLARE(Font_RexBold_68);
LV_FONT_DECLARE(Font_RexBold_89);

/*AppWindow*/
void AppWindow_Create(lv_obj_t* par);
lv_obj_t* AppWindow_GetObj(uint8_t pageID);
lv_obj_t* Cont_Create(lv_obj_t* par);
lv_obj_t* Title_Create(lv_obj_t* par,const char* title);
lv_anim_timeline_t* PageAnim_Create(lv_obj_t* cont, lv_obj_t* title);
void PageAnim_Play(lv_anim_timeline_t* animtimeline, bool playback);
#define APP_WIN_HEIGHT lv_obj_get_height(appWindow)
#define APP_WIN_WIDTH  lv_obj_get_width(appWindow)

//int WiFi_Scan(void);
//void WIFI_List_Clear(void);
//void WIFI_Add_List(char* ssid);
//int WiFi_Join(const char *ssid, const char *password);
//void wifi_join_event(int ret);
#endif
