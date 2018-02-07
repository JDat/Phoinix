/************************************************************************
 *  ram.c
 *                      Palm RAM support
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
 *  $Log: ram.c,v $
 *  Revision 1.8  2005/05/26 19:35:47  bodowenzel
 *  Moved game's info into game's entry, removing MemoryMbcInfo
 *
 *  Revision 1.7  2005/01/28 17:35:13  bodowenzel
 *  Manager form uses a scrollable table now for the list of games
 *
 *  Revision 1.6  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.5  2004/06/20 14:21:41  bodowenzel
 *  Fixed: only one onboard card supported
 *
 *  Revision 1.4  2004/06/11 16:14:19  bodowenzel
 *  Enhanced error reporting with MiscPostError()
 *
 *  Revision 1.3  2004/04/13 19:46:07  bodowenzel
 *  Refactoring of deleting states
 *
 *  Revision 1.2  2004/04/05 21:56:55  bodowenzel
 *  VFS review and its consequences
 *
 *  Revision 1.1  2004/01/11 19:03:49  bodowenzel
 *  Added module 'ram' for on-board memory
 *
 ************************************************************************
 */

/* === Includes =======================================================	*/

#include <PalmOS.h>

#include "ram.h"

#include "misc.h"
#include "manager.h"
#include "states.h"
#include "vfs.h"

/* === Scanning Palm RAM ==============================================	*/

/**
 * Scans Palm RAM for game databases.
 *
 * @return error code from misc.h, miscErrNone if successful.
 */
MiscErrorType RamScanForGames(void) {
  Err               err;
  DmSearchStateType stateInfo;
  UInt16            cardNo;
  LocalID           dbId;

  /* scan all databases */
  for (err = DmGetNextDatabaseByTypeCreator(
	 true, &stateInfo, miscGameDbType, miscCreator, false, &cardNo,
	 &dbId);
       err == errNone;
       err = DmGetNextDatabaseByTypeCreator(
	 false, &stateInfo, miscGameDbType, miscCreator, false, &cardNo,
	 &dbId)) {
    Char dbName[dmDBNameLength];

    /* accept only games on the default card */
    if (cardNo == ramGameDbCardNo) {
      DmDatabaseInfo(cardNo, dbId, dbName, NULL, NULL, NULL, NULL, NULL,
		     NULL, NULL, NULL, NULL, NULL);
      if (!ManagerListAdd(dbName, vfsRamVolIndex)) {
	return miscErrMemoryFull;
      }
    }
  }

  return miscErrNone;
}

/* === Support functions for the manager form =========================	*/

/**
 * Fetches the first block of the game in order to obtain CRC and
 * cartridge infos.
 *
 * @param gameP pointer to the game's entry.
 * @return      handle of the block, NULL if error.
 */
MemHandle RamGetFirstBlock(const ManagerDbListEntryType *gameP) {
  LocalID   dbId;
  DmOpenRef dbP;
  MemHandle srcRecH, dstRecH;
  UInt32    size;

  dbId = DmFindDatabase(ramGameDbCardNo, gameP->name);
  if (dbId == 0) {
    return NULL;
  }
  dbP = DmOpenDatabase(ramGameDbCardNo, dbId, dmModeReadOnly);
  if (dbP == NULL) {
    return NULL;
  }
  srcRecH = DmQueryRecord(dbP, 0);
  if (srcRecH == NULL) {
    DmCloseDatabase(dbP);
    return NULL;
  }
  size = MemHandleSize(srcRecH);
  dstRecH = MemHandleNew(size);
  if (dstRecH == NULL) {
    DmCloseDatabase(dbP);
    return NULL;
  }
  MemMove(MemHandleLock(dstRecH), MemHandleLock(srcRecH), size);
  MemHandleUnlock(srcRecH);
  MemHandleUnlock(dstRecH);
  DmCloseDatabase(dbP);
  return dstRecH;
}

/**
 * Retrieves the total size of both the game and the states database.
 *
 * @param gameP pointer to the game's entry.
 * @return      size in bytes, 0 on error.
 */
