/************************************************************************
 *  vfs.c
 *                      Virtual file system support
 *
 ************************************************************************
 *
 * Phoinix,
 * Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
 *
 * (c)2000-2006 Bodo Wenzel
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
 *  $Log: vfs.c,v $
 *  Revision 1.21  2006/12/19 18:36:05  bodowenzel
 *  Relaxed import from Launcher directory
 *
 *  Revision 1.20  2005/05/26 19:35:47  bodowenzel
 *  Moved game's info into game's entry, removing MemoryMbcInfo
 *
 *  Revision 1.19  2005/05/03 08:40:34  bodowenzel
 *  Adding states dirty flag for fewer error messages
 *
 *  Revision 1.18  2005/01/30 19:35:27  bodowenzel
 *  Removed support for VFS mount and unmount
 *
 *  Revision 1.17  2005/01/29 10:25:37  bodowenzel
 *  Added const qualifier to pointer parameters
 *
 *  Revision 1.16  2005/01/28 17:35:13  bodowenzel
 *  Manager form uses a scrollable table now for the list of games
 *
 *  Revision 1.15  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.14  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.13  2004/09/19 12:35:54  bodowenzel
 *  Enhanced error reporting with string parameter
 *
 *  Revision 1.12  2004/06/20 14:19:46  bodowenzel
 *  Adjustments for volume names, correct RAM name
 *
 *  Revision 1.11  2004/06/11 16:15:31  bodowenzel
 *  Enhanced error reporting with MiscPostError()
 *  Show names of game and volume when asking for overwrite
 *
 *  Revision 1.10  2004/04/13 19:47:04  bodowenzel
 *  Last cleanups
 *  Refactoring of deleting states
 *
 *  Revision 1.9  2004/04/05 21:56:55  bodowenzel
 *  VFS review and its consequences
 *
 *  Revision 1.8  2004/03/02 19:24:16  bodowenzel
 *  Changed to new error function MiscShowError()
 *
 *  Revision 1.7  2004/01/11 19:05:58  bodowenzel
 *  Start of VFS review and correction
 *
 *  Revision 1.6  2003/04/27 09:35:00  bodowenzel
 *  Create new file at copy
 *  Added Lite edition
 *
 *  Revision 1.5  2002/12/19 21:23:24  bodowenzel
 *  Corrected display of busy form
 *  Move between media only when not overwriting
 *
 *  Revision 1.4  2002/12/16 20:38:58  bodowenzel
 *  Switches for showing the busy form
 *  Renaming games on VFS
 *  Better import of games from Launcher directory
 *
 *  Revision 1.3  2002/10/23 16:34:35  bodowenzel
 *  Busy form centralized
 *
 *  Revision 1.2  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.1  2002/05/10 15:17:39  bodowenzel
 *  Till's VFS contribution
 *
 ************************************************************************
 */

/* === Includes =======================================================	*/

#include <PalmOS.h>
#include <VFSMgr.h>

#include "vfs.h"

#include "Phoinix.h"
#include "misc.h"
#include "manager.h"
#include "states.h"
#include "ram.h"

/* === Constants ======================================================	*/

/* I didn't find this definition in any header... */
#define fileNameLength (256)

#define phoinixDirName           "/PALM/Phoinix"
#define phoinixDirFileNameLength \
  (sizeof(phoinixDirName) - 1 + dmDBNameLength + pdbSuffixNameLength)

#define launcherDirName           "/PALM/Launcher"
#define launcherDirFileNameLength \
  (sizeof(launcherDirName) - 1 + fileNameLength + pdbSuffixNameLength)

#define pdbSuffixName       ".pdb"
#define pdbSuffixNameLength (sizeof(pdbSuffixName) - 1)

#define copyChunkSize (8192)

/* === Typedefinitions ================================================	*/

/* I didn't find this definition in any header file... */
typedef struct {
  UInt8          name[dmDBNameLength];
  UInt16         attributes;
  UInt32         creationDate;
  UInt32         modificationDate;
  UInt32         lastBackupDate;
  UInt32         modificationNumber;
  LocalID        appInfoID;
  LocalID        sortInfoID;
  UInt32         type;
  UInt32         creator;
  UInt32         uniqueIDSeed;
/*  RecordListType recordList; not used */
} DatabaseHdrType;

/* === Global and static variables ====================================	*/

UInt16        VfsNumberOfVolumes;
VfsVolumeType **VfsVolume;

static VfsVolumeType VolumePalmRam = { "", vfsInvalidVolRef };
static VfsVolumeType *VolumeFallback = &VolumePalmRam;

/* === Function prototypes ============================================	*/

static Boolean ImportFromLauncherDir(UInt16 volRefNum)
  VFS_SECTION;

