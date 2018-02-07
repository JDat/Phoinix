/************************************************************************
 *  misc.h
 *                      Miscellaneous functions
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
 *  $Log: misc.h,v $
 *  Revision 1.20  2007/02/11 17:14:43  bodowenzel
 *  Multiple saved state feature implemented
 *
 *  Revision 1.19  2005/10/11 14:48:05  bodowenzel
 *  Centralized device type into MiscDevice
 *
 *  Revision 1.18  2005/05/03 08:41:13  bodowenzel
 *  Moved SetBackupBit here from states.c
 *
 *  Revision 1.17  2005/01/30 19:35:27  bodowenzel
 *  Removed support for VFS mount and unmount
 *
 *  Revision 1.16  2005/01/29 10:25:37  bodowenzel
 *  Added const qualifier to pointer parameters
 *
 *  Revision 1.15  2005/01/28 17:35:13  bodowenzel
 *  Manager form uses a scrollable table now for the list of games
 *
 *  Revision 1.14  2004/12/28 13:58:40  bodowenzel
 *  Split up all C-Code to multi-segmented
 *  Support for devices without direct screen access
 *
 *  Revision 1.13  2004/12/02 20:02:16  bodowenzel
 *  Utility function MiscGetObjectPointer()
 *
 *  Revision 1.12  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.11  2004/09/19 12:35:54  bodowenzel
 *  Enhanced error reporting with string parameter
 *
 *  Revision 1.10  2004/06/20 14:23:20  bodowenzel
 *  VFS mount and unmount support
 *
 *  Revision 1.9  2004/06/11 16:18:30  bodowenzel
 *  Review and cleanup of miscErr* constants and texts
 *
 *  Revision 1.8  2004/03/02 19:24:16  bodowenzel
 *  Changed to new error function MiscShowError()
 *
 *  Revision 1.7  2004/01/11 19:09:13  bodowenzel
 *  Moved PDB type here
 *
 *  Revision 1.6  2003/04/19 13:16:45  bodowenzel
 *  New variable MiscIsOs5 indicates OS5
 *
 *  Revision 1.5  2002/11/02 15:53:41  bodowenzel
 *  Check for OS 5 added
 *
 *  Revision 1.4  2002/10/23 16:34:35  bodowenzel
 *  Busy form centralized
 *
 *  Revision 1.3  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:10  bodowenzel
 *  Import
 *
 ************************************************************************
 */

#ifndef MISC_H
#define MISC_H

/* === Includes =======================================================	*/

#include "sections.h"

/* === Constants ======================================================	*/

#define miscCreator    'Phnx'
#define miscGameDbType 'ROMU'

#define miscErrorStringMaxLength (100)

/* Actually these are indices into a string table.
 * Check carefully against the texts in the resources!
 */
typedef enum {
  miscErrNone = 0,
  /* actions when errors occur: */
  miscErrInitialization,
  miscErrResumeGame,
  miscErrBuildingList,
  miscErrVfs,
  miscErrOs,
  miscErrEmulator,
  miscErrDeleting,
  miscErrRenaming,
  miscErrMoving,
  /* errors that occur: */
  miscErrMemoryFull,
  miscErrScreen,
  miscErrSystemVersion,
  miscErrImport,
  miscErrSearchVfs,
  miscErrStateSave,
  miscErrStateLoad,
  miscErrSaveLocationChanged,
  miscErrRuntime,
  miscErrCantRun,
  miscErrFileNotFound,
  miscErrNoPhoinixDir,
  miscErrBadName,
  miscErrRamReadOnly,
  miscErrVfsReadOnly,
  miscErrAlreadyExists,
  miscErrWrite,
  miscErrRead,
  miscErrNewState,
  miscErrVolumeFull,
} MiscErrorType;

typedef enum {
  miscDeviceStandard = 0,
  miscDeviceH330,
  miscDeviceDana,
  miscDeviceWrist,
} MiscDeviceType;

/* === Global variables ===============================================	*/

extern MiscDeviceType MiscDevice;

/* === Function prototypes ============================================	*/

Err MiscRomVersionCompatible(UInt32 minV, UInt32 maxV,
			     Int16 launchFlags)
  MISC_SECTION;

void MiscShowAbout(void)
  MISC_SECTION;

void MiscShowBusy(Boolean on)
  MISC_SECTION;

void MiscPostError(MiscErrorType mainIndex, MiscErrorType detailIndex,
		   const Char *parameter)
  MISC_SECTION;
void MiscShowPendingError(void)
  MISC_SECTION;
void MiscShowError(MiscErrorType mainIndex, MiscErrorType detailIndex,
		   const Char *parameter)
  MISC_SECTION;

void MiscSetBackupBit(DmOpenRef dbP)
  MISC_SECTION;

void *MiscGetObjectPtr(const FormType *frmP, UInt16 id)
  MISC_SECTION;

/* === The end ========================================================	*/

#endif
