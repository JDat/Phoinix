/************************************************************************
 *  misc.c
 *                      Miscellaneous functions
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
 *  $Log: misc.c,v $
 *  Revision 1.20  2005/10/11 14:48:05  bodowenzel
 *  Centralized device type into MiscDevice
 *
 *  Revision 1.19  2005/05/03 08:41:22  bodowenzel
 *  Insert forgotten DmReleaseResource() call
 *  Moved SetBackupBit here from states.c
 *
 *  Revision 1.18  2005/01/30 19:36:18  bodowenzel
 *  Support for LC_Lite
 *
 *  Revision 1.17  2005/01/29 10:25:37  bodowenzel
 *  Added const qualifier to pointer parameters
 *
 *  Revision 1.16  2005/01/28 17:35:13  bodowenzel
 *  Manager form uses a scrollable table now for the list of games
 *
 *  Revision 1.15  2004/12/28 13:58:21  bodowenzel
 *  Support for devices without direct screen access
 *
 *  Revision 1.14  2004/12/02 20:02:16  bodowenzel
 *  Utility function MiscGetObjectPointer()
 *
 *  Revision 1.13  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.12  2004/09/19 12:35:54  bodowenzel
 *  Enhanced error reporting with string parameter
 *
 *  Revision 1.11  2004/06/20 14:23:20  bodowenzel
 *  VFS mount and unmount support
 *
 *  Revision 1.10  2004/06/11 16:16:43  bodowenzel
 *  Review and cleanup of miscErr* constants and texts
 *  Work-around for 'Freeze' error in MiscShowBusy() removed
 *
 *  Revision 1.9  2004/03/02 19:24:16  bodowenzel
 *  Changed to new error function MiscShowError()
 *
 *  Revision 1.8  2004/01/11 19:08:24  bodowenzel
 *  Work-around for 'Freeze' error
 *
 *  Revision 1.7  2003/04/27 09:36:18  bodowenzel
 *  Added Lite edition
 *
 *  Revision 1.6  2003/04/19 13:16:45  bodowenzel
 *  New variable MiscIsOs5 indicates OS5
 *
 *  Revision 1.5  2002/11/02 15:53:41  bodowenzel
 *  Check for OS 5 added
 *
 *  Revision 1.4  2002/10/23 16:34:35  bodowenzel
 *  Busy form centralized
 *
 *  Revision 1.3  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:02  bodowenzel
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

#include "Phoinix.h"

#ifdef handera
	/* Handera H330 support */
	#include <HandEra/Vga.h>
#endif

#ifdef alphasmart
	/* AlphaSmart Dana support */
	#include <Screen.h>
#endif

#ifdef fossil
	/* Fossil WristPDA support */
	#include <WristPDA.h>
#endif

#include "misc.h"



/* === Global variables ===============================================	*/

MiscDeviceType MiscDevice;

/* last posted error, set to miscErrNone after showing */
static MiscErrorType ErrMainIndex;
static MiscErrorType ErrDetailIndex;
static const Char    *ErrParameter;

/* === Test for OS version ============================================	*/

/**
 * Checks for the required OS version.
 *
 * @param minV        value as calculated by sysMakeROMVersion().
 * @param maxV        value as calculated by sysMakeROMVersion().
 * @param launchFlags flags given by the launcher.
 * @return            error code, errNone if all is OK.
 */
