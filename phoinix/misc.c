/************************************************************************
 *  misc.c
 *                      Miscellaneous functions
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

#include "misc.h"
#include "Phoinix.h"

/* === Test for OS version ============================================	*/

/**
 * Checks for the required OS version.
 *
 * @param  minV        value as calculated by sysMakeROMVersion().
 * @param  maxV        value as calculated by sysMakeROMVersion().
 * @param  launchFlags flags given by the launcher.
 * @return error code, errNone if all is OK.
 */
Err MiscRomVersionCompatible(UInt32 minV, UInt32 maxV,
			     Int16 launchFlags) {
  UInt32 romV;

  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romV);
  if (romV >= minV && romV < maxV)
    return errNone;

  if ((launchFlags &
       (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
      (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) {
    FrmAlert(alertIdStartupIncompatibleRom);
    if (romV < sysMakeROMVersion(2, 0, 0, sysROMStageDevelopment, 0))
      /* on OS version before 2 the launcher was just a pop-up! */
      AppLaunchWithCommand(sysFileCDefaultApp, sysAppLaunchCmdNormalLaunch,
			   NULL);
  }

  return sysErrRomIncompatible;
}

/* === Form when no games =============================================	*/

/**
 * Handles the events.
 *
 * @param  evtP pointer to the event structure.
 * @return true when the event is handled.
 */
Boolean MiscNoGameFormHandleEvent(EventType *evtP) {
  switch (evtP->eType) {

  case frmOpenEvent:
    FrmDrawForm(FrmGetActiveForm());
    return true;

  case menuEvent:
    switch (evtP->data.menu.itemID) {
    case menuIdNoGameOptionsTips:
      FrmHelp(helpIdNoGame);
      break;
    case menuIdNoGameOptionsAbout:
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

/* === The "About..." form ============================================	*/

/**
 * Shows the form.
 */
void MiscShowAbout(void) {
  FormType *prevFrmP, *frmP;

  /* prepare new form */
  prevFrmP = FrmGetActiveForm();
  frmP = FrmInitForm(formIdAbout);
  FrmSetActiveForm(frmP);

  /* wait until the form returns */
  FrmDoDialog(frmP);

  /* cleanup and return */
  if (prevFrmP != NULL)
    FrmSetActiveForm(prevFrmP);
  FrmDeleteForm(frmP);
}

/* === The "Busy..." form =============================================	*/

/**
 * Draws or removes the form.
 */
void MiscShowBusy(Boolean on) {
  static UInt8               invocations = 0;
  static WinHandle           oldWinH;
  static FormActiveStateType theFormState;
  static FormType            *frmP;

  if (on) {
    if (invocations == 0) {
      FrmSaveActiveState(&theFormState);
      oldWinH = WinGetDrawWindow();
      frmP = FrmInitForm(formIdBusy);
      FrmSetActiveForm(frmP);
      FrmDrawForm(frmP);
    }
    invocations++;

  } else {
    invocations--;
    if (invocations == 0) {
      FrmEraseForm(frmP);
      FrmDeleteForm(frmP);
      WinSetDrawWindow(oldWinH);
      FrmRestoreActiveState(&theFormState);
    }
  }
}

/* === The end ========================================================	*/
