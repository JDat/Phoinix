/************************************************************************
 *  emulation_c.c
 *                      Main emulation routines (C part)
 *
 ************************************************************************
 *
 * Phoinix,
 * Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
 *
 * (c)2000-2008 Bodo Wenzel
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
 *  $Log: emulation_c.c,v $
 *  Revision 1.34  2008/06/08 08:54:08  bodowenzel
 *  Tracking dead zone for pen cursor
 *
 *  Revision 1.33  2007/02/25 13:15:32  bodowenzel
 *  Show last state in Load State while emulating
 *
 *  Revision 1.32  2007/02/14 15:23:57  bodowenzel
 *  Set HDMA5 bit 7 for better compatibility
 *
 *  Revision 1.31  2007/02/11 17:14:36  bodowenzel
 *  New info control for wall clock
 *  Multiple saved state feature implemented
 *
 *  Revision 1.30  2006/12/19 18:58:43  bodowenzel
 *  Prevent both directions (u/d, l/r) simultaneously
 *
 *  Revision 1.29  2006/11/18 16:18:23  bodowenzel
 *  Capture the Back key of the Fossil Wrist PDA
 *
 *  Revision 1.28  2005/10/11 14:48:23  bodowenzel
 *  Disable Jot on Wrist PDA
 *
 *  Revision 1.27  2005/06/28 15:58:37  bodowenzel
 *  GB didn't redraw after relaunch with paused game
 *
 *  Revision 1.26  2005/06/25 06:48:21  bodowenzel
 *  Redraw of GB screen can't be clipped, delayed now
 *
 *  Revision 1.25  2005/05/06 11:13:25  bodowenzel
 *  Correct redraw when paused
 *
 *  Revision 1.24  2005/05/03 08:40:12  bodowenzel
 *  Moved coldReset, ime, im and error into EmulationCpuState
 *  Show boost state after reset, too
 *  Adding states dirty flag for fewer error messages
 *  Reset pause state after reset
 *
 *  Revision 1.23  2005/04/03 14:07:42  bodowenzel
 *  Open menu with taps in title area also while pausing
 *  Option to save the state for each game
 *
 *  Revision 1.22  2005/03/01 17:19:57  bodowenzel
 *  While emulating open menu with taps in title area
 *
 *  Revision 1.21  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.20  2005/01/30 19:35:26  bodowenzel
 *  Removed support for VFS mount and unmount
 *
 *  Revision 1.19  2005/01/29 10:25:34  bodowenzel
 *  Added const qualifier to pointer parameters
 *
 *  Revision 1.18  2004/12/28 13:57:56  bodowenzel
 *  Disable auto-off with pressed keys
 *  Menu command to press all keys
 *  Menu shortcuts allowed
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.17  2004/12/02 20:01:57  bodowenzel
 *  Utility function MiscGetObjectPointer()
 *  New configurable screen layout
 *
 *  Revision 1.16  2004/10/24 09:11:05  bodowenzel
 *  New button mapping
 *
 *  Revision 1.15  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.14  2004/09/19 12:36:41  bodowenzel
 *  Enhanced error reporting with string parameter
 *  New info type (None)
 *
 *  Revision 1.13  2004/06/20 14:20:45  bodowenzel
 *  Adjustments for volume names, correct RAM name
 *  VFS mount and unmount support
 *
 *  Revision 1.12  2004/06/11 16:14:19  bodowenzel
 *  Enhanced error reporting with MiscPostError()
 *
 *  Revision 1.11  2004/06/06 09:25:01  bodowenzel
 *  Change game's location to RAM on error saving states
 *
 *  Revision 1.10  2004/03/02 19:24:16  bodowenzel
 *  Changed to new error function MiscShowError()
 *
 *  Revision 1.9  2004/01/11 19:07:14  bodowenzel
 *  Corrected copyright
 *
 *  Revision 1.8  2003/04/27 09:35:22  bodowenzel
 *  Support Tungsten T keys (quick-n-dirty)
 *  Added Lite edition
 *
 *  Revision 1.7  2002/10/23 16:33:28  bodowenzel
 *  Better handling of system events (alarms and timeouts)
 *
 *  Revision 1.6  2002/10/19 08:08:10  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.5  2002/02/18 17:16:19  bodowenzel
 *  Crash if autostarting at short memory
 *
 *  Revision 1.4  2002/01/05 10:21:16  bodowenzel
 *  Bug: delay after switch-on while emulating
 *
 *  Revision 1.3  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.2  2001/12/28 18:27:23  bodowenzel
 *  Disabled shortcut-bar for OS >= 3.5, can't intercept its display
 *  Bug found, if restarted twice without running (GB-PC was reset)
 *  Speed display now with TimGetSeconds(), for use with overclockers
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:58  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-16  Bodo Wenzel   Saving states now possible
 *  2001-07-27  Bodo Wenzel   v.4.0beta, changed to C
 *  2001-07-01  Bodo Wenzel   v.3.3b, Bugfix corrected ;-)
 *  2001-06-23  Bodo Wenzel   v.3.3, Bug in MBC1 code corrected
 *  2001-03-18  Bodo Wenzel   Support of more cartridges,
 *                            new stack management, STOP corrected
 *  2000-11-21  Bodo Wenzel   Support of color devices and
 *                            better joypad emulation
 *  2000-11-03  Bodo Wenzel   Request of EasterEgg corrected
 *  2000-10-22  Bodo Wenzel   Interrupts and timer
 *  2000-09-30  Bodo Wenzel   Scroller in GB code
 *  2000-09-13  Bodo Wenzel   Some things in separated files
 *  2000-08-02  Bodo Wenzel   Now nearly full screen emulation
 *  2000-05-07  Bodo Wenzel   First working version
 *  2000-05-06  Bodo Wenzel   First working version with built-in games
 *  2000-05-01  Bodo Wenzel   Got from full source v.0.4 as runmode.asm
 ************************************************************************
 */

/* IMPORTANT!
 * Please make sure that any referenced resource is included!
 * Unfortunately there is no automatic!
 */

/* === Includes =======================================================	*/

/* Access needed until a function is provided, see below */
#define ALLOW_ACCESS_TO_INTERNALS_OF_MENUS

#include <PalmOS.h>

#include "Phoinix.h"

#ifdef handera
	/* Handera H330 support */
	#include <HandEra/Vga.h>
#endif

#ifdef palmgamepad
	/* GamePad support */
	#define UInt8 UInt8 /* they check it with #ifdef :-( */
	#include "GPDLib.h"
#endif

#ifdef fossil
	/* Fossil WristPDA support */
	#include <JotAPI.h>
	#include <WristPDA.h>
#endif

#include "emulation.h"


#include "prefs.h"
#include "misc.h"
#include "states.h"
#include "ram.h"
#include "vfs.h"
#include "video.h"
#include "memory.h"
#include "gbemu.h"