UInt32 RamTotalSize(const ManagerDbListEntryType *gameP) {
  UInt32  sizeGame, sizeStates;
  LocalID dbId;
  Char    dbName[dmDBNameLength];

  /* the game has to exist */
  dbId = DmFindDatabase(ramGameDbCardNo, gameP->name);
  if (dbId == 0) {
    return 0;
  }
  if (DmDatabaseSize(ramGameDbCardNo, dbId, NULL, &sizeGame, NULL)
      != errNone) {
    return 0;
  }

  /* the states database doesn't have to exist */
  sizeStates = 0;
  StrPrintF(dbName, statesDbNameFormat, gameP->crc);
  dbId = DmFindDatabase(ramStatesDbCardNo, dbName);
  if (dbId != 0) {
    if (DmDatabaseSize(ramStatesDbCardNo, dbId, NULL, &sizeStates, NULL)
	!= errNone) {
      return 0;
    }
  }

  return sizeGame + sizeStates;
}

/**
 * Rename the game.
 *
 * @param gameP pointer to the game's entry.
 * @param name  pointer to the new name.
 * @return      error code from misc.h, miscErrNone if successful.
 */
MiscErrorType RamRenameGame(const ManagerDbListEntryType *gameP,
			    const Char *name) {
  LocalID dbId;
  UInt16  attr;
  Err     err;

  dbId = DmFindDatabase(ramGameDbCardNo, gameP->name);
  if (dbId == 0) {
    return miscErrFileNotFound;
  }
  DmDatabaseInfo(ramGameDbCardNo, dbId, NULL, &attr, NULL, NULL, NULL,
		 NULL, NULL, NULL, NULL, NULL, NULL);
  if ((attr & dmHdrAttrReadOnly) != 0) {
    return miscErrRamReadOnly;
  }
  err = DmSetDatabaseInfo(ramGameDbCardNo, dbId, name, NULL, NULL, NULL,
			  NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  if (err == dmErrInvalidDatabaseName) {
    return miscErrBadName;
  } else if (err == dmErrAlreadyExists) {
    return miscErrAlreadyExists;
  }

  return miscErrNone;
}

/**
 * Delete the game.
 *
 * @param gameP pointer to the game's entry.
 * @return      error code from misc.h, miscErrNone if successful.
 */
MiscErrorType RamDeleteGame(const ManagerDbListEntryType *gameP) {
  LocalID dbId;

  dbId = DmFindDatabase(ramGameDbCardNo, gameP->name);
  if (dbId != 0) {
    DmDeleteDatabase(ramGameDbCardNo, dbId);
  }

  /* check whether game is still there */
  if (DmFindDatabase(ramGameDbCardNo, gameP->name) != 0) {
    return miscErrRamReadOnly;
  }

  return miscErrNone;
}

/**
 * Delete the states database. Errors are silently ignored.
 *
 * @param dbP reference to open database.
 */
void RamDeleteStates(DmOpenRef dbP) {
  LocalID dbId;
  UInt16  cardNo;

  if (DmOpenDatabaseInfo(dbP, &dbId, NULL, NULL, &cardNo, NULL)
      == errNone) {
    DmCloseDatabase(dbP);
    DmDeleteDatabase(cardNo, dbId);
  }
}

/* === Open and close games ===========================================	*/

/**
 * Open the game.
 *
 * @param gameP pointer to the game's entry.
 * @return      reference to open database, NULL on error.
 */
DmOpenRef RamGameDbOpen(const ManagerDbListEntryType *gameP) {
  LocalID dbId;

  dbId = DmFindDatabase(ramGameDbCardNo, gameP->name);
  if (dbId == 0) {
    return NULL;
  }
  return DmOpenDatabase(ramGameDbCardNo, dbId, dmModeReadOnly);
}

/**
 * Close the game.
 *
 * @param dbP reference to open database.
 */
void RamGameDbClose(DmOpenRef dbP) {
  DmCloseDatabase(dbP);
}

/* === Open and close states ==========================================	*/

/**
 * Open the states database.
 *
 * @param gameP pointer to the game's entry.
 * @return      reference to open database, NULL on error.
 */
DmOpenRef RamStatesDbOpen(const ManagerDbListEntryType *gameP) {
  Char    dbName[dmDBNameLength];
  LocalID dbId;

  StrPrintF(dbName, statesDbNameFormat, gameP->crc);
  dbId = DmFindDatabase(ramStatesDbCardNo, dbName);
  if (dbId == 0) {
    return NULL;
  }
  return DmOpenDatabase(ramStatesDbCardNo, dbId, dmModeReadWrite);
}

/**
 * Close the states database.
 *
 * @param dbP reference to open database.
 */
void RamStatesDbClose(DmOpenRef dbP) {
  DmCloseDatabase(dbP);
}

/* === The end ========================================================	*/
