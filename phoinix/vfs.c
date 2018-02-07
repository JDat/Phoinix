/************************************************************************
 *  vfs.c
 *                      Virtual file system support
 *
 ************************************************************************
 *
 * Phoinix,
 * Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
 *
 * (c)2000-2004 Bodo Wenzel
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
 *  Revision 1.5.2.1  2004/02/14 11:52:59  bodowenzel
 *  Corrected import from launcher
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

/* set non-zero for showing the busy form while calculating the CRC */
#define SHOW_BUSY_FOR_CRC (0)
/* set non-zero for showing the busy form while loading the game's state */
#define SHOW_BUSY_FOR_LOAD (0)
/* set non-zero for showing the busy form while unloading the game's state */
#define SHOW_BUSY_FOR_UNLOAD (0)

/* === Includes =======================================================	*/

#include <PalmOS.h>
#include <VFSMgr.h>

#include "vfs.h"
#include "Phoinix.h"
#include "manager.h"
#include "memory.h"
#include "misc.h"
#include "states.h"

/* === Constants ======================================================	*/

#define PHOINIX_DIR  "/PALM/Phoinix"
#define LAUNCHER_DIR "/PALM/Launcher"

#define SUFFIX_PDB  ".pdb"
#define COPY_CHUNK_SIZE 8192

/* === Typedefinitions ================================================	*/

/* I didn't find this definition in any header... */
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

UInt16        VFSPresent = 0;  /* number of installed file systems */
VFSVolumeType **VFSVolume = NULL;

/* infos on currently opened state db */
static UInt16                 VFSStateLoadedCardNo;
static UInt16                 VFSStateLoadedVolIdx=0;
static UInt32                 VFSStateLoadedCRC;
static ManagerDbListEntryType *VFSGameLoaded = NULL;

/* table for error to error string mapping */
static const UInt16 VFSErrors[][2]= {
  { errNone,                    stringerrNone },
  { vfsErrBufferOverflow,       stringvfsErrBufferOverflow},
  { vfsErrFileGeneric,          stringvfsErrFileGeneric},
  { vfsErrFileBadRef,           stringvfsErrFileBadRef},
  { vfsErrFileStillOpen,        stringvfsErrFileStillOpen},
  { vfsErrFilePermissionDenied, stringvfsErrFilePermissionDenied},
  { vfsErrFileAlreadyExists,    stringvfsErrFileAlreadyExists},
  { vfsErrFileEOF,              stringvfsErrFileEOF},
  { vfsErrFileNotFound,         stringvfsErrFileNotFound},
  { vfsErrVolumeBadRef,         stringvfsErrVolumeBadRef},
  { vfsErrVolumeStillMounted,   stringvfsErrVolumeStillMounted},
  { vfsErrNoFileSystem,         stringvfsErrNoFileSystem},
  { vfsErrBadData,              stringvfsErrBadData},
  { vfsErrDirNotEmpty,          stringvfsErrDirNotEmpty},
  { vfsErrBadName,              stringvfsErrBadName},
  { vfsErrVolumeFull,           stringvfsErrVolumeFull},
  { vfsErrUnimplemented,        stringvfsErrUnimplemented},
  { vfsErrNotADirectory,        stringvfsErrNotADirectory},
  { vfsErrIsADirectory,         stringvfsErrIsADirectory},
  { vfsErrDirectoryNotFound,    stringvfsErrDirectoryNotFound},
  { vfsErrNameShortened,        stringvfsErrNameShortened},
  { dmErrAlreadyExists,         stringdmErrAlreadyExists},
  { 0xffff,                     0xffff },
};

/* === Function prototypes ============================================	*/

static void VFSError(Err err, UInt16 msgStr);
static Boolean VFSCheckDir(UInt16 volId);
static Boolean VFSFileExists(UInt16 volId, Char *name);

static void VFSMove_P2V(ManagerDbListEntryType *src, UInt16 dst);
static void VFSMove_V2V(ManagerDbListEntryType *src, UInt16 dst);
static void VFSMove_V2P(ManagerDbListEntryType *src, UInt16 dst);
static Boolean VFSCopyFile(UInt16 err_msg,
			   UInt16 src_vol, Char *src_name,
			   UInt16 dst_vol, Char *dst_name);

static void VFSImportFromLauncher(UInt16 vol);

static Boolean VFSGetMissnamed(UInt16 vol, char *name1, char *name2);
static void VFSPreScanVolume(UInt16 vol);

static void VFSScanFS(void);

/* === Init and Exit ==================================================	*/

/**
 * Checks for VFS and if present scans volumes.
 */
