/************************************************************************
 *  memory.h
 *                      Memory emulation routines
 *
 ************************************************************************
 *
 * Phoinix,
 * Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
 *
 * (c)2000-2005 Bodo Wenzel
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
 *  $Log: memory.h,v $
 *  Revision 1.7  2005/05/26 19:35:47  bodowenzel
 *  Moved game's info into game's entry, removing MemoryMbcInfo
 *
 *  Revision 1.6  2005/04/03 14:08:34  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.5  2005/01/29 10:26:08  bodowenzel
 *  Manager form uses a scrollable table now for the list of games
 *
 *  Revision 1.4  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.3  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:03  bodowenzel
 *  Import
 *
 ************************************************************************
 */

#ifndef MEMORY_H
#define MEMORY_H

/* === Includes =======================================================	*/

#include "sections.h"

/* === Constants ======================================================	*/

#define memoryMaxRomPages (256)
#define memoryMaxRamPages (4)

/* === Type definitions ===============================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

typedef enum {
  memoryMbcError = 0, memoryMbcUnknown, memoryMbcDefault,
  memoryMbc1, memoryMbc2, memoryMbc3, memoryMbc5
} MemoryMbcType;

typedef struct {
  UInt8 vid8000[0x2000]; /* video RAM */
  UInt8 intC000[0x2000]; /* internal RAM */
  UInt8 oamFe00[0x0100]; /* object attribute memory */
} MemoryRamType;

typedef struct {
  UInt8 seconds;
  UInt8 minutes;
  UInt8 hours;
  UInt8 days;
  UInt8 control;
} MemoryMbc3RtcType;

/* types of variables of a state to be saved */

typedef struct {
  UInt16            romPage;
  UInt8             ramPage;
  UInt16            mbc1RomLsb;
  UInt16            mbc1RomMsb;
  UInt8             mbc1Mode;
  UInt8             mbc3Mode;
  UInt16            mbc3RtcDivider;
  MemoryMbc3RtcType mbc3RtcCurrent;
  MemoryMbc3RtcType mbc3RtcLatched;
} MemoryMbcStateType;

typedef struct {
  MemoryRamType      ram;
  MemoryMbcStateType mbcState;
} MemoryStateType;

#endif

/* === Global variables ===============================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

/* NOTE:
 * As GB addresses are used as offsets, the addresses for Palm memory
 * blocks are offset by the base addresses.
 */
extern UInt8 *MemoryRomStart;
extern UInt8 *MemoryRomPages[memoryMaxRomPages];
extern UInt8 *MemoryRamPages[memoryMaxRamPages];

extern UInt8 *MemoryBaseRom0;
extern UInt8 *MemoryBaseRom1;
extern UInt8 *MemoryBaseRamVid;
extern UInt8 *MemoryBaseRamExt;
extern UInt8 *MemoryBaseRamInt;
extern UInt8 *MemoryBaseRamOam;

extern UInt16 MemoryMbc1RomLsb;
extern UInt16 MemoryMbc1RomMsb;
extern UInt8  MemoryMbc1Mode;

extern UInt16            MemoryMbc3RtcDivider; /* dividing frame rate */
extern MemoryMbc3RtcType MemoryMbc3RtcCurrent;
extern MemoryMbc3RtcType MemoryMbc3RtcLatched;
extern UInt8             MemoryMbc3Mode;
extern UInt8             *MemoryMbc3RtcPtr; /* ptr to current register */

#endif

/* === Function prototypes in C part ==================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

Boolean MemoryRetrieveMbcInfo(void)
  MEMORY_SECTION;

Boolean MemorySetup(void)
  MEMORY_SECTION;
void MemoryCleanup(void)
  MEMORY_SECTION;
void MemoryResetState(void *recP, UInt32 offset)
  MEMORY_SECTION;
void MemoryLoadState(MemoryStateType *stateP)
  MEMORY_SECTION;
void MemorySaveState(void *recP, UInt32 offset)
  MEMORY_SECTION;

#endif

/* === Function prototypes in assembler part ==========================	*/

#ifdef __GNUC__ /* only if C preprocessing */

/* NOTE:
 * These functions are called from the emulation and get their parameters
 * via several registers. This is not documented here, the prototypes are
 * just for exporting the names.
 */

void MemoryMbcDummy(void)
  MEMORY_SECTION;
void MemoryMbc1RomSelect(void)
  MEMORY_SECTION;
void MemoryMbc1RomSelectL(void)
  MEMORY_SECTION;
void MemoryMbc1RomSelectH(void)
  MEMORY_SECTION;
void MemoryMbc1RamSelect(void)
  MEMORY_SECTION;
void MemoryMbc1ModeSelect(void)
  MEMORY_SECTION;
void MemoryMbc2RomSelect(void)
  MEMORY_SECTION;
void MemoryMbc3RomSelect(void)
  MEMORY_SECTION;
void MemoryMbc3RamSelect(void)
  MEMORY_SECTION;
void MemoryMbc3RtcRd(void)
  MEMORY_SECTION;
void MemoryMbc3RtcWr(void)
  MEMORY_SECTION;
void MemoryMbc3RtcLatch(void)
  MEMORY_SECTION;
void MemoryMbc3RtcWork(void)
  MEMORY_SECTION;
void MemoryMbc5RomSelect(void)
  MEMORY_SECTION;
void MemoryMbc5RamSelect(void)
  MEMORY_SECTION;

void MemoryRamRdExtNone(void)
  MEMORY_SECTION;
void MemoryRamWrExtNone(void)
  MEMORY_SECTION;
void MemoryRamRdExtMbc2(void)
  MEMORY_SECTION;
void MemoryRamWrExtMbc2(void)
  MEMORY_SECTION;
void MemoryRamRdExt2K(void)
  MEMORY_SECTION;
void MemoryRamWrExt2K(void)
  MEMORY_SECTION;
void MemoryRamRdExt8K(void)
  MEMORY_SECTION;
void MemoryRamWrExt8K(void)
  MEMORY_SECTION;

void MemoryRamNone(void)
  MEMORY_SECTION;
void MemoryRamTileBg(void)
  MEMORY_SECTION;
void MemoryRamTileBgWi(void)
  MEMORY_SECTION;
void MemoryRamTileOb(void)
  MEMORY_SECTION;
void MemoryRamTileBgOb(void)
  MEMORY_SECTION;
void MemoryRamTileBgWiOb(void)
  MEMORY_SECTION;
void MemoryRamMapNone(void)
  MEMORY_SECTION;
void MemoryRamMapBg(void)
  MEMORY_SECTION;
void MemoryRamMapWi(void)
  MEMORY_SECTION;
void MemoryRamMapBgWi(void)
  MEMORY_SECTION;
void MemoryRamOam(void)
  MEMORY_SECTION;
void MemoryRamOamEntry(void)
  MEMORY_SECTION;

#endif

/* === The end ========================================================	*/

#endif
