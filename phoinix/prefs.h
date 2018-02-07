/************************************************************************
 *  prefs.h
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
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:39:10  bodowenzel
 *  Import
 *
 ************************************************************************
 */

#ifndef PREFS_H
#define PREFS_H

/* === Includes =======================================================	*/

#include "manager.h"
#include "emulation.h"

/* === Type definitions ===============================================	*/

typedef enum {
  prefsButtonLeft = 0, prefsButtonRight, prefsButtonUp, prefsButtonDown,
  prefsButtonB, prefsButtonA, prefsButtonCount
} PrefsButtonMappingType;

typedef struct {
  UInt16                 version;
  PrefsButtonMappingType buttonMapping[prefsButtonCount];
  ManagerDbListEntryType game;
  EmulationInfoType      info;
} PrefsPreferencesType;

/* === Global variables ===============================================	*/

extern PrefsPreferencesType PrefsPreferences;

/* === Function prototypes ============================================	*/

void PrefsLoad(void);
void PrefsSave(void);
void PrefsButtonMappingDialog(PrefsButtonMappingType *buttonMapping);

/* === The end ========================================================	*/

#endif
