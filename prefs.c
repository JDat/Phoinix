/************************************************************************
 *  prefs.c
 *                      Preferences
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
 *  $Log: prefs.c,v $
 *  Revision 1.19  2007/02/11 17:14:24  bodowenzel
 *  New info control for wall clock
 *
 *  Revision 1.18  2006/11/18 16:18:23  bodowenzel
 *  Capture the Back key of the Fossil Wrist PDA
 *
 *  Revision 1.17  2005/05/03 08:40:34  bodowenzel
 *  Adding states dirty flag for fewer error messages
 *
 *  Revision 1.16  2005/04/03 14:08:34  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.15  2005/02/27 16:37:30  bodowenzel
 *  Changed maximum and default values for frameskip
 *
 *  Revision 1.14  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.13  2005/01/29 10:25:37  bodowenzel
 *  Added const qualifier to pointer parameters
 *
 *  Revision 1.12  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.11  2004/12/02 20:01:57  bodowenzel
 *  Utility function MiscGetObjectPointer()
 *  New configurable screen layout
 *
 *  Revision 1.10  2004/10/24 09:11:05  bodowenzel
 *  New button mapping
 *
 *  Revision 1.9  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.8  2004/09/19 12:37:06  bodowenzel
 *  Default info is battery
 *
 *  Revision 1.7  2004/06/20 14:27:30  bodowenzel
 *  Adjustments for volume names, correct RAM name
 *
 *  Revision 1.6  2004/04/13 19:45:14  bodowenzel
 *  Refactoring, setting .volIndex default
 *
 *  Revision 1.5  2002/12/07 08:57:27  bodowenzel
 *  Tweaked button mapping form like proposed by HJ
 *
 *  Revision 1.4  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.3  2002/05/10 15:17:39  bodowenzel
 *  Till's VFS contribution
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:13  bodowenzel
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

#ifdef fossil
	/* Fossil WristPDA support */
	#include <WristPDA.h>
#endif


#include "prefs.h"

#include "misc.h"
#include "states.h"
#include "vfs.h"
#include "emulation.h"

/* === Constants ======================================================	*/

/* The old versions are not converted, as long as only few informations
 * are stored here.
 */
#define prefsVersion (4)
#define prefsId      (0)

#define frameskipMaxMaxFs (20)
#define frameskipMaxBoost (99)

/* Sony Clies use bit 31 for their own purpose - bad! */
#define prefsButtonMappingKeyBitsAll ((UInt32)0x7FFFFFFF)

/* === Global and static variables ====================================	*/

PrefsPreferencesType PrefsPreferences;

static const PrefsEmulationOptionsType EmulationOptionsDefault = {
  false, 3, 10
};

static PrefsEmulationOptionsType EmulationOptionsTemp;

static const PrefsButtonMappingType ButtonMappingDefault = {
  { { 1,                       prefsButtonMappingNoChr },
    { 2,                       prefsButtonMappingNoChr },
    { 4,                       vchrHard2 },
    { 5,                       vchrHard3 },
    { prefsButtonMappingNoBit, prefsButtonMappingNoChr },
    { prefsButtonMappingNoBit, prefsButtonMappingNoChr },
    { 3,                       vchrHard1 },
    { 6,                       vchrHard4 } },
  false, false
};

static PrefsButtonMappingType ButtonMappingTemp;
static PrefsKeyType           ButtonMappingKeySample;

static const PrefsScreenLayoutType ScreenLayoutDefault = {
  {
    offsetEmulationPause,
    offsetEmulationSelect,
    offsetEmulationStart,
    offsetEmulationBattery
  }
};

static PrefsScreenLayoutType ScreenLayoutTemp;

/* === Function prototypes ============================================	*/

static Boolean EmulationOptionsFormHandleEvent(EventType *evtP)
  PREFS_SECTION;
static void EmulationOptionsUpdateFrameskip(
  FormType *frmP, UInt16 value, UInt16 max, UInt16 labelId,
  UInt16 buttonIdUp, UInt16 buttonIdDown)
  PREFS_SECTION;

static Boolean ButtonMappingFormHandleEvent(EventType *evtP)
  PREFS_SECTION;
static void ButtonMappingShow(const FormType *frmP)
  PREFS_SECTION;
static void ButtonMappingSample(UInt16 i)
  PREFS_SECTION;
static Boolean ButtonMappingSampleFormHandleEvent(EventType *evtP)
  PREFS_SECTION;
