/************************************************************************
 *  manager.c
 *                      Game manager
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
 *  $Log: manager.c,v $
 *  Revision 1.35  2007/02/11 17:14:43  bodowenzel
 *  Multiple saved state feature implemented
 *
 *  Revision 1.34  2005/09/13 18:25:32  bodowenzel
 *  Unnecessary CtlSetUsable(..., false) for labels removed.
 *
 *  Revision 1.33  2005/06/25 06:48:01  bodowenzel
 *  Set backup bit for Phoinix-Games
 *  Show GO! label if launched without games
 *
 *  Revision 1.32  2005/05/26 19:35:47  bodowenzel
 *  Moved game's info into game's entry, removing MemoryMbcInfo
 *
 *  Revision 1.31  2005/05/03 08:39:40  bodowenzel
 *  No command bar info icon because of the Lite edition
 *  One pixel gap around names of games looks nicer
 *  Correct drawing of GO-button with moved/erroneous game
 *
 *  Revision 1.30  2005/04/03 14:08:33  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.29  2005/03/07 17:25:19  bodowenzel
 *  Filter command keys (up/down didn't work)
 *
 *  Revision 1.28  2005/03/01 20:09:51  bodowenzel
 *  Quick navigation via entering charaters
 *
 *  Revision 1.27  2005/02/27 16:39:01  bodowenzel
 *  Enhanced user interface
 *  Added module Eggs for easter eggs
 *
 *  Revision 1.26  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.25  2005/01/30 19:36:00  bodowenzel
 *  Remember top game in games list
 *  Show marker for location in games list
 *  Removed support for VFS mount and unmount
 *
 *  Revision 1.24  2005/01/29 10:25:34  bodowenzel
 *  Added const qualifier to pointer parameters
 *
 *  Revision 1.23  2005/01/28 17:35:13  bodowenzel
 *  Manager form uses a scrollable table now for the list of games
 *
 *  Revision 1.22  2004/12/28 13:57:17  bodowenzel
 *  New configurable screen layout
 *  Menu command to press all keys
 *  Menu shortcuts allowed
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.21  2004/12/02 20:02:16  bodowenzel
 *  Utility function MiscGetObjectPointer()
 *
 *  Revision 1.20  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.19  2004/09/19 12:36:55  bodowenzel
 *  Enhanced error reporting with string parameter
 *  Prevent renaming across volumes
 *
 *  Revision 1.18  2004/06/20 14:20:45  bodowenzel
 *  Adjustments for volume names, correct RAM name
 *  VFS mount and unmount support
 *
 *  Revision 1.17  2004/06/11 16:14:19  bodowenzel
 *  Enhanced error reporting with MiscPostError()
 *
 *  Revision 1.16  2004/06/06 09:26:05  bodowenzel
 *  Prefer location in RAM when game in both RAM and VFS
 *
 *  Revision 1.15  2004/04/13 19:43:25  bodowenzel
 *  Refactoring, nothing else
 *
 *  Revision 1.14  2004/04/05 21:56:55  bodowenzel
 *  VFS review and its consequences
 *
 *  Revision 1.13  2004/03/02 19:24:16  bodowenzel
 *  Changed to new error function MiscShowError()
 *
 *  Revision 1.12  2004/01/11 19:10:56  bodowenzel
 *  Added module 'ram' for on-board memory
 *  Start of VFS review and correction
 *
 *  Revision 1.11  2003/04/27 09:36:18  bodowenzel
 *  Added Lite edition
 *
 *  Revision 1.10  2002/12/19 21:23:01  bodowenzel
 *  Rebuild to show only one copy of each game again
 *  Corrected display of location
 *
 *  Revision 1.9  2002/12/16 20:38:33  bodowenzel
 *  Show games with same names when on different media
 *  Renaming games on VFS
 *
 *  Revision 1.8  2002/11/02 15:54:05  bodowenzel
 *  Reset graffiti state
 *
 *  Revision 1.7  2002/10/23 16:34:35  bodowenzel
 *  Busy form centralized
 *
 *  Revision 1.6  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.5  2002/05/10 15:17:39  bodowenzel
 *  Till's VFS contribution
 *
 *  Revision 1.4  2002/02/18 17:16:19  bodowenzel
 *  Crash if autostarting at short memory
 *
 *  Revision 1.3  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.2  2001/12/28 18:26:29  bodowenzel
 *  Work-around for rename-bug in some OS versions
 *  Shown game now correct even after resorting the list, cleanup code
 *
 *  Revision 1.1.1.1  2001/12/16 13:39:39  bodowenzel
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
#include <PalmOSGlue.h>

#include "manager.h"

#include "Phoinix.h"
#include "misc.h"
#include "prefs.h"
#include "eggs.h"
#include "states.h"
#include "ram.h"
#include "vfs.h"
#include "emulation.h"
#include "memory.h"

/* === Constants ======================================================	*/

