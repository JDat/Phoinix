/************************************************************************
 *  memory_c.c
 *                      Memory emulation routines (C part)
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
 *  Revision 1.3  2002/05/10 15:17:39  bodowenzel
 *  Till's VFS contribution
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:10  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-30  Bodo Wenzel   Saving states now possible
 *  2001-07-21  Bodo Wenzel   Separated from emumem.asm, changed to C
 *  2001-02-24  Bodo Wenzel   Support of more cartridges
 *  2000-11-15  Bodo Wenzel   Support of color devices
 *  2000-10-19  Bodo Wenzel   Interrupts and timer
 *  2000-09-17  Bodo Wenzel   Now nearly full screen emulation
 *  2000-09-10  Bodo Wenzel   Got from runmode.asm
 ************************************************************************
 */

/* === Includes =======================================================	*/

#include <PalmOS.h>

#include "Phoinix.h"
#include "memory.h"
#include "prefs.h"
#include "states.h"
#include "vfs.h"
#include "emulation.h"
#include "gbemu.h"

/* === Constants ======================================================	*/

#define crcSeed      (0xb0d0) /* setup for CRC checks */
#define crcBlockSize (0x1000)

/* === Type definitions ===============================================	*/

typedef struct {
  Int16         code;  /* byte in GB code */
  MemoryMbcType mbc;
  const Char    *name;
} CartridgeType;

typedef struct { /* pointer to functions */
  void *ramEnable;
  void *romSelectL;
  void *romSelectH;
  void *ramSelect;
  void *modeSelect;
} MbcVectorsType;

/* === Global and static variables ====================================	*/

MemoryMbcInfoType MemoryMbcInfo;

static const CartridgeType CartridgesKnown[] = {
  /* look into the resource about the maximum length of the name! */
  { 0x00, memoryMbcDefault, "ROM" },
  { 0x01, memoryMbc1,       "MBC1" },
  { 0x02, memoryMbc1,       "MBC1+RAM" },
  { 0x03, memoryMbc1,       "MBC1+RAM+BAT" },
  { 0x05, memoryMbc2,       "MBC2" },
  { 0x06, memoryMbc2,       "MBC2+BAT" },
  { 0x08, memoryMbcDefault, "RAM" },
  { 0x09, memoryMbcDefault, "RAM+BAT" },
  { 0x0b, memoryMbcUnknown, "MMM01" },
  { 0x0c, memoryMbcUnknown, "MMM01+RAM" },
  { 0x0d, memoryMbcUnknown, "MMM01+RAM+BAT" },
  { 0x0f, memoryMbc3,       "MBC3+BAT+RTC" },
  { 0x10, memoryMbc3,       "MBC3+RAM+BAT+RTC" },
  { 0x11, memoryMbc3,       "MBC3" },
  { 0x12, memoryMbc3,       "MBC3+RAM" },
  { 0x13, memoryMbc3,       "MBC3+RAM+BAT" },
  { 0x15, memoryMbcUnknown, "MBC4" },
  { 0x16, memoryMbcUnknown, "MBC4+RAM" },
  { 0x17, memoryMbcUnknown, "MBC4+RAM+BAT" },
  { 0x19, memoryMbc5,       "MBC5" },
  { 0x1a, memoryMbc5,       "MBC5+RAM" },
  { 0x1b, memoryMbc5,       "MBC5+RAM+BAT" },
  { 0x1c, memoryMbc5,       "MBC5+MOT" },
  { 0x1d, memoryMbc5,       "MBC5+RAM+MOT" },
  { 0x1e, memoryMbc5,       "MBC5+RAM+BAT+MOT" },
  { 0xfc, memoryMbcUnknown, "Pocket Camera" },
  { 0xfd, memoryMbcUnknown, "Bandai TAMA5" },
  { 0xfe, memoryMbcUnknown, "Hudson HuC-3" },
  { 0xff, memoryMbcUnknown, "Hudson HuC-1" },
  {   -1, memoryMbcUnknown, NULL }
};

static DmOpenRef GameDbP = NULL;
static MemHandle GameDbRecH[memoryMaxRomPages - 1];

/* NOTE:
 * As GB addresses are used as offsets, the addresses for Palm memory
 * blocks are offset by the base addresses.
 */
       UInt8  *MemoryRomStart;
       UInt8  *MemoryRomPages[memoryMaxRomPages];
static UInt16 RomPage;
       UInt8  *MemoryRamPages[memoryMaxRamPages];
static UInt8  RamPage;

