/************************************************************************
 *  vfs.h
 *                      Virtual file system support
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
 *  $Log: vfs.h,v $
 *  Revision 1.14  2005/05/26 19:35:47  bodowenzel
 *  Moved game's info into game's entry, removing MemoryMbcInfo
 *
 *  Revision 1.13  2005/05/03 08:40:36  bodowenzel
 *  Adding states dirty flag for fewer error messages
 *
 *  Revision 1.12  2005/01/30 19:35:27  bodowenzel
 *  Removed support for VFS mount and unmount
 *
 *  Revision 1.11  2005/01/29 10:25:37  bodowenzel
 *  Added const qualifier to pointer parameters
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
 *  Revision 1.6  2004/04/13 19:47:13  bodowenzel
 *  Last cleanups
 *  Refactoring of deleting states
 *
 *  Revision 1.5  2004/04/05 21:56:55  bodowenzel
 *  VFS review and its consequences
 *
 *  Revision 1.4  2004/01/11 19:05:58  bodowenzel
 *  Start of VFS review and correction
 *
 *  Revision 1.3  2002/12/16 20:39:01  bodowenzel
 *  Switches for showing the busy form
 *  Renaming games on VFS
 *  Better import of games from Launcher directory
 *
 *  Revision 1.2  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.1  2002/05/10 15:17:39  bodowenzel
 *  Till's VFS contribution
 *
 ************************************************************************
 */

#ifndef VFS_H
#define VFS_H

/* === Includes =======================================================	*/

#include "sections.h"
#include "manager.h"
#include "misc.h"

/* === Constants ======================================================	*/

/* Palm RAM is always the first entry in the volumes list */
#define vfsRamVolIndex (0)

#define vfsVolNameLength (256)

/* === Type definitions ===============================================	*/

/* To keep the UI code working the first member has to be the name! */ 
typedef struct {
  Char   name[vfsVolNameLength];
  UInt16 volRefNum;
} VfsVolumeType;

/* === Global variables ===============================================	*/

extern UInt16        VfsNumberOfVolumes;
extern VfsVolumeType **VfsVolume;

/* === Function prototypes ============================================	*/

Boolean VfsInit(void)
  VFS_SECTION;
UInt16 VfsFindVolIndex(const Char *name)
  VFS_SECTION;
void VfsExit(void)
  VFS_SECTION;

MiscErrorType VfsScanForGames(void)
  VFS_SECTION;

MiscErrorType VfsMove(const ManagerDbListEntryType *gameP,
		      UInt16 newVolIndex)
  VFS_SECTION;

MemHandle VfsGetFirstBlock(const ManagerDbListEntryType *gameP)
  VFS_SECTION;
UInt32 VfsTotalSize(const ManagerDbListEntryType *gameP)
  VFS_SECTION;
MiscErrorType VfsRenameGame(const ManagerDbListEntryType *gameP,
			    const Char *name)
  VFS_SECTION;
MiscErrorType VfsDeleteGame(const ManagerDbListEntryType *gameP)
  VFS_SECTION;
void VfsDeleteStates(const ManagerDbListEntryType *gameP)
  VFS_SECTION;

DmOpenRef VfsGameDbOpen(const ManagerDbListEntryType *gameP)
  VFS_SECTION;
void VfsGameDbClose(DmOpenRef dbP)
  VFS_SECTION;

DmOpenRef VfsStatesDbOpen(const ManagerDbListEntryType *gameP)
  VFS_SECTION;
Boolean VfsStatesDbClose(DmOpenRef dbP,
			 const ManagerDbListEntryType *gameP,
			 Boolean save)
  VFS_SECTION;

/* === The end ========================================================	*/

#endif
