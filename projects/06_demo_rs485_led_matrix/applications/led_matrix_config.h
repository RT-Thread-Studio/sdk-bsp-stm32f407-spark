#ifndef _LED_MATRIX_CONFIG
#define _LED_MATRIX_CONFIG
#include <drv_matrix_led.h>
/* define 外部 LED  */
enum{
    EXTERN_LED_0,
    EXTERN_LED_1,
    EXTERN_LED_2,
    EXTERN_LED_3,
    EXTERN_LED_4,
    EXTERN_LED_5,
    EXTERN_LED_6,
    EXTERN_LED_7,
    EXTERN_LED_8,
    EXTERN_LED_9,
    EXTERN_LED_10,
    EXTERN_LED_11,
    EXTERN_LED_12,
    EXTERN_LED_13,
    EXTERN_LED_14,
    EXTERN_LED_15,
    EXTERN_LED_16,
    EXTERN_LED_17,
    EXTERN_LED_18,
};

/* define led location */
enum{
    INTERNAL,
    EXTERNAL
};

typedef struct LED_NODE
{
    rt_uint8_t pin;
    pixel_rgb_t status;
    rt_uint8_t location;
    rt_uint8_t reverse;
    void (*io_ctl)(struct LED_NODE *node, pixel_rgb_t color);
}rt_led_node_t;



#endif