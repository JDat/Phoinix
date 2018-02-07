/************************************************************************
 *  manager.c
 *                      Game manager
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

#include "manager.h"
#include "Phoinix.h"
#include "misc.h"
#include "prefs.h"
#include "states.h"
#include "vfs.h"
#include "emulation.h"
#include "memory.h"

/* === Constants ======================================================	*/

#define listDbCardNo (0)
#define listDbName   "Phoinix-Games"
#define listDbType   'List'

/* === Global and static variables ====================================	*/

static DmOpenRef ListDbP = NULL;

static Boolean MyCompareHit;

static Int16 GameNo;

/* === Function prototypes ============================================	*/

static Boolean SetupGamesList(void);

static void DrawGameName(Int16 itemNum, RectangleType *bounds,
			 Char **itemsText);
static void UpdateForm(FormType *frmP);
static void UpdateGameNoAndForm(FormType *frmP, Int16 offset);

static Boolean SaveGameName(FormType *frmP, Boolean alert);
static void DeleteGame(FormType *frmP);
static Int16 MyCompare(void* rec1, void* rec2, Int16 other,
		       SortRecordInfoPtr rec1SortInfo,
		       SortRecordInfoPtr rec2SortInfo,
		       MemHandle appInfoH);

/* === Init and Exit ==================================================	*/

/**
 * Sets up the list of games as a database, creates it if necessary.
 *
 * @return true if a list is built.
 */
