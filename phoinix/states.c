/************************************************************************
 *  states.c
 *                      Emulation states
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

#include <PalmOS.h>

#include "states.h"
#include "Phoinix.h"
#include "misc.h"
#include "prefs.h"
#include "manager.h"
#include "vfs.h"
#include "emulation.h"
#include "memory.h"
#include "video.h"

/* NOTES:
 * A game's preferences and states database consists of several records:
 *
 * Variant A, game has no cartridge RAM:
 * #0           the preferences
 *
 * Variant B, game has cartridge RAM:
 * #0           the preferences
 * #1           current cartridge RAM
 */

/* === Constants ======================================================	*/

#define gameStatesDbCardNo        (0)
#define gameStatesDbType          'Save'
#define gameStatesDbPrefsRecIndex (0)
#define gameStatesDbRamRecIndex   (1)

#define gamePrefsVersion  (1)

#define currentStateDbName   "Phoinix-State"
#define currentStateDbType   'Runs'

#define crashedStateDbName   "Phoinix-Crash"
#define crashedStateDbType   'Boom'

#define savedStateVersion  (1)

/* === Type definitions ===============================================	*/

typedef struct {
  UInt16                 version;
  PrefsButtonMappingType buttonMapping[prefsButtonCount];
} GamePreferencesType;

typedef struct {
  UInt16                 version;
  ManagerDbListEntryType game;
  UInt32                 crc;
} GameStateType;

typedef struct {
  GameStateType          gameState;
  EmulationStateType     emulation;
  MemoryStateType        memory;
  VideoStateType         video;
} SavedStateType;

/* === Global and static variables ====================================	*/

static DmOpenRef           GameStatesDbP = NULL;
static GamePreferencesType GamePreferences;

PrefsButtonMappingType *StatesButtonMappingP;

UInt8 *StatesCartridgeRamPtr;

static DmOpenRef      CurrentStateDbP;
static SavedStateType *CurrentStateP;

/* === Function prototypes ============================================	*/

static void SetBackupBit(DmOpenRef dbP);

/* === Create, delete, load and save preferences and states ===========	*/

/**
 * Retrieves preferences etc. of the current game.
 *
 * @return true if successful.
 */
