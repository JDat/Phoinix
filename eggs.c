/************************************************************************
 *  eggs.c
 *                      Easter eggs
 *
 ************************************************************************
 *
 * Phoinix,
 * Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
 *
 * (c)2000-2006 Bodo Wenzel
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
 * Notes:
 *
 * This source is also compiled and run on the development host to
 * generate the default hidden game.
 *
 * Run length encoding scheme, all values are in hex:
 * 80             = start of compressed data
 * 1 to n entries = compressed data
 * 00             = end of compressed data
 * An entry starts with a counter, flagged in the most significant bit:
 * [01..7F] 1 to 127 bytes = 1 to 127 bytes uncompressed data
 * [81..FF] byte           = 1 to 127 bytes compressed data
 ************************************************************************
 *  History:
 *
 *  $Log: eggs.c,v $
 *  Revision 1.4  2006/04/25 18:51:40  bodowenzel
 *  Corrections around the easter egg
 *
 *  Revision 1.3  2005/05/26 19:35:46  bodowenzel
 *  Moved game's info into game's entry, removing MemoryMbcInfo
 *
 *  Revision 1.2  2005/04/03 14:08:33  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.1  2005/02/27 16:38:24  bodowenzel
 *  Added module Eggs for easter eggs
 *
 ************************************************************************
 */

/* IMPORTANT!
 * Please make sure that any referenced resource is included!
 * Unfortunately there is no automatic!
 */

#ifdef __palmos__ /******************************************************/

/* === Includes =======================================================	*/

#include <PalmOS.h>

#include "eggs.h"

#include "Phoinix.h"
#include "misc.h"
#include "prefs.h"
#include "states.h"
#include "ram.h"
#include "vfs.h"
#include "gbemu.h"
#include "memory.h"

/* === Constants ======================================================	*/

#define hiddenGameDbName "Phoinix-HiddenGame"

#else /* defined(__palmos__) ********************************************/

/* === Adjustments ====================================================	*/

#define EGGS_SECTION

typedef enum { false = 0, true } Boolean;

typedef unsigned char  UInt8;

typedef unsigned short UInt16;

/* === Includes =======================================================	*/

#include <stdio.h>
#include <stdlib.h>

#include "gbemu.h"

#endif /* defined(__palmos__) else **************************************/

/* === Constants ======================================================	*/

/* the combos are numbered first 0x80 to 0xff, then 0x00 to 0x7f */
#define maxCombo (((gbemuLastUnused ^ 0x80) & 0xFF) + 1)

/* only 32K games are supported */
#define expandedSize   (32 * 1024L)
#define compressedSize (gbemuXtSize * maxCombo)

#define maxCount       (0x7F)
#define compressedFlag (0x80)
#define endMarker      (0x00)

/* === Function prototypes ============================================	*/

static Boolean Compress(UInt8 *destP, const UInt8 *srceP)
  EGGS_SECTION;

#ifdef __palmos__ /******************************************************/

/* === Type definitions ===============================================	*/

typedef struct {
  const Char *code;         /* keys to enter, must be more than 1! */
  void       (*func)(void); /* function to call on completion */
} FsmType;

/* === Function prototypes ============================================	*/

static void RunHiddenGame(void)
  EGGS_SECTION;
static Boolean ExtractHiddenGame(void)
  EGGS_SECTION;
static Boolean SetupHiddenGame(void)
  EGGS_SECTION;

static void SaveAsHiddenGame(void)
  EGGS_SECTION;

static Boolean Expand(UInt8 *destP, const UInt8 *srceP)
  EGGS_SECTION;

/* === Static variables ===============================================	*/

static const FsmType Fsm[] = {
  { "run",  RunHiddenGame },
  { "save", SaveAsHiddenGame },
};

static UInt16 FsmIndex;
static UInt16 CodeIndex = 0;

/* === Handle keys ====================================================	*/

/**
 * Handles entered keys and if triggered starts the secret action.
 *
 * @param key entered key.
 */
