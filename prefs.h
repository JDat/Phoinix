/************************************************************************
 *  prefs.h
 *                      Preferences
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
 *  $Log: prefs.h,v $
 *  Revision 1.11  2005/04/03 14:08:34  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.10  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.9  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.8  2004/12/02 20:01:22  bodowenzel
 *  New configurable screen layout
 *
 *  Revision 1.7  2004/10/24 09:11:05  bodowenzel
 *  New button mapping
 *
 *  Revision 1.6  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.5  2004/06/20 14:19:46  bodowenzel
 *  Adjustments for volume names, correct RAM name
 *
 *  Revision 1.4  2004/04/13 19:45:14  bodowenzel
 *  Refactoring, setting .volIndex default
 *
 *  Revision 1.3  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
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

#include "sections.h"
#include "manager.h"
#include "vfs.h"
#include "emulation.h"

/* === Constant =======================================================	*/

#define prefsButtonMappingNoBit (-1)
#define prefsButtonMappingNoChr (0)

#define prefsScreenLayoutControlCount (4)

/* === Type definitions ===============================================	*/

typedef struct {
  Boolean save;
  UInt16  maxFs;
  UInt16  boost;
} PrefsEmulationOptionsType;

typedef struct {
  Int16  bit;
  UInt16 chr;
} PrefsKeyType;

typedef struct {
  PrefsKeyType key[emulationKeyCount];
  Boolean      holdVertical;
  Boolean      holdHorizontal;
} PrefsButtonMappingType;

typedef struct {
  UInt8 ctl[prefsScreenLayoutControlCount];
} PrefsScreenLayoutType;

typedef struct {
  UInt16                    version;
  PrefsEmulationOptionsType emulationOptions;
  PrefsButtonMappingType    buttonMapping;
  PrefsScreenLayoutType     screenLayout;
  Char                      volumeName[vfsVolNameLength];
  ManagerDbListEntryType    game;
  Boolean                   running;
} PrefsPreferencesType;

/* === Global variables ===============================================	*/

extern PrefsPreferencesType PrefsPreferences;

/* === Function prototypes ============================================	*/

void PrefsLoad(void) PREFS_SECTION;
void PrefsSave(void) PREFS_SECTION;

void PrefsEmulationOptionsDialog(
  PrefsEmulationOptionsType *emulationOptionsP)
  PREFS_SECTION;

void PrefsButtonMappingDialog(PrefsButtonMappingType *buttonMappingP)
  PREFS_SECTION;

void PrefsScreenLayoutDialog(PrefsScreenLayoutType *screenLayoutP)
  PREFS_SECTION;

/* === The end ========================================================	*/

#endif