Boolean StatesOpenPrefsAndStates(void) {
  Char      dbName[20+dmDBNameLength];
  LocalID   dbId;
  MemHandle recH;
  UInt16    cardNo;

  /* save and close current game's preferences */
  StatesClosePrefsAndStates();

  /* preferences not yet found */
  StatesButtonMappingP = PrefsPreferences.buttonMapping;

  /* if running from vfs, copy the state database from vfs */
  if(PrefsPreferences.game.volIdx == 0) {
    /* game running from palm memory */

    /* create database name */
    StrPrintF(dbName, gameStatesDbName, MemoryMbcInfo.crc);

    /* create database if necessary */
    dbId = DmFindDatabase(gameStatesDbCardNo, dbName);
    if (dbId == 0) {

      (void)DmCreateDatabase(gameStatesDbCardNo, dbName,
			     miscCreator, gameStatesDbType, false);
      dbId = DmFindDatabase(gameStatesDbCardNo, dbName);
    }

    cardNo = gameStatesDbCardNo;

  } else {
    /* load state database from VFS */
    if(!VFSStateLoad(&PrefsPreferences.game, &cardNo, &dbId)) {

      /* create database name */
      StrPrintF(dbName, gameStatesDbName, MemoryMbcInfo.crc);

      (void)DmCreateDatabase(gameStatesDbCardNo, dbName,
			     miscCreator, gameStatesDbType, false);
      dbId = DmFindDatabase(gameStatesDbCardNo, dbName);

      cardNo = gameStatesDbCardNo;
    }
  }

  if (dbId == 0)
    return false;

  /* open database, on error alert and abort, else set backup bit */
  GameStatesDbP = DmOpenDatabase(cardNo, dbId, dmModeReadWrite);
  if (GameStatesDbP == NULL) {
    if(PrefsPreferences.game.volIdx == 0)
	VFSStateUnload();
    return false;
  }
  SetBackupBit(GameStatesDbP);

  /* nothing loaded yet */
  GamePreferences.version = 0;

  /* try to load the preferences, on errors try to correct */
  recH = DmQueryRecord(GameStatesDbP, gameStatesDbPrefsRecIndex);
  if (recH == NULL) {
    UInt16 recIndex;

    recIndex = gameStatesDbPrefsRecIndex;
    recH = DmNewRecord(GameStatesDbP, &recIndex,
		       sizeof(GamePreferencesType));
    if (recH != NULL)
      DmReleaseRecord(GameStatesDbP, gameStatesDbPrefsRecIndex, false);
  } else {
    if (MemHandleSize(recH) != sizeof(GamePreferencesType)) {
      recH = DmResizeRecord(GameStatesDbP, gameStatesDbPrefsRecIndex,
			    sizeof(GamePreferencesType));
    } else {
      GamePreferences = *((GamePreferencesType *)MemHandleLock(recH));
      MemHandleUnlock(recH);
    }
  }
  if (recH == NULL) {
    DmCloseDatabase(GameStatesDbP);
    GameStatesDbP = NULL;

    if(PrefsPreferences.game.volIdx == 0)
      VFSStateUnload();

    return false;
  }

  /* check for correct version, correct silently */
  if (GamePreferences.version != gamePrefsVersion) {
    UInt16 i;

    GamePreferences.version = gamePrefsVersion;
    for (i = 0; i < prefsButtonCount; i++)
      GamePreferences.buttonMapping[i] = PrefsPreferences.buttonMapping[i];
  }
  StatesButtonMappingP = GamePreferences.buttonMapping;

  /* if cartridge has no RAM, it's OK now */
  if (MemoryMbcInfo.ramSize == 0)
    return true;

  /* check for cartridge RAM, on error try to correct */
  recH = DmQueryRecord(GameStatesDbP, gameStatesDbRamRecIndex);
  if (recH == NULL) {
    UInt16 recIndex;

    recIndex = gameStatesDbRamRecIndex;
    recH = DmNewRecord(GameStatesDbP, &recIndex, MemoryMbcInfo.ramSize);
    if (recH != NULL)
      DmReleaseRecord(GameStatesDbP, gameStatesDbRamRecIndex, false);
  } else {
    if (MemHandleSize(recH) != MemoryMbcInfo.ramSize)
      recH = DmResizeRecord(GameStatesDbP, gameStatesDbRamRecIndex,
			    MemoryMbcInfo.ramSize);
  }
  if (recH == NULL) {
    DmCloseDatabase(GameStatesDbP);
    GameStatesDbP = NULL;

    if(PrefsPreferences.game.volIdx == 0)
      VFSStateUnload();

    return false;
  }

  return true;
}

/**
 * Gets and locks the game's RAM if appropriate.
 *
 * @return true if successful.
 */
Boolean StatesSetupPrefsAndStates(void) {
  MemHandle recH;

  /* no cartridge RAM yet */
  StatesCartridgeRamPtr = NULL;

  /* if no game's preferences are open, abort */
  if (GameStatesDbP == NULL)
    return false;

  /* if cartridge has no RAM, it's OK now */
  if (MemoryMbcInfo.ramSize == 0)
    return true;

  /* get and lock the record, on error abort */
  recH = DmGetRecord(GameStatesDbP, gameStatesDbRamRecIndex);
  if (recH == NULL)
    return false;
  StatesCartridgeRamPtr = MemHandleLock(recH);

  return true;
}

/**
 * Unlocks and releases the game's RAM after emulation.
 */
void StatesCleanupPrefsAndStates(void) {
  /* unlock and release record of cartridge RAM, if any */
  if (StatesCartridgeRamPtr != NULL) {
    MemPtrUnlock(StatesCartridgeRamPtr);
    DmReleaseRecord(GameStatesDbP, gameStatesDbRamRecIndex, true);
  }
}

/**
 * Closes the preferences etc. of the current game.
 */
void StatesClosePrefsAndStates(void) {
  MemHandle recH;

  /* if no game's preferences are open, do nothing */
  if (GameStatesDbP == NULL)
    return;

  /* get the record and save the current preferences */
  recH = DmGetRecord(GameStatesDbP, gameStatesDbPrefsRecIndex);
  if (recH == NULL)
    FrmAlert(alertIdStatesSaveError);
  else {
    DmWrite(MemHandleLock(recH), gameStatesDbPrefsRecIndex,
	    &GamePreferences, sizeof(GamePreferencesType));
    MemHandleUnlock(recH);
    DmReleaseRecord(GameStatesDbP, gameStatesDbPrefsRecIndex, true);
  }

  /* close the game's preferences database */
  DmCloseDatabase(GameStatesDbP);
  GameStatesDbP = NULL;

  /* and export state to vfs if required */
  VFSStateUnload();
}