UInt8 *MemoryBaseRom0;
UInt8 *MemoryBaseRom1;
UInt8 *MemoryBaseRamVid;
UInt8 *MemoryBaseRamExt;
UInt8 *MemoryBaseRamInt;
UInt8 *MemoryBaseRamOam;

static UInt16 ramPageOffset;

UInt16 MemoryMbc1RomLsb;
UInt16 MemoryMbc1RomMsb;
UInt8  MemoryMbc1Mode;

UInt16            MemoryMbc3RtcDivider; /* dividing the frame rate */
MemoryMbc3RtcType MemoryMbc3RtcCurrent;
MemoryMbc3RtcType MemoryMbc3RtcLatched;
UInt8             MemoryMbc3Mode;
UInt8             *MemoryMbc3RtcPtr; /* pointer to current register */

static const MbcVectorsType DefaultVectors = {
  MemoryMbcDummy,
  MemoryMbcDummy,
  MemoryMbcDummy,
  MemoryMbcDummy,
  MemoryMbcDummy };
static const MbcVectorsType Mbc1VectorsMode168 = {
  MemoryMbcDummy,
  MemoryMbc1RomSelectL,
  MemoryMbc1RomSelectL,
  MemoryMbc1RomSelectH,
  MemoryMbc1ModeSelect };
static const MbcVectorsType Mbc1VectorsMode432 = {
  MemoryMbcDummy,
  MemoryMbc1RomSelect,
  MemoryMbc1RomSelect,
  MemoryMbc1RamSelect,
  MemoryMbc1ModeSelect };
static const MbcVectorsType Mbc2Vectors = {
  MemoryMbcDummy,
  MemoryMbc2RomSelect,
  MemoryMbc2RomSelect,
  MemoryMbcDummy,
  MemoryMbcDummy };
static const MbcVectorsType Mbc3Vectors = {
  MemoryMbcDummy,
  MemoryMbc3RomSelect,
  MemoryMbc3RomSelect,
  MemoryMbc3RamSelect,
  MemoryMbc3RtcLatch };
static const MbcVectorsType Mbc5Vectors = {
  MemoryMbcDummy,
  MemoryMbc5RomSelect,
  MemoryMbcDummy,
  MemoryMbc5RamSelect,
  MemoryMbcDummy };

/* === Function prototypes ============================================	*/

static void SetupState(MemoryStateType *stateP);

/* === Retrieve MBC information =======================================	*/

/**
 * Retrieves and stores infos of the current game in MemoryMbcInfo.
 */
void MemoryRetrieveMbcInfo(void) {
  LocalID   dbId;
  DmOpenRef dbP=0;
  MemHandle recH;
  UInt8     *recP;
  UInt16    crc1, crc2, numRecs, i;

  /* set default value to show error */
  MemoryMbcInfo.mbc = memoryMbcError;

  /* fetch data from database stored in palms main memory */
  if(PrefsPreferences.game.volIdx == 0) {

    /* open game's database and get the first record */
    dbId = DmFindDatabase(PrefsPreferences.game.cardNo,
			  PrefsPreferences.game.name);
    if (dbId == 0)
      return;

    dbP = DmOpenDatabase(PrefsPreferences.game.cardNo, dbId, dmModeReadOnly);
    if (dbP == NULL)
      return;
    recH = DmQueryRecord(dbP, 0);
    if (recH == NULL) {
      DmCloseDatabase(dbP);
      return;
    }

    /* check size of first record */
    if (MemHandleSize(recH) != 32 * 1024L) {
      DmCloseDatabase(dbP);
      return;
    }

    recP = MemHandleLock(recH);

  } else {
    /* fetch first record from file stored in vfs */
    recH = VFSFetchHead(&PrefsPreferences.game);
    if(recH == NULL)
      return;

    /* check size of first record */
    if (MemHandleSize(recH) != 32 * 1024L) {
      MemHandleFree(recH);
      return;
    }

    if(recH == NULL)
      return;
    recP = MemHandleLock(recH);
  }

  /* calculate CRCs */
  crc1 = Crc16CalcBlock(recP + 0           , crcBlockSize, crcSeed);
  crc2 = Crc16CalcBlock(recP + crcBlockSize, crcBlockSize, crcSeed);
  MemoryMbcInfo.crc = ((UInt32)crc1 << 16) | crc2;

  /* calculate ROM size */
  MemoryMbcInfo.romSize = 32 << (recP[0x148] & 0x0f);
  if ((recP[0x148] & 0xf0) != 0)
    MemoryMbcInfo.romSize += 32 << (recP[0x148] >> 4);

  if(PrefsPreferences.game.volIdx == 0) {

    /* check count and sizes of other record */
    numRecs = DmNumRecords(dbP);
    if (numRecs > memoryMaxRomPages -1 ||
        numRecs != (MemoryMbcInfo.romSize / 16) - 1) {
      MemHandleUnlock(recH);
      DmCloseDatabase(dbP);
      return;
    }
    for (i = 1; i < numRecs; i++) {
      MemHandle otherRecH;

      otherRecH = DmQueryRecord(dbP, i);
      if (otherRecH == NULL) {
        MemHandleUnlock(recH);
        DmCloseDatabase(dbP);
        return;
      }

      if (MemHandleSize(otherRecH) != 16 * 1024L) {
        MemHandleUnlock(recH);
        DmCloseDatabase(dbP);
        return;
      }
    }
  } else {
    /* we might check the filesize here, currently i am just */
    /* too lazy (Till) */
  }

  /* scan the list of known MBC types */
  for (i = 0; CartridgesKnown[i].code != -1; i++)
    if (recP[0x147] == CartridgesKnown[i].code)
      break;
  MemoryMbcInfo.mbc = CartridgesKnown[i].mbc;
  MemoryMbcInfo.name = CartridgesKnown[i].name;

  /* calculate RAM size (in bytes needed for the RAM record!) */
  if (MemoryMbcInfo.mbc == memoryMbc2)
    MemoryMbcInfo.ramSize = 512;
  else if (recP[0x149] == 0)
    MemoryMbcInfo.ramSize = 0;
  else if (recP[0x149] <= 3)
    MemoryMbcInfo.ramSize = 512 << (recP[0x149] << 1);
  else
    MemoryMbcInfo.mbc = memoryMbcError;

  /* release everything */
  if(PrefsPreferences.game.volIdx == 0) {
    MemHandleUnlock(recH);
    DmCloseDatabase(dbP);
  } else {
    MemHandleUnlock(recH);
    MemHandleFree(recH);
  }
}