#define listDbCardNo (0)
#define listDbName   "Phoinix-Games"
#define listDbType   'List'

#define tableEntryColumn (0)
#define markerVfsChar    (chrCardIcon)

#define gameDetailsStringMaxLength (20)

/* === Global and static variables ====================================	*/

static DmOpenRef ListDbP = NULL;
static Int16     NumberOfGames = 0;

static Boolean MyCompareHit;

static Int16 TopGame = -1;
static Int16 GameNo;

/* === Function prototypes ============================================	*/

static void ManagerSetupGamesList(void)
  MANAGER_SECTION;
static Int16 MyCompare(void* rec1, void* rec2, Int16 other,
		       SortRecordInfoPtr rec1SortInfo,
		       SortRecordInfoPtr rec2SortInfo,
		       MemHandle appInfoH)
  MANAGER_SECTION;

static void DrawGameName(void *tableP, Int16 row, Int16 col,
			 RectanglePtr bounds)
  MANAGER_SECTION;
static void SelectGame(FormType *frmP, TableType *tblP, ListType *lstP,
		       Boolean drawGoButton)
  MANAGER_SECTION;
static void ScrollTable(FormType *frmP, TableType *tblP, Int16 top)
  MANAGER_SECTION;

static Boolean GameDetailsDialog(void)
  MANAGER_SECTION;
static Boolean GameDetailsFormHandleEvent(EventType *evtP)
  MANAGER_SECTION;
static MiscErrorType GameDetailsHandleName(const FormType *frmP)
  MANAGER_SECTION;
static MiscErrorType GameDetailsHandleLocation(const FormType *frmP)
  MANAGER_SECTION;
static Boolean DeleteGame(void)
  MANAGER_SECTION;

/* === Init and Exit ==================================================	*/

/**
 * Sets up the list of games as a database, creates it if necessary.
 */
static void ManagerSetupGamesList(void) {
  MiscErrorType err;
  UInt16        index;

  /* remove hidden game, if existing */
  EggsRemoveHiddenGame();

  /* if games' list is already setup, just return */
  if (ListDbP != NULL) {
    return;
  }

  /* set defaults */
  NumberOfGames = 0;
  GameNo = 0;

  /* open games' list, create if not existent */
  ListDbP = DmOpenDatabaseByTypeCreator(
    listDbType, miscCreator, dmModeReadWrite);
  if (ListDbP == NULL) {
    (void)DmCreateDatabase(
      listDbCardNo, listDbName, miscCreator, listDbType, false);
    ListDbP = DmOpenDatabaseByTypeCreator(
      listDbType, miscCreator, dmModeReadWrite);
  }
  /* if the games' list can't be opened, abort */
  if (ListDbP == NULL) {
    MiscShowError(miscErrBuildingList, miscErrMemoryFull, NULL);
    return;
  }

  /* show what's going on */
  MiscShowBusy(true);

  /* probably we are going to change something, set the backup bit */
  MiscSetBackupBit(ListDbP);

  /* mark all records as 'not checked' and remove wrongly sized ones */
  for (index = DmNumRecords(ListDbP); index > 0;) {
    UInt16    attr;
    MemHandle recH;

    --index;
    DmRecordInfo(ListDbP, index, &attr, NULL, NULL);
    attr &= ~dmRecAttrDirty;
    DmSetRecordInfo(ListDbP, index, &attr, NULL);

    recH = DmQueryRecord(ListDbP, index);
    if (recH != NULL) {
      if (MemHandleSize(recH) != sizeof(ManagerDbListEntryType)) {
	DmRemoveRecord(ListDbP, index);
      }
    }
  }

  /* scan the onboard RAM and the VFS volumes for games */
  err = RamScanForGames();
  if (err == miscErrNone) {
    err = VfsScanForGames();
  }

  /* remove entries of games not found */
  for (index = DmNumRecords(ListDbP); index > 0;) {
    UInt16 attr;

    --index;
    DmRecordInfo(ListDbP, index, &attr, NULL, NULL);
    if ((attr & dmRecAttrDirty) == 0) {
      DmRemoveRecord(ListDbP, index);
    }
  }
  NumberOfGames = DmNumRecords(ListDbP);
  /* Saved states of a game whose ROM database was not found are
   * intentionaly _NOT_ removed. If the user re-installs the game, s/he
   * will get the states back :-)
   * If this gets annoying, a tool to clean-up might become necessary...
   */

  /* try to find last selected game in list */
  MyCompareHit = false;
  index = DmFindSortPosition(ListDbP, &PrefsPreferences.game, NULL,
			     MyCompare, 0);
  if (MyCompareHit) {
    GameNo = index - 1;
  }

  /* now we're ready */
  MiscShowBusy(false);
  if (err != miscErrNone) {
    MiscShowError(miscErrBuildingList, err, NULL);
  }
}

