/************************************************************************
 *  states.c
 *                      Emulation states
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
 *  $Log: states.c,v $
 *  Revision 1.27  2007/07/01 10:12:37  bodowenzel
 *  Deleting a saved state works now in VFS, too
 *
 *  Revision 1.26  2007/03/17 10:11:50  bodowenzel
 *  Set backup bit on crash database
 *
 *  Revision 1.25  2007/02/27 13:28:06  bodowenzel
 *  Show last state in Load State while emulating
 *  High resolution thumbnail
 *
 *  Revision 1.24  2007/02/11 17:14:44  bodowenzel
 *  Multiple saved state feature implemented
 *
 *  Revision 1.23  2005/05/26 19:35:47  bodowenzel
 *  Moved game's info into game's entry, removing MemoryMbcInfo
 *
 *  Revision 1.22  2005/05/06 10:32:23  bodowenzel
 *  Changed sequence of RAM and saved state in record
 *  Adding states dirty flag for fewer error messages
 *  Saving full resolution screen in state
 *
 *  Revision 1.21  2005/04/03 14:08:34  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.20  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.19  2005/01/30 19:35:27  bodowenzel
 *  Removed support for VFS mount and unmount
 *
 *  Revision 1.18  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.17  2004/12/02 20:01:22  bodowenzel
 *  New configurable screen layout
 *
 *  Revision 1.16  2004/10/24 09:11:05  bodowenzel
 *  New button mapping
 *
 *  Revision 1.15  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.14  2004/09/19 12:35:54  bodowenzel
 *  Enhanced error reporting with string parameter
 *
 *  Revision 1.13  2004/06/11 16:14:19  bodowenzel
 *  Enhanced error reporting with MiscPostError()
 *
 *  Revision 1.12  2004/06/06 09:25:01  bodowenzel
 *  Change game's location to RAM on error saving states
 *
 *  Revision 1.11  2004/04/13 19:46:07  bodowenzel
 *  Refactoring of deleting states
 *
 *  Revision 1.10  2004/04/05 21:56:55  bodowenzel
 *  VFS review and its consequences
 *
 *  Revision 1.9  2004/03/02 19:24:16  bodowenzel
 *  Changed to new error function MiscShowError()
 *
 *  Revision 1.8  2004/01/11 19:10:56  bodowenzel
 *  Added module 'ram' for on-board memory
 *  Start of VFS review and correction
 *
 *  Revision 1.7  2003/04/27 09:34:35  bodowenzel
 *  Check for game on VFS corrected
 *  Added Lite edition
 *
 *  Revision 1.6  2003/04/12 15:23:49  bodowenzel
 *  Accept version 1 of saved state
 *
 *  Revision 1.5  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.4  2002/05/10 15:17:39  bodowenzel
 *  Till's VFS contribution
 *
 *  Revision 1.3  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.2  2001/12/28 18:37:23  bodowenzel
 *  Set backup-bit of databases with states and cartridge RAM
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:38  bodowenzel
 *  Import
 *
 ************************************************************************
 */

/* IMPORTANT!
 * Please make sure that any referenced resource is included!
 * Unfortunately there is no automatic!
 */

/* === Includes =======================================================	*/

/* Access needed, see below */
#define ALLOW_ACCESS_TO_INTERNALS_OF_BITMAPS

#include <PalmOS.h>

#include "states.h"

#include "Phoinix.h"
#include "misc.h"
#include "prefs.h"
#include "manager.h"
#include "ram.h"
#include "vfs.h"
#include "emulation.h"
#include "memory.h"
#include "video.h"

/* NOTES:
 * A game's preferences and states database consists of several records:
 * #0  the preferences
 * #1  RAM (if such a cartridge) and/or state (if a state is saved),
 *     the record is minimized with no RAM and no state
 * #n  additional saved states: RAM (if such a cartridge) and state
 */

/* === Constants ======================================================	*/

#define dbType           'Save'
#define dbPrefsRecIndex  (0)
#define dbStatesRecIndex (1)

#define gamePrefsVersion (3)

#define minDummySize (1)

#define crashDbCardNo        (0)
#define crashDbName          "Phoinix-Crash"
#define crashDbType          'Boom'
#define crashDbGameRecIndex  (0)
#define crashDbStateRecIndex (1)

