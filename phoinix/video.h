/************************************************************************
 *  video.h
 *                      Video emulation routines
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

/* === Constants ======================================================	*/

#define videoScreenWidth  (160) /* size of GB screen in pixel */
#define videoScreenHeight (144)

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
  struct {
    UInt8 unused[16]; /* was a bitmap header */
    UInt8 data[videoScreenHeight / 2 *
	       videoScreenWidth / 2 / 8 * videoBitsPerPixel];
  }      bitmap;
} VideoStateType;

#endif

/* === Global variables ===============================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

extern Boolean       VideoIsH330;
extern void          *VideoDisplayPtr;
extern UInt16        VideoDisplayWidth;
extern RectangleType VideoGbScreenBounds;

extern UInt16 VideoCycles; /* opcodes until next LCD line */
extern UInt16 VideoFrames;

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

Boolean VideoGraySwitchOn(void);
void VideoGraySwitchOff(void);

Boolean VideoSetup(void);
void VideoCleanup(void);
void VideoResetState(VideoStateType *stateP);
void VideoLoadState(VideoStateType *stateP);
void VideoSaveState(void *recP, UInt32 offset);

/* call this function with memory protection disabled! */
void VideoClearScreen(void);
/* call this function with memory protection disabled! */
void VideoCopyScreen(void);

#endif

/* === Function prototypes in assembler part ==========================	*/

#ifdef __GNUC__ /* only if C preprocessing */

void VideoUserPoll(void);
void VideoIoWrTac(void);

/* call this function with memory protection disabled! */
void VideoClearScreenByDirectAccess(void);
/* call this function with memory protection disabled! */
void VideoCopyScreenByDirectAccess(void);

/* NOTE:
 * These functions are called from the emulation and get their parameters
 * via several registers. This is not documented here, the prototypes are
 * just for exporting the names.
 */

void VideoConvertBgWi(void);
void VideoConvertOb(void);
void VideoObLink(void);
void VideoObTileCheck(void);
void VideoIoWrLcdc(void);
void VideoIoRdStat(void);
void VideoIoWrStat(void);
void VideoIoWrScy(void);
void VideoIoWrScx(void);
void VideoIoWrLyc(void);
void VideoIoWrDma(void);
void VideoIoWrBgp(void);
void VideoIoWrObp0(void);
void VideoIoWrObp1(void);
void VideoIoWrWy(void);
void VideoIoWrWx(void);

#endif

/* === The end ========================================================	*/

#endif