/**
 * Adds a game to the list.
 *
 * @param name     pointer to name of the game.
 * @param volIndex volume index.
 * @return         true if successful.
 */
Boolean ManagerListAdd(const Char *name, UInt16 volIndex) {
  MemHandle              currentH;
  ManagerDbListEntryType *currentP;
  ManagerDbListEntryType emptyEntry;
  UInt16                 index;

  /* get some memory for the entry, on error abort */
  currentH = DmNewHandle(ListDbP, sizeof(ManagerDbListEntryType));
  if (currentH == NULL) {
    return false;
  }

  currentP = MemHandleLock(currentH);

  /* fill the entry */
  MemSet(&emptyEntry, sizeof(emptyEntry), 0);
  StrCopy(emptyEntry.name, name);
  emptyEntry.volIndex = volIndex;
  emptyEntry.mbc = memoryMbcUnknown;
  DmWrite(currentP, 0, &emptyEntry, sizeof(emptyEntry));

  /* look for this game in the list */
  MyCompareHit = false;
  index = DmFindSortPosition(ListDbP, currentP, NULL, MyCompare, 0);

  MemHandleUnlock(currentH);

  if (MyCompareHit) {
    /* game found */
    MemHandle recH;

    /* correct index */
    index--;

    /* release new memory, not needed */
    MemHandleFree(currentH);

    recH = DmGetRecord(ListDbP, index);
    if (recH == NULL) {
      return false;

    } else {
      UInt16                 attr;
      ManagerDbListEntryType *recP;

      /* ignore the new entry when the old is in Palm RAM */
      DmRecordInfo(ListDbP, index, &attr, NULL, NULL);
      recP = MemHandleLock(recH);
      if ((attr & dmRecAttrDirty) == 0 ||
	  recP->volIndex != vfsRamVolIndex) {
	DmWrite(recP, OffsetOf(ManagerDbListEntryType, volIndex),
		&volIndex, sizeof(volIndex));
      }
      MemHandleUnlock(recH);
      DmReleaseRecord(ListDbP, index, true);
    }

  } else {
    /* add the entry to games' list, on error abort */
    Err err;

    err = DmAttachRecord(ListDbP, &index, currentH, NULL);
    if (err != errNone) {
      MemHandleFree(currentH);
      return false;
    }
  }

  return true;
}

/**
 * Callback function to compare two entries of the games list.
 * Note: as a side effect the flag MyCompareHit is set if the entries are
 * equal.
 *
 * @param rec1         pointer to the 1st record.
 * @param rec2         pointer to the 2nd record.
 * @param other        additional information (unused).
 * @param rec1SortInfo pointer to the 1st SortRecordInfo (unused).
 * @param rec2SortInfo pointer to the 1st SortRecordInfo (unused).
 * @param appInfoH     handle to the database's application info block.
 * @return             <0, 0, >0 if rec1 <,==,> rec2.
 */
static Int16 MyCompare(void* rec1, void* rec2, Int16 other,
		       SortRecordInfoPtr rec1SortInfo,
		       SortRecordInfoPtr rec2SortInfo,
		       MemHandle appInfoH) {
  Int16 result;

  result = StrCompare(((ManagerDbListEntryType*)rec1)->name,
		      ((ManagerDbListEntryType*)rec2)->name);

  /* set flag if names are identical */
  if (result == 0) {
    MyCompareHit = true;
  }

  return result;
}

/**
 * Closes the games list database.
 */
void ManagerExit(void) {
  /* close games' list, if open */
  if (ListDbP != NULL) {
    DmCloseDatabase(ListDbP);
  }
}