#define savedStateVersion   (3)

/* === Type definitions ===============================================	*/

typedef struct {
  UInt16                    version;
  PrefsEmulationOptionsType emulationOptions;
  PrefsButtonMappingType    buttonMapping;
  PrefsScreenLayoutType     screenLayout;
} GamePreferencesType;

typedef struct {
  UInt16             version;
  EmulationStateType emulation;
  MemoryStateType    memory;
  VideoStateType     video;
} SavedStateType;

/* === Global and static variables ====================================	*/

static DmOpenRef           DbP = NULL;
static GamePreferencesType GamePreferences;

PrefsEmulationOptionsType *StatesEmulationOptionsP;

PrefsButtonMappingType *StatesButtonMappingP;

PrefsScreenLayoutType *StatesScreenLayoutP;

static void           *StateP;
UInt8                 *StatesCartridgeRamPtr;
static SavedStateType *SavedStateP;
Boolean               StatesPrefsAndStatesDirty;

/* === Function prototypes ============================================	*/

static void DrawStateName(Int16 itemNum, RectangleType *bounds,
			  Char **itemsText)
  STATES_SECTION;

static void *GetStatePointer(Int16 index)
  STATES_SECTION;

/* === Create, delete, load and save preferences and states ===========	*/

/**
 * Opens states database and load preferences and RAM/state.
 *
 * @return true if successful.
 */
Boolean StatesOpenPrefsAndStates(void) {
  MemHandle recH;

  /* nothing loaded yet */
  GamePreferences.version = 0;
  StateP = NULL;
  StatesPrefsAndStatesDirty = false;

  /* provide default for preferences */
  StatesEmulationOptionsP = &PrefsPreferences.emulationOptions;
  StatesButtonMappingP = &PrefsPreferences.buttonMapping;
  StatesScreenLayoutP = &PrefsPreferences.screenLayout;

  /* open/load states database */
  if (PrefsPreferences.game.volIndex == vfsRamVolIndex) {
    DbP = RamStatesDbOpen(&PrefsPreferences.game);
  } else {
    DbP = VfsStatesDbOpen(&PrefsPreferences.game);
  }

  /* no states database found, create one, abort on error */
  if (DbP == NULL) {
    Char    dbName[dmDBNameLength];
    LocalID dbId;

    StrPrintF(dbName, statesDbNameFormat, PrefsPreferences.game.crc);
    (void)DmCreateDatabase(ramStatesDbCardNo, dbName, miscCreator, dbType,
			   false);
    dbId = DmFindDatabase(ramStatesDbCardNo, dbName);
    if (dbId == 0) {
      return false;
    }
    DbP = DmOpenDatabase(ramStatesDbCardNo, dbId, dmModeReadWrite);
    if (DbP == NULL) {
      return false;
    }
  }

  /* try to load the preferences, on errors try to correct */
  recH = DmQueryRecord(DbP, dbPrefsRecIndex);
  if (recH == NULL) {
    UInt16 recIndex;

    recIndex = dbPrefsRecIndex;
    recH = DmNewRecord(DbP, &recIndex, sizeof(GamePreferencesType));
    if (recH != NULL) {
      DmReleaseRecord(DbP, dbPrefsRecIndex, false);
    }
  } else {
    if (MemHandleSize(recH) != sizeof(GamePreferencesType)) {
      recH = DmResizeRecord(DbP, dbPrefsRecIndex,
			    sizeof(GamePreferencesType));
    } else {
      GamePreferences = *((GamePreferencesType *)MemHandleLock(recH));
      MemHandleUnlock(recH);
    }
  }
  if (recH == NULL) {
    if (PrefsPreferences.game.volIndex == vfsRamVolIndex) {
      RamStatesDbClose(DbP);
    } else {
      VfsStatesDbClose(DbP, &PrefsPreferences.game, false);
    }
    DbP = NULL;
    return false;
  }

  /* check for correct version, correct silently */
  if (GamePreferences.version != gamePrefsVersion) {
    GamePreferences.version = gamePrefsVersion;
    GamePreferences.emulationOptions = PrefsPreferences.emulationOptions;
    GamePreferences.buttonMapping = PrefsPreferences.buttonMapping;
    GamePreferences.screenLayout = PrefsPreferences.screenLayout;
    StatesPrefsAndStatesDirty = true;
  }
  StatesEmulationOptionsP = &GamePreferences.emulationOptions;
  StatesButtonMappingP = &GamePreferences.buttonMapping;
  StatesScreenLayoutP = &GamePreferences.screenLayout;

  /* at least a dummy state should exist, if possible */
  if (DmQueryRecord(DbP, dbStatesRecIndex) == NULL) {
    UInt16 recIndex;

    recIndex = dbStatesRecIndex;
    recH = DmNewRecord(DbP, &recIndex, minDummySize);
    if (recH != NULL) {
      DmReleaseRecord(DbP, recIndex, false);
    }
  }
  return true;
}