static MiscErrorType MoveRamToVfs(const ManagerDbListEntryType *gameP,
				  UInt16 newVolIndex)
  VFS_SECTION;
static MiscErrorType MoveVfsToVfs(const ManagerDbListEntryType *gameP,
				  UInt16 newVolIndex)
  VFS_SECTION;
static MiscErrorType MoveVfsToRam(const ManagerDbListEntryType *gameP)
  VFS_SECTION;
static MiscErrorType CopyFile(
  UInt16 srcVolRefNum, const Char *srcFileName,
  UInt16 dstVolRefNum, const Char *dstFileName)
  VFS_SECTION;

static Boolean ProvidePhoinixDir(UInt16 volRefNum)
  VFS_SECTION;
static Boolean FileExists(UInt16 volRefNum, const Char *fileName)
  VFS_SECTION;

/* === Init and Exit ==================================================	*/

/**
 * Sets up the volume list, checks for VFS and if present collects volume
 * names.
 *
 * @return true if successful.
 */
Boolean VfsInit(void) {
  UInt32 vfsMgrVersion;
  UInt32 iterator;
  UInt16 volRefNum;
  UInt16 volIndex;

  /* get name of Palm RAM, caution with the size of the name entry */
  MemCardInfo(ramGameDbCardNo, VolumePalmRam.name, NULL, NULL, NULL, NULL,
	      NULL, NULL);

  /* Palm RAM always exists and counts as a volume until categories are
   * implemented.
   */
  VfsNumberOfVolumes = 1;
  VfsVolume = &VolumeFallback;

  /* determine number of volumes */
  if (FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &vfsMgrVersion)
      == errNone) {
    UInt32 iterator;
    UInt16 volRefNum;

    iterator = vfsIteratorStart;
    while (iterator != vfsIteratorStop) {
      if (VFSVolumeEnumerate(&volRefNum, &iterator) != errNone) {
	break;
      }

      VfsNumberOfVolumes++;
    }
  }

  /* if no VFS volumes found, quit */
  if (VfsNumberOfVolumes == 1) {
    return true;
  }

  /* allocate volume list */
  VfsVolume = MemPtrNew((sizeof(VfsVolumeType*) + sizeof(VfsVolumeType))
			* VfsNumberOfVolumes);
  if (VfsVolume == NULL) {
    VfsNumberOfVolumes = 1;
    VfsVolume = &VolumeFallback;
    return false;
  }

  /* first entry is Palm RAM, allocated memory is unused */
#if vfsRamVolIndex != 0
#error "vfsRamVolIndex must be 0!"
#endif
  VfsVolume[vfsRamVolIndex] = &VolumePalmRam;

  /* get the volume names */
  volIndex = vfsRamVolIndex;
  iterator = vfsIteratorStart;
  while (iterator != vfsIteratorStop) {
    if (VFSVolumeEnumerate(&volRefNum, &iterator) != errNone) {
      break;
    }

    volIndex++;
    VfsVolume[volIndex] =
      (VfsVolumeType*)(VfsVolume + VfsNumberOfVolumes) + volIndex;
    VFSVolumeGetLabel(volRefNum, VfsVolume[volIndex]->name,
		      vfsVolNameLength);
    if (VfsVolume[volIndex]->name[0] == '\0') {
      StrPrintF(VfsVolume[volIndex]->name, "Volume %d", volIndex);
    }
    VfsVolume[volIndex]->volRefNum = volRefNum;
  }

  return true;
}

/**
 * Finds the volume index for a volume name.
 *
 * @param name volume name.
 * @return     volume index, vfsRamVolIndex on error.
 */
UInt16 VfsFindVolIndex(const Char *name) {
  UInt16 volume;

  for (volume = vfsRamVolIndex; volume < VfsNumberOfVolumes; volume++) {
    if (StrCompare(VfsVolume[volume]->name, name) == 0) {
      return volume;
    }
  }
  return vfsRamVolIndex;
}

/**
 * Release all used resources.
 */
void VfsExit(void) {
  if (VfsVolume != &VolumeFallback) {
    MemPtrFree(VfsVolume);
  }
}

/* === Scanning VFS volumes ===========================================	*/

/**
 * Scans volumes for game databases, importing new games from the
 * Launcher directory. Only the last error is reported.
 *
 * @return error code from misc.h, miscErrNone if successful.
 */
