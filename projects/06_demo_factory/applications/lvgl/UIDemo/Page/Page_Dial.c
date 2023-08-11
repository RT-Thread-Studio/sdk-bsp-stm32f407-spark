#include "../DisplayPrivate.h"
#include "aht10.h"
PAGE_EXPORT(Dial);

LV_IMG_DECLARE(IMG_Heart);
LV_IMG_DECLARE(IMG_Power);
LV_IMG_DECLARE(IMG_CHNSecond);

typedef struct{
    lv_obj_t * label_1;
    lv_obj_t * label_2;
    lv_anim_t anim_now;
    lv_anim_t anim_next; 
    lv_coord_t y_offset;
    uint8_t value_last;
    lv_coord_t x_offset;
}lv_label_time_effect_t;


static lv_obj_t* contBatt;
static lv_obj_t* ledBattGrp[10];

static lv_obj_t* contDate;
static lv_obj_t* labelDate;
static lv_obj_t* labelWeek;

static lv_obj_t* contTime;
static lv_label_time_effect_t labelTimeEffect[4];

static lv_obj_t* imgPower;
static lv_obj_t* ledSecGrp[2];

static lv_obj_t* contHeartRate;
static lv_obj_t* labelHeartRate;

static lv_obj_t* contSteps;
static lv_obj_t* labelSteps;

static lv_obj_t* imgCHN;

static Clock_Value_t Clock;

static lv_label_time_effect_t labelTimeEffect[4];
static lv_obj_t* ledSecGrp[2];
static lv_obj_t* Cont;
static lv_obj_t* contTime;
static lv_obj_t* labelDate;
static lv_obj_t* labelWeather;
static lv_anim_timeline_t* anim_timeline = NULL;

static rt_mutex_t dynamic_mutex = RT_NULL;
static rt_thread_t dial_thread = RT_NULL;
static aht10_device_t aht10_dev;

static void lv_label_time_effect_check_value(lv_label_time_effect_t * effect, uint8_t value);
static void lv_label_time_effect_init(
    lv_label_time_effect_t* effect,
    lv_obj_t* cont,
    lv_obj_t* label_copy,
    uint16_t anim_time,
    lv_coord_t x_offset
);
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

static void ContBatt_UpdateBattUsage(uint8_t usage)
{
    int8_t maxIndexTarget = __Map(usage, 0, 100, 0, __Sizeof(ledBattGrp));

    for (int i = 0; i < __Sizeof(ledBattGrp); i++)
    {
        lv_obj_t* led = ledBattGrp[i];

        (i < maxIndexTarget) ? lv_led_on(led) : lv_led_off(led);
    }
}

static void ContBatt_AnimCallback(void* obj, int16_t usage)
{
    ContBatt_UpdateBattUsage(usage);
}

static void ContBatt_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, 222, 20);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 5);
    //lv_obj_set_style_border_width(cont, 2, LV_PART_MAIN);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    contBatt = cont;

    lv_obj_t* img = lv_img_create(cont);
    lv_img_set_src(img, &IMG_Power);
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 0, 0);
    imgPower = img;

    const lv_coord_t led_w = (lv_obj_get_width(contBatt) - lv_obj_get_width(img)) / 10 - 2;
    const lv_coord_t led_h = lv_obj_get_height(contBatt);

    for (int i = 0; i < __Sizeof(ledBattGrp); i++)
    {
        lv_obj_t* led = lv_led_create(contBatt);
        lv_obj_set_size(led, 14, 20);
        lv_obj_set_style_radius(led, 0, LV_PART_MAIN);
        lv_obj_set_style_border_width(led, 0, LV_PART_MAIN);
        //lv_obj_set_style_bg_color(led, LV_COLOR_RED, LV_PART_MAIN);
        lv_led_set_color(led, LV_COLOR_RED);
        lv_obj_set_style_shadow_width(led, 0, LV_PART_MAIN);
        if(i==0)
            lv_obj_align(led, LV_ALIGN_RIGHT_MID, 0, 0);
        else
            lv_obj_align_to(led, ledBattGrp[i - 1], LV_ALIGN_OUT_LEFT_MID, -2, 0);
        lv_led_off(led);
        ledBattGrp[i] = led;
    }
}