/* === User interface =================================================	*/

/**
 * Handles events in the manager form.
 *
 * While in the manager form, the states database of the current game has
 * to be open to have access to all saved states.
 *
 * @param evtP pointer to the event structure.
 * @return     true when the event is handled.
 */
Boolean ManagerFormHandleEvent(EventType *evtP) {
  FormType  *frmP;
  TableType *tblP;
  ListType  *lstP;
  UInt16    row;
  WChar     chr;

  frmP = FrmGetActiveForm();
  tblP = MiscGetObjectPtr(frmP, tableIdManagerList);
  lstP = MiscGetObjectPtr(frmP, listIdManagerStates);

  switch (evtP->eType) {

  case frmOpenEvent:
    TblSetColumnUsable(tblP, tableEntryColumn, true);
    TblSetCustomDrawProcedure(tblP, tableEntryColumn, DrawGameName);
    for (row = 0; row < tableManagerHeight; row++) {
      TblSetItemStyle(tblP, row, tableEntryColumn, customTableItem);
    }
    ManagerSetupGamesList();
    if (TopGame < 0) {
      TopGame = 0;
    }
    SelectGame(frmP, tblP, lstP, false);
    /* to fall through is necessary e.g. if the manager form opens after
     * the emulation and an error is posted with MiscPostError(), so that
     * a draw window is established
     */
    /* falls through */

  case frmUpdateEvent:
    TblUnhighlightSelection(tblP);
    FrmDrawForm(frmP);
    if (NumberOfGames != 0 &&
	GameNo >= TopGame && GameNo < TopGame + tableManagerHeight) {
      TblSelectItem(tblP, GameNo - TopGame, tableEntryColumn);
    }
    StatesDrawGoButton(frmP, lstP, buttonIdManagerGo, labelIdManagerGo);
    return true;

  case menuCmdBarOpenEvent:
    MenuCmdBarAddButton(menuCmdBarOnRight, BarInfoBitmap,
			menuCmdBarResultMenuItem,
			menuIdManagerGameDetails, NULL);
    MenuCmdBarAddButton(menuCmdBarOnRight, BarDeleteBitmap,
			menuCmdBarResultMenuItem,
			menuIdManagerGameDelete, NULL);
    return false;

  case tblExitEvent:
    /* re-select former game selection */
    if (GameNo >= TopGame && GameNo < TopGame + tableManagerHeight) {
      TblSelectItem(tblP, GameNo - TopGame, tableEntryColumn);
    }
    return false;

  case tblSelectEvent:
    if (GameNo != TopGame + evtP->data.tblSelect.row) {
      GameNo = TopGame + evtP->data.tblSelect.row;
      SelectGame(frmP, tblP, lstP, true);
    }
    return false;

  case sclRepeatEvent:
    ScrollTable(frmP, tblP, evtP->data.sclRepeat.newValue);
    return false;

  case keyDownEvent:
    switch (evtP->data.keyDown.chr) {
    case pageUpChr:
      if (GameNo > 0) {
	GameNo--;
      } else {
	GameNo = NumberOfGames - 1;
      }
      SelectGame(frmP, tblP, lstP, true);
      break;
    case pageDownChr:
      if (GameNo < NumberOfGames - 1) {
	GameNo++;
      } else {
	GameNo = 0;
      }
      SelectGame(frmP, tblP, lstP, true);
      break;
    default:
      chr = evtP->data.keyDown.chr;
      if ((evtP->data.keyDown.modifiers & commandKeyMask) == 0 &&
	  TxtGlueCharIsPrint(chr)) {
	/* select game based on the entered character */
	ManagerDbListEntryType entry;

	MemSet(&entry, sizeof(ManagerDbListEntryType), '\0');
	TxtGlueSetNextChar(entry.name, 0, chr);
	MyCompareHit = false;
	GameNo = DmFindSortPosition(ListDbP, &entry, NULL, MyCompare, 0);
	if (MyCompareHit) {
	  GameNo--;
	}
	if (GameNo >= NumberOfGames) {
	  GameNo = NumberOfGames - 1;
	}
	SelectGame(frmP, tblP, lstP, true);

	/* check for easter eggs */
	EggsHandleKey(chr);
      }
      return false;
    }
    return true;

  case lstSelectEvent:
    StatesDrawGoButton(frmP, lstP, buttonIdManagerGo, labelIdManagerGo);
    return true;

  case ctlSelectEvent:
    switch (evtP->data.ctlSelect.controlID) {

    case buttonIdManagerDelete:
      StatesDeleteState(LstGetSelection(lstP));
      StatesSetupList(frmP, lstP, LstGetSelection(lstP));
      StatesDrawGoButton(frmP, lstP, buttonIdManagerGo, labelIdManagerGo);
      return true;

    case buttonIdManagerGo:
      if (NumberOfGames == 0) {
	FrmAlert(alertIdManagerNoGames);
	/* check for runnable game type */
      } else if (PrefsPreferences.game.mbc <= memoryMbcUnknown) {
	MiscShowError(miscErrEmulator, miscErrCantRun, NULL);
	/* try to get emulation memory */
      } else if (!EmulationSetupAll()) {
	MiscShowError(miscErrEmulator, miscErrMemoryFull, NULL);
      } else {
	/* on we go! */
	if (!StatesCopyStateToCurrentState(LstGetSelection(lstP))) {
	  MiscShowError(miscErrEmulator, miscErrStateLoad, NULL);
	}
	StatesLoadCurrentState();
	FrmGotoForm(formIdEmulation);
	/* frmCloseEvent will come, soon... */
      }
      return true;

    default:
      return false;
    }

  case menuEvent:
    if (NumberOfGames == 0) {
      switch (evtP->data.menu.itemID) {
      case menuIdManagerGameDelete:
      case menuIdManagerGameDetails:
      case menuIdManagerOptionsEmulation:
      case menuIdManagerOptionsButtonMapping:
      case menuIdManagerOptionsScreenLayout:
	FrmAlert(alertIdManagerNoGames);
	break;
      case menuIdManagerOptionsTips:
	FrmHelp(helpIdManagerNoGames);
	break;
      case menuIdManagerOptionsAbout:
	MiscShowAbout();
	break;
      default:
	return false;
      }
    } else {
      switch (evtP->data.menu.itemID) {
      case menuIdManagerGameDelete:
	if (DeleteGame()) {
	  SelectGame(frmP, tblP, lstP, true);
	}
	break;
      case menuIdManagerGameDetails:
	if (GameDetailsDialog()) {
	  SelectGame(frmP, tblP, lstP, true);
	}
	break;
      case menuIdManagerOptionsEmulation:
	PrefsEmulationOptionsDialog(StatesEmulationOptionsP);
	break;
      case menuIdManagerOptionsButtonMapping:
	PrefsButtonMappingDialog(StatesButtonMappingP);
	break;
      case menuIdManagerOptionsScreenLayout:
	PrefsScreenLayoutDialog(StatesScreenLayoutP);
	break;
      case menuIdManagerOptionsTips:
	FrmHelp(helpIdManager);
	break;
      case menuIdManagerOptionsAbout:
	MiscShowAbout();
	break;
      default:
	return false;
      }
    }
    return true;

  default:
    return false;
  }
}

