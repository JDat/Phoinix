/************************************************************************
 *  video_c.c
 *                      Video emulation routines (C part)
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
 *  Revision 1.4.2.1  2004/02/14 11:54:21  bodowenzel
 *  Support for devices without direct screen access
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

#include <PalmOS.h>
#include <PalmOSGlue.h>

/* Sony HiRez support */
#include <SonyCLIE.h>

/* H330 support */
#include <HandEra/Vga.h>

#include "video.h"
#include "Phoinix.h"
#include "misc.h"
#include "emulation.h"
#include "memory.h"
#include "gbemu.h"

/* === Constants ======================================================	*/

#define tempDbCardNo (0)
#define tempDbName   "Phoinix-Video"
#define tempDbType   'tmpV'

#define bytesPerBuffer (videoScreenHeight * \
                        videoScreenWidth / 8 * videoBitsPerPixel)

#define bitsPerPixelOs5Best  (8)
#define bitsPerPixelCheckMax (31)

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
Boolean       VideoIsH330 = false;
void          *VideoDisplayPtr;    /* pointer to display memory */
UInt16        VideoDisplayWidth;   /* offset between two pixel lines */

/* for a framing rectangle: */
RectangleType VideoGbScreenBounds;

static DmOpenRef TempDbP = NULL;

UInt16 VideoCycles; /* opcodes until next LCD line */
UInt16 VideoFrames;

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

/* === Function prototypes ============================================	*/

static void SetupState(VideoStateType *stateP);

static void Compress(UInt8 *destP);
static void Expand(UInt8 *srceP);

/* === Functions for switching the screen mode ========================	*/

/**
 * Switches into the four level grey mode, retrieving video parameters.
 *
 * @return true if successful.
 */
Boolean VideoGraySwitchOn(void) {
  FormType *frmP;
  UInt32   romV;
  UInt32   depth;
  Err      err;
  UInt16   libRef;

  /* get GB screen values */
  frmP = FrmInitForm(formIdEmulationScreen);
  FrmGetObjectBounds(frmP,
		     FrmGetObjectIndex(frmP, gadgetIdEmulationSmall),
		     &VideoGbScreenBounds);
  FrmDeleteForm(frmP);

  /* retrieve the OS version */
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romV);

  /* on OS version 5 or higher, access via OS */
  if (romV >= sysMakeROMVersion(5, 0, 0, sysROMStageDevelopment, 0)) {
    depth = bitsPerPixelOs5Best;
    /* try to switch the screen mode, but don't care for success */
    (void)WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL);
    ScreenAccessViaOs = true;
    return true;
  }

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

  /* if Sony HiRez, switch to low resolution */
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
    /* switching didn't work, access via OS */
    if (err != errNone) {
      ScreenAccessViaOs = true;
      return true;
    }
  }

  /* get important display values */
  if (romV >= sysMakeROMVersion(3, 5, 0, sysROMStageDevelopment, 0)) {
    BitmapType *bmpP;
    UInt32     vgaV;

    bmpP = WinGetBitmap(WinGetDisplayWindow());
    VideoDisplayPtr = BmpGetBits(bmpP);
    if (romV >= sysMakeROMVersion(4, 0, 0, sysROMStageRelease, 0)) {
      BmpGetDimensions(bmpP, NULL, NULL, &VideoDisplayWidth);
    } else {
      BmpGlueGetDimensions(bmpP, NULL, NULL, &VideoDisplayWidth);
    }

    /* on H330, set screen mode */
    if (FtrGet(TRGSysFtrID, TRGVgaFtrNum, &vgaV) == errNone) {
      VideoIsH330 = true;
      VgaSetScreenMode(screenModeScaleToFit, rotateModeNone);
      frmP = FrmInitForm(formIdEmulationScreen);
      FrmGetObjectBounds(frmP,
			 FrmGetObjectIndex(frmP, gadgetIdEmulationH330),
			 &VideoGbScreenBounds);
      FrmDeleteForm(frmP);
    }

  } else {
    WindowType *winP;

    winP = WinGetWindowPointer(WinGetDisplayWindow());
    VideoDisplayPtr = winP->displayAddrV20;
    VideoDisplayWidth = winP->displayWidthV20 / (8 / videoBitsPerPixel);
  }

  /* correct display pointer */
  VideoDisplayPtr += VideoGbScreenBounds.topLeft.y * VideoDisplayWidth +
    VideoGbScreenBounds.topLeft.x / (8 / videoBitsPerPixel);

  return true;
}

