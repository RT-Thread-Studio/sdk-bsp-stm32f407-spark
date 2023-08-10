
#include "../DisplayPrivate.h"
#include "icm20608.h"
#include "aht10.h"
#include "ap3216c.h"
#include <dfs_file.h>
#include <unistd.h>
#include <stdio.h> /* rename() */
#include <sys/stat.h>
#include <sys/statfs.h> /* statfs() */

//#include <spi_wifi_rw007.h>

#include "spi_flash_sfud.h"
/*导出页面*/
PAGE_EXPORT(HWTest);

#define	TEST_STATE_WITE  LV_SYMBOL_MINUS
#define TEST_STATE_TESTING LV_SYMBOL_REFRESH
#define TEST_STATE_PASS LV_SYMBOL_OK
#define TEST_STATE_FAIL LV_SYMBOL_CLOSE

#define PIN_BEEP        GET_PIN(B, 0)     // PA1:  BEEP         --> BEEP (PB1)
#define PIN_IR_SEND     GET_PIN(B, 1)
#define PIN_IR_RECV     GET_PIN(F, 8)
extern void led_matrix_test3();
extern icm20608_device_t icm20608_init(const char *i2c_bus_name);
extern void icm20608_deinit(icm20608_device_t dev);
extern rt_err_t icm20608_get_accel(icm20608_device_t dev, rt_int16_t *accel_x, rt_int16_t *accel_y, rt_int16_t *accel_z);

static void* beep_test(void);
static void* BackLight_test(void);
static void* RW007_test(void);
static void* AHT21_test(void);
static void* RGB_LED_test(void);
static void* AP3216C_test(void);
static void* ICM20608_test(void);
static void* SDCard_test(void);
static void* W25Q128_test(void);
static void* ES8388_test(void);
static void* RS485_test(void);
static void* CAN_test(void);
static void* IRM_H638T_test(void);
static void* IR_LED_test(void);
	
typedef struct
{
	const char* name;
    const uint8_t pageID;
	void* (*callback)(void);
	void* state;
    lv_obj_t* obj;
}TestBTN_t;

static TestBTN_t Test_BTNs[]=
{
	{"BEEP",		PAGE_SysInfo,	beep_test,		TEST_STATE_WITE, NULL},
	{"SDCard", 		PAGE_BackLight, SDCard_test, 	TEST_STATE_WITE, NULL},
//	{"RW007", 		PAGE_TimeSet, 	RW007_test, 	TEST_STATE_WITE, NULL},
	{"AHT21", 		PAGE_SysInfo, 	AHT21_test, 	TEST_STATE_WITE, NULL},
	{"AP3216C IR", 	PAGE_TimeSet, 	AP3216C_test, 	TEST_STATE_WITE, NULL},
	{"ICM20608", 	PAGE_SysInfo, 	ICM20608_test, 	TEST_STATE_WITE, NULL},
//	{"W25Q128", 	PAGE_TimeSet, 	W25Q128_test, 	TEST_STATE_WITE, NULL},
	{"RGB LED", 	PAGE_BackLight, RGB_LED_test, 	TEST_STATE_WITE, NULL},
//	{"ES8388", 		PAGE_SysInfo, 	ES8388_test, 	TEST_STATE_WITE, NULL},
//	{"RS485", 		PAGE_SysInfo, 	RS485_test, 	TEST_STATE_WITE, NULL},
//	{"CAN", 		PAGE_SysInfo, 	CAN_test, 		TEST_STATE_WITE, NULL},
//	{"IRM-H638T", 	PAGE_SysInfo, 	IRM_H638T_test, TEST_STATE_WITE, NULL},
};


static lv_anim_timeline_t* anim_timeline = NULL;
static lv_obj_t* Cont;
static lv_obj_t* Title;
static lv_obj_t* list1;

void beep_ctrl(rt_uint8_t on)
{
    if (on)
    {
        rt_pin_write(PIN_BEEP, PIN_HIGH);
    }
    else
    {
        rt_pin_write(PIN_BEEP, PIN_LOW);
    }
}