static void ContWeek_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, lv_obj_get_width(par) / 2, lv_obj_get_height(par));
    lv_obj_align(cont, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(cont, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, &Font_RexBold_28, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, LV_COLOR_BLACK, LV_PART_MAIN);
    lv_label_set_text(label, "SUN");
    lv_obj_align(label,LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_SCROLLABLE);
    labelWeek = label;
}

static void ContDate_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, 222, 32);
    lv_obj_align_to(cont, contBatt, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_style_border_width(cont, 2, LV_PART_MAIN);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    contDate = cont;

    lv_obj_t* label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, &Font_RexBold_28, LV_PART_MAIN);
    lv_label_set_text(label, "00.00.00");
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_SCROLLABLE);

    labelDate = label;

    ContWeek_Create(contDate);
}

static void LabelDate_Update(int year, int mon, int day, int week)
{
    lv_label_set_text_fmt(labelDate, "%02d.%02d.%02d", year % 100, mon, day);

    const char* week_str[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
    lv_label_set_text(labelWeek, week_str[week]);
}

static void LabelTime_Update(int h,int m)
{
    
    lv_label_time_effect_check_value(&labelTimeEffect[3], m % 10);
    
    lv_label_time_effect_check_value(&labelTimeEffect[2], m / 10);

    lv_label_time_effect_check_value(&labelTimeEffect[1], h % 10);
    
    lv_label_time_effect_check_value(&labelTimeEffect[0], h / 10);

    lv_led_toggle(ledSecGrp[0]);
    lv_led_toggle(ledSecGrp[1]);
}

static void LabelTime_Create(lv_obj_t* par)
{
    const lv_coord_t x_mod[4] = { -70, -30, 30, 70 };
    for (int i = 0; i < __Sizeof(labelTimeEffect); i++)
    {
        lv_obj_t* label = lv_label_create(par);
        lv_label_set_text(label, "0");
        lv_obj_align(label, LV_ALIGN_CENTER, x_mod[i], 0);
        //lv_obj_set_auto_realign(label, true);

        lv_label_time_effect_init(&labelTimeEffect[i], par, label, 500, x_mod[i]);
    }

    lv_obj_t* led = lv_led_create(par);
    lv_obj_set_size(led, 8, 8);
    //lv_obj_set_hidden(led, true);
    lv_obj_set_style_radius(led, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(led, 0, LV_PART_MAIN);
    lv_led_set_color(led, LV_COLOR_RED);
    lv_obj_set_style_shadow_width(led, 0, LV_PART_MAIN);
    lv_obj_align(led, LV_ALIGN_CENTER, 0, -15);
    ledSecGrp[0] = led;

    led = lv_led_create(par);
    lv_obj_set_size(led, 8, 8);
    //lv_obj_set_hidden(led, true);
    lv_obj_set_style_radius(led, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(led, 0, LV_PART_MAIN);
    lv_led_set_color(led, LV_COLOR_RED);
    lv_obj_set_style_shadow_width(led, 0, LV_PART_MAIN);
    lv_obj_align(led, LV_ALIGN_CENTER, 0, 15);
    ledSecGrp[1] = led;
}

static void ContTime_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, 222, 93);
    lv_obj_align_to(cont, contDate, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_style_border_color(cont, LV_COLOR_RED, LV_PART_MAIN);
    lv_obj_set_style_border_width(cont, 2, LV_PART_MAIN);
    lv_obj_set_style_text_font(cont, &Font_RexBold_89, LV_PART_MAIN);
    lv_obj_set_style_text_color(cont, LV_COLOR_RED, LV_PART_MAIN);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    contTime = cont;

    LabelTime_Create(contTime);
}

static void LabelHeartRate_Update()
{
    //lv_label_set_text_fmt(labelHeartRate, "%04.01f", ParticleSensor_GetBeats());
}

static void ContHeartRate_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, 150, 32);
    lv_obj_align_to(cont, contTime, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_set_style_border_width(cont, 2, LV_PART_MAIN);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    contHeartRate = cont;

    cont = lv_obj_create(contHeartRate);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, lv_obj_get_width(contHeartRate) / 3, lv_obj_get_height(contHeartRate));
    lv_obj_align(cont, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(cont, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_set_style_text_color(cont, LV_COLOR_BLACK, LV_PART_MAIN);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, &Font_RexBold_28, LV_PART_MAIN);
    lv_label_set_text(label, "TMP");
    lv_obj_align(label,LV_ALIGN_CENTER, 0, 0);

    label = lv_label_create(contHeartRate);
    lv_obj_set_style_text_font(label, &Font_RexBold_28, LV_PART_MAIN);
    lv_label_set_text(label, "00.0");
    lv_obj_align_to(label, cont, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    labelHeartRate = label;

    cont = lv_obj_create(contHeartRate);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, lv_obj_get_width(contHeartRate) / 4, lv_obj_get_height(contHeartRate));
    lv_obj_align(cont, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(cont, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* img = lv_img_create(cont);
    lv_img_set_src(img, &IMG_Heart);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, img);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
    lv_anim_set_values(&a, LV_IMG_ZOOM_NONE, 160);
    lv_anim_set_time(&a, 300);
    lv_anim_set_playback_time(&a, 300);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&a, 800);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);

    lv_anim_start(&a);
}