static Boolean SetupGamesList(void) {
  UInt16                 index, numRecords;
  Err                    err;
  DmSearchStateType      stateInfo;
  UInt16                 cardNo;
  LocalID                dbId;
  MemHandle              currentH = NULL, recH;
  ManagerDbListEntryType *currentP = NULL, *recP; /* initialize for compiler */

  /* if games' list is already setup, just return */
  if (ListDbP != NULL)
    return DmNumRecords(ListDbP) != 0;

  /* set default index */
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
    FrmAlert(alertIdManagerListError);
    return false;
  }

  /* show what's going on */
  MiscShowBusy(true);

  /* mark all records as 'not checked' */
  numRecords = DmNumRecords(ListDbP);
  for (index = 0; index < numRecords; index++) {
    UInt16 attr;

    DmRecordInfo(ListDbP, index, &attr, NULL, NULL);
    attr &= ~dmRecAttrDirty;
    DmSetRecordInfo(ListDbP, index, &attr, NULL);
  }

  /* look for all games, currently the number is not limited! */
  err = DmGetNextDatabaseByTypeCreator(
    true, &stateInfo, gameDbType, miscCreator, false, &cardNo, &dbId);
  for (;;) {
    Char   dbName[dmDBNameLength];
    UInt16 attr, volIdx=0;

    /* if needed, get some memory for the entry, on error abort */
    if (currentH == NULL) {
      currentH = DmNewHandle(ListDbP, sizeof(ManagerDbListEntryType));
      if (currentH == NULL) {
	FrmAlert(alertIdManagerListIncomplete);
	MiscShowBusy(false);
	return DmNumRecords(ListDbP) != 0;
      }
      currentP = (ManagerDbListEntryType *)MemHandleLock(currentH);
    }

    /* quit if no (more) games found */
    if (err != errNone)
      break;

    /* fill the entry with card number and name */
    DmDatabaseInfo(cardNo, dbId, dbName, NULL, NULL, NULL, NULL, NULL,
		   NULL, NULL, NULL, NULL, NULL);
    DmWrite(currentP, OffsetOf(ManagerDbListEntryType, cardNo),
	    &cardNo, sizeof(cardNo));
    DmWrite(currentP, OffsetOf(ManagerDbListEntryType, volIdx),
	    &volIdx, sizeof(volIdx));
    DmSet(currentP, OffsetOf(ManagerDbListEntryType, name),
	  dmDBNameLength, '\0');
    DmStrCopy(currentP, OffsetOf(ManagerDbListEntryType, name), dbName);

    /* look for this game in the list */
    MyCompareHit = false;
    index = DmFindSortPosition(ListDbP, currentP, NULL, MyCompare, 0);
    if (MyCompareHit) {
      /* game found, adjust index */
      index--;

      /* game found, adjust index and update location/card info */
      recH = DmGetRecord(ListDbP, index);
      if(recH != NULL) {
        recP = MemHandleLock(recH);

        /* update both fields */
        DmWrite(recP, OffsetOf(ManagerDbListEntryType, cardNo),
	        &cardNo, sizeof(cardNo));
        DmWrite(recP, OffsetOf(ManagerDbListEntryType, volIdx),
	        &volIdx, sizeof(volIdx));

        MemHandleUnlock(recH);
        DmReleaseRecord(ListDbP, index, true);
      }
    } else {
      /* add entry to games' list, on error abort */
      MemHandleUnlock(currentH);
      err = DmAttachRecord(ListDbP, &index, currentH, NULL);
      if (err != errNone) {
	MemHandleFree(currentH);
	FrmAlert(alertIdManagerListIncomplete);
	MiscShowBusy(false);
	return DmNumRecords(ListDbP) != 0;
      }
      /* now the memory belongs to the games' list... */
      currentH = NULL;
    }

    /* this game is recorded */
    DmRecordInfo(ListDbP, index, &attr, NULL, NULL);
    attr |= dmRecAttrDirty;
    DmSetRecordInfo(ListDbP, index, &attr, NULL);

    /* look for another game */
    err = DmGetNextDatabaseByTypeCreator(
      false, &stateInfo, gameDbType, miscCreator, false, &cardNo, &dbId);
  }

  /* now scan the VFS volumes for games */
  VFSScanForGames(ListDbP);

  /* remove entries of games not found */
  for (index = DmNumRecords(ListDbP); index > 0;) {
    UInt16 attr;

    --index;
    DmRecordInfo(ListDbP, index, &attr, NULL, NULL);
    if ((attr & dmRecAttrDirty) == 0)
      DmRemoveRecord(ListDbP, index);
  }
  /* Saved states of a game whose ROM database was not found are
   * intentionaly _NOT_ removed. If the user re-installs the game, s/he
   * will get the states back :-)
   * If this gets annoying, a tool to clean-up might become necessary...
   */

  /* try to find last selected game in list */
  DmWrite(currentP, 0,
	  &PrefsPreferences.game, sizeof(ManagerDbListEntryType));
  MyCompareHit = false;
  index = DmFindSortPosition(ListDbP, currentP, NULL, MyCompare, 0);
  if (MyCompareHit)
    GameNo = index - 1;

  /* release used memory */
  MemHandleUnlock(currentH);
  MemHandleFree(currentH);

  /* now we're ready */
  MiscShowBusy(false);
  return DmNumRecords(ListDbP) != 0;
}

/**
 * Callback function to compare two entries of the games list.
 * Note: as a side effect the flag MyCompareHit is set if the entries are
 * equal.
 *
 * @param  rec1         pointer to the 1st record.
 * @param  rec2         pointer to the 2nd record.
 * @param  other        additional information (unused).
 * @param  rec1SortInfo pointer to the 1st SortRecordInfo (unused).
 * @param  rec2SortInfo pointer to the 1st SortRecordInfo (unused).
 * @param  appInfoH     handle to the database's application info block.
 * @return 0, if rec1 = rec2. < 0 if rec1 < rec2. > 0 if rec1 > rec2.
 */
static Int16 MyCompare(void* rec1, void* rec2, Int16 other,
		       SortRecordInfoPtr rec1SortInfo,
		       SortRecordInfoPtr rec2SortInfo,
		       MemHandle appInfoH) {
  Int16 result;

  result = StrCompare(((ManagerDbListEntryType*)rec1)->name,
		      ((ManagerDbListEntryType*)rec2)->name);

  /* set flag if names are identical */
  if (result == 0)
    MyCompareHit = true;

  return result;
}

