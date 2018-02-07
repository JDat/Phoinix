/************************************************************************
 *  emulation.h
 *                      Main emulation routines
 *
 ************************************************************************
 *
 * Phoinix,
 * Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
 *
 * (c)2000-2007 Bodo Wenzel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 ************************************************************************
 *  History:
 *
 *  $Log: emulation.h,v $
 *  Revision 1.14  2007/02/11 17:14:43  bodowenzel
 *  Multiple saved state feature implemented
 *
 *  Revision 1.13  2005/05/03 08:39:11  bodowenzel
 *  Moved coldReset, ime, im and error into EmulationCpuState
 *
 *  Revision 1.12  2005/04/03 14:08:33  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.11  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.10  2005/01/29 10:25:37  bodowenzel
 *  Added const qualifier to pointer parameters
 *
 *  Revision 1.9  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.8  2004/12/02 20:01:22  bodowenzel
 *  New configurable screen layout
 *
 *  Revision 1.7  2004/10/24 09:11:05  bodowenzel
 *  New button mapping
 *
 *  Revision 1.6  2004/09/19 12:36:25  bodowenzel
 *  New info type (None)
 *
 *  Revision 1.5  2004/06/06 09:25:01  bodowenzel
 *  Change game's location to RAM on error saving states
 *
 *  Revision 1.4  2002/10/19 08:08:10  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.3  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.2  2001/12/28 18:28:04  bodowenzel
 *  Bug found, if restarted twice without running (GB-PC was reset)
 *
 *  Revision 1.1.1.1  2001/12/16 13:37:54  bodowenzel
 *  Import
 *
 ************************************************************************
 */

#ifndef EMULATION_H
#define EMULATION_H

/* === Includes =======================================================	*/

#include "sections.h"
#include "gbemu.h"

/* === Constants ======================================================	*/

#define emulationKeyCount (8)

/* offsets, depending on Gameboy hardware */
#define emulationJoypadBoth (0x0)
#define emulationJoypadCtrl (0x1)
#define emulationJoypadCurs (0x2)
#define emulationJoypadNone (0x3)

#define emulationMaxTime (100UL * 60 * 60)

/* === Type definitions ===============================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

typedef struct {
/* NOTE: this has to match the .bss section in emulation_s.S! */
  UInt8 *pcBase;
  Int16 pcOffset;
  UInt8 *spBase;
  Int16 spOffset;
  UInt8 a;
  UInt8 f;
  UInt8 b;
  UInt8 c;
  UInt8 d;
  UInt8 e;
  UInt8 h;
  UInt8 l;
  UInt8 coldReset;
  UInt8 ime;
  UInt8 im;
  UInt8 error;
} EmulationCpuStateType;