MiscErrorType VfsScanForGames(void) {
  MiscErrorType err = miscErrNone;
  UInt16        volume;

  for (volume = vfsRamVolIndex + 1;
       volume < VfsNumberOfVolumes;
       volume++) {
    FileRef dir;
    Err     vfsErr;

    if (!ImportFromLauncherDir(VfsVolume[volume]->volRefNum)) {
      err = miscErrImport;
    }

    vfsErr = VFSFileOpen(VfsVolume[volume]->volRefNum, phoinixDirName,
			 vfsModeRead, &dir);
    if (vfsErr != errNone) {
      if (vfsErr != vfsErrFileNotFound) {
	err = miscErrSearchVfs;
      }

    } else {
      UInt32       iterator;
      FileInfoType info;
      Char         fileName[fileNameLength];

      info.nameP = fileName;
      info.nameBufLen = sizeof(fileName);

      /* scan all entries of the Phoinix directory,
       * checking for the correct name length, suffix, creator, and type,
       * if OK: add to the games list
       */
      for (iterator = vfsIteratorStart;
	   iterator != vfsIteratorStop
	     && VFSDirEntryEnumerate(dir, &iterator, &info) == errNone;
	   /**/) {
	if (StrLen(fileName) - pdbSuffixNameLength < dmDBNameLength &&
	    StrCompare(
	      fileName + StrLen(fileName) - pdbSuffixNameLength,
	      pdbSuffixName) == 0) {
	  Char    fullFileName[phoinixDirFileNameLength];
	  FileRef file;

	  StrCopy(fullFileName, phoinixDirName);
	  StrCat(fullFileName, "/");
	  StrCat(fullFileName, fileName);
	  if (VFSFileOpen(VfsVolume[volume]->volRefNum, fullFileName,
			  vfsModeRead, &file) != errNone) {
	    err = miscErrSearchVfs;

	  } else {
	    UInt32 type, creator;

	    vfsErr = VFSFileDBInfo(file, NULL, NULL, NULL, NULL, NULL,
				   NULL, NULL, NULL, NULL, &type,
				   &creator, NULL);
	    VFSFileClose(file);
	    if (vfsErr != errNone) {
	      err = miscErrSearchVfs;

	    } else if (creator == miscCreator && type == miscGameDbType) {
	      /* OK, it's a game, let's add it with its filename! */
	      fileName[StrLen(fileName) - pdbSuffixNameLength] = '\0';
	      if (!ManagerListAdd(fileName, volume)) {
		VFSFileClose(dir);
		return miscErrMemoryFull;
	      }
	    }
	  }
	}
      }
      VFSFileClose(dir);
    }
  }
  return err;
}

/**
 * Import games from the Launcher directory into Phoinix directory. The
 * new filename is derived from the database name. Only the last error is
 * reported.
 *
 * @param volRefNum volume reference.
 * @return          true if successful.
 */
static Boolean ImportFromLauncherDir(UInt16 volRefNum) {
  Boolean ok = true;
  FileRef dir;

  if (VFSFileOpen(volRefNum, launcherDirName, vfsModeRead, &dir)
      == errNone) {
    UInt32       iterator;
    FileInfoType info;
    Char         fileName[fileNameLength];

    info.nameP = fileName;
    info.nameBufLen = sizeof(fileName);

    /* scan all entries of the Launcher directory,
     * checking for the correct suffix, creator, and type,
     * if OK: try to copy, on success delete the source
     */
    for (iterator = vfsIteratorStart;
	 iterator != vfsIteratorStop
	   && VFSDirEntryEnumerate(dir, &iterator, &info) == errNone;
	 /**/) {
      if (StrCaselessCompare(
	    fileName + StrLen(fileName) - pdbSuffixNameLength,
	    pdbSuffixName) == 0) {
	Char    oldFileName[launcherDirFileNameLength];
	FileRef file;

	StrCopy(oldFileName, launcherDirName);
	StrCat(oldFileName, "/");
	StrCat(oldFileName, fileName);
	if (VFSFileOpen(volRefNum, oldFileName, vfsModeRead, &file)
	    == errNone) {
	  Char   dbName[dmDBNameLength];
	  UInt32 type, creator;
	  Err    err;

	  err = VFSFileDBInfo(file, dbName, NULL, NULL, NULL, NULL, NULL,
			      NULL, NULL, NULL, &type, &creator, NULL);
	  VFSFileClose(file);
	  if (err == errNone
	      && creator == miscCreator && type == miscGameDbType) {
	    /* OK, it's a game, let's move it! */
	    if (!ProvidePhoinixDir(volRefNum)) {
	      ok = false;
	    } else {
	      Char newFileName[phoinixDirFileNameLength];

	      StrPrintF(newFileName, phoinixDirName  "/%s" pdbSuffixName,
			dbName);
	      if (FileExists(volRefNum, newFileName)) {
		ok = false;
	      } else {
		if (CopyFile(volRefNum, oldFileName, volRefNum,
			     newFileName) != miscErrNone) {
		  ok = false;
		} else {
		  (void)VFSFileDelete(volRefNum, oldFileName);
		  /* since we changed the contents of the directory, the
		   * iteration has to start over again
		   */
		  iterator = vfsIteratorStart;
		}
	      }
	    }
	  }
	}
      }
    }
    VFSFileClose(dir);
  }
  return ok;
}