static void LabelSteps_Update()
{
    //lv_label_set_text_fmt(labelSteps, "%05d", IMU_GetSteps());
}

static void ContSteps_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, 150, 32);
    lv_obj_align_to(cont, contHeartRate, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(cont, 2, LV_PART_MAIN);
    contSteps = cont;

    cont = lv_obj_create(contSteps);
    lv_obj_add_style(cont, cont_style, LV_PART_MAIN);
    lv_obj_set_size(cont, lv_obj_get_width(contSteps) / 3, lv_obj_get_height(contSteps));
    lv_obj_align(cont, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(cont, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_set_style_text_color(cont, LV_COLOR_BLACK, LV_PART_MAIN);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, &Font_RexBold_28, LV_PART_MAIN);
    lv_label_set_text(label, "HUM");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    label = lv_label_create(contSteps);
    lv_obj_set_style_text_font(label, &Font_RexBold_28, LV_PART_MAIN);
    lv_label_set_text(label, "00000");
    lv_obj_align_to(label, cont, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    labelSteps = label;
}

static void ImgCHN_Create(lv_obj_t* par)
{
    lv_obj_t* img = lv_img_create(par);
    lv_img_set_src(img, &IMG_CHNSecond);
    lv_obj_align_to(img, contTime, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 5);
    imgCHN = img;
}
typedef struct lv_anim_timeline_info
{
    uint32_t start_time;
    lv_obj_t* obj;

    lv_anim_exec_xcb_t exec_cb;
    int32_t start;
    int32_t end;
    uint16_t duration;
    lv_anim_path_cb_t path_cb;

}lv_anim_timeline_info_t;

static void ObjsOpa_AnimCallback(void* obj, int16_t opa)
{
    lv_obj_set_style_opa(labelTimeEffect[0].label_1, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(labelTimeEffect[1].label_1, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(labelTimeEffect[2].label_1, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(labelTimeEffect[3].label_1, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(labelTimeEffect[0].label_2, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(labelTimeEffect[1].label_2, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(labelTimeEffect[2].label_2, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(labelTimeEffect[3].label_2, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(labelDate, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(labelHeartRate, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(labelSteps, opa, LV_PART_MAIN);
    lv_obj_set_style_opa(imgCHN, opa, LV_PART_MAIN);
}

static void PagePlayAnim_Create(void)
{
#define ANIM_WIDTH_DEF(start_time, obj) {start_time, obj, (lv_anim_exec_xcb_t)lv_obj_set_width, 0,  lv_obj_get_width(obj), 300, lv_anim_path_ease_out}
    lv_anim_timeline_info_t anim_timeline_info[] = {
        ANIM_WIDTH_DEF(0,   contBatt),
        ANIM_WIDTH_DEF(100, contDate),
        ANIM_WIDTH_DEF(200, contTime),
        ANIM_WIDTH_DEF(300, contHeartRate),
        ANIM_WIDTH_DEF(400, contSteps),

        {400, NULL, (lv_anim_exec_xcb_t)ContBatt_AnimCallback, 0, 100, 400, lv_anim_path_linear},
        {800, NULL, (lv_anim_exec_xcb_t)ObjsOpa_AnimCallback, LV_OPA_TRANSP, LV_OPA_COVER, 500, lv_anim_path_ease_out},

    };
    anim_timeline = lv_anim_timeline_create();
    for (int i = 0; i < __Sizeof(anim_timeline_info); i++)
    {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, anim_timeline_info[i].obj);
        lv_anim_set_values(&a, anim_timeline_info[i].start, anim_timeline_info[i].end);
        //lv_anim_set_early_apply(&a, false);
        lv_anim_set_exec_cb(&a, anim_timeline_info[i].exec_cb);
        lv_anim_set_path_cb(&a, anim_timeline_info[i].path_cb);
        lv_anim_set_time(&a, anim_timeline_info[i].duration);
        lv_anim_timeline_add(anim_timeline, anim_timeline_info[i].start_time, &a);
    }
}

static void PagePlayAnim(bool playback)
{
    ObjsOpa_AnimCallback(NULL, 0);
    lv_anim_timeline_set_reverse(anim_timeline, playback);
    lv_anim_timeline_start(anim_timeline);
    PageDelay(lv_anim_timeline_get_playtime(anim_timeline));
}



/****************************************************************************************/


void lv_label_time_effect_init(
    lv_label_time_effect_t* effect,
    lv_obj_t* cont,
    lv_obj_t* label_copy,
    uint16_t anim_time,
    lv_coord_t x_offset
)
{
    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, lv_label_get_text(label_copy));
    
    lv_obj_set_style_text_color(label, LV_COLOR_RED, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &Font_RexBold_89, LV_PART_MAIN);

    effect->y_offset = (lv_obj_get_height(cont) + 90) / 2 + 1;
    //LV_LOG_USER("cont_h %d, label_copy_h %d", lv_obj_get_height(cont), lv_obj_get_height(label_copy));
    lv_obj_align(label, LV_ALIGN_CENTER, x_offset, -effect->y_offset);
    //lv_obj_align(label, label_copy, LV_ALIGN_CENTER, 0, 0);
    effect->label_1 = label_copy;
    effect->label_2 = label;

    effect->value_last = 0;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_time(&a, anim_time);
    lv_anim_set_delay(&a, 0);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);

    effect->anim_now = a;
    effect->anim_next = a;
    //LV_LOG_USER("effect->y_offset [%d], effect->label_1 y [%d],effect->label_2 y [%d]",
//        effect->y_offset,lv_obj_get_y(effect->label_1),
//        lv_obj_get_y(effect->label_2));
}


static void lv_label_time_effect_check_value(lv_label_time_effect_t * effect, uint8_t value)
{
    if(value == effect->value_last)
        return;
    lv_obj_t * next_label;
    lv_obj_t * now_label;

    if(lv_obj_get_y(effect->label_2) > lv_obj_get_y(effect->label_1))
    {
        now_label = effect->label_1;
        next_label = effect->label_2;
        //LV_LOG_USER("effect->label_2 %d < effect->label_1 %d ", lv_obj_get_y(effect->label_2) ,lv_obj_get_y(effect->label_1));
    }
    else 
    {
        now_label = effect->label_2;
        next_label = effect->label_1;
        //LV_LOG_USER("effect->label_2 %d > effect->label_1 %d", lv_obj_get_y(effect->label_2), lv_obj_get_y(effect->label_1));
    }

    lv_label_set_text_fmt(now_label, "%d", effect->value_last);
    lv_label_set_text_fmt(next_label, "%d", value);
    effect->value_last = value;

    //lv_obj_align(next_label, LV_ALIGN_CENTER, 0, -effect->y_offset);

    lv_obj_set_y(next_label, -effect->y_offset);
    //lv_obj_align_to(next_label, now_label, LV_ALIGN_OUT_TOP_MID, 0, -effect->y_offset);

    lv_coord_t y_offset = abs(lv_obj_get_y(now_label) - lv_obj_get_y(next_label));

    lv_anim_t* a;
    a = &(effect->anim_now);
    lv_anim_set_var(a, now_label);
    lv_anim_set_values(a, 0, effect->y_offset);
    lv_anim_start(a);

    a = &(effect->anim_next);
    lv_anim_set_var(a, next_label);
    lv_anim_set_values(a, -effect->y_offset, 0);
    lv_anim_start(a);

    //LV_LOG_USER("y_offset [%d], now_label y [%d],next_label y [%d]",
//        y_offset, lv_obj_get_y(now_label),
//        lv_obj_get_y(next_label));
}
static void dial_update(void *parameter)
{
	struct tm *Time;
	time_t now;
	float humidity, temperature;
	while(1)
	{
		
		humidity = aht10_read_humidity(aht10_dev);
		lv_label_set_text_fmt(labelSteps, "%d.%d %%", (int)humidity, (int)(humidity * 10) % 10);
		temperature = aht10_read_temperature(aht10_dev);
		lv_label_set_text_fmt(labelHeartRate, "%d.%d", (int)temperature, (int)(temperature * 10) % 10);

		rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);
		now = time(RT_NULL) + 28800;
		Time = gmtime(&now);
		Clock.year = Time->tm_year+1900;
		Clock.month = Time->tm_mon+1;
		Clock.date = Time->tm_mday;
		Clock.week = Time->tm_wday;
		Clock.hour = Time->tm_hour;
		Clock.min = Time->tm_min;
		Clock.sec = Time->tm_sec;
		LabelDate_Update(Clock.year, Clock.month, Clock.date, Clock.week);
		LabelTime_Update(Clock.hour,Clock.min);
		rt_mutex_release(dynamic_mutex);
		rt_thread_mdelay(1000);
	}
}

static void date_thread_crearte(void)
{
	dynamic_mutex = rt_mutex_create("dmutex", RT_IPC_FLAG_PRIO);
	dial_thread = rt_thread_create("daterun", dial_update, RT_NULL, 2048, 25, 10);
    if (dial_thread != RT_NULL)
    {
        rt_thread_startup(dial_thread);
    }
    else
    {
        rt_kprintf("create weather_update failed!\n");
    }
}

static void Setup()
{
	rt_device_t device = RT_NULL;
    lv_obj_move_foreground(appWindow);
    ContStyle_Setup();
    ContBatt_Create(appWindow);
    ContDate_Create(appWindow);
    ContTime_Create(appWindow);

    ContHeartRate_Create(appWindow);

    ContSteps_Create(appWindow);

    ImgCHN_Create(appWindow);
    PagePlayAnim_Create();
    PagePlayAnim(false);
	
	PageDelay(100);
    device = rt_device_find("rtc");
	aht10_dev = aht10_init("i2c3");
    if (!device)
    {
		rt_kprintf("find %s failed!\n", "rtc");
    }
	else
	{
		if(rt_device_open(device, 0) != RT_EOK)
			rt_kprintf("open %s failed!\n", "rtc");
	}
	date_thread_crearte();
}

static void Exit()
{
	rt_thread_delete(dial_thread);
	rt_mutex_delete (dynamic_mutex);
	aht10_deinit(aht10_dev);
    PagePlayAnim(true);
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
		if(key == PAGE_KEY_RIGHT)
		{
			PM_Push(Page, PAGE_MainMenu);
		}
		
		if(key == PAGE_KEY_LEFT)
		{
			PM_Push(Page, PAGE_HWTest);
		}
		
	}
    if (code == LV_EVENT_PRESSING)
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if (dir == LV_DIR_RIGHT)
        {
            PM_Push(Page, PAGE_MainMenu);
        }
    }
}