void VFSSetup(void) {
  UInt32 vfsMgrVersion;

  /* try to get vfs version from feature manager */
  if (!FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &vfsMgrVersion)) {
    /* ok, the VFS is there, scan for volumes ... */
    VFSScanFS();

    /* check if more than just the palms memory is available */
    if (VFSPresent > 1) {
      VFSVolume = MemPtrNew((sizeof(VFSVolumeType*) + sizeof(VFSVolumeType))
	  		    * VFSPresent);
      VFSScanFS();
    }
  }
}

/**
 * Cleans up.
 */
void VFSExit(void) {
  /* free allocated memory */
  if (VFSVolume != NULL)
    MemPtrFree(VFSVolume);
}

/* === General support functions ======================================	*/

/**
 * Prints a VFS error message.
 *
 * @param  err    error number.
 * @param  msgStr index of additional message string.
 */
static void VFSError(Err err, UInt16 msgStr) {
  UInt16    i, errStr = stringvfsErrUnknown;
  MemHandle errH, msgH;
  char      *errP, *msgP;

  if (msgStr == -1)
    return;

  msgH = (MemHandle)DmGetResource(strRsc, msgStr);
  msgP = (char *)MemHandleLock(msgH);

  if (err != -1) {
    /* try to find matching error string */
    for (i = 0; VFSErrors[i][0] != 0xffff; i++)
      if (VFSErrors[i][0] == err)
	errStr = VFSErrors[i][1];

    errH = (MemHandle)DmGetResource(strRsc, errStr);
    errP = (char *)MemHandleLock(errH);
    FrmCustomAlert(alertIdVFS, msgP, errP, 0);
    MemHandleUnlock(errH);
  } else
    FrmCustomAlert(alertIdGeneric, msgP, 0, 0);

  MemHandleUnlock(msgH);
}

/**
 * Checks for application specific directory, creates it if necessary.
 *
 * @param  volId volume number.
 * @return true if successful.
 */
static Boolean VFSCheckDir(UInt16 volId) {
  Err     err;
  FileRef file;

  /* try to access phoinix dir */
  if (VFSFileOpen(volId, PHOINIX_DIR, vfsModeRead, &file) == errNone) {
    VFSFileClose(file);
    return true;
  }

  /* try to create dir */
  if ((err = VFSDirCreate(volId, PHOINIX_DIR)) == errNone)
    return true;

  VFSError(err, stringCreateDir);
  return false;
}

/**
 * Checks for an existing file of the given name.
 *
 * @param  volId volume number.
 * @param  name  pointer to the filename.
 * @return true if file exists.
 */
static Boolean VFSFileExists(UInt16 volId, Char *name) {
  FileRef file;
  Err     err;

  /* try to open file */
  if ((err = VFSFileOpen(volId, name, vfsModeRead, &file)) == errNone) {
    VFSFileClose(file);
    return true;
  }

  return false;
}

/* === Moving games between different media ===========================	*/

/**
 * Moves the current game into the given destination.
 *
 * @param src pointer to the entry in the games list.
 * @param dst destination volume number.
 */
void VFSMove(ManagerDbListEntryType *src, UInt16 dst) {
  /* no VFS? */
  if (VFSVolume == NULL)
    return;

  /* check if volume has really been changed */
  if (src->volIdx == dst)
    return;

  if ((src->volIdx == 0) && (dst != 0))
    VFSMove_P2V(src, dst);                   /* from palm to vfs */
  else if ((src->volIdx != 0) && (dst != 0))
    VFSMove_V2V(src, dst);                   /* from vfs to vfs */
  else
    VFSMove_V2P(src, dst);                   /* from vfs to palm */
}

/**
 * Moves from Palm RAM (internal) to VFS (external).
 *
 * @param src pointer to the entry in the games list.
 * @param dst destination volume number.
 */