/* === Moving games between different media ===========================	*/

/**
 * Moves the game into the given destination.
 *
 * @param gameP       pointer to the game's entry.
 * @param newVolIndex destination volume number.
 * @return            error code from misc.h, miscErrNone if successful.
 */
MiscErrorType VfsMove(const ManagerDbListEntryType *gameP,
		      UInt16 newVolIndex) {
  if (gameP->volIndex == vfsRamVolIndex) {
    return MoveRamToVfs(gameP, newVolIndex);
  } else if (newVolIndex == vfsRamVolIndex) {
    return MoveVfsToRam(gameP);
  } else {
    return MoveVfsToVfs(gameP, newVolIndex);
  }
}

/**
 * Moves the game from Palm RAM to VFS.
 *
 * @param gameP       pointer to the game's entry.
 * @param newVolIndex destination volume number.
 * @return            error code from misc.h, miscErrNone if successful.
 */
static MiscErrorType MoveRamToVfs(const ManagerDbListEntryType *gameP,
				  UInt16 newVolIndex) {
  Char          gameFileName[phoinixDirFileNameLength];
  Char          stateDbName[dmDBNameLength];
  Char          stateFileName[phoinixDirFileNameLength];
  LocalID       dbIdGame, dbIdState;
  UInt16        newVolRefNum;
  Err           vfsErr;
  MiscErrorType err;

  if (!ProvidePhoinixDir(VfsVolume[newVolIndex]->volRefNum)) {
    return miscErrNoPhoinixDir;
  }

  StrPrintF(gameFileName, phoinixDirName "/%s" pdbSuffixName,
	    gameP->name);
  StrPrintF(stateDbName, statesDbNameFormat, gameP->crc);
  StrPrintF(stateFileName, phoinixDirName "/%s" pdbSuffixName,
	    stateDbName);

  dbIdGame = DmFindDatabase(ramGameDbCardNo, gameP->name);
  dbIdState = DmFindDatabase(ramStatesDbCardNo, stateDbName);
  if (dbIdGame == 0) {
    return miscErrFileNotFound;
  }

  newVolRefNum = VfsVolume[newVolIndex]->volRefNum;

  /* if the destination exists, ask for confirmation to overwrite */
  if (FileExists(newVolRefNum, gameFileName)) {
    if (FrmCustomAlert(alertIdOverwrite, gameP->name,
		       VfsVolume[newVolIndex]->name, NULL) != 0) {
      return miscErrNone;
    }
    if (VFSFileDelete(newVolRefNum, gameFileName) != errNone) {
      return miscErrVfsReadOnly;
    }
  }
  if (FileExists(newVolRefNum, stateFileName)) {
    if (VFSFileDelete(newVolRefNum, stateFileName) != errNone) {
      return miscErrVfsReadOnly;
    }
  }

  MiscShowBusy(true);

  vfsErr =VFSExportDatabaseToFile(newVolRefNum, gameFileName,
				  ramGameDbCardNo, dbIdGame);
  if (vfsErr == errNone && dbIdState != 0) {
    vfsErr = VFSExportDatabaseToFile(newVolRefNum, stateFileName,
				     ramStatesDbCardNo, dbIdState);
    if (vfsErr != errNone) {
      (void)VFSFileDelete(newVolRefNum, stateFileName);
    }
  }

  MiscShowBusy(false);

  if (vfsErr != errNone) {
    (void)VFSFileDelete(newVolRefNum, gameFileName);
    if (vfsErr == vfsErrVolumeFull) {
      return miscErrVolumeFull;
    } else {
      return miscErrWrite;
    }
  }

  /* everything seems to be fine, delete origin */
  ManagerListSetLocation(newVolIndex);
  err = miscErrNone;
  if (DmDeleteDatabase(ramGameDbCardNo, dbIdGame) != errNone) {
    err = miscErrRamReadOnly;
  }
  if (dbIdState != 0) {
    if (DmDeleteDatabase(ramStatesDbCardNo, dbIdState) != errNone) {
      err = miscErrRamReadOnly;
    }
  }
  return err;
}

/**
 * Moves the game from VFS to Palm RAM.
 *
 * @param gameP pointer to the game's entry.
 * @return      error code from misc.h, miscErrNone if successful.
 */