/**
 * Closes the states database after saving the preferences.
 *
 * @return true if successful.
 */
Boolean StatesClosePrefsAndStates(void) {
  MemHandle recH;
  Boolean   ok = true;

  /* if no states database is open, do nothing */
  if (DbP == NULL) {
    return ok;
  }

  /* unlock and release record of saved state */
  ok = StatesCloseCurrentState();

  /* get the record and save the current preferences */
  recH = DmGetRecord(DbP, dbPrefsRecIndex);
  if (recH == NULL) {
    ok = false;
  } else {
    DmWrite(MemHandleLock(recH), dbPrefsRecIndex, &GamePreferences,
	    sizeof(GamePreferencesType));
    MemHandleUnlock(recH);
    DmReleaseRecord(DbP, dbPrefsRecIndex, true);
  }

  /* set backup bit */
  if (StatesPrefsAndStatesDirty) {
    MiscSetBackupBit(DbP);
  }

  /* close the states database */
  if (PrefsPreferences.game.volIndex == vfsRamVolIndex) {
    RamStatesDbClose(DbP);
  } else {
    if (!VfsStatesDbClose(DbP, &PrefsPreferences.game,
			  StatesPrefsAndStatesDirty)) {
      ok = false;
    }
  }
  DbP = NULL;
  return ok;
}

/**
 * Closes the states database, showing errors that occurred.
 */
void StatesClosePrefsAndStatesWithShow(
  void (*ErrorFunction)(MiscErrorType mainIndex,
			MiscErrorType detailIndex,
			const Char *parameter), Boolean keepLocation) {
  if (!StatesClosePrefsAndStates()) {
    if (PrefsPreferences.game.volIndex == vfsRamVolIndex) {
      (*ErrorFunction)(miscErrOs, miscErrStateSave, NULL);
    } else if (keepLocation) {
      (*ErrorFunction)(miscErrVfs, miscErrStateSave, NULL);
    } else {
      ManagerListSetLocation(vfsRamVolIndex);
      (*ErrorFunction)(miscErrVfs, miscErrSaveLocationChanged,
		       VfsVolume[vfsRamVolIndex]->name);
    }
  }
}

/**
 * Deletes the states database (when the game is deleted). Errors are
 * silently ignored.
 */
void StatesDeletePrefsAndStates(void) {
  /* if no states database is open, do nothing */
  if (DbP == NULL) {
    return;
  }

  /* remove database, errors are silently ignored */
  if (PrefsPreferences.game.volIndex != vfsRamVolIndex) {
    VfsDeleteStates(&PrefsPreferences.game);
  }
  /* the copy in Palm RAM has to be deleted in any case */
  RamDeleteStates(DbP);
  DbP = NULL;
}

/* === User interface functions =======================================	*/

/**
 * Sets up the list of saved states.
 *
 * @param frmP  pointer to the manager form.
 * @param lstP  pointer to the list of states.
 * @param index selected entry (statesLastState for last state).
 */
void StatesSetupList(FormType *frmP, ListType *lstP, Int16 index) {
  Int16 numberOfStates;

  /* if no states database is open, there are no states */
  if (DbP == NULL) {
    numberOfStates = 0;

  } else {
    numberOfStates = (Int16)DmNumRecords(DbP) - dbStatesRecIndex;
    if (numberOfStates < 0) {
      numberOfStates = 0;
    }
  }

  LstSetDrawFunction(lstP, DrawStateName);
  LstSetListChoices(lstP, NULL, numberOfStates);

  if (numberOfStates > 0) {
    if (index == statesLastState || index >= numberOfStates) {
      index = numberOfStates - 1;
    }
    LstSetSelection(lstP, index);
    LstMakeItemVisible(lstP, index);
  } else {
    LstSetSelection(lstP, noListSelection);
  }

  if (FrmVisible(frmP)) {
    LstDrawList(lstP);
  }
}

