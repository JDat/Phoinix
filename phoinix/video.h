/************************************************************************
 *  video.h
 *                      Video emulation routines
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
 *  $Log: video.h,v $
 *  Revision 1.13  2006/10/23 15:39:27  bodowenzel
 *  Clearing the screen at STOP corrected
 *
 *  Revision 1.12  2005/10/11 14:48:05  bodowenzel
 *  Centralized device type into MiscDevice
 *
 *  Revision 1.11  2005/05/06 10:32:36  bodowenzel
 *  Saving full resolution screen in state
 *
 *  Revision 1.10  2005/04/03 14:08:34  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.9  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.8  2005/01/29 10:25:37  bodowenzel
 *  Added const qualifier to pointer parameters
 *
 *  Revision 1.7  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.6  2004/01/11 19:07:14  bodowenzel
 *  Corrected copyright
 *
 *  Revision 1.5  2003/04/19 13:18:18  bodowenzel
 *  Screen copy for OS5 via WinDrawBitmap()
 *
 *  Revision 1.4  2003/04/12 15:24:02  bodowenzel
 *  Removed useless bitmap header in saved state
 *
 *  Revision 1.3  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:31  bodowenzel
 *  Import
 *
 ************************************************************************
 */

#ifndef VIDEO_H
#define VIDEO_H

/* === Includes =======================================================	*/

#include "sections.h"

/* === Constants ======================================================	*/

#define videoScreenWidth  (160) /* size of GB screen in pixel */
#define videoScreenHeight (144)

#define videoBytesPerBuffer (videoScreenHeight * \
                             videoScreenWidth / 8 * videoBitsPerPixel)

#define videoMapWrap (32) /* number of tiles hor./ver. */

#define videoFramesPerSec (60) /* frame rate of the original */

#define videoClksPerLine (456) /* ratio = hor. freq. to DIV freq. */

#define videoBitsPerPixel (2) /* 4 levels of gray */

/* === Type definitions ===============================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

/* types of variables of a state to be saved */

typedef struct {
  UInt16 timerDiv;
  UInt16 timerTima;
  UInt8  videoBuffer[videoBytesPerBuffer];
} VideoStateType;

#endif

/* === Global variables ===============================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

extern void    *VideoDisplayPtr;
extern UInt16  VideoDisplayWidth;

extern RectangleType VideoGbScreenBounds;

extern UInt16 VideoCycles; /* opcodes until next LCD line */
extern UInt16 VideoFrames;

extern Boolean VideoBoosted;
extern UInt16  VideoFramesToSkip; /* 0 if frame has to be rendered */

extern UInt16 VideoTimerDiv;       /* fix-real for DIV */
extern UInt16 VideoTimerTima;      /* fix-real for TIMA */
extern UInt16 VideoTimerDeltaTima; /* increment for TIMA */

extern Int16 VideoLyPrv; /* private line counter */

extern UInt16 *VideoMapIndices; /* pointer to miscellaneous arrays */
extern void   *VideoBgUsed;
extern void   *VideoBgTile;
extern void   *VideoWiUsed;
extern void   *VideoWiTile;
extern void   *VideoObFirst;
extern void   *VideoObTiles;

extern void *VideoBufferPtr; /* pointer to screen buffer */

extern void   (*VideoObColor0H)(void); /* addresses of color functions */
extern void   (*VideoObColor0L)(void);
extern void   (*VideoObColor1H)(void);
extern void   (*VideoObColor1L)(void);
extern UInt16 VideoObCount;          /* prehandled height */
extern UInt16 VideoObTilemask;

extern const UInt16 VideoGrayCopyTable[];

extern UInt8 VideoSavedLcdc;

#endif

/* === Function prototypes in C part ==================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

Boolean VideoGraySwitchOn(void)
  VIDEO_SECTION;
void VideoGraySwitchOff(void)
  VIDEO_SECTION;

Boolean VideoSetup(void)
  VIDEO_SECTION;
void VideoCleanup(void)
  VIDEO_SECTION;
void VideoLoadState(const VideoStateType *stateP)
  VIDEO_SECTION;
void VideoSaveState(void *recP, UInt32 offset)
  VIDEO_SECTION;

void VideoSetupPaceControl(void)
  VIDEO_SECTION;
void VideoControlPace(void)
  VIDEO_SECTION;

/* call this function with memory protection disabled! */
void VideoCopyScreen(void)
  VIDEO_SECTION;

#endif

/* === Function prototypes in assembler part ==========================	*/

#ifdef __GNUC__ /* only if C preprocessing */

void VideoUserPoll(void)
  VIDEO_SECTION;
void VideoIoWrTac(void)
  VIDEO_SECTION;

/* call this function with memory protection disabled! */
void VideoClearBuffer(void)
  VIDEO_SECTION;
/* call this function with memory protection disabled! */
void VideoCopyScreenByDirectAccess(void)
  VIDEO_SECTION;

/* NOTE:
 * These functions are called from the emulation and get their parameters
 * via several registers. This is not documented here, the prototypes are
 * just for exporting the names.
 */

void VideoConvertBgWi(void)
  VIDEO_SECTION;
void VideoConvertOb(void)
  VIDEO_SECTION;
void VideoObLink(void)
  VIDEO_SECTION;
void VideoObTileCheck(void)
  VIDEO_SECTION;
void VideoIoWrLcdc(void)
  VIDEO_SECTION;
void VideoIoRdStat(void)
  VIDEO_SECTION;
void VideoIoWrStat(void)
  VIDEO_SECTION;
void VideoIoWrScy(void)
  VIDEO_SECTION;
void VideoIoWrScx(void)
  VIDEO_SECTION;
void VideoIoWrLyc(void)
  VIDEO_SECTION;
void VideoIoWrDma(void)
  VIDEO_SECTION;
void VideoIoWrBgp(void)
  VIDEO_SECTION;
void VideoIoWrObp0(void)
  VIDEO_SECTION;
void VideoIoWrObp1(void)
  VIDEO_SECTION;
void VideoIoWrWy(void)
  VIDEO_SECTION;
void VideoIoWrWx(void)
  VIDEO_SECTION;

#endif

/* === The end ========================================================	*/

#endif