static MiscErrorType MoveVfsToRam(const ManagerDbListEntryType *gameP) {
  Char          gameFileName[phoinixDirFileNameLength];
  Char          stateDbName[dmDBNameLength];
  Char          stateFileName[phoinixDirFileNameLength];
  UInt16        gameCardNo, stateCardNo;
  LocalID       gameDbId, stateDbId;
  UInt16        oldVolRefNum;
  Err           vfsErr;
  MiscErrorType err;

  StrPrintF(gameFileName, phoinixDirName "/%s" pdbSuffixName,
	    gameP->name);
  StrPrintF(stateDbName, statesDbNameFormat, gameP->crc);
  StrPrintF(stateFileName, phoinixDirName "/%s" pdbSuffixName,
	    stateDbName);

  /* if the destination exists, ask for confirmation to overwrite */
  gameDbId = DmFindDatabase(ramGameDbCardNo, gameP->name);
  if (gameDbId != 0) {
    if (FrmCustomAlert(alertIdOverwrite, gameP->name, VolumePalmRam.name,
		       NULL) != 0) {
      return miscErrNone;
    }
    if (DmDeleteDatabase(ramGameDbCardNo, gameDbId) != errNone) {
      return miscErrRamReadOnly;
    }
  }
  stateDbId = DmFindDatabase(ramStatesDbCardNo, stateDbName);
  if (stateDbId != 0) {
    if (DmDeleteDatabase(ramStatesDbCardNo, stateDbId) != errNone) {
      return miscErrRamReadOnly;
    }
  }

  MiscShowBusy(true);

  oldVolRefNum = VfsVolume[gameP->volIndex]->volRefNum;

  vfsErr = VFSImportDatabaseFromFile(oldVolRefNum, gameFileName,
				     &gameCardNo, &gameDbId);
  if (vfsErr == errNone) {
    if (gameCardNo != ramGameDbCardNo) {
      vfsErr = dmErrMemError;
    } else {
      vfsErr = VFSImportDatabaseFromFile(oldVolRefNum, stateFileName,
					 &stateCardNo, &stateDbId);
      if (vfsErr == errNone) {
	if (stateCardNo != ramStatesDbCardNo) {
	  vfsErr = dmErrMemError;
	}
      }
      if (vfsErr == vfsErrFileNotFound) {
	vfsErr = errNone;
      } else if (vfsErr != errNone) {
	(void)DmDeleteDatabase(stateCardNo, stateDbId);
      }
    }
  }

  MiscShowBusy(false);

  if (vfsErr != errNone) {
    (void)DmDeleteDatabase(gameCardNo, gameDbId);
    if (vfsErr == dmErrMemError) {
      return miscErrMemoryFull;
    } else {
      return miscErrWrite;
    }
  }

  /* everything seems to be fine, delete origin */
  ManagerListSetLocation(vfsRamVolIndex);
  err = miscErrNone;
  if (VFSFileDelete(oldVolRefNum, gameFileName) != errNone) {
    err = miscErrVfsReadOnly;
  }
  if (FileExists(oldVolRefNum, stateFileName)) {
    if (VFSFileDelete(oldVolRefNum, stateFileName) != errNone) {
      err = miscErrVfsReadOnly;
    }
  }
  return err;
}

/**
 * Moves the game from VFS to VFS.
 *
 * @param gameP       pointer to the game's entry.
 * @param newVolIndex destination volume number.
 * @return            error code from misc.h, miscErrNone if successful.
 */
static MiscErrorType MoveVfsToVfs(const ManagerDbListEntryType *gameP,
				  UInt16 newVolIndex) {
  Char          gameFileName[phoinixDirFileNameLength];
  Char          stateFileName[phoinixDirFileNameLength];
  UInt16        oldVolRefNum, newVolRefNum;
  MiscErrorType err;

  if (!ProvidePhoinixDir(VfsVolume[newVolIndex]->volRefNum)) {
    return miscErrNoPhoinixDir;
  }

  StrPrintF(gameFileName, phoinixDirName "/%s" pdbSuffixName,
	    gameP->name);
  StrPrintF(stateFileName,
	    phoinixDirName "/" statesDbNameFormat pdbSuffixName,
	    gameP->crc);

  oldVolRefNum = VfsVolume[gameP->volIndex]->volRefNum;
  newVolRefNum = VfsVolume[newVolIndex]->volRefNum;

  /* if the destination exists, ask for confirmation to overwrite */
  if (FileExists(newVolRefNum, gameFileName)) {
    if (FrmCustomAlert(alertIdOverwrite, gameP->name,
		       VfsVolume[newVolIndex]->name, NULL) != 0) {
      return miscErrNone;
    }
    if (VFSFileDelete(newVolRefNum, gameFileName) != errNone) {
      return miscErrVfsReadOnly;
    }
  }
  if (FileExists(newVolRefNum, stateFileName)) {
    if (VFSFileDelete(newVolRefNum, stateFileName) != errNone) {
      return miscErrVfsReadOnly;
    }
  }

  MiscShowBusy(true);

  err = CopyFile(oldVolRefNum, gameFileName, newVolRefNum, gameFileName);
  if (err == miscErrNone) {
    err = CopyFile(oldVolRefNum, stateFileName, newVolRefNum,
		   stateFileName);
    if (err == miscErrFileNotFound) {
      err = miscErrNone;
    }
  }

  MiscShowBusy(false);

  if (err != miscErrNone) {
    return err;
  }

  /* everything seems to be fine, delete origin */
  ManagerListSetLocation(newVolIndex);
  err = miscErrNone;
  if (VFSFileDelete(oldVolRefNum, gameFileName) != errNone) {
    err = miscErrVfsReadOnly;
  }
  if (FileExists(oldVolRefNum, stateFileName)) {
    if (VFSFileDelete(oldVolRefNum, stateFileName) != errNone) {
      err = miscErrVfsReadOnly;
    }
  }
  return err;
}

