/************************************************************************
 *  prefs.c
 *                      Preferences
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

#include "prefs.h"
#include "Phoinix.h"
#include "misc.h"
#include "emulation.h"

/* === Constants ======================================================	*/

#define prefsVersion (2)
#define prefsId      (0)

/* === Global and static variables ====================================	*/

/* please make sure that these texts match the enum definition! */
static Char *buttonLabels[prefsButtonCount] = {
  "\x1", "\x2", "\x3", "\x4", "\x5", "\x6" };

PrefsPreferencesType PrefsPreferences;

/* === Load and Save ==================================================	*/

/**
 * Loads the preferences from the system's database. If necessary, set to
 * default values.
 */
void PrefsLoad(void) {
  UInt16 size, i;

  /* get preferences and check for version and size */
  size = sizeof(PrefsPreferencesType);
  if (PrefGetAppPreferences(miscCreator, prefsId, &PrefsPreferences,
			    &size, true) != noPreferenceFound)
    if (size == sizeof(PrefsPreferencesType) &&
	PrefsPreferences.version == prefsVersion)
      return;

  /* on error, set default values */
  PrefsPreferences.version = prefsVersion;
  for (i = 0; i < prefsButtonCount; i++)
    PrefsPreferences.buttonMapping[i] = i;
  PrefsPreferences.game.cardNo = 0;
  PrefsPreferences.game.name[0] = '\0';
  PrefsPreferences.info = emulationInfoTime;
}

/**
 * Saves the preferences into the system's database.
 */
void PrefsSave(void) {
  PrefSetAppPreferences(miscCreator, prefsId, 1, &PrefsPreferences,
			sizeof(PrefsPreferencesType), true);
}

/* === Form for button mapping ========================================	*/

/**
 * Shows the button mapping form.
 *
 * @param buttonMapping pointer to the button mapping array.
 */
void PrefsButtonMappingDialog(PrefsButtonMappingType *buttonMapping) {
  MemHandle fntH;
  FormType  *prevFrmP, *frmP;
  ListType  *lstP;
  UInt16    i;
  UInt16    listId, popupId;

  /* define custom font for the buttons */
  fntH = DmGetResource('NFNT', fontIdButtonMapping);
  FntDefineFont(fontButtonMapping, (FontType *)MemHandleLock(fntH));

  /* prepare new form */
  prevFrmP = FrmGetActiveForm();
  frmP = FrmInitForm(formIdButtonMapping);
  FrmSetActiveForm(frmP);
  listId = listIdButtonMappingDb;
  popupId = popupIdButtonMappingDb;
  for (i = 0; i < prefsButtonCount; i++) {
    lstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, listId));
    LstSetListChoices(lstP, buttonLabels, prefsButtonCount);
    LstSetHeight(lstP, prefsButtonCount);
    LstSetSelection(lstP, buttonMapping[i]);
    CtlSetLabel(FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, popupId)),
		LstGetSelectionText(lstP, buttonMapping[i]));
    listId += 10;
    popupId += 10;
  }

  /* show form, on "OK" save new mapping locally and globally */
  if (FrmDoDialog(frmP) == buttonIdButtonMappingOk) {
    listId = listIdButtonMappingDb;
    popupId = popupIdButtonMappingDb;
    for (i = 0; i < prefsButtonCount; i++) {
      lstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, listId));
      buttonMapping[i] = LstGetSelection(lstP);
      PrefsPreferences.buttonMapping[i] = buttonMapping[i];
      listId += 10;
      popupId += 10;
    }
  }

  /* cleanup and return */
  if (prevFrmP != NULL)
    FrmSetActiveForm(prevFrmP);
  FrmDeleteForm(frmP);

  /* release font resource */
  MemHandleUnlock(fntH);
  DmReleaseResource(fntH);
}

/* === The end ========================================================	*/