static void VFSMove_P2V(ManagerDbListEntryType *src, UInt16 dst) {
  Err     err;
  Char    gameName[20 + dmDBNameLength];   /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  Char    stateName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  LocalID dbIdGame, dbIdState;

  /* check if phoinix dir exists */
  if (!VFSCheckDir(VFSVolume[dst]->volId))
    return;

  /* build game file name */
  StrPrintF(gameName, PHOINIX_DIR "/%s" SUFFIX_PDB, src->name);

  if (VFSFileExists(VFSVolume[dst]->volId, gameName)) {
    if (FrmAlert(alertIdOverwrite) != 0)
      return;
    VFSFileDelete(VFSVolume[dst]->volId, gameName);
  }

  /* get local index of game database */
  if ((dbIdGame = DmFindDatabase(src->cardNo, src->name)) == 0) {
    VFSError(-1, stringExportDb);
    return;
  }

  /* copy game database to card */
  MiscShowBusy(true);
  err = VFSExportDatabaseToFile(VFSVolume[dst]->volId,
				gameName, src->cardNo, dbIdGame);
  MiscShowBusy(false);
  if (err != errNone) {
    /* on error delete game database */
    VFSError(err, stringExportDb);
    VFSFileDelete(VFSVolume[dst]->volId, gameName);
    return;
  }

  /* build name of state database */
  StrPrintF(stateName, gameStatesDbName, MemoryMbcInfo.crc);

  /* close state database if open */
  StatesClosePrefsAndStates();

  /* get local index of state database */
  if ((dbIdState = DmFindDatabase(src->cardNo, stateName)) != 0) {
    /* build vfs file name of state database */
    StrPrintF(stateName, PHOINIX_DIR "/" gameStatesDbName SUFFIX_PDB,
	      MemoryMbcInfo.crc);

    /* copy game database to card */
    MiscShowBusy(true);
    if (VFSFileExists(VFSVolume[dst]->volId, stateName))
      VFSFileDelete(VFSVolume[dst]->volId, stateName);
    err = VFSExportDatabaseToFile(VFSVolume[dst]->volId,
				  stateName, src->cardNo, dbIdState);
    MiscShowBusy(false);
    if (err != errNone) {
      /* on error delete game/state database already copied */
      VFSError(err, stringExportDb);
      VFSFileDelete(VFSVolume[dst]->volId, gameName);
      VFSFileDelete(VFSVolume[dst]->volId, stateName);
      return;
    }

    /* everything seems to be fine, remove state database from handheld */
    DmDeleteDatabase(src->cardNo, dbIdState);
  }

  /* everything seems to be fine, remove game database from handheld */
  DmDeleteDatabase(src->cardNo, dbIdGame);

  /* reflect change in list */
  ManagerListChangeLocation(dst, 0);
}

/**
 * Moves from VFS (external) to VFS (external).
 *
 * @param src pointer to the entry in the games list.
 * @param dst destination volume number.
 */
static void VFSMove_V2V(ManagerDbListEntryType *src, UInt16 dst) {
  Err  err;
  Char gameName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  Char stateName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */

  /* check if phoinix dir exists */
  if (!VFSCheckDir(VFSVolume[dst]->volId))
    return;

  /* build game database file name */
  StrPrintF(gameName, PHOINIX_DIR "/%s" SUFFIX_PDB, src->name);

  if (VFSFileExists(VFSVolume[dst]->volId, gameName)) {
    if (FrmAlert(alertIdOverwrite) != 0)
      return;
  }

  /* copy the game database itself */
  if (!VFSCopyFile(stringMoveDb,
		   VFSVolume[src->volIdx]->volId, gameName,
		   VFSVolume[dst]->volId,         gameName)) {
    /* try to delete probably defective destination file */
    VFSFileDelete(VFSVolume[dst]->volId, gameName);
    return;
  }

  /* build vfs file name of state database */
  StrPrintF(stateName, PHOINIX_DIR "/" gameStatesDbName SUFFIX_PDB,
	    MemoryMbcInfo.crc);

  /* copy the state database */
  if (!VFSCopyFile(stringMoveDb,
		   VFSVolume[src->volIdx]->volId, stateName,
		   VFSVolume[dst]->volId,         stateName)) {
    /* erase both new destination files */
    VFSFileDelete(VFSVolume[dst]->volId, gameName);
    VFSFileDelete(VFSVolume[dst]->volId, stateName);
    return;
  }

  /* delete original file */
  if ((err = VFSFileDelete(VFSVolume[src->volIdx]->volId, gameName))
      != errNone) {
    VFSError(err, stringMoveDb);
    VFSFileDelete(VFSVolume[dst]->volId, gameName);
    VFSFileDelete(VFSVolume[dst]->volId, stateName);
    return;
  }

  /* delete original file */
  if ((err = VFSFileDelete(VFSVolume[src->volIdx]->volId, stateName))
      != errNone) {
    VFSError(err, stringMoveDb);
    VFSFileDelete(VFSVolume[dst]->volId, gameName);
    VFSFileDelete(VFSVolume[dst]->volId, stateName);
    return;
  }

  /* reflect change in list */
  ManagerListChangeLocation(dst, 0);
}

/**
 * Moves from VFS (external) to Palm RAM (internal).
 *
 * @param src pointer to the entry in the games list.
 * @param dst destination volume number.
 */