/* === Setup, cleanup, reset, load and save ===========================	*/

/**
 * Sets up all memory stuff for the emulation.
 *
 * @return true if successful.
 */
Boolean MemorySetup(void) {
  LocalID dbId;
  UInt16  numRecs, i, romPages;
  UInt16  card;

  /* load game from palm memory or from card */
  if(PrefsPreferences.game.volIdx != 0) {
    if(!VFSGameLoad(&PrefsPreferences.game, &card, &dbId))
      return false;

    GameDbP = DmOpenDatabase(card, dbId, dmModeReadOnly);
  } else {
    dbId = DmFindDatabase(PrefsPreferences.game.cardNo,
			  PrefsPreferences.game.name);
    if (dbId == 0)
      return false;

    GameDbP = DmOpenDatabase(PrefsPreferences.game.cardNo, dbId,
			     dmModeReadOnly);
  }

  if (GameDbP == NULL) {
    if(PrefsPreferences.game.volIdx != 0)
      VFSGameUnload();

    return false;
  }

  /* get all records */
  numRecs = DmNumRecords(GameDbP);
  for (i = 0; i < numRecs; i++)
    GameDbRecH[i] = DmQueryRecord(GameDbP, i);
  for (; i < memoryMaxRomPages - 1; i++)
    GameDbRecH[i] = NULL;

  /* setup ROM pages, locking all records */
  romPages = numRecs + 1;
  MemoryRomStart = MemHandleLock(GameDbRecH[0]);
  MemoryRomPages[0] = MemoryRomStart + 0x0000 - 0x4000;
  MemoryRomPages[1] = MemoryRomStart + 0x4000 - 0x4000;
  for (i = 2; i < romPages; i++)
    MemoryRomPages[i] = MemHandleLock(GameDbRecH[i - 1]) - 0x4000;
  /* most MBCs have a paging "error" */
  switch (MemoryMbcInfo.mbc) {
  case memoryMbc1:
    for (i = 0; i < romPages; i += 32)
      MemoryRomPages[i] = MemoryRomPages[i + 1];
    break;
  case memoryMbc5:
    break;
  default:
    MemoryRomPages[0] = MemoryRomPages[1];
    break;
  }
  for (i = romPages; i < memoryMaxRomPages; i++)
    MemoryRomPages[i] = MemoryRomPages[i - romPages];

  MemSemaphoreReserve(true);

  /* setup RAM access vectors */
  if (MemoryMbcInfo.ramSize == 0) {
    EmulationEmulator[gbemuJmpRamRdExt].xt.vector.address =
      MemoryRamRdExtNone;
    EmulationEmulator[gbemuJmpRamWrExt].xt.vector.address =
      MemoryRamWrExtNone;
    ramPageOffset = 0x0000;
  } else if (MemoryMbcInfo.ramSize <= 512) {
    EmulationEmulator[gbemuJmpRamRdExt].xt.vector.address =
      MemoryRamRdExtMbc2;
    EmulationEmulator[gbemuJmpRamWrExt].xt.vector.address =
      MemoryRamWrExtMbc2;
    ramPageOffset = 0x0000;
  } else if (MemoryMbcInfo.ramSize <= 2 * 1024) {
    EmulationEmulator[gbemuJmpRamRdExt].xt.vector.address =
      MemoryRamRdExt2K;
    EmulationEmulator[gbemuJmpRamWrExt].xt.vector.address =
      MemoryRamWrExt2K;
    ramPageOffset = 0x0000;
  } else if (MemoryMbcInfo.ramSize <= 8 * 1024) {
    EmulationEmulator[gbemuJmpRamRdExt].xt.vector.address =
      MemoryRamRdExt8K;
    EmulationEmulator[gbemuJmpRamWrExt].xt.vector.address =
      MemoryRamWrExt8K;
    ramPageOffset = 0x0000;
  } else {
    EmulationEmulator[gbemuJmpRamRdExt].xt.vector.address =
      MemoryRamRdExt8K;
    EmulationEmulator[gbemuJmpRamWrExt].xt.vector.address =
      MemoryRamWrExt8K;
    ramPageOffset = 0x2000;
  }

  /* this is for MBC3 which accesses its RTC via RAM adresses */
  EmulationEmulator[gbemuJmpRamRdExt].xt.vector.buffer =
    EmulationEmulator[gbemuJmpRamRdExt].xt.vector.address;
  EmulationEmulator[gbemuJmpRamWrExt].xt.vector.buffer =
    EmulationEmulator[gbemuJmpRamWrExt].xt.vector.address;

  MemSemaphoreRelease(true);
  return true;
}