/**
 * Closes the games list database.
 */
void ManagerExit(void) {
  /* close games' list, if open */
  if (ListDbP != NULL)
    DmCloseDatabase(ListDbP);
}

/* === User interface =================================================	*/

/**
 * Handles events in the manager form.
 *
 * @param  evtP pointer to the event structure.
 * @return true when the event is handled.
 */
Boolean ManagerFormHandleEvent(EventType *evtP) {
  FormType *frmP;
  ListType *lstP;

  frmP = FrmGetActiveForm();

  switch (evtP->eType) {

  case frmOpenEvent:
    /* reset graffiti state */
    GrfSetState(false, false, false);

    /* setup games' list, if not yet done; if no games, switch form */
    if (!SetupGamesList())
      FrmGotoForm(formIdNoGame);
    else {
      UInt16 numRecords;

      /* setup list */
      numRecords = DmNumRecords(ListDbP);
      lstP = FrmGetObjectPtr(frmP,
			     FrmGetObjectIndex(frmP, listIdManagerName));
      LstSetListChoices(lstP, NULL, numRecords);
      LstSetDrawFunction(lstP, DrawGameName);
      if (numRecords < popupManagerNameHeight)
	LstSetHeight(lstP, numRecords);
      else
	LstSetHeight(lstP, popupManagerNameHeight);

      /* No text is needed for the list label, because a field is drawn
       * over. But without CtlSetLabel(), the popup isn't functional...
       */
      CtlSetLabel(
	FrmGetObjectPtr(frmP,
			FrmGetObjectIndex(frmP, popupIdManagerName)), "");
      /* draw form */
      UpdateForm(frmP);
    }
    return true;

  case frmCloseEvent:
    /* before closing, check for changed name, and save current name */
    if (ListDbP != NULL)
      /* may be called after deleting the last game, so check this */
      if (DmNumRecords(ListDbP) != 0)
	/* errors could not be alerted, don't care for need of sorting */
        (void)SaveGameName(frmP, false);
    return false;

  case ctlSelectEvent:
    switch (evtP->data.ctlSelect.controlID) {

    case buttonIdManagerGo:
      /* check for changed name, and save current name */
      UpdateGameNoAndForm(frmP, 0);

      /* check for runnable game type */
      if (MemoryMbcInfo.mbc <= memoryMbcUnknown)
	FrmAlert(alertIdManagerCantRun);
      /* try to get emulation memory */
      else if (!StatesNewCurrentState())
	FrmAlert(alertIdEmulationSetupError);
      /* try to setup the emulation */
      else if (!EmulationSetupAll()) {
	StatesExitCurrentState(false, false);
	EmulationCleanupAll();
	FrmAlert(alertIdEmulationSetupError);
      } else {
	/* on we go! */
	StatesResetCurrentState();
	FrmGotoForm(formIdEmulation);
	/* frmCloseEvent will come, too... */
      }
      return true;

    default:
      return false;
    }

  case popSelectEvent:
    switch(evtP->data.popSelect.listID) {
    case listIdManagerName:

      /* if same game is selected, revert to old name */
      if (evtP->data.popSelect.selection != GameNo)
        UpdateGameNoAndForm(frmP, evtP->data.popSelect.selection - GameNo);
      else
        UpdateForm(frmP);
      return true;

    case listIdManagerLocation:
      VFSMove(&PrefsPreferences.game,
	      evtP->data.popSelect.selection);
      /* setup the location list */
      lstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP,
						     listIdManagerLocation));
      LstSetSelection(lstP, PrefsPreferences.game.volIdx);
      LstMakeItemVisible(lstP, PrefsPreferences.game.volIdx);
      CtlSetLabel(FrmGetObjectPtr(
	frmP, FrmGetObjectIndex(frmP, popupIdManagerLocation)),
		  LstGetSelectionText(lstP, PrefsPreferences.game.volIdx));
      return true;

    default:
      return false;
    }

  case keyDownEvent:
    switch (evtP->data.keyDown.chr) {

    case pageUpChr:
      UpdateGameNoAndForm(frmP, -1);
      break;

    case pageDownChr:
      UpdateGameNoAndForm(frmP, +1);
      break;

    default:
      return false;
    }
    return true;

  case menuEvent:
    switch (evtP->data.menu.itemID) {
    case menuIdManagerActionsDeleteGame:
      DeleteGame(frmP);
      break;
    case menuIdManagerOptionsButtonMapping:
      PrefsButtonMappingDialog(StatesButtonMappingP);
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
    return true;

  default:
    return false;
  }
}