static void VFSMove_V2P(ManagerDbListEntryType *src, UInt16 dst) {
  Err     err;
  Char    gameName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  Char    stateName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  UInt16  gameCardNo = 0, stateCardNo = 0;
  LocalID gameDbId, stateDbId;

  /* close state database if open */
  StatesClosePrefsAndStates();

  /* erase existing destination file */
  if ((gameDbId = DmFindDatabase(gameCardNo, src->name)) != 0) {
    if (FrmAlert(alertIdOverwrite) != 0)
      return;

    DmDeleteDatabase(gameCardNo, gameDbId);
  }

  /* build game file name */
  StrPrintF(gameName, PHOINIX_DIR "/%s" SUFFIX_PDB, src->name);

  /* import game database from card */
  MiscShowBusy(true);
  err = VFSImportDatabaseFromFile(VFSVolume[src->volIdx]->volId,
				  gameName, &gameCardNo, &gameDbId);
  MiscShowBusy(false);
  if (err != errNone) {
    VFSError(err, stringImportDb);
    return;
  }

  /* build vfs file name of state database */
  StrPrintF(stateName, gameStatesDbName, MemoryMbcInfo.crc);

  /* erase existing destination file */
  if ((stateDbId = DmFindDatabase(gameCardNo, stateName)) != 0)
    DmDeleteDatabase(gameCardNo, stateDbId);

  /* build vfs file name of state database */
  StrPrintF(stateName, PHOINIX_DIR "/" gameStatesDbName SUFFIX_PDB,
	    MemoryMbcInfo.crc);

  /* import state database from card */
  MiscShowBusy(true);
  err = VFSImportDatabaseFromFile(VFSVolume[src->volIdx]->volId,
				  stateName, &stateCardNo, &stateDbId);
  MiscShowBusy(false);
  if (err != errNone) {
    VFSError(err, stringImportDb);
    DmDeleteDatabase(gameCardNo, gameDbId);
    return;
  }

  /* delete files in VFS */
  if ((err = VFSFileDelete(VFSVolume[src->volIdx]->volId, gameName))
      != errNone)
    VFSError(err, stringImportDb);

  if ((err = VFSFileDelete(VFSVolume[src->volIdx]->volId, stateName))
      != errNone)
    VFSError(err, stringImportDb);

  /* reflect change in list */
  ManagerListChangeLocation(dst, gameCardNo);
}

/**
 * Copies the source file into the destination file.
 *
 * @param  err_msg  index of additional string in case of an error.
 * @param  src_vol  source volume number.
 * @param  src_name source filename.
 * @param  dst_vol  destination volume number.
 * @param  dst_name destination filename.
 * @return true if successful.
 */
static Boolean VFSCopyFile(UInt16 err_msg,
			   UInt16 src_vol, Char *src_name,
			   UInt16 dst_vol, Char *dst_name) {
  FileRef srcFile = -1, dstFile = -1;
  void    *buffer = NULL;
  UInt32  bytesread;
  Err     err = -1;
  Boolean retVal = false;

  MiscShowBusy(true);

  buffer = MemPtrNew(COPY_CHUNK_SIZE);
  if (buffer == NULL)
    goto VFSCopyError;

  /* open source file */
  if ((err = VFSFileOpen(src_vol, src_name, vfsModeRead, &srcFile)) != errNone)
    goto VFSCopyError;

  /* open destination file */
  if ((err = VFSFileOpen(dst_vol, dst_name,
			 vfsModeWrite | vfsModeCreate | vfsModeTruncate,
			 &dstFile)) != errNone)
    goto VFSCopyError;

  /* and now copy the files block by block */
  while ((err = VFSFileEOF(srcFile)) == errNone) {

    if ((err = VFSFileRead(srcFile, COPY_CHUNK_SIZE, buffer, &bytesread))
	!= errNone)
      if (err != vfsErrFileEOF)
        goto VFSCopyError;

    if ((err = VFSFileWrite(dstFile, bytesread, buffer, NULL)) != errNone)
      goto VFSCopyError;
  }

  /* other error than eof? */
  if (err != vfsErrFileEOF)
    goto VFSCopyError;

  /* everything went fine */
  retVal = true;

 VFSCopyError:
  MiscShowBusy(false);

  /* close files and free memory */
  if (srcFile != -1)
    VFSFileClose(srcFile);
  if (dstFile != -1)
    VFSFileClose(dstFile);
  if (buffer != NULL)
    MemPtrFree(buffer);

  /* display error, if there was one */
  if (retVal == false)
    VFSError(err, err_msg);

  return retVal;
}

/* === Support functions for the manager form =========================	*/

/**
 * Fetches the file head in order to calc CRC and get cart infos.
 *
 * @param  game pointer to the entry in the games list.
 * @return handle of the file head record, NULL if error.
 */
