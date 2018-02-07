/************************************************************************
 *  emulation_c.c
 *                      Main emulation routines (C part)
 *
 ************************************************************************
 *
 * Phoinix,
 * Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
 *
 * (c)2000-2004 Bodo Wenzel
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

#include <PalmOS.h>

/* H330 support */
#include <HandEra/Vga.h>

/* GamePad support */
#define UInt8 UInt8 /* they check it with #ifdef :-( */
#include <GPDLib.h>

#include "emulation.h"
#include "Phoinix.h"
#include "prefs.h"
#include "misc.h"
#include "states.h"
#include "video.h"
#include "memory.h"
#include "gbemu.h"

/* === Constants ======================================================	*/

#define codeIdEmulator (2)

#define lineBegX (0)   /* coordinates for separator */
#define lineEndX (159)
#define linePosY (144)

/* Key bits are inverted: released/pressed = 1/0 */
#define jpCursorMask    (0xef) /* bits: 1 1 1 0 down up left right */
#define jpCursorDown    (0x08)
#define jpCursorUp      (0x04)
#define jpCursorLeft    (0x02)
#define jpCursorRight   (0x01)
#define jpControlMask   (0xdf) /* bits: 1 1 0 1 start select b a */
#define jpControlStart  (0x08)
#define jpControlSelect (0x04)
#define jpControlB      (0x02)
#define jpControlA      (0x01)

/* until Palm defines those, here are my private key masks for the TT: */
#define keyBitTTCenter (0x04000000)
#define keyBitTTLeft   (0x01000000)
#define keyBitTTRight  (0x02000000)
#define keyBitTTAudio  (0x00002000)

#define myKeys    (keyBitPageUp | keyBitPageDown | \
                   keyBitHard1 | keyBitHard2 | \
                   keyBitHard3 | keyBitHard4 | \
                   keyBitTTCenter | keyBitTTLeft | keyBitTTRight | \
                   keyBitTTAudio)
#define myKeyMask (keyBitsAll & ~myKeys)

#define showBatteryInfoTimeout (10)

/* === Global and static variables ====================================	*/

static Boolean ShowInfoBattery = false;

EventType EmulationTheEvent;

static FormType    *FormP = NULL;
static ControlType *InfoLabelP;
static UInt16      SelectButtonIndex;
static UInt16      StartButtonIndex;
static Coord       SelectButtonRight, StartButtonLeft,
  BothButtonsLeft, BothButtonsRight,
  BothButtonsTop, BothButtonsBottom;

static UInt16 GamePadLibRefNum;

static Boolean               ColdReset;
static MemHandle             EmulatorH = NULL;
static Boolean               Running = false;
static Boolean               ResetRunning;
static Boolean               GameRunning;
static EmulationCpuStateType SavedCpuState;
static Boolean               SaveState, Crashed;

static UInt32 Seconds;
static UInt32 GameTime;
static UInt16 FramesPerSecond[2];

/* Address of emulator kernel; NOTE: This points to combo 0x00 at offset
 * 0x8000 because of SInt16 indices!
 */
GbemuComboType *EmulationEmulator;

UInt8 EmulationIntIme; /* interrupt master enable */
UInt8 EmulationIntIm;  /* mask = IME & IE */

static UInt32 KeyMask[prefsButtonCount];

UInt8 *EmulationJoypadPtr;
UInt8 EmulationJoypad[4];
UInt8 EmulationJoypadOld; /* saved joypad value before user poll */

UInt8 EmulationError; /* error code from emulator */

/* === Function prototypes ============================================	*/

static void ShowForm(void);
static void ShowInfo(void);

static void SetupKeyMasks(void);

static void SetupReset(void);
static void SetupGame(void);

/* === User interface =================================================	*/

/**
 * Retrieves the next event and stores it in the global variable. While
 * emulating, events are sampled at another function; on return from
 * emulation, the GB is switched if appropriate.
 */