/**
 * Callback function to draw the game's name in the table.
 *
 * @param tableP pointer to the table.
 * @param row    row number of the item to draw.
 * @param col    column number of the item to draw.
 * @param bounds bounds of the list.
 */
static void DrawGameName(void *tableP, Int16 row, Int16 col,
			 RectangleType *boundsP) {
  MemHandle recH;

  /* get local copy of the game's name */
  recH = DmQueryRecord(ListDbP, TopGame + row);
  if (recH != NULL) {
    ManagerDbListEntryType *gameP;
    Char                   name[dmDBNameLength];
    Int16                  markerWidth;
    Int16                  len;
    Int16                  width;
    Boolean                dummy;

    gameP = MemHandleLock(recH);
    StrCopy(name, gameP->name);

    /* draw VFS marker */
    markerWidth = FntCharWidth(markerVfsChar);
    if (gameP->volIndex != vfsRamVolIndex) {
      WinDrawChar(markerVfsChar,
		  boundsP->topLeft.x + boundsP->extent.x - markerWidth,
		  boundsP->topLeft.y);
    }

    /* draw it with limited length */
    len = StrLen(name);
    width = boundsP->extent.x - 1 - markerWidth;
    if (FntCharsWidth(name, len) > width) {
      width -= FntCharWidth(chrEllipsis);
      FntCharsInWidth(name, &width, &len, &dummy);
      ChrHorizEllipsis(name + len);
      len++;
    }
    WinDrawChars(name, len, boundsP->topLeft.x + 1, boundsP->topLeft.y);

    MemHandleUnlock(recH);
  }
}