static Int16 ButtonMappingPatternToBit(UInt32 pattern)
  PREFS_SECTION;

static Boolean ScreenLayoutFormHandleEvent(EventType *evtP)
  PREFS_SECTION;

/* === Load and Save ==================================================	*/

/**
 * Loads the preferences from the system's database. If necessary, set to
 * default values.
 */
void PrefsLoad(void) {
  UInt16 size;

  /* get preferences and check for version and size */
  size = sizeof(PrefsPreferencesType);
  if (PrefGetAppPreferences(miscCreator, prefsId, &PrefsPreferences,
			    &size, true) != noPreferenceFound) {
    if (size == sizeof(PrefsPreferencesType) &&
	PrefsPreferences.version == prefsVersion) {
      PrefsPreferences.game.volIndex =
	VfsFindVolIndex(PrefsPreferences.volumeName);
      return;
    }
  }

  /* on error, set default values */
  PrefsPreferences.version = prefsVersion;
  PrefsPreferences.emulationOptions = EmulationOptionsDefault;
  PrefsPreferences.buttonMapping = ButtonMappingDefault;
  PrefsPreferences.screenLayout = ScreenLayoutDefault;
  PrefsPreferences.volumeName[0] = '\0';
  PrefsPreferences.game.volIndex = vfsRamVolIndex;
  PrefsPreferences.game.name[0] = '\0';
  PrefsPreferences.running = false;
}

/**
 * Saves the preferences into the system's database.
 */
void PrefsSave(void) {
  PrefSetAppPreferences(miscCreator, prefsId, 1, &PrefsPreferences,
			sizeof(PrefsPreferencesType), true);
}

/* === Emulation Options ==============================================	*/

/**
 * Shows the emulation options form.
 *
 * @param emulationP pointer to the emulation options.
 */
void PrefsEmulationOptionsDialog(
  PrefsEmulationOptionsType *emulationOptionsP) {
  FormType *prevFrmP, *frmP;

  /* prepare new form */
  prevFrmP = FrmGetActiveForm();
  frmP = FrmInitForm(formIdEmulationOptions);
  FrmSetActiveForm(frmP);
  FrmSetEventHandler(frmP, EmulationOptionsFormHandleEvent);
  EmulationOptionsTemp = *emulationOptionsP;
  FrmSetControlValue(
    frmP, FrmGetObjectIndex(frmP, checkboxIdEmulationOptionsSaveState),
    EmulationOptionsTemp.save ? 1 : 0);
  EmulationOptionsUpdateFrameskip(
    frmP, EmulationOptionsTemp.maxFs, frameskipMaxMaxFs,
    labelIdEmulationOptionsMaxFs, buttonIdEmulationOptionsMaxFsUp,
    buttonIdEmulationOptionsMaxFsDown);
  EmulationOptionsUpdateFrameskip(
    frmP, EmulationOptionsTemp.boost, frameskipMaxBoost,
    labelIdEmulationOptionsBoost, buttonIdEmulationOptionsBoostUp,
    buttonIdEmulationOptionsBoostDown);

  /* show form, on "OK" save new mapping locally and globally */
  if (FrmDoDialog(frmP) == buttonIdEmulationOptionsOk) {
    EmulationOptionsTemp.save = FrmGetControlValue(
      frmP, FrmGetObjectIndex(
	frmP, checkboxIdEmulationOptionsSaveState)) != 0;
    *emulationOptionsP = EmulationOptionsTemp;
    PrefsPreferences.emulationOptions = EmulationOptionsTemp;
    StatesSetPrefsAndStatesDirty();
  }

  /* cleanup and return */
  if (prevFrmP != NULL) {
    FrmSetActiveForm(prevFrmP);
  }
  FrmDeleteForm(frmP);
}

/**
 * Handles events in the emulation options form.
 *
 * @param evtP pointer to the event structure.
 * @return     true if the event is handled.
 */