/**
 * Copies the source file into the destination file.
 *
 * @param srcVolRefNum source volume number.
 * @param srcFileName  source filename.
 * @param dstVolRefNum destination volume number.
 * @param dstFileName  destination filename.
 * @return             error code from misc.h, miscErrNone if successful.
 */
static MiscErrorType CopyFile(
  UInt16 srcVolRefNum, const Char *srcFileName,
  UInt16 dstVolRefNum, const Char *dstFileName) {
  MiscErrorType err = miscErrNone;
  void          *buffer;

  buffer = MemPtrNew(copyChunkSize);
  if (buffer == NULL) {
    err = miscErrMemoryFull;

  } else {
    FileRef srcFile;

    if (VFSFileOpen(srcVolRefNum, srcFileName, vfsModeRead, &srcFile)
	!= errNone) {
      err = miscErrFileNotFound;

    } else {
      Err     vfsErr;
      FileRef dstFile;

      vfsErr = VFSFileOpen(dstVolRefNum, dstFileName,
			   vfsModeWrite | vfsModeCreate | vfsModeTruncate,
			   &dstFile);
      if (vfsErr == vfsErrBadName || vfsErr == vfsErrDirectoryNotFound) {
	err = miscErrBadName;
      } else if (vfsErr != errNone) {
	err = miscErrVfsReadOnly;

      } else {
	UInt32 numberOfBytes;

	/* here the copying takes place: */
	for (;;) {
	  vfsErr = VFSFileRead(srcFile, copyChunkSize, buffer,
			       &numberOfBytes);
	  if (vfsErr != errNone && vfsErr != vfsErrFileEOF) {
	    err = miscErrRead;
	    break;
	  }
	  if (numberOfBytes == 0) {
	    break;
	  }

	  vfsErr = VFSFileWrite(dstFile, numberOfBytes, buffer, NULL);
	  if (vfsErr == vfsErrVolumeFull) {
	    err = miscErrVolumeFull;
	    break;
	  } else if (vfsErr != errNone) {
	    err = miscErrWrite;
	    break;
	  }
	}

	vfsErr = VFSFileClose(dstFile);
	if (err == miscErrNone) {
	  if (vfsErr == vfsErrVolumeFull) {
	    err = miscErrVolumeFull;
	  } else if (vfsErr != errNone) {
	    err = miscErrWrite;
	  }
	}
	if (err != miscErrNone) {
	  (void)VFSFileDelete(dstVolRefNum, dstFileName);
	}
      }
      VFSFileClose(srcFile);
    }
    MemPtrFree(buffer);
  }
  return err;
}

/* === Support functions for the manager form =========================	*/

/**
 * Fetches the first block of the game in order to obtain CRC and
 * cartridge infos.
 *
 * @param gameP pointer to the game's entry.
 * @return      handle of the block, NULL if error.
 */
MemHandle VfsGetFirstBlock(const ManagerDbListEntryType *gameP) {
  Char      fileName[phoinixDirFileNameLength];
  FileRef   file;
  MemHandle recH;

  StrPrintF(fileName, phoinixDirName "/%s" pdbSuffixName, gameP->name);

  if (VFSFileOpen(VfsVolume[gameP->volIndex]->volRefNum, fileName,
		  vfsModeRead, &file) != errNone) {
    return NULL;
  }
  if (VFSFileDBGetRecord(file, 0, &recH, NULL, NULL) != errNone) {
    if (recH != NULL) {
      MemHandleFree(recH);
      recH = NULL;
    }
  }
  VFSFileClose(file);

  return recH;
}

/**
 * Retrieves the total size of both the game and the states database.
 *
 * @param gameP pointer to the game's entry.
 * @return      size in bytes, 0 on error.
 */