/**
 * Deletes the preferences etc. (when the game is deleted).
 */
void StatesDeletePrefsAndStates(void) {
  LocalID dbId;
  UInt16  cardNo;

  /* if no game's preferences are open, do nothing */
  if (GameStatesDbP == NULL)
    return;

  /* remove database, errors are silently ignored */
  if (DmOpenDatabaseInfo(GameStatesDbP, &dbId, NULL, NULL, &cardNo, NULL)
      == errNone) {
    DmCloseDatabase(GameStatesDbP);
    GameStatesDbP = NULL;
    DmDeleteDatabase(cardNo, dbId);
  }
}

/* === Create, load and save the current state ========================	*/

/**
 * Checks for autostart, and if possible sets up emulation.
 *
 * @return true if emulation should be started.
 */
Boolean StatesAutostartCurrentState(void) {
  /* no saved state */
  CurrentStateP = NULL;

  /* look whether there's a saved state */
  if (DmFindDatabase(currentStateDbCardNo, currentStateDbName) == 0)
    return false;
  CurrentStateDbP = DmOpenDatabaseByTypeCreator(
    currentStateDbType, miscCreator, dmModeReadWrite);
  if (CurrentStateDbP == NULL)
    return false;

  /* try to open saved state, on error no autostart */
  if (DmNumRecords(CurrentStateDbP) != 0) {
    MemHandle recH;

    recH = DmGetRecord(CurrentStateDbP, 0);
    if (recH != NULL) {
      if (MemHandleSize(recH) == sizeof(SavedStateType)) {
	CurrentStateP = MemHandleLock(recH);

	/* if checks are OK, try to start the saved game */
	if (CurrentStateP->gameState.version == savedStateVersion) {
	  MemoryRetrieveMbcInfo();
	  if (MemoryMbcInfo.mbc > memoryMbcUnknown) {
	    if (MemoryMbcInfo.crc == CurrentStateP->gameState.crc) {
	      if (!StatesOpenPrefsAndStates())
		FrmAlert(alertIdStatesLoadError);
	      else {
		if (EmulationSetupAll()) {
		  StatesLoadCurrentState();
		  return true;
		}
		EmulationCleanupAll();
		FrmAlert(alertIdEmulationSetupError);
	      }
	    }
	  }
	}

	/* some other error occured */
	MemHandleUnlock(recH);
	CurrentStateP = NULL;
      }
      DmReleaseRecord(CurrentStateDbP, 0, false);
    }
  }
  DmCloseDatabase(CurrentStateDbP);

  /* the database isn't deleted in case that it's for another version */
  return false;
}

/**
 * Creates the state database for the current game.
 *
 * @return true if successful.
 */
Boolean StatesNewCurrentState(void) {
  LocalID   dbId;
  UInt16    recIndex;
  MemHandle recH;

  /* delete existing database, create new database, on error abort */
  dbId = DmFindDatabase(currentStateDbCardNo, currentStateDbName);
  if (dbId != 0)
    DmDeleteDatabase(currentStateDbCardNo, dbId);
  (void)DmCreateDatabase(currentStateDbCardNo, currentStateDbName,
			 miscCreator, currentStateDbType, false);
  CurrentStateDbP = DmOpenDatabaseByTypeCreator(
    currentStateDbType, miscCreator, dmModeReadWrite);
  if (CurrentStateDbP == NULL)
    return false;

  /* set backup bit */
  SetBackupBit(CurrentStateDbP);

  /* allocate a record for the current state, on error abort */
  recIndex = 0;
  recH = DmNewRecord(CurrentStateDbP, &recIndex, sizeof(SavedStateType));
  if (recH == NULL) {
    DmCloseDatabase(CurrentStateDbP);
    dbId = DmFindDatabase(currentStateDbCardNo, currentStateDbName);
    if (dbId != 0)
      DmDeleteDatabase(currentStateDbCardNo, dbId);
    return false;
  }
  CurrentStateP = MemHandleLock(recH);

  return true;
}

/**
 * Closes the state database after emulation, save as crash if commanded.
 *
 * @param save  true if state should be saved (for autostart).
 * @param crash true if state should be saved as crash database.
 */
