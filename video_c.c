/************************************************************************
 *  video_c.c
 *                      Video emulation routines (C part)
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
 *  $Log: video_c.c,v $
 *  Revision 1.19  2006/12/18 17:00:15  bodowenzel
 *  Switch video mode especially for the Fossil Wrist PDA
 *
 *  Revision 1.18  2006/10/23 15:39:27  bodowenzel
 *  Clearing the screen at STOP corrected
 *
 *  Revision 1.17  2006/04/23 18:41:25  bodowenzel
 *  Corrected memory release
 *
 *  Revision 1.16  2005/10/11 14:48:05  bodowenzel
 *  Centralized device type into MiscDevice
 *
 *  Revision 1.15  2005/05/06 10:32:36  bodowenzel
 *  Saving full resolution screen in state
 *
 *  Revision 1.14  2005/04/03 14:08:34  bodowenzel
 *  Option to save the state for each game
 *
 *  Revision 1.13  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.12  2005/01/29 10:25:37  bodowenzel
 *  Added const qualifier to pointer parameters
 *
 *  Revision 1.11  2004/12/28 13:59:03  bodowenzel
 *  Split up all C-Code to multi-segmented
 *  Support for devices without direct screen access
 *  Support for AlphaSmart Dana
 *
 *  Revision 1.10  2004/10/18 17:48:51  bodowenzel
 *  Dropped macro LITE, no need to maintain smaller code
 *
 *  Revision 1.9  2004/01/11 19:07:14  bodowenzel
 *  Corrected copyright
 *
 *  Revision 1.8  2003/04/27 09:36:18  bodowenzel
 *  Added Lite edition
 *
 *  Revision 1.7  2003/04/19 13:18:18  bodowenzel
 *  Screen copy for OS5 via WinDrawBitmap()
 *
 *  Revision 1.6  2003/04/12 15:24:17  bodowenzel
 *  Removed useless bitmap header in saved state
 *  Corrected compress() for thumbnail
 *
 *  Revision 1.5  2003/03/12 21:20:25  bodowenzel
 *  Switch Cliés into low resolution
 *
 *  Revision 1.4  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.3  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.2  2001/12/28 18:36:49  bodowenzel
 *  VideoCopyScreen() now after rendering all lines
 *  Thumbnail taken from VideoBuffer
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:27  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-16  Bodo Wenzel   Saving states now possible
 *  2001-07-22  Bodo Wenzel   Separated from emuvideo.asm, joined with
 *                            parts of gray.asm, changed to C
 *
 *  emuvideo.asm:
 *  2001-02-18  Bodo Wenzel   Window bug removed,
 *                            bug with visibility of objects found,
 *                            interrupts for hsync implemented
 *  2000-11-18  Bodo Wenzel   Support of color devices
 *  2000-10-23  Bodo Wenzel   Interrupts and timer
 *  2000-09-17  Bodo Wenzel   Now nearly full screen emulation
 *  2000-09-10  Bodo Wenzel   Got from runmode.asm
 *
 *  gray.asm:
 *  2001-02-10  Bodo Wenzel   Support for grayhack: no register writes
 *  2000-11-19  Bodo Wenzel   Clean-up of support for color devices
 *  2000-11-01  Till Harbaum  Added support for the Palm IIIc and friends
 *  2000-09-13  Bodo Wenzel   Now nearly full screen emulation
 *  2000-07-26  Bodo Wenzel   Longer off time while mode switching
 *  2000-05-05  Bodo Wenzel   Got from full source v.0.4
 ************************************************************************
 */

/* IMPORTANT!
 * Please make sure that any referenced resource is included!
 * Unfortunately there is no automatic!
 */

/* === Includes =======================================================	*/

/* Access needed until a function is provided, see below */
#define ALLOW_ACCESS_TO_INTERNALS_OF_WINDOWS

#include <PalmOS.h>
#include <PalmOSGlue.h>

#include "Phoinix.h"

#ifdef sonyclie
	/* Sony HiRez support */
	#include <SonyCLIE.h>
#endif

#ifdef handera
	/* Handera H330 support */
	#include <HandEra/Vga.h>
#endif

#ifdef alphasmart
	/* AlphaSmart Dana support */
	#include <Screen.h>
#endif

#include "video.h"

#include "misc.h"
#include "states.h"
#include "emulation.h"
#include "memory.h"
#include "gbemu.h"

/* === Constants ======================================================	*/

#define tempDbCardNo (0)
#define tempDbName   "Phoinix-Video"
#define tempDbType   'tmpV'

#define bitsPerPixelForWrist (4)
#define bitsPerPixelOsOptimum (8)
#define bitsPerPixelCheckMax  (31)