void EmulationGetEvent(void) {
  UInt32 keyOldMask;

  if (!Running) {
    /* emulation is not running,
     * but if battery info is shown, update now and then
     */
    if (ShowInfoBattery) {
      ShowInfo();
      EvtGetEvent(&EmulationTheEvent,
		  showBatteryInfoTimeout * SysTicksPerSecond());
    } else
      EvtGetEvent(&EmulationTheEvent, evtWaitForever);
    return;
  }

  /* if there are still events, handle them all before emulating */
  if (EmulationTheEvent.eType != nilEvent)
    if (EvtEventAvail()) {
      EvtGetEvent(&EmulationTheEvent, evtNoWait);
      return;
    }

  keyOldMask = KeySetMask(myKeyMask);
  MemSemaphoreReserve(true);

  /* *** HERE IT IS! THE EMULATOR STARTS EXECUTION... *****************	*/
  EmulationStartKernel();
  /* ******************************************************************	*/

  if (ResetRunning)
    /* the GB reset code ends with an unknown opcode - normally ;-) */
    if (EmulationError == gbemuRetOpcodeUnknown) {
      /* setup the game now */
      SetupGame();
      Running = GameRunning;
      ResetRunning = false;

      /* prevent stopping */
      EmulationError = gbemuRetUserStop;
      EmulationTheEvent.eType = nilEvent;
    }

  MemSemaphoreRelease(true);
  KeySetMask(keyOldMask);

  /* on emulation errors alert and go back to the manager */
  if (EmulationError != gbemuRetUserStop) {
    FrmAlert(alertIdEmulationRunError);
    FrmGotoForm(formIdManager);

    /* stop the emulation right NOW! */
    Running = false;
    Crashed = true;
    EmulationTheEvent.eType = nilEvent;
  }
}

/**
 * Handles events in the emulation form.
 *
 * @param  evtP pointer to the event structure.
 * @return true if the event is handled.
 */