void EggsHandleKey(WChar key) {
  /* the system global switch has to be "on" */
  if (PrefGetPreference(prefAllowEasterEggs) == 0) {
    return;
  }

  /* check if it's the next expected key */
  if (CodeIndex != 0) {
    if (key == Fsm[FsmIndex].code[CodeIndex]) {
      CodeIndex++;
      if (Fsm[FsmIndex].code[CodeIndex] == '\0') {
	/* code is complete, call function */
	CodeIndex = 0;
	(*Fsm[FsmIndex].func)();
      }
    } else {
      CodeIndex = 0;
    }
  }

  /* check for a new code */
  if (CodeIndex == 0) {
    for (FsmIndex = 0;
	 FsmIndex < sizeof(Fsm) / sizeof(FsmType);
	 FsmIndex++) {
      if (key == Fsm[FsmIndex].code[0]) {
	CodeIndex++;
	break;
      }
    }
  }
}

/* === Hidden game ====================================================	*/

/**
 * Runs the hidden game. Aborts silently on errors.
 */
static void RunHiddenGame(void) {
  /* extract the hidden game */
  if (ExtractHiddenGame()) {
    /* if possible, run the hidden game, else refresh the manager form */
    if (SetupHiddenGame()) {
      FrmGotoForm(formIdEmulation);
    } else {
      FrmGotoForm(formIdManager);
    }
  }
}

/**
 * Extracts the hidden game.
 *
 * @return true if successful.
 */
static Boolean ExtractHiddenGame(void) {
  MemHandle emulatorH;

  /* get and lock the emulator kernel */
  emulatorH = DmGetResource('code', codeIdKernel);
  if (emulatorH != NULL) {
    GbemuComboType *emulator;
    LocalID        dbId = 0;
    DmOpenRef      dbP = NULL;

    emulator = (GbemuComboType *)MemHandleLock(emulatorH);

    /* check that a game is actually stored */
    if (emulator[0].xt.code[0] == compressedFlag) {
      /* create temporary database for the game */
      if (DmCreateDatabase(ramGameDbCardNo, hiddenGameDbName, miscCreator,
			   miscGameDbType, false) == errNone) {
	dbId = DmFindDatabase(ramGameDbCardNo, hiddenGameDbName);
	if (dbId != 0) {
	  dbP = DmOpenDatabase(ramGameDbCardNo, dbId, dmModeReadWrite);
	}
      }
    }

    if (dbP != NULL) {
      UInt16    recId;
      MemHandle recH;

      /* make some room for the game */
      recId = 0;
      recH = DmNewRecord(dbP, &recId, expandedSize + compressedSize);
      if (recH != NULL) {
	UInt8     *recP;
	UInt16    combo;
	Boolean   ok;
	MemHandle newH;

	recP = MemHandleLock(recH);

	/* expand the compressed data */
	for (combo = 0; combo < maxCombo; combo++) {
	  DmWrite(recP, expandedSize + combo * gbemuXtSize,
		  emulator[combo].xt.code, gbemuXtSize);
	}
	MemSemaphoreReserve(true);
	ok = Expand(recP, recP + expandedSize);
	MemSemaphoreRelease(true);

	/* release and free the memory for the compressed data */
	MemHandleUnlock(recH);
	DmReleaseRecord(dbP, recId, false);
	newH = DmResizeRecord(dbP, recId, expandedSize);

	if (recId == 0 && ok && newH != NULL) {
	  /* the hidden game is successfully extracted */
	  DmCloseDatabase(dbP);
	  MemHandleUnlock(emulatorH);
	  DmReleaseResource(emulatorH);
	  return true;
	}
      }
      DmCloseDatabase(dbP);
      DmDeleteDatabase(ramGameDbCardNo, dbId);
    }
    MemHandleUnlock(emulatorH);
    DmReleaseResource(emulatorH);
  }
  return false;
}

/**
 * Sets up the hidden game.
 *
 * @return true if sucessful.
 */
static Boolean SetupHiddenGame(void) {
  /* close state database of old game, if open */
  StatesClosePrefsAndStatesWithShow(MiscShowError, true);

  /* erase current volume name to flag the hidden game */
  PrefsPreferences.volumeName[0] = '\0';

  /* create dummy game entry */
  StrCopy(PrefsPreferences.game.name, hiddenGameDbName);
  PrefsPreferences.game.volIndex = vfsRamVolIndex;

  /* set up the emulation */
  if (MemoryRetrieveMbcInfo()) {
    if (StatesOpenPrefsAndStates()) {
      if (EmulationSetupAll()) {
	StatesLoadCurrentState();
	return true;
      }
    }
  }
  return false;
}