/* === Constants ======================================================	*/

/* Key bits are inverted: released/pressed = 1/0 */
#define joypadCursorMask    (0xef) /* bits: 1 1 1 0 down up left right */
#define joypadCursorDown    (0x08)
#define joypadCursorUp      (0x04)
#define joypadCursorLeft    (0x02)
#define joypadCursorRight   (0x01)
#define joypadControlMask   (0xdf) /* bits: 1 1 0 1 start select b a */
#define joypadControlStart  (0x08)
#define joypadControlSelect (0x04)
#define joypadControlB      (0x02)
#define joypadControlA      (0x01)
#define joypadAllMask       (0x0f)

#define showBatteryPeriod (10) /* seconds */

#define keyHoldFrames (15) /* frames */

#define penTitleHeight (16) /* pen coordinates */
#define penMinDeadZone (10) /* pen coordinates */

/* === Type definitions ===============================================	*/

typedef enum {
  keyHoldFsmOff = 0,
  keyHoldFsmIdle,
  keyHoldFsmHold1,
  keyHoldFsmWait1,
  keyHoldFsmHold2,
  keyHoldFsmWait2,
} KeyHoldFsmType;

typedef enum {
  penStateUp = 0,
  penStateTitle,
  penStateCursor,
  penStateControl,
} PenStateType;

/* === Global and static variables ====================================	*/

EventType EmulationTheEvent;

static Boolean CmdBarOpen = false;

static FormType      *FormP = NULL;
static ControlType   *PauseCtlP;
static ControlType   *BoostCtlP;
static ControlType   *SelectCtlP;
static RectangleType SelectBounds;
static ControlType   *StartCtlP;
static RectangleType StartBounds;
static ControlType   *BCtlP;
static RectangleType BBounds;
static ControlType   *ACtlP;
static RectangleType ABounds;
static ControlType   *TimeCtlP;
static ControlType   *BatteryCtlP;
static ControlType   *PaceCtlP;
static ControlType   *ClockCtlP;

#ifdef palmgamepad
static UInt16 GamePadLibRefNum;
#endif
static MemHandle             EmulatorH = NULL;
static Boolean               Running = false;
static Boolean               ResetRunning;
static Boolean               Paused;
static EmulationCpuStateType SavedCpuState;
static Boolean               Crashed;

static UInt32 Seconds;
static UInt32 GameTime;
static UInt16 FramesPerSecond[2];

static UInt32         KeyMask[emulationKeyCount];
static KeyHoldFsmType KeyHoldFsmVertical;
static KeyHoldFsmType KeyHoldFsmHorizontal;
static UInt16         KeyAllKeysHoldFrames;

static PenStateType PenState;
static Coord        PenCenterX;
static Coord        PenCenterY;
static Coord        PenDeadZone;
static UInt8        PenJoypadCursor;
static UInt8        PenJoypadControl;

/* Address of emulator kernel; NOTE: This points to combo 0x00 at offset
 * 0x8000 because of SInt16 indices!
 */
GbemuComboType *EmulationEmulator;

UInt8 *EmulationJoypadPtr;
UInt8 EmulationJoypad[4];
UInt8 EmulationJoypadOld; /* saved joypad value before user poll */

/* === Function prototypes ============================================	*/

#ifdef fossil
static UInt32 Jot(UInt32 feature)
  EMULATION_SECTION;
#endif

static void ShowForm(void)
  EMULATION_SECTION;
static void ShowInfo(void)
  EMULATION_SECTION;
static void LoadStateDialog(void)
  EMULATION_SECTION;
static Boolean LoadStateFormHandleEvent(EventType *evtP)
  EMULATION_SECTION;

static void SetupKeyHandling(void)
  EMULATION_SECTION;
static void HandleHoldFsm(KeyHoldFsmType *holdFsmP, UInt32 *keysP,
			  UInt32 mask1, UInt32 mask2)
  EMULATION_SECTION;

static void SetupScreenLayout(FormType *frmP)
  EMULATION_SECTION;
static Boolean PenStartTitle(Coord x, Coord y)
  EMULATION_SECTION;
#define PenTrackTitle(x, y) /* not needed */
static Boolean PenStartCursor(Coord x, Coord y)
  EMULATION_SECTION;
static void PenTrackCursor(Coord x, Coord y)
  EMULATION_SECTION;
#define PenStartControl PenTrackControl
static Boolean PenTrackControl(Coord x, Coord y)
  EMULATION_SECTION;
static void PenFinish(Coord x, Coord y)
  EMULATION_SECTION;

static void SetupReset(void)
  EMULATION_SECTION;
static void SetupGame(void)
  EMULATION_SECTION;

/* === User interface =================================================	*/

/**
 * Retrieves the next event and stores it in the global variable. While
 * emulating, events are sampled at another function; on return from
 * emulation, the GB is switched if appropriate.
 */