static Boolean EmulationOptionsFormHandleEvent(EventType *evtP) {
  FormType *frmP;

  frmP = FrmGetActiveForm();

  switch (evtP->eType) {

  case ctlRepeatEvent:
    switch (evtP->data.ctlRepeat.controlID) {
    case buttonIdEmulationOptionsMaxFsUp:
      if (EmulationOptionsTemp.maxFs < frameskipMaxMaxFs) {
	EmulationOptionsTemp.maxFs++;
	EmulationOptionsUpdateFrameskip(
	  frmP, EmulationOptionsTemp.maxFs, frameskipMaxMaxFs,
	  labelIdEmulationOptionsMaxFs, buttonIdEmulationOptionsMaxFsUp,
	  buttonIdEmulationOptionsMaxFsDown);
      }
      break;
    case buttonIdEmulationOptionsMaxFsDown:
      if (EmulationOptionsTemp.maxFs > 0) {
	EmulationOptionsTemp.maxFs--;
	EmulationOptionsUpdateFrameskip(
	  frmP, EmulationOptionsTemp.maxFs, frameskipMaxMaxFs,
	  labelIdEmulationOptionsMaxFs, buttonIdEmulationOptionsMaxFsUp,
	  buttonIdEmulationOptionsMaxFsDown);
      }
      break;
    case buttonIdEmulationOptionsBoostUp:
      if (EmulationOptionsTemp.boost < frameskipMaxBoost) {
	EmulationOptionsTemp.boost++;
	EmulationOptionsUpdateFrameskip(
	  frmP, EmulationOptionsTemp.boost, frameskipMaxBoost,
	  labelIdEmulationOptionsBoost, buttonIdEmulationOptionsBoostUp,
	  buttonIdEmulationOptionsBoostDown);
      }
      break;
    case buttonIdEmulationOptionsBoostDown:
      if (EmulationOptionsTemp.boost > 0) {
	EmulationOptionsTemp.boost--;
	EmulationOptionsUpdateFrameskip(
	  frmP, EmulationOptionsTemp.boost, frameskipMaxBoost,
	  labelIdEmulationOptionsBoost, buttonIdEmulationOptionsBoostUp,
	  buttonIdEmulationOptionsBoostDown);
      }
      break;
    default:
      break;
    }
    return false;

  default:
    return false;
  }
}

/**
 * Updates the frameskip on the emulation options form.
 *
 * @param frmP         pointer to the form.
 * @param value        value of the frameskip.
 * @param max          maximum value of the frameskip.
 * @param labelId      ID of the frameskip label to show the value.
 * @param buttonIdUp   ID of the increasing button.
 * @param buttonIdDown ID of the decreasing button.
 */
static void EmulationOptionsUpdateFrameskip(
  FormType *frmP, UInt16 value, UInt16 max, UInt16 labelId,
  UInt16 buttonIdUp, UInt16 buttonIdDown) {
  Char   line[4];
  UInt16 ctlIndex;

  StrPrintF(line, "%u", value);
  ctlIndex = FrmGetObjectIndex(frmP, labelId);
  FrmHideObject(frmP, ctlIndex);
  FrmCopyLabel(frmP, labelId, line);
  FrmShowObject(frmP, ctlIndex);
  FrmUpdateScrollers(frmP, FrmGetObjectIndex(frmP, buttonIdUp),
		     FrmGetObjectIndex(frmP, buttonIdDown),
		     value < max, value > 0);
}

/* === Button mapping =================================================	*/

/**
 * Shows the button mapping form.
 *
 * @param buttonMappingP pointer to the button mapping.
 */
void PrefsButtonMappingDialog(PrefsButtonMappingType *buttonMappingP) {
  FormType *prevFrmP, *frmP;

  /* prepare new form */
  prevFrmP = FrmGetActiveForm();
  frmP = FrmInitForm(formIdButtonMapping);
  FrmSetActiveForm(frmP);
  FrmSetEventHandler(frmP, ButtonMappingFormHandleEvent);
  ButtonMappingTemp = *buttonMappingP;
  FrmSetControlValue(
    frmP, FrmGetObjectIndex(frmP, checkboxIdButtonMappingHoldVertical),
    ButtonMappingTemp.holdVertical ? 1 : 0);
  FrmSetControlValue(
    frmP, FrmGetObjectIndex(frmP, checkboxIdButtonMappingHoldHorizontal),
    ButtonMappingTemp.holdHorizontal ? 1 : 0);
  ButtonMappingShow(frmP);

  /* show form, on "OK" save new mapping locally and globally */
  if (FrmDoDialog(frmP) == buttonIdButtonMappingOk) {
    ButtonMappingTemp.holdVertical = FrmGetControlValue(
      frmP, FrmGetObjectIndex(
	frmP, checkboxIdButtonMappingHoldVertical)) != 0;
    ButtonMappingTemp.holdHorizontal = FrmGetControlValue(
      frmP, FrmGetObjectIndex(
	frmP, checkboxIdButtonMappingHoldHorizontal)) != 0;
    *buttonMappingP = ButtonMappingTemp;
    PrefsPreferences.buttonMapping = ButtonMappingTemp;
    StatesSetPrefsAndStatesDirty();
  }

  /* cleanup and return */
  if (prevFrmP != NULL) {
    FrmSetActiveForm(prevFrmP);
  }
  FrmDeleteForm(frmP);
}