/**
 * Cleans up all memory stuff after emulation.
 */
void MemoryCleanup(void) {
  UInt16 i;

  /* if game is not open, do nothing */
  if (GameDbP == NULL)
    return;

  /* unlock all records */
  for (i = 0; i < memoryMaxRomPages - 1; i++)
    if (GameDbRecH[i] != NULL)
      MemHandleUnlock(GameDbRecH[i]);

  DmCloseDatabase(GameDbP);

  if(PrefsPreferences.game.volIdx != 0)
    VFSGameUnload();

  GameDbP = NULL;
}

/**
 * Sets up all memory stuff to the "reset" state.
 *
 * @param stateP pointer to the memory state structure.
 */
void MemoryResetState(MemoryStateType *stateP) {
  UInt32       t;
  DateType     d;
  DateTimeType dt;

  RomPage = 0;
  RamPage = 0;

  MemoryMbc1RomLsb = 0;
  MemoryMbc1RomMsb = 0;
  MemoryMbc1Mode = 0;

  /* get time from OS */
  MemoryMbc3Mode = 0;
  t = TimGetSeconds();
  DateSecondsToDate(t, &d);
  TimSecondsToDateTime(t, &dt);
  MemoryMbc3RtcCurrent.control = (DateToDays(d) & 0x100) != 0 ?
    0x01 : 0x00;
  MemoryMbc3RtcCurrent.days = (UInt8)DateToDays(d);
  MemoryMbc3RtcCurrent.hours = (UInt8)dt.hour;
  MemoryMbc3RtcCurrent.minutes = (UInt8)dt.minute;
  MemoryMbc3RtcCurrent.seconds = (UInt8)dt.second;

  SetupState(stateP);
}

/**
 * Sets up all memory stuff to the given state.
 *
 * @param stateP pointer to the memory state structure.
 */
void MemoryLoadState(MemoryStateType *stateP) {
  RomPage = stateP->mbcState.romPage;
  RamPage = stateP->mbcState.ramPage;

  MemoryMbc1RomLsb = stateP->mbcState.mbc1RomLsb;
  MemoryMbc1RomMsb = stateP->mbcState.mbc1RomMsb;
  MemoryMbc1Mode = stateP->mbcState.mbc1Mode;

  MemoryMbc3Mode = stateP->mbcState.mbc3Mode;
  MemoryMbc3RtcDivider = stateP->mbcState.mbc3RtcDivider;
  MemoryMbc3RtcCurrent = stateP->mbcState.mbc3RtcCurrent;
  MemoryMbc3RtcLatched = stateP->mbcState.mbc3RtcLatched;

  SetupState(stateP);
}

/**
 * Saves the memory state in the given record.
 *
 * @param recP   pointer to the record.
 * @param offset offset into the record.
 */
