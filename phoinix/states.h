/************************************************************************
 *  states.h
 *                      Emulation states
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

#include "prefs.h"

/* === Constants ======================================================	*/

#define currentStateDbCardNo (0)
#define gameStatesDbName          "Phoinix-%08lX"

/* === Global variables ===============================================	*/

extern UInt8 *StatesCartridgeRamPtr;

/* === Type definitions ===============================================	*/

extern PrefsButtonMappingType *StatesButtonMappingP;

/* === Function prototypes ============================================	*/

Boolean StatesOpenPrefsAndStates(void);
Boolean StatesSetupPrefsAndStates(void);
void StatesCleanupPrefsAndStates(void);
void StatesClosePrefsAndStates(void);
void StatesDeletePrefsAndStates(void);

Boolean StatesAutostartCurrentState(void);
Boolean StatesNewCurrentState(void);
void StatesExitCurrentState(Boolean save, Boolean crash);
void StatesResetCurrentState(void);
void StatesLoadCurrentState(void);

/* === The end ========================================================	*/

#endif
