/************************************************************************
 *  Phoinix.h
 *                      Constants for the user interface
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
 *  $Log: Phoinix.h,v $
 *  Revision 1.22  2007/07/01 10:12:51  bodowenzel
 *  Cleanup
 *
 *  Revision 1.21  2007/02/03 14:42:18  bodowenzel
 *  New info control for wall clock
 *  Multiple saved state feature implemented
 *
 *  Revision 1.20  2005/04/03 14:08:33  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.19  2005/02/27 16:37:51  bodowenzel
 *  Enhanced user interface
 *
 *  Revision 1.18  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.17  2005/01/30 19:35:26  bodowenzel
 *  Removed support for VFS mount and unmount
 *
 *  Revision 1.16  2005/01/28 17:35:13  bodowenzel
 *  Manager form uses a scrollable table now for the list of games
 *
 *  Revision 1.15  2004/12/28 13:58:10  bodowenzel
 *  Menu command to press all keys
 *  Support for devices without direct screen access
 *
 *  Revision 1.14  2004/12/02 20:01:22  bodowenzel
 *  New configurable screen layout
 *
 *  Revision 1.13  2004/10/24 09:11:05  bodowenzel
 *  New button mapping
 *
 *  Revision 1.12  2004/06/20 14:23:20  bodowenzel
 *  VFS mount and unmount support
 *
 *  Revision 1.11  2004/06/11 16:14:19  bodowenzel
 *  Enhanced error reporting with MiscPostError()
 *
 *  Revision 1.10  2004/03/02 19:24:16  bodowenzel
 *  Changed to new error function MiscShowError()
 *
 *  Revision 1.9  2004/01/11 19:07:14  bodowenzel
 *  Corrected copyright
 *
 *  Revision 1.8  2003/04/27 09:36:18  bodowenzel
 *  Added Lite edition
 *
 *  Revision 1.7  2002/12/07 08:57:27  bodowenzel
 *  Tweaked button mapping form like proposed by HJ
 *
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

/*************************************************************************
 * Global config for varios devices
 *************************************************************************/
//#define handera
//#define alphasmart
//#define sonyclie
//#define fossil
//#define palmgamepad
//#define vfs			// not implemented
//*************************************************************************



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

#define firstUserEvent 0x6000

/* === Application specific ===========================================	*/

/* PilRC can't handle enums, even if its values aren't necessary */
#define phoinixErrorEvent   (firstUserEvent + 0)

/* === Game Manager ===================================================	*/

#define formIdManager          (1000)
#define tableIdManagerList     (1011)
#define tableManagerHeight     (6)
#define labelIdManagerNoGames  (1012)
#define scrollbarIdManagerList (1021)
#define listIdManagerStates    (1031)
#define buttonIdManagerDelete  (1032)
#define buttonIdManagerGo      (1041)
#define labelIdManagerGo       (1042)

#define menuIdManager                     (1100)
#define menuIdManagerGameDelete           (1111)
#define menuIdManagerGameDetails          (1112)
#define menuIdManagerOptionsEmulation     (1121)
#define menuIdManagerOptionsButtonMapping (1122)
#define menuIdManagerOptionsScreenLayout  (1123)
#define menuIdManagerOptionsTips          (1124)
#define menuIdManagerOptionsAbout         (1125)

#define helpIdManager (1001)

#define alertIdManagerNoGames (1500)
#define helpIdManagerNoGames  (1501)

#define alertIdManagerDeleteGame (1600)

/* === Emulation ======================================================	*/

/* PilRC can't handle enum's */
#define offsetEmulationNone    (0)
#define offsetEmulationMin     (1) /* first */
#define offsetEmulationPause   (1)
#define offsetEmulationBoost   (2)
#define offsetEmulationSelect  (3)
#define offsetEmulationStart   (4)
#define offsetEmulationB       (5)
#define offsetEmulationA       (6)
#define offsetEmulationTime    (7)
#define offsetEmulationBattery (8)
#define offsetEmulationPace    (9)
#define offsetEmulationClock   (10)
#define offsetEmulationMax     (11) /* last + 1 */

#define formIdEmulation          (2000)
#define ctlIdEmulation           (2011) /* used +1 .. +9 */
#define checkboxIdEmulationPause (ctlIdEmulation + offsetEmulationPause)
#define checkboxIdEmulationBoost (ctlIdEmulation + offsetEmulationBoost)
#define buttonIdEmulationSelect  (ctlIdEmulation + offsetEmulationSelect)
#define buttonIdEmulationStart   (ctlIdEmulation + offsetEmulationStart)
#define buttonIdEmulationB       (ctlIdEmulation + offsetEmulationB)
#define buttonIdEmulationA       (ctlIdEmulation + offsetEmulationA)
#define buttonIdEmulationTime    (ctlIdEmulation + offsetEmulationTime)
#define buttonIdEmulationBattery (ctlIdEmulation + offsetEmulationBattery)
#define buttonIdEmulationPace    (ctlIdEmulation + offsetEmulationPace)
#define buttonIdEmulationClock   (ctlIdEmulation + offsetEmulationClock)
#define ctlOffsetEmulation       (40)

