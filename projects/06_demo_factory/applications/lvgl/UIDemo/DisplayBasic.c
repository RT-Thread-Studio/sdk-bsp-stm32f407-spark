
#include "DisplayPrivate.h"

/**
  * @brief  通用显示初始化
  * @param  无
  * @retval 无
  */
static void Display_CommonInit()
{
    /*自定义主题初始化*/
    //lv_theme_t * th = lv_theme_watchx_init(
    //    LV_COLOR_WHITE, 
    //    LV_COLOR_RED,
    //    0,
    //    &Font_MicrosoftYaHei_28,
    //    &Font_MicrosoftYaHei_28,
    //    &Font_MicrosoftYaHei_28,
    //    &Font_MicrosoftYaHei_28
    //);
    //lv_theme_set_act(th);

    /*文件系统初始化*/
    //lv_fs_if_init();

    /*APP窗口初始化*/
    AppWindow_Create(lv_scr_act());

    /*状态栏初始化*/
    //StatusBar_Create(lv_layer_top());

    /*页面初始化*/
    DisplayPage_Init();
}

void Display_Init()
{
    Display_CommonInit();
}
