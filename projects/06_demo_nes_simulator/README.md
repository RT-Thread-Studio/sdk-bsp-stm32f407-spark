# nes 模拟器实验

## 简介

本例程主要介绍了在星火 1 号开发板上移植 nes 模拟器的方法。

## 硬件说明

本例程使用到了 LCD 显示 nes 模拟器。使用了按键作为 nes 模拟器的输入设备。

## 软件说明

本例程的源码位于 `/projects/06_demo_nes_simulator`。nes 的移植代码主要在 application/NES 文件夹下。

其中 application/NES/InfoNES_system.c 为移植的内容

```c
void InfoNES_LoadLine()
{

  for(int i =0;i<NES_DISP_WIDTH;i++)
  {
    WorkLine[i] = change_byte_order(WorkLine[i]);
  }

  lcd_address_set(0, PPU_Scanline, (LCD_W - 1), PPU_Scanline);
  lcd_write_data_buffer(&WorkLine[0],LCD_W * sizeof(WorkLine[0]));

}
```

上面的函数实现了屏幕显示的刷新和 FPS 计算。 可以使用 PPU 控制台来计算 FPS，但是这需要硬件资源。

```c
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem )
{
  rt_uint32_t key_data = 0;
  *pdwPad1=0;
  
  if(rt_event_recv(key_event,TP_UP | TP_DOWN | TP_LEFT | TP_RIGHT | TP_SELECT \
  | TP_START | TP_A | TP_B,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_NO,\
  &key_data) == RT_EOK)
  {
    if(key_data&TP_UP)        *pdwPad1|=PAD_JOY_UP;
    if(key_data&TP_DOWN)    *pdwPad1|=PAD_JOY_DOWN;
    if(key_data&TP_LEFT)    *pdwPad1|=PAD_JOY_LEFT;
    if(key_data&TP_RIGHT)    *pdwPad1|=PAD_JOY_RIGHT;
    if(key_data&TP_SELECT)    *pdwPad1|=PAD_JOY_SELECT;
    if(key_data&TP_START)    {*pdwPad1|=PAD_JOY_START;}
    if(key_data&TP_A)        *pdwPad1|=PAD_JOY_A;
    if(key_data&TP_B)        *pdwPad1|=PAD_JOY_B;

    if(key_data&TP_UP) rt_kprintf("\nreceive TP_UP");
    if(key_data&TP_DOWN) rt_kprintf("\nreceive TP_DOWN");
    if(key_data&TP_LEFT) rt_kprintf("\nreceive TP_LEFT");
    if(key_data&TP_RIGHT) rt_kprintf("\nreceive TP_RIGHT");
    if(key_data&TP_SELECT) rt_kprintf("\nreceive TP_SELECT");
    if(key_data&TP_START) rt_kprintf("\nreceive TP_START");
    if(key_data&TP_A) rt_kprintf("\nreceive TP_A");
    if(key_data&TP_B) rt_kprintf("\nreceive TP_B");
  }
}
```

上面函数实现了接收从按键线程发送过来的按键事件。

application/NES/rt_key_scan.c 为按键捕获函数

```c
/* 配置 KEY 输入引脚  */
#define PIN_KEY0        GET_PIN(C, 0)     // PC0:  KEY0         --> KEY
#define PIN_KEY1        GET_PIN(C, 1)     // PC1 :  KEY1         --> KEY
#define PIN_KEY2        GET_PIN(C, 4)     // PC4 :  KEY2         --> KEY
#define PIN_WK_UP       GET_PIN(C, 5)     // PC5:  WK_UP        --> KEY

rt_event_t key_event;

void key_scan()
{
    uint32_t key_data;
    while(1)
    {
        rt_thread_mdelay(20); //10ms tick wait for input change.
        key_data = 0;
        if (rt_pin_read(PIN_KEY0) == PIN_LOW)
        {
            rt_kprintf("KEY0 pressed!\n");
            key_data |= TP_LEFT;
        }
        if (rt_pin_read(PIN_KEY1) == PIN_LOW)
        {
            rt_kprintf("KEY1 pressed!\n");
            key_data |= TP_START;
        }
        if (rt_pin_read(PIN_KEY2) == PIN_LOW)
        {
            rt_kprintf("KEY2 pressed!\n");
            key_data |= TP_RIGHT;
        }
        if (rt_pin_read(PIN_WK_UP) == PIN_LOW)
        {
            rt_kprintf("WK_UP pressed!\n");
            key_data |= TP_A;
        }
        if(key_data)
        {
            rt_event_send(key_event,key_data);
        }
    }

}
void key_scan_init()
{

    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_WK_UP, PIN_MODE_INPUT_PULLUP);

    key_event = rt_event_create("key", RT_IPC_FLAG_FIFO);

    rt_thread_t thread = rt_thread_create("key scan",key_scan,RT_NULL,1024,20,1);
    if(thread == RT_NULL) rt_kprintf("key scan thread create failed!\n");
    rt_thread_startup(thread);
}
INIT_APP_EXPORT(key_scan_init);
```

此文件初始化了一个按键扫描线程，一旦捕获到按键按下就会发送按键事件。

```c
int main(void)
{
    unsigned int count = 1;

    rt_pin_mode(PIN_LED_R, PIN_MODE_OUTPUT);

    if(InfoNES_Load(NULL)==0)
    {
        rt_thread_t nes_thread = rt_thread_create("nes",InfoNES_Main,RT_NULL,40960*2,21,10000);
        if(nes_thread == RT_NULL) rt_kprintf("nes thread create failed!\n");
        rt_thread_startup(nes_thread);
        LOG_D("nes start");

    }else{
        LOG_E("nes error");
    }

    while (count > 0)
    {
        rt_pin_write(PIN_LED_R, PIN_LOW);
        rt_thread_mdelay(500);

        rt_pin_write(PIN_LED_R, PIN_HIGH);
        rt_thread_mdelay(500);

        count++;

    }

    return 0;
}
```
主函数完成的主要功能为创建游戏机线程。然后周期点亮 led 提示系统运行状态。

## 运行

### 编译 & 下载

- RT-Thread Studio：在 RT-Thread Studio 的包管理器中下载 `STM32F407-RT-SPARK` 资源包，然后创建新工程，执行编译。
- MDK：首先双击 mklinks.bat，生成 rt-thread 与 libraries 文件夹链接；再使用 Env 生成 MDK5 工程；最后双击 project.uvprojx 打开 MDK5 工程，执行编译。

编译完成后，将开发板的 ST-Link USB 口与 PC 机连接，然后将固件下载至开发板。

### 运行效果

按下复位按键重启开发板，可以观察到游戏已经启动。按下按键，模拟器均有相应反馈。

![nes 模拟器](figures/nes.png)

## 注意事项

暂无

## 引用参考

- 文档中心：[RT-Thread 文档中心](https://www.rt-thread.org/document/site/#/)
