/************************************************************************
 *  Phoinix.c
 *                      Main module
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
 *  Revision 1.4  2002/10/19 08:08:10  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.3  2002/05/10 15:17:39  bodowenzel
 *  Till's VFS contribution
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:30  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-07-23  Bodo Wenzel     v.4.0, Many sources converted to C,
 *                              compiler/assembler now GCC
 *  2001-07-01  Bodo Wenzel     v.3.3b, Bugfix corrected ;-)
 *  2001-06-23  Bodo Wenzel     v.3.3, Bug in MBC1 code corrected
 *  2001-02-22  Bodo Wenzel     v.3.2 (v.3.14159), Hsync interrupts and
 *                              support of more cartridges
 *  2000-11-18  Bodo Wenzel     v.3.1, Support of color devices
 *  2000-11-03  Bodo Wenzel     v.3.0a, No more "Visor crash"
 *  2000-10-16  Bodo Wenzel     v.3.0, Interrupts and timer
 *  2000-09-30  Bodo Wenzel     v.2.1, Scroller in GB code
 *  2000-09-11  Bodo Wenzel     v.2.0, Nearly full screen emulation
 *  2000-05-07  Bodo Wenzel     v.1.0, First working version
 *  2000-05-06  Bodo Wenzel     v.0.9, First working version with built-in
 *                              games
 *  2000-05-01  Bodo Wenzel     v.0.4, Quick'n'dirty screen conversion,
 *                              splitting in modules, and usage of more
 *                              registers
 *  2000-04-25  Bodo Wenzel     v.0.3, Errors fixed with Palm IIIC, V
 *  2000-04-19  Bodo Wenzel     v.0.2, Creation of user interface
 *  2000-04-15  Bodo Wenzel     v.0.1, Creation of user interface without
 *                              gray image
 *  2000-03-28  Bodo Wenzel     v.0.0, Creation of user interface without
 *                              emulator mock-up and gray image
 ************************************************************************
 */

/* IMPORTANT!
 * Please make sure that any referenced resource is included!
 * Unfortunately there is no automatic!
 */

/* === Includes =======================================================	*/

#include <PalmOS.h>

#include "Phoinix.h"
#include "misc.h"
#include "prefs.h"
#include "manager.h"
#include "states.h"
#include "vfs.h"
#include "emulation.h"
#include "video.h"

/* === Function prototypes ============================================	*/

static Err StartApplication(UInt16 launchFlags);
static void StopApplication(void);
static void EventLoop(void);
static Boolean ApplicationHandleEvent(EventType *evtP);

/* === M A I N ========================================================	*/

/**
 * Main function of the application, returns an error code.
 *
 * @param  cmd         launch code.
 * @param  cmdPBP      pointer to the parameter block.
 * @param  launchFlags several flags from the launcher.
 * @return error code, errNone if all is OK.
 */
UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags) {
  Err ret;

  ret = errNone;

  if (cmd == sysAppLaunchCmdNormalLaunch) {
    /* normal launch */
    ret = StartApplication(launchFlags);
    if (ret == errNone) {
      EventLoop();
      StopApplication();
    }
  }

  return ret;
}

/* === Start and stop =================================================	*/

/**
 * Initializes the application, run once before the event loop.
 *
 * @param  launchFlags several flags from the launcher.
 * @return error code, errNone if all is OK.
 */
static Err StartApplication(UInt16 launchFlags) {
  Err err;

  /* check for adequate OS version */
  err = MiscRomVersionCompatible(
    sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0),
    0xFFFFFFFFUL, /* no upper limit yet */
    launchFlags);
  if (err != errNone)
    return err;

  /* check the display, if OK switch to gray */
  if (!VideoGraySwitchOn()) {
    FrmAlert(alertIdStartupIncompatibleScreen);
    return sysErrParamErr;
  }

  /* load preferences */
  PrefsLoad();

  /* try to access vfs */
  VFSSetup();

  /* try to restart an interrupted game */
  if (StatesAutostartCurrentState())
    FrmGotoForm(formIdEmulation);
  else
    FrmGotoForm(formIdManager);

  return errNone;
}

/**
 * Cleans up after the event loop quits.
 */
static void StopApplication(void) {
  /* cleanup GUI */
  FrmCloseAllForms();

  /* cleanup databases */
  StatesClosePrefsAndStates();
  ManagerExit();

  /* shut down access vfs */
  VFSExit();

  /* save preferences */
  PrefsSave();

  /* switch back to default video mode */
  VideoGraySwitchOff();
}

/* === Event handling =================================================	*/

/**
 * Retrieves and handles all events in all standard cases.
 */
static void EventLoop(void) {
  Err err;

  do {
    EmulationGetEvent();
    if (!SysHandleEvent(&EmulationTheEvent))
      if (!MenuHandleEvent(NULL, &EmulationTheEvent, &err))
	if (!ApplicationHandleEvent(&EmulationTheEvent))
	  FrmDispatchEvent(&EmulationTheEvent);
  } while (EmulationTheEvent.eType != appStopEvent);
}

/**
 * Handles just the frmLoadEvent.
 *
 * @param  evtP pointer to the event structure.
 * @return true when the event is handled.
 */
static Boolean ApplicationHandleEvent(EventType *evtP) {
  FormType *frmP;

  if (evtP->eType == frmLoadEvent) {
    frmP = FrmInitForm(evtP->data.frmLoad.formID);
    FrmSetActiveForm(frmP);

    switch (evtP->data.frmLoad.formID) {
    case formIdNoGame:
      FrmSetEventHandler(frmP, MiscNoGameFormHandleEvent);
      break;
    case formIdManager:
      FrmSetEventHandler(frmP, ManagerFormHandleEvent);
      break;
    case formIdEmulation:
      FrmSetEventHandler(frmP, EmulationFormHandleEvent);
      break;
    default:
      break;
    }
    return true;
  }
  return false;
}

/* === The end ========================================================	*/
