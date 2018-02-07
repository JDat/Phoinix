/************************************************************************
 *  vfs.h
 *                      Virtual file system support
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

#include "manager.h"

/* === Constants ======================================================	*/

#define VFS_VOLUME_PALM (0xffff)

/* === Type definitions ===============================================	*/

typedef struct {
  char   volName[16];
  UInt16 volId;
} VFSVolumeType;

/* === Global variables ===============================================	*/

extern UInt16        VFSPresent;  /* number of installed file systems */
extern VFSVolumeType **VFSVolume;

/* === Function prototypes ============================================	*/

void      VFSSetup(void);
void      VFSExit(void);
void      VFSMove(ManagerDbListEntryType *src, UInt16 dst);
MemHandle VFSFetchHead(ManagerDbListEntryType *game);
Boolean   VFSScanForGames(DmOpenRef ListDbP);
Boolean   VFSGameLoad(ManagerDbListEntryType *game,
		      UInt16 *card, LocalID *id);
Boolean   VFSGameUnload(void);
Boolean   VFSStateLoad(ManagerDbListEntryType *game,
		       UInt16 *card, LocalID *id);
Boolean   VFSStateUnload(void);
Boolean   VFSRenameGame(ManagerDbListEntryType *game, Char *name);
Boolean   VFSDeleteGame(ManagerDbListEntryType *game);

/* === The end ========================================================	*/

#endif
