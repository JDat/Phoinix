/************************************************************************
 *  ram.h
 *                      Palm RAM support
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
 *  $Log: ram.h,v $
 *  Revision 1.7  2005/05/26 19:35:47  bodowenzel
 *  Moved game's info into game's entry, removing MemoryMbcInfo
 *
 *  Revision 1.6  2005/01/28 17:35:13  bodowenzel
 *  Manager form uses a scrollable table now for the list of games
 *
 *  Revision 1.5  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.4  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.3  2004/04/13 19:46:07  bodowenzel
 *  Refactoring of deleting states
 *
 *  Revision 1.2  2004/04/05 21:56:55  bodowenzel
 *  VFS review and its consequences
 *
 *  Revision 1.1  2004/01/11 19:03:49  bodowenzel
 *  Added module 'ram' for on-board memory
 *
 ************************************************************************
 */

#ifndef RAM_H
#define RAM_H

/* === Includes =======================================================	*/

#include "sections.h"
#include "manager.h"
#include "misc.h"

/* === Constants ======================================================	*/

#define ramGameDbCardNo   (0)
#define ramStatesDbCardNo (0)

/* === Function prototypes ============================================	*/

MiscErrorType RamScanForGames(void)
  RAM_SECTION;

MemHandle RamGetFirstBlock(const ManagerDbListEntryType *gameP)
  RAM_SECTION;
UInt32 RamTotalSize(const ManagerDbListEntryType *gameP)
  RAM_SECTION;
MiscErrorType RamRenameGame(const ManagerDbListEntryType *gameP,
			    const Char *name)
  RAM_SECTION;
MiscErrorType RamDeleteGame(const ManagerDbListEntryType *gameP)
  RAM_SECTION;
void RamDeleteStates(DmOpenRef dbP)
  RAM_SECTION;

DmOpenRef RamGameDbOpen(const ManagerDbListEntryType *gameP)
  RAM_SECTION;
void RamGameDbClose(DmOpenRef dbP)
  RAM_SECTION;

DmOpenRef RamStatesDbOpen(const ManagerDbListEntryType *gameP)
  RAM_SECTION;
void RamStatesDbClose(DmOpenRef dbP)
  RAM_SECTION;

/* === The end ========================================================	*/

#endif
