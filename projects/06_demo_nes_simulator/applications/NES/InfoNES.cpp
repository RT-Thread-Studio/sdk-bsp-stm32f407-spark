/*===================================================================*/
/*                                                                   */
/*  InfoNES.cpp : NES Emulator for Win32, Linux(x86), Linux(PS2)     */
/*                                                                   */
/*  2000/05/18  InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/
/*-------------------------------------------------------------------
 * File List :
 *
 * [NES Hardware]
 *   InfoNES.cpp
 *   InfoNES.h
 *   K6502_rw.h
 *
 * [Mapper function]
 *   InfoNES_Mapper.cpp
 *   InfoNES_Mapper.h
 *
 * [The function which depends on a system]
 *   InfoNES_System_ooo.cpp (ooo is a system name. win, ...)
 *   InfoNES_System.h
 *
 * [CPU]
 *   K6502.cpp
 *   K6502.h
 *
 * [Others]
 *   InfoNES_Types.h
 *
 --------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_Mapper.h"
#include "InfoNES_pAPU.h"
#include "K6502.h"

// #include "timer.h"

 

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */ 

/*-------------------------------------------------------------------*/
/*  NES resources                                                    */
/*-------------------------------------------------------------------*/

/* RAM */
BYTE RAM[ RAM_SIZE ];

/* SRAM */
BYTE SRAM[ SRAM_SIZE ];

/* ROM */
BYTE *ROM;

/* SRAM BANK ( 8Kb ) */
BYTE *SRAMBANK;

/* ROM BANK ( 8Kb * 4 ) */
BYTE *ROMBANK0;
BYTE *ROMBANK1;
BYTE *ROMBANK2;
BYTE *ROMBANK3;

/*-------------------------------------------------------------------*/
/*  PPU resources                                                    */
/*-------------------------------------------------------------------*/

#ifdef RAM_LACK
/* PPU VRAM */
BYTE VRAM[ VRAM_SIZE ];
#else
/* PPU RAM */
BYTE PPURAM[ PPURAM_SIZE ];
#endif

/* VROM */
BYTE *VROM;

/* PPU BANK ( 1Kb * 16 ) */
BYTE *PPUBANK[ 16 ];

/* Sprite RAM */
BYTE SPRRAM[ SPRRAM_SIZE ];

/* PPU Register */
BYTE PPU_R0;
BYTE PPU_R1;
BYTE PPU_R2;
BYTE PPU_R3;
BYTE PPU_R7;

/* Vertical scroll value */
BYTE PPU_Scr_V;
BYTE PPU_Scr_V_Next;
BYTE PPU_Scr_V_Byte;
BYTE PPU_Scr_V_Byte_Next;
BYTE PPU_Scr_V_Bit;
BYTE PPU_Scr_V_Bit_Next;

/* Horizontal scroll value */
BYTE PPU_Scr_H;
BYTE PPU_Scr_H_Next;
BYTE PPU_Scr_H_Byte;
BYTE PPU_Scr_H_Byte_Next;
BYTE PPU_Scr_H_Bit;
BYTE PPU_Scr_H_Bit_Next;

/* PPU Address */
WORD PPU_Addr;

/* PPU Address */
WORD PPU_Temp;

/* The increase value of the PPU Address */
WORD PPU_Increment;

/* Current Scanline */
WORD PPU_Scanline;

/* Scanline Table */
BYTE PPU_ScanTable[ 263 ];

/* Name Table Bank */
BYTE PPU_NameTableBank;

/* BG Base Address */
BYTE *PPU_BG_Base;

/* Sprite Base Address */
BYTE *PPU_SP_Base;

/* Sprite Height */
WORD PPU_SP_Height;

/* Sprite #0 Scanline Hit Position */
int SpriteJustHit;

/* VRAM Write Enable ( 0: Disable, 1: Enable ) */
BYTE byVramWriteEnable;

/* PPU Address and Scroll Latch Flag*/
BYTE PPU_Latch_Flag;

/* Up and Down Clipping Flag ( 0: non-clip, 1: clip ) */ 
BYTE PPU_UpDown_Clip;

/* Frame IRQ ( 0: Disabled, 1: Enabled )*/
BYTE FrameIRQ_Enable;
WORD FrameStep;

/*-------------------------------------------------------------------*/
/*  Display and Others resouces                                      */
/*-------------------------------------------------------------------*/

/* Frame Skip */
WORD FrameSkip;
WORD FrameCnt;

/* Display Buffer */
#if WORKFRAME_DEFINE == WORKFRAME_DOUBLE
  WORD DoubleFrame[ 2 ][ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
  WORD *WorkFrame;
  WORD WorkFrameIdx;
#elif WORKFRAME_DEFINE == WORKFRAME_SINGLE
  WORD WorkFrame[ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
#else
  WORD WorkLine[ NES_DISP_WIDTH*2 ];
#endif

#ifndef RAM_LACK
/* Character Buffer */
BYTE ChrBuf[ 256 * 2 * 8 * 8 ];
/* Update flag for ChrBuf */
BYTE ChrBufUpdate;
#endif /* !RAM_LACK */

/* Palette Table */
WORD PalTable[ 32 ];

/* Table for Mirroring */
BYTE PPU_MirrorTable[][ 4 ] =
{
#ifdef RAM_LACK
  { NAME_TABLE0 & 0x3, NAME_TABLE0 & 0x3, NAME_TABLE1 & 0x3, NAME_TABLE1 & 0x3 },
  { NAME_TABLE0 & 0x3, NAME_TABLE1 & 0x3, NAME_TABLE0 & 0x3, NAME_TABLE1 & 0x3 },
  { NAME_TABLE1 & 0x3, NAME_TABLE1 & 0x3, NAME_TABLE1 & 0x3, NAME_TABLE1 & 0x3 },
  { NAME_TABLE0 & 0x3, NAME_TABLE0 & 0x3, NAME_TABLE0 & 0x3, NAME_TABLE0 & 0x3 },
  { NAME_TABLE0 & 0x3, NAME_TABLE1 & 0x3, NAME_TABLE2 & 0x3, NAME_TABLE3 & 0x3 },
  { NAME_TABLE0 & 0x3, NAME_TABLE0 & 0x3, NAME_TABLE0 & 0x3, NAME_TABLE1 & 0x3 }
#else
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE1, NAME_TABLE1 },
  { NAME_TABLE0, NAME_TABLE1, NAME_TABLE0, NAME_TABLE1 },
  { NAME_TABLE1, NAME_TABLE1, NAME_TABLE1, NAME_TABLE1 },
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE0, NAME_TABLE0 },
  { NAME_TABLE0, NAME_TABLE1, NAME_TABLE2, NAME_TABLE3 },
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE0, NAME_TABLE1 }
#endif
};