MemHandle VFSFetchHead(ManagerDbListEntryType *game) {
  Char      dbName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  FileRef   file;
  MemHandle recH;
  Err       err;

  if (VFSVolume == NULL)
    return NULL;

  /* build complete file name */
  StrPrintF(dbName, PHOINIX_DIR "/%s" SUFFIX_PDB, game->name);

  /* open file */
  err = VFSFileOpen(VFSVolume[game->volIdx]->volId,
		    dbName, vfsModeRead, &file);
  if (err != errNone) {
    if (err != vfsErrFileNotFound)
      VFSError(err, stringFetchHead);
    return NULL;
  }

  /* get first record */
#if SHOW_BUSY_FOR_CRC
  MiscShowBusy(true);
#endif
  err = VFSFileDBGetRecord(file, 0, &recH, NULL, NULL);
#if SHOW_BUSY_FOR_CRC
  MiscShowBusy(false);
#endif
  if (err != errNone) {
    VFSError(err, stringFetchHead);
    recH = NULL;
  }

  VFSFileClose(file);

  return recH;
}

/**
 * Import games from the launcher's directory if present.
 *
 * @param vol volume number.
 */
static void VFSImportFromLauncher(UInt16 vol) {
  Err          err;
  UInt32       it;
  FileRef      dir;
  FileInfoType info;
  Char         dbName[4 + dmDBNameLength];    /* {dmDBNameLength}.pdb */
  Char         cur_name[20 + dmDBNameLength]; /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  Char         new_name[20 + dmDBNameLength]; /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  FileRef      file;
  UInt32       type, creator;

  /* open the directory to scan, no real error reporting here, since */
  /* the directory will be scanned again and errors will be reported */
  /* then */
  if ((err=VFSFileOpen(vol, LAUNCHER_DIR, vfsModeRead, &dir)) == errNone) {
    it = vfsIteratorStart;
    info.nameBufLen = 4 + dmDBNameLength;
    info.nameP = dbName;

    while (it != vfsIteratorStop) {
      if (VFSDirEntryEnumerate(dir, &it, &info) != errNone)
	break;

      /* is this a .pdb or .PDB name? */
      if (StrCaselessCompare(SUFFIX_PDB, &info.nameP[StrLen(info.nameP) - 4]) == 0) {
	/* attach file name to path */
	StrPrintF(cur_name, LAUNCHER_DIR "/%s", info.nameP);
	StrPrintF(new_name, PHOINIX_DIR  "/%s", info.nameP);

	/* check whether this really is a game database */
	if (VFSFileOpen(vol, cur_name, vfsModeRead, &file) == errNone) {
	  Err err;
	  /* fetch name, type and creator */
	  err = VFSFileDBInfo(file, NULL, NULL, NULL, NULL,
			      NULL, NULL, NULL, NULL, NULL,
			      &type, &creator, NULL);
	  VFSFileClose(file);
	  if (err == errNone) {
	    /* verify creator and type */
	    if ((creator == miscCreator) && (type == gameDbType)) {
	      /* verify that phoinix dir exists */
	      if (VFSCheckDir(vol)) {
		/* ok, move this file */
		if (!VFSCopyFile(-1, vol, cur_name, vol, new_name)) {
		  /* copying failed, try to delete new file */
		  VFSFileDelete(vol, new_name);
		} else {
		  /* copying successful, try to delete old file */
		  VFSFileDelete(vol, cur_name);
		  it = vfsIteratorStart;
		}
	      }
	    }
	  }
	}
      }
    }
    /* finally close the directory */
    VFSFileClose(dir);
  }
}

/* === Scanning VFS volumes ===========================================	*/

/**
 * Checks a file name for correctness and duplicate.
 *
 * @param  vol   volume number.
 * @param  name1 pointer to buffer for database generated name.
 * @param  name2 pointer to buffer for actual file name.
 * @return true if no error and name is OK.
 */
static Boolean VFSGetMissnamed(UInt16 vol, char *name1, char *name2) {
  Err          err;
  UInt32       it;
  FileRef      dir;
  FileInfoType info;
  Char         dbName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  FileRef      file;
  UInt32       type, creator;

  /* open the directory to scan, no real error reporting here, since */
  /* the directory will be scanned again and errors will be reported */
  /* then */
  if ((err=VFSFileOpen(vol, PHOINIX_DIR, vfsModeRead, &dir)) == errNone) {
    it = vfsIteratorStart;
    info.nameBufLen = 20 + dmDBNameLength;
    info.nameP = dbName;

    while (it != vfsIteratorStop) {
      if (VFSDirEntryEnumerate(dir, &it, &info) != errNone)
	break;

      /* is this a .pdb or .PDB name? */
      if (StrCaselessCompare(SUFFIX_PDB, &info.nameP[StrLen(info.nameP) - 4]) == 0) {
	/* attach file name to path */
	StrCopy(name2, PHOINIX_DIR "/");
	StrCat(name2, info.nameP);

	/* check whether this really is a game database */
	if (VFSFileOpen(vol, name2, vfsModeRead, &file) == errNone) {
	  /* fetch name, type and creator */
	  if (VFSFileDBInfo(file, name2, NULL, NULL, NULL,
			    NULL, NULL, NULL, NULL, NULL,
			    &type, &creator, NULL) == errNone) {
	    /* verify creator and type */
	    if ((creator == miscCreator) && (type == gameDbType)) {
	      /* check whether filename and internal database */
	      /* name match and ignore the file if they */
	      /* do not match */
	      StrCat(name2, SUFFIX_PDB);
	      if (StrCompare(name2, dbName) != 0) {
		/* return internal and real file name */
		StrPrintF(name1, PHOINIX_DIR "/%s", info.nameP );
		VFSFileClose(file);
		VFSFileClose(dir);
		return true;
	      }
	    }
	  }
	  /* and close the database again */
	  VFSFileClose(file);
	}
      }
    }
    /* finally close the directory */
    VFSFileClose(dir);
  }
  return false;
}