UInt32 VfsTotalSize(const ManagerDbListEntryType *gameP) {
  UInt32  sizeGame, sizeStates;
  Err     err;
  Char    fileName[phoinixDirFileNameLength];
  FileRef file;

  /* the game has to exist */
  StrPrintF(fileName, phoinixDirName "/%s" pdbSuffixName, gameP->name);
  if (VFSFileOpen(VfsVolume[gameP->volIndex]->volRefNum, fileName,
		  vfsModeRead, &file) != errNone) {
    return 0;
  }
  err = VFSFileSize(file, &sizeGame);
  VFSFileClose(file);
  if (err != errNone) {
    return 0;
  }

  /* the states database doesn't have to exist */
  sizeStates = 0;
  StrPrintF(fileName, phoinixDirName "/" statesDbNameFormat pdbSuffixName,
	    gameP->crc);
  if (VFSFileOpen(VfsVolume[gameP->volIndex]->volRefNum, fileName,
		  vfsModeRead, &file) == errNone) {
    err = VFSFileSize(file, &sizeStates);
    VFSFileClose(file);
    if (err != errNone) {
      return 0;
    }
  }

  return sizeGame + sizeStates;
}

/**
 * Rename the game.
 *
 * @param gameP pointer to the game's entry.
 * @param name  pointer to the new name.
 * @return      error code from misc.h, miscErrNone if successful.
 */
MiscErrorType VfsRenameGame(const ManagerDbListEntryType *gameP,
			    const Char *name) {
  Char    fileName[phoinixDirFileNameLength];
  FileRef file;
  Char    newName[dmDBNameLength + pdbSuffixNameLength];
  Err     err;

  StrPrintF(fileName, phoinixDirName "/%s" pdbSuffixName, name);
  if (FileExists(VfsVolume[gameP->volIndex]->volRefNum, fileName)) {
    return miscErrAlreadyExists;
  }

  StrPrintF(fileName, phoinixDirName "/%s" pdbSuffixName, gameP->name);

  /* rename the database in the file */
  if (VFSFileOpen(VfsVolume[gameP->volIndex]->volRefNum, fileName,
		  vfsModeReadWrite, &file) != errNone) {
    return miscErrVfsReadOnly;
  }
  err = VFSFileSeek(file, vfsOriginBeginning,
		    OffsetOf(DatabaseHdrType, name));
  if (err == errNone) {
    err = VFSFileWrite(file, dmDBNameLength, name, NULL);
  }
  (void)VFSFileClose(file);
  if (err != errNone) {
    return miscErrWrite;
  }

  /* rename the file, on error try to restore the old database name */
  StrPrintF(newName, "%s" pdbSuffixName, name);
  err = VFSFileRename(VfsVolume[gameP->volIndex]->volRefNum, fileName,
		      newName);
  if (err != errNone) {
    if (VFSFileOpen(VfsVolume[gameP->volIndex]->volRefNum, fileName,
		    vfsModeReadWrite, &file) == errNone) {
      if (VFSFileSeek(file, vfsOriginBeginning,
		      OffsetOf(DatabaseHdrType, name)) == errNone) {
	(void)VFSFileWrite(file, dmDBNameLength, gameP->name, NULL);
      }
      VFSFileClose(file);
    }
    return miscErrBadName;
  }

  return miscErrNone;
}

/**
 * Delete the game.
 *
 * @param gameP pointer to the game's entry.
 * @return      error code from misc.h, miscErrNone if successful.
 */
MiscErrorType VfsDeleteGame(const ManagerDbListEntryType *gameP) {
  Char fileName[phoinixDirFileNameLength];

  StrPrintF(fileName, phoinixDirName "/%s" pdbSuffixName, gameP->name);
  if (VFSFileDelete(VfsVolume[gameP->volIndex]->volRefNum, fileName)
      != errNone) {
    return miscErrVfsReadOnly;
  }

  return miscErrNone;
}

/**
 * Delete the states. Errors are silently ignored.
 *
 * @param gameP pointer to the game's entry.
 */
void VfsDeleteStates(const ManagerDbListEntryType *gameP) {
  Char fileName[phoinixDirFileNameLength];

  StrPrintF(fileName, phoinixDirName "/" statesDbNameFormat pdbSuffixName,
	    gameP->crc);
  (void)VFSFileDelete(VfsVolume[gameP->volIndex]->volRefNum, fileName);
}

/* === Open and close games ===========================================	*/

/**
 * Copy the game to Palm RAM and open it.
 *
 * @param gameP pointer to the game's entry.
 * @return      reference to open database, NULL on error.
 */
DmOpenRef VfsGameDbOpen(const ManagerDbListEntryType *gameP) {
  Char    fileName[phoinixDirFileNameLength];
  UInt16  cardNo;
  LocalID dbId;

  StrPrintF(fileName, phoinixDirName "/%s" pdbSuffixName, gameP->name);
  if (VFSImportDatabaseFromFile(VfsVolume[gameP->volIndex]->volRefNum,
				fileName, &cardNo, &dbId) == errNone) {
    if (cardNo == ramGameDbCardNo) {
      DmOpenRef dbP;

      dbP = DmOpenDatabase(cardNo, dbId, dmModeReadOnly);
      if (dbP != NULL) {
	return dbP;
      }
    }
    (void)DmDeleteDatabase(cardNo, dbId);
  }
  return NULL;
}