/**
 * Callback function to draw the game's name in the popup list.
 *
 * @param  itemNum   number of the item to draw.
 * @param  bounds    bounds of the list.
 * @param  itemsText pointer to an array of text pointers (unused).
 */
static void DrawGameName(Int16 itemNum, RectangleType *bounds,
			 Char **itemsText) {
  MemHandle              recH;
  ManagerDbListEntryType *recP;

  (void)itemsText;

  /* get the name of the game and draw it */
  recH = DmQueryRecord(ListDbP, itemNum);
  recP = MemHandleLock(recH);
  WinDrawChars(recP->name, StrLen(recP->name),
	       bounds->topLeft.x, bounds->topLeft.y);
  MemHandleUnlock(recH);
}

/**
 * Updates the manager form with current data.
 *
 * @param  frmP pointer to the manager form.
 */
static void UpdateForm(FormType *frmP) {
  MemHandle recH;
  ListType  *lstP;
  FieldType *fldP;
  MemHandle fldH;
  Char      line[20], format[20];
  UInt16    ctlIndex;

  /* save game's name */
  recH = DmQueryRecord(ListDbP, GameNo);
  PrefsPreferences.game = *(ManagerDbListEntryType *)MemHandleLock(recH);
  MemHandleUnlock(recH);

  /* no focus is default */
  FrmSetFocus(frmP, noFocus);

  /* setup the list */
  lstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, listIdManagerName));
  LstSetSelection(lstP, GameNo);
  LstMakeItemVisible(lstP, GameNo);

  /* setup the field for the game's name */
  fldP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, fieldIdManagerName));
  fldH = FldGetTextHandle(fldP);
  FldSetTextHandle(fldP, NULL);
  if (fldH != NULL)
    MemHandleFree(fldH);
  FldInsert(fldP, PrefsPreferences.game.name,
	    StrLen(PrefsPreferences.game.name));

  if(VFSVolume != NULL) {
    /* setup the location list */
    lstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP,
						   listIdManagerLocation));
    LstSetListChoices(lstP, (char**)VFSVolume, VFSPresent);

    if (VFSPresent < popupManagerNameHeight)
      LstSetHeight(lstP, VFSPresent);
    else
      LstSetHeight(lstP, popupManagerNameHeight);

    LstSetSelection(lstP, PrefsPreferences.game.volIdx);
    LstMakeItemVisible(lstP, PrefsPreferences.game.volIdx);
    CtlSetLabel(FrmGetObjectPtr(
      frmP, FrmGetObjectIndex(frmP, popupIdManagerLocation)),
		LstGetSelectionText(lstP, PrefsPreferences.game.volIdx));

  } else {
    /* hide location stuff */
    FrmHideObject(frmP, FrmGetObjectIndex(frmP, labelIdManagerLocation));
    FrmHideObject(frmP, FrmGetObjectIndex(frmP, popupIdManagerLocation));
    FrmHideObject(frmP, FrmGetObjectIndex(frmP, listIdManagerLocation));
  }

  /* retrieve MBC information */
  MemoryRetrieveMbcInfo();

  /* show info */

  ctlIndex = FrmGetObjectIndex(frmP, labelIdManagerType);
  FrmHideObject(frmP, ctlIndex);
  if (MemoryMbcInfo.mbc == memoryMbcError) {
    SysCopyStringResource(format, stringIdManagerGameError);
    FrmCopyLabel(frmP, labelIdManagerType, format);
  } else if (MemoryMbcInfo.mbc == memoryMbcUnknown) {
    SysCopyStringResource(format, stringIdManagerGameUnknown);
    FrmCopyLabel(frmP, labelIdManagerType, format);
  } else
    FrmCopyLabel(frmP, labelIdManagerType, MemoryMbcInfo.name);
  FrmShowObject(frmP, ctlIndex);

  SysCopyStringResource(format, stringIdManagerGameKBytes);
  StrPrintF(line, format, MemoryMbcInfo.romSize);
  ctlIndex = FrmGetObjectIndex(frmP, labelIdManagerRom);
  FrmHideObject(frmP, ctlIndex);
  if (MemoryMbcInfo.mbc == memoryMbcError)
    FrmCopyLabel(frmP, labelIdManagerRom, "");
  else
    FrmCopyLabel(frmP, labelIdManagerRom, line);
  FrmShowObject(frmP, ctlIndex);

  SysCopyStringResource(format, stringIdManagerGameKBytes);
  StrPrintF(line, format, MemoryMbcInfo.ramSize / 1024);
  ctlIndex = FrmGetObjectIndex(frmP, labelIdManagerRam);
  FrmHideObject(frmP, ctlIndex);
  if (MemoryMbcInfo.mbc == memoryMbcError)
    FrmCopyLabel(frmP, labelIdManagerRam, "");
  else if (MemoryMbcInfo.mbc == memoryMbc2) {
    SysCopyStringResource(format, stringIdManagerGameMbc2);
    FrmCopyLabel(frmP, labelIdManagerRam, format);
  } else
    FrmCopyLabel(frmP, labelIdManagerRam, line);
  FrmShowObject(frmP, ctlIndex);

  /* initialize the game's state */
  if (!StatesOpenPrefsAndStates())
    FrmAlert(alertIdStatesLoadError);

  /* update the form */
  FrmDrawForm(frmP);
}

