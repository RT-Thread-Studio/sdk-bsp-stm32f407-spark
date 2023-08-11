/*
 * MIT License
 *
 * Copyright (c) 2023 Dozingfiretruck
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <math.h>
#include "nes.h"
#include "drv_lcd.h"
#include "rt_key_scan.h"

/* memory */
void *nes_malloc(int num){
    // return SDL_malloc(num);
    return rt_malloc(num);
}


void nes_free(void *address){
    return rt_free(address);
}

void *nes_memcpy(void *str1, const void *str2, size_t n){
    return rt_memcpy(str1,str2,n);
}

void *nes_memset(void *str, int c, size_t n){
    return rt_memset(str,c,n);
}

int nes_memcmp(const void *str1, const void *str2, size_t n){
    return rt_memcmp(str1,str2,n);
}

#if (NES_USE_FS == 1)
/* io */
FILE *nes_fopen( const char * filename, const char * mode ){
    return fopen(filename,mode);
}

size_t nes_fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file){
    return fread(ptr, size_of_elements, number_of_elements,a_file);
}

int nes_fseek(FILE *stream, long int offset, int whence){
    return fseek(stream,offset,whence);
}

int nes_fclose( FILE *fp ){
    return fclose(fp);
}
#endif

/* wait */
void nes_wait(uint32_t ms){
    // SDL_Delay(ms);
    rt_thread_delay(ms);
}

/* Get a joypad state */
extern nes_t static_nes;
static void update_joypad()
{
    rt_uint32_t key_data = 0;
    static_nes.nes_cpu.joypad.joypad = 0;
    if(rt_event_recv(key_event,TP_UP | TP_DOWN | TP_LEFT | TP_RIGHT | TP_SELECT \
    | TP_START | TP_A | TP_B,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_NO,\
    &key_data) == RT_EOK)
    {
        if(key_data&TP_UP)		static_nes.nes_cpu.joypad.U1 = 1;
        else                    static_nes.nes_cpu.joypad.U1 = 0;
        if(key_data&TP_DOWN)	static_nes.nes_cpu.joypad.D1 = 1;
        else                    static_nes.nes_cpu.joypad.D1 = 0;
        if(key_data&TP_LEFT)	static_nes.nes_cpu.joypad.L1 = 1;
        else                    static_nes.nes_cpu.joypad.L1 = 0;
        if(key_data&TP_RIGHT)	static_nes.nes_cpu.joypad.R1 = 1;
        else                    static_nes.nes_cpu.joypad.R1 = 0;
        if(key_data&TP_SELECT)	static_nes.nes_cpu.joypad.SE1 = 1;
        else                    static_nes.nes_cpu.joypad.SE1 = 0;
        if(key_data&TP_START)	static_nes.nes_cpu.joypad.ST1 = 1;
        else                    static_nes.nes_cpu.joypad.ST1 = 0;
        if(key_data&TP_A)		static_nes.nes_cpu.joypad.A1 = 1;
        else                    static_nes.nes_cpu.joypad.A1 = 0;
        if(key_data&TP_B)		static_nes.nes_cpu.joypad.B1 = 1;
        else                    static_nes.nes_cpu.joypad.B1 = 0;

#if     DEBUG_PRINTF
        if(key_data&TP_UP) nes_printf("\nreceive TP_UP");
        if(key_data&TP_DOWN) nes_printf("\nreceive TP_DOWN");
        if(key_data&TP_LEFT) nes_printf("\nreceive TP_LEFT");
        if(key_data&TP_RIGHT) nes_printf("\nreceive TP_RIGHT");
        if(key_data&TP_SELECT) nes_printf("\nreceive TP_SELECT");
        if(key_data&TP_START) nes_printf("\nreceive TP_START");
        if(key_data&TP_A) nes_printf("\nreceive TP_A");
        if(key_data&TP_B) nes_printf("\nreceive TP_B");
#endif  //debug
    }
}


#define FREQ 44100
#define SAMPLES 2048
static const double SoundFreq = 261.63;
static const double TimeLag = 1.0 / FREQ;
static int g_callbackIndex = 0;

static void AudioCallback(void* userdata, uint8_t* stream, int len) {
    int16_t* source = (int16_t*)stream;
    int count = len / 2;
    double r = 0.0;
    int startIndex = (g_callbackIndex * count) % (int)(FREQ/SoundFreq*10);
    for (int i = 0; i < count; ++i) {
        r = M_PI * 2.0 * SoundFreq * TimeLag * (startIndex + i);
        source[i] = INT16_MAX * sin(r);
    }
    g_callbackIndex++;
}

int nes_initex(nes_t *nes){
    key_scan_init();
    return 0;
}

int nes_deinitex(nes_t *nes){

    return 0;
}

int nes_draw(size_t x1, size_t y1, size_t x2, size_t y2, nes_color_t* color_data){
    
    extern void lcd_fill_array(rt_uint16_t x_start, rt_uint16_t y_start, rt_uint16_t x_end, rt_uint16_t y_end, void *pcolor);
    lcd_fill_array(x1, y1, x2, y2, color_data);
    return 0;
}

#define FRAMES_PER_SECOND   1000/60

void nes_frame(nes_t* nes){
    nes_wait(FRAMES_PER_SECOND);
    update_joypad();
}