#define HS(x) ((x) * STEP_PER_SCANLINE / NES_DISP_WIDTH)
const WORD JustHitStep[256]={
  HS(0x00), HS(0x01), HS(0x02), HS(0x03), HS(0x04), HS(0x05), HS(0x06), HS(0x07), HS(0x08), HS(0x09), HS(0x0A), HS(0x0B), HS(0x0C), HS(0x0D), HS(0x0E), HS(0x0F),
  HS(0x10), HS(0x11), HS(0x12), HS(0x13), HS(0x14), HS(0x15), HS(0x16), HS(0x17), HS(0x18), HS(0x19), HS(0x1A), HS(0x1B), HS(0x1C), HS(0x1D), HS(0x1E), HS(0x1F),
  HS(0x20), HS(0x21), HS(0x22), HS(0x23), HS(0x24), HS(0x25), HS(0x26), HS(0x27), HS(0x28), HS(0x29), HS(0x2A), HS(0x2B), HS(0x2C), HS(0x2D), HS(0x2E), HS(0x2F),
  HS(0x30), HS(0x31), HS(0x32), HS(0x33), HS(0x34), HS(0x35), HS(0x36), HS(0x37), HS(0x38), HS(0x39), HS(0x3A), HS(0x3B), HS(0x3C), HS(0x3D), HS(0x3E), HS(0x3F),
  HS(0x40), HS(0x41), HS(0x42), HS(0x43), HS(0x44), HS(0x45), HS(0x46), HS(0x47), HS(0x48), HS(0x49), HS(0x4A), HS(0x4B), HS(0x4C), HS(0x4D), HS(0x4E), HS(0x4F),
  HS(0x50), HS(0x51), HS(0x52), HS(0x53), HS(0x54), HS(0x55), HS(0x56), HS(0x57), HS(0x58), HS(0x59), HS(0x5A), HS(0x5B), HS(0x5C), HS(0x5D), HS(0x5E), HS(0x5F),
  HS(0x60), HS(0x61), HS(0x62), HS(0x63), HS(0x64), HS(0x65), HS(0x66), HS(0x67), HS(0x68), HS(0x69), HS(0x6A), HS(0x6B), HS(0x6C), HS(0x6D), HS(0x6E), HS(0x6F),
  HS(0x70), HS(0x71), HS(0x72), HS(0x73), HS(0x74), HS(0x75), HS(0x76), HS(0x77), HS(0x78), HS(0x79), HS(0x7A), HS(0x7B), HS(0x7C), HS(0x7D), HS(0x7E), HS(0x7F),
  HS(0x80), HS(0x81), HS(0x82), HS(0x83), HS(0x84), HS(0x85), HS(0x86), HS(0x87), HS(0x88), HS(0x89), HS(0x8A), HS(0x8B), HS(0x8C), HS(0x8D), HS(0x8E), HS(0x8F),
  HS(0x90), HS(0x91), HS(0x92), HS(0x93), HS(0x94), HS(0x95), HS(0x96), HS(0x97), HS(0x98), HS(0x99), HS(0x9A), HS(0x9B), HS(0x9C), HS(0x9D), HS(0x9E), HS(0x9F),
  HS(0xA0), HS(0xA1), HS(0xA2), HS(0xA3), HS(0xA4), HS(0xA5), HS(0xA6), HS(0xA7), HS(0xA8), HS(0xA9), HS(0xAA), HS(0xAB), HS(0xAC), HS(0xAD), HS(0xAE), HS(0xAF),
  HS(0xB0), HS(0xB1), HS(0xB2), HS(0xB3), HS(0xB4), HS(0xB5), HS(0xB6), HS(0xB7), HS(0xB8), HS(0xB9), HS(0xBA), HS(0xBB), HS(0xBC), HS(0xBD), HS(0xBE), HS(0xBF),
  HS(0xC0), HS(0xC1), HS(0xC2), HS(0xC3), HS(0xC4), HS(0xC5), HS(0xC6), HS(0xC7), HS(0xC8), HS(0xC9), HS(0xCA), HS(0xCB), HS(0xCC), HS(0xCD), HS(0xCE), HS(0xCF),
  HS(0xD0), HS(0xD1), HS(0xD2), HS(0xD3), HS(0xD4), HS(0xD5), HS(0xD6), HS(0xD7), HS(0xD8), HS(0xD9), HS(0xDA), HS(0xDB), HS(0xDC), HS(0xDD), HS(0xDE), HS(0xDF),
  HS(0xE0), HS(0xE1), HS(0xE2), HS(0xE3), HS(0xE4), HS(0xE5), HS(0xE6), HS(0xE7), HS(0xE8), HS(0xE9), HS(0xEA), HS(0xEB), HS(0xEC), HS(0xED), HS(0xEE), HS(0xEF),
  HS(0xF0), HS(0xF1), HS(0xF2), HS(0xF3), HS(0xF4), HS(0xF5), HS(0xF6), HS(0xF7), HS(0xF8), HS(0xF9), HS(0xFA), HS(0xFB), HS(0xFC), HS(0xFD), HS(0xFE), HS(0xFF)
};
#undef HS

/*-------------------------------------------------------------------*/
/*  APU and Pad resources                                            */
/*-------------------------------------------------------------------*/

#ifndef APU_NONE
/* APU Register */
BYTE APU_Reg[ 0x18 ];

/* APU Mute ( 0:OFF, 1:ON ) */
int APU_Mute = 0;
#endif /* APU_NONE */

/* Pad data */
DWORD PAD1_Latch;
DWORD PAD2_Latch;
DWORD PAD_System;
DWORD PAD1_Bit;
DWORD PAD2_Bit;

/*-------------------------------------------------------------------*/
/*  ROM information                                                  */
/*-------------------------------------------------------------------*/

/* .nes File Header */
struct NesHeader_tag NesHeader;

/* Mapper Number */
BYTE MapperNo;

/* Mirroring 0:Horizontal 1:Vertical */
BYTE ROM_Mirroring;
/* It has SRAM */
BYTE ROM_SRAM;
/* It has Trainer */
BYTE ROM_Trainer;
/* Four screen VRAM  */
BYTE ROM_FourScr;

/*===================================================================*/
/*                                                                   */
/*                InfoNES_Init() : Initialize InfoNES                */
/*                                                                   */
/*===================================================================*/
void InfoNES_Init()
{
/*
 *  Initialize InfoNES
 *
 *  Remarks
 *    Initialize K6502 and Scanline Table.
 */
  int nIdx;

  // Initialize 6502
  K6502_Init();

  // Initialize Scanline Table
  for ( nIdx = 0; nIdx < 263; ++nIdx )
  {
    if ( nIdx < SCAN_ON_SCREEN_START )
      PPU_ScanTable[ nIdx ] = SCAN_ON_SCREEN;
    else
    if ( nIdx < SCAN_BOTTOM_OFF_SCREEN_START )
      PPU_ScanTable[ nIdx ] = SCAN_ON_SCREEN;
    else
    if ( nIdx < SCAN_UNKNOWN_START )
      PPU_ScanTable[ nIdx ] = SCAN_ON_SCREEN;
    else
    if ( nIdx < SCAN_VBLANK_START )
      PPU_ScanTable[ nIdx ] = SCAN_UNKNOWN;
    else
      PPU_ScanTable[ nIdx ] = SCAN_VBLANK;
  }
}

/*===================================================================*/
/*                                                                   */
/*                InfoNES_Fin() : Completion treatment               */
/*                                                                   */
/*===================================================================*/
void InfoNES_Fin()
{
/*
 *  Completion treatment
 *
 *  Remarks
 *    Release resources
 */
#ifndef APU_NONE
  // Finalize pAPU
  InfoNES_pAPUDone();
#endif /* APU_NONE */

  // Release a memory for ROM
  InfoNES_ReleaseRom();
}

