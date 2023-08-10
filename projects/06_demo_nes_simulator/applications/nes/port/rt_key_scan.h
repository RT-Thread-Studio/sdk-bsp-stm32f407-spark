#ifndef _KEY_SCAN
#define _KEY_SCAN

#include <rtthread.h>



#define TP_UP           0x01
#define TP_DOWN         0x02
#define TP_LEFT         0x04
#define TP_RIGHT        0x08
#define TP_SELECT       0x10
#define TP_START        0x20
#define TP_A            0x40
#define TP_B            0x80

#define DEBUG_PRINTF    0
void key_scan_init();
extern rt_event_t key_event;

#endif