/**
 * Updates the selected game and the manager form.
 *
 * @param  frmP   pointer to the manager form
 * @param  offset offset (index) of the selected game.
 */
static void UpdateGameNoAndForm(FormType *frmP, Int16 offset) {
  /* check for legal offset */
  if (GameNo + offset < 0 || GameNo + offset >= DmNumRecords(ListDbP))
    return;

  /* save changed game name */
  if (SaveGameName(frmP, true)) {
    MemHandle              recH;
    ManagerDbListEntryType new_game;

    /* save new game's name, before list is sorted, get new index */
    recH = DmQueryRecord(ListDbP, GameNo + offset);
    new_game = *(ManagerDbListEntryType *)MemHandleLock(recH);
    MemHandleUnlock(recH);
    DmQuickSort(ListDbP, MyCompare, 0);
    GameNo = DmFindSortPosition(ListDbP, &new_game,
				NULL, MyCompare, 0) - 1;
  } else
    GameNo += offset;

  UpdateForm(frmP);
}

/* === Change game name / delete game =================================	*/

/**
 * Saves the name of the current game, if changed.
 *
 * @param  frmP  pointer to the manager form.
 * @param  alert set true when an error alert may be drawn.
 * @return true if the list of games may need sorting.
 */
static Boolean SaveGameName(FormType *frmP, Boolean alert) {
  MemHandle              fldH;
  Char                   name[dmDBNameLength];
  MemHandle              recH;
  ManagerDbListEntryType *recP;
  LocalID                dbId;
  Err                    err;

  /* get local copy of new text, do nothing if empty */
  fldH = FldGetTextHandle(
    FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, fieldIdManagerName)));
  if (fldH == NULL)
    return false;
  StrCopy(name, MemHandleLock(fldH));
  MemHandleUnlock(fldH);

  /* get the game's entry */
  recH = DmQueryRecord(ListDbP, GameNo);
  recP = MemHandleLock(recH);

  /* rename only if name is not empty and actually changed */
  if (StrLen(name) == 0 || StrCompare(name, recP->name) == 0) {
    MemHandleUnlock(recH);
    return false;
  }

  if (recP->volIdx == 0) {
    /* try to rename game in palm ram */
    if (DmFindDatabase(recP->cardNo, name) != 0)
      err = dmErrAlreadyExists;
    else {
      dbId = DmFindDatabase(recP->cardNo, recP->name);
      if (dbId == 0)
	err = DmGetLastErr();
      else {
	UInt16 attr;

	err = DmDatabaseInfo(recP->cardNo, dbId, NULL, &attr, NULL, NULL,
			     NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if (err == errNone) {
	  if ((attr & dmHdrAttrReadOnly) != 0)
	    err = dmErrReadOnly;
	  else
	    err = DmSetDatabaseInfo(recP->cardNo, dbId, name, NULL, NULL,
				    NULL, NULL, NULL, NULL, NULL, NULL,
				    NULL, NULL);
	}
      }
    }

  } else {
    /* rename game on external card */
    if (!VFSRenameGame(recP, name)) {
      return false;
    }
    err = errNone;
  }

  MemHandleUnlock(recH);
  /* alert errors only if allowed to */
  if (err != errNone) {
    if (alert)
      FrmAlert(alertIdManagerRenameError);
    return false;
  }

  /* reflect change in list, save entry in prefs, leave list unsorted */
  recH = DmGetRecord(ListDbP, GameNo);
  recP = MemHandleLock(recH);
  DmWrite(recP, OffsetOf(ManagerDbListEntryType, name),
	  name, dmDBNameLength);
  PrefsPreferences.game = *recP;
  MemHandleUnlock(recH);
  DmReleaseRecord(ListDbP, GameNo, true);

  /* list needs sorting */
  return true;
}