/* fortunately there's just one display size ;-) */
#define h330DisplayOffsetX (40) /* divisible by 8 / videoBitsPerPixel */
#define h330DisplayOffsetY (36)

/* === Type definitions ===============================================	*/

/* both of pattern and mask are
 *   8 pixel lines in
 *     $100 possible tiles of
 *       1 shifted word as long
 */
typedef struct {
  UInt8  used[0x100];       /* which tiles are used */
  UInt32 tile[2][8][0x100]; /* converted tiles */
} VideoBgWiRamType;

/* (SCR_HEIGHT*8) lines with
 *   1 word offset to first background object
 *   1 word offset to first foreground object
 * 40 objects of
 *   16 pixel lines with
 *     1 word screen horizontal offset
 *     1 shifted word as long mask
 *     1 shifted word as long pattern
 *     1 word next offset
 *     1 word prev offset
 */
typedef struct {
  UInt16 dummy; /* for offset -2 */
  struct {
    UInt16 bg;
    UInt16 fg;
  }      first[videoScreenHeight]; /* offsets */
  struct {
    UInt16 hOffset;
    UInt32 mask;
    UInt32 pattern;
    UInt16 next;
    UInt16 prev;
  }      tiles[40][16]; /* converted tiles */
} VideoObRamType;

/* === Global and static variables ====================================	*/

static Boolean ScreenAccessViaOs = false;

/* for direct screen access: */
void    *VideoDisplayPtr;    /* pointer to display memory */
UInt16  VideoDisplayWidth;   /* offset between two pixel lines */

#ifdef alphasmart
static ScrnRotateModeType DanaRotateMode;
#endif
/* for a framing rectangle: */
RectangleType VideoGbScreenBounds = {
  { 0, 0 },
  { videoScreenWidth, videoScreenHeight }
};

static DmOpenRef TempDbP = NULL;

UInt16 VideoCycles; /* opcodes until next LCD line */
UInt16 VideoFrames;

Boolean VideoBoosted;
UInt16  VideoFramesToSkip;

static Boolean Skipping;
static UInt32  TicksExpected;
static UInt32  TicksDelta;

UInt16 VideoTimerDiv;       /* fix-real for DIV */
UInt16 VideoTimerTima;      /* fix-real for TIMA */
UInt16 VideoTimerDeltaTima; /* increment for TIMA */

UInt16 *VideoMapIndices = NULL; /* pointer to allocated MapIndices */

static VideoBgWiRamType *BgRamPtr = NULL; /* background */
       void             *VideoBgUsed;
       void             *VideoBgTile;

static VideoBgWiRamType *WiRamPtr = NULL; /* window */
       void             *VideoWiUsed;
       void             *VideoWiTile;

static VideoObRamType *ObRamPtr = NULL; /* objects */
       void           *VideoObFirst;
       void           *VideoObTiles;

static BitmapType *BufferBitmapPtr = NULL;
       void       *VideoBufferPtr = NULL;

UInt8 VideoSavedLcdc; /* save LCDC for setup via GB code */

/* === Functions for switching the screen mode ========================	*/

/**
 * Prepare video mode, retrieving video parameters.
 *
 * On OS 5.0 and greater, an attempt to use 8 bits per pixel is made.
 * Since this is just for speed reasons, it doesn't matter if it can't be
 * used. The screen memory is accessed via the OS in any case.
 * On an OS before 5.0 the video mode with the lowest number of bits per
 * pixel is searched. If the GB-compatible pixel depth on a screen with
 * low density can be used, direct access to the screen memory is used.
 * Otherwise the OS is used to write to the screen.
 *
 * @return true if successful.
 */
Boolean VideoGraySwitchOn(void) {
  UInt32 romV;
  UInt32 depth;
#ifdef sonyclie  
  Err    err;
  UInt16 libRef;
#endif

  /* retrieve the OS version */
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romV);

  /* on OS version 5 or higher, access via OS */
  if (romV >= sysMakeROMVersion(5, 0, 0, sysROMStageDevelopment, 0)) {
    depth = bitsPerPixelOsOptimum;
    /* try to switch the screen mode, but don't care for success */
    (void)WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL);
    ScreenAccessViaOs = true;
    return true;
  }

#ifdef alphasmart
  /* on Dana, the rotate mode has to be set before the screen mode */
  if (MiscDevice == miscDeviceDana) {
    ScrnGetRotateMode(&DanaRotateMode);
    (void)ScrnSetRotateMode(rotateScrnMode0);
  }