void EmulationGetEvent(void) {
  UInt16  initDelayOld, periodOld, doubleTapDelayOld;
  Boolean queueAheadOld;
  UInt16  initDelay, period, doubleTapDelay;
  Boolean queueAhead;

#ifdef fossil
  UInt32  jotFeature = 0;
  UInt16  fossilBackKeyMode = 0;
#endif

  /* track opened command bar on OS 3.5 and greater */
  if (CmdBarOpen) {
    if (!MenuCmdBarGetButtonData (0, NULL, NULL, NULL, NULL)) {
      /* this works only if at least one icon is shown */
      CmdBarOpen = false;
    }
  }

  if (CmdBarOpen || !Running) {
    /* emulation is not running */
    if (FormP != NULL) {
      ShowInfo();
    }
    EvtGetEvent(&EmulationTheEvent,
		showBatteryPeriod * SysTicksPerSecond());
    return;
  }

  /* if there are still events, handle them all before emulating */
  if (EmulationTheEvent.eType != nilEvent) {
    if (EvtEventAvail()) {
      EvtGetEvent(&EmulationTheEvent, evtNoWait);

      if (EmulationTheEvent.eType != keyDownEvent) {
	return;
      } else {
	/* filter mapped application buttons */
	UInt16 modifiers;

	modifiers = EmulationTheEvent.data.keyDown.modifiers;
	if ((modifiers & commandKeyMask) == 0 ||
	    (modifiers & poweredOnKeyMask) != 0) {
	  return;
	} else {
	  UInt16 i;

	  for (i = 0; i < emulationKeyCount; i++) {
	    UInt16 chr;

	    chr = PrefsPreferences.buttonMapping.key[i].chr;
	    if (chr != prefsButtonMappingNoChr &&
		chr == EmulationTheEvent.data.keyDown.chr) {
	      break;
	    }
	  }
	  if (i >= emulationKeyCount) {
	    return;
	  }
	}
      }
    }
  }

  /* try to reduce the frequency of key events
   * (though the docs say it doesn't work really well)
   */
  KeyRates(false, &initDelayOld, &periodOld, &doubleTapDelayOld,
	   &queueAheadOld);
  initDelay = slowestKeyDelayRate;
  period = slowestKeyPeriodRate;
  doubleTapDelay = slowestKeyPeriodRate;
  queueAhead = false;
  KeyRates(true, &initDelay, &period, &doubleTapDelay, &queueAhead);

#ifdef fossil
  /* special handling of the Wrist PDA */
  if (MiscDevice == miscDeviceWrist) {
    jotFeature = Jot(DISABLE_JOT);
    if ((jotFeature & DISABLE_JOT) == 0) {
      jotFeature = ENABLE_JOT;
    }
    fossilBackKeyMode = FossilBackKeyModeGet();
    FossilBackKeyModeSet(kFossilBackKeyNoAction);
  }
#endif

  /* release memory protection */
  MemSemaphoreReserve(true);

  /* *** HERE IT IS! THE EMULATOR STARTS EXECUTION... *****************	*/
  EmulationStartKernel();
  /* ******************************************************************	*/

  if (ResetRunning) {
    /* the GB reset code ends with an unknown opcode - normally ;-) */
    if (EmulationCpuState.error == gbemuRetOpcodeUnknown) {
      /* setup the game now */
      SetupGame();
      /* prevent stopping */
      EmulationCpuState.error = gbemuRetUserStop;
      EmulationTheEvent.eType = nilEvent;
    }
  }

  /* regain memory protection */
  MemSemaphoreRelease(true);

#ifdef fossil
  /* restore settings on Wrist PDA */
  if (MiscDevice == miscDeviceWrist) {
    (void)Jot(jotFeature);
    FossilBackKeyModeSet(fossilBackKeyMode);
  }
#endif

  /* restore key rates */
  KeyRates(true, &initDelayOld, &periodOld, &doubleTapDelayOld,
	   &queueAheadOld);

  /* on emulation errors alert and go back to the manager */
  if (EmulationCpuState.error != gbemuRetUserStop) {
    MiscShowError(miscErrEmulator, miscErrRuntime, NULL);
    FrmGotoForm(formIdManager);

    /* stop the emulation right NOW! */
    PrefsPreferences.running = false;
    Running = false;
    Crashed = true;
    EmulationTheEvent.eType = nilEvent;
  }
}

/**
 * Handles events in the emulation form.
 *
 * @param evtP pointer to the event structure.
 * @return     true if the event is handled.
 */