/*===================================================================*/
/*                                                                   */
/*                  InfoNES_Load() : Load a cassette                 */
/*                                                                   */
/*===================================================================*/
int InfoNES_Load( const char *pszFileName )
{
/*
 *  Load a cassette
 *
 *  Parameters
 *    const char *pszFileName            (Read)
 *      File name of ROM image
 *
 *  Return values
 *     0 : It was finished normally.
 *    -1 : An error occurred.
 *
 *  Remarks
 *    Read a ROM image in the memory. 
 *    Reset InfoNES.
 */

  // Release a memory for ROM
  InfoNES_ReleaseRom();

  // Read a ROM image in the memory
  if ( InfoNES_ReadRom( pszFileName ) < 0 )
    return -1;

  // Reset InfoNES
  if ( InfoNES_Reset() < 0 )
    return -1;

  // Successful
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*                 InfoNES_Reset() : Reset InfoNES                   */
/*                                                                   */
/*===================================================================*/
int InfoNES_Reset()
{
/*
 *  Reset InfoNES
 *
 *  Return values
 *     0 : Normally
 *    -1 : Non support mapper
 *
 *  Remarks
 *    Initialize Resources, PPU and Mapper.
 *    Reset CPU.
 */

  int nIdx;

  /*-------------------------------------------------------------------*/
  /*  Get information on the cassette                                  */
  /*-------------------------------------------------------------------*/

  // Get Mapper Number
  MapperNo = NesHeader.byInfo1 >> 4;

  // Check bit counts of Mapper No.
  for ( nIdx = 4; nIdx < 8 && NesHeader.byReserve[ nIdx ] == 0; ++nIdx )
    ;

  if ( nIdx == 8 )
  {
    // Mapper Number is 8bits
    MapperNo |= ( NesHeader.byInfo2 & 0xf0 );
  }

  // Get information on the ROM
  ROM_Mirroring = NesHeader.byInfo1 & 1;
  ROM_SRAM = NesHeader.byInfo1 & 2;
  ROM_Trainer = NesHeader.byInfo1 & 4;
  ROM_FourScr = NesHeader.byInfo1 & 8;

  /*-------------------------------------------------------------------*/
  /*  Initialize resources                                             */
  /*-------------------------------------------------------------------*/

  // Clear RAM
  InfoNES_MemorySet( RAM, 0, sizeof RAM );

  // Reset frame skip and frame count
  FrameSkip = 0;
  FrameCnt = 0;

#if WORKFRAME_DEFINE == WORKFRAME_DOUBLE
  // Reset work frame
  WorkFrame = DoubleFrame[ 0 ];
  WorkFrameIdx = 0;
#endif

#ifndef RAM_LACK
  // Reset update flag of ChrBuf
  ChrBufUpdate = 0xff;
#endif /* !RAM_LACK */

  // Reset palette table
  InfoNES_MemorySet( PalTable, 0, sizeof PalTable );

#ifndef APU_NONE
  // Reset APU register
  InfoNES_MemorySet( APU_Reg, 0, sizeof APU_Reg );
#endif /* APU_NONE */

  // Reset joypad
  PAD1_Latch = PAD2_Latch = PAD_System = 0;
  PAD1_Bit = PAD2_Bit = 0;

  /*-------------------------------------------------------------------*/
  /*  Initialize PPU                                                   */
  /*-------------------------------------------------------------------*/

  InfoNES_SetupPPU();

#ifndef APU_NONE
  /*-------------------------------------------------------------------*/
  /*  Initialize pAPU                                                  */
  /*-------------------------------------------------------------------*/

  InfoNES_pAPUInit();
#endif /* APU_NONE */

  /*-------------------------------------------------------------------*/
  /*  Initialize Mapper                                                */
  /*-------------------------------------------------------------------*/

  // Get Mapper Table Index
  for ( nIdx = 0; MapperTable[ nIdx ].nMapperNo != -1; ++nIdx )
  {
    if ( MapperTable[ nIdx ].nMapperNo == MapperNo )
      break;
  }

  if ( MapperTable[ nIdx ].nMapperNo == -1 )
  {
    // Non support mapper
    InfoNES_MessageBox( "Mapper #%d is unsupported.\n", MapperNo );
    return -1;
  }

  // Set up a mapper initialization function
  MapperTable[ nIdx ].pMapperInit();

  /*-------------------------------------------------------------------*/
  /*  Reset CPU                                                        */
  /*-------------------------------------------------------------------*/

  K6502_Reset();

  // Successful
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*                InfoNES_SetupPPU() : Initialize PPU                */
/*                                                                   */
/*===================================================================*/
void InfoNES_SetupPPU()
{
/*
 *  Initialize PPU
 *
 */
  int nPage;

  // Clear PPU and Sprite Memory
#ifdef RAM_LACK
  InfoNES_MemorySet( CRAM, 0, CRAM_SIZE );
  InfoNES_MemorySet( VRAM, 0, VRAM_SIZE );
#else
  InfoNES_MemorySet( PPURAM, 0, sizeof PPURAM );
#endif
  InfoNES_MemorySet( SPRRAM, 0, sizeof SPRRAM );

  // Reset PPU Register
  PPU_R0 = PPU_R1 = PPU_R2 = PPU_R3 = PPU_R7 = 0;

  // Reset latch flag
  PPU_Latch_Flag = 0;

  // Reset up and down clipping flag
  PPU_UpDown_Clip = 0;

  FrameStep = 0;
  FrameIRQ_Enable = 0;

  // Reset Scroll values
  PPU_Scr_V = PPU_Scr_V_Next = PPU_Scr_V_Byte = PPU_Scr_V_Byte_Next = PPU_Scr_V_Bit = PPU_Scr_V_Bit_Next = 0;
  PPU_Scr_H = PPU_Scr_H_Next = PPU_Scr_H_Byte = PPU_Scr_H_Byte_Next = PPU_Scr_H_Bit = PPU_Scr_H_Bit_Next = 0;

  // Reset PPU address
  PPU_Addr = 0;
  PPU_Temp = 0;

  // Reset scanline
  PPU_Scanline = 0;

  // Reset hit position of sprite #0 
  SpriteJustHit = 0;

  // Reset information on PPU_R0
  PPU_Increment = 1;
  PPU_NameTableBank = NAME_TABLE0;
#ifdef RAM_LACK
  PPU_BG_Base = PPUBANK[ 0 ];
  PPU_SP_Base = PPUBANK[ 4 ];
#else
  PPU_BG_Base = ChrBuf;
  PPU_SP_Base = ChrBuf + 256 * 64;
#endif /* RAM_LACK */
  PPU_SP_Height = 8;

  // Reset PPU banks
#ifdef RAM_LACK
  for ( nPage = 0; nPage < 8; ++nPage )
    PPUBANK[ nPage ] = CRAMPAGE( nPage );
  for ( nPage = 8; nPage < 12; ++nPage )
    PPUBANK[ nPage ] = VRAMPAGE( nPage & 0x3 );
#else
  for ( nPage = 0; nPage < 16; ++nPage )
    PPUBANK[ nPage ] = &PPURAM[ nPage * 0x400 ];
#endif

  /* Mirroring of Name Table */
  InfoNES_Mirroring( ROM_Mirroring );

  /* Reset VRAM Write Enable */
  byVramWriteEnable = ( NesHeader.byVRomSize == 0 ) ? 1 : 0;
}

/*===================================================================*/
/*                                                                   */
/*       InfoNES_Mirroring() : Set up a Mirroring of Name Table      */
/*                                                                   */
/*===================================================================*/
void InfoNES_Mirroring( int nType )
{
/*
 *  Set up a Mirroring of Name Table
 *
 *  Parameters
 *    int nType          (Read)
 *      Mirroring Type
 *        0 : Horizontal
 *        1 : Vertical
 *        2 : One Screen 0x2400
 *        3 : One Screen 0x2000
 *        4 : Four Screen
 *        5 : Special for Mapper #233
 */
#ifdef RAM_LACK
  PPUBANK[ NAME_TABLE0 ] = VRAMPAGE( PPU_MirrorTable[ nType ][ 0 ] );
  PPUBANK[ NAME_TABLE1 ] = VRAMPAGE( PPU_MirrorTable[ nType ][ 1 ] );
  PPUBANK[ NAME_TABLE2 ] = VRAMPAGE( PPU_MirrorTable[ nType ][ 2 ] );
  PPUBANK[ NAME_TABLE3 ] = VRAMPAGE( PPU_MirrorTable[ nType ][ 3 ] );
#else
  PPUBANK[ NAME_TABLE0 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 0 ] * 0x400 ];
  PPUBANK[ NAME_TABLE1 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 1 ] * 0x400 ];
  PPUBANK[ NAME_TABLE2 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 2 ] * 0x400 ];
  PPUBANK[ NAME_TABLE3 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 3 ] * 0x400 ];
