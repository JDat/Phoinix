/************************************************************************
 *  Phoinix.c
 *                      Main module
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
 *  $Log: Phoinix.c,v $
 *  Revision 1.19  2005/04/03 14:08:33  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.18  2005/01/30 19:35:26  bodowenzel
 *  Removed support for VFS mount and unmount
 *
 *  Revision 1.17  2005/01/29 10:25:34  bodowenzel
 *  Added const qualifier to pointer parameters
 *
 *  Revision 1.16  2005/01/28 17:35:13  bodowenzel
 *  Manager form uses a scrollable table now for the list of games
 *
 *  Revision 1.15  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.14  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.13  2004/09/19 12:35:54  bodowenzel
 *  Enhanced error reporting with string parameter
 *
 *  Revision 1.12  2004/06/20 14:23:20  bodowenzel
 *  VFS mount and unmount support
 *
 *  Revision 1.11  2004/06/11 16:14:19  bodowenzel
 *  Enhanced error reporting with MiscPostError()
 *
 *  Revision 1.10  2004/04/05 21:56:55  bodowenzel
 *  VFS review and its consequences
 *
 *  Revision 1.9  2004/03/02 19:24:16  bodowenzel
 *  Changed to new error function MiscShowError()
 *
 *  Revision 1.8  2004/01/11 19:05:58  bodowenzel
 *  Start of VFS review and correction
 *
 *  Revision 1.7  2003/04/27 09:36:18  bodowenzel
 *  Added Lite edition
 *
 *  Revision 1.6  2002/12/07 08:57:43  bodowenzel
 *  OS5 check removed
 *
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
#include <VFSMgr.h>

#include "Phoinix.h"

#include "sections.h"
#include "misc.h"
#include "prefs.h" 
#include "manager.h"
#include "states.h"
#include "vfs.h"
#include "emulation.h"
#include "video.h"

/* === Function prototypes ============================================	*/

static Err StartApplication(UInt16 launchFlags)
  PHOINIX_SECTION;
static void StopApplication(void)
  PHOINIX_SECTION;
static void EventLoop(void)
  PHOINIX_SECTION;
static Boolean ApplicationHandleEvent(const EventType *evtP)
  PHOINIX_SECTION;

/* === M A I N ========================================================	*/

/**
 * Main function of the application, returns an error code.
 *
 * @param cmd         launch code.
 * @param cmdPBP      pointer to the parameter block.
 * @param launchFlags several flags from the launcher.
 * @return            error code, errNone if all is OK.
 */
UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags) {
  Err ret;

  ret = errNone;

  switch (cmd) {
  case sysAppLaunchCmdNormalLaunch:
    /* normal launch */
    ret = StartApplication(launchFlags);
    if (ret == errNone) {
      EventLoop();
      StopApplication();
    }

  default:
    break;
  }

  return ret;
}

/* === Start and stop =================================================	*/

/**
 * Initializes the application, run once before the event loop.
 *
 * @param launchFlags several flags from the launcher.
 * @return            error code, errNone if all is OK.
 */
static Err StartApplication(UInt16 launchFlags) {
  Err err;

  /* check for adequate OS version */
  err = MiscRomVersionCompatible(
    sysMakeROMVersion(3, 0, 0, sysROMStageRelease, 0),
    0xFFFFFFFFUL, /* no upper limit yet */
    launchFlags);
  if (err != errNone) {
    return err;
  }

  /* check the display, if OK switch to gray */
  if (!VideoGraySwitchOn()) {
    MiscShowError(miscErrInitialization, miscErrScreen, NULL);
    return sysErrParamErr;
  }

  /* set up VFS support */
  if (!VfsInit()) {
    MiscShowError(miscErrVfs, miscErrMemoryFull, NULL);
  }

  /* load preferences, after VFS support to get the volume index */
  PrefsLoad();

  /* try to restart an interrupted game */
  if (StatesAutostart()) {
    FrmGotoForm(formIdEmulation);
  } else {
    FrmGotoForm(formIdManager);
  }
  return errNone;
}

/**
 * Cleans up after the event loop quits.
 */
static void StopApplication(void) {
  /* cleanup GUI */
  FrmCloseAllForms();

  /* cleanup databases */
  StatesClosePrefsAndStatesWithShow(MiscShowError, true);
  ManagerExit();

  /* cleanup volumes */
  VfsExit();

  /* save preferences */
  PrefsSave();

  /* now display the last error, if any */
  MiscShowPendingError();

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
    if (!SysHandleEvent(&EmulationTheEvent)) {
      if (!MenuHandleEvent(NULL, &EmulationTheEvent, &err)) {
	if (!ApplicationHandleEvent(&EmulationTheEvent)) {
	  FrmDispatchEvent(&EmulationTheEvent);
	}
      }
    }
  } while (EmulationTheEvent.eType != appStopEvent);
}

/**
 * Handles globally used events.
 *
 * @param evtP pointer to the event structure.
 * @return     true when the event is handled.
 */
static Boolean ApplicationHandleEvent(const EventType *evtP) {
  FormType  *frmP;

  switch (evtP->eType) {
  case frmLoadEvent:
    frmP = FrmInitForm(evtP->data.frmLoad.formID);
    FrmSetActiveForm(frmP);

    switch (evtP->data.frmLoad.formID) {
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
  case phoinixErrorEvent:
    MiscShowPendingError();
    return true;
  default:
    return false;
  }
}

/* === The end ========================================================	*/