Boolean EmulationFormHandleEvent(EventType *evtP) {
  ListType      *lstP;
  ControlType   *ctlP;
  RectangleType r;
  Err           err;

  switch (evtP->eType) {

  case frmOpenEvent:
    FormP = FrmGetActiveForm();

    /* remember pointer to info label, for ShowInfo() */
    InfoLabelP = FrmGetObjectPtr(
      FormP, FrmGetObjectIndex(FormP, popupIdEmulationInfo));

    /* setup info popup */
    lstP = FrmGetObjectPtr(
      FormP, FrmGetObjectIndex(FormP, listIdEmulationInfo));
    LstSetSelection(lstP, PrefsPreferences.info);

    /* setup run checkbox */
    ctlP = FrmGetObjectPtr(
      FormP, FrmGetObjectIndex(FormP, checkboxIdEmulationRun));
    CtlSetValue(ctlP, GameRunning ? 1 : 0);

    /* show form, sets draw window */
    ShowForm();

    /* calculate bounds of emulated keys */
    SelectButtonIndex = FrmGetObjectIndex(FormP, buttonIdEmulationSelect);
    FrmGetObjectBounds(FormP, SelectButtonIndex, &r);
    BothButtonsLeft = r.topLeft.x;
    SelectButtonRight = r.topLeft.x + r.extent.x - 1;
    BothButtonsTop = r.topLeft.y;
    BothButtonsBottom = r.topLeft.y + r.extent.y - 1;
    StartButtonIndex = FrmGetObjectIndex(FormP, buttonIdEmulationStart);
    FrmGetObjectBounds(FormP, StartButtonIndex, &r);
    StartButtonLeft = r.topLeft.x;
    BothButtonsRight = r.topLeft.x + r.extent.x - 1;
    if (BothButtonsTop < r.topLeft.y)
      BothButtonsTop = r.topLeft.y;
    if (BothButtonsBottom > r.topLeft.y + r.extent.y - 1)
      BothButtonsBottom = r.topLeft.y + r.extent.y - 1;
    /* needs FrmDrawForm() to set draw window prior */
    WinWindowToDisplayPt(&SelectButtonRight, &BothButtonsTop);
    WinWindowToDisplayPt(&StartButtonLeft, &BothButtonsTop);
    WinWindowToDisplayPt(&BothButtonsLeft, &BothButtonsTop);
    WinWindowToDisplayPt(&BothButtonsRight, &BothButtonsBottom);

    /* setup button mapping */
    SetupKeyMasks();

    /* reserve and open GamePad if present */
    err = SysLibFind(GPD_LIB_NAME, &GamePadLibRefNum);
    if (err == sysErrLibNotFound)
      err = SysLibLoad('libr', GPD_LIB_CREATOR, &GamePadLibRefNum);
    if (err == errNone)
      err = GPDOpen(GamePadLibRefNum);
    if (err != errNone && err != GPDErrGamePadNotPresent)
      GamePadLibRefNum = sysInvalidRefNum;

    /* start the GB reset code */
    SetupReset();
    Running = true;
    ResetRunning = true;
    Crashed = false;

    /* setup for info */
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
    ShowInfoBattery = false;

    /* stop emulation gracefully */
    StatesExitCurrentState(SaveState, Crashed);
    EmulationCleanupAll();

    /* if GamePad was found, close and release it */
    if (GamePadLibRefNum != sysInvalidRefNum) {
      UInt32 numUsers;

      GPDClose(GamePadLibRefNum, &numUsers);
      if (numUsers == 0)
	SysLibRemove(GamePadLibRefNum);
    }
    return false;

  case winExitEvent:
    /* stop the emulation while handling the GUI events */
    if (evtP->data.winExit.exitWindow == (WinHandle)FormP)
      Running = false;
    return false;

  case winEnterEvent:
    /* restart the emulation after handling the GUI events */
    if (evtP->data.winEnter.enterWindow == (WinHandle)FormP)
      Running = (ResetRunning || GameRunning) && !Crashed;
    return false;

  case menuCmdBarOpenEvent:
    /* On OS 3.5 and newer the display of the menu command bar can't be
     * catched, and it's too big, so it has to be disabled.
     */
    return true;

  case popSelectEvent:
    if (evtP->data.popSelect.listID != listIdEmulationInfo)
      return false;

    PrefsPreferences.info = evtP->data.popSelect.selection;
    ShowInfo();
    /* return 'true' to prevent the OS from drawing over */
    return true;

  case ctlSelectEvent:
    if (evtP->data.ctlSelect.controlID != checkboxIdEmulationRun)
      return false;

    GameRunning = evtP->data.ctlSelect.on;
    Running = ResetRunning || GameRunning;
    /* in case the emulation was stopped, correct the seconds */
    Seconds = TimGetSeconds();
    return true;

  case menuEvent:
    switch (evtP->data.menu.itemID) {

    case menuIdEmulationGameQuit:
      SaveState = false;
      FrmGotoForm(formIdManager);
      break;

    case menuIdEmulationGameReset:
      StatesResetCurrentState();

      /* start the GB reset code */
      SetupReset();
      Running = true;
      ResetRunning = true;
      Crashed = false;
      GameRunning = CtlGetValue(FrmGetObjectPtr(
	FormP, FrmGetObjectIndex(FormP, checkboxIdEmulationRun))) != 0;

      /* reflect changes on screen */
      ShowInfo();
      break;

    case menuIdEmulationOptionsButtonMapping:
      PrefsButtonMappingDialog(StatesButtonMappingP);
      SetupKeyMasks();
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

/**
 * Draws the emulation form, on bigger screens with a visible frame for the
 * GB screen.
 */
static void ShowForm(void) {
  /* show form */
  FrmDrawForm(FormP);

  /* on H330, switch screen mode to draw the full rectangle */
  if (VideoIsH330) {
    VgaSetScreenMode(screenMode1To1, rotateModeNone);
    WinSetDrawWindow(WinGetDisplayWindow());
  }
  WinDrawRectangleFrame(simpleFrame, &VideoGbScreenBounds);
  if (VideoIsH330) {
    VgaSetScreenMode(screenModeScaleToFit, rotateModeNone);
    WinSetDrawWindow(WinGetDisplayWindow());
  }
}

/**
 * Draws the info line.
 */
static void ShowInfo(void) {
  UInt16      h, m, si, sf;
  UInt8       b;
  static Char line[8];

  ShowInfoBattery = false;

  switch (PrefsPreferences.info) {

  case emulationInfoTime:
    /* show played time */
    m = (UInt16)(GameTime / 60);
    h = m / 60;
    m %= 60;
    StrPrintF(line, "%u:%02u", h, m);
    break;

  case emulationInfoBattery:
    /* show current battery power */
    ShowInfoBattery = true;

    (void)SysBatteryInfo(false, NULL, NULL, NULL, NULL, NULL, &b);
    StrPrintF(line, "%u%%", b);
    break;

  case emulationInfoSpeed:
    /* show emulation speed */
    sf = (50 * (FramesPerSecond[0] + FramesPerSecond[1])) /
	  videoFramesPerSec;
    si = sf / 100;
    sf %= 100;
    StrPrintF(line, "%u.%02u\xd7", si, sf);
    break;

  default:
    /* this should never happen, just for sure */
    line[0] = '\0';
    break;
  }
  CtlSetLabel(InfoLabelP, line);
}

/* === GUI while emulating ============================================	*/

/**
 * Polls the user interface and the system. When events have to be
 * handled, the emulation is stopped.
 *
 * @return true if the emulation should quit.
 */
Boolean EmulationUserPoll(void) {
  Boolean        getEvent, quit;
  UInt8          jpCursor, jpControl;
  UInt32         keys;
  UInt8          gpKeys;
  UInt32         secs;
  static Boolean selectPressed = false;
  static Boolean startPressed = false;

  /* default: don't quit the emulation... */
  quit = false;

  MemSemaphoreRelease(true);

  /* start with no buttons pressed */
  jpCursor = jpCursorMask;
  jpControl = jpControlMask;

  /* read own hard keys */
  keys = KeyCurrentState();
  /* temporarily a quick-n-dirty hack */
  if (keys & keyBitTTLeft)
    jpCursor &= ~jpCursorLeft;
  if (keys & keyBitTTRight)
    jpCursor &= ~jpCursorRight;
  if (keys & KeyMask[prefsButtonDown])
    jpCursor &= ~jpCursorDown;
  if (keys & KeyMask[prefsButtonUp])
    jpCursor &= ~jpCursorUp;
  if (keys & KeyMask[prefsButtonLeft])
    jpCursor &= ~jpCursorLeft;
  if (keys & KeyMask[prefsButtonRight])
    jpCursor &= ~jpCursorRight;
  if (keys & KeyMask[prefsButtonB])
    jpControl &= ~jpControlB;
  if (keys & KeyMask[prefsButtonA])
    jpControl &= ~jpControlA;

  /* if found, check GamePad's keys */
  if (GamePadLibRefNum != sysInvalidRefNum)
    if (GPDReadInstant(GamePadLibRefNum, &gpKeys) == errNone) {
      if (gpKeys & GAMEPAD_DOWN)
	jpCursor &= ~jpCursorDown;
      if (gpKeys & GAMEPAD_UP)
	jpCursor &= ~jpCursorUp;
      if (gpKeys & GAMEPAD_LEFT)
	jpCursor &= ~jpCursorLeft;
      if (gpKeys & GAMEPAD_RIGHT)
	jpCursor &= ~jpCursorRight;
      if (gpKeys & GAMEPAD_START)
	jpControl &= ~jpControlStart;
      if (gpKeys & GAMEPAD_SELECT)
	jpControl &= ~jpControlSelect;
      if (gpKeys & GAMEPAD_LEFTFIRE)
	jpControl &= ~jpControlB;
      if (gpKeys & GAMEPAD_RIGHTFIRE)
	jpControl &= ~jpControlA;
    }

  /* update info */
  secs = TimGetSeconds();
  if (secs != Seconds) {
    Seconds = secs;

    /* latch current frame rate */
    FramesPerSecond[1] = FramesPerSecond[0];
    FramesPerSecond[0] = VideoFrames;
    VideoFrames = 0;

    /* limit to a maximum of 99 hours, 59 minutes */
    if (++GameTime == 100UL * 60 * 60)
      GameTime = 0;

    ShowInfo();

    /* just to enable system alarms and timeouts... */
    getEvent = true;
  } else {
    getEvent = false;
  }

  if (EvtEventAvail()) {
    getEvent = true;
  }

  if (getEvent) {
    /* on events, mostly quit the emulation */
    EvtGetEvent(&EmulationTheEvent, evtNoWait);
    quit = true;

    switch (EmulationTheEvent.eType) {

    case penDownEvent:
    case penMoveEvent:
      /* follow pen-down on emulated "Select" and "Start" */
      if (EmulationTheEvent.screenX >= BothButtonsLeft &&
	  EmulationTheEvent.screenX <= BothButtonsRight &&
	  EmulationTheEvent.screenY >= BothButtonsTop &&
	  EmulationTheEvent.screenY <= BothButtonsBottom) {
	if (EmulationTheEvent.screenX <= SelectButtonRight)
	  selectPressed = true;
	else if (EmulationTheEvent.screenX >= StartButtonLeft)
	  startPressed = true;
      }
      if (selectPressed || startPressed)
	quit = false;
      break;

    case penUpEvent:
      if (selectPressed || startPressed)
	quit = false;
      selectPressed = false;
      startPressed = false;
      break;

    case keyDownEvent:
      if ((EmulationTheEvent.data.keyDown.modifiers & commandKeyMask) != 0)
	/* problems with multiple keys pressed at the same time on PalmV */
	if (EmulationTheEvent.data.keyDown.chr >= vchrHard1 &&
	    EmulationTheEvent.data.keyDown.chr <= vchrHard4)
	  quit = false;
      break;

    default:
      break;
    }
  }

  /* handle buttons on the screen */
  if (selectPressed)
    jpControl &= ~jpControlSelect;
  if (startPressed)
    jpControl &= ~jpControlStart;
  /* this can be done on each turn, it doesn't cost relevant time! */
  FrmSetControlValue(FormP, SelectButtonIndex,
		     (jpControl & jpControlSelect) == 0);
  FrmSetControlValue(FormP, StartButtonIndex,
		     (jpControl & jpControlStart) == 0);

  MemSemaphoreReserve(true);

  /* copy buttons into emulation */
  EmulationJoypad[emulationJoypadCurs] = jpCursor;
  EmulationJoypad[emulationJoypadCtrl] = jpControl;
  EmulationJoypad[emulationJoypadBoth] = jpCursor & jpControl;

  return quit;
}

/**
 * Sets up all key masks.
 */
static void SetupKeyMasks(void) {
  UInt16 i;

  /* set a bit for each assigned hard key */
  for (i = 0; i < prefsButtonCount; i++) {
    KeyMask[i] = 0;
    if (StatesButtonMappingP[0] == i)
      KeyMask[i] |= keyBitHard1;
    if (StatesButtonMappingP[1] == i)
      KeyMask[i] |= keyBitHard2;
    if (StatesButtonMappingP[2] == i)
      KeyMask[i] |= keyBitPageUp;
    if (StatesButtonMappingP[3] == i)
      KeyMask[i] |= keyBitPageDown;
    if (StatesButtonMappingP[4] == i)
      KeyMask[i] |= keyBitHard3;
    if (StatesButtonMappingP[5] == i)
      KeyMask[i] |= keyBitHard4;
  }
}

/* === Run the emulator ===============================================	*/

/**
 * Sets up everything before emulation.
 *
 * @return true if successful.
 */
Boolean EmulationSetupAll(void) {
  /* make sure that we have the game's cartridge RAM, if we need it */
  if (!StatesSetupPrefsAndStates())
    return false;

  /* try to lock and to setup the emulator kernel */
  if (!EmulationSetup())
    return false;

  /* try to open the game's database and to setup memory */
  if (!MemorySetup())
    return false;

  /* try to setup video */
  if (!VideoSetup())
    return false;

  return true;
}

/**
 * Cleans up everything after emulation.
 */
void EmulationCleanupAll(void) {
  VideoCleanup();
  MemoryCleanup();
  EmulationCleanup();
  StatesCleanupPrefsAndStates();
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
  EmulationCpuState.a = ColdReset;
  EmulationCpuState.b = EmulationIo.p1;
  EmulationCpuState.c = EmulationIo.stat;
  EmulationCpuState.d = VideoSavedLcdc;
  EmulationCpuState.e = PrefGetPreference(prefAllowEasterEggs) != 0 ?
    0xff : 0x00;

  /* give enough cycles for not calling any VideoLineFunction too early */
  VideoCycles = 64000;

  /* now the GB reset code will be started,
   * the state should not be saved on stop
   */
  SaveState = false;
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
  if (ColdReset)
    EmulationCpuState.pcOffset = 0x0100;
  else
    EmulationCpuState = SavedCpuState;
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
  VideoCycles = 2;
  EmulationEmulator[gbemuJmpLcdLine].xt.vector.address =
    VideoUserPoll;

  /* now the GB game code will be started,
   * and the state should saved on stop as default
   */
  SaveState = true;
}

/* === Setup, cleanup, reset, load and save ===========================	*/

/**
 * Sets up the emulation, especially locking the emulator kernel.
 *
 * @return true if successful.
 */
Boolean EmulationSetup(void) {
  /* get and lock the emulator kernel */
  EmulatorH = DmGetResource('code', codeIdEmulator);
  if (EmulatorH == NULL)
    return false;
  EmulationEmulator = (GbemuComboType *)MemHandleLock(EmulatorH) + 0x80;

  /* set vectors */
  MemSemaphoreReserve(true);
  EmulationEmulator[gbemuJmpOpcodeStop].xt.vector.address =
    EmulationOpcodeStop;
  MemSemaphoreRelease(true);

  EmulationJoypad[emulationJoypadBoth] = jpCursorMask & jpControlMask;
  EmulationJoypad[emulationJoypadCtrl] = jpControlMask;
  EmulationJoypad[emulationJoypadCurs] = jpCursorMask;
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
 * Sets up the emulation stuff to the "reset" state.
 */
void EmulationResetState(void) {
  /* start running as default */
  ColdReset = true;
  GameRunning = true;
  GameTime = 0;

  EmulationIntIme = 0;

  EmulationIo.sb = 0xff;
  EmulationIo.dma = 0xff;
  EmulationIo.if_ = 0x00;
}

/**
 * Sets up the emulation stuff to the given state.
 *
 * @param stateP pointer to the emulation state structure.
 */
void EmulationLoadState(EmulationStateType *stateP) {
  ColdReset = false;
  GameRunning = stateP->misc.running;
  GameTime = stateP->misc.time;

  EmulationIntIme = stateP->misc.intIme;

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

  misc.running = GameRunning;
  misc.time = GameTime;
  misc.error = EmulationError;
  misc.intIme = EmulationIntIme;
  DmWrite(recP, offset + OffsetOf(EmulationStateType, misc),
	  &misc, sizeof(EmulationMiscType));

  DmWrite(recP, offset + OffsetOf(EmulationStateType, cpuState),
	  &EmulationCpuState, sizeof(EmulationCpuStateType));

  EmulationIo.p1 = (EmulationJoypadPtr - EmulationJoypad) << 4;
  DmWrite(recP, offset + OffsetOf(EmulationStateType, io),
	  &EmulationIo, sizeof(EmulationIoType));
}

/* === The end ========================================================	*/