#endif
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_Main() : The main loop of InfoNES            */
/*                                                                   */
/*===================================================================*/
void InfoNES_Main()
{
/*
 *  The main loop of InfoNES
 *
 */

  // Initialize InfoNES
  InfoNES_Init();

  // Main loop
  while ( 1 )
  {
    /*-------------------------------------------------------------------*/
    /*  To the menu screen                                               */
    /*-------------------------------------------------------------------*/
    if ( InfoNES_Menu() == -1 )
      break;  // Quit
    
    /*-------------------------------------------------------------------*/
    /*  Start a NES emulation                                            */
    /*-------------------------------------------------------------------*/
    InfoNES_Cycle();
  }

  // Completion treatment
  InfoNES_Fin();
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_Cycle() : The loop of emulation              */
/*                                                                   */
/*===================================================================*/
void InfoNES_Cycle()
{
/*
 *  The loop of emulation
 *
 */

#if 0
  // Set the PPU adress to the buffered value
  if ( ( PPU_R1 & R1_SHOW_SP ) || ( PPU_R1 & R1_SHOW_SCR ) )
		PPU_Addr = PPU_Temp;
#endif

  // Emulation loop
//  for (;;)
	while(1)
  {    
    int nStep;

    // Set a flag if a scanning line is a hit in the sprite #0
    if ( SpriteJustHit == PPU_Scanline &&
      PPU_ScanTable[ PPU_Scanline ] == SCAN_ON_SCREEN )
    {
      // # of Steps to execute before sprite #0 hit
      nStep = JustHitStep[SPRRAM[ SPR_X ]];

      // Execute instructions
      K6502_Step( nStep );

      // Set a sprite hit flag
      if ( ( PPU_R1 & R1_SHOW_SP ) && ( PPU_R1 & R1_SHOW_SCR ) )
        PPU_R2 |= R2_HIT_SP;

      // NMI is required if there is necessity
      if ( ( PPU_R0 & R0_NMI_SP ) && ( PPU_R1 & R1_SHOW_SP ) )
        NMI_REQ;

      // Execute instructions
      K6502_Step( STEP_PER_SCANLINE - nStep );
    }
    else
    {
      // Execute instructions
      K6502_Step( STEP_PER_SCANLINE );
    }

    // Frame IRQ in H-Sync
    FrameStep += STEP_PER_SCANLINE;
    if ( FrameStep > STEP_PER_FRAME && FrameIRQ_Enable )
    {
      FrameStep %= STEP_PER_FRAME;
      IRQ_REQ;
#ifndef APU_NONE
      APU_Reg[ 0x4015 ] |= 0x40;
#endif /* APU_NONE */
    }

    // A mapper function in H-Sync
    MapperHSync();
    
    // A function in H-Sync
    if ( InfoNES_HSync() == -1 )
      return;  // To the menu screen

    // HSYNC Wait
    InfoNES_Wait();
	

  }
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_HSync() : A function in H-Sync               */
/*                                                                   */
/*===================================================================*/

int InfoNES_HSync()
{
/*
 *  A function in H-Sync
 *
 *  Return values
 *    0 : Normally
 *   -1 : Exit an emulation
 */

  /*-------------------------------------------------------------------*/
  /*  Render a scanline                                                */
  /*-------------------------------------------------------------------*/
  if ( FrameCnt == 0 &&
       PPU_ScanTable[ PPU_Scanline ] == SCAN_ON_SCREEN )
  {
    InfoNES_DrawLine();
#if WORKFRAME_DEFINE == WORKFRAME_NONE
	// Render a scanline to screen
	InfoNES_LoadLine();
#endif  
  }

  /*-------------------------------------------------------------------*/
  /*  Set new scroll values                                            */
  /*-------------------------------------------------------------------*/
  PPU_Scr_V      = PPU_Scr_V_Next;
  PPU_Scr_V_Byte = PPU_Scr_V_Byte_Next;
  PPU_Scr_V_Bit  = PPU_Scr_V_Bit_Next;

  PPU_Scr_H      = PPU_Scr_H_Next;
  PPU_Scr_H_Byte = PPU_Scr_H_Byte_Next;
  PPU_Scr_H_Bit  = PPU_Scr_H_Bit_Next;

  /*-------------------------------------------------------------------*/
  /*  Next Scanline                                                    */
  /*-------------------------------------------------------------------*/
  PPU_Scanline = ( PPU_Scanline == SCAN_VBLANK_END ) ? 0 : PPU_Scanline + 1;

  /*-------------------------------------------------------------------*/
  /*  Operation in the specific scanning line                          */
  /*-------------------------------------------------------------------*/
  switch ( PPU_Scanline )
  {
    case SCAN_TOP_OFF_SCREEN:
      // Reset a PPU status
      PPU_R2 = 0;

      // Set up a character data
      if ( NesHeader.byVRomSize == 0 && FrameCnt == 0 )
        InfoNES_SetupChr();

      // Get position of sprite #0
      InfoNES_GetSprHitY();
      break;

    case SCAN_UNKNOWN_START:
      if ( FrameCnt == 0 )
      {
#if WORKFRAME_DEFINE != WORKFRAME_NONE
        // Transfer the contents of work frame on the screen
        InfoNES_LoadFrame();
#if WORKFRAME_DEFINE == WORKFRAME_DOUBLE
        // Switching of the double buffer
        WorkFrameIdx = 1 - WorkFrameIdx;
        WorkFrame = DoubleFrame[ WorkFrameIdx ];
#endif
#endif
      }
      break;

    case SCAN_VBLANK_START:
      // FrameCnt + 1
      FrameCnt = ( FrameCnt >= FrameSkip ) ? 0 : FrameCnt + 1;

      // Set a V-Blank flag
      PPU_R2 = R2_IN_VBLANK;

      // Reset latch flag
      PPU_Latch_Flag = 0;

#ifndef APU_NONE
      // pAPU Sound function in V-Sync
      if ( !APU_Mute )
        InfoNES_pAPUVsync();
#endif /* APU_NONE */

      // A mapper function in V-Sync
      MapperVSync();

      // Get the condition of the joypad
      InfoNES_PadState( &PAD1_Latch, &PAD2_Latch, &PAD_System );
      
      // NMI on V-Blank
      if ( PPU_R0 & R0_NMI_VB )
        NMI_REQ;

      // Exit an emulation if a QUIT button is pushed
      if ( PAD_PUSH( PAD_System, PAD_SYS_QUIT ) )
        return -1;  // Exit an emulation      
      
      break;
  }

  // Successful
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_DrawLine() : Render a scanline               */
/*                                                                   */
/*===================================================================*/
// void InfoNES_DrawLine()__attribute__((optimize(0)));
void InfoNES_DrawLine()
{
/*
 *  Render a scanline
 *
 */
  int nX;
  int nY;
  int nY4;
  int nYBit;
  WORD *pPalTbl;
  BYTE *pAttrBase;
  WORD *pPoint;
  int nNameTable;
  BYTE *pbyNameTable;
  BYTE *pbyChrData;
#ifdef RAM_LACK
  BYTE tileData[2] = {0, 0};
#else
  int nIdx;
#endif /* RAM_LACK */
  BYTE *pSPRRAM;
  int nAttr;
  int nSprCnt;
  int nSprData;
  BYTE bySprCol;
  BYTE pSprBuf[ NES_DISP_WIDTH + 7 ];

  /*-------------------------------------------------------------------*/
  /*  Render Background                                                */
  /*-------------------------------------------------------------------*/

  /* MMC5 VROM switch */
  MapperRenderScreen( 1 );

  // Pointer to the render position
#if WORKFRAME_DEFINE == WORKFRAME_NONE
  pPoint = WorkLine;
#else
  pPoint = &WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];
#endif

  // Clear a scanline if screen is off
  if ( !( PPU_R1 & R1_SHOW_SCR ) )
  {
    InfoNES_MemorySet( pPoint, 0, NES_DISP_WIDTH << 1 );
  }
  else
  {
    nNameTable = PPU_NameTableBank;

    nY = PPU_Scr_V_Byte + ( PPU_Scanline >> 3 );

    nYBit = PPU_Scr_V_Bit + ( PPU_Scanline & 7 );

    if ( nYBit > 7 )
    {
      ++nY;
      nYBit &= 7;
    }
#ifndef RAM_LACK
    nYBit <<= 3;
#endif /* !RAM_LACK */

    if ( nY > 29 )
    {
      // Next NameTable (An up-down direction)
      nNameTable ^= NAME_TABLE_V_MASK;
      nY -= 30;
    }

    nX = PPU_Scr_H_Byte;

    nY4 = ( ( nY & 2 ) << 1 );

    /*-------------------------------------------------------------------*/
    /*  Rendering of the block of the left end                           */
    /*-------------------------------------------------------------------*/

    pbyNameTable = PPUBANK[ nNameTable ] + nY * 32 + nX;
#ifdef RAM_LACK
    pbyChrData = PPU_BG_Base + ( *pbyNameTable << 4 ) + nYBit;
    tileData[0] = ( ( pbyChrData[ 0 ] >> 1 ) & 0x55 ) | ( pbyChrData[ 8 ] & 0xAA );
    tileData[1] = ( pbyChrData[ 0 ] & 0x55 ) | ( ( pbyChrData[ 8 ] << 1 ) & 0xAA );
#else
    pbyChrData = PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
#endif /* RAM_LACK */
    pAttrBase = PPUBANK[ nNameTable ] + 0x3c0 + ( nY / 4 ) * 8;
    pPalTbl =  &PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];

#ifdef RAM_LACK
	switch(PPU_Scr_H_Bit)
	{
		case 0:
		  *( pPoint++ ) = pPalTbl[ ( tileData[0] >> 6 ) & 3 ];
		case 1:
		  *( pPoint++ ) = pPalTbl[ ( tileData[1] >> 6 ) & 3 ];
		case 2:
		  *( pPoint++ ) = pPalTbl[ ( tileData[0] >> 4 ) & 3 ];
		case 3:
		  *( pPoint++ ) = pPalTbl[ ( tileData[1] >> 4 ) & 3 ];
		case 4:
		  *( pPoint++ ) = pPalTbl[ ( tileData[0] >> 2 ) & 3 ];
		case 5:
		  *( pPoint++ ) = pPalTbl[ ( tileData[1] >> 2 ) & 3 ];
		case 6:
		  *( pPoint++ ) = pPalTbl[ ( tileData[0] ) & 3 ];
		case 7:
		  *( pPoint++ ) = pPalTbl[ ( tileData[1] ) & 3 ];
		default:
			break;
	}
#else
    for ( nIdx = PPU_Scr_H_Bit; nIdx < 8; ++nIdx )
    {
      *( pPoint++ ) = pPalTbl[ pbyChrData[ nIdx ] ];
    }
#endif

    // Callback at PPU read/write
    MapperPPU( PATTBL( pbyChrData ) );

    ++nX;
    ++pbyNameTable;

    /*-------------------------------------------------------------------*/
    /*  Rendering of the left table                                      */
    /*-------------------------------------------------------------------*/

    for ( ; nX < 32; ++nX )
    {
#ifdef RAM_LACK
      pbyChrData = PPU_BG_Base + ( *pbyNameTable << 4 ) + nYBit;
      tileData[0] = ( ( pbyChrData[ 0 ] >> 1 ) & 0x55 ) | ( pbyChrData[ 8 ] & 0xAA );
      tileData[1] = ( pbyChrData[ 0 ] & 0x55 ) | ( ( pbyChrData[ 8 ] << 1 ) & 0xAA );
#else
      pbyChrData = PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
#endif /* RAM_LACK */
      pPalTbl = &PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];

#ifdef RAM_LACK
      pPoint[ 0 ] = pPalTbl[ ( tileData[0] >> 6 ) & 3 ]; 
      pPoint[ 1 ] = pPalTbl[ ( tileData[1] >> 6 ) & 3 ];
      pPoint[ 2 ] = pPalTbl[ ( tileData[0] >> 4 ) & 3 ];
      pPoint[ 3 ] = pPalTbl[ ( tileData[1] >> 4 ) & 3 ];
      pPoint[ 4 ] = pPalTbl[ ( tileData[0] >> 2 ) & 3 ];
      pPoint[ 5 ] = pPalTbl[ ( tileData[1] >> 2 ) & 3 ];
      pPoint[ 6 ] = pPalTbl[ ( tileData[0] ) & 3 ];
      pPoint[ 7 ] = pPalTbl[ ( tileData[1] ) & 3 ];
#else
      pPoint[ 0 ] = pPalTbl[ pbyChrData[ 0 ] ]; 
      pPoint[ 1 ] = pPalTbl[ pbyChrData[ 1 ] ];
      pPoint[ 2 ] = pPalTbl[ pbyChrData[ 2 ] ];
      pPoint[ 3 ] = pPalTbl[ pbyChrData[ 3 ] ];
      pPoint[ 4 ] = pPalTbl[ pbyChrData[ 4 ] ];
      pPoint[ 5 ] = pPalTbl[ pbyChrData[ 5 ] ];
      pPoint[ 6 ] = pPalTbl[ pbyChrData[ 6 ] ];
      pPoint[ 7 ] = pPalTbl[ pbyChrData[ 7 ] ];
#endif

      pPoint += 8;

      // Callback at PPU read/write
      MapperPPU( PATTBL( pbyChrData ) );

      ++pbyNameTable;
    }

    // Holizontal Mirror
    nNameTable ^= NAME_TABLE_H_MASK;

    pbyNameTable = PPUBANK[ nNameTable ] + nY * 32;
    pAttrBase = PPUBANK[ nNameTable ] + 0x3c0 + ( nY / 4 ) * 8;

    /*-------------------------------------------------------------------*/
    /*  Rendering of the right table                                     */
    /*-------------------------------------------------------------------*/

    for ( nX = 0; nX < PPU_Scr_H_Byte; ++nX )
    {
#ifdef RAM_LACK
      pbyChrData = PPU_BG_Base + ( *pbyNameTable << 4 ) + nYBit;
      tileData[0] = ( ( pbyChrData[ 0 ] >> 1 ) & 0x55 ) | ( pbyChrData[ 8 ] & 0xAA );
      tileData[1] = ( pbyChrData[ 0 ] & 0x55 ) | ( ( pbyChrData[ 8 ] << 1 ) & 0xAA );
#else
      pbyChrData = PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
#endif /* RAM_LACK */
      pPalTbl = &PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];

#ifdef RAM_LACK
      pPoint[ 0 ] = pPalTbl[ ( tileData[0] >> 6 ) & 3 ]; 
      pPoint[ 1 ] = pPalTbl[ ( tileData[1] >> 6 ) & 3 ];
      pPoint[ 2 ] = pPalTbl[ ( tileData[0] >> 4 ) & 3 ];
      pPoint[ 3 ] = pPalTbl[ ( tileData[1] >> 4 ) & 3 ];
      pPoint[ 4 ] = pPalTbl[ ( tileData[0] >> 2 ) & 3 ];
      pPoint[ 5 ] = pPalTbl[ ( tileData[1] >> 2 ) & 3 ];
      pPoint[ 6 ] = pPalTbl[ ( tileData[0] ) & 3 ];
      pPoint[ 7 ] = pPalTbl[ ( tileData[1] ) & 3 ];
#else
      pPoint[ 0 ] = pPalTbl[ pbyChrData[ 0 ] ]; 
      pPoint[ 1 ] = pPalTbl[ pbyChrData[ 1 ] ];
      pPoint[ 2 ] = pPalTbl[ pbyChrData[ 2 ] ];
      pPoint[ 3 ] = pPalTbl[ pbyChrData[ 3 ] ];
      pPoint[ 4 ] = pPalTbl[ pbyChrData[ 4 ] ];
      pPoint[ 5 ] = pPalTbl[ pbyChrData[ 5 ] ];
      pPoint[ 6 ] = pPalTbl[ pbyChrData[ 6 ] ];
      pPoint[ 7 ] = pPalTbl[ pbyChrData[ 7 ] ];
#endif

      pPoint += 8;

      // Callback at PPU read/write
      MapperPPU( PATTBL( pbyChrData ) );

      ++pbyNameTable;
    }

    /*-------------------------------------------------------------------*/
    /*  Rendering of the block of the right end                          */
    /*-------------------------------------------------------------------*/

#ifdef RAM_LACK
    pbyChrData = PPU_BG_Base + ( *pbyNameTable << 4 ) + nYBit;
    tileData[0] = ( ( pbyChrData[ 0 ] >> 1 ) & 0x55 ) | ( pbyChrData[ 8 ] & 0xAA );
    tileData[1] = ( pbyChrData[ 0 ] & 0x55 ) | ( ( pbyChrData[ 8 ] << 1 ) & 0xAA );
#else
    pbyChrData = PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
#endif /* RAM_LACK */
    pPalTbl = &PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];