typedef struct {
/* NOTES:
 * This has to match the .bss section in emulation_s.S!
 * The layout has to be equal to the layout of the GB!
 */
  UInt8 p1;                   /* joypad, used bit 1 and 0 */
  UInt8 sb;                   /* serial transfer data */
  UInt8 sc;                   /* serial transfer control */
  UInt8 nonExists03;
  UInt8 div;                  /* divider, not used */
  UInt8 tima;                 /* timer counter, not used */
  UInt8 tma;                  /* timer modulo */
  UInt8 tca;                  /* timer control */
  UInt8 nonExists08_0e[0x07];
  UInt8 if_;                  /* interrupt flags */
  UInt8 ch1_ent;              /* sound, not emulated */
  UInt8 ch1_wave;             /* sound, not emulated */
  UInt8 ch1_env;              /* sound, not emulated */
  UInt8 ch1_freq_lo;          /* sound, not emulated */
  UInt8 ch1_freq_hi_kick;     /* sound, not emulated */
  UInt8 nonExists15;
  UInt8 ch2_wave;             /* sound, not emulated */
  UInt8 ch2_env;              /* sound, not emulated */
  UInt8 ch2_freq_lo;          /* sound, not emulated */
  UInt8 ch2_freq_hi_kick;     /* sound, not emulated */
  UInt8 ch3_onoff;            /* sound, not emulated */
  UInt8 ch2_length;           /* sound, not emulated */
  UInt8 ch3_volume;           /* sound, not emulated */
  UInt8 ch3_freq_lo;          /* sound, not emulated */
  UInt8 ch3_freq_hi_kick;     /* sound, not emulated */
  UInt8 nonExists1f;
  UInt8 ch4_length;           /* sound, not emulated */
  UInt8 ch4_env;              /* sound, not emulated */
  UInt8 ch4_poly;             /* sound, not emulated */
  UInt8 ch4_kick;             /* sound, not emulated */
  UInt8 snd_vin;              /* sound, not emulated */
  UInt8 snd_stereo;           /* sound, not emulated */
  UInt8 snd_stat;             /* sound, not emulated */
  UInt8 nonExists27_2f[0x09];
  UInt8 wave[0x10];           /* sound, not emulated */
  UInt8 lcdc;                 /* LCD control */
  UInt8 stat;                 /* LCD status */
  UInt8 scy;                  /* LCD scroll y */
  UInt8 scx;                  /* LCD scroll x */
  UInt8 ly;                   /* LCD y coordinate */
  UInt8 lyc;                  /* LCD y compare */
  UInt8 dma;                  /* LCD DMA transfer */
  UInt8 bgp;                  /* LCD background palette */
  UInt8 obp0;                 /* LCD object palette 0 */
  UInt8 obp1;                 /* LCD object palette 1 */
  UInt8 wy;                   /* LCD window y */
  UInt8 wx;                   /* LCD window x */
  UInt8 nonExists4c_7f[0x34];
  UInt8 ram[0x7f];            /* CPU RAM */
  UInt8 ie;                   /* interrupt enable */
} EmulationIoType;

typedef struct {
  Boolean paused;
  Boolean boosted;
  UInt32  time;
} EmulationMiscType;

typedef struct {
  EmulationMiscType     misc;
  EmulationCpuStateType cpuState;
  EmulationIoType       io;
} EmulationStateType;

#endif

/* === Global variables ===============================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

extern EventType EmulationTheEvent;

/* Address of emulator kernel; NOTE: This points to combo 0x00 at offset
 * 0x8000 because of SInt16 indices!
 */
extern GbemuComboType *EmulationEmulator;

extern EmulationCpuStateType EmulationCpuState;

extern EmulationIoType EmulationIo; /* input/output space */

extern UInt8 *EmulationJoypadPtr;
extern UInt8 EmulationJoypad[4];
extern UInt8 EmulationJoypadOld; /* saved joypad value before user poll */

#endif

/* === Function prototypes in C part ==================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

void EmulationGetEvent(void)
  EMULATION_SECTION;

Boolean EmulationFormHandleEvent(EventType *evtP)
  EMULATION_SECTION;
Boolean EmulationUserPoll(void)
  EMULATION_SECTION;

Boolean EmulationSetupAll(void)
  EMULATION_SECTION;

Boolean EmulationSetup(void)
  EMULATION_SECTION;
void EmulationCleanup(void)
  EMULATION_SECTION;
void EmulationResetState(void *recP, UInt32 offset)
  EMULATION_SECTION;
void EmulationLoadState(const EmulationStateType *stateP)
  EMULATION_SECTION;
void EmulationSaveState(void *recP, UInt32 offset)
  EMULATION_SECTION;

#endif

/* === Function prototypes in assembler part ==========================	*/

#ifdef __GNUC__ /* only if C preprocessing */

void EmulationStartKernel(void)
  EMULATION_SECTION;

/* NOTE:
 * These functions are called from the emulation and get their parameters
 * via several registers. This is not documented here, the prototypes are
 * just for exporting the names.
 */

void EmulationOpcodeStop(void)
  EMULATION_SECTION;

#endif

/* === The end ========================================================	*/

#endif
