/************************************************************************
 *  memory.h
 *                      Memory emulation routines
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
  MemoryMbcType mbc;
  const Char    *name;
  UInt32        crc;
  UInt16        romSize; /* in KB */
  UInt16        ramSize; /* in bytes */
} MemoryMbcInfoType;

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

extern MemoryMbcInfoType MemoryMbcInfo;

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

void MemoryRetrieveMbcInfo(void);

Boolean MemorySetup(void);
void MemoryCleanup(void);
void MemoryResetState(MemoryStateType *stateP);
void MemoryLoadState(MemoryStateType *stateP);
void MemorySaveState(void *recP, UInt32 offset);

#endif

/* === Function prototypes in assembler part ==========================	*/

#ifdef __GNUC__ /* only if C preprocessing */

/* NOTE:
 * These functions are called from the emulation and get their parameters
 * via several registers. This is not documented here, the prototypes are
 * just for exporting the names.
 */

void MemoryMbcDummy(void);
void MemoryMbc1RomSelect(void);
void MemoryMbc1RomSelectL(void);
void MemoryMbc1RomSelectH(void);
void MemoryMbc1RamSelect(void);
void MemoryMbc1ModeSelect(void);
void MemoryMbc2RomSelect(void);
void MemoryMbc3RomSelect(void);
void MemoryMbc3RamSelect(void);
void MemoryMbc3RtcRd(void);
void MemoryMbc3RtcWr(void);
void MemoryMbc3RtcLatch(void);
void MemoryMbc3RtcWork(void);
void MemoryMbc5RomSelect(void);
void MemoryMbc5RamSelect(void);

void MemoryRamRdExtNone(void);
void MemoryRamWrExtNone(void);
void MemoryRamRdExtMbc2(void);
void MemoryRamWrExtMbc2(void);
void MemoryRamRdExt2K(void);
void MemoryRamWrExt2K(void);
void MemoryRamRdExt8K(void);
void MemoryRamWrExt8K(void);

void MemoryRamNone(void);
void MemoryRamTileBg(void);
void MemoryRamTileBgWi(void);
void MemoryRamTileOb(void);
void MemoryRamTileBgOb(void);
void MemoryRamTileBgWiOb(void);
void MemoryRamMapNone(void);
void MemoryRamMapBg(void);
void MemoryRamMapWi(void);
void MemoryRamMapBgWi(void);
void MemoryRamOam(void);
void MemoryRamOamEntry(void);

#endif

/* === The end ========================================================	*/

#endif