#ifdef RAM_LACK
	switch(PPU_Scr_H_Bit)
	{
    case 8:
		  pPoint[ 7 ] = pPalTbl[ ( tileData[1] ) & 3 ];
		case 7:
		  pPoint[ 6 ] = pPalTbl[ ( tileData[0] ) & 3 ];
		case 6:
		  pPoint[ 5 ] = pPalTbl[ ( tileData[1] >> 2 ) & 3 ];
		case 5:
		  pPoint[ 4 ] = pPalTbl[ ( tileData[0] >> 2 ) & 3 ];
		case 4:
		  pPoint[ 3 ] = pPalTbl[ ( tileData[1] >> 4 ) & 3 ];
		case 3:
		  pPoint[ 2 ] = pPalTbl[ ( tileData[0] >> 4 ) & 3 ];
		case 2:
		  pPoint[ 1 ] = pPalTbl[ ( tileData[1] >> 6 ) & 3 ];
		case 1:
		  pPoint[ 0 ] = pPalTbl[ ( tileData[0] >> 6 ) & 3 ];
		case 0:
		default:
			break;
	}
#else
    for ( nIdx = 0; nIdx < PPU_Scr_H_Bit; ++nIdx )
    {
      pPoint[ nIdx ] = pPalTbl[ pbyChrData[ nIdx ] ];
    }