/**
 * Removes the temporary database with the hidden game.
 */
void EggsRemoveHiddenGame(void) {
  LocalID   dbId;

  dbId = DmFindDatabase(ramGameDbCardNo, hiddenGameDbName);
  if (dbId != 0) {
    DmDeleteDatabase(ramGameDbCardNo, dbId);
  }
}

/**
 * Saves the current game as the hidden game. Aborts silently on errors.
 */
static void SaveAsHiddenGame(void) {
  MemHandle recH;

  /* check for an appropriate game */
  if (PrefsPreferences.game.name[0] == '\0') {
    return;
  }
  if (PrefsPreferences.game.mbc <= memoryMbcUnknown) {
    return;
  }
  if (PrefsPreferences.game.romSize > (UInt16)(expandedSize / 1024)) {
    return;
  }

  /* get and lock the image of the current game */
#if expandedSize != 32 * 1024
#error "Adjust saving a new hidden game"
#endif
  if (PrefsPreferences.game.volIndex == vfsRamVolIndex) {
    recH = RamGetFirstBlock(&PrefsPreferences.game);
  } else {
    recH = VfsGetFirstBlock(&PrefsPreferences.game);
  }
  if (recH != NULL) {
    UInt8 *expandedP, *compressedP;

    expandedP = MemHandleLock(recH);

    /* try to compress the current game */
    compressedP = MemPtrNew(compressedSize);
    if (compressedP != NULL) {
      if (Compress(compressedP, expandedP)) {
	MemHandle emulatorH;

	/* get and lock the emulator kernel */
	emulatorH = DmGetResource('code', codeIdKernel);
	if (emulatorH != NULL) {
	  GbemuComboType *emulator;
	  UInt16         combo;

	  emulator = (GbemuComboType *)MemHandleLock(emulatorH);

	  /* copy new hidden game into emulator kernel */
	  for (combo = 0; combo < maxCombo; combo++) {
	    DmWrite(emulator, combo * 0x100 + gbemuXtOffset,
		    compressedP + combo * gbemuXtSize, gbemuXtSize);
	  }
	  MemHandleUnlock(emulatorH);
	  DmReleaseResource(emulatorH);
	}
      }
      MemPtrFree(compressedP);
    }
    MemHandleUnlock(recH);
    MemHandleFree(recH);
  }
}

#else /* defined(__palmos__) ********************************************/

/* === Main ===========================================================	*/

/**
 * Generates the macros with the compressed data of a hidden game. Both
 * filenames of a game and the destination source file are expected as
 * command line arguments. If the game's file doesn't exist, all macros
 * contain empty data.
 *
 * On errors the application aborts. All cleanup is left to the C runtime
 * system.
 *
 * @param argc number of arguments in argv[].
 * @param argv pointer to array of pointers to arguments.
 * @return     EXIT_SUCCESS or EXIT_FAILURE.
 */
int main(int argc, char *argv[]) {
  UInt8 *srceP, *destP;
  FILE  *file;
  int   offset, macro;

  /* check for filename */
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <gb-file> <dest-file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  /* obtain buffers */
  srceP = malloc(expandedSize);
  destP = malloc(compressedSize);
  if (srceP == NULL || destP == NULL) {
    fputs("Memory full!\n", stderr);
    return EXIT_FAILURE;
  }
  memset(destP, endMarker, compressedSize);

  /* read hidden game, and compress, if any */
  file = fopen(argv[1], "rb");
  if (file != NULL) {
    /* check length */
    fseek(file, 0, SEEK_END);
    if (ftell(file) != expandedSize) {
      fputs("Wrong size of game file!\n", stderr);
      return EXIT_FAILURE;
    }
    fseek(file, 0, SEEK_SET);

    /* read data */
    if (fread(srceP, sizeof(UInt8), expandedSize, file) != expandedSize) {
      fputs("Read error on game file!\n", stderr);
      return EXIT_FAILURE;
    }

    /* compress data */
    if (!Compress(destP, srceP)) {
      fputs("Compression error!\n", stderr);
      return EXIT_FAILURE;
    }

    fclose(file);
  }

  /* write macros */
  file = fopen(argv[2], "w");
  if (file == NULL) {
    fputs("Can't create destination file!\n", stderr);
    return EXIT_FAILURE;
  }

  /* print header */
  fprintf(file, "/* Automatically generated, don't edit! */\n\n");

  /* generate macros */
  offset = 0;
  for (macro = 0; macro < maxCombo; macro++) {
    int index;

    fprintf(file, ".macro\txt_%02x\n"
	    "xt_%02x:\n", macro ^ 0x80, macro ^ 0x80);
    for (index = 0; index < gbemuXtSize; index++) {
      fprintf(file, "\t.byte\t0x%02x\n", destP[offset++]);
    }
    fprintf(file, "\t.endm\n"
	    "\n");
  }

  /* print footer */
  fprintf(file, "/* The end */\n");

  /* check for write errors */
  if (ferror(file)) {
    fputs("Write error on destination file!\n", stderr);
    return EXIT_FAILURE;
  }

  /* all is well ;-) */
  fclose(file);
  return EXIT_SUCCESS;
}

