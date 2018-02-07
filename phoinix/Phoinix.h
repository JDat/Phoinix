/************************************************************************
 *  Phoinix.h
 *                      Constants for the user interface
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
 *  Revision 1.6  2002/11/02 15:53:29  bodowenzel
 *  Reduced number of button bitmaps to one
 *
 *  Revision 1.5  2002/10/23 16:34:35  bodowenzel
 *  Busy form centralized
 *
 *  Revision 1.4  2002/10/19 08:08:10  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.3  2002/05/10 15:17:39  bodowenzel
 *  Till's VFS contribution
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:39:05  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-16  Bodo Wenzel   Saving states now possible
 *  2001-07-23  Bodo Wenzel   For executable v.4.0, changed to C
 *  2000-09-30  Bodo Wenzel   For executable v.2.1
 *  2000-05-07  Bodo Wenzel   For executable v.1.0
 *  2000-04-22  Bodo Wenzel   For executable v.0.3 as resource.inc
 ************************************************************************
 */

/* === PilRC is a bit limited... ======================================	*/

/* from PalmOS.h, checked when included by the C compiler: */

#define sysEditMenuID           10000
#define sysEditMenuUndoCmd      10000
#define sysEditMenuCutCmd       10001
#define sysEditMenuCopyCmd      10002
#define sysEditMenuPasteCmd     10003
#define sysEditMenuSelectAllCmd 10004
#define sysEditMenuSeparator    10005
#define sysEditMenuKeyboardCmd  10006
#define sysEditMenuGraffitiCmd  10007

#define dmDBNameLength 32

#define fntAppFontCustomBase 128

/* === Application specific ===========================================	*/

#define alertIdStartupIncompatibleRom    (1001)
#define alertIdStartupIncompatibleScreen (1002)

#define formIdAbout     (1100)
#define bmpIdAbout      (1111)
#define buttonIdAboutOk (1121)

#define helpIdAbout (1101)

#define fontIdButtonMapping (1200)
#define fontButtonMapping   (fntAppFontCustomBase+1)

/* NOTE: the increment between listId* and popupId* must be 10! */
#define formIdButtonMapping         (1200)
#define bmpIdButtonMapping1         (1211)
#define bmpIdButtonMapping2         (1212)
#define popupIdButtonMappingDb      (1213)
#define popupIdButtonMappingAb      (1223)
#define popupIdButtonMappingUp      (1233)
#define popupIdButtonMappingDn      (1243)
#define popupIdButtonMappingTd      (1253)
#define popupIdButtonMappingMm      (1263)
#define listIdButtonMappingDb       (1214)
#define listIdButtonMappingAb       (1224)
#define listIdButtonMappingUp       (1234)
#define listIdButtonMappingDn       (1244)
#define listIdButtonMappingTd       (1254)
#define listIdButtonMappingMm       (1264)
#define buttonIdButtonMappingOk     (1271)
#define buttonIdButtonMappingCancel (1272)

#define helpIdButtonMapping (1201)

#define formIdNoGame (1300)

#define menuIdNoGame             (1310)
#define menuIdNoGameOptionsTips  (1321)
#define menuIdNoGameOptionsAbout (1322)

#define helpIdNoGame (1301)

#define alertIdManagerListError      (1401)
#define alertIdManagerListIncomplete (1402)

#define formIdManager          (1500)
#define popupIdManagerName     (1521)
#define popupManagerNameHeight (6)
#define listIdManagerName      (1522)
#define fieldIdManagerName     (1523)
#define popupIdManagerLocation (1538)
#define listIdManagerLocation  (1539)
#define labelIdManagerLocation (1540)
#define labelIdManagerType     (1541)
#define labelIdManagerRom      (1542)
#define labelIdManagerRam      (1543)
#define buttonIdManagerGo      (1562)

#define menuIdManager                     (1570)
#define menuIdManagerActionsDeleteGame    (1584)
#define menuIdManagerOptionsButtonMapping (1591)
#define menuIdManagerOptionsTips          (1593)
#define menuIdManagerOptionsAbout         (1594)

#define helpIdManager (1501)

#define alertIdManagerDeleteGame  (1611)
#define alertIdManagerDeleteError (1612)
#define alertIdManagerRenameError (1613)
#define alertIdManagerCantRun     (1614)

#define stringIdManagerGameError   (1621)
#define stringIdManagerGameUnknown (1622)
#define stringIdManagerGameKBytes  (1623)
#define stringIdManagerGameMbc2    (1624)

#define alertIdStatesLoadError (1701)
#define alertIdStatesSaveError (1702)

#define formIdEmulation         (1800)
#define buttonIdEmulationSelect (1811)
#define buttonIdEmulationStart  (1812)
#define popupIdEmulationInfo    (1821)
#define listIdEmulationInfo     (1822)
#define checkboxIdEmulationRun  (1831)

#define formIdEmulationScreen  (1890)
#define gadgetIdEmulationSmall (1891)
#define gadgetIdEmulationH330  (1892)

#define menuIdEmulation                     (1840)
#define menuIdEmulationGameQuit             (1841)
#define menuIdEmulationGameReset            (1842)
#define menuIdEmulationOptionsButtonMapping (1851)
#define menuIdEmulationOptionsTips          (1852)
#define menuIdEmulationOptionsAbout         (1853)

#define helpIdEmulation (1801)

#define alertIdEmulationSetupError (1861)
#define alertIdEmulationRunError   (1862)

#define alertIdVFS                       (1970)
#define stringerrNone                    (1971)
#define stringvfsErrBufferOverflow       (1972)
#define stringvfsErrFileGeneric          (1973)
#define stringvfsErrFileBadRef           (1974)
#define stringvfsErrFileStillOpen        (1975)
#define stringvfsErrFilePermissionDenied (1976)
#define stringvfsErrFileAlreadyExists    (1977)
#define stringvfsErrFileEOF              (1978)
#define stringvfsErrFileNotFound         (1979)
#define stringvfsErrVolumeBadRef         (1980)
#define stringvfsErrVolumeStillMounted   (1981)
#define stringvfsErrNoFileSystem         (1982)
#define stringvfsErrBadData              (1983)
#define stringvfsErrDirNotEmpty          (1984)
#define stringvfsErrBadName              (1985)
#define stringvfsErrVolumeFull           (1986)
#define stringvfsErrUnimplemented        (1987)
#define stringvfsErrNotADirectory        (1988)
#define stringvfsErrIsADirectory         (1989)
#define stringvfsErrDirectoryNotFound    (1990)
#define stringvfsErrNameShortened        (1991)
#define stringdmErrAlreadyExists         (1992)
#define stringvfsErrUnknown              (1993)

/* vfs Phoinix internal error messages */
#define stringCreateDir                  (2000)
#define stringExportDb                   (2001)
#define stringFetchHead                  (2002)
#define stringScanForGames               (2003)
#define stringImportDb                   (2004)
#define stringMoveDb                     (2005)
#define stringDeleteDb                   (2006)

#define alertIdOverwrite                 (2007)

#define alertIdGeneric                   (2009)

#define formIdBusy (5000)
#define bmpIdBusy  (5001)

/* === The end ========================================================	*/