/**
 * Selects a game, closing the states database of the current game and
 * opening that of the new game.
 *
 * @param frmP         pointer to the manager form.
 * @param tblP         pointer to the table of games.
 * @param lstP         pointer to the list of states.
 * @param drawGoButton true if the "GO!" button has to be drawn.
 */
static void SelectGame(FormType *frmP, TableType *tblP, ListType *lstP,
		       Boolean drawGoButton) {
  Int16 top;

  /* close state database of old game, if open */
  StatesClosePrefsAndStatesWithShow(MiscShowError, true);

  /* initialize new game, if there's a game at all */
  if (NumberOfGames == 0) {
    PrefsPreferences.game.name[0] = '\0';
  } else {
    MemHandle              recH;
    ManagerDbListEntryType *recP;

    /* save the properties of the current game in the preferences */
    recH = DmGetRecord(ListDbP, GameNo);
    recP = MemHandleLock(recH);
    PrefsPreferences.game = *recP;
    StrCopy(PrefsPreferences.volumeName,
	    VfsVolume[PrefsPreferences.game.volIndex]->name);

    /* try to open states database */
    if (!MemoryRetrieveMbcInfo()) {
      MiscShowError(miscErrInitialization, miscErrRead, NULL);
    } else if (!StatesOpenPrefsAndStates()) {
      MiscShowError(miscErrOs, miscErrMemoryFull, NULL);
    }

    /* update the properties of the current game */
    DmWrite(recP, 0, &PrefsPreferences.game,
	    sizeof(ManagerDbListEntryType));
    MemHandleUnlock(recH);
    DmReleaseRecord(ListDbP, GameNo, true);
  }

  /* scroll table to new game */
  if (TopGame > GameNo) {
    top = GameNo;
  } else if (TopGame < GameNo - (tableManagerHeight - 1)) {
    top = GameNo - (tableManagerHeight - 1);
  } else {
    top = TopGame;
  }
  ScrollTable(frmP, tblP, top);

  StatesSetupList(frmP, lstP, 0);
  if (drawGoButton) {
    StatesDrawGoButton(frmP, lstP, buttonIdManagerGo, labelIdManagerGo);
  }
}

/**
 * Scrolls the table of games.
 *
 * @param frmP pointer to the manager form.
 * @param tblP pointer to the table of games.
 * @param top  new index of topmost game.
 */
static void ScrollTable(FormType *frmP, TableType *tblP, Int16 top) {
  ScrollBarType *sclP;
  Int16         max, row;
  UInt16        labelIndex;

  /* while setting up the scrollbar, the top row is corrected */
  sclP = MiscGetObjectPtr(frmP, scrollbarIdManagerList);
  TopGame = top;
  max = NumberOfGames - tableManagerHeight;
  if (max < 0) {
    max = 0;
  }
  if (TopGame > max) {
    TopGame = max;
  }
  SclSetScrollBar(sclP, TopGame, 0, max, tableManagerHeight);

  /* make the right number of rows usable */
  for (row = 0; row < tableManagerHeight; row++) {
    TblSetRowUsable(tblP, row, row < NumberOfGames);
  }

  /* redraw table */
  TblUnhighlightSelection(tblP);
  TblMarkTableInvalid(tblP);
  TblRedrawTable(tblP);
  if (NumberOfGames != 0 &&
      GameNo >= TopGame && GameNo < TopGame + tableManagerHeight) {
    TblSelectItem(tblP, GameNo - TopGame, tableEntryColumn);
  }

  /* hide or show "No games" as appropriate */
  labelIndex = FrmGetObjectIndex(frmP, labelIdManagerNoGames);
  if (NumberOfGames == 0) {
    FrmShowObject(frmP, labelIndex);
  } else {
    FrmHideObject(frmP, labelIndex);
  }
}

/* === Change game properties / delete game ===========================	*/

/**
 * Shows the game details form.
 *
 * @return true, if the list of games has changed.
 */