static void* beep_test(void)
{
	rt_pin_mode(PIN_BEEP,PIN_MODE_OUTPUT);
	beep_ctrl(1);
	rt_thread_mdelay(300);
	beep_ctrl(0);
	return TEST_STATE_PASS;
}

static void* RW007_test(void)
{
//	extern int wifi_spi_device_init(void);
//	wifi_spi_device_init();
//	char sn_version[32];
//	if(rw007_sn_get(sn_version)!= RT_EOK)
//	{
//		rt_hw_wifi_deinit();
//		return TEST_STATE_FAIL;
//	}
//	rt_hw_wifi_deinit();
	return TEST_STATE_PASS;
}

static void* AHT21_test(void)
{
	float humidity, temperature;
    aht10_device_t dev;
    const char *i2c_bus_name = "i2c3";
    dev = aht10_init(i2c_bus_name);
	
	humidity = aht10_read_humidity(dev);
	rt_kprintf("humidity   : %d.%d %%\n", (int)humidity, (int)(humidity * 10) % 10);

	temperature = aht10_read_temperature(dev);
	rt_kprintf("temperature: %d.%d\n", (int)temperature, (int)(temperature * 10) % 10);
	aht10_deinit(dev);
	return TEST_STATE_PASS;
}

static void* RGB_LED_test(void)
{
	extern void led_matrix_test3();
	led_matrix_test3();
	return TEST_STATE_PASS;
}

static void* AP3216C_test(void)
{
	ap3216c_device_t dev;
	dev = ap3216c_init("i2c2");
	float light = ap3216c_read_ambient_light(dev);
	rt_kprintf("ap3216c_read_ambient_light %d.%d\n", (int)light, (int)(light * 10) % 10);
	if(light <= 0)
	{
		ap3216c_deinit(dev);
		return TEST_STATE_FAIL;
	}

	ap3216c_deinit(dev);
	return TEST_STATE_PASS;
}

static void* ICM20608_test(void)
{
	rt_int16_t a_x = 0;
	rt_int16_t a_y = 0;
	rt_int16_t a_z = 0;
	icm20608_device_t dev = icm20608_init("i2c2");
	if(dev==RT_NULL)
	{
		icm20608_deinit(dev);
		return TEST_STATE_FAIL;
	}
	rt_err_t ret = icm20608_get_accel(dev, &a_x, &a_y, &a_z);
	if(ret != RT_EOK)
	{
		icm20608_deinit(dev);
		return TEST_STATE_FAIL;
	}
	rt_kprintf("accel_x %d, accel_y %d, accel_z %d\n", a_x, a_y, a_z);
	icm20608_deinit(dev);
	return TEST_STATE_PASS;
	
}

static void* SDCard_test(void)
{
	int fd, size;
    char s[] = "RT-Thread Programmer!", buffer[80];

    rt_kprintf("Write string %s to test.txt.\n", s);

    /* 以创建和读写模式打开 /text.txt 文件，如果该文件不存在则创建该文件 */
    fd = open("/sdcard/text.txt", O_WRONLY | O_CREAT);
    if (fd>= 0)
    {
        write(fd, s, sizeof(s));
        close(fd);
        rt_kprintf("Write done.\n");
    }
	else
		return TEST_STATE_FAIL;

      /* 以只读模式打开 /text.txt 文件 */
    fd = open("/sdcard/text.txt", O_RDONLY);
    if (fd>= 0)
    {
        size = read(fd, buffer, sizeof(buffer));
        close(fd);
        rt_kprintf("Read from file test.txt : %s \n", buffer);
        if (size < 0)
            return TEST_STATE_FAIL;
    }
	else
		return TEST_STATE_FAIL;
	return TEST_STATE_PASS;
}