#endif

    // Callback at PPU read/write
    MapperPPU( PATTBL( pbyChrData ) );

    /*-------------------------------------------------------------------*/
    /*  Backgroud Clipping                                               */
    /*-------------------------------------------------------------------*/
    if ( !( PPU_R1 & R1_CLIP_BG ) )
    {
      WORD *pPointTop;

#if WORKFRAME_DEFINE == WORKFRAME_NONE
			pPointTop = WorkLine;
#else
			pPointTop = &WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];
#endif
      InfoNES_MemorySet( pPointTop, 0, 8 << 1 );
    }

    /*-------------------------------------------------------------------*/
    /*  Clear a scanline if up and down clipping flag is set             */
    /*-------------------------------------------------------------------*/
    if ( PPU_UpDown_Clip && 
       ( SCAN_ON_SCREEN_START > PPU_Scanline || PPU_Scanline > SCAN_BOTTOM_OFF_SCREEN_START ) )
    {
      WORD *pPointTop;

#if WORKFRAME_DEFINE == WORKFRAME_NONE
			pPointTop = WorkLine;
#else
			pPointTop = &WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];
#endif
      InfoNES_MemorySet( pPointTop, 0, NES_DISP_WIDTH << 1 );
    }  
  }

  /*-------------------------------------------------------------------*/
  /*  Render a sprite                                                  */
  /*-------------------------------------------------------------------*/

  /* MMC5 VROM switch */
  MapperRenderScreen( 0 );

  if ( PPU_R1 & R1_SHOW_SP )
  {
    // Reset Scanline Sprite Count
    PPU_R2 &= ~R2_MAX_SP;

    // Reset sprite buffer
    InfoNES_MemorySet( pSprBuf, 0, sizeof pSprBuf );

    // Render a sprite to the sprite buffer
    nSprCnt = 0;
    for ( pSPRRAM = SPRRAM + ( 63 << 2 ); pSPRRAM >= SPRRAM; pSPRRAM -= 4 )
    {
      nY = pSPRRAM[ SPR_Y ] + 1;
      if ( nY > PPU_Scanline || nY + PPU_SP_Height <= PPU_Scanline )
        continue;  // Next sprite

     /*-------------------------------------------------------------------*/
     /*  A sprite in scanning line                                        */
     /*-------------------------------------------------------------------*/

      // Holizontal Sprite Count +1
      ++nSprCnt;
      
      nAttr = pSPRRAM[ SPR_ATTR ];
      nYBit = PPU_Scanline - nY;
      nYBit = ( nAttr & SPR_ATTR_V_FLIP ) ? ( PPU_SP_Height - nYBit - 1 ) : nYBit;
#ifndef RAM_LACK
      nYBit <<= 3;
#endif /* !RAM_LACK */

      if ( PPU_R0 & R0_SP_SIZE )
      {
        // Sprite size 8x16
        if ( pSPRRAM[ SPR_CHR ] & 1 )
        {
#ifdef RAM_LACK
          pbyChrData = PPUBANK[ 4 ] + ( ( pSPRRAM[ SPR_CHR ] & 0xfe ) << 4 ) + ((nYBit & 0x8) << 1) + (nYBit & 0x7);
#else
          pbyChrData = ChrBuf + 256 * 64 + ( ( pSPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit;
#endif /* RAM_LACK */
        }
        else
        {
#ifdef RAM_LACK
          pbyChrData = PPUBANK[ 0 ] + ( ( pSPRRAM[ SPR_CHR ] & 0xfe ) << 4 ) + ((nYBit & 0x8) << 1) + (nYBit & 0x7);
#else
          pbyChrData = ChrBuf + ( ( pSPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit;
#endif /* RAM_LACK */
        }
      }
      else
      {
        // Sprite size 8x8
#ifdef RAM_LACK
        pbyChrData = PPU_SP_Base + ( pSPRRAM[ SPR_CHR ] << 4 ) + nYBit;
#else
        pbyChrData = PPU_SP_Base + ( pSPRRAM[ SPR_CHR ] << 6 ) + nYBit;
#endif /* RAM_LACK */
      }

      nAttr ^= SPR_ATTR_PRI;
      bySprCol = ( nAttr & ( SPR_ATTR_COLOR | SPR_ATTR_PRI ) ) << 2;
      nX = pSPRRAM[ SPR_X ];
#ifdef RAM_LACK
      tileData[0] = ( ( pbyChrData[ 0 ] >> 1 ) & 0x55 ) | ( pbyChrData[ 8 ] & 0xAA );
      tileData[1] = ( pbyChrData[ 0 ] & 0x55 ) | ( ( pbyChrData[ 8 ] << 1 ) & 0xAA );
#endif /* RAM_LACK */
      if ( nAttr & SPR_ATTR_H_FLIP )
      {
        // Horizontal flip
#ifdef RAM_LACK
        if ( ( tileData[1] ) & 3 )
          pSprBuf[ nX ]     = bySprCol | (( tileData[1] ) & 3);
        if ( ( tileData[0] ) & 3 )
          pSprBuf[ nX + 1 ] = bySprCol | (( tileData[0] ) & 3);
        if ( ( tileData[1] >> 2 ) & 3 )
          pSprBuf[ nX + 2 ] = bySprCol | (( tileData[1] >> 2 ) & 3);
        if ( ( tileData[0] >> 2 ) & 3 )
          pSprBuf[ nX + 3 ] = bySprCol | (( tileData[0] >> 2 ) & 3);
        if ( ( tileData[1] >> 4 ) & 3 )
          pSprBuf[ nX + 4 ] = bySprCol | (( tileData[1] >> 4 ) & 3);
        if ( ( tileData[0] >> 4 ) & 3 )
          pSprBuf[ nX + 5 ] = bySprCol | (( tileData[0] >> 4 ) & 3);
        if ( ( tileData[1] >> 6 ) & 3 )
          pSprBuf[ nX + 6 ] = bySprCol | (( tileData[1] >> 6 ) & 3);
        if ( ( tileData[0] >> 6 ) & 3 )
          pSprBuf[ nX + 7 ] = bySprCol | (( tileData[0] >> 6 ) & 3);
#else
        if ( pbyChrData[ 7 ] )
          pSprBuf[ nX ]     = bySprCol | pbyChrData[ 7 ];
        if ( pbyChrData[ 6 ] )
          pSprBuf[ nX + 1 ] = bySprCol | pbyChrData[ 6 ];
        if ( pbyChrData[ 5 ] )
          pSprBuf[ nX + 2 ] = bySprCol | pbyChrData[ 5 ];
        if ( pbyChrData[ 4 ] )
          pSprBuf[ nX + 3 ] = bySprCol | pbyChrData[ 4 ];
        if ( pbyChrData[ 3 ] )
          pSprBuf[ nX + 4 ] = bySprCol | pbyChrData[ 3 ];
        if ( pbyChrData[ 2 ] )
          pSprBuf[ nX + 5 ] = bySprCol | pbyChrData[ 2 ];
        if ( pbyChrData[ 1 ] )
          pSprBuf[ nX + 6 ] = bySprCol | pbyChrData[ 1 ];
        if ( pbyChrData[ 0 ] )
          pSprBuf[ nX + 7 ] = bySprCol | pbyChrData[ 0 ];
#endif /* RAM_LACK */
      }
      else
      {
        // Non flip
#ifdef RAM_LACK
        if ( ( tileData[0] >> 6 ) & 3 )
          pSprBuf[ nX ]     = bySprCol | (( tileData[0] >> 6 ) & 3);
        if ( ( tileData[1] >> 6 ) & 3 )
          pSprBuf[ nX + 1 ] = bySprCol | (( tileData[1] >> 6 ) & 3);
        if ( ( tileData[0] >> 4 ) & 3 )
          pSprBuf[ nX + 2 ] = bySprCol | (( tileData[0] >> 4 ) & 3);
        if ( ( tileData[1] >> 4 ) & 3 )
          pSprBuf[ nX + 3 ] = bySprCol | (( tileData[1] >> 4 ) & 3);
        if ( ( tileData[0] >> 2 ) & 3 )
          pSprBuf[ nX + 4 ] = bySprCol | (( tileData[0] >> 2 ) & 3);
        if ( ( tileData[1] >> 2 ) & 3 )
          pSprBuf[ nX + 5 ] = bySprCol | (( tileData[1] >> 2 ) & 3);
        if ( ( tileData[0] ) & 3 )
          pSprBuf[ nX + 6 ] = bySprCol | (( tileData[0] ) & 3);
        if ( ( tileData[1] ) & 3 )
          pSprBuf[ nX + 7 ] = bySprCol | (( tileData[1] ) & 3);
#else
        if ( pbyChrData[ 0 ] )
          pSprBuf[ nX ]     = bySprCol | pbyChrData[ 0 ];
        if ( pbyChrData[ 1 ] )
          pSprBuf[ nX + 1 ] = bySprCol | pbyChrData[ 1 ];
        if ( pbyChrData[ 2 ] )
          pSprBuf[ nX + 2 ] = bySprCol | pbyChrData[ 2 ];
        if ( pbyChrData[ 3 ] )
          pSprBuf[ nX + 3 ] = bySprCol | pbyChrData[ 3 ];
        if ( pbyChrData[ 4 ] )
          pSprBuf[ nX + 4 ] = bySprCol | pbyChrData[ 4 ];
        if ( pbyChrData[ 5 ] )
          pSprBuf[ nX + 5 ] = bySprCol | pbyChrData[ 5 ];
        if ( pbyChrData[ 6 ] )
          pSprBuf[ nX + 6 ] = bySprCol | pbyChrData[ 6 ];
        if ( pbyChrData[ 7 ] )
          pSprBuf[ nX + 7 ] = bySprCol | pbyChrData[ 7 ];
#endif /* RAM_LACK */
      }
    }

    // Rendering sprite
    pPoint -= ( NES_DISP_WIDTH - PPU_Scr_H_Bit );
    for ( nX = 0; nX < NES_DISP_WIDTH; ++nX )
    {
      nSprData = pSprBuf[ nX ];
      if ( nSprData  && ( nSprData & 0x80 || pPoint[ nX ] & TRANSPARENT_COLOR ) )
      {
        pPoint[ nX ] = PalTable[ ( nSprData & 0xf ) + 0x10 ];
      }
    }

    /*-------------------------------------------------------------------*/
    /*  Sprite Clipping                                                  */
    /*-------------------------------------------------------------------*/
    if ( !( PPU_R1 & R1_CLIP_SP ) )
    {
      WORD *pPointTop;

#if WORKFRAME_DEFINE == WORKFRAME_NONE
			pPointTop = WorkLine;
#else
			pPointTop = &WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];
#endif
      InfoNES_MemorySet( pPointTop, 0, 8 << 1 );
    }

    if ( nSprCnt >= 8 )
      PPU_R2 |= R2_MAX_SP;  // Set a flag of maximum sprites on scanline
  }
}

/*===================================================================*/
/*                                                                   */
/* InfoNES_GetSprHitY() : Get a position of scanline hits sprite #0  */
/*                                                                   */
/*===================================================================*/
void InfoNES_GetSprHitY()
{
/*
 * Get a position of scanline hits sprite #0
 *
 */
  int nYBit;
	int nLine;
#ifdef RAM_LACK
  BYTE *pbyChrData;
#else
  DWORD *pdwChrData;
#endif
  int nOff;

  if ( SPRRAM[ SPR_ATTR ] & SPR_ATTR_V_FLIP )
  {
    // Vertical flip
    nYBit = ( PPU_SP_Height - 1 );
#ifdef RAM_LACK
    nOff = -1;
#else
    nYBit <<= 3;
    nOff = -2;
#endif /* RAM_LACK */
  }
  else
  {
    // Non flip
    nYBit = 0;
#ifdef RAM_LACK
    nOff = 1;
#else
    nOff = 2;
#endif /* RAM_LACK */
  }

  if ( PPU_R0 & R0_SP_SIZE )
  {
    // Sprite size 8x16
    if ( SPRRAM[ SPR_CHR ] & 1 )
    {
#ifdef RAM_LACK
      pbyChrData = ( PPUBANK[ 4 ] + ( ( SPRRAM[ SPR_CHR ] & 0xfe ) << 4 ) + ((nYBit & 0x8) << 1) + (nYBit & 0x7) );
#else
      pdwChrData = (DWORD *)( ChrBuf + 256 * 64 + ( ( SPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit );
#endif /* RAM_LACK */
    }
    else
    {
#ifdef RAM_LACK
      pbyChrData = ( PPUBANK[ 0 ] + ( ( SPRRAM[ SPR_CHR ] & 0xfe ) << 4 ) + ((nYBit & 0x8) << 1) + (nYBit & 0x7) );
#else
      pdwChrData = (DWORD * )( ChrBuf + ( ( SPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit );
#endif /* RAM_LACK */
    } 
  }
  else
  {
    // Sprite size 8x8
#ifdef RAM_LACK
    pbyChrData = ( PPU_SP_Base + ( SPRRAM[ SPR_CHR ] << 4 ) + nYBit );
#else
    pdwChrData = (DWORD *)( PPU_SP_Base + ( SPRRAM[ SPR_CHR ] << 6 ) + nYBit );
#endif /* RAM_LACK */
  }

  if ( ( SPRRAM[ SPR_Y ] + 1 <= SCAN_UNKNOWN_START ) && ( SPRRAM[SPR_Y] > 0 ) )
	{
		for ( nLine = 0; nLine < PPU_SP_Height; nLine++ )
		{
#ifdef RAM_LACK
			if ( pbyChrData[ 0 ] | pbyChrData[ 8 ] )
#else
			if ( pdwChrData[ 0 ] | pdwChrData[ 1 ] )
#endif /* RAM_LACK */
			{
        // Scanline hits sprite #0
				SpriteJustHit = SPRRAM[SPR_Y] + 1 + nLine;
				nLine = SCAN_VBLANK_END;
			}
#ifdef RAM_LACK
      if( nLine == 7)
      {
        pbyChrData += nOff * 9;
      }
      else
      {
        pbyChrData += nOff;
      }
#else
			pdwChrData += nOff;
#endif /* RAM_LACK */
		}
  } else {
    // Scanline didn't hit sprite #0
		SpriteJustHit = SCAN_UNKNOWN_START + 1;
  }
}

#ifndef RAM_LACK
/*===================================================================*/
/*                                                                   */
/*            InfoNES_SetupChr() : Develop character data            */
/*                                                                   */
/*===================================================================*/
void InfoNES_SetupChr()
{
/*
 *  Develop character data
 *
 */

  BYTE *pbyBGData;
  BYTE byData1;
  BYTE byData2;
  int nIdx;
  int nY;
  int nOff;
  static BYTE *pbyPrevBank[ 8 ];
  int nBank;

  for ( nBank = 0; nBank < 8; ++nBank )
  {
    if ( pbyPrevBank[ nBank ] == PPUBANK[ nBank ] && !( ( ChrBufUpdate >> nBank ) & 1 ) )
      continue;  // Next bank

    /*-------------------------------------------------------------------*/
    /*  An address is different from the last time                       */
    /*    or                                                             */
    /*  An update flag is being set                                      */
    /*-------------------------------------------------------------------*/

    for ( nIdx = 0; nIdx < 64; ++nIdx )
    {
      nOff = ( nBank << 12 ) + ( nIdx << 6 );

      for ( nY = 0; nY < 8; ++nY )
      {
        pbyBGData = PPUBANK[ nBank ] + ( nIdx << 4 ) + nY;

        byData1 = ( ( pbyBGData[ 0 ] >> 1 ) & 0x55 ) | ( pbyBGData[ 8 ] & 0xAA );
        byData2 = ( pbyBGData[ 0 ] & 0x55 ) | ( ( pbyBGData[ 8 ] << 1 ) & 0xAA );

        ChrBuf[ nOff ]     = ( byData1 >> 6 ) & 3;
        ChrBuf[ nOff + 1 ] = ( byData2 >> 6 ) & 3;
        ChrBuf[ nOff + 2 ] = ( byData1 >> 4 ) & 3;
        ChrBuf[ nOff + 3 ] = ( byData2 >> 4 ) & 3;
        ChrBuf[ nOff + 4 ] = ( byData1 >> 2 ) & 3;
        ChrBuf[ nOff + 5 ] = ( byData2 >> 2 ) & 3;
        ChrBuf[ nOff + 6 ] = byData1 & 3;
        ChrBuf[ nOff + 7 ] = byData2 & 3;

        nOff += 8;
      }
    }
    // Keep this address
    pbyPrevBank[ nBank ] = PPUBANK[ nBank ];
  }

  // Reset update flag
  ChrBufUpdate = 0;
}
#endif /* !RAM_LACK */

#ifdef __cplusplus 
} 
#endif /* __cplusplus */ 