/**
 * Callback function to draw the state's time in the list.
 *
 * @param itemNum   number of the item to draw.
 * @param bounds    bounds of the list.
 * @param itemsText pointer supplied to LstSetListChoices.
 */
static void DrawStateName(Int16 itemNum, RectangleType *bounds,
			  Char **itemsText) {
  void *indexedStateP;
  Char line[8];

  (void)itemsText;

  indexedStateP = GetStatePointer(itemNum);

  if (indexedStateP == NULL) {
    if (itemNum == 0) {
      StrCopy(line, "[--:--]");
    } else {
      StrCopy(line, "--:--");
    }

  } else {
    SavedStateType *indexedSavedStateP;
    UInt32         time;
    UInt16         h, m;

    indexedSavedStateP = indexedStateP + PrefsPreferences.game.ramSize;
    time = indexedSavedStateP->emulation.misc.time;
    MemPtrUnlock(indexedStateP);

    m = (UInt16)(time / 60);
    h = m / 60;
    m %= 60;

    if (itemNum == 0) {
      StrPrintF(line, "[%u:%02u]", h, m);
    } else {
      StrPrintF(line, "%u:%02u", h, m);
    }
  }

  WinDrawChars(line, StrLen(line), bounds->topLeft.x, bounds->topLeft.y);
}

/**
 * Draws the "GO!" button. When a saved screen is available, it is used.
 *
 * @param frmP     pointer to the manager form.
 * @param lstP     pointer to the list of states.
 * @param buttonId ID of the button.
 * @param labelId  ID of the default label in the button.
 */