void StatesExitCurrentState(Boolean save, Boolean crash) {
  /* if no state database opened, do nothing */
  if (CurrentStateP == NULL)
    return;

  /* on request, rename to crash database and save it */
  if (crash) {
    LocalID dbId;
    UInt32  type;

    /* delete existing crash database */
    dbId = DmFindDatabase(currentStateDbCardNo, crashedStateDbName);
    if (dbId != 0)
      DmDeleteDatabase(currentStateDbCardNo, dbId);

    /* renaming current state database for crash */
    DmOpenDatabaseInfo(CurrentStateDbP, &dbId, NULL, NULL, NULL, NULL);
    type = crashedStateDbType;
    DmSetDatabaseInfo(currentStateDbCardNo, dbId, crashedStateDbName,
		      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		      &type, NULL);

    /* copy the current cartridge RAM, if any */
    if (StatesCartridgeRamPtr != NULL) {
      UInt16    recIndex;
      MemHandle recH;

      /* copy to new record, errors are silently ignored */
      recIndex = 1;
      recH = DmNewRecord(CurrentStateDbP, &recIndex,
			 MemoryMbcInfo.ramSize);
      if (recH != NULL) {
	DmWrite(MemHandleLock(recH), 0,
		StatesCartridgeRamPtr, MemoryMbcInfo.ramSize);
	MemHandleUnlock(recH);
	DmReleaseRecord(CurrentStateDbP, 1, true);
      }
    }

    /* don't delete the database */
    save = true;
  }

  /* save all emulation data, if the state has to be saved */
  if (save) {
    GameStateType gameState;

    /* save the several parts */
    EmulationSaveState(CurrentStateP, OffsetOf(SavedStateType, emulation));
    MemorySaveState(CurrentStateP, OffsetOf(SavedStateType, memory));
    VideoSaveState(CurrentStateP, OffsetOf(SavedStateType, video));

    /* now save the version and the rest */
    gameState.version = savedStateVersion;
    gameState.game = PrefsPreferences.game;
    gameState.crc = MemoryMbcInfo.crc;
    DmWrite(CurrentStateP, OffsetOf(SavedStateType, gameState),
	    &gameState, sizeof(GameStateType));
  }

  /* release the record and close the database */
  MemPtrUnlock(CurrentStateP);
  DmReleaseRecord(CurrentStateDbP, 0, true);
  DmCloseDatabase(CurrentStateDbP);
  CurrentStateP = NULL;

  /* if not saving, delete the database */
  if (!save) {
    LocalID dbId;

    dbId = DmFindDatabase(currentStateDbCardNo, currentStateDbName);
    if (dbId != 0)
      DmDeleteDatabase(currentStateDbCardNo, dbId);
  }
}

/**
 * Sets up emulation to the "reset" state.
 */
void StatesResetCurrentState(void) {
  /* Fill with 0x00 bytes:
   * gameState.version: state will be rejected until saved correctly
   * memory:            clean memory, for some freeware games
   * video:             empty screen
   */
  DmSet(CurrentStateP, 0, sizeof(SavedStateType), 0x00);

  EmulationResetState();
  MemoryResetState(&(CurrentStateP->memory));
  VideoResetState(&(CurrentStateP->video));
}

/**
 * Sets up emulation to the currently loaded state.
 */
void StatesLoadCurrentState(void) {
  EmulationLoadState(&(CurrentStateP->emulation));
  MemoryLoadState(&(CurrentStateP->memory));
  VideoLoadState(&(CurrentStateP->video));
}

/* === Support ========================================================	*/

/**
 * Sets the backup bit for the given database.
 *
 * @param dbP reference of the database.
 */
static void SetBackupBit(DmOpenRef dbP) {
  LocalID dbId;
  UInt16  cardNo;
  UInt16  attr;

  DmOpenDatabaseInfo(dbP, &dbId, NULL, NULL, &cardNo, NULL);
  DmDatabaseInfo(cardNo, dbId, NULL, &attr, NULL, NULL, NULL, NULL, NULL,
		 NULL, NULL, NULL, NULL);
  attr |= dmHdrAttrBackup;
  DmSetDatabaseInfo(cardNo, dbId, NULL, &attr, NULL, NULL, NULL, NULL,
		    NULL, NULL, NULL, NULL, NULL);
}

/* === The end ========================================================	*/