/**
 * Stores the new location of the current game.
 *
 * @param loc  new volume index.
 * @param card new card number.
 */
void ManagerListChangeLocation(UInt16 loc, UInt16 card) {
  MemHandle              recH;
  ManagerDbListEntryType *recP;

  recH = DmGetRecord(ListDbP, GameNo);
  recP = MemHandleLock(recH);
  DmWrite(recP, OffsetOf(ManagerDbListEntryType, volIdx), &loc,  sizeof(loc));
  DmWrite(recP, OffsetOf(ManagerDbListEntryType, cardNo), &card, sizeof(card));
  PrefsPreferences.game = *recP;
  MemHandleUnlock(recH);
  DmReleaseRecord(ListDbP, GameNo, true);
}

/**
 * Adds a game to the list.
 *
 * @param  ListDbP pointer to the list database.
 * @param  name    pointer to name of the game.
 * @param  cardNo  card number.
 * @param  volIdx  volume index.
 * @return true if successful.
 */
Boolean ManagerListAdd(DmOpenRef ListDbP, Char *name,
		       UInt16 cardNo, UInt16 volIdx) {
  MemHandle currentH, recH;
  ManagerDbListEntryType *currentP, *recP;
  UInt16 attr, index;
  Err err;

  /* get some memory for the entry, on error abort */
  if((currentH = DmNewHandle(ListDbP,
	      sizeof(ManagerDbListEntryType))) == NULL) {
      return false;
  }

  currentP = (ManagerDbListEntryType *)MemHandleLock(currentH);

  /* fill the entry with card number and name */
  DmWrite(currentP, OffsetOf(ManagerDbListEntryType, cardNo),
	  &cardNo, sizeof(cardNo));
  DmWrite(currentP, OffsetOf(ManagerDbListEntryType, volIdx),
	  &volIdx, sizeof(volIdx));
  DmSet(currentP, OffsetOf(ManagerDbListEntryType, name),
	dmDBNameLength, '\0');
  DmStrCopy(currentP, OffsetOf(ManagerDbListEntryType, name), name);

  /* look for this game in the list */
  MyCompareHit = false;
  index = DmFindSortPosition(ListDbP, currentP, NULL,
			     MyCompare, 0);
  if (MyCompareHit) {

    index--;

    /* game found, adjust index and update location/card info */
    recH = DmGetRecord(ListDbP, index);
    if(recH != NULL) {
      recP = MemHandleLock(recH);

      /* update both fields */
      DmWrite(recP, OffsetOf(ManagerDbListEntryType, cardNo),
	      &cardNo, sizeof(cardNo));
      DmWrite(recP, OffsetOf(ManagerDbListEntryType, volIdx),
	      &volIdx, sizeof(volIdx));

      MemHandleUnlock(recH);
      DmReleaseRecord(ListDbP, index, true);
    }
  } else {
    /* add entry to games' list, on error abort */
    MemHandleUnlock(currentH);
    err = DmAttachRecord(ListDbP, &index, currentH, NULL);
    if (err != errNone) {
      MemHandleFree(currentH);
      return false;
    }
    /* now the memory belongs to the games' list... */
    currentH = NULL;
  }

  /* this game is recorded */
  DmRecordInfo(ListDbP, index, &attr, NULL, NULL);
  attr |= dmRecAttrDirty;
  DmSetRecordInfo(ListDbP, index, &attr, NULL);

  return true;
}