Err MiscRomVersionCompatible(UInt32 minV, UInt32 maxV,
			     Int16 launchFlags) {
  UInt32 version;

  /* check for certain devices */
  MiscDevice = miscDeviceStandard;
  
#ifdef handera
  if (FtrGet(TRGSysFtrID, TRGVgaFtrNum, &version) == errNone) {
    MiscDevice = miscDeviceH330;
  }
#endif
#ifdef alphasmart
  if (FtrGet(AlphaSmartSysFtrID, ScrnFtrNum, &version)
	     == errNone) {
    MiscDevice = miscDeviceDana;
  }
#endif
#ifdef fossil
  if (FtrGet(WPdaCreator, WPdaFtrNumVersion, &version)
	     == errNone) {
    MiscDevice = miscDeviceWrist;
  }
#endif


  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
  if (version >= minV && version < maxV) {
    return errNone;
  }

  if ((launchFlags &
       (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
      (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) {
    MiscShowError(miscErrInitialization, miscErrSystemVersion, NULL);
    if (version < sysMakeROMVersion(2, 0, 0, sysROMStageDevelopment, 0)) {
      /* on OS version before 2 the launcher was just a pop-up! */
      AppLaunchWithCommand(sysFileCDefaultApp,
			   sysAppLaunchCmdNormalLaunch, NULL);
    }
  }

  return sysErrRomIncompatible;
}

/* === The "About..." form ============================================	*/

/**
 * Shows the about form. With the 'Lite' resource set, only the help
 * string is shown.
 */
void MiscShowAbout(void) {
  MemHandle rsrcH;

  rsrcH = DmGetResource('tFRM', formIdAbout);
  if (rsrcH != NULL) {
    FormType *prevFrmP, *frmP;

    DmReleaseResource(rsrcH);

    /* prepare new form */
    prevFrmP = FrmGetActiveForm();
    frmP = FrmInitForm(formIdAbout);
    FrmSetActiveForm(frmP);

    /* wait until the form returns */
    FrmDoDialog(frmP);

    /* cleanup and return */
    if (prevFrmP != NULL) {
      FrmSetActiveForm(prevFrmP);
    }
    FrmDeleteForm(frmP);

  } else {
    FrmHelp(helpIdAbout);
  }
}

/* === The "Busy..." form =============================================	*/

/**
 * Draws or removes the form.
 */
void MiscShowBusy(Boolean on) {
  static WinHandle           oldWinH;
  static FormActiveStateType theFormState;
  static FormType            *frmP;

  if (on) {
    FrmSaveActiveState(&theFormState);
    oldWinH = WinGetDrawWindow();
    frmP = FrmInitForm(formIdBusy);
    FrmSetActiveForm(frmP);
    FrmDrawForm(frmP);

  } else {
    FrmEraseForm(frmP);
    FrmDeleteForm(frmP);
    WinSetDrawWindow(oldWinH);
    FrmRestoreActiveState(&theFormState);
  }
}

/* === Showing errors =================================================	*/

/* To get errors displayed "between" switching forms (that is, one form
 * was just closed, but a new form is not yet open), errors are posted in
 * the event queue.
 * To show the last error when stopping the application, the variable
 * ErrMainIndex is read. If it doesn't contain miscErrNone, the error is
 * shown.
 */

/**
 * Posts an event to show the error form.
 *
 * @param errorIndex  index to main string.
 * @param detailIndex index to detail string.
 * @param parameter   pointer to string, must be static.
 */
void MiscPostError(MiscErrorType mainIndex, MiscErrorType detailIndex,
		   const Char *parameter) {
  EventType evt;

  ErrMainIndex = mainIndex;
  ErrDetailIndex = detailIndex;
  ErrParameter = parameter;

  MemSet(&evt, sizeof(evt), 0);
  evt.eType = phoinixErrorEvent;
  EvtAddEventToQueue(&evt);
}

/**
 * Shows a pending error.
 */
void MiscShowPendingError(void) {
  if (ErrMainIndex != miscErrNone) {
    MiscShowError(ErrMainIndex, ErrDetailIndex, ErrParameter);
    ErrMainIndex = miscErrNone;
  }
}

/**
 * Shows the error form.
 *
 * @param errorIndex  index to main string.
 * @param detailIndex index to detail string.
 * @param parameter   pointer to string, must be static.
 */
void MiscShowError(MiscErrorType mainIndex, MiscErrorType detailIndex,
		   const Char *parameter) {
  Char mainLine[miscErrorStringMaxLength];
  Char detailLine[miscErrorStringMaxLength];

  SysStringByIndex(stblIdError, detailIndex, detailLine,
		   miscErrorStringMaxLength);
  if (detailLine[0] == '\0') {
    if (parameter == NULL) {
      StrPrintF(detailLine, "(Minor error #%u)", detailIndex);
    } else {
      StrPrintF(detailLine, "(Minor error #%u\n\"%s\")", detailIndex,
		parameter);
    }
  } else if (parameter != NULL) {
    StrCopy(mainLine, detailLine);
    StrPrintF(detailLine, mainLine, parameter);
  }

  SysStringByIndex(stblIdError, mainIndex, mainLine,
		   miscErrorStringMaxLength);
  if (mainLine[0] == '\0') {
    StrPrintF(mainLine, "(Major error #%u)", mainIndex);
  }

  FrmCustomAlert(alertIdError, mainLine, detailLine, NULL);
}

/* === Helper functions ===============================================	*/

/**
 * Sets the backup bit for the given database.
 *
 * @param dbP reference of the database.
 */
void MiscSetBackupBit(DmOpenRef dbP) {
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

/**
 * Return a pointer to the data structure of an object in a form.
 *
 * @param frmP pointer to the form.
 * @param id   ID of the object.
 * @return     pointer to the data structure.
 */
void *MiscGetObjectPtr(const FormType *frmP, UInt16 id) {
  return FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, id));
}

/* === The end ========================================================	*/