/**
 * Close the game. Since the game was loaded from VFS, the database is
 * simply deleted.
 *
 * @param dbP reference to open database.
 */
void VfsGameDbClose(DmOpenRef dbP) {
  LocalID dbId;
  UInt16  cardNo;

  DmOpenDatabaseInfo(dbP, &dbId, NULL, NULL, &cardNo, NULL);
  DmCloseDatabase(dbP);
  if (dbId != 0) {
    DmDeleteDatabase(cardNo, dbId);
  }
}

/* === Open and close states ==========================================	*/

/**
 * Copy the states to Palm RAM and open it.
 *
 * @param gameP pointer to the game's entry.
 * @return      reference to open database, NULL on error.
 */
DmOpenRef VfsStatesDbOpen(const ManagerDbListEntryType *gameP) {
  Char    dbName[dmDBNameLength];
  Char    fileName[phoinixDirFileNameLength];
  LocalID dbId;
  UInt16  cardNo;

  StrPrintF(dbName, statesDbNameFormat, gameP->crc);
  StrPrintF(fileName, phoinixDirName "/%s" pdbSuffixName, dbName);

  if (!FileExists(VfsVolume[gameP->volIndex]->volRefNum, fileName)) {
    return NULL;
  }

  dbId = DmFindDatabase(ramStatesDbCardNo, dbName);
  if (dbId != 0) {
    (void)DmDeleteDatabase(ramStatesDbCardNo, dbId);
  }

  if (VFSImportDatabaseFromFile(VfsVolume[gameP->volIndex]->volRefNum,
				fileName, &cardNo, &dbId) == errNone) {
    if (cardNo == ramStatesDbCardNo) {
      DmOpenRef dbP;

      dbP = DmOpenDatabase(cardNo, dbId, dmModeReadWrite);
      if (dbP != NULL) {
	return dbP;
      }
    }
    (void)DmDeleteDatabase(cardNo, dbId);
  }
  MiscShowError(miscErrVfs, miscErrNewState, NULL);
  return NULL;
}

/**
 * Close states. Since the states were loaded from VFS, the database may
 * have to be written back to the VFS. In any case the local copy is
 * deleted.
 *
 * @param dbP   reference to open database.
 * @param gameP pointer to the game's entry.
 * @param save  if true, write database back to VFS.
 * @return      true if successful.
 */
Boolean VfsStatesDbClose(DmOpenRef dbP,
			 const ManagerDbListEntryType *gameP,
			 Boolean save) {
  LocalID dbId;
  UInt16  cardNo;

  DmOpenDatabaseInfo(dbP, &dbId, NULL, NULL, &cardNo, NULL);
  DmCloseDatabase(dbP);
  if (dbId == 0) {
    return false;
  }

  if (save) {
    Char fileName[phoinixDirFileNameLength];

    StrPrintF(fileName,
	      phoinixDirName "/" statesDbNameFormat pdbSuffixName,
	      gameP->crc);
    if (FileExists(VfsVolume[gameP->volIndex]->volRefNum, fileName)) {
      (void)VFSFileDelete(VfsVolume[gameP->volIndex]->volRefNum,
			  fileName);
    }
    if (VFSExportDatabaseToFile(VfsVolume[gameP->volIndex]->volRefNum,
				fileName, cardNo, dbId) != errNone) {
      return false;
    }
  }
  DmDeleteDatabase(cardNo, dbId);
  return true;
}

/* === General support functions ======================================	*/

/**
 * Checks for Phoinix directory, creates it if necessary.
 *
 * @param volRefNum volume reference.
 * @return          true if directory exists.
 */
static Boolean ProvidePhoinixDir(UInt16 volRefNum) {
  FileRef dir;

  if (VFSFileOpen(
	volRefNum, phoinixDirName, vfsModeRead, &dir) == errNone) {
    VFSFileClose(dir);
    return true;
  }

  if (VFSDirCreate(volRefNum, phoinixDirName) == errNone) {
    return true;
  }

  return false;
}

/**
 * Checks for an existing file of the given name.
 *
 * @param volRefNum volume reference.
 * @param name      pointer to the filename.
 * @return          true if file exists.
 */
static Boolean FileExists(UInt16 volRefNum, const Char *fileName) {
  FileRef file;

  if (VFSFileOpen(volRefNum, fileName, vfsModeRead, &file) == errNone) {
    VFSFileClose(file);
    return true;
  }

  return false;
}

/* === The end ========================================================	*/