void StatesDrawGoButton(FormType *frmP, ListType *lstP, UInt16 buttonId,
			UInt16 labelId) {
  UInt16        labelIndex;
  UInt16        buttonIndex;
  RectangleType r;
  void           *indexedStateP;

  labelIndex = FrmGetObjectIndex(frmP, labelId);
  buttonIndex = FrmGetObjectIndex(frmP, buttonId);
  FrmGetObjectBounds(frmP, buttonIndex, &r);
  /* adjust the size */
  r.topLeft.x += 1;
  r.topLeft.y += 1;
  r.extent.x -= 2;
  r.extent.y -= 2;

  indexedStateP = GetStatePointer(LstGetSelection(lstP));

  if (indexedStateP == NULL) {
    /* no saved state, simply show the default label */
    WinEraseRectangle(&r, 0);
    FrmShowObject(frmP, labelIndex);

  } else {
    typedef struct {
      /* we need ALLOW_ACCESS_TO_INTERNALS_OF_BITMAPS defined */
      BitmapTypeV1 bmpLowResolution;
      UInt8        bitsLowResolution[videoBytesPerBuffer / 2 / 2];
      BitmapTypeV1 bmpSeparator;
      BitmapTypeV3 bmpHighResolution;
      UInt8        bitsHighResolution[videoBytesPerBuffer];
    } ThumbnailType;

    ThumbnailType  *thumbnailP;

    thumbnailP = MemPtrNew(sizeof(ThumbnailType));

    if (thumbnailP == NULL) {
      /* no memory, simply show nothing */
      FrmHideObject(frmP, labelIndex);
      WinEraseRectangle(&r, 0);

    } else {
      SavedStateType *indexedSavedStateP;
      UInt8          *destP, *src1P, *src2P, w;
      UInt16         x, y;

      thumbnailP->bmpLowResolution = (BitmapTypeV1){
	videoScreenWidth / 2, videoScreenHeight / 2,
	videoScreenWidth / 2 / 8 * videoBitsPerPixel,
	{ false, }, videoBitsPerPixel, BitmapVersionOne,
	/* fortunately these are multiples of 4: */
	(sizeof(BitmapTypeV1) + videoBytesPerBuffer / 2 / 2) / 4 };
      thumbnailP->bmpSeparator = (BitmapTypeV1){
	0, 0, 0, { false, }, 0xFF, BitmapVersionOne, 0 };
      thumbnailP->bmpHighResolution = (BitmapTypeV3){
	videoScreenWidth, videoScreenHeight,
	videoScreenWidth / 8 * videoBitsPerPixel,
	{ false, }, videoBitsPerPixel, BitmapVersionThree,
	sizeof(BitmapTypeV3), pixelFormatIndexed, 0, 0, kDensityDouble, 0,
	0 };

      indexedSavedStateP = indexedStateP + PrefsPreferences.game.ramSize;

      destP = thumbnailP->bitsLowResolution;
      src1P = indexedSavedStateP->video.videoBuffer;

      for (y = 0; y < videoScreenHeight / 2; y++) {
	src2P = src1P + videoScreenWidth / (8 / videoBitsPerPixel);
	for (x = 0;
	     x < videoScreenWidth / 2 / (8 / videoBitsPerPixel);
	     x++) {
	  w  = ((*src1P & 0xC0) + (*src2P & 0xC0)) >> 2;
	  w +=  (*src1P & 0x30) + (*src2P & 0x30);
	  w += 0x20;
	  *destP = w & 0xC0;
	  w  =  (*src1P & 0x0C) + (*src2P & 0x0C);
	  w += ((*src1P & 0x03) + (*src2P & 0x03)) << 2;
	  w += 0x08;
	  *destP |= w & 0x30;
	  src1P++;
	  src2P++;
	  w  = ((*src1P & 0xC0) + (*src2P & 0xC0)) >> 2;
	  w +=  (*src1P & 0x30) + (*src2P & 0x30);
	  w += 0x20;
	  *destP |= (w >> 4) & 0x0C;
	  w  =  (*src1P & 0x0C) + (*src2P & 0x0C);
	  w += ((*src1P & 0x03) + (*src2P & 0x03)) << 2;
	  w += 0x08;
	  *destP |= (w >> 4) & 0x03;
	  src1P++;
	  src2P++;
	  destP++;
	}
	src1P = src2P;
      }

      MemMove(thumbnailP->bitsHighResolution,
	      indexedSavedStateP->video.videoBuffer, videoBytesPerBuffer);

      FrmHideObject(frmP, labelIndex);
      WinDrawBitmap((BitmapType *)thumbnailP, r.topLeft.x,
		    r.topLeft.y);

      MemPtrFree(thumbnailP);
    }

    MemPtrUnlock(indexedStateP);
  }
}

/* === Functions for single states ====================================	*/

/**
 * Checks for autostart, and if possible sets up emulation.
 *
 * @return true if emulation should be started.
 */
Boolean StatesAutostart(void) {
  /* the current game didn't run on quit */
  if (!PrefsPreferences.running) {
    return false;
  }

  /* set up the emulation */
  if (PrefsPreferences.game.mbc > memoryMbcUnknown) {
    if (StatesOpenPrefsAndStates()) {
      if (EmulationSetupAll()) {
	StatesLoadCurrentState();
	return true;
      }
      /* no need to close the states database, it's done when the game is
       * selected in the manager
       */
    }
  }

  MiscShowError(miscErrResumeGame, miscErrCantRun, NULL);
  return false;
}

/**
 * Gets or creates, and locks the game's RAM and state.
 *
 * @return true if successful.
 */
Boolean StatesOpenCurrentState(void) {
  UInt32    size;
  Boolean   reset;
  MemHandle recH;

  /* if no states database is open, abort */
  if (DbP == NULL) {
    return false;
  }

  size = PrefsPreferences.game.ramSize + sizeof(SavedStateType);
  reset = true;

  if (DmQueryRecord(DbP, dbStatesRecIndex) == NULL) {
    /* a new record for RAM and state is needed */
    UInt16 recIndex;

    recIndex = dbStatesRecIndex;
    recH = DmNewRecord(DbP, &recIndex, size);

  } else {
    /* get RAM and state, on error try to correct */
    recH = DmGetRecord(DbP, dbStatesRecIndex);
    if (recH != NULL) {
      if (MemHandleSize(recH) == size) {
	reset = false;
      } else {
	recH = DmResizeRecord(DbP, dbStatesRecIndex, size);
	if (recH == NULL) {
	  DmReleaseRecord(DbP, dbStatesRecIndex, false);
	}
      }
    }
  }
  if (recH == NULL) {
    return false;
  }

  /* looks fine, now lock and set pointers */
  StateP = MemHandleLock(recH);
  StatesCartridgeRamPtr = StateP + 0;
  SavedStateP = StateP + PrefsPreferences.game.ramSize;
  if (reset) {
    StatesResetCurrentState();
  }
  return true;
}

