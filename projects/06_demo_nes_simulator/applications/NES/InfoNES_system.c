/*-------------------------------------------------------------------*/
/*  InfoNES_system.c                                                 */
/*-------------------------------------------------------------------*/

#include "InfoNES.h"
#include <string.h>
#include <stdio.h>

#include "drv_lcd.h"
#include <rt_key_scan.h>
// #include "led.h"
// #include "key.h"
// #include "delay.h"
// #include "timer.h"
// #include "touch.h"
/*-------------------------------------------------------------------*/
/*  Palette data                                                     */
/*-------------------------------------------------------------------*/
WORD NesPalette[64]={
#if 0
  0x738E,0x88C4,0xA800,0x9808,0x7011,0x1015,0x0014,0x004F,
  0x0148,0x0200,0x0280,0x11C0,0x59C3,0x0000,0x0000,0x0000,
  0xBDD7,0xEB80,0xE9C4,0xF010,0xB817,0x581C,0x015B,0x0A59,
  0x0391,0x0480,0x0540,0x3C80,0x8C00,0x0000,0x0000,0x0000,
  0xFFDF,0xFDC7,0xFC8B,0xFC48,0xFBDE,0xB39F,0x639F,0x3CDF,
  0x3DDE,0x1690,0x4EC9,0x9FCB,0xDF40,0x0000,0x0000,0x0000,
  0xFFDF,0xFF15,0xFE98,0xFE5A,0xFE1F,0xDE1F,0xB5DF,0xAEDF,
  0xA71F,0xA7DC,0xBF95,0xCFD6,0xF7D3,0x0000,0x0000,0x0000,
#else 
  0x738E,0x20D1,0x0015,0x4013,0x880E,0xA802,0xA000,0x7840,
  0x4140,0x0200,0x0280,0x01C2,0x19CB,0x0000,0x0000,0x0000,
  0xBDD7,0x039D,0x21DD,0x801E,0xB817,0xE00B,0xD940,0xCA41,
  0x8B80,0x0480,0x0540,0x0487,0x0411,0x0000,0x0000,0x0000,
  0xFFDF,0x3DDF,0x5C9F,0x445F,0xF3DF,0xFB96,0xFB8C,0xFCC7,
  0xF5C7,0x8682,0x4EC9,0x5FD3,0x075B,0x0000,0x0000,0x0000,
  0xFFDF,0xAF1F,0xC69F,0xD65F,0xFE1F,0xFE1B,0xFDD6,0xFED5,
  0xFF14,0xE7D4,0xAF97,0xB7D9,0x9FDE,0x0000,0x0000,0x0000,	
#endif		
};

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

/* Menu screen */
int InfoNES_Menu()
{
	return 0;
}

/* Read ROM image file */
extern const BYTE nes_rom[];
int InfoNES_ReadRom( const char *pszFileName )
{
/*
 *  Read ROM image file
 *
 *  Parameters
 *    const char *pszFileName          (Read)
 *
 *  Return values
 *     0 : Normally
 *    -1 : Error
 */


  /* Read ROM Header */
  BYTE * rom = (BYTE*)nes_rom;
  memcpy( &NesHeader, rom, sizeof(NesHeader));
  if ( memcmp( NesHeader.byID, "NES\x1a", 4 ) != 0 )
  {
    /* not .nes file */
    return -1;
  }
  rom += sizeof(NesHeader);

  /* Clear SRAM */
  memset( SRAM, 0, SRAM_SIZE );

  /* If trainer presents Read Triner at 0x7000-0x71ff */
  if ( NesHeader.byInfo1 & 4 )
  {
    //memcpy( &SRAM[ 0x1000 ], rom, 512);
	rom += 512;
  }

  /* Allocate Memory for ROM Image */
  ROM = rom;
  rom += NesHeader.byRomSize * 0x4000;

  if ( NesHeader.byVRomSize > 0 )
  {
    /* Allocate Memory for VROM Image */
	VROM = (BYTE*)rom;
	rom += NesHeader.byVRomSize * 0x2000;
  }

  /* Successful */
  return 0;
}

/* Release a memory for ROM */
void InfoNES_ReleaseRom()
{
}

/* Transfer the contents of work frame on the screen */
extern 
void InfoNES_LoadFrame()
{

}

/* Calculate refresh frame rate */
rt_uint16_t Calculate_frate()
{
  
  static rt_uint16_t frame = 0;
  static rt_uint16_t last_time = 0;
  static rt_uint16_t now_time = 0;
  if(PPU_Scanline ==0)
  {
    last_time = now_time;
    now_time = rt_tick_get();

    rt_uint16_t err_time = now_time - last_time;
    frame = 1000 / err_time;
    rt_kprintf("%d\n",frame);
  }
  return frame;
}

/* Transfer the contents of work line on the screen */
void InfoNES_LoadLine()
{
  
  for(int i =0;i<NES_DISP_WIDTH;i++)
  {
    WorkLine[i] = change_byte_order(WorkLine[i]);
  }

  lcd_address_set(0, PPU_Scanline, (LCD_W - 1), PPU_Scanline);
  lcd_write_data_buffer(&WorkLine[0],LCD_W * sizeof(WorkLine[0]));
  
  rt_uint8_t fps_buf[10];
  rt_uint16_t fps = Calculate_frate();
  // rt_sprintf(fps_buf,"%d",fps);
  // lcd_show_string(10, 200, 16, fps_buf);
}


/* Get a joypad state */
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem )
{
  rt_uint32_t key_data = 0;
	*pdwPad1=0;
  if(rt_event_recv(key_event,TP_UP | TP_DOWN | TP_LEFT | TP_RIGHT | TP_SELECT \
  | TP_START | TP_A | TP_B,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_NO,\
  &key_data) == RT_EOK)
  {
    if(key_data&TP_UP)		*pdwPad1|=PAD_JOY_UP;
		if(key_data&TP_DOWN)	*pdwPad1|=PAD_JOY_DOWN;
		if(key_data&TP_LEFT)	*pdwPad1|=PAD_JOY_LEFT;
		if(key_data&TP_RIGHT)	*pdwPad1|=PAD_JOY_RIGHT;
		if(key_data&TP_SELECT)	*pdwPad1|=PAD_JOY_SELECT;
		if(key_data&TP_START)	{*pdwPad1|=PAD_JOY_START;}
		if(key_data&TP_A)		*pdwPad1|=PAD_JOY_A;
		if(key_data&TP_B)		*pdwPad1|=PAD_JOY_B;

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

/* memcpy */
void *InfoNES_MemoryCopy( void *dest, const void *src, int count ){return memcpy(dest,src,count);}

/* memset */
void *InfoNES_MemorySet( void  *dest, int c, int count ){return memset(dest,c,count);}

/* Print debug message */
void InfoNES_DebugPrint( char *pszMsg )
{
}

/* Wait */
void InfoNES_Wait()
{
}

/* Sound Initialize */
void InfoNES_SoundInit( void )
{
}

/* Sound Open */
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate )
{
  return 0;
}

/* Sound Close */
void InfoNES_SoundClose( void )
{
}

/* Sound Output 5 Waves - 2 Pulse, 1 Triangle, 1 Noise, 1 DPCM */
void InfoNES_SoundOutput(int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4, BYTE *wave5)
{
}

/* Print system message */
void InfoNES_MessageBox( char *pszMsg, ... )
{
}

