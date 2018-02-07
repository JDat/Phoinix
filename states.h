/************************************************************************
 *  states.h
 *                      Emulation states
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
 *  $Log: states.h,v $
 *  Revision 1.14  2007/02/25 13:15:32  bodowenzel
 *  Show last state in Load State while emulating
 *
 *  Revision 1.13  2007/02/11 17:14:44  bodowenzel
 *  Multiple saved state feature implemented
 *
 *  Revision 1.12  2005/05/03 08:40:34  bodowenzel
 *  Adding states dirty flag for fewer error messages
 *
 *  Revision 1.11  2005/04/03 14:08:34  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.10  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.9  2005/01/30 19:35:27  bodowenzel
 *  Removed support for VFS mount and unmount
 *
 *  Revision 1.8  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.7  2004/12/02 20:01:22  bodowenzel
 *  New configurable screen layout
 *
 *  Revision 1.6  2004/06/06 09:25:01  bodowenzel
 *  Change game's location to RAM on error saving states
 *
 *  Revision 1.5  2004/01/11 19:10:00  bodowenzel
 *  Correct names for constants
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
 *  Revision 1.1.1.1  2001/12/16 13:38:11  bodowenzel
 *  Import
 *
 ************************************************************************
 */

#ifndef STATES_H
#define STATES_H

/* === Includes =======================================================	*/

#include "sections.h"
#include "misc.h"
#include "prefs.h"

/* === Constants ======================================================	*/

#define statesDbCardNo     (0)
#define statesDbNameFormat "Phoinix-%08lX"

#define statesLastState (-2)

/* === Global variables ===============================================	*/

extern UInt8 *StatesCartridgeRamPtr;

extern Boolean StatesPrefsAndStatesDirty;

/* === Type definitions ===============================================	*/

extern PrefsEmulationOptionsType *StatesEmulationOptionsP;

extern PrefsButtonMappingType *StatesButtonMappingP;

extern PrefsScreenLayoutType *StatesScreenLayoutP;

/* === Function prototypes ============================================	*/

Boolean StatesOpenPrefsAndStates(void)
  STATES_SECTION;
Boolean StatesSetupPrefsAndStates(void)
  STATES_SECTION;
#define StatesSetPrefsAndStatesDirty() do { \
  StatesPrefsAndStatesDirty = true;         \
} while (false)
Boolean StatesClosePrefsAndStates(void)
  STATES_SECTION;
void StatesClosePrefsAndStatesWithShow(
  void (*ErrorFunction)(MiscErrorType mainIndex,
			MiscErrorType detailIndex,
			const Char *parameter), Boolean keepLocation)
  STATES_SECTION;
void StatesDeletePrefsAndStates(void)
  STATES_SECTION;

void StatesSetupList(FormType *frmP, ListType *lstP, Int16 index)
  STATES_SECTION;
void StatesDrawGoButton(FormType *frmP, ListType *lstP, UInt16 buttonId,
			UInt16 labelId)
  STATES_SECTION;

Boolean StatesAutostart(void)
  STATES_SECTION;
Boolean StatesOpenCurrentState(void)
  STATES_SECTION;
Boolean StatesCloseCurrentState(void)
  STATES_SECTION;
void StatesResetCurrentState(void)
  STATES_SECTION;
void StatesLoadCurrentState(void)
  STATES_SECTION;
void StatesSaveCurrentState(void)
  STATES_SECTION;
void StatesSaveCurrentStateAsCrashed(void)
  STATES_SECTION;
Boolean StatesCreateStateFromCurrentState(void)
  STATES_SECTION;
Boolean StatesCopyStateToCurrentState(Int16 index)
  STATES_SECTION;
void StatesDeleteState(Int16 index)
  STATES_SECTION;

/* === The end ========================================================	*/

#endif