/**
 * Removes the current game.
 *
 * @param frmP pointer to the manager form.
 */
static void DeleteGame(FormType *frmP) {
  MemHandle              recH;
  ManagerDbListEntryType *recP;
  LocalID                dbId;
  UInt16                 ret, numRecords;
  ListType               *lstP;

  /* check for changed name, and save current name */
  UpdateGameNoAndForm(frmP, 0);

  /* get the game's entry */
  recH = DmQueryRecord(ListDbP, GameNo);
  recP = MemHandleLock(recH);

  /* ask for confirmation */
  ret = FrmCustomAlert(alertIdManagerDeleteGame, recP->name, NULL, NULL);
  if (ret != 0) {
    MemHandleUnlock(recH);
    return;
  }

  if (recP->volIdx == 0) {
    /* remove game from palm ram */
    dbId = DmFindDatabase(recP->cardNo, recP->name);
    if (dbId != 0)
      DmDeleteDatabase(recP->cardNo, dbId);
    /* if game is still there, don't remove saved states and entry */
    if (DmFindDatabase(recP->cardNo, recP->name) != 0) {
      FrmAlert(alertIdManagerDeleteError);
      MemHandleUnlock(recH);
      return;
    }

  } else {
    if(!VFSDeleteGame(recP)) {
      MemHandleUnlock(recH);
      return;
    }
  }

  /* release the game's entry */
  MemHandleUnlock(recH);

  /* remove saved states database */
  StatesDeletePrefsAndStates();

  /* remove entry from list, and change form if list is empty */
  (void)DmRemoveRecord(ListDbP, GameNo);
  numRecords = DmNumRecords(ListDbP);
  if (numRecords == 0) {
    FrmGotoForm(formIdNoGame);
    return;
  }

  /* update form */
  lstP = FrmGetObjectPtr(frmP,
			 FrmGetObjectIndex(frmP, listIdManagerName));
  LstSetListChoices(lstP, NULL, numRecords);
  if (numRecords < popupManagerNameHeight)
    LstSetHeight(lstP, numRecords);
  else
    LstSetHeight(lstP, popupManagerNameHeight);
  if (GameNo >= numRecords)
    GameNo = numRecords - 1;
  UpdateForm(frmP);
}

/* === The end ========================================================	*/