static void* W25Q128_test(void)
{
//	extern int onboard_fal_mount(void);
	extern int rt_hw_spi_flash_init(void);
	rt_err_t ret = RT_EOK;
	ret = rt_hw_spi_flash_init();
	rt_kprintf("rt_hw_spi_flash_init %d",ret);
//	ret = onboard_fal_mount();
	rt_kprintf("onboard_spiflash_mount %d",ret);
	rt_spi_flash_device_t  rtt_dev = (rt_spi_flash_device_t) rt_device_find("W25Q128");
	ret = rt_sfud_flash_delete(rtt_dev);
	rt_kprintf("rt_sfud_flash_delete %d",ret);
	if(ret == RT_EOK)
		return TEST_STATE_PASS;
	else
		return TEST_STATE_FAIL;
}

static void* ES8388_test(void)
{
	extern int wavplayer_play(char *uri);
	if(RT_EOK == wavplayer_play("/rom/3.wav"))
	{
		return TEST_STATE_PASS;
	}
	return TEST_STATE_FAIL;
}

static void* RS485_test(void)
{
	return TEST_STATE_FAIL;
}

static void* CAN_test(void)
{
	return TEST_STATE_FAIL;
}

static void* IRM_H638T_test(void)
{
	rt_pin_mode(PIN_IR_SEND,PIN_MODE_OUTPUT);
	rt_pin_mode(PIN_IR_RECV,PIN_MODE_INPUT_PULLUP);
	rt_pin_write(PIN_IR_SEND, PIN_LOW);
	rt_thread_mdelay(100);
	if(rt_pin_read(PIN_IR_RECV)!=PIN_HIGH)
		return TEST_STATE_FAIL;
	
	rt_pin_write(PIN_IR_SEND, PIN_LOW);
	return TEST_STATE_PASS;
}

static void* IR_LED_test(void)
{
	return TEST_STATE_FAIL;
}

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
    for (int i = 0; i < __Sizeof(Test_BTNs); i++)
    {
        lv_obj_t* btn;
        btn = lv_list_add_btn(list, TEST_STATE_WITE, Test_BTNs[i].name);
        lv_obj_add_style(btn, &style, LV_PART_MAIN);
        lv_obj_set_style_border_width(btn, 2, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_height(btn, 40);
        lv_obj_set_style_bg_color(btn, lv_color_make(0, 0x40, 0xDD), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_text_font(btn, &lv_font_montserrat_28, LV_PART_MAIN);
        Test_BTNs[i].obj = btn;
    }
	list1 = list;
}

void set_test_state(TestBTN_t* test, void* state)
{
	test->state = state;
	lv_obj_t* obj = test->obj;
	lv_obj_t* img = lv_obj_get_child(obj, 0);
	lv_img_set_src(img, state);
}

void run_tests(void)
{
	int testpassed_cont = 0;
	for (int i = 0; i < __Sizeof(Test_BTNs); i++)
    {
		set_test_state(&Test_BTNs[i], TEST_STATE_TESTING);
		void* ret = NULL;
		ret = Test_BTNs[i].callback();
		if(strcmp(ret, TEST_STATE_PASS) != 0)
			set_test_state(&Test_BTNs[i], TEST_STATE_FAIL);
		else
		{
			set_test_state(&Test_BTNs[i], TEST_STATE_PASS);
			testpassed_cont++;
		}
		lv_group_set_editing(appWindow_g, false);
		lv_group_focus_next(appWindow_g);
		PageDelay(100);
	}
	if(testpassed_cont == __Sizeof(Test_BTNs))
	{
		for (int i = 0; i < __Sizeof(Test_BTNs); i++)
		{
			lv_obj_set_style_bg_color(Test_BTNs[i].obj, lv_color_hex(0x33cc66), LV_PART_MAIN);
		}
	}
}

static void Setup()
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);
	Cont = Cont_Create(appWindow);
    Title = Title_Create(appWindow, "HWTest");
	List_Creat(Cont);
    anim_timeline = PageAnim_Create(Cont, Title);
    PageAnim_Play(anim_timeline, false);
	
	run_tests();
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
			LV_LOG("Page->Pop()\n");
            PM_Pop(Page);
		}
		
	}
}
