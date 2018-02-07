/************************************************************************
 *  emulation.h
 *                      Main emulation routines
 *
 ************************************************************************
 *
 * Phoinix,
 * Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
 *
 * (c)2000-2002 Bodo Wenzel
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 ************************************************************************
 *  History:
 *
 *  $Log$
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

#include "gbemu.h"

/* === Constants ======================================================	*/

/* offsets, depending on Gameboy hardware */
#define emulationJoypadBoth (0x0)
#define emulationJoypadCtrl (0x1)
#define emulationJoypadCurs (0x2)
#define emulationJoypadNone (0x3)

/* === Type definitions ===============================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

typedef enum {
/* NOTE: this has to match the texts in LC_.../Phoinix.rcp! */
  emulationInfoTime = 0, emulationInfoBattery, emulationInfoSpeed
} EmulationInfoType;

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
} EmulationCpuStateType;

typedef struct {
/* NOTE: this has to match the .bss section in emulation_s.S! */
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
  Boolean               running;
  UInt32                time;
  UInt8                 error;
  UInt8                 intIme;
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

extern UInt8 EmulationIntIme;
extern UInt8 EmulationIntIm;

extern UInt8 EmulationError;

extern EmulationIoType EmulationIo; /* input/output space */

extern UInt8 *EmulationJoypadPtr;
extern UInt8 EmulationJoypad[4];
extern UInt8 EmulationJoypadOld; /* saved joypad value before user poll */

#endif

/* === Function prototypes in C part ==================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

void EmulationGetEvent(void);

Boolean EmulationFormHandleEvent(EventType *evtP);
Boolean EmulationUserPoll(void);

Boolean EmulationSetupAll(void);
void EmulationCleanupAll(void);

Boolean EmulationSetup(void);
void EmulationCleanup(void);
void EmulationResetState(void);
void EmulationLoadState(EmulationStateType *stateP);
void EmulationSaveState(void *recP, UInt32 offset);

#endif

/* === Function prototypes in assembler part ==========================	*/

#ifdef __GNUC__ /* only if C preprocessing */

void EmulationStartKernel(void);

/* NOTE:
 * These functions are called from the emulation and get their parameters
 * via several registers. This is not documented here, the prototypes are
 * just for exporting the names.
 */

void EmulationOpcodeStop(void);

#endif

/* === The end ========================================================	*/

#endif