#endif
#ifdef fossil
  /* on Fossil Wrist PDA the standard gray shows vertical stripes */
  if (MiscDevice == miscDeviceWrist) {
    depth = bitsPerPixelForWrist;
    if (WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL)
	== errNone) {
      ScreenAccessViaOs = true;
      return true;
    }
  }
#endif

  /* when optimal bits-per-pixel mode isn't available, access via OS */
  depth = videoBitsPerPixel;
  if (WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL)
      != errNone) {
    /* use mode with lowest bits-per-pixel */
    for (depth = videoBitsPerPixel + 1;
	 depth <= bitsPerPixelCheckMax;
	 depth++) {
      if (WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL)
	  == errNone) {
	ScreenAccessViaOs = true;
	return true;
      }
    }
    /* no usable mode found */
    return false;
  }

#ifdef sonyclie
  /* when low resolution on Sony isn't available, access via OS */
  err = SysLibFind(sonySysLibNameHR, &libRef);
  if (err == sysErrLibNotFound) {
    err = SysLibLoad('libr', sonySysFileCHRLib, &libRef);
  }
  if (err == errNone) {
    err = HROpen(libRef);
    if (err == errNone) {
      UInt32 width, height;

      width = stdWidth;
      height = stdHeight;
      err = HRWinScreenMode(libRef, winScreenModeSet, &width, &height,
			    NULL, NULL);
      (void)HRClose(libRef);
    }
    if (err != errNone) {
      ScreenAccessViaOs = true;
      return true;
    }
  }
#endif

  /* get important display values */
  if (romV >= sysMakeROMVersion(3, 5, 0, sysROMStageDevelopment, 0)) {
    BitmapType *bmpP;

    bmpP = WinGetBitmap(WinGetDisplayWindow());
    VideoDisplayPtr = BmpGetBits(bmpP);
    BmpGlueGetDimensions(bmpP, NULL, NULL, &VideoDisplayWidth);

#ifdef handera
    /* on H330, set screen mode and correct values */
    if (MiscDevice == miscDeviceH330) {
      VgaSetScreenMode(screenModeScaleToFit, rotateModeNone);

      VideoGbScreenBounds.topLeft.x = h330DisplayOffsetX;
      VideoGbScreenBounds.topLeft.y = h330DisplayOffsetY;
      VideoDisplayPtr +=
	VideoGbScreenBounds.topLeft.y * VideoDisplayWidth +
	VideoGbScreenBounds.topLeft.x / (8 / videoBitsPerPixel);
    }
#endif

#ifdef alphasmart
    /* on Dana, correct values */
    if (MiscDevice == miscDeviceDana) {
      ScrnSystemStateType state;

      ScrnGetSystemState(&state);
      /* hopefully the offset_X is divisible by 8 / videoBitsPerPixel */
      VideoDisplayPtr += state.offset_Y * VideoDisplayWidth +
	state.offset_X / (8 / videoBitsPerPixel);
    }
#endif

  } else {
    WindowType *winP;

    winP = WinGetWindowPointer(WinGetDisplayWindow());
    /* we need ALLOW_ACCESS_TO_INTERNALS_OF_WINDOWS defined */
    VideoDisplayPtr = winP->displayAddrV20;
    /* we need ALLOW_ACCESS_TO_INTERNALS_OF_WINDOWS defined */
    VideoDisplayWidth = winP->displayWidthV20 / (8 / videoBitsPerPixel);
  }

  return true;
}

/**
 * Switches back into the default mode.
 */
void VideoGraySwitchOff(void) {
  WinScreenMode(winScreenModeSetToDefaults, NULL, NULL, NULL, NULL);
#ifdef alphasmart
  if (MiscDevice == miscDeviceDana) {
    (void)ScrnSetRotateMode(DanaRotateMode);
  }
#endif
}

/* === Setup, cleanup, reset, load and save ===========================	*/

/**
 * Sets up all video stuff for the emulation, especially obtaining
 * enough memory in a temporary database.
 *
 * @return true if successful.
 */