/**
 * Handles events in the button mapping form.
 *
 * @param evtP pointer to the event structure.
 * @return     true if the event is handled.
 */
static Boolean ButtonMappingFormHandleEvent(EventType *evtP) {
  UInt16 i;

  switch (evtP->eType) {

  case ctlSelectEvent:
    i = evtP->data.ctlSelect.controlID - selIdButtonMapping;
    if (i < emulationKeyCount) {
      ButtonMappingSample(i);
      ButtonMappingShow(FrmGetActiveForm());
    }
    return false;

  default:
    return false;
  }
}

/**
 * Shows the current mapping (bit numbers) on the form.
 *
 * @param formP pointer to the form.
 */
static void ButtonMappingShow(const FormType *frmP) {
  static Char label[emulationKeyCount][6];
  UInt16      i;

  for (i = 0; i < emulationKeyCount; i++) {
    ControlType *ctlP;

    ctlP = MiscGetObjectPtr(frmP, selIdButtonMapping + i);
    if (ButtonMappingTemp.key[i].bit != prefsButtonMappingNoBit) {
      StrPrintF(label[i], "%d", ButtonMappingTemp.key[i].bit);
      CtlSetLabel(ctlP, label[i]);
    } else {
      CtlSetLabel(ctlP, "-");
    }
  }
}

/**
 * Shows the button sampling form.
 *
 * @param i index of button to be mapped.
 */
static void ButtonMappingSample(UInt16 i) {
  FormType *prevFrmP, *frmP;

  /* no key sampled yet */
  ButtonMappingKeySample.bit = prefsButtonMappingNoBit;
  ButtonMappingKeySample.chr = prefsButtonMappingNoChr;

  /* prepare new form */
  prevFrmP = FrmGetActiveForm();
  frmP = FrmInitForm(formIdButtonMappingSample);
  FrmSetActiveForm(frmP);
  FrmSetEventHandler(frmP, ButtonMappingSampleFormHandleEvent);

  /* show form */
  if (FrmDoDialog(frmP) == buttonIdButtonMappingSampleOk) {
    /* store sampled key */
    ButtonMappingTemp.key[i] = ButtonMappingKeySample;
  }

  /* cleanup and return */
  FrmSetActiveForm(prevFrmP);
  FrmDeleteForm(frmP);
}

/**
 * Handles events in the button sampling form. If a usable key is pressed,
 * its bit number is saved, and also the character code in case it's an
 * application switching key.
 *
 * @param evtP pointer to the event structure.
 * @return     true if the event is handled.
 */
static Boolean ButtonMappingSampleFormHandleEvent(EventType *evtP) {
  switch (evtP->eType) {

  case keyDownEvent:
    ButtonMappingKeySample.bit = ButtonMappingPatternToBit(
      KeyCurrentState());

    if (evtP->data.keyDown.chr == vchrResetAutoOff ||
	(evtP->data.keyDown.modifiers & commandKeyMask) == 0 ||
	(evtP->data.keyDown.modifiers & poweredOnKeyMask) != 0) {
      /* react as usual when switched on */
      return false;
    }

#ifdef fossil
    /* special handling of the Back key of the Wrist PDA */
    if (evtP->data.keyDown.chr == vchrThumbWheelBack &&
	MiscDevice == miscDeviceWrist) {
      /* Because we catch the Back key here, there's no need to change the
       * Back key mode.
       */
      ButtonMappingKeySample.bit = ButtonMappingPatternToBit(keyBitBack);
    }
#endif

    if (ButtonMappingKeySample.bit != prefsButtonMappingNoBit) {
      FormType  *frmP;
      EventType evt = { 0 };

      /* if application switching key, store it for filtering */
      if ((evtP->data.keyDown.chr >= vchrHardKeyMin &&
	   evtP->data.keyDown.chr <= vchrHardKeyMax) ||
	  /* special trap for the Wrist PDA's Back key */
	  evtP->data.keyDown.chr == vchrThumbWheelBack) {
	ButtonMappingKeySample.chr = evtP->data.keyDown.chr;
      }

      /* quit the form by sending itself an event */
      frmP = FrmGetActiveForm();
      evt.eType = ctlSelectEvent;
      evt.data.ctlSelect.controlID = buttonIdButtonMappingSampleOk;
      evt.data.ctlSelect.pControl =
	MiscGetObjectPtr(frmP, buttonIdButtonMappingSampleOk);
      EvtAddEventToQueue(&evt);
      return true;
    }

    return false;

  default:
    return false;
  }
}