void MemorySaveState(void *recP, UInt32 offset) {
  MemoryMbcStateType mbcState;

  /* look for current ROM page */
  for (mbcState.romPage = 0; mbcState.romPage < memoryMaxRomPages;
       mbcState.romPage++)
    if (MemoryRomPages[mbcState.romPage] == MemoryBaseRom1)
      break;
  mbcState.romPage %= memoryMaxRomPages;

  /* look for current RAM page */
  for (mbcState.ramPage = 0; mbcState.ramPage < memoryMaxRamPages;
       mbcState.ramPage++)
    if (MemoryRamPages[mbcState.ramPage] == MemoryBaseRamExt)
      break;
  mbcState.ramPage %= memoryMaxRamPages;

  mbcState.mbc1RomLsb = MemoryMbc1RomLsb;
  mbcState.mbc1RomMsb = MemoryMbc1RomMsb;
  mbcState.mbc1Mode = MemoryMbc1Mode;

  mbcState.mbc3Mode = MemoryMbc3Mode;
  mbcState.mbc3RtcDivider = MemoryMbc3RtcDivider;
  mbcState.mbc3RtcCurrent = MemoryMbc3RtcCurrent;
  mbcState.mbc3RtcLatched = MemoryMbc3RtcLatched;

  DmWrite(recP, offset + OffsetOf(MemoryStateType, mbcState),
	  &mbcState, sizeof(MemoryMbcStateType));
}

/**
 * Sets up common memory stuff.
 *
 * @param stateP pointer to the memory state structure.
 */
static void SetupState(MemoryStateType *stateP) {
  UInt16               i;
  const MbcVectorsType *mbcVectorsP;

  /* setup pointer to the emulated memories */
  MemoryBaseRamVid = stateP->ram.vid8000 + (0x10000 - 0x8000);
  MemoryBaseRamInt = stateP->ram.intC000 + (0x10000 - 0xc000);
  MemoryBaseRamOam = stateP->ram.oamFe00 + (0x10000 - 0xfe00);
  for (i = 0; i < memoryMaxRamPages; i++)
    MemoryRamPages[i] = StatesCartridgeRamPtr + i * ramPageOffset +
      (0x10000 - 0xa000);

  /* restore pointer to current ROM and RAM page */
  MemoryBaseRom1   = MemoryRomPages[RomPage];
  MemoryBaseRamExt = MemoryRamPages[RamPage];

  MemSemaphoreReserve(true);

  /* setup MBC vectors */
  switch (MemoryMbcInfo.mbc) {
  case memoryMbc1:
    if ((MemoryMbc1Mode & 0x01) == 0)
      mbcVectorsP = &Mbc1VectorsMode168;
    else
      mbcVectorsP = &Mbc1VectorsMode432;
    break;
  case memoryMbc2:
    mbcVectorsP = &Mbc2Vectors;
    break;
  case memoryMbc3:
    if ((MemoryMbc3Mode & 0x08) == 0) {
      EmulationEmulator[gbemuJmpRamRdExt].xt.vector.address =
	EmulationEmulator[gbemuJmpRamRdExt].xt.vector.buffer;
      EmulationEmulator[gbemuJmpRamWrExt].xt.vector.address =
	EmulationEmulator[gbemuJmpRamWrExt].xt.vector.buffer;
    } else {
      EmulationEmulator[gbemuJmpRamRdExt].xt.vector.address =
	MemoryMbc3RtcRd;
      EmulationEmulator[gbemuJmpRamWrExt].xt.vector.address =
	MemoryMbc3RtcWr;
      MemoryMbc3RtcPtr = (UInt8 *)&MemoryMbc3RtcLatched +
	(MemoryMbc3Mode & 0x07);
    }
    mbcVectorsP = &Mbc3Vectors;
    break;
  case memoryMbc5:
    mbcVectorsP = &Mbc5Vectors;
    break;
  default:
    mbcVectorsP = &DefaultVectors;
    break;
  }
  EmulationEmulator[gbemuJmpRamEnable ].xt.vector.address =
    mbcVectorsP->ramEnable;
  EmulationEmulator[gbemuJmpRomSelectL].xt.vector.address =
    mbcVectorsP->romSelectL;
  EmulationEmulator[gbemuJmpRomSelectH].xt.vector.address =
    mbcVectorsP->romSelectH;
  EmulationEmulator[gbemuJmpRamSelect ].xt.vector.address =
    mbcVectorsP->ramSelect;
  EmulationEmulator[gbemuJmpModeSelect].xt.vector.address =
    mbcVectorsP->modeSelect;

  MemSemaphoreRelease(true);
}

/* === The end ========================================================	*/