/**
 * Checks for miss-named files and files in the launcher dir.
 *
 * @param  vol volume number.
 */
static void VFSPreScanVolume(UInt16 vol) {
  Char name1[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  Char name2[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  Err  err;

  /* import games from the launcher dir */
  VFSImportFromLauncher(vol);

  /* rename miss named games */
  while (VFSGetMissnamed(vol, name1, name2)) {
    if ((err = VFSFileRename(vol, name1, name2)) != errNone) {
      VFSFileDelete(vol, name1);
      VFSError(err, stringScanForGames);
    }
  }
}

/**
 * Scans volumes for game databases.
 * The busy form is shown by the caller.
 *
 * @param  ListDbP pointer to database of games list.
 * @return true if successful.
 */
Boolean VFSScanForGames(DmOpenRef ListDbP) {
  UInt16       volume;
  FileRef      dir;
  Err          err;
  UInt32       it;
  FileInfoType info;
  Char         dbName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  Char         fileName[20 + dmDBNameLength];
  FileRef      file;
  UInt32       type, creator;

  /* no volumes installed, just return */
  if (VFSVolume == NULL)
    return true;

  /* scan all volumes */
  for (volume = 1; volume < VFSPresent; volume++) {
    /* check for miss-named files and files in the launcher dir */
    VFSPreScanVolume(VFSVolume[volume]->volId);

    /* open the directory to scan */
    if ((err=VFSFileOpen(VFSVolume[volume]->volId, PHOINIX_DIR,
			 vfsModeRead, &dir)) != errNone) {
      /* ignore file not found */
      if (err != vfsErrFileNotFound)
	VFSError(err, stringScanForGames);
    } else {
      /* finally scan for game databases */
      it = vfsIteratorStart;
      info.nameBufLen = 20 + dmDBNameLength;
      info.nameP = dbName;

      while (it != vfsIteratorStop) {
	if (VFSDirEntryEnumerate(dir, &it, &info) != errNone)
	  break;

	/* is this a .pdb name? */
	if (StrCompare(SUFFIX_PDB, &info.nameP[StrLen(info.nameP) - 4]) == 0) {
	  /* attach file name to path */
	  StrCopy(fileName, PHOINIX_DIR "/");
	  StrCat(fileName, info.nameP);

	  /* check whether this really is a game database */
	  if (VFSFileOpen(VFSVolume[volume]->volId, fileName,
			  vfsModeRead, &file) == errNone) {
	    /* fetch name, type and creator */
	    if (VFSFileDBInfo(file, fileName, NULL, NULL, NULL,
			      NULL, NULL, NULL, NULL, NULL,
			      &type, &creator, NULL) == errNone) {
	      /* verify creator and type */
	      if ((creator == miscCreator) && (type == gameDbType)) {
		/* cut off .pdb and check whether int. and ext. name match */
		dbName[StrLen(dbName) - 4] = 0;
		if (StrCompare(fileName, dbName) == 0) {
		  /* ok, add this file to the list */
		  if (!ManagerListAdd(ListDbP, fileName, 0, volume)) {
		    VFSFileClose(file);
		    VFSFileClose(dir);
		    return false;
		  }
		}
	      }
	    }

	    /* and close the database again */
	    VFSFileClose(file);
	  }
	}
      }
      /* finally close the directory */
      VFSFileClose(dir);
    }
  }
  return true;
}

/* === Load and unload games ==========================================	*/

/**
 * Loads a game file as database into Palm RAM.
 *
 * @param  game pointer to the games list entry.
 * @param  card pointer to card number of generated database.
 * @param  id   pointer to local ID of generated database.
 * @return true if successful.
 */
Boolean VFSGameLoad(ManagerDbListEntryType *game, UInt16 *card, LocalID *id) {
  Err  err;
  Char dbName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */

  /* try to import the game from the memory card */
  if (VFSVolume == NULL)
    return false;

  /* build file name */
  StrPrintF(dbName, PHOINIX_DIR "/%s" SUFFIX_PDB, game->name);

  /* copy game database to card */
  MiscShowBusy(true);
  err = VFSImportDatabaseFromFile(VFSVolume[game->volIdx]->volId,
				  dbName, card, id);
  MiscShowBusy(false);
  if (err != errNone) {
    /* don't complain/bail out of database already exists */
    if (err != dmErrAlreadyExists) {
      VFSError(err, stringImportDb);
      return false;
    }
  }

  VFSGameLoaded = game;
  return true;
}

/**
 * Unloads the current game, i.e. delete the copy in Palm RAM.
 *
 * @return true if successful.
 */
Boolean VFSGameUnload(void) {
  LocalID dbIdGame;

  if (VFSVolume == NULL)
    return false;

  if (VFSGameLoaded == NULL)
    return false;

  /* just delete the local copy of the game */
  /* get local index of state database */
  if ((dbIdGame = DmFindDatabase(VFSGameLoaded->cardNo,
				 VFSGameLoaded->name)) != 0)
    DmDeleteDatabase(VFSGameLoaded->cardNo, dbIdGame);

  VFSGameLoaded = NULL;
  return true;
}

/**
 * Loads a game state as database into Palm RAM.
 *
 * @param  game pointer to the games list entry.
 * @param  card pointer to card number of generated database.
 * @param  id   pointer to local ID of generated database.
 * @return true if successful.
 */
Boolean VFSStateLoad(ManagerDbListEntryType *game, UInt16 *card, LocalID *id) {
  Err     err;
  Char    dbName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  LocalID dbId;

  if (MemoryMbcInfo.crc == 0)
    return false;

  if (VFSVolume == NULL)
    return false;

  /* delete locally stored database before trying to open vfs */
  StrPrintF(dbName, gameStatesDbName, MemoryMbcInfo.crc);

  dbId = DmFindDatabase(currentStateDbCardNo, dbName);
  if (dbId != 0)
    DmDeleteDatabase(currentStateDbCardNo, dbId);

  /* build name of state database */
  StrPrintF(dbName, PHOINIX_DIR "/" gameStatesDbName SUFFIX_PDB,
	    MemoryMbcInfo.crc);

  /* copy state database from card */
#if SHOW_BUSY_FOR_STATE_LOAD
  MiscShowBusy(true);
#endif
  err = VFSImportDatabaseFromFile(VFSVolume[game->volIdx]->volId,
				  dbName, card, id);
#if SHOW_BUSY_FOR_STATE_LOAD
  MiscShowBusy(false);
#endif
  if (err != errNone) {
    if ((err != vfsErrFileNotFound)&&(err != dmErrAlreadyExists)) {
      VFSError(err, stringImportDb);
      return false;
    }
  }

  /* remember that this is the state currently loaded */
  VFSStateLoadedCardNo = game->cardNo;
  VFSStateLoadedVolIdx = game->volIdx;
  VFSStateLoadedCRC    = MemoryMbcInfo.crc;

  /* report missing file */
  if (err == vfsErrFileNotFound)
    return false;

  return true;
}

/**
 * Unloads the current state, i.e. move state back into VFS and delete the copy.
 *
 * @return true if successful.
 */
Boolean VFSStateUnload(void) {
  Err     err;
  Char    dbName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  LocalID dbIdState;

  if (VFSVolume == NULL)
    return false;

  if (VFSStateLoadedVolIdx == 0)
    return false;

  /* check if phoinix dir exists */
  if (!VFSCheckDir(VFSVolume[VFSStateLoadedVolIdx]->volId)) {
    return false;
  }

  /* build vfs file name of state database */
  StrPrintF(dbName, gameStatesDbName, VFSStateLoadedCRC);

  /* get local index of state database */
  if ((dbIdState = DmFindDatabase(VFSStateLoadedCardNo, dbName)) != 0) {
    /* build vfs file name of state database */
    StrPrintF(dbName, PHOINIX_DIR "/" gameStatesDbName SUFFIX_PDB,
	      VFSStateLoadedCRC);

    /* delete file, if already existant */
    if ((err = VFSFileDelete(VFSVolume[VFSStateLoadedVolIdx]->volId, dbName))
	!= errNone) {
      if ((err != vfsErrFileNotFound)&&(err != vfsErrFileGeneric))
	VFSError(err, stringExportDb);
    }

    /* copy game database to card */
#if SHOW_BUSY_FOR_STATE_UNLOAD
    MiscShowBusy(true);
#endif
    err = VFSExportDatabaseToFile(VFSVolume[VFSStateLoadedVolIdx]->volId,
				  dbName, VFSStateLoadedCardNo, dbIdState);
#if SHOW_BUSY_FOR_STATE_UNLOAD
    MiscShowBusy(false);
#endif
    if (err != errNone) {
      VFSError(err, stringExportDb);
      VFSStateLoadedVolIdx = 0;
      return false;
    }

    /* everything seems to be fine, remove state database from handheld */
    DmDeleteDatabase(VFSStateLoadedCardNo, dbIdState);
  }

  VFSStateLoadedVolIdx = 0;
  return true;
}

/**
 * Rename the given game with the new name.
 *
 * @param  game pointer to the games list entry.
 * @param  name pointer to the new name.
 * @return true if successful.
 */
Boolean VFSRenameGame(ManagerDbListEntryType *game, Char *name) {
  Char    dbName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  Char    newName[6 + dmDBNameLength];  /* {dmDBNameLength}.pdb */
  FileRef file;
  Err     err;

  if (VFSVolume == NULL)
    return false;

  MiscShowBusy(true);

  /* build file names */
  StrPrintF(dbName, PHOINIX_DIR "/%s" SUFFIX_PDB, game->name);
  StrPrintF(newName, "%s" SUFFIX_PDB, name);

  /* try to rename, first the database, then the file */
  err = VFSFileOpen(VFSVolume[game->volIdx]->volId, dbName, vfsModeReadWrite, &file);
  if (err == errNone) {
    err = VFSFileSeek(file, vfsOriginBeginning, OffsetOf(DatabaseHdrType, name));
    if (err == errNone) {
      err = VFSFileWrite(file, dmDBNameLength, name, NULL);
    }
    VFSFileClose(file);
  }
  if (err == errNone) {
    err = VFSFileRename(VFSVolume[game->volIdx]->volId, dbName, newName);
  }

  MiscShowBusy(false);

  if (err != errNone) {
/* Currently this message doesn't exist. Because I'm planning to switch to
 * PQAs, I don't feel the need to correct this :-/
 */
/*    VFSError(err, stringRenameDb); */
    VFSError(err, stringMoveDb);
    return false;
  }

  return true;
}

/**
 * Delete the given game from the VFS.
 *
 * @param  game pointer to the games list entry.
 * @return true if successful.
 */
Boolean VFSDeleteGame(ManagerDbListEntryType *game) {
  Char dbName[20 + dmDBNameLength];  /* /PALM/Phoinix/{dmDBNameLength}.pdb */
  Err  err;

  if (VFSVolume == NULL)
    return false;

  /* build file name */
  StrPrintF(dbName, PHOINIX_DIR "/%s" SUFFIX_PDB, game->name);
  if ((err = VFSFileDelete(VFSVolume[game->volIdx]->volId, dbName))
      != errNone) {
    VFSError(err, stringDeleteDb);
    return false;
  }

  /* build vfs file name of state database */
  StrPrintF(dbName, PHOINIX_DIR "/" gameStatesDbName SUFFIX_PDB,
	    MemoryMbcInfo.crc);
  if ((err = VFSFileDelete(VFSVolume[game->volIdx]->volId, dbName))
      != errNone) {
    VFSError(err, stringDeleteDb);
    return false;
  }
  return true;
}

/* === Local support functions ========================================	*/

/**
 * Collects all volume names.
 */
static void VFSScanFS(void) {
  UInt32 it;
  UInt16 vn, cnt = 0;
  char   *vol = (char*)VFSVolume;

  /* create dummy entry for palm itself */
  if (VFSVolume != NULL) {
    VFSVolume[cnt]= (VFSVolumeType*)(vol + sizeof(VFSVolumeType*) * VFSPresent);
    VFSVolume[cnt]->volId = VFS_VOLUME_PALM;
    StrCopy(VFSVolume[cnt]->volName, "Palm RAM");
  }

  /* scan for installed volumes */
  it = vfsIteratorStart;
  while (it != vfsIteratorStop) {
    if (VFSVolumeEnumerate(&vn, &it) != errNone)
      break;

    /* increase volume counter */
    cnt++;

    /* get and save volume name if buffer allocated */
    if (VFSVolume != NULL) {
      VFSVolume[cnt]= (VFSVolumeType*)(vol +
				       sizeof(VFSVolumeType*) * VFSPresent +
				       sizeof(VFSVolumeType) * cnt);

      /* save volume reference */
      VFSVolume[cnt]->volId = vn;

      /* get volumes name */
      VFSVolumeGetLabel(vn, VFSVolume[cnt]->volName, 15);

      /* replace empty name */
      if (StrLen(VFSVolume[cnt]->volName) == 0)
	StrPrintF(VFSVolume[cnt]->volName, "Volume %d", cnt);
    }
  }

  /* init volume counter */
  if (VFSVolume == NULL)
    VFSPresent = cnt + 1;
}

/* === The end ========================================================	*/