Boolean VideoSetup(void) {
  LocalID   dbId;
  UInt16    recId;
  MemHandle recH;

  /* create temporary database, on error abort */
  dbId = DmFindDatabase(tempDbCardNo, tempDbName);
  if (dbId != 0) {
    DmDeleteDatabase(tempDbCardNo, dbId);
  }
  (void)DmCreateDatabase(tempDbCardNo, tempDbName, miscCreator,
			 tempDbType, false);
  TempDbP = DmOpenDatabaseByTypeCreator(tempDbType, miscCreator,
					dmModeReadWrite);
  if (TempDbP == NULL) {
    return false;
  }

  /* allocate and lock records, on error abort */

  recId = dmMaxRecordIndex;
  recH = DmNewRecord(TempDbP, &recId,
		     2 * videoMapWrap * videoMapWrap * videoBitsPerPixel);
  if (recH == NULL) {
    return false;
  }
  VideoMapIndices = MemHandleLock(recH);

  recId = dmMaxRecordIndex;
  recH = DmNewRecord(TempDbP, &recId, sizeof(VideoBgWiRamType));
  if (recH == NULL) {
    return false;
  }
  BgRamPtr = MemHandleLock(recH);
  VideoBgUsed = BgRamPtr->used;
  VideoBgTile = BgRamPtr->tile;

  recId = dmMaxRecordIndex;
  recH = DmNewRecord(TempDbP, &recId, sizeof(VideoBgWiRamType));
  if (recH == NULL) {
    return false;
  }
  WiRamPtr = MemHandleLock(recH);
  VideoWiUsed = WiRamPtr->used;
  VideoWiTile = WiRamPtr->tile;

  recId = dmMaxRecordIndex;
  recH = DmNewRecord(TempDbP, &recId, sizeof(VideoObRamType));
  if (recH == NULL) {
    return false;
  }
  ObRamPtr = MemHandleLock(recH);
  VideoObFirst = ObRamPtr->first;
  VideoObTiles = ObRamPtr->tiles;

  if (ScreenAccessViaOs) {
    Err err;

    BufferBitmapPtr = BmpCreate(videoScreenWidth, videoScreenHeight,
				videoBitsPerPixel, NULL, &err);
    if (BufferBitmapPtr == NULL) {
      return false;
    }
    VideoBufferPtr = BmpGetBits(BufferBitmapPtr);
  } else {
    recId = dmMaxRecordIndex;
    recH = DmNewRecord(TempDbP, &recId, videoBytesPerBuffer);
    if (recH == NULL) {
      return false;
    }
    VideoBufferPtr = MemHandleLock(recH);
  }

  /* setup vectors */

  MemSemaphoreReserve(true);

  EmulationEmulator[0x07].out.vector.address = VideoIoWrTac;

  EmulationEmulator[0x40].out.vector.address = VideoIoWrLcdc;
  EmulationEmulator[0x41].in.vector.address = VideoIoRdStat;
  EmulationEmulator[0x41].out.vector.address = VideoIoWrStat;
  EmulationEmulator[0x42].out.vector.address = VideoIoWrScy;
  EmulationEmulator[0x43].out.vector.address = VideoIoWrScx;
  EmulationEmulator[0x45].out.vector.address = VideoIoWrLyc;
  EmulationEmulator[gbemuJmpIoWrDma ].xt.vector.address = VideoIoWrDma;
  EmulationEmulator[0x47].out.vector.address = VideoIoWrBgp;
  EmulationEmulator[0x48].out.vector.address = VideoIoWrObp0;
  EmulationEmulator[0x49].out.vector.address = VideoIoWrObp1;
  EmulationEmulator[0x4a].out.vector.address = VideoIoWrWy;
  EmulationEmulator[0x4b].out.vector.address = VideoIoWrWx;

  MemSemaphoreRelease(true);

  return true;
}

/**
 * Cleans up all video stuff after emulation.
 */
void VideoCleanup(void) {
  /* remove temporary database, if present */
  if (TempDbP != NULL) {
    LocalID dbId;
    UInt16  i;

    /* unlock all allocated records */
    if (VideoMapIndices != NULL) {
      MemPtrUnlock(VideoMapIndices);
    }
    if (BgRamPtr != NULL) {
      MemPtrUnlock(BgRamPtr);
    }
    if (WiRamPtr != NULL) {
      MemPtrUnlock(WiRamPtr);
    }
    if (ObRamPtr != NULL) {
      MemPtrUnlock(ObRamPtr);
    }
    if (BufferBitmapPtr != NULL) {
      (void)BmpDelete(BufferBitmapPtr);
      /* don't unlock/free VideoBufferPtr */
    } else if (VideoBufferPtr != NULL) {
      MemPtrUnlock(VideoBufferPtr);
    }

    /* release all records */
    for (i = 0; i < DmNumRecords(TempDbP); i++) {
      DmReleaseRecord(TempDbP, i, false);
    }

    /* close and delete database */
    DmCloseDatabase(TempDbP);
    dbId = DmFindDatabase(tempDbCardNo, tempDbName);
    if (dbId != 0) {
      DmDeleteDatabase(tempDbCardNo, dbId);
    }
  }

  TempDbP = NULL;
  VideoMapIndices = NULL;
  BgRamPtr = NULL;
  WiRamPtr = NULL;
  ObRamPtr = NULL;
  BufferBitmapPtr = NULL;
  VideoBufferPtr = NULL;
}