/**
 * Scans the key pattern for a set key bit.
 *
 * @param pattern key pattern to be scanned.
 * @return        bit number found, prefsButtonMappingNoBit if none.
 */
static Int16 ButtonMappingPatternToBit(UInt32 pattern) {
  Int16  bit;
  UInt32 mask;

  pattern &= prefsButtonMappingKeyBitsAll;
  for (bit = 0, mask = 1; mask != 0; bit++, mask <<= 1) {
    if ((pattern & mask) != 0) {
      return bit;
    }
  }
  return prefsButtonMappingNoBit;
}

/* === Screen Layout ==================================================	*/

/**
 * Shows the screen layout form.
 *
 * @param screenLayoutP pointer to the screen layout.
 */
void PrefsScreenLayoutDialog(PrefsScreenLayoutType *screenLayoutP) {
  FormType *prevFrmP, *frmP;
  UInt16   i;
  ListType *lstP;

  /* prepare new form */
  prevFrmP = FrmGetActiveForm();
  frmP = FrmInitForm(formIdScreenLayout);
  FrmSetActiveForm(frmP);
  FrmSetEventHandler(frmP, ScreenLayoutFormHandleEvent);
  lstP = MiscGetObjectPtr(frmP, listIdScreenLayout);
  for (i = 0; i < prefsScreenLayoutControlCount; i++) {
    CtlSetLabel(MiscGetObjectPtr(frmP, popupIdScreenLayout + i),
		LstGetSelectionText(lstP, screenLayoutP->ctl[i]));
  }
  ScreenLayoutTemp = *screenLayoutP;

  /* show form, on "OK" save new mapping locally and globally */
  if (FrmDoDialog(frmP) == buttonIdScreenLayoutOk) {
    *screenLayoutP = ScreenLayoutTemp;
    PrefsPreferences.screenLayout = ScreenLayoutTemp;
    StatesSetPrefsAndStatesDirty();
  }

  /* cleanup and return */
  if (prevFrmP != NULL) {
    FrmSetActiveForm(prevFrmP);
  }
  FrmDeleteForm(frmP);
}

/**
 * Handles events in the screen layout form.
 *
 * @param evtP pointer to the event structure.
 * @return     true if the event is handled.
 */
static Boolean ScreenLayoutFormHandleEvent(EventType *evtP) {
  static UInt16 control;
  FormType      *frmP;
  UInt16        lstIndex;
  Coord         x, y;
  UInt16        selection, i;
  ListType      *lstP;

  frmP = FrmGetActiveForm();

  switch (evtP->eType) {

  case ctlEnterEvent:
    /* save index of the control */
    control = evtP->data.ctlEnter.controlID - popupIdScreenLayout;
    /* position the popup list and set the selection */
    lstIndex = FrmGetObjectIndex(frmP, listIdScreenLayout);
    FrmGetObjectPosition(frmP, FrmGetObjectIndex(
			   frmP, evtP->data.ctlEnter.controlID), &x, &y);
    FrmSetObjectPosition(frmP, lstIndex, x + listScreenLayoutOffset, y);
    LstSetSelection(MiscGetObjectPtr(frmP, listIdScreenLayout),
		    ScreenLayoutTemp.ctl[control]);
    return false;

  case popSelectEvent:
    /* save selection */
    lstP = MiscGetObjectPtr(frmP, listIdScreenLayout);
    selection = LstGetSelection(lstP);
    ScreenLayoutTemp.ctl[control] = selection;
    /* avoid duplicates */
    for (i = 0; i < prefsScreenLayoutControlCount; i++) {
      if (i != control && selection == ScreenLayoutTemp.ctl[i]) {
	ScreenLayoutTemp.ctl[i] = offsetEmulationNone;
	CtlSetLabel(MiscGetObjectPtr(frmP, popupIdScreenLayout + i),
		    LstGetSelectionText(lstP, offsetEmulationNone));
      }
    }
    return false;

  default:
    return false;
  }
}

/* === The end ========================================================	*/
