
#include "../DisplayPrivate.h"

/*导出页面*/
PAGE_EXPORT(RGBLed);
static lv_obj_t* Cont;
static lv_obj_t* Title;
static lv_obj_t* ColorCont;
static lv_obj_t* ColorLabel;
static lv_style_t* cont_style;
static lv_anim_timeline_t* anim_timeline = NULL;

typedef struct {
    lv_obj_t* slider;
    lv_obj_t* label;
}RGBSlider_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
}RGBColor_t;

static RGBSlider_t rgbSliders[3];
static RGBColor_t LedColor;
static uint8_t isSelect;

static void slider_event_cb(lv_event_t* e)
{
    lv_obj_t* slider = lv_event_get_target(e);
    lv_obj_t* label = lv_event_get_user_data(e);
    int val = (int)lv_slider_get_value(slider);
    lv_label_set_text_fmt(label, "%d%%", val);
    if (slider == rgbSliders[0].slider)
    {
        LedColor.r = (uint8_t)lv_slider_get_value(slider) * 255 / 100;
    }
    else if (slider == rgbSliders[1].slider)
    {
        LedColor.g = (uint8_t)lv_slider_get_value(slider) * 255 / 100;
    }
    else if (slider == rgbSliders[2].slider)
    {
        LedColor.b = (uint8_t)lv_slider_get_value(slider) * 255 / 100;
    }
    lv_label_set_text_fmt(ColorLabel, "%d,%d,%d", LedColor.r, LedColor.g, LedColor.b);
    lv_obj_set_style_bg_color(ColorCont, lv_color_make(LedColor.r, LedColor.g, LedColor.b), LV_PART_MAIN);
    extern void led_matrix_fill_rgb(uint8_t r, uint8_t g, uint8_t b)
	led_matrix_fill_rgb(LedColor.r, LedColor.g, LedColor.b);
}
void Slider_Create(lv_obj_t* par)
{

    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, WINDOW_WIDTH, 150);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(cont, 20, LV_PART_MAIN);
    for (int i = 0; i < __Sizeof(rgbSliders); i++)
    {
        /*Create a slider in the center of the display*/
        lv_obj_t* slider = lv_slider_create(cont);
        lv_obj_set_size(slider, 120, 20);
		lv_obj_set_style_border_width(slider, 2, LV_PART_MAIN | LV_STATE_FOCUSED);
		lv_obj_set_style_border_color(slider, LV_COLOR_WHITE, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(slider, 2, LV_PART_INDICATOR | LV_STATE_FOCUSED);
		lv_obj_set_style_border_color(slider, LV_COLOR_WHITE, LV_PART_INDICATOR | LV_STATE_FOCUSED);
		rgbSliders[i].slider = slider;
        /*Create a label below the slider*/
        lv_obj_t* label= lv_label_create(par);
        lv_label_set_text(label, "0%");
		lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN);
        rgbSliders[i].label = label;
        lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, label);
		lv_group_add_obj(appWindow_g, slider);
    }
    lv_obj_align_to(rgbSliders[0].slider, cont, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_set_style_bg_color(rgbSliders[0].slider, lv_color_make(0xff,0,0) , LV_PART_KNOB);
    lv_obj_align_to(rgbSliders[1].slider, cont, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_set_style_bg_color(rgbSliders[1].slider, lv_color_make(0,0xff,0) , LV_PART_KNOB);
    lv_obj_align_to(rgbSliders[2].slider, cont, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	lv_obj_set_style_bg_color(rgbSliders[2].slider, lv_color_make(0,0,0xff) , LV_PART_KNOB);
    for (int i = 0; i < __Sizeof(rgbSliders); i++)
    {
        lv_obj_align_to(rgbSliders[i].label, rgbSliders[i].slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    }

    lv_obj_t* colorCont = lv_obj_create(par);
    //lv_obj_add_style(colorCont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(colorCont, 70, 40);
    lv_obj_align_to(colorCont, cont, LV_ALIGN_OUT_BOTTOM_LEFT, 20, 0);
    lv_obj_clear_flag(colorCont, LV_OBJ_FLAG_SCROLLABLE);
    ColorCont = colorCont;

    lv_obj_t* colorlabel = lv_label_create(par);
    lv_label_set_text(colorlabel, "0,0,0");
	lv_obj_set_style_text_font(colorlabel, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_align_to(colorlabel, ColorCont, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    ColorLabel = colorlabel;
}

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

static void Setup(void)
{
    lv_obj_move_foreground(appWindow);
    ContStyle_Setup();
	Cont = Cont_Create(appWindow);
	Title = Title_Create(appWindow,"Settings");
    Slider_Create(Cont);
	anim_timeline = PageAnim_Create(Cont, Title);
    PageAnim_Play(anim_timeline, false);
}

static void Exit(void)
{
    PageAnim_Play(anim_timeline, true);
    lv_anim_timeline_del(anim_timeline);
	extern void led_matrix_rst();
	led_matrix_rst();
    lv_obj_clean(appWindow);
}

static void Event(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_KEY)
    {
        uint8_t key = *(uint8_t*)lv_event_get_user_data(e);
        if (key == PAGE_KEY_LEFT)
        {
			if(isSelect)
			{
				isSelect = 0;
				lv_obj_t* obj = lv_group_get_focused(appWindow_g);
				lv_obj_set_style_border_width(obj, 0, LV_PART_KNOB | LV_STATE_FOCUSED);
			}
			else
			{
				LV_LOG("Page->Pop()\n");
				PM_Pop(Page);
			}
        }

        if (key == PAGE_KEY_UP)
        {
			if(!isSelect)
			{
				lv_group_set_editing(appWindow_g, false);
				lv_group_focus_prev(appWindow_g);
			}
			else
			{
				lv_group_send_data(appWindow_g, LV_KEY_RIGHT);
			}
        }

        if (key == PAGE_KEY_DOWN)
        {
			if(!isSelect)
			{
				lv_group_set_editing(appWindow_g, false);
				lv_group_focus_next(appWindow_g);
			}
			else
			{
				lv_group_send_data(appWindow_g, LV_KEY_LEFT);
			}
        }

        if (key == PAGE_KEY_RIGHT)
        {
			isSelect = 1;
			lv_obj_t* obj = lv_group_get_focused(appWindow_g);
			lv_obj_set_style_border_width(obj, 2, LV_PART_KNOB | LV_STATE_FOCUSED);
			lv_obj_set_style_border_color(obj, LV_COLOR_WHITE, LV_PART_KNOB | LV_STATE_FOCUSED);
        }

    }
}