static Boolean GameDetailsDialog(void) {
  FormType  *prevFrmP, *frmP;
  UInt16    fldIndex;
  FieldType *fldP;
  MemHandle txtH;
  ListType  *lstP;
  Char      line[gameDetailsStringMaxLength];
  UInt32    size;
  Boolean   listChanged;

  /* prepare new form */
  prevFrmP = FrmGetActiveForm();
  frmP = FrmInitForm(formIdGameDetails);
  FrmSetActiveForm(frmP);
  FrmSetEventHandler(frmP, GameDetailsFormHandleEvent);

  /* setup the field for the game's name */
  GrfSetState(false, false, false);
  fldIndex = FrmGetObjectIndex(frmP, fieldIdGameDetailsName);
  FrmSetFocus(frmP, fldIndex);
  fldP = FrmGetObjectPtr(frmP, fldIndex);
  txtH = FldGetTextHandle(fldP);
  FldSetTextHandle(fldP, NULL);
  if (txtH != NULL) {
    MemHandleFree(txtH);
  }
  FldInsert(fldP, PrefsPreferences.game.name,
	    StrLen(PrefsPreferences.game.name));

  /* setup the game's location */
  lstP = MiscGetObjectPtr(frmP, listIdGameDetailsLocation);
  LstSetListChoices(lstP, (Char**)VfsVolume, VfsNumberOfVolumes);
  LstSetHeight(lstP, VfsNumberOfVolumes);
  LstSetSelection(lstP, PrefsPreferences.game.volIndex);
  LstMakeItemVisible(lstP, PrefsPreferences.game.volIndex);
  CtlSetLabel(MiscGetObjectPtr(frmP, popupIdGameDetailsLocation),
	      LstGetSelectionText(lstP, PrefsPreferences.game.volIndex));

  /* print code */
  if (PrefsPreferences.game.mbc != memoryMbcError) {
    StrPrintF(line, "%lx", PrefsPreferences.game.crc);
  } else {
    line[0] = '\0';
  }
  FrmCopyLabel(frmP, labelIdGameDetailsCode, line);

  /* print type */
  SysStringByIndex(stblIdGameDetailsType, PrefsPreferences.game.mbc, line,
		   gameDetailsStringMaxLength);
  FrmCopyLabel(frmP, labelIdGameDetailsType, line);

  /* print size */
  if (PrefsPreferences.game.mbc == memoryMbcError) {
    size = 0;
  } else if (PrefsPreferences.game.volIndex == vfsRamVolIndex) {
    size = RamTotalSize(&PrefsPreferences.game);
  } else {
    size = VfsTotalSize(&PrefsPreferences.game);
  }
  if (size != 0) {
    StrPrintF(line, "%lu", size);
  } else {
    line[0] = '\0';
  }
  FrmCopyLabel(frmP, labelIdGameDetailsSize, line);

  /* show form, saving is done in the event handler */
  listChanged = FrmDoDialog(frmP) == buttonIdGameDetailsOk;

  /* cleanup and return */
  if (prevFrmP != NULL) {
    FrmSetActiveForm(prevFrmP);
  }
  FrmDeleteForm(frmP);

  return listChanged;
}

/**
 * Handles events in the game details form.
 *
 * @param evtP pointer to the event structure.
 * @return     true when the event is handled.
 */
static Boolean GameDetailsFormHandleEvent(EventType *evtP) {
  if (evtP->eType == ctlSelectEvent) {
    if (evtP->data.ctlSelect.controlID == buttonIdGameDetailsOk) {
      /* The OK button has to be trapped to keep the form open in case of
       * errors.
       */
      FormType      *frmP;
      MiscErrorType err;

      frmP = FrmGetActiveForm();
      err = GameDetailsHandleName(frmP);
      if (err != miscErrNone) {
	MiscShowError(miscErrRenaming, err, NULL);
	return true;
      } else {
	err = GameDetailsHandleLocation(frmP);
	if (err != miscErrNone) {
	  MiscShowError(miscErrMoving, err, NULL);
	  return true;
	}
      }
    }
  }
  return false;
}

/**
 * Handles name entry.
 *
 * @param frmP pointer to the game details form.
 * @return     error code from misc.h, miscErrNone if successful.
 */