#endif /* defined(__palmos__) else **************************************/

/* === Compress and expand a game =====================================	*/

/**
 * Compresses by run length encoding. The buffer sizes are fixed.
 *
 * @param destP pointer to the destination buffer.
 * @param srceP pointer to the source buffer.
 * @return      true if successful.
 */
static Boolean Compress(UInt8 *destP, const UInt8 *srceP) {
  UInt16 destCount, srceCount;

  /* set flag */
  *destP++ = compressedFlag;
  destCount = 1;

  srceCount = 0;
  for (;;) {
    UInt8   count, *currP, curr;
    Boolean runFound;

    /* remember start of entry */
    currP = destP++;
    if (++destCount == compressedSize) {
      return false;
    }

    /* collect uncompressable data */
    runFound = false;
    for (count = 0; count < maxCount; count++) {
      curr = *srceP;
      *destP++ = *srceP++;
      if (++destCount == compressedSize) {
	return false;
      }
      if (++srceCount == expandedSize) {
	*currP = ++count & ~compressedFlag;
	*destP = endMarker;
	return true;
      }
      if (srceCount < expandedSize - 1) {
	if (curr == srceP[0] && curr == srceP[1]) {
	  runFound = true;
	  break;
	}
      }
    }

    /* if there are uncompressable data, store count
     * then start a new entry
     */
    if (count != 0) {
      *currP = count & ~compressedFlag;
      if (runFound) {
	currP = destP - 1;
	*destP++ = curr;
	if (++destCount == compressedSize) {
	  return false;
	}
      }
    }

    /* if compressable data were found, find its end */
    if (runFound) {
      for (count = 1; count < maxCount && curr == *srceP; count++) {
	srceP++;
	if (++srceCount == expandedSize) {
	  *currP = ++count | compressedFlag;
	  *destP = endMarker;
	  return true;
	}
      }
      *currP = count | compressedFlag;
    }
  }
}

#if defined(__palmos__) /************************************************/

/**
 * Expands the run length encoded data. The buffer sizes are fixed.
 *
 * @param destP pointer to the destination buffer.
 * @param srceP pointer to the source buffer.
 * @return      true if successful.
 */
static Boolean Expand(UInt8 *destP, const UInt8 *srceP) {
  UInt16 destCount, srceCount;

  /* check for compressed data */
  if (*srceP++ != compressedFlag) {
    return false;
  }

  srceCount = 1;
  destCount = 0;
  for (;;) {
    UInt8 count;

    if (srceCount++ == compressedSize) {
      return false;
    }

    count = *srceP++;
    if (count == endMarker) {
      /* end of compressed data reached */
      return destCount == expandedSize;
    }

    if ((count & compressedFlag) == 0) {
      /* copy uncompressed data */
      do {
	if (destCount++ == expandedSize) {
	  return false;
	}
	if (srceCount++ == compressedSize) {
	  return false;
	}
	*destP++ = *srceP++;
      } while (--count != 0);

    } else {
      /* expand compressed data */
      UInt8 data;

      if (srceCount++ == compressedSize) {
	return false;
      }
      data = *srceP++;
      count &= ~compressedFlag;
      do {
	if (destCount++ == expandedSize) {
	  return false;
	}
	*destP++ = data;
      } while (--count != 0);
    }
  }
}

#endif /* defined(__palmos__) *******************************************/

/* === The end ========================================================	*/