#define menuIdEmulation                     (2100)
#define menuIdEmulationGamePressAllKeys     (2111)
#define menuIdEmulationGamePause            (2112)
#define menuIdEmulationGameBoost            (2113)
#define menuIdEmulationGameReset            (2114)
#define menuIdEmulationGameQuit             (2115)
#define menuIdEmulationStateSave            (2121)
#define menuIdEmulationStateLoad            (2122)
#define menuIdEmulationOptionsEmulation     (2131)
#define menuIdEmulationOptionsButtonMapping (2132)
#define menuIdEmulationOptionsScreenLayout  (2133)
#define menuIdEmulationOptionsTips          (2134)
#define menuIdEmulationOptionsAbout         (2135)

#define helpIdEmulation (2001)

#define formIdLoadState   (2200)
#define listIdLoadState   (2211)
#define buttonIdLoadState (2221)
#define labelIdLoadState  (2222)

#define helpIdLoadState (2201)

/* === Game Details ===================================================	*/

#define formIdGameDetails          (5100)
#define fieldIdGameDetailsName     (5111)
#define popupIdGameDetailsLocation (5121)
#define listIdGameDetailsLocation  (5122)
#define labelIdGameDetailsCode     (5131)
#define labelIdGameDetailsType     (5141)
#define labelIdGameDetailsSize     (5151)
#define buttonIdGameDetailsOk      (5161)
#define buttonIdGameDetailsCancel  (5162)

#define helpIdGameDetails (5101)

#define stblIdGameDetailsType (5151)

/* === Emulation Options ==============================================	*/

#define formIdEmulationOptions              (5200)
#define checkboxIdEmulationOptionsSaveState (5211)
#define labelIdEmulationOptionsMaxFs        (5221)
#define buttonIdEmulationOptionsMaxFsUp     (5222)
#define buttonIdEmulationOptionsMaxFsDown   (5223)
#define labelIdEmulationOptionsBoost        (5231)
#define buttonIdEmulationOptionsBoostUp     (5232)
#define buttonIdEmulationOptionsBoostDown   (5233)
#define buttonIdEmulationOptionsOk          (5241)
#define buttonIdEmulationOptionsCancel      (5242)

#define helpIdEmulationOptions (5201)

/* === Button Mapping =================================================	*/

/* PilRC can't handle enum's */
#define offsetButtonMappingUp     (0)
#define offsetButtonMappingDown   (1)
#define offsetButtonMappingLeft   (2)
#define offsetButtonMappingRight  (3)
#define offsetButtonMappingSelect (4)
#define offsetButtonMappingStart  (5)
#define offsetButtonMappingB      (6)
#define offsetButtonMappingA      (7)

#define formIdButtonMapping                   (5300)
#define checkboxIdButtonMappingHoldVertical   (5311)
#define checkboxIdButtonMappingHoldHorizontal (5312)
#define selIdButtonMapping                    (5321) /* used +0 .. +7 */
/* PilRC can't handle line-splitted #define's, these lines are loooong */
#define selIdButtonMappingUp     (selIdButtonMapping + offsetButtonMappingUp)
#define selIdButtonMappingDown   (selIdButtonMapping + offsetButtonMappingDown)
#define selIdButtonMappingLeft   (selIdButtonMapping + offsetButtonMappingLeft)
#define selIdButtonMappingRight  (selIdButtonMapping + offsetButtonMappingRight)
#define selIdButtonMappingSelect (selIdButtonMapping + offsetButtonMappingSelect)
#define selIdButtonMappingStart  (selIdButtonMapping + offsetButtonMappingStart)
#define selIdButtonMappingB      (selIdButtonMapping + offsetButtonMappingB)
#define selIdButtonMappingA      (selIdButtonMapping + offsetButtonMappingA)
#define buttonIdButtonMappingOk               (5331)
#define buttonIdButtonMappingCancel           (5332)

#define helpIdButtonMapping (5301)

#define formIdButtonMappingSample         (5350)
#define buttonIdButtonMappingSampleOk     (5361)
#define buttonIdButtonMappingSampleCancel (5362)

#define helpIdButtonMappingSample (5351)

/* === Screen Layout ==================================================	*/

#define formIdScreenLayout         (5400)
#define popupIdScreenLayout        (5411) /* used +0 .. +3 */
#define listIdScreenLayout         (5421)
#define listScreenLayoutOffset     (8)
#define buttonIdScreenLayoutOk     (5431)
#define buttonIdScreenLayoutCancel (5432)

#define helpIdScreenLayout (5401)

/* === About ==========================================================	*/

#define formIdAbout     (5500)
#define bmpIdAbout      (5511)
#define buttonIdAboutOk (5521)

#define helpIdAbout (5501)

/* === VFS ============================================================	*/

#define alertIdOverwrite (8100)

/* === Miscellaneous ==================================================	*/

#define formIdBusy (8200)
#define bmpIdBusy  (8211)

#define alertIdError (8300)
#define helpIdError  (8301)
#define stblIdError  (8302)

/* === The end ========================================================	*/