Boolean EmulationFormHandleEvent(EventType *evtP) {
#ifdef palmgamepad
  Err err;
#endif
  switch (evtP->eType) {

  case frmOpenEvent:
    FormP = FrmGetActiveForm();

    /* remember pointer to display buttons for ShowInfo() */
    TimeCtlP = MiscGetObjectPtr(FormP, buttonIdEmulationTime);
    BatteryCtlP = MiscGetObjectPtr(FormP, buttonIdEmulationBattery);
    PaceCtlP = MiscGetObjectPtr(FormP, buttonIdEmulationPace);
    ClockCtlP = MiscGetObjectPtr(FormP, buttonIdEmulationClock);

    /* setup pause checkbox */
    PauseCtlP = MiscGetObjectPtr(FormP, checkboxIdEmulationPause);
    CtlSetValue(PauseCtlP, Paused ? 1 : 0);

    /* setup boost checkbox */
    BoostCtlP = MiscGetObjectPtr(FormP, checkboxIdEmulationBoost);
    CtlSetValue(BoostCtlP, VideoBoosted ? 1 : 0);

    /* setup screen layout */
    SetupScreenLayout(FormP);

    /* show form, sets draw window */
    ShowForm();

    /* setup key handling */
    SetupKeyHandling();

#ifdef palmgamepad
    /* reserve and open GamePad library if present */
    err = SysLibFind(GPD_LIB_NAME, &GamePadLibRefNum);
    if (err == sysErrLibNotFound) {
      err = SysLibLoad('libr', GPD_LIB_CREATOR, &GamePadLibRefNum);
    }
    if (err == errNone) {
      err = GPDOpen(GamePadLibRefNum);
    }
    if (err != errNone && err != GPDErrGamePadNotPresent) {
      GamePadLibRefNum = sysInvalidRefNum;
    }
#endif

    /* start the GB reset code */
    SetupReset();

    VideoSetupPaceControl();

    /* setup for user interface while emulating */
    Seconds = TimGetSeconds();
    ShowInfo();
    return true;

  case frmUpdateEvent:
    ShowForm();
    return true;

  case frmCloseEvent:
    /* reset variables for EmulationGetEvent() */
    FormP = NULL;
    Running = false;

    /* stop emulation gracefully */
    if (Crashed) {
      StatesSaveCurrentStateAsCrashed();
    } else if (PrefsPreferences.running ||
	       (StatesEmulationOptionsP->save && !ResetRunning)) {
      StatesSaveCurrentState();
    } else {
      StatesResetCurrentState();
    }
    StatesSetPrefsAndStatesDirty();
    VideoCleanup();
    EmulationCleanup();

    /* close states database here so that MemoryCleanup() works correctly
     * in case of errors
     */
    StatesClosePrefsAndStatesWithShow(MiscPostError, false);
    MemoryCleanup();

#ifdef palmgamepad
    /* if GamePad library was found, close and release it */
    if (GamePadLibRefNum != sysInvalidRefNum) {
      UInt32 numUsers;

      GPDClose(GamePadLibRefNum, &numUsers);
      if (numUsers == 0) {
	SysLibRemove(GamePadLibRefNum);
      }
    }
#endif
    return false;

  case winExitEvent:
    /* stop the emulation while handling the GUI events */
    if (evtP->data.winExit.exitWindow == FrmGetWindowHandle(FormP)) {
      Running = false;
    }
    return false;

  case winEnterEvent:
    /* restart the emulation after handling the GUI events */
    if (evtP->data.winEnter.enterWindow == FrmGetWindowHandle(FormP)) {
      Running = (ResetRunning || !Paused) && !Crashed;
      VideoSetupPaceControl();

      /* now we can safely update the GB screen */
      MemSemaphoreReserve(true);
      VideoCopyScreen();
      MemSemaphoreRelease(true);

      /* this works also after changing the button mapping */
      SetupKeyHandling();
    }
    return false;

  case ctlSelectEvent:
    switch (evtP->data.ctlSelect.controlID) {

    case checkboxIdEmulationPause:
      Paused = evtP->data.ctlSelect.on;
      Running = ResetRunning || !Paused;
      VideoSetupPaceControl();
      /* in case the emulation was stopped, correct the seconds */
      Seconds = TimGetSeconds();
      break;

    case checkboxIdEmulationBoost:
      VideoBoosted = evtP->data.ctlSelect.on;
      VideoSetupPaceControl();
      break;

    default:
      return false;
    }
    return true;

  case penDownEvent:
    if (PenStartTitle(EmulationTheEvent.screenX,
		      EmulationTheEvent.screenY)) {
      PenState = penStateTitle;
    } else {
      PenState = penStateUp;
    }
    return false;

  case penMoveEvent:
    switch (PenState) {
    case penStateTitle:
      PenTrackTitle(EmulationTheEvent.screenX, EmulationTheEvent.screenY);
      break;
    default:
      break;
    }
    return false;

  case penUpEvent:
    if (PenState != penStateUp) {
      PenFinish(EmulationTheEvent.screenX, EmulationTheEvent.screenY);
      PenState = penStateUp;
    }
    return false;

  case menuCmdBarOpenEvent:
    CmdBarOpen = true;
    MenuCmdBarAddButton(menuCmdBarOnRight, BarPasteBitmap,
			menuCmdBarResultMenuItem,
			menuIdEmulationStateLoad, NULL);
    MenuCmdBarAddButton(menuCmdBarOnRight, BarCopyBitmap,
			menuCmdBarResultMenuItem,
			menuIdEmulationStateSave, NULL);
    MenuCmdBarAddButton(menuCmdBarOnRight, BarUndoBitmap,
			menuCmdBarResultMenuItem,
			menuIdEmulationGameReset, NULL);
    MenuCmdBarAddButton(menuCmdBarOnRight, BarDeleteBitmap,
			menuCmdBarResultMenuItem,
			menuIdEmulationGameQuit, NULL);
    return false;

  case menuEvent:
    /* the display of the chosen menu command can't be catched and it will
     * get garbled by Phoinix' output, so it has to erased if displayed
     */
    MenuEraseStatus(NULL);

    switch (evtP->data.menu.itemID) {

    case menuIdEmulationGamePressAllKeys:
      KeyAllKeysHoldFrames = keyHoldFrames;
      break;

    case menuIdEmulationGamePause:
      /* toggle Paused */
      Paused = !Paused;
      CtlSetValue(PauseCtlP, Paused ? 1 : 0);
      Running = ResetRunning || !Paused;
      VideoSetupPaceControl();
      /* in case the emulation was stopped, correct the seconds */
      Seconds = TimGetSeconds();
      break;

    case menuIdEmulationGameBoost:
      /* toggle Boosted */
      VideoBoosted = !VideoBoosted;
      CtlSetValue(BoostCtlP, VideoBoosted ? 1 : 0);
      VideoSetupPaceControl();
      break;

    case menuIdEmulationGameReset:
      StatesResetCurrentState();
      StatesLoadCurrentState();
      /* start the GB reset code */
      SetupReset();
      /* reflect changes on screen */
      CtlSetValue(PauseCtlP, Paused ? 1 : 0);
      CtlSetValue(BoostCtlP, VideoBoosted ? 1 : 0);
      ShowInfo();
      break;

    case menuIdEmulationGameQuit:
      PrefsPreferences.running = false;
      FrmGotoForm(formIdManager);
      /* frmCloseEvent will come, too... */
      break;

    case menuIdEmulationStateLoad:
      LoadStateDialog();
      /* reflect changes on screen */
      CtlSetValue(PauseCtlP, Paused ? 1 : 0);
      CtlSetValue(BoostCtlP, VideoBoosted ? 1 : 0);
      ShowInfo();
      break;

    case menuIdEmulationStateSave:
      if (!StatesCreateStateFromCurrentState()) {
	MiscShowError(miscErrEmulator, miscErrStateSave, NULL);
      }
      break;

    case menuIdEmulationOptionsEmulation:
      PrefsEmulationOptionsDialog(StatesEmulationOptionsP);
      VideoSetupPaceControl();
      break;

    case menuIdEmulationOptionsButtonMapping:
      PrefsButtonMappingDialog(StatesButtonMappingP);
      break;

    case menuIdEmulationOptionsScreenLayout:
      PrefsScreenLayoutDialog(StatesScreenLayoutP);
      SetupScreenLayout(FormP);
      break;

    case menuIdEmulationOptionsTips:
      FrmHelp(helpIdEmulation);
      break;

    case menuIdEmulationOptionsAbout:
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

#ifdef fossil
/**
 * Calls Jot on Wrist PDA.
 *
 * @param feature bit mask of feature to switch.
 * @return        bit mask of currently set features.
 */
static UInt32 Jot(UInt32 feature) {
  UInt16            cardNo;
  LocalID           dbId;
  DmSearchStateType searchState;

  if (DmGetNextDatabaseByTypeCreator(
	true, &searchState, sysFileTApplication, JOTCREATOR, true,
	&cardNo, &dbId) == errNone) {
    SysAppLaunch(cardNo, dbId, 0, 32801, &feature, NULL);
    return feature;
  }
  return 0;
}
#endif

/**
 * Draws the emulation form, on bigger screens with a visible frame for
 * the GB screen.
 */
static void ShowForm(void) {
  /* show form */
  FrmDrawForm(FormP);

#ifdef handera
  /* on H330, switch screen mode to draw the full rectangle */
  if (MiscDevice == miscDeviceH330) {
    VgaSetScreenMode(screenMode1To1, rotateModeNone);
    WinSetDrawWindow(WinGetDisplayWindow());
  }
#endif
  WinDrawRectangleFrame(simpleFrame, &VideoGbScreenBounds);
#ifdef handera
  if (MiscDevice == miscDeviceH330) {
    VgaSetScreenMode(screenModeScaleToFit, rotateModeNone);
    WinSetDrawWindow(FrmGetWindowHandle(FormP));
  }
#endif
}

/**
 * Draws the info lines.
 */
static void ShowInfo(void) {
  MenuBarType  *menuBarP;
  UInt16       h, m;
  UInt8        b;
  UInt16       si, sf;
  DateTimeType dt;
  static Char  timeLine[8];
  static Char  batteryLine[8];
  static Char  paceLine[8];
  static Char  clockLine[8];

  /* don't draw over an open command bar */
  if (CmdBarOpen) {
    return;
  }
  menuBarP = MenuGetActiveMenu();
  if (menuBarP != NULL) {
    /* this is for OS versions below 3.5 */
    /* we need ALLOW_ACCESS_TO_INTERNALS_OF_MENUS defined */
    if (menuBarP->attr.commandPending) {
      return;
    }
  }

  /* show played time */
  m = (UInt16)(GameTime / 60);
  h = m / 60;
  m %= 60;
  StrPrintF(timeLine, "%u:%02u", h, m);
  CtlSetLabel(TimeCtlP, timeLine);

  /* show current battery power */
  (void)SysBatteryInfo(false, NULL, NULL, NULL, NULL, NULL, &b);
  StrPrintF(batteryLine, "%u%%", b);
  CtlSetLabel(BatteryCtlP, batteryLine);

  /* show emulation speed */
  sf = (50 * (FramesPerSecond[0] + FramesPerSecond[1])) /
    videoFramesPerSec;
  si = sf / 100;
  sf %= 100;
  StrPrintF(paceLine, "%u.%02u\xd7", si, sf);
  CtlSetLabel(PaceCtlP, paceLine);

  /* show wall clock */
  TimSecondsToDateTime(TimGetSeconds(), &dt);
  StrPrintF(clockLine, "%u:%02u", dt.hour, dt.minute);
  CtlSetLabel(ClockCtlP, clockLine);
}

/**
 * Shows the load state form.
 */
static void LoadStateDialog(void) {
  FormType *prevFrmP, *frmP;
  ListType *lstP;
  Int16    index;

  /* prepare new form */
  prevFrmP = FrmGetActiveForm();
  frmP = FrmInitForm(formIdLoadState);
  FrmSetActiveForm(frmP);
  FrmSetEventHandler(frmP, LoadStateFormHandleEvent);
  lstP = MiscGetObjectPtr(frmP, listIdLoadState);
  StatesSetupList(frmP, lstP, statesLastState);

  /* show form and get selected entry */
  FrmDoDialog(frmP);
  index = LstGetSelection(lstP);

  /* cleanup */
  if (prevFrmP != NULL) {
    FrmSetActiveForm(prevFrmP);
  }
  FrmDeleteForm(frmP);

  /* the first entry is the default state = cancel */
  if (index > 0) {
    if (!StatesCopyStateToCurrentState(index)) {
      MiscShowError(miscErrEmulator, miscErrStateLoad, NULL);
    } else {
      StatesLoadCurrentState();
      /* start the GB reset code */
      SetupReset();
    }
  }
}

/**
 * Handles events in the load state form.
 *
 * @param evtP pointer to the event structure.
 * @return     true if the event is handled.
 */
static Boolean LoadStateFormHandleEvent(EventType *evtP) {
  FormType *frmP;
  ListType *lstP;

  frmP = FrmGetActiveForm();
  lstP = MiscGetObjectPtr(frmP, listIdLoadState);

  switch (evtP->eType) {

  case winEnterEvent:
    /* apparently we get this event only for our window */
    /* there is no frmUpdateEvent for modal forms through FrmDoDialog */
    StatesDrawGoButton(frmP, lstP, buttonIdLoadState, labelIdLoadState);
    return false;

  case lstSelectEvent:
    StatesDrawGoButton(frmP, lstP, buttonIdLoadState, labelIdLoadState);
    return true;

  case appStopEvent:
    /* to cancel the first entry must be selected */
    LstSetSelection(lstP, 0);
    return false;

  default:
    return false;
  }
}

/* === Polling while emulating ========================================	*/

/**
 * Polls the user interface and the system. When events have to be
 * handled, the emulation is stopped.
 *
 * @return true if the emulation should quit.
 */
Boolean EmulationUserPoll(void) {
  Boolean getEvent, quit;
  UInt8   joypadCursor, joypadControl;
  UInt32  keys;
  UInt32  secs;
#ifdef palmgamepad  
  UInt8   gamepadKeys;
#endif

  /* default: don't quit the emulation... */
  quit = false;

  /* regain memory protection */
  MemSemaphoreRelease(true);

  /* start with no buttons pressed */
  joypadCursor = joypadCursorMask;
  joypadControl = joypadControlMask;

  /* read and evaluate own hard keys */
  keys = KeyCurrentState();
  HandleHoldFsm(&KeyHoldFsmVertical, &keys,
		KeyMask[offsetButtonMappingUp],
		KeyMask[offsetButtonMappingDown]);
  HandleHoldFsm(&KeyHoldFsmHorizontal, &keys,
		KeyMask[offsetButtonMappingLeft],
		KeyMask[offsetButtonMappingRight]);
  if ((keys & KeyMask[offsetButtonMappingUp]) != 0) {
    joypadCursor &= ~joypadCursorUp;
  }
  if ((keys & KeyMask[offsetButtonMappingDown]) != 0) {
    joypadCursor &= ~joypadCursorDown;
  }
  if ((keys & KeyMask[offsetButtonMappingLeft]) != 0) {
    joypadCursor &= ~joypadCursorLeft;
  }
  if ((keys & KeyMask[offsetButtonMappingRight]) != 0) {
    joypadCursor &= ~joypadCursorRight;
  }
  if ((keys & KeyMask[offsetButtonMappingSelect]) != 0) {
    joypadControl &= ~joypadControlSelect;
  }
  if ((keys & KeyMask[offsetButtonMappingStart]) != 0) {
    joypadControl &= ~joypadControlStart;
  }
  if ((keys & KeyMask[offsetButtonMappingB]) != 0) {
    joypadControl &= ~joypadControlB;
  }
  if ((keys & KeyMask[offsetButtonMappingA]) != 0) {
    joypadControl &= ~joypadControlA;
  }

#ifdef palmgamepad
  /* if found, check GamePad's keys */
  if (GamePadLibRefNum != sysInvalidRefNum) {
    if (GPDReadInstant(GamePadLibRefNum, &gamepadKeys) == errNone) {
      if ((gamepadKeys & GAMEPAD_DOWN) != 0) {
	joypadCursor &= ~joypadCursorDown;
      }
      if ((gamepadKeys & GAMEPAD_UP) != 0) {
	joypadCursor &= ~joypadCursorUp;
      }
      if ((gamepadKeys & GAMEPAD_LEFT) != 0) {
	joypadCursor &= ~joypadCursorLeft;
      }
      if ((gamepadKeys & GAMEPAD_RIGHT) != 0) {
	joypadCursor &= ~joypadCursorRight;
      }
      if ((gamepadKeys & GAMEPAD_START) != 0) {
	joypadControl &= ~joypadControlStart;
      }
      if ((gamepadKeys & GAMEPAD_SELECT) != 0) {
	joypadControl &= ~joypadControlSelect;
      }
      if ((gamepadKeys & GAMEPAD_LEFTFIRE) != 0) {
	joypadControl &= ~joypadControlB;
      }
      if ((gamepadKeys & GAMEPAD_RIGHTFIRE) != 0) {
	joypadControl &= ~joypadControlA;
      }
    }
  }
#endif

  secs = TimGetSeconds();
  if (secs != Seconds) {
    /* update info */
    Seconds = secs;

    /* latch current frame rate */
    FramesPerSecond[1] = FramesPerSecond[0];
    FramesPerSecond[0] = VideoFrames;
    VideoFrames = 0;

    /* limit the played time, wrap around */
    if (++GameTime == emulationMaxTime) {
      GameTime = 0;
    }

    ShowInfo();

    /* if any hard keys is pressed, reset auto-off time */
    if ((joypadCursor & joypadControl & joypadAllMask) != joypadAllMask) {
      EvtResetAutoOffTimer();
    }

    /* just to enable system alarms and timeouts... */
    getEvent = true;

  } else if (EvtEventAvail()) {
    /* handle events */
    getEvent = true;

  } else {
    /* no events need processing */
    getEvent = false;
  }

  if (getEvent) {
    /* on events, mostly quit the emulation */
    EvtGetEvent(&EmulationTheEvent, evtNoWait);
    quit = true;

    switch (EmulationTheEvent.eType) {

    case nilEvent:
      if (PenState != penStateUp) {
	quit = false;
      }
      break;

    case penDownEvent:
      if (PenStartTitle(EmulationTheEvent.screenX,
			EmulationTheEvent.screenY)) {
	PenState = penStateTitle;
	quit = false;
      } else if (PenStartCursor(EmulationTheEvent.screenX,
				EmulationTheEvent.screenY)) {
	PenState = penStateCursor;
	quit = false;
      } else if (PenStartControl(EmulationTheEvent.screenX,
				 EmulationTheEvent.screenY)) {
	PenState = penStateControl;
	quit = false;
      } else {
	PenState = penStateUp;
      }
      break;

    case penMoveEvent:
      switch (PenState) {
      case penStateTitle:
	PenTrackTitle(EmulationTheEvent.screenX,
		      EmulationTheEvent.screenY);
	quit = false;
	break;
      case penStateCursor:
	PenTrackCursor(EmulationTheEvent.screenX,
		       EmulationTheEvent.screenY);
	quit = false;
	break;
      case penStateControl:
	(void)PenTrackControl(EmulationTheEvent.screenX,
			      EmulationTheEvent.screenY);
	quit = false;
	break;
      default:
	break;
      }
      break;

    case penUpEvent:
      if (PenState != penStateUp) {
        PenFinish(EmulationTheEvent.screenX, EmulationTheEvent.screenY);
	PenState = penStateUp;
	quit = false;
      }
      break;

    case keyDownEvent:
      if ((EmulationTheEvent.data.keyDown.modifiers & commandKeyMask) !=
	  0) {
	UInt16 i;

	for (i = 0; i < emulationKeyCount; i++) {
	  UInt16 chr;

	  chr = StatesButtonMappingP->key[i].chr;
	  if (chr != prefsButtonMappingNoChr &&
	      chr == EmulationTheEvent.data.keyDown.chr) {
	    /* filter mapped application buttons */
	    quit = false;
	    break;
	  }
	}
      }
      break;

    default:
      break;
    }
  }

  /* merge in joypad masks from pen tracking */
  joypadCursor &= PenJoypadCursor;
  joypadControl &= PenJoypadControl;
  if (KeyAllKeysHoldFrames != 0) {
    KeyAllKeysHoldFrames--;
    joypadControl &= joypadControlSelect & joypadControlStart &
      joypadControlB & joypadControlA;
  }

  /* this can be done on each turn, it doesn't cost relevant time! */
  CtlSetValue(SelectCtlP, (joypadControl & joypadControlSelect) == 0);
  CtlSetValue(StartCtlP, (joypadControl & joypadControlStart) == 0);
  CtlSetValue(BCtlP, (joypadControl & joypadControlB) == 0);
  CtlSetValue(ACtlP, (joypadControl & joypadControlA) == 0);

  VideoControlPace();

  /* ensure that only one direction is pressed */
  if ((joypadCursor & (joypadCursorUp | joypadCursorDown)) == 0) {
    joypadCursor |= joypadCursorUp | joypadCursorDown;
  }
  if ((joypadCursor & (joypadCursorLeft | joypadCursorRight)) == 0) {
    joypadCursor |= joypadCursorLeft | joypadCursorRight;
  }

  /* release memory protection */
  MemSemaphoreReserve(true);

  /* copy buttons into emulation */
  EmulationJoypad[emulationJoypadCurs] = joypadCursor;
  EmulationJoypad[emulationJoypadCtrl] = joypadControl;
  EmulationJoypad[emulationJoypadBoth] = joypadCursor & joypadControl;

  return quit;
}

/* === Key handling ===================================================	*/

/**
 * Sets up the key handling.
 */
static void SetupKeyHandling(void) {
  UInt16 i;

  /* set up key masks */
  for (i = 0; i < emulationKeyCount; i++) {
    if (StatesButtonMappingP->key[i].bit != prefsButtonMappingNoBit) {
      KeyMask[i] = (UInt32)1 << StatesButtonMappingP->key[i].bit;
    } else {
      KeyMask[i] = 0;
    }
  }

  /* initialize hold FSM */
  KeyHoldFsmVertical = StatesButtonMappingP->holdVertical ?
    keyHoldFsmIdle : keyHoldFsmOff;
  KeyHoldFsmHorizontal = StatesButtonMappingP->holdHorizontal ?
    keyHoldFsmIdle : keyHoldFsmOff;

  /* initialize key hold period */
  KeyAllKeysHoldFrames = 0;
}

/**
 * Handle key hold FSM (finite state machine).
 *
 * @param holdFsmP pointer to FSM.
 * @param keysP    pointer to current key bit mask.
 * @param mask1    mask of key 1.
 * @param mask2    mask of key 2.
 */
static void HandleHoldFsm(KeyHoldFsmType *holdFsmP, UInt32 *keysP,
			  UInt32 mask1, UInt32 mask2) {
  /* do nothing if holding is off */
  if (*holdFsmP == keyHoldFsmOff) {
    return;
  }

  if ((*keysP & mask1) != 0) {
    if (*holdFsmP == keyHoldFsmHold2 ||
	*holdFsmP == keyHoldFsmWait2) {
      *holdFsmP = keyHoldFsmWait2;
      *keysP &= ~mask1;
    } else {
      *holdFsmP = keyHoldFsmHold1;
    }
  } else {
    if (*holdFsmP == keyHoldFsmWait2) {
      *holdFsmP = keyHoldFsmIdle;
    }
  }

  if ((*keysP & mask2) != 0) {
    if (*holdFsmP == keyHoldFsmHold1 ||
	*holdFsmP == keyHoldFsmWait1) {
      *holdFsmP = keyHoldFsmWait1;
      *keysP &= ~mask2;
    } else {
      *holdFsmP = keyHoldFsmHold2;
    }
  } else {
    if (*holdFsmP == keyHoldFsmWait1) {
      *holdFsmP = keyHoldFsmIdle;
    }
  }

  if (*holdFsmP == keyHoldFsmHold1) {
    *keysP |= mask1;
  }
  if (*holdFsmP == keyHoldFsmHold2) {
    *keysP |= mask2;
  }
}

/* === Screen tap handling ============================================	*/

/**
 * Sets up the screen layout.
 *
 * @param frmP pointer to the form.
 */
static void SetupScreenLayout(FormType *frmP) {
  UInt16 i, ctlIndex;

  /* first hide all controls */
  for (i = offsetEmulationMin; i < offsetEmulationMax; i++) {
    ctlIndex = FrmGetObjectIndex(frmP, ctlIdEmulation + i);
    FrmHideObject(frmP, ctlIndex);
    /* work-around for OS versions below 3.2: */
    CtlSetUsable(FrmGetObjectPtr(frmP, ctlIndex), false);
  }

  /* now show only the configured controls */
  for (i = 0; i < prefsScreenLayoutControlCount; i++) {
    if (StatesScreenLayoutP->ctl[i] != offsetEmulationNone) {
      Coord x, y;

      ctlIndex = FrmGetObjectIndex(
	frmP, ctlIdEmulation + StatesScreenLayoutP->ctl[i]);
      FrmGetObjectPosition(frmP, ctlIndex, &x, &y);
      x %= ctlOffsetEmulation;
      x += i * ctlOffsetEmulation;
      FrmSetObjectPosition(frmP, ctlIndex, x, y);
      FrmShowObject(frmP, ctlIndex);
    }
  }

  /* cache control properties */
  SelectCtlP = MiscGetObjectPtr(frmP, buttonIdEmulationSelect);
  FrmGetObjectBounds(frmP,
		     FrmGetObjectIndex(frmP, buttonIdEmulationSelect),
		     &SelectBounds);
  StartCtlP = MiscGetObjectPtr(FormP, buttonIdEmulationStart);
  FrmGetObjectBounds(frmP,
		     FrmGetObjectIndex(frmP, buttonIdEmulationStart),
		     &StartBounds);
  BCtlP = MiscGetObjectPtr(FormP, buttonIdEmulationB);
  FrmGetObjectBounds(frmP,
		     FrmGetObjectIndex(frmP, buttonIdEmulationB),
		     &BBounds);
  ACtlP = MiscGetObjectPtr(FormP, buttonIdEmulationA);
  FrmGetObjectBounds(frmP,
		     FrmGetObjectIndex(frmP, buttonIdEmulationA),
		     &ABounds);

  /* no button pressed yet */
  PenState = penStateUp;
  PenJoypadCursor = joypadCursorMask;
  PenJoypadControl = joypadControlMask;
}

/**
 * Starts tracking the pen in the title area.
 *
 * @param x x coordinate of pen.
 * @param y y coordinate of pen.
 * @return  true if pen is in the title area.
 */
static Boolean PenStartTitle(Coord x, Coord y) {
  return y < penTitleHeight;
}

/**
 * Starts tracking the pen for cursor control.
 *
 * @param x x coordinate of pen.
 * @param y y coordinate of pen.
 * @return  true if pen controls the "cursor".
 */
static Boolean PenStartCursor(Coord x, Coord y) {
  /* just store the coordinates */
  PenCenterX = x;
  PenCenterY = y;
  PenDeadZone = penMinDeadZone;
  return y < videoScreenHeight;
}

/**
 * Tracks cursor control via pen.
 *
 * @param x x coordinate of pen.
 * @param y y coordinate of pen.
 */
static void PenTrackCursor(Coord x, Coord y) {
  /* track the dead zone */
  Coord distanceX;
  Coord distanceY;

  distanceX = abs(x - PenCenterX);
  distanceY = abs(y - PenCenterY);
  if (distanceX < distanceY) {
    distanceX = distanceY;
  }
  distanceX /= 2;
  if (distanceX > penMinDeadZone) {
    PenDeadZone = distanceX;
  } else {
    PenDeadZone = penMinDeadZone;
  }

  /* mask the corresponding cursor bits */
  PenJoypadCursor = joypadCursorMask;
  if (x + PenDeadZone < PenCenterX) {
    PenJoypadCursor &= ~joypadCursorLeft;
  } else if (x > PenCenterX + PenDeadZone) {
    PenJoypadCursor &= ~joypadCursorRight;
  }
  if (y + PenDeadZone < PenCenterY) {
    PenJoypadCursor &= ~joypadCursorUp;
  } else if (y > PenCenterY + PenDeadZone) {
    PenJoypadCursor &= ~joypadCursorDown;
  }
}

/**
 * Tracks the pen for "pressed" buttons on the screen.
 *
 * @param x x coordinate of pen.
 * @param y y coordinate of pen.
 * @return  true if pen is on a button. Only needed for the start.
 */
static Boolean PenTrackControl(Coord x, Coord y) {
  UInt16 i;

  /* mask the corresponding control bits accumulatively */
  for (i = 0; i < prefsScreenLayoutControlCount; i++) {
    switch (StatesScreenLayoutP->ctl[i]) {
    case offsetEmulationSelect:
      if (RctPtInRectangle(x, y, &SelectBounds)) {
	PenJoypadControl &= ~joypadControlSelect;
	return true;
      }
      break;
    case offsetEmulationStart:
      if (RctPtInRectangle(x, y, &StartBounds)) {
	PenJoypadControl &= ~joypadControlStart;
	return true;
      }
      break;
    case offsetEmulationB:
      if (RctPtInRectangle(x, y, &BBounds)) {
	PenJoypadControl &= ~joypadControlB;
	return true;
      }
      break;
    case offsetEmulationA:
      if (RctPtInRectangle(x, y, &ABounds)) {
	PenJoypadControl &= ~joypadControlA;
	return true;
      }
      break;
    default:
      break;
    }
  }
  return false;
}

/**
 * Finishes tracking the pen.
 *
 * @param x x coordinate of pen.
 * @param y y coordinate of pen.
 */
static void PenFinish(Coord x, Coord y) {
  if (PenState == penStateTitle && y < penTitleHeight) {
    EventType evt;

    MemSet(&evt, sizeof(evt), 0);
    evt.eType = keyDownEvent;
    evt.data.keyDown.chr = vchrMenu;
    evt.data.keyDown.modifiers = commandKeyMask;
    EvtAddEventToQueue(&evt);
  }

  PenJoypadCursor = joypadCursorMask;
  PenJoypadControl = joypadControlMask;
}

/* === Run the emulator ===============================================	*/

/**
 * Sets up everything before emulation.
 *
 * @return true if successful.
 */
Boolean EmulationSetupAll(void) {
  /* try to provide memory for a state */
  if (StatesOpenCurrentState()) {
    /* try to lock and to setup the emulator kernel */
    if (EmulationSetup()) {
      /* try to open the game's database and to setup memory */
      if (MemorySetup()) {
	/* try to setup video */
	if (VideoSetup()) {
	  return true;
	}
	VideoCleanup();
      }
      MemoryCleanup();
    }
    EmulationCleanup();
  }
  (void)StatesCloseCurrentState();
  return false;
}

/**
 * Sets up for a "reset".
 */
static void SetupReset(void) {
  /* yet no speed measured */
  FramesPerSecond[1] = 0;
  FramesPerSecond[0] = 0;
  VideoFrames = 0;

  /* in case a game is continued, save the current CPU state */
  SavedCpuState = EmulationCpuState;

  /* start reset code in GB machine language */
  MemoryBaseRom0 = EmulationEmulator[gbemuGbCodeReset].xt.code;
  EmulationCpuState.pcBase = EmulationEmulator[gbemuGbCodeReset].xt.code;
  EmulationCpuState.pcOffset = 0x0000;
  EmulationCpuState.a = EmulationCpuState.coldReset;
  EmulationCpuState.b = EmulationIo.p1;
  EmulationCpuState.c = EmulationIo.stat;
  EmulationCpuState.d = VideoSavedLcdc;
  EmulationCpuState.e = PrefGetPreference(prefAllowEasterEggs) != 0 ?
    0xff : 0x00;

  /* give enough cycles for not calling any VideoLineFunction too early */
  VideoCycles = 64000;

  /* now the GB reset code will be started, the state must not be saved */
  PrefsPreferences.running = false;
  Running = true;
  ResetRunning = true;
  Crashed = false;
}

/**
 * Sets up before jumping into emulation.
 */
static void SetupGame(void) {
  /* show screen after reset */
  VideoCopyScreen();

  /* Start with video support immediately after restoring PC and SP.
   * Look carefully in gbemu_xt.inc for the count of needed cycles...
   */
  MemoryBaseRom0 = MemoryRomStart;
  if (EmulationCpuState.coldReset != 0) {
    EmulationCpuState.coldReset = 0;
    EmulationCpuState.pcOffset = 0x0100;
  } else {
    EmulationCpuState = SavedCpuState;
  }
  EmulationEmulator[gbemuGbCodeRestart].xt.code[0x01] =
    EmulationCpuState.spOffset & 0xff;
  EmulationEmulator[gbemuGbCodeRestart].xt.code[0x02] =
    EmulationCpuState.spOffset >> 8;
  EmulationEmulator[gbemuGbCodeRestart].xt.code[0x04] =
    EmulationCpuState.pcOffset & 0xff;
  EmulationEmulator[gbemuGbCodeRestart].xt.code[0x05] =
    EmulationCpuState.pcOffset >> 8;
  EmulationCpuState.pcBase =
    EmulationEmulator[gbemuGbCodeRestart].xt.code -
    EmulationCpuState.pcOffset;
  VideoCycles = 2 - 1;
  EmulationEmulator[gbemuJmpLcdLine].xt.vector.address =
    VideoUserPoll;

  /* now the GB game code will be started, the state can be saved */
  PrefsPreferences.running = true;
  Running = !Paused;
  ResetRunning = false;
}

/* === Setup, cleanup, reset, load and save ===========================	*/

/**
 * Sets up the emulation, especially locking the emulator kernel.
 *
 * @return true if successful.
 */
Boolean EmulationSetup(void) {
  /* get and lock the emulator kernel */
  EmulatorH = DmGetResource('code', codeIdKernel);
  if (EmulatorH == NULL) {
    return false;
  }
  EmulationEmulator = (GbemuComboType *)MemHandleLock(EmulatorH) + 0x80;

  /* set vectors */
  MemSemaphoreReserve(true);
  EmulationEmulator[gbemuJmpOpcodeStop].xt.vector.address =
    EmulationOpcodeStop;
  MemSemaphoreRelease(true);

  EmulationJoypad[emulationJoypadBoth] =
    joypadCursorMask & joypadControlMask;
  EmulationJoypad[emulationJoypadCtrl] = joypadControlMask;
  EmulationJoypad[emulationJoypadCurs] = joypadCursorMask;
  EmulationJoypad[emulationJoypadNone] = 0xff;
  EmulationJoypadOld = 0xff;

  return true;
}

/**
 * Cleans up after emulation, unlocks and releases the kernel.
 */
void EmulationCleanup(void) {
  /* release the emulator kernel */
  if (EmulatorH != NULL) {
    MemHandleUnlock(EmulatorH);
    DmReleaseResource(EmulatorH);
  }

  EmulatorH = NULL;
}

/**
 * Resets the saved emulation stuff.
 *
 * @param recP   pointer to the record.
 * @param offset offset into the record.
 */
void EmulationResetState(void *recP, UInt32 offset) {
  EmulationCpuState = *(EmulationCpuStateType *)
    (recP + offset + OffsetOf(EmulationStateType, cpuState));
  EmulationIo = *(EmulationIoType *)
    (recP + offset + OffsetOf(EmulationStateType, io));
  EmulationCpuState.coldReset = 1;
  EmulationIo.sb = 0xff;
  EmulationIo.dma = 0xff;
  EmulationIo.bgp =  0xfc;
  EmulationIo.obp0 = 0xff;
  EmulationIo.obp1 = 0xff;
  /* some games use this flag of the GBC */
  EmulationIo.nonExists4c_7f[0x55 - 0x4c] = 0x80;
  DmWrite(recP, offset + OffsetOf(EmulationStateType, cpuState),
	  &EmulationCpuState, sizeof(EmulationCpuStateType));
  DmWrite(recP, offset + OffsetOf(EmulationStateType, io),
	  &EmulationIo, sizeof(EmulationIoType));

/* already set to false or 0, respectively:
 * misc.paused = false;
 * misc.boosted = false;
 * misc.time = 0;
 * cpuState.ime = 0;
 * io.if_ = 0x00;
 */
}

/**
 * Sets up the emulation stuff to the given state.
 *
 * @param stateP pointer to the emulation state structure.
 */
void EmulationLoadState(const EmulationStateType *stateP) {
  Paused = stateP->misc.paused;
  VideoBoosted = stateP->misc.boosted;
  VideoSetupPaceControl();
  GameTime = stateP->misc.time;

  EmulationCpuState = stateP->cpuState;

  EmulationIo = stateP->io;
}

/**
 * Saves the emulation state in the given record.
 *
 * @param recP   pointer to the record.
 * @param offset offset into the record.
 */
void EmulationSaveState(void *recP, UInt32 offset) {
  EmulationMiscType misc;

  misc.paused = Paused;
  misc.boosted = VideoBoosted;
  misc.time = GameTime;
  DmWrite(recP, offset + OffsetOf(EmulationStateType, misc),
	  &misc, sizeof(EmulationMiscType));

  DmWrite(recP, offset + OffsetOf(EmulationStateType, cpuState),
	  &EmulationCpuState, sizeof(EmulationCpuStateType));

  EmulationIo.p1 = (EmulationJoypadPtr - EmulationJoypad) << 4;
  DmWrite(recP, offset + OffsetOf(EmulationStateType, io),
	  &EmulationIo, sizeof(EmulationIoType));
}

/* === The end ========================================================	*/