static MiscErrorType GameDetailsHandleName(const FormType *frmP) {
  MiscErrorType          err = miscErrNone;
  MemHandle              txtH;
  Char                   newName[dmDBNameLength];
  UInt16                 recI;
  MemHandle              recH;
  ManagerDbListEntryType *recP;

  /* get local copy of new text, do nothing if empty */
  txtH = FldGetTextHandle(MiscGetObjectPtr(frmP, fieldIdGameDetailsName));
  if (txtH == NULL) {
    return err;
  }
  StrCopy(newName, MemHandleLock(txtH));
  MemHandleUnlock(txtH);

  /* rename only if name is not empty and actually changed */
  if (StrLen(newName) == 0 ||
      StrCompare(newName, PrefsPreferences.game.name) == 0) {
    return err;
  }

  /* rename only if no other game has the same name */
  for (recI = 0; recI < DmNumRecords(ListDbP); recI++) {
    if (recI != GameNo) {
      recH = DmQueryRecord(ListDbP, recI);
      recP = MemHandleLock(recH);
      if (StrCompare(newName, recP->name) == 0) {
	err = miscErrAlreadyExists;
      }
      MemHandleUnlock(recH);
      if (err != miscErrNone) {
	break;
      }
    }
  }

  /* rename the stored game */
  if (err == miscErrNone) {
    if (PrefsPreferences.game.volIndex == vfsRamVolIndex) {
      err = RamRenameGame(&PrefsPreferences.game, newName);
    } else {
      err = VfsRenameGame(&PrefsPreferences.game, newName);
    }
  }

  /* abort on errors */
  if (err != miscErrNone) {
    return err;
  }

  /* reflect change in list, save entry in preferences */
  recH = DmGetRecord(ListDbP, GameNo);
  recP = MemHandleLock(recH);
  DmWrite(recP, OffsetOf(ManagerDbListEntryType, name), newName,
	  dmDBNameLength);
  StrCopy(PrefsPreferences.game.name, newName);
  MemHandleUnlock(recH);
  DmReleaseRecord(ListDbP, GameNo, true);

  /* sort list, get new index */
  DmQuickSort(ListDbP, MyCompare, 0);
  GameNo = DmFindSortPosition(ListDbP, &PrefsPreferences.game, NULL,
			      MyCompare, 0) - 1;
  return err;
}

/**
 * Handles location entry.
 *
 * @param frmP pointer to the game details form.
 * @return     error code from misc.h, miscErrNone if successful.
 */
static MiscErrorType GameDetailsHandleLocation(const FormType *frmP) {
  MiscErrorType err = miscErrNone;
  ListType      *lstP;
  Int16         volIndex;

  /* get new location */
  lstP = MiscGetObjectPtr(frmP, listIdGameDetailsLocation);
  volIndex = LstGetSelection(lstP);

  /* move only if location is actually changed */
  if (volIndex == PrefsPreferences.game.volIndex) {
    return err;
  }

  /* close state database if open */
  if (!StatesClosePrefsAndStates()) {
    err = miscErrStateSave;
  }

  /* move the game and its states */
  if (err == miscErrNone) {
    err = VfsMove(&PrefsPreferences.game, volIndex);
  }
  return err;
}

/**
 * Stores the new location of the current game.
 *
 * @param volIndex volume index.
 */
void ManagerListSetLocation(UInt16 volIndex) {
  PrefsPreferences.game.volIndex = volIndex;
  StrCopy(PrefsPreferences.volumeName, VfsVolume[volIndex]->name);

  if (ListDbP != NULL) {
    MemHandle recH;

    recH = DmGetRecord(ListDbP, GameNo);
    DmWrite(MemHandleLock(recH),
	    OffsetOf(ManagerDbListEntryType, volIndex), &volIndex,
	    sizeof(volIndex));
    MemHandleUnlock(recH);
    DmReleaseRecord(ListDbP, GameNo, true);
  }
}

/**
 * Removes the current game.
 *
 * @return true, if the list of games has changed.
 */
static Boolean DeleteGame(void) {
  MiscErrorType err;

  /* ask for confirmation */
  if (FrmCustomAlert(alertIdManagerDeleteGame,
		     PrefsPreferences.game.name, NULL, NULL) != 0) {
    return false;
  }

  /* delete the game */
  if (PrefsPreferences.game.volIndex == vfsRamVolIndex) {
    err = RamDeleteGame(&PrefsPreferences.game);
  } else {
    err = VfsDeleteGame(&PrefsPreferences.game);
  }
  if (err != miscErrNone) {
    MiscShowError(miscErrDeleting, err, NULL);
    return false;
  }

  /* delete prefs and states, errors are silently ignored */
  StatesDeletePrefsAndStates();

  /* remove entry from list, errors are silently ignored */
  (void)DmRemoveRecord(ListDbP, GameNo);
  NumberOfGames--;
  if (GameNo >= NumberOfGames && NumberOfGames > 0) {
    GameNo--;
  }

  return true;
}

/* === The end ========================================================	*/