/**
 * Switches back into the default mode.
 */
void VideoGraySwitchOff(void) {
  WinScreenMode(winScreenModeSetToDefaults, NULL, NULL, NULL, NULL);
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
  if (dbId != 0)
    DmDeleteDatabase(tempDbCardNo, dbId);
  (void)DmCreateDatabase(tempDbCardNo, tempDbName, miscCreator, tempDbType,
			 false);
  TempDbP = DmOpenDatabaseByTypeCreator(tempDbType, miscCreator,
					dmModeReadWrite);
  if (TempDbP == NULL)
    return false;

  /* allocate and lock records, on error abort */

  recId = dmMaxRecordIndex;
  recH = DmNewRecord(TempDbP, &recId,
		     2 * videoMapWrap * videoMapWrap * videoBitsPerPixel);
  if (recH == NULL)
    return false;
  VideoMapIndices = MemHandleLock(recH);

  recId = dmMaxRecordIndex;
  recH = DmNewRecord(TempDbP, &recId, sizeof(VideoBgWiRamType));
  if (recH == NULL)
    return false;
  BgRamPtr = MemHandleLock(recH);
  VideoBgUsed = BgRamPtr->used;
  VideoBgTile = BgRamPtr->tile;

  recId = dmMaxRecordIndex;
  recH = DmNewRecord(TempDbP, &recId, sizeof(VideoBgWiRamType));
  if (recH == NULL)
    return false;
  WiRamPtr = MemHandleLock(recH);
  VideoWiUsed = WiRamPtr->used;
  VideoWiTile = WiRamPtr->tile;

  recId = dmMaxRecordIndex;
  recH = DmNewRecord(TempDbP, &recId, sizeof(VideoObRamType));
  if (recH == NULL)
    return false;
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
    recH = DmNewRecord(TempDbP, &recId, bytesPerBuffer);
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
    if (VideoMapIndices != NULL)
      MemPtrUnlock(VideoMapIndices);
    if (BgRamPtr != NULL)
      MemPtrUnlock(BgRamPtr);
    if (WiRamPtr != NULL)
      MemPtrUnlock(WiRamPtr);
    if (ObRamPtr != NULL)
      MemPtrUnlock(ObRamPtr);
    if (ScreenAccessViaOs) {
      if (BufferBitmapPtr != NULL)
	(void)BmpDelete(BufferBitmapPtr);
    } else {
      if (VideoBufferPtr != NULL)
	MemPtrUnlock(VideoBufferPtr);
    }

    /* release all records */
    for (i = 0; i < DmNumRecords(TempDbP); i++)
      DmReleaseRecord(TempDbP, i, false);

    /* close and delete database */
    DmCloseDatabase(TempDbP);
    dbId = DmFindDatabase(tempDbCardNo, tempDbName);
    if (dbId != 0)
      DmDeleteDatabase(tempDbCardNo, dbId);
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
 * Sets up all video stuff to the "reset" state.
 *
 * @param stateP pointer to the video state structure.
 */
void VideoResetState(VideoStateType *stateP) {
  EmulationIo.bgp =  0xfc;
  EmulationIo.obp0 = 0xff;
  EmulationIo.obp1 = 0xff;

  SetupState(stateP);
}

/**
 * Sets up all video stuff to the given state.
 *
 * @param stateP pointer to the video state structure.
 */
void VideoLoadState(VideoStateType *stateP) {
  VideoTimerDiv = stateP->timerDiv;
  VideoTimerTima = stateP->timerTima;

  SetupState(stateP);
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

  MemSemaphoreReserve(true);
  Compress(recP + offset + OffsetOf(VideoStateType, bitmap.data));
  MemSemaphoreRelease(true);
}

/**
 * Sets up common video stuff.
 *
 * @param stateP pointer to the video state structure.
 */
static void SetupState(VideoStateType *stateP) {
  MemSemaphoreReserve(true);

  /* Some memory and variables have to be changed and will be corrected by
   * the setup code. This ensures that all video variables are set up
   * properly and the pre-calculated variables don't have to be saved with
   * the game's state.
   * The LCDC register has to be saved because it can't be inverted simply.
   */
  VideoSavedLcdc = EmulationIo.lcdc;
  EmulationIo.lcdc = 0x80;
  EmulationIo.bgp = ~EmulationIo.bgp;
  EmulationIo.obp0 = ~EmulationIo.obp0;
  EmulationIo.obp1 = ~EmulationIo.obp1;
  {
    UInt8  *p;
    UInt16 i;
    for (p = MemoryBaseRamVid - (0x10000 - 0x8000), i = 0x1800;
	 i < 0x2000; i++)
      p[i] = ~p[i];
  }

  /* create default screen contents */
  Expand(stateP->bitmap.data);

  MemSemaphoreRelease(true);
}

/* === Functions for the screen thumbnail =============================	*/

/**
 * Compresses the GB screen contents into the thumbnail.
 *
 * @param destP pointer to the thumbnail data buffer
 */
static void Compress(UInt8 *destP) {
  UInt8  *src1P, *src2P, w;
  UInt16 x, y;

  src1P = VideoBufferPtr;

  for (y = 0; y < videoScreenHeight / 2; y++) {
    src2P = src1P + videoScreenWidth / (8 / videoBitsPerPixel);
    for (x = 0; x < videoScreenWidth / 2 / (8 / videoBitsPerPixel); x++) {
      w  = ((*src1P & 0xC0) + (*src2P & 0xC0)) >> 2;
      w +=  (*src1P & 0x30) + (*src2P & 0x30);
      w += 0x20;
      *destP = w & 0xC0;
      w  =  (*src1P & 0x0C) + (*src2P & 0x0C);
      w += ((*src1P & 0x03) + (*src2P & 0x03)) << 2;
      w += 0x08;
      *destP |= w & 0x30;
      src1P++;
      src2P++;
      w  = ((*src1P & 0xC0) + (*src2P & 0xC0)) >> 2;
      w +=  (*src1P & 0x30) + (*src2P & 0x30);
      w += 0x20;
      *destP |= (w >> 4) & 0x0C;
      w  =  (*src1P & 0x0C) + (*src2P & 0x0C);
      w += ((*src1P & 0x03) + (*src2P & 0x03)) << 2;
      w += 0x08;
      *destP |= (w >> 4) & 0x03;
      src1P++;
      src2P++;
      destP++;
    }
    src1P = src2P;
  }
}

/**
 * Expands the GB screen contents from the thumbnail.
 *
 * @param srceP pointer to the thumbnail data buffer
 */
static void Expand(UInt8 *srceP) {
  UInt8  *destP;
  UInt16 x, y;

  destP = VideoBufferPtr;

  for (y = 0; y < videoScreenHeight / 2; y++) {
    for (x = 0; x < videoScreenWidth / 2 / (8 / videoBitsPerPixel); x++) {
      UInt8 pixel;

      pixel  = (*srceP & 0xC0) | ((*srceP & 0x30) >> 2);
      pixel |= pixel >> 2;
      *(destP + videoScreenWidth / (8 / videoBitsPerPixel)) = pixel;
      *destP++ = pixel;
      pixel  = ((*srceP & 0x0C) << 2) | (*srceP & 0x03);
      pixel |= pixel << 2;
      *(destP + videoScreenWidth / (8 / videoBitsPerPixel)) = pixel;
      *destP++ = pixel;
      srceP++;
    }
    destP += videoScreenWidth / (8 / videoBitsPerPixel);
  }
}

/* === Clear screen contents ==========================================	*/

/**
 * Clears the GB screen in the display.
 *
 * Call this function with memory protection disabled!
 */

void VideoClearScreen(void) {
  if (ScreenAccessViaOs) {
    const static RectangleType r = {
      { 0, 0 },
      { videoScreenWidth, videoScreenHeight }
    };

    MemSemaphoreRelease(true);
    WinEraseRectangle(&r, 0);
    MemSemaphoreReserve(true);
  } else {
    VideoClearScreenByDirectAccess();
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