/**
 * Unlocks and releases the game's RAM and state.
 *
 * @return true if successful.
 */
Boolean StatesCloseCurrentState(void) {
  if (StateP != NULL) {
    UInt16 version;

    version = SavedStateP->version;

    MemPtrUnlock(StateP);
    DmReleaseRecord(DbP, dbStatesRecIndex, true);
    StateP = NULL;

    if (version != savedStateVersion) {
      /* no automatic saving of the current state */
      UInt32 size;

      if (PrefsPreferences.game.ramSize != 0) {
	/* remove the state, keep the RAM */
	size = PrefsPreferences.game.ramSize;
      } else {
	size = minDummySize;
      }
      if (DmResizeRecord(DbP, dbStatesRecIndex, size) == NULL) {
	return false;
      }
    }
  }

  return true;
}

/**
 * Sets up emulation to the "reset" state.
 */
void StatesResetCurrentState(void) {
  /* Fill with 0x00 bytes:
   * version: state will be rejected until saved correctly
   * memory:  clean memory, for some freeware games
   * video:   empty screen
   */
  DmSet(StateP, PrefsPreferences.game.ramSize + 0, sizeof(SavedStateType),
	0x00);
  EmulationResetState(StateP, PrefsPreferences.game.ramSize +
		      OffsetOf(SavedStateType, emulation));
  MemoryResetState(StateP, PrefsPreferences.game.ramSize +
		   OffsetOf(SavedStateType, memory));
}

/**
 * Sets up emulation to the currently loaded state.
 */
void StatesLoadCurrentState(void) {
  UInt16 version;

  EmulationLoadState(&(SavedStateP->emulation));
  MemoryLoadState(&(SavedStateP->memory));
  VideoLoadState(&(SavedStateP->video));

  /* make the current state invalid */
  version = 0;
  DmWrite(StateP, PrefsPreferences.game.ramSize +
	  OffsetOf(SavedStateType, version), &version, sizeof(version));
}

/**
 * Saves the current state.
 */
void StatesSaveCurrentState(void) {
  UInt16 version;

  /* save the several parts */
  EmulationSaveState(StateP, PrefsPreferences.game.ramSize +
		     OffsetOf(SavedStateType, emulation));
  MemorySaveState(StateP, PrefsPreferences.game.ramSize +
		  OffsetOf(SavedStateType, memory));
  VideoSaveState(StateP, PrefsPreferences.game.ramSize +
		 OffsetOf(SavedStateType, video));

  /* make the current state valid */
  version = savedStateVersion;
  DmWrite(StateP, PrefsPreferences.game.ramSize +
	  OffsetOf(SavedStateType, version), &version, sizeof(version));
}

/**
 * Saves the current state in the crash database.
 */
void StatesSaveCurrentStateAsCrashed(void) {
  LocalID   dbId;
  DmOpenRef crashedDbP;

  StatesSaveCurrentState();

  /* create crash database, on error abort silently */
  dbId = DmFindDatabase(crashDbCardNo, crashDbName);
  if (dbId != 0) {
    DmDeleteDatabase(crashDbCardNo, dbId);
  }
  (void)DmCreateDatabase(crashDbCardNo, crashDbName, miscCreator,
			 crashDbType, false);
  crashedDbP = DmOpenDatabaseByTypeCreator(crashDbType, miscCreator,
					   dmModeReadWrite);
  if (crashedDbP != NULL) {
    UInt16    recIndex;
    MemHandle recH;

    /* save game information */
    recIndex = crashDbGameRecIndex;
    recH = DmNewRecord(crashedDbP, &recIndex, sizeof(PrefsPreferences));
    if (recH != NULL) {
      DmWrite(MemHandleLock(recH), 0, &PrefsPreferences,
	      sizeof(PrefsPreferences));
      MemHandleUnlock(recH);
      DmReleaseRecord(crashedDbP, recIndex, true);
    }

    /* save current state */
    recIndex = crashDbStateRecIndex;
    recH = DmNewRecord(crashedDbP, &recIndex, MemPtrSize(StateP));
    if (recH != NULL) {
      DmWrite(MemHandleLock(recH), 0, StateP, MemPtrSize(StateP));
      MemHandleUnlock(recH);
      DmReleaseRecord(crashedDbP, recIndex, true);
    }

    /* ensure a fresh next launch */
    StatesResetCurrentState();

    MiscSetBackupBit(crashedDbP);
    DmCloseDatabase(crashedDbP);
  }
}