/**
 * Sets up all video stuff to the given state.
 *
 * @param stateP pointer to the video state structure.
 */
void VideoLoadState(const VideoStateType *stateP) {
  VideoTimerDiv = stateP->timerDiv;
  VideoTimerTima = stateP->timerTima;

  /* Some memory and variables have to be changed and will be corrected by
   * the setup code. This ensures that all video variables are set up
   * properly and the pre-calculated variables don't have to be saved with
   * the game's state.
   * The LCDC register has to be saved because it can't be inverted
   * simply.
   */
  VideoSavedLcdc = EmulationIo.lcdc;
  EmulationIo.lcdc = 0x80;
  EmulationIo.bgp = ~EmulationIo.bgp;
  EmulationIo.obp0 = ~EmulationIo.obp0;
  EmulationIo.obp1 = ~EmulationIo.obp1;

  MemSemaphoreReserve(true);

  {
    UInt8  *p;
    UInt16 i;

    for (p = MemoryBaseRamVid - (0x10000 - 0x8000), i = 0x1800;
	 i < 0x2000;
	 i++) {
      p[i] = ~p[i];
    }
  }

  {
    /* create default screen contents */
    const UInt8 *s;
    UInt8       *d;
    UInt16      i;

    for (d = VideoBufferPtr, s = stateP->videoBuffer, i = 0;
	 i < videoBytesPerBuffer;
	 i++) {
      *d++ = *s++;
    }
  }

  MemSemaphoreRelease(true);
}

/**
 * Saves the video state in the given record.
 *
 * @param recP   pointer to the record.
 * @param offset offset into the record.
 */
void VideoSaveState(void *recP, UInt32 offset) {
  DmWrite(recP, offset + OffsetOf(VideoStateType, timerDiv),
	  &VideoTimerDiv, sizeof(VideoTimerDiv));
  DmWrite(recP, offset + OffsetOf(VideoStateType, timerTima),
	  &VideoTimerTima, sizeof(VideoTimerTima));
  DmWrite(recP, offset + OffsetOf(VideoStateType, videoBuffer),
	  VideoBufferPtr, videoBytesPerBuffer);
}

/* === Pace control ===================================================	*/

/**
 * Sets up the pace control.
 *
 * Note: here's an opportunity to set the pace: multiply the TicksDelta.
 */
void VideoSetupPaceControl(void) {
  TicksDelta = ((UInt32)SysTicksPerSecond() << 16) / videoFramesPerSec;
  Skipping = false;
  VideoFramesToSkip = 0;
  TicksExpected = TimGetTicks() << 16;
}

/**
 * Controls the pace by skipping frames or inserting delays.
 */
void VideoControlPace(void) {
  if (VideoBoosted) {
    /* boost mode */
    if (VideoFramesToSkip == 0) {
      VideoFramesToSkip = StatesEmulationOptionsP->boost;
    } else {
      VideoFramesToSkip--;
    }

  } else {
    /* real speed mode */
    UInt16 currentTicks;
    Int16  diffTicks;

    /* the lower 16 bits are sufficient */
    currentTicks = (UInt16)TimGetTicks();

    /* calculate difference between elapsed and expected ticks */
    TicksExpected += TicksDelta;
    diffTicks = (TicksExpected >> 16) - currentTicks;

    if (VideoFramesToSkip == 0) {
      /* the last frame was shown */
      if (diffTicks < 0) {
	if (Skipping) {
	  TicksExpected = (UInt32)currentTicks << 16;
	}
	Skipping = true;
	VideoFramesToSkip = StatesEmulationOptionsP->maxFs;
      } else if (diffTicks > 0 && !Skipping) {
	SysTaskDelay(diffTicks);
      } else {
	Skipping = false;
      }

    } else {
      /* the last frame was skipped */
      VideoFramesToSkip--;
      if (diffTicks >= 0) {
	Skipping = false;
	VideoFramesToSkip = 0;
      }
    }
  }
}

/* === Copying screen contents ========================================	*/

/**
 * Copies the rendered GB screen into the display.
 *
 * Call this function with memory protection disabled!
 */

void VideoCopyScreen(void) {
  if (ScreenAccessViaOs) {
    MemSemaphoreRelease(true);
    WinDrawBitmap(BufferBitmapPtr, 0, 0);
    MemSemaphoreReserve(true);
  } else {
    VideoCopyScreenByDirectAccess();
  }
}

/* === The end ========================================================	*/
