/************************************************************************
 *  manager.h
 *                      Game manager
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
 *  $Log: manager.h,v $
 *  Revision 1.12  2005/05/26 19:35:47  bodowenzel
 *  Moved game's info into game's entry, removing MemoryMbcInfo
 *
 *  Revision 1.11  2005/01/30 19:35:27  bodowenzel
 *  Removed support for VFS mount and unmount
 *
 *  Revision 1.10  2005/01/28 17:35:13  bodowenzel
 *  Manager form uses a scrollable table now for the list of games
 *
 *  Revision 1.9  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.8  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.7  2004/06/20 14:20:45  bodowenzel
 *  Adjustments for volume names, correct RAM name
 *  VFS mount and unmount support
 *
 *  Revision 1.6  2004/04/13 19:43:25  bodowenzel
 *  Refactoring, nothing else
 *
 *  Revision 1.5  2004/01/11 19:10:56  bodowenzel
 *  Added module 'ram' for on-board memory
 *  Start of VFS review and correction
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
 *  Revision 1.1.1.1  2001/12/16 13:39:04  bodowenzel
 *  Import
 *
 ************************************************************************
 */

#ifndef MANAGER_H
#define MANAGER_H

/* === Includes =======================================================	*/

#include "sections.h"
#include "memory.h"

/* === Global and static variables ====================================	*/

typedef struct ManagerDbListEntryStruct {
  Char          name[dmDBNameLength];
  UInt16        volIndex;
  MemoryMbcType mbc;
  UInt8         pad; /* currently unused */
  UInt32        crc;
  UInt16        romSize; /* in KB */
  UInt16        ramSize; /* in bytes */
} ManagerDbListEntryType;

/* === Function prototypes ============================================	*/

Boolean ManagerListAdd(const Char *name, UInt16 volIndex)
  MANAGER_SECTION;
void ManagerExit(void)
  MANAGER_SECTION;

Boolean ManagerFormHandleEvent(EventType *evtP)
  MANAGER_SECTION;

void ManagerListSetLocation(UInt16 volIndex)
  MANAGER_SECTION;

/* === The end ========================================================	*/

#endif