/**
 * Creates a new saved state from the current state.
 *
 * @return true if successful.
 */
Boolean StatesCreateStateFromCurrentState(void) {
  UInt16    recIndex;
  MemHandle recH;
  void      *newStateP;
  UInt16    version;

  recIndex = dmMaxRecordIndex;
  recH = DmNewRecord(DbP, &recIndex, PrefsPreferences.game.ramSize +
		     sizeof(SavedStateType));
  if (recH == NULL) {
    return false;
  }
  newStateP = MemHandleLock(recH);

  /* save the several parts */
  EmulationSaveState(StateP, PrefsPreferences.game.ramSize +
		     OffsetOf(SavedStateType, emulation));
  MemorySaveState(StateP, PrefsPreferences.game.ramSize +
		  OffsetOf(SavedStateType, memory));
  VideoSaveState(StateP, PrefsPreferences.game.ramSize +
		 OffsetOf(SavedStateType, video));

  /* now copy the whole state */
  DmWrite(newStateP, 0, StateP, MemPtrSize(newStateP));

  /* make the state valid */
  version = savedStateVersion;
  DmWrite(newStateP, PrefsPreferences.game.ramSize +
	  OffsetOf(SavedStateType, version), &version, sizeof(version));

  MemHandleUnlock(recH);
  DmReleaseRecord(DbP, recIndex, true);

  StatesPrefsAndStatesDirty = true;
  return true;
}

/**
 * Copies the indexed state to the current state.
 *
 * @param index the index of the state.
 * @returns     true if successful.
 */
Boolean StatesCopyStateToCurrentState(Int16 index) {
  void *indexedStateP;

  /* the current state needs no copying */
  if (index <= 0) {
    return true;
  }

  indexedStateP = GetStatePointer(index);
  if (indexedStateP == NULL) {
    return false;
  }

  DmWrite(StateP, 0, indexedStateP, MemPtrSize(StateP));

  MemPtrUnlock(indexedStateP);
  return true;
}

/**
 * Deletes the indexed state. Errors are silently ignored.
 *
 * @param index the index of the state.
 */
void StatesDeleteState(Int16 index) {
  /* if no states database is open, do nothing */
  if (DbP == NULL || index < 0) {
    return;
  }

  if (index == 0) {
    /* the current state has to stay, but a dummy suffices */
    DmResizeRecord(DbP, dbStatesRecIndex, minDummySize);
  } else {
    DmRemoveRecord(DbP, dbStatesRecIndex + index);
  }

  StatesPrefsAndStatesDirty = true;
}

/* === Obtain values from saved states ================================	*/

/**
 * Returns the locked pointer of the indexed saved state.
 *
 * @param index the index of the state.
 * @return      the locked pointer, or NULL.
 */
static void *GetStatePointer(Int16 index) {
  MemHandle      recH;
  void           *indexedStateP;
  SavedStateType *indexedSavedStateP;

  /* if no states database is open, there is no state */
  if (DbP == NULL || index < 0) {
    return NULL;
  }

  recH = DmQueryRecord(DbP, dbStatesRecIndex + index);
  if (recH == NULL) {
    return NULL;
  }

  /* the state has to have the correct size */
  if (MemHandleSize(recH) != PrefsPreferences.game.ramSize +
      sizeof(SavedStateType)) {
    return NULL;
  }

  indexedStateP = MemHandleLock(recH);
  indexedSavedStateP = indexedStateP + PrefsPreferences.game.ramSize;

  /* the state has to have the correct version */
  if (indexedSavedStateP->version != savedStateVersion) {
    MemHandleUnlock(recH);
    return NULL;
  }

  return indexedStateP;
}

/* === The end ========================================================	*/
