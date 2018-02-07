/************************************************************************
 *  video_s.S
 *                      Video emulation routines (assembler part)
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
 *  $Log: video_s.S,v $
 *  Revision 1.15  2007/02/25 13:15:08  bodowenzel
 *  No interrupt when writing into STAT
 *  Check for LYC on line 144 (Vsync) added
 *
 *  Revision 1.14  2007/02/14 15:23:45  bodowenzel
 *  Frame counting like before gives better info display
 *  Writing into LYC should not generate an interrupt
 *
 *  Revision 1.13  2007/02/11 17:14:53  bodowenzel
 *  Correct counting of video frames
 *  Correct handling of opcode HALT
 *
 *  Revision 1.12  2007/01/08 13:46:56  bodowenzel
 *  Hsync interrupts only in line 0 to 143
 *
 *  Revision 1.11  2007/01/05 14:17:33  bodowenzel
 *  LYC=0 interrupt now on first line
 *
 *  Revision 1.10  2006/10/23 15:39:27  bodowenzel
 *  Clearing the screen at STOP corrected
 *
 *  Revision 1.9  2005/02/17 19:42:35  bodowenzel
 *  Added pace control
 *
 *  Revision 1.8  2005/01/30 19:34:39  bodowenzel
 *  Shortened long error message
 *
 *  Revision 1.7  2004/12/28 13:58:40  bodowenzel
 *  Split up all C-Code to multi-segmented
 *  Support for devices without direct screen access
 *
 *  Revision 1.6  2004/01/11 19:07:14  bodowenzel
 *  Corrected copyright
 *
 *  Revision 1.5  2003/04/19 13:17:36  bodowenzel
 *  Now all calls are jbsr xxx(%pc)
 *  Screen copy for OS5 via WinDrawBitmap()
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
 *  Revision 1.1.1.1  2001/12/16 13:39:34  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-16  Bodo Wenzel   Saving states now possible
 *  2001-07-26  Bodo Wenzel   Changed syntax to CPP + GAS, joined with
 *                            parts of gray.asm
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

/* === Notes ============================================================
 *
 * > Unlike documented for the real part, this emulation never shows
 *   153/0 in the last line to the CPU, but 255.
 * > For speeding up the timings for LCD status modes 2 and 3 are
 *   shortened.
 * real:  152 gap 153/0 line   0   line ... 143   vsync 144 gap ...
 * emu:   152 gap 255   line   0   line ... 143   vsync 144 gap ...
 * i_ly:  -10 gap 143   line 142   line ...  -1   vsync  -2 gap ...
 * stat:  1       1          2/3/0      ... 2/3/0       1       ...
 *
 * > Unlike the real part the interrupts are just sampled at vsync (for
 *   vsync and joypad) and at the end of each line (for LYC, timer, hsync,
 *   and serial[currently not emulated]).
 * > The display is built by different functions, each called for just one
 *   line. In general every line function uses preconverted patterns and
 *   masks. There are counters to manage the wrapping at borders, too.
 * > The objects are drawn at last.
 * > If data is written into video relevant memory, conversion takes place
 *   immediate. Since memory contents change not as often as screen output
 *   is made, this saves time.
 */

/* === Includes =======================================================	*/

#include "video.h"

#include "gbemu.h"

/* === Constants ======================================================	*/

#define screenYMax (videoScreenHeight-1)	/* last line index */

#define opsPerLine (videoClksPerLine/6)
/* Calculated as clocks per line divided by 6 clocks average per opcode.
 * This determines greatly the emulation speed!
 */

#define widthInBytes (videoScreenWidth/8*videoBitsPerPixel)

/* === Global and static variables ====================================	*/

	.comm	VideoLyPrv,2		/* private line counters */
	.lcomm	VideoWyPrv,2

	.lcomm	VideoLineFunction,4	/* addr of next render function */
	.lcomm	VideoBgCount1,2		/* counter for loops, sum is 19 */
	.lcomm	VideoBgCount2,2
	.lcomm	VideoBgWiCount1,2	/* if <0: no wrap before window */
	.lcomm	VideoBgWiCount2,2	/* if <0: only window */
	.lcomm	VideoBgWiCount3,2	/* if <0: window not visible */

	.lcomm	VideoBufferCurPtr,4	/* pointer into display buffer */

	.comm	VideoBgWiColorH,4	/* addresses of color functions */
	.comm	VideoBgWiColorL,4
	.comm	VideoBgWiTileLow,4	/* pointer for tiles 0x00..0x7f */
	.comm	VideoBgWiTileHigh,4	/* pointer for tiles 0x80..0xff */
	.lcomm	VideoBgWiMask,widthInBytes	/* mask buffer one line */

	.comm	VideoBgScxPixel,2	/* prehandled scroll */
	.lcomm	VideoBgScxTile,2
	.lcomm	VideoBgMapStart,4	/* pointer into background */
	.lcomm	VideoBgMapOffset,2
	.lcomm	VideoBgTilePtr,4	/* pointer to converted tiles */
	.lcomm	VideoBgTileMax,4

	.comm	VideoWiWxPixel,2	/* prehandled scroll */
	.lcomm	VideoWiWxMask,2
	.lcomm	VideoWiMapStart,4	/* pointer into window */
	.lcomm	VideoWiMapOffset,2
	.lcomm	VideoWiTilePtr,4	/* pointer to converted tiles */
	.lcomm	VideoWiTileMax,4

	.lcomm	VideoObYOffset,2	/* offset into offset table */
	.comm	VideoObColor0H,4	/* addresses of color functions */
	.comm	VideoObColor0L,4
	.comm	VideoObColor1H,4
	.comm	VideoObColor1L,4
	.comm	VideoObCount,2		/* prehandled height */
	.comm	VideoObTilemask,2

	.lcomm	VideoIntMaskVsync,1	/* mask for vsync int */
	.lcomm	VideoIntMaskStat,1	/* mask for STAT int */
	.lcomm	VideoIntMaskSV,1	/* mask for STAT, vsync */
	.lcomm	VideoIntMaskSH,1	/* mask for STAT, hsync */
	.lcomm	VideoIntLycReload,1	/* counter for STAT, LYC */
	.lcomm	VideoIntLycCount,1

/* === Placing the code ===============================================	*/

	.section	emulator,"x"

/* === Function for user polling ======================================	*/

	.globl	VideoUserPoll
VideoUserPoll:
/* This function is called after the last screen line. The function
 * returns to the caller of the emulator if the emulation quits.
 */
#define vupRegs %d3/%a0/%a1
	movem.l	vupRegs,-(%sp)

/* - - copy screen if frame doesn't have to be skipped - - - - - - - -	*/
	move.w	VideoFramesToSkip@END.w(%a5),%d0
	jne	VupNoCopy		/* skip frame? */

	jbsr	VideoCopyScreen(%pc)	/* show last rendered screen */
VupNoCopy:
	add.w	#1,VideoFrames@END.w(%a5)

/* - - poll for user and system requests - - - - - - - - - - - - - - -	*/
	jbsr	EmulationUserPoll(%pc)
	tst.b	%d0
	jeq	VupGoon			/* continue emulation */

	movem.l	(%sp)+,vupRegs

	lea	0x76*0x0100(%a4),%a2
	lea	gbemuOpSize(%a2),%a3
	move.l	(%sp)+,%d0
	cmp.l	%a2,%d0
	jcs	VupQuit
	cmp.l	%a3,%d0			/* check for opcode $76 (HALT) */
	jcc	VupQuit
	subq.w	#1,%a0			/* correct PC to repeat it */

VupQuit:
	moveq	#gbemuRetUserStop,%d0	/* return to emulation caller */
	rts
VupGoon:

/* - - check for joypad interrupt  - - - - - - - - - - - - - - - - - -	*/
	move.l	EmulationJoypadPtr@END.w(%a5),%a0
	move.b	(%a0),%d0
	not.b	%d0
	and.b	EmulationJoypadOld@END.w(%a5),%d0
	sne	%d1
	and.b	#0x10,%d1
	or.b	%d1,EmulationIo.if_@END.w(%a5)
	move.b	(%a0),EmulationJoypadOld@END.w(%a5)

/* - - MBC3 work (count RTC) - - - - - - - - - - - - - - - - - - - - -	*/
	jbsr	MemoryMbc3RtcWork(%pc)

/* - - start a new screen  - - - - - - - - - - - - - - - - - - - - - -	*/
	move.w	#screenYMax-1-(videoScreenHeight),VideoLyPrv@END.w(%a5)
	lea	LineGap(%pc),%a2
	move.l	%a2,2+indexToAddress(gbemuJmpLcdLine)

/* - - return to emulation - - - - - - - - - - - - - - - - - - - - - -	*/
	move.w	#opsPerLine-1,%d6

	add.w	#videoClksPerLine,VideoTimerDiv@END.w(%a5)

	move.w	VideoTimerDeltaTima@END.w(%a5),%d0
	add.w	%d0,VideoTimerTima@END.w(%a5)
	jcc	VupTExit
	move.b	EmulationIo.tma@END.w(%a5),%d0
VupTLoop:
	add.b	%d0,VideoTimerTima@END.w(%a5)
	jcs	VupTLoop
	bset	#2,EmulationIo.if_@END.w(%a5)
VupTExit:

	subq.b	#1,VideoIntLycCount@END.w(%a5)
	jcc	VupLycExit
	btst	#6,EmulationIo.stat@END.w(%a5)
	jeq	VupLycExit
	move.b	VideoIntMaskStat@END.w(%a5),%d0
	or.b	%d0,EmulationIo.if_@END.w(%a5)
VupLycExit:

	move.b	VideoIntMaskSV@END.w(%a5),%d0
	and.b	VideoIntMaskStat@END.w(%a5),%d0
	or.b	VideoIntMaskVsync@END.w(%a5),%d0
	or.b	%d0,EmulationIo.if_@END.w(%a5)

	movem.l	(%sp)+,vupRegs
	jmp	indexToAddress(gbemuCodeCheckInt)

/* === Timer support ==================================================	*/

	.globl	VideoIoWrTac
VideoIoWrTac:
	move.b	%d0,EmulationIo.tac@END.w(%a5)

	and.w	#7,%d0			/* set delta value */
	add.w	%d0,%d0
	lea	ViwtTable(%pc),%a3
	move.w	(%a3,%d0.w),VideoTimerDeltaTima@END.w(%a5)
	jmp	(%a6)
ViwtTable:
	.short	0,0,0,0			/* timer stopped */
	.short	videoClksPerLine/4	/* timer running */
	.short	videoClksPerLine*16
	.short	videoClksPerLine*4
	.short	videoClksPerLine

/* === Functions for screen rendering =================================	*/

	.globl	VideoClearBuffer
VideoClearBuffer:
/* Can be called from emulation and C-code:
 *	d0.w	(scratch)
 *	d1	(scratch)
 * Call this function with memory protection disabled!
 */
	move.l	%a2,-(%sp)
	move.l	VideoBufferPtr@END.w(%a5),%a2
	move.w	#videoScreenHeight-1,%d0
	moveq	#0,%d1
VcbLoop:
#if videoScreenWidth != 160
#error "Please adjust function VideoClearBuffer!"
#endif
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	dbra	%d0,VcbLoop
	move.l	(%sp)+,%a2
	rts

/* --------------------------------------------------------------------	*/

	.globl	VideoCopyScreenByDirectAccess
VideoCopyScreenByDirectAccess:
/* Can be called from emulation and C-code:
 *	d0.w	(scratch)
 *	d1.w	(scratch)
 *	d2	(scratch)
 * Call this function with memory protection disabled!
 */

#if videoScreenWidth != 160
#error "Please adjust function VideoCopyScreenByDirectAccess!"
#endif
#define vcpsRegs     %d3-%d7/%a0-%a4/%a6
#define vcpsCopyRegs %d2-%d7/%a2-%a4/%a6	/* 10 registers !!! */
	movem.l	vcpsRegs,-(%sp)

	move.l	VideoBufferPtr@END.w(%a5),%a0
	move.l	VideoDisplayPtr@END.w(%a5),%a1
	move.w	#videoScreenHeight-1,%d0
	move.w	VideoDisplayWidth@END.w(%a5),%d1
VcpsLoop:
	movem.l	(%a0)+,vcpsCopyRegs
	movem.l	vcpsCopyRegs,(%a1)
	add.w	%d1,%a1
	dbra	%d0,VcpsLoop

	movem.l	(%sp)+,vcpsRegs
	rts

/* --------------------------------------------------------------------	*/

LineGap:
	move.w	VideoLyPrv@END.w(%a5),%d0
	subq.w	#1,%d0
	cmp.w	#screenYMax-1-153,%d0
	jeq	LgNewPic		/* vsync finished? */
	move.w	%d0,VideoLyPrv@END.w(%a5)
	jra	LineGapQuitReturn

/* - - start a new picture, no need to check for on/off - - - - - - - -	*/
LgNewPic:
	move.w	#screenYMax-1+1,VideoLyPrv@END.w(%a5)

	move.b	EmulationIo.scy@END.w(%a5),%d6
	add.w	%d6,%d6
	add.w	%d6,%d6
	move.b	%d6,-(%sp)
	and.w	#(videoMapWrap-1)*videoMapWrap,%d6
	add.w	VideoBgScxTile@END.w(%a5),%d6
	add.w	%d6,%d6
	move.w	%d6,VideoBgMapOffset@END.w(%a5)
	move.w	(%sp)+,%d6
	and.w	#7*0x100*4,%d6
	move.l	VideoBgTile@END.w(%a5),%a3
	lea	8*0x100*4(%a3),%a2
	add.w	%d6,%a3
	move.l	%a3,VideoBgTilePtr@END.w(%a5)
	move.l	%a2,VideoBgTileMax@END.w(%a5)	/* set up for background */

	moveq	#0,%d1
	move.w	%d1,VideoWiMapOffset@END.w(%a5)
	move.b	EmulationIo.wy@END.w(%a5),%d1
	move.w	%d1,VideoWyPrv@END.w(%a5)
	move.l	VideoWiTile@END.w(%a5),%a3
	lea	8*0x100*4(%a3),%a2
	move.l	%a3,VideoWiTilePtr@END.w(%a5)
	move.l	%a2,VideoWiTileMax@END.w(%a5)	/* set up for window */

	clr.w	VideoObYOffset@END.w(%a5)	/* set up for objects */

	move.l	VideoBufferPtr@END.w(%a5),VideoBufferCurPtr@END.w(%a5)

	move.l	VideoLineFunction@END.w(%a5),2+indexToAddress(gbemuJmpLcdLine)

/* - - return to emulation - - - - - - - - - - - - - - - - - - - - - -	*/
	move.w	#opsPerLine-1,%d6

	add.w	#videoClksPerLine,VideoTimerDiv@END.w(%a5)

	move.w	VideoTimerDeltaTima@END.w(%a5),%d0
	add.w	%d0,VideoTimerTima@END.w(%a5)
	jcc	LgTExit
	move.b	EmulationIo.tma@END.w(%a5),%d0
LgTLoop:
	add.b	%d0,VideoTimerTima@END.w(%a5)
	jcs	LgTLoop
	bset	#2,EmulationIo.if_@END.w(%a5)
LgTExit:

	move.b	VideoIntLycReload@END.w(%a5),VideoIntLycCount@END.w(%a5)
	jmp	indexToAddress(gbemuCodeCheckInt)

/* --------------------------------------------------------------------	*/

LineNone:
/*	a2	pointer into palm display
 */
	move.w	VideoFramesToSkip@END.w(%a5),%d0
	jne	LineQuit		/* skip frame? */

	move.l	VideoBufferCurPtr@END.w(%a5),%a2

	moveq	#widthInBytes/4/2-1,%d0
	moveq	#0,%d1

/* - - erase line - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
LnLoop:
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	dbra	%d0,LnLoop

/* - - update pointer - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	move.l	%a2,VideoBufferCurPtr@END.w(%a5)

	/* falls through to LineQuit */

/* --------------------------------------------------------------------	*/

LineQuit:
	subq.w	#1,VideoLyPrv@END.w(%a5)
	jcc	LineQuitReturn		/* rendering not finished? */

	lea	VideoUserPoll(%pc),%a2
	move.l	%a2,2+indexToAddress(gbemuJmpLcdLine)

	/* falls through to LineQuitReturn */

/* --------------------------------------------------------------------	*/

LineQuitReturn:
	move.b	VideoIntMaskSH@END.w(%a5),%d0
	and.b	VideoIntMaskStat@END.w(%a5),%d0
	or.b	%d0,EmulationIo.if_@END.w(%a5)

LineGapQuitReturn:
	move.w	#opsPerLine-1,%d6

	add.w	#videoClksPerLine,VideoTimerDiv@END.w(%a5)

	move.w	VideoTimerDeltaTima@END.w(%a5),%d0
	add.w	%d0,VideoTimerTima@END.w(%a5)
	jcc	LqrTExit
	move.b	EmulationIo.tma@END.w(%a5),%d0
LqrTLoop:
	add.b	%d0,VideoTimerTima@END.w(%a5)
	jcs	LqrTLoop
	bset	#2,EmulationIo.if_@END.w(%a5)
LqrTExit:

	subq.b	#1,VideoIntLycCount@END.w(%a5)
	jcc	LqrLycExit
	btst	#6,EmulationIo.stat@END.w(%a5)
	jeq	LqrLycExit
	move.b	VideoIntMaskStat@END.w(%a5),%d0
	or.b	%d0,EmulationIo.if_@END.w(%a5)
LqrLycExit:

	jmp	indexToAddress(gbemuCodeCheckInt)

/* --------------------------------------------------------------------	*/

LineOb:
/*	a0	pointer into palm display incl. offset
 *	a2	pointer into palm display
 *	a3	pointer to pixel line in object
 *	a6	pointer VideoObFirst
 */
	move.w	VideoFramesToSkip@END.w(%a5),%d0
	jne	LineQuit		/* skip frame? */

	move.l	%a0,-(%sp)

	move.l	VideoBufferCurPtr@END.w(%a5),%a2

	moveq	#widthInBytes/4/2-1,%d0
	moveq	#0,%d1

/* - - erase line - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
LoLoop:
	move.l	%d1,(%a2)+
	move.l	%d1,(%a2)+
	dbra	%d0,LoLoop

/* - - update offsets and pointers - - - - - - - - - - - - - - - - - -	*/
	move.l	%a2,VideoBufferCurPtr@END.w(%a5)

/* - - objects behind background/window - - - - - - - - - - - - - - - -	*/
	lea	-widthInBytes(%a2),%a2

	move.l	VideoObFirst@END.w(%a5),%a6
	move.w	VideoObYOffset@END.w(%a5),%d0
	move.w	(%a6,%d0.w),%d1
	jmi	LoBgQuit		/* no objects behind bg./wi.? */

LoBgLoop:
	lea	(%a6,%d1.w),%a3
	move.w	(%a3)+,%d1
	lea	(%a2,%d1.w),%a0
	move.l	(%a3)+,%d2
	and.l	%d2,(%a0)
	move.l	(%a3)+,%d2
	or.l	%d2,(%a0)		/* insert object */

	move.w	(%a3),%d1
	jpl	LoBgLoop		/* more objects? */
LoBgQuit:

/* - - objects in front of background/window - - - - - - - - - - - - -	*/
	addq.w	#2,%d0
	move.w	(%a6,%d0.w),%d1
	jmi	LoFgQuit		/* no obj's in front of bg./wi.? */

LoFgLoop:
	lea	(%a6,%d1.w),%a3
	move.w	(%a3)+,%d1
	lea	(%a2,%d1.w),%a0
	move.l	(%a3)+,%d2
	and.l	%d2,(%a0)
	move.l	(%a3)+,%d2
	or.l	%d2,(%a0)		/* insert object */

	move.w	(%a3),%d1
	jpl	LoFgLoop		/* more objects? */
LoFgQuit:

	addq.w	#2,%d0
	move.w	%d0,VideoObYOffset@END.w(%a5)

	move.l	(%sp)+,%a0
	jra	LineQuit

/* --------------------------------------------------------------------	*/
LineBgCh:
	move.w	VideoFramesToSkip@END.w(%a5),%d0
	jne	LineQuit		/* skip frame? */

	move.w	VideoWyPrv@END.w(%a5),%d0
	dbmi	%d0,LbcBg		/* window not reached? */
	move.w	%d0,VideoWyPrv@END.w(%a5)

	lea	LineBgWi(%pc),%a2
	move.l	%a2,2+indexToAddress(gbemuJmpLcdLine)
	jmp	(%a2)			/* now render with window */

LbcBg:
	move.w	%d0,VideoWyPrv@END.w(%a5)

	/* falls through to LineBg */

/* --------------------------------------------------------------------	*/

LineBg:
/*	a0	pointer to left tile pattern
 *	a1	pointer to right tile pattern
 *	a2	pointer into palm display
 *	a3	pointer to pixel line base
 *	a6	pointer to actual tile number in background
 */
	move.w	VideoFramesToSkip@END.w(%a5),%d0
	jne	LineQuit		/* skip frame? */

	move.l	%a0,-(%sp)
	move.l	%a1,-(%sp)

	move.l	VideoBufferCurPtr@END.w(%a5),%a2
	move.l	VideoBgTilePtr@END.w(%a5),%a3
	move.l	VideoBgMapStart@END.w(%a5),%a6
	add.w	VideoBgMapOffset@END.w(%a5),%a6

	move.w	(%a6)+,%d1
	lea	2(%a3,%d1.w),%a0	/* ptr to "old" right pattern */

	move.w	VideoBgCount2@END.w(%a5),%d0
	move.w	VideoBgCount1@END.w(%a5),%d2
	jmi	LbLoop2			/* no line wrap? */
	jra	LbNext1

/* - - render until wrap - - - - - - - - - - - - - - - - - - - - - - -	*/
LbLoop1:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */
LbNext1:
	dbra	%d2,LbLoop1

	lea	-videoMapWrap*2(%a6),%a6	/* wrap pointer */

/* - - render until finished - - - - - - - - - - - - - - - - - - - - -	*/
LbLoop2:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */

	dbra	%d0,LbLoop2

/* - - update pointers - - - - - - - - - - - - - - - - - - - - - - - -	*/
	move.l	%a2,VideoBufferCurPtr@END.w(%a5)

	lea	0x100*4(%a3),%a3
	cmp.l	VideoBgTileMax@END.w(%a5),%a3
	jcs	LbExit			/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	moveq	#videoMapWrap*2,%d0
	add.w	VideoBgMapOffset@END.w(%a5),%d0
	cmp.w	#videoMapWrap*videoMapWrap*2,%d0
	jcs	LbNewline		/* no vertical wrap? */
	sub.w	#videoMapWrap*videoMapWrap*2,%d0
LbNewline:
	move.w	%d0,VideoBgMapOffset@END.w(%a5)

LbExit:
	move.l	%a3,VideoBgTilePtr@END.w(%a5)

	move.l	(%sp)+,%a1
	move.l	(%sp)+,%a0
	jra	LineQuit

/* --------------------------------------------------------------------	*/

LineBgWi:
/*	a0	pointer to left tile pattern
 *	a1	pointer to right tile pattern
 *	a2	pointer into palm display
 *	a3	pointer to pixel line base
 *	a6	pointer to actual tile number in background
 */
	move.w	VideoFramesToSkip@END.w(%a5),%d0
	jne	LineQuit		/* skip frame? */

	move.l	%a0,-(%sp)
	move.l	%a1,-(%sp)

	move.l	VideoBufferCurPtr@END.w(%a5),%a2
	move.l	VideoBgTilePtr@END.w(%a5),%a3

	move.w	VideoBgWiCount2@END.w(%a5),%d0
	jmi	LbwWi			/* only window? */

	move.l	VideoBgMapStart@END.w(%a5),%a6
	add.w	VideoBgMapOffset@END.w(%a5),%a6

	move.w	(%a6)+,%d1
	lea	2(%a3,%d1.w),%a0	/* ptr to "old" right pattern */

	move.w	VideoBgWiCount1@END.w(%a5),%d2
	jmi	LbwLoop2		/* no wrap before window? */
	jra	LbwNext1

/* - - render until wrap - - - - - - - - - - - - - - - - - - - - - - -	*/
LbwLoop1:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */
LbwNext1:
	dbra	%d2,LbwLoop1

	lea	-videoMapWrap*2(%a6),%a6	/* wrap pointer */

/* - - render until window - - - - - - - - - - - - - - - - - - - - - -	*/
LbwLoop2:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */

	dbra	%d0,LbwLoop2

/* - - update background pointers - - - - - - - - - - - - - - - - - - -	*/
	lea	0x100*4(%a3),%a3
	cmp.l	VideoBgTileMax@END.w(%a5),%a3
	jcs	LbwBgExit		/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	moveq	#videoMapWrap*2,%d0
	add.w	VideoBgMapOffset@END.w(%a5),%d0
	cmp.w	#videoMapWrap*videoMapWrap*2,%d0
	jcs	LbwBgNewline		/* no vertical wrap? */
	sub.w	#videoMapWrap*videoMapWrap*2,%d0
LbwBgNewline:
	move.w	%d0,VideoBgMapOffset@END.w(%a5)

LbwBgExit:
	move.l	%a3,VideoBgTilePtr@END.w(%a5)

/* - - render window after background - - - - - - - - - - - - - - - - -	*/
	move.l	VideoWiTilePtr@END.w(%a5),%a3

	move.w	VideoBgWiCount3@END.w(%a5),%d0
	jmi	LbwExit			/* window not visible? */

	move.l	VideoWiMapStart@END.w(%a5),%a6
	add.w	VideoWiMapOffset@END.w(%a5),%a6

	move.w	VideoWiWxMask@END.w(%a5),%d2
	and.w	%d2,-(%a2)

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to first pattern */

	move.w	(%a0)+,%d1
	or.w	%d1,(%a2)+		/* store first pattern */
	jra	LbwNext3

/* - - render window only, but first update background pointers - - - -	*/
LbwWi:
	lea	0x100*4(%a3),%a3
	cmp.l	VideoBgTileMax@END.w(%a5),%a3
	jcs	LbwWiBgExit		/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	moveq	#videoMapWrap*2,%d0
	add.w	VideoBgMapOffset@END.w(%a5),%d0
	cmp.w	#videoMapWrap*videoMapWrap*2,%d0
	jcs	LbwWiBgNewline		/* no vertical wrap? */
	sub.w	#videoMapWrap*videoMapWrap*2,%d0
LbwWiBgNewline:
	move.w	%d0,VideoBgMapOffset@END.w(%a5)

LbwWiBgExit:
	move.l	%a3,VideoBgTilePtr@END.w(%a5)

	move.l	VideoWiTilePtr@END.w(%a5),%a3
	move.l	VideoWiMapStart@END.w(%a5),%a6
	add.w	VideoWiMapOffset@END.w(%a5),%a6

	move.w	(%a6)+,%d1
	lea	2(%a3,%d1.w),%a0	/* ptr to "old" right pattern */

	move.w	VideoBgWiCount3@END.w(%a5),%d0

/* - - render until finished - - - - - - - - - - - - - - - - - - - - -	*/
LbwLoop3:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */
LbwNext3:
	dbra	%d0,LbwLoop3

/* - - update picture and window pointers - - - - - - - - - - - - - - -	*/
LbwExit:
	move.l	%a2,VideoBufferCurPtr@END.w(%a5)

	lea	0x100*4(%a3),%a3
	cmp.l	VideoWiTileMax@END.w(%a5),%a3
	jcs	LbwWiExit		/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	add.w	#videoMapWrap*2,VideoWiMapOffset@END.w(%a5)

LbwWiExit:
	move.l	%a3,VideoWiTilePtr@END.w(%a5)

	move.l	(%sp)+,%a1
	move.l	(%sp)+,%a0
	jra	LineQuit

/* --------------------------------------------------------------------	*/

LineBgChOb:
	move.w	VideoFramesToSkip@END.w(%a5),%d0
	jne	LineQuit		/* skip frame? */

	move.w	VideoWyPrv@END.w(%a5),%d0
	dbmi	%d0,LbcoBg		/* window not reached? */
	move.w	%d0,VideoWyPrv@END.w(%a5)

	lea	LineBgWiOb(%pc),%a2
	move.l	%a2,2+indexToAddress(gbemuJmpLcdLine)
	jmp	(%a2)			/* now render with window */

LbcoBg:
	move.w	%d0,VideoWyPrv@END.w(%a5)

	/* falls through to LineBgOb */

/* --------------------------------------------------------------------	*/

LineBgOb:
#define lboRegs %a0/%a1/%a4
/*	a0	pointer to left tile pattern / palm display incl. offset
 *	a1	pointer to right tile pattern
 *	a2	pointer into palm display
 *	a3	pointer pixel line base / to pixel line in object
 *	a4	pointer to mask
 *	a6	pointer to actual tile number in background / VideoObFirst
 */
	move.w	VideoFramesToSkip@END.w(%a5),%d0
	jne	LineQuit		/* skip frame? */

	movem.l	lboRegs,-(%sp)

	move.l	VideoBufferCurPtr@END.w(%a5),%a2
	move.l	VideoBgTilePtr@END.w(%a5),%a3
	move.l	VideoBgMapStart@END.w(%a5),%a6
	add.w	VideoBgMapOffset@END.w(%a5),%a6

	move.w	(%a6)+,%d1
	lea	2(%a3,%d1.w),%a0	/* ptr to "old" right pattern */

	move.l	VideoObFirst@END.w(%a5),%a1
	move.w	VideoObYOffset@END.w(%a5),%d1
	move.w	(%a1,%d1.w),%d1
	jpl	LboMasking		/* objects behind bg./wi.? */

	move.w	VideoBgCount2@END.w(%a5),%d0
	move.w	VideoBgCount1@END.w(%a5),%d2
	jmi	LboLoop2		/* no line wrap? */
	jra	LboNext1

/* - - render until wrap - - - - - - - - - - - - - - - - - - - - - - -	*/
LboLoop1:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */
LboNext1:
	dbra	%d2,LboLoop1

	lea	-videoMapWrap*2(%a6),%a6	/* wrap pointer */

/* - - render until finished - - - - - - - - - - - - - - - - - - - - -	*/
LboLoop2:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */

	dbra	%d0,LboLoop2

/* - - update pointers - - - - - - - - - - - - - - - - - - - - - - - -	*/
	move.l	%a2,VideoBufferCurPtr@END.w(%a5)

	lea	0x100*4(%a3),%a3
	cmp.l	VideoBgTileMax@END.w(%a5),%a3
	jcs	LboExit			/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	moveq	#videoMapWrap*2,%d0
	add.w	VideoBgMapOffset@END.w(%a5),%d0
	cmp.w	#videoMapWrap*videoMapWrap*2,%d0
	jcs	LboNewline		/* no vertical wrap? */
	sub.w	#videoMapWrap*videoMapWrap*2,%d0
LboNewline:
	move.w	%d0,VideoBgMapOffset@END.w(%a5)

LboExit:
	move.l	%a3,VideoBgTilePtr@END.w(%a5)

/* - - prepare for objects - - - - - - - - - - - - - - - - - - - - - -	*/
	lea	-widthInBytes(%a2),%a2

	move.l	VideoObFirst@END.w(%a5),%a6
	jra	LboForeground

/* - - render and save mask for this line - - - - - - - - - - - - - - -	*/
LboMasking:
	lea	VideoBgWiMask@END.w(%a5),%a4

	move.w	VideoBgCount2@END.w(%a5),%d0
	move.w	VideoBgCount1@END.w(%a5),%d2
	jmi	LboMLoop2		/* no line wrap? */
	jra	LboMNext1

/* - - render until wrap - - - - - - - - - - - - - - - - - - - - - - -	*/
LboMLoop1:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	8*0x100*4(%a1),%d1
	or.w	8*0x100*4(%a0),%d1
	move.w	%d1,(%a4)+		/* store mask */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */
LboMNext1:
	dbra	%d2,LboMLoop1

	lea	-videoMapWrap*2(%a6),%a6	/* wrap pointer */

/* - - render until finished - - - - - - - - - - - - - - - - - - - - -	*/
LboMLoop2:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	8*0x100*4(%a1),%d1
	or.w	8*0x100*4(%a0),%d1
	move.w	%d1,(%a4)+		/* store mask */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */

	dbra	%d0,LboMLoop2

/* - - update pointers - - - - - - - - - - - - - - - - - - - - - - - -	*/
	move.l	%a2,VideoBufferCurPtr@END.w(%a5)

	lea	0x100*4(%a3),%a3
	cmp.l	VideoBgTileMax@END.w(%a5),%a3
	jcs	LboMExit		/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	moveq	#videoMapWrap*2,%d0
	add.w	VideoBgMapOffset@END.w(%a5),%d0
	cmp.w	#videoMapWrap*videoMapWrap*2,%d0
	jcs	LboMNewline		/* no vertical wrap? */
	sub.w	#videoMapWrap*videoMapWrap*2,%d0
LboMNewline:
	move.w	%d0,VideoBgMapOffset@END.w(%a5)

LboMExit:
	move.l	%a3,VideoBgTilePtr@END.w(%a5)

/* - - objects behind background/window - - - - - - - - - - - - - - - -	*/
	moveq	#widthInBytes,%d1
	sub.w	%d1,%a2
	sub.w	%d1,%a4

	move.l	VideoObFirst@END.w(%a5),%a6
	move.w	VideoObYOffset@END.w(%a5),%d0
	move.w	(%a6,%d0.w),%d1

LboBgLoop:
	lea	(%a6,%d1.w),%a3
	move.w	(%a3)+,%d1
	lea	(%a2,%d1.w),%a0
	move.l	(%a4,%d1.w),%d0
	move.l	(%a3)+,%d2
	or.l	%d0,%d2
	and.l	%d2,(%a0)
	move.l	(%a3)+,%d2
	not.l	%d0
	and.l	%d0,%d2
	or.l	%d2,(%a0)		/* insert object */

	move.w	(%a3),%d1
	jpl	LboBgLoop		/* more objects? */

/* - - objects in front of background/window - - - - - - - - - - - - -	*/
LboForeground:
	move.w	VideoObYOffset@END.w(%a5),%d0
	addq.w	#2,%d0
	move.w	(%a6,%d0.w),%d1
	jmi	LboFgQuit		/* no obj's in front of bg./wi.? */

LboFgLoop:
	lea	(%a6,%d1.w),%a3
	move.w	(%a3)+,%d1
	lea	(%a2,%d1.w),%a0
	move.l	(%a3)+,%d2
	and.l	%d2,(%a0)
	move.l	(%a3)+,%d2
	or.l	%d2,(%a0)		/* insert object */

	move.w	(%a3),%d1
	jpl	LboFgLoop		/* more objects? */
LboFgQuit:

	addq.w	#2,%d0
	move.w	%d0,VideoObYOffset@END.w(%a5)

	movem.l	(%sp)+,lboRegs
	jra	LineQuit

/* --------------------------------------------------------------------	*/

LineBgWiOb:
#define lbwoRegs %a0/%a1/%a4
/*	a0	pointer to left tile pattern / palm display incl. offset
 *	a1	pointer to right tile pattern
 *	a2	pointer into palm display
 *	a3	pointer pixel line base / to pixel line in object
 *	a4	pointer to mask
 *	a6	pointer to actual tile number in background / VideoObFirst
 */
	move.w	VideoFramesToSkip@END.w(%a5),%d0
	jne	LineQuit		/* skip frame? */

	movem.l	lbwoRegs,-(%sp)

	move.l	VideoBufferCurPtr@END.w(%a5),%a2
	move.l	VideoBgTilePtr@END.w(%a5),%a3

	move.l	VideoObFirst@END.w(%a5),%a1
	move.w	VideoObYOffset@END.w(%a5),%d1
	move.w	(%a1,%d1.w),%d1
	jpl	LbwoMasking		/* objects behind bg./wi.? */

	move.w	VideoBgWiCount2@END.w(%a5),%d0
	jmi	LbwoWi			/* only window? */

	move.l	VideoBgMapStart@END.w(%a5),%a6
	add.w	VideoBgMapOffset@END.w(%a5),%a6

	move.w	(%a6)+,%d1
	lea	2(%a3,%d1.w),%a0	/* ptr to "old" right pattern */

	move.w	VideoBgWiCount1@END.w(%a5),%d2
	jmi	LbwoLoop2		/* no wrap before window? */
	jra	LbwoNext1

/* - - render until wrap - - - - - - - - - - - - - - - - - - - - - - -	*/
LbwoLoop1:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */
LbwoNext1:
	dbra	%d2,LbwoLoop1

	lea	-videoMapWrap*2(%a6),%a6	/* wrap pointer */

/* - - render until window - - - - - - - - - - - - - - - - - - - - - -	*/
LbwoLoop2:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */

	dbra	%d0,LbwoLoop2

/* - - update background pointers - - - - - - - - - - - - - - - - - - -	*/
	lea	0x100*4(%a3),%a3
	cmp.l	VideoBgTileMax@END.w(%a5),%a3
	jcs	LbwoBgExit		/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	moveq	#videoMapWrap*2,%d0
	add.w	VideoBgMapOffset@END.w(%a5),%d0
	cmp.w	#videoMapWrap*videoMapWrap*2,%d0
	jcs	LbwoBgNewline		/* no vertical wrap? */
	sub.w	#videoMapWrap*videoMapWrap*2,%d0
LbwoBgNewline:
	move.w	%d0,VideoBgMapOffset@END.w(%a5)

LbwoBgExit:
	move.l	%a3,VideoBgTilePtr@END.w(%a5)

/* - - render window after background - - - - - - - - - - - - - - - - -	*/
	move.l	VideoWiTilePtr@END.w(%a5),%a3

	move.w	VideoBgWiCount3@END.w(%a5),%d0
	jmi	LbwoExit		/* window not visible? */

	move.l	VideoWiMapStart@END.w(%a5),%a6
	add.w	VideoWiMapOffset@END.w(%a5),%a6

	move.w	VideoWiWxMask@END.w(%a5),%d2
	and.w	%d2,-(%a2)

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to first pattern */

	move.w	(%a0)+,%d1
	or.w	%d1,(%a2)+		/* store first pattern */
	jra	LbwoNext3

/* - - render window only, but first update background pointers - - - -	*/
LbwoWi:
	lea	0x100*4(%a3),%a3
	cmp.l	VideoBgTileMax@END.w(%a5),%a3
	jcs	LbwoWiBgExit		/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	moveq	#videoMapWrap*2,%d0
	add.w	VideoBgMapOffset@END.w(%a5),%d0
	cmp.w	#videoMapWrap*videoMapWrap*2,%d0
	jcs	LbwoWiBgNewline		/* no vertical wrap? */
	sub.w	#videoMapWrap*videoMapWrap*2,%d0
LbwoWiBgNewline:
	move.w	%d0,VideoBgMapOffset@END.w(%a5)

LbwoWiBgExit:
	move.l	%a3,VideoBgTilePtr@END.w(%a5)

	move.l	VideoWiTilePtr@END.w(%a5),%a3
	move.l	VideoWiMapStart@END.w(%a5),%a6
	add.w	VideoWiMapOffset@END.w(%a5),%a6

	move.w	(%a6)+,%d1
	lea	2(%a3,%d1.w),%a0	/* ptr to "old" right pattern */

	move.w	VideoBgWiCount3@END.w(%a5),%d0

/* - - render until finished - - - - - - - - - - - - - - - - - - - - -	*/
LbwoLoop3:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */
LbwoNext3:
	dbra	%d0,LbwoLoop3

/* - - update picture and window pointers - - - - - - - - - - - - - - -	*/
LbwoExit:
	move.l	%a2,VideoBufferCurPtr@END.w(%a5)

	lea	0x100*4(%a3),%a3
	cmp.l	VideoWiTileMax@END.w(%a5),%a3
	jcs	LbwoWiExit		/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	add.w	#videoMapWrap*2,VideoWiMapOffset@END.w(%a5)

LbwoWiExit:
	move.l	%a3,VideoWiTilePtr@END.w(%a5)

/* - - prepare for objects - - - - - - - - - - - - - - - - - - - - - -	*/
	lea	-widthInBytes(%a2),%a2

	move.l	VideoObFirst@END.w(%a5),%a6
	jra	LbwoForeground

/* - - render and save mask for this line - - - - - - - - - - - - - - -	*/
LbwoMasking:
	lea	VideoBgWiMask@END.w(%a5),%a4

	move.w	VideoBgWiCount2@END.w(%a5),%d0
	jmi	LbwoMWi			/* only window? */

	move.l	VideoBgMapStart@END.w(%a5),%a6
	add.w	VideoBgMapOffset@END.w(%a5),%a6

	move.w	(%a6)+,%d1
	lea	2(%a3,%d1.w),%a0	/* ptr to "old" right pattern */

	move.w	VideoBgWiCount1@END.w(%a5),%d2
	jmi	LbwoMLoop2		/* no wrap before window? */
	jra	LbwoMNext1

/* - - render until wrap - - - - - - - - - - - - - - - - - - - - - - -	*/
LbwoMLoop1:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	8*0x100*4(%a1),%d1
	or.w	8*0x100*4(%a0),%d1
	move.w	%d1,(%a4)+		/* store mask */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */
LbwoMNext1:
	dbra	%d2,LbwoMLoop1

	lea	-videoMapWrap*2(%a6),%a6	/* wrap pointer */

/* - - render until window - - - - - - - - - - - - - - - - - - - - - -	*/
LbwoMLoop2:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	8*0x100*4(%a1),%d1
	or.w	8*0x100*4(%a0),%d1
	move.w	%d1,(%a4)+		/* store mask */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */

	dbra	%d0,LbwoMLoop2

/* - - update background pointers - - - - - - - - - - - - - - - - - - -	*/
	lea	0x100*4(%a3),%a3
	cmp.l	VideoBgTileMax@END.w(%a5),%a3
	jcs	LbwoMBgExit		/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	moveq	#videoMapWrap*2,%d0
	add.w	VideoBgMapOffset@END.w(%a5),%d0
	cmp.w	#videoMapWrap*videoMapWrap*2,%d0
	jcs	LbwoMBgNewline		/* no vertical wrap? */
	sub.w	#videoMapWrap*videoMapWrap*2,%d0
LbwoMBgNewline:
	move.w	%d0,VideoBgMapOffset@END.w(%a5)

LbwoMBgExit:
	move.l	%a3,VideoBgTilePtr@END.w(%a5)

/* - - render window after background - - - - - - - - - - - - - - - - -	*/
	move.l	VideoWiTilePtr@END.w(%a5),%a3

	move.w	VideoBgWiCount3@END.w(%a5),%d0
	jmi	LbwoMExit		/* window not visible? */

	move.l	VideoWiMapStart@END.w(%a5),%a6
	add.w	VideoWiMapOffset@END.w(%a5),%a6

	move.w	VideoWiWxMask@END.w(%a5),%d2
	and.w	%d2,-(%a2)
	and.w	%d2,-(%a4)

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to first pattern */

	move.w	8*0x100*4(%a0),%d1
	or.w	%d1,(%a4)+		/* store mask */

	move.w	(%a0)+,%d1
	or.w	%d1,(%a2)+		/* store first pattern */
	jra	LbwoMNext3

/* - - render window only, but first update background pointers - - - -	*/
LbwoMWi:
	lea	0x100*4(%a3),%a3
	cmp.l	VideoBgTileMax@END.w(%a5),%a3
	jcs	LbwoMWiBgExit		/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	moveq	#videoMapWrap*2,%d0
	add.w	VideoBgMapOffset@END.w(%a5),%d0
	cmp.w	#videoMapWrap*videoMapWrap*2,%d0
	jcs	LbwoMWiBgNewline	/* no vertical wrap? */
	sub.w	#videoMapWrap*videoMapWrap*2,%d0
LbwoMWiBgNewline:
	move.w	%d0,VideoBgMapOffset@END.w(%a5)

LbwoMWiBgExit:
	move.l	%a3,VideoBgTilePtr@END.w(%a5)

	move.l	VideoWiTilePtr@END.w(%a5),%a3
	move.l	VideoWiMapStart@END.w(%a5),%a6
	add.w	VideoWiMapOffset@END.w(%a5),%a6

	move.w	(%a6)+,%d1
	lea	2(%a3,%d1.w),%a0	/* ptr to "old" right pattern */

	move.w	VideoBgWiCount3@END.w(%a5),%d0

/* - - render until finished - - - - - - - - - - - - - - - - - - - - -	*/
LbwoMLoop3:
	move.l	%a0,%a1			/* right becomes left */

	move.w	(%a6)+,%d1
	lea	(%a3,%d1.w),%a0		/* pointer to right pattern */

	move.w	8*0x100*4(%a1),%d1
	or.w	8*0x100*4(%a0),%d1
	move.w	%d1,(%a4)+		/* store mask */

	move.w	(%a1)+,%d1
	or.w	(%a0)+,%d1
	move.w	%d1,(%a2)+		/* join patterns */
LbwoMNext3:
	dbra	%d0,LbwoMLoop3

/* - - update picture and window pointers - - - - - - - - - - - - - - -	*/
LbwoMExit:
	move.l	%a2,VideoBufferCurPtr@END.w(%a5)

	lea	0x100*4(%a3),%a3
	cmp.l	VideoWiTileMax@END.w(%a5),%a3
	jcs	LbwoMWiExit		/* not last pixel line? */

	lea	-8*0x100*4(%a3),%a3

	add.w	#videoMapWrap*2,VideoWiMapOffset@END.w(%a5)

LbwoMWiExit:
	move.l	%a3,VideoWiTilePtr@END.w(%a5)

/* - - objects behind background/window - - - - - - - - - - - - - - - -	*/
	moveq	#widthInBytes,%d1
	sub.w	%d1,%a2
	sub.w	%d1,%a4

	move.l	VideoObFirst@END.w(%a5),%a6
	move.w	VideoObYOffset@END.w(%a5),%d0
	move.w	(%a6,%d0.w),%d1

LbwoBgLoop:
	lea	(%a6,%d1.w),%a3
	move.w	(%a3)+,%d1
	lea	(%a2,%d1.w),%a0
	move.l	(%a4,%d1.w),%d0
	move.l	(%a3)+,%d2
	or.l	%d0,%d2
	and.l	%d2,(%a0)
	move.l	(%a3)+,%d2
	not.l	%d0
	and.l	%d0,%d2
	or.l	%d2,(%a0)		/* insert object */

	move.w	(%a3),%d1
	jpl	LbwoBgLoop		/* more objects? */

/* - - objects in front of background/window - - - - - - - - - - - - -	*/
LbwoForeground:
	move.w	VideoObYOffset@END.w(%a5),%d0
	addq.w	#2,%d0
	move.w	(%a6,%d0.w),%d1
	jmi	LbwoFgQuit		/* no obj's in front of bg./wi.? */

LbwoFgLoop:
	lea	(%a6,%d1.w),%a3
	move.w	(%a3)+,%d1
	lea	(%a2,%d1.w),%a0
	move.l	(%a3)+,%d2
	and.l	%d2,(%a0)
	move.l	(%a3)+,%d2
	or.l	%d2,(%a0)		/* insert object */

	move.w	(%a3),%d1
	jpl	LbwoFgLoop		/* more objects? */
LbwoFgQuit:

	addq.w	#2,%d0
	move.w	%d0,VideoObYOffset@END.w(%a5)

	movem.l	(%sp)+,lbwoRegs
	jra	LineQuit

/* === Functions for converting =======================================	*/

	.globl	VideoGrayCopyTable
VideoGrayCopyTable:
	dc.w	0x0000,0x0001,0x0004,0x0005,0x0010,0x0011,0x0014,0x0015
	dc.w	0x0040,0x0041,0x0044,0x0045,0x0050,0x0051,0x0054,0x0055
	dc.w	0x0100,0x0101,0x0104,0x0105,0x0110,0x0111,0x0114,0x0115
	dc.w	0x0140,0x0141,0x0144,0x0145,0x0150,0x0151,0x0154,0x0155
	dc.w	0x0400,0x0401,0x0404,0x0405,0x0410,0x0411,0x0414,0x0415
	dc.w	0x0440,0x0441,0x0444,0x0445,0x0450,0x0451,0x0454,0x0455
	dc.w	0x0500,0x0501,0x0504,0x0505,0x0510,0x0511,0x0514,0x0515
	dc.w	0x0540,0x0541,0x0544,0x0545,0x0550,0x0551,0x0554,0x0555
	dc.w	0x1000,0x1001,0x1004,0x1005,0x1010,0x1011,0x1014,0x1015
	dc.w	0x1040,0x1041,0x1044,0x1045,0x1050,0x1051,0x1054,0x1055
	dc.w	0x1100,0x1101,0x1104,0x1105,0x1110,0x1111,0x1114,0x1115
	dc.w	0x1140,0x1141,0x1144,0x1145,0x1150,0x1151,0x1154,0x1155
	dc.w	0x1400,0x1401,0x1404,0x1405,0x1410,0x1411,0x1414,0x1415
	dc.w	0x1440,0x1441,0x1444,0x1445,0x1450,0x1451,0x1454,0x1455
	dc.w	0x1500,0x1501,0x1504,0x1505,0x1510,0x1511,0x1514,0x1515
	dc.w	0x1540,0x1541,0x1544,0x1545,0x1550,0x1551,0x1554,0x1555
	dc.w	0x4000,0x4001,0x4004,0x4005,0x4010,0x4011,0x4014,0x4015
	dc.w	0x4040,0x4041,0x4044,0x4045,0x4050,0x4051,0x4054,0x4055
	dc.w	0x4100,0x4101,0x4104,0x4105,0x4110,0x4111,0x4114,0x4115
	dc.w	0x4140,0x4141,0x4144,0x4145,0x4150,0x4151,0x4154,0x4155
	dc.w	0x4400,0x4401,0x4404,0x4405,0x4410,0x4411,0x4414,0x4415
	dc.w	0x4440,0x4441,0x4444,0x4445,0x4450,0x4451,0x4454,0x4455
	dc.w	0x4500,0x4501,0x4504,0x4505,0x4510,0x4511,0x4514,0x4515
	dc.w	0x4540,0x4541,0x4544,0x4545,0x4550,0x4551,0x4554,0x4555
	dc.w	0x5000,0x5001,0x5004,0x5005,0x5010,0x5011,0x5014,0x5015
	dc.w	0x5040,0x5041,0x5044,0x5045,0x5050,0x5051,0x5054,0x5055
	dc.w	0x5100,0x5101,0x5104,0x5105,0x5110,0x5111,0x5114,0x5115
	dc.w	0x5140,0x5141,0x5144,0x5145,0x5150,0x5151,0x5154,0x5155
	dc.w	0x5400,0x5401,0x5404,0x5405,0x5410,0x5411,0x5414,0x5415
	dc.w	0x5440,0x5441,0x5444,0x5445,0x5450,0x5451,0x5454,0x5455
	dc.w	0x5500,0x5501,0x5504,0x5505,0x5510,0x5511,0x5514,0x5515
	dc.w	0x5540,0x5541,0x5544,0x5545,0x5550,0x5551,0x5554,0x5555

/* -------------------------------------------------------------------- */

ColorTab:
	dc.w	Color0000-.
	dc.w	Color0001-.
	dc.w	Color0010-.
	dc.w	Color0011-.
	dc.w	Color0100-.
	dc.w	Color0101-.
	dc.w	Color0110-.
	dc.w	Color0111-.
	dc.w	Color1000-.
	dc.w	Color1001-.
	dc.w	Color1010-.
	dc.w	Color1011-.
	dc.w	Color1100-.
	dc.w	Color1101-.
	dc.w	Color1110-.
	dc.w	Color1111-.

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color0000:
	sub.b	%d0,%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color0001:
	move.b	1(%a3),%d0
	or.b	(%a3),%d0
	not.b	%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color0010:
	move.b	1(%a3),%d0
	not.b	%d0
	and.b	(%a3),%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color0011:
	move.b	1(%a3),%d0
	not.b	%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color0100:
	move.b	(%a3),%d0
	not.b	%d0
	and.b	1(%a3),%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color0101:
	move.b	(%a3),%d0
	not.b	%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color0110:
	move.b	1(%a3),%d0
	move.b	(%a3),%d1
	eor.b	%d1,%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color0111:
	move.b	1(%a3),%d0
	and.b	(%a3),%d0
	not.b	%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color1000:
	move.b	1(%a3),%d0
	and.b	(%a3),%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color1001:
	move.b	1(%a3),%d0
	move.b	(%a3),%d1
	eor.b	%d1,%d0
	not.b	%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color1010:
	move.b	(%a3),%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color1011:
	move.b	1(%a3),%d0
	not.b	%d0
	or.b	(%a3),%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color1100:
	move.b	1(%a3),%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color1101:
	move.b	(%a3),%d0
	not.b	%d0
	or.b	1(%a3),%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color1110:
	move.b	1(%a3),%d0
	or.b	(%a3),%d0
	rts

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Color1111:
	move.b	#0xff,%d0
	rts

/* --------------------------------------------------------------------	*/

	.globl	VideoConvertBgWi
VideoConvertBgWi:
/*	d0	(scratch)
 *	d1	(scratch)
 *	d2	shift left 0..7 pixel
 *	d3.w	(counter)
 *	a0,a1	addresses of color functions
 *	a2	pointer to destination, will be increased by 4
 *	a3	pointer to source, will be increased by 8*2
 *	a4	pointer to VideoGrayCopyTable
 */
	move.w	#8-1,%d3
VcbwLoop:
	moveq	#0,%d0
	moveq	#0,%d1
	jsr	(%a0)
	swap	%d0
	jsr	(%a1)
	add.l	%d0,%d0
	move.w	(%a4,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a4,%d0.w),%d1
	lsl.l	%d2,%d1
	move.l	%d1,(%a2)
	moveq	#0,%d1
	move.b	(%a3)+,%d1
	or.b	(%a3)+,%d1
	add.w	%d1,%d1
	move.w	(%a4,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsl.l	%d2,%d1
	move.l	%d1,8*0x100*4(%a2)

	lea	0x100*4(%a2),%a2

	dbra	%d3,VcbwLoop
	lea	4-8*0x100*4(%a2),%a2
	rts

/* --------------------------------------------------------------------	*/

ConvertBgWiAllMarked:
	move.w	%d0,-(%sp)
	move.w	%d1,-(%sp)
	move.l	%a4,-(%sp)

	lea	VideoGrayCopyTable(%pc),%a4

	move.w	#0x80-1,%d3
	move.l	VideoBgWiTileLow@END.w(%a5),%a3
CbwamLLoop:
	move.b	(%a6)+,%d0
	jmi	CbwamLNext
	cmp.b	%d0,%d2
	jne	CbwamLConv		/* conversion necessary? */
CbwamLNext:
	addq.w	#4,%a2
	lea	2*8(%a3),%a3
	dbra	%d3,CbwamLLoop
	jra	CbwamLReady
CbwamLConv:
	move.b	%d2,-1(%a6)
	swap	%d3
	jbsr	VideoConvertBgWi(%pc)
	swap	%d3
	dbra	%d3,CbwamLLoop
CbwamLReady:				/* convert tiles 0x00..0x7f */

	move.w	#0x80-1,%d3
	move.l	VideoBgWiTileHigh@END.w(%a5),%a3
	lea	0x800(%a3),%a3
CbwamHLoop:
	move.b	(%a6)+,%d0
	jmi	CbwamHNext
	cmp.b	%d0,%d2
	jne	CbwamHConv		/* conversion necessary? */
CbwamHNext:
	addq.w	#4,%a2
	lea	2*8(%a3),%a3
	dbra	%d3,CbwamHLoop
	jra	CbwamHReady
CbwamHConv:
	move.b	%d2,-1(%a6)
	swap	%d3
	jbsr	VideoConvertBgWi(%pc)
	swap	%d3
	dbra	%d3,CbwamHLoop
CbwamHReady:				/* convert tiles 0x80..0xff */

	move.l	(%sp)+,%a4
	move.w	(%sp)+,%d1
	move.w	(%sp)+,%d0
	rts

/* --------------------------------------------------------------------	*/

ConvertBgWiAllUsed:
	move.l	%a4,-(%sp)

	lea	VideoGrayCopyTable(%pc),%a4

	move.w	#0x80-1,%d3
	move.l	VideoBgWiTileLow@END.w(%a5),%a3
CbwauLLoop:
	move.b	(%a6)+,%d0
	jmi	CbwauLNext
	cmp.b	%d0,%d2
	jeq	CbwauLConv		/* conversion necessary? */
CbwauLNext:
	addq.w	#4,%a2
	lea	2*8(%a3),%a3
	dbra	%d3,CbwauLLoop
	jra	CbwauLReady
CbwauLConv:
	move.b	%d2,-1(%a6)
	swap	%d3
	jbsr	VideoConvertBgWi(%pc)
	swap	%d3
	dbra	%d3,CbwauLLoop
CbwauLReady:				/* convert tiles 0x00..0x7f */

	move.w	#0x80-1,%d3
	move.l	VideoBgWiTileHigh@END.w(%a5),%a3
	lea	0x800(%a3),%a3
CbwauHLoop:
	move.b	(%a6)+,%d0
	jmi	CbwauHNext
	cmp.b	%d0,%d2
	jeq	CbwauHConv		/* conversion necessary? */
CbwauHNext:
	addq.w	#4,%a2
	lea	2*8(%a3),%a3
	dbra	%d3,CbwauHLoop
	jra	CbwauHReady
CbwauHConv:
	move.b	%d2,-1(%a6)
	swap	%d3
	jbsr	VideoConvertBgWi(%pc)
	swap	%d3
	dbra	%d3,CbwauHLoop
CbwauHReady:				/* convert tiles 0x80..0xff */

	move.l	(%sp)+,%a4
	rts

/* --------------------------------------------------------------------	*/

ReverseTable:
	dc.b	0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0
	dc.b	0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0
	dc.b	0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8
	dc.b	0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8
	dc.b	0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4
	dc.b	0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4
	dc.b	0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec
	dc.b	0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc
	dc.b	0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2
	dc.b	0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2
	dc.b	0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea
	dc.b	0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa
	dc.b	0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6
	dc.b	0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6
	dc.b	0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee
	dc.b	0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe
	dc.b	0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1
	dc.b	0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1
	dc.b	0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9
	dc.b	0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9
	dc.b	0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5
	dc.b	0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5
	dc.b	0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed
	dc.b	0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd
	dc.b	0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3
	dc.b	0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3
	dc.b	0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb
	dc.b	0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb
	dc.b	0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7
	dc.b	0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7
	dc.b	0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef
	dc.b	0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff

/* --------------------------------------------------------------------	*/

	.globl	VideoConvertOb
VideoConvertOb:
/*	d0	object flags / (shifted pattern)
 *	d1	(shifted mask)
 *	d2	(shift left/right 0..7 pixel)
 *	d3.w	counter 16-1 / 8-1 / 1-1
 *	d4	object x+8 / (screen offset)
 *	d5	(mask)
 *	a0,a1	addresses of color functions
 *	a2	pointer to destination, may be increased
 *	a3	pointer to source, may be increased
 *	a4	pointer to VideoGrayCopyTable
 *	a6	(pointer to ReverseTable)
 */
	tst.b	%d4
	jeq	VcoQuit
	cmp.b	#videoScreenWidth+8,%d4
	jcc	VcoQuit
	moveq	#0,%d5
	moveq	#7,%d2
	cmp.b	#videoScreenWidth,%d4
	jcc	VcoRight
	sub.b	#9,%d4
	jcc	VcoMid
	not.b	%d4
	and.b	%d4,%d2
	addq.b	#8,%d2
	moveq	#0,%d4
	jra	VcoLeft
VcoQuit:
	rts

VcoMid:
	eor.b	%d2,%d4
	and.b	%d4,%d2
	lsr.b	#3,%d4
	ext.w	%d4
	add.w	%d4,%d4

VcoLeft:
	add.w	%d2,%d2

	moveq	#0x40,%d1
	and.b	%d0,%d1
	jeq	VcoLXN
	moveq	#0x20,%d1
	and.b	%d0,%d1
	jne	VcoLFF
	jra	VcoLNF
VcoLXN:
	moveq	#0x20,%d1
	and.b	%d0,%d1
	jne	VcoLFN

/* - - convert shifting left x normal, y normal - - - - - - - - - - - -	*/
VcoLNNLoop:
	move.w	%d4,(%a2)+

	moveq	#0,%d1
	move.b	(%a3),%d5
	or.b	1(%a3),%d5
	move.w	%d5,%d1
	add.w	%d1,%d1
	move.w	(%a4,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsl.l	%d2,%d1
	not.l	%d1
	move.l	%d1,(%a2)+

	moveq	#0,%d0
	moveq	#0,%d1
	jsr	(%a0)
	and.b	%d5,%d0
	swap	%d0
	jsr	(%a1)
	and.b	%d5,%d0
	add.l	%d0,%d0
	move.w	(%a4,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a4,%d0.w),%d1
	lsl.l	%d2,%d1
	move.l	%d1,(%a2)+

	addq.w	#2,%a3
	addq.w	#4,%a2
	dbra	%d3,VcoLNNLoop
	rts

/* - - convert shifting left x normal, y flipped - - - - - - - - - - -	*/
VcoLNF:
	move.w	%d3,%d0
	addq.w	#1,%d0
	add.w	%d0,%d0
	add.w	%d0,%a3
VcoLNFLoop:
	move.w	%d4,(%a2)+

	moveq	#0,%d1
	move.b	-(%a3),%d5
	or.b	-(%a3),%d5
	move.w	%d5,%d1
	add.w	%d1,%d1
	move.w	(%a4,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsl.l	%d2,%d1
	not.l	%d1
	move.l	%d1,(%a2)+

	moveq	#0,%d0
	moveq	#0,%d1
	jsr	(%a0)
	and.b	%d5,%d0
	swap	%d0
	jsr	(%a1)
	and.b	%d5,%d0
	add.l	%d0,%d0
	move.w	(%a4,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a4,%d0.w),%d1
	lsl.l	%d2,%d1
	move.l	%d1,(%a2)+

	addq.w	#4,%a2
	dbra	%d3,VcoLNFLoop
	rts

/* - - convert shifting left x flipped, y normal - - - - - - - - - - -	*/
VcoLFN:
	lea	ReverseTable(%pc),%a6
VcoLFNLoop:
	move.w	%d4,(%a2)+

	moveq	#0,%d1
	move.b	(%a3),%d5
	or.b	1(%a3),%d5
	move.w	%d5,%d1
	move.b	(%a6,%d1.w),%d1
	add.w	%d1,%d1
	move.w	(%a4,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsl.l	%d2,%d1
	not.l	%d1
	move.l	%d1,(%a2)+

	moveq	#0,%d0
	moveq	#0,%d1
	jsr	(%a0)
	and.b	%d5,%d0
	move.b	(%a6,%d0.w),%d0
	swap	%d0
	jsr	(%a1)
	and.b	%d5,%d0
	move.b	(%a6,%d0.w),%d0
	add.l	%d0,%d0
	move.w	(%a4,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a4,%d0.w),%d1
	lsl.l	%d2,%d1
	move.l	%d1,(%a2)+

	addq.w	#2,%a3
	addq.w	#4,%a2
	dbra	%d3,VcoLFNLoop
	rts

/* - - convert shifting left x flipped, y flipped - - - - - - - - - - -	*/
VcoLFF:
	move.w	%d3,%d0
	addq.w	#1,%d0
	add.w	%d0,%d0
	add.w	%d0,%a3
	lea	ReverseTable(%pc),%a6
VcoLFFLoop:
	move.w	%d4,(%a2)+

	moveq	#0,%d1
	move.b	-(%a3),%d5
	or.b	-(%a3),%d5
	move.w	%d5,%d1
	move.b	(%a6,%d1.w),%d1
	add.w	%d1,%d1
	move.w	(%a4,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsl.l	%d2,%d1
	not.l	%d1
	move.l	%d1,(%a2)+

	moveq	#0,%d0
	moveq	#0,%d1
	jsr	(%a0)
	and.b	%d5,%d0
	move.b	(%a6,%d0.w),%d0
	swap	%d0
	jsr	(%a1)
	and.b	%d5,%d0
	move.b	(%a6,%d0.w),%d0
	add.l	%d0,%d0
	move.w	(%a4,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a4,%d0.w),%d1
	lsl.l	%d2,%d1
	move.l	%d1,(%a2)+

	addq.w	#4,%a2
	dbra	%d3,VcoLFFLoop
	rts

/* - - convert shifting right - - - - - - - - - - - - - - - - - - - - -	*/
VcoRight:
	and.b	%d4,%d2
	add.w	%d2,%d2
	moveq	#2*(videoScreenWidth/8-2),%d4

	moveq	#0x40,%d1
	and.b	%d0,%d1
	jeq	VcoRXN
	moveq	#0x20,%d1
	and.b	%d0,%d1
	jne	VcoRFF
	jra	VcoRNF
VcoRXN:
	moveq	#0x20,%d1
	and.b	%d0,%d1
	jne	VcoRFN

/* - - convert shifting right x normal, y normal - - - - - - - - - - -	*/
VcoRNNLoop:
	move.w	%d4,(%a2)+

	move.b	(%a3),%d5
	or.b	1(%a3),%d5
	move.w	%d5,%d1
	add.w	%d1,%d1
	move.w	(%a4,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsr.l	%d2,%d1
	not.l	%d1
	move.l	%d1,(%a2)+

	moveq	#0,%d0
	moveq	#0,%d1
	jsr	(%a0)
	and.b	%d5,%d0
	swap	%d0
	jsr	(%a1)
	and.b	%d5,%d0
	add.l	%d0,%d0
	move.w	(%a4,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a4,%d0.w),%d1
	lsr.l	%d2,%d1
	move.l	%d1,(%a2)+

	addq.w	#2,%a3
	addq.w	#4,%a2
	dbra	%d3,VcoRNNLoop
	rts

/* - - convert shifting right x normal, y flipped - - - - - - - - - - -	*/
VcoRNF:
	move.w	%d3,%d0
	addq.w	#1,%d0
	add.w	%d0,%d0
	add.w	%d0,%a3
VcoRNFLoop:
	move.w	%d4,(%a2)+

	move.b	-(%a3),%d5
	or.b	-(%a3),%d5
	move.w	%d5,%d1
	add.w	%d1,%d1
	move.w	(%a4,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsr.l	%d2,%d1
	not.l	%d1
	move.l	%d1,(%a2)+

	moveq	#0,%d0
	moveq	#0,%d1
	jsr	(%a0)
	and.b	%d5,%d0
	swap	%d0
	jsr	(%a1)
	and.b	%d5,%d0
	add.l	%d0,%d0
	move.w	(%a4,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a4,%d0.w),%d1
	lsr.l	%d2,%d1
	move.l	%d1,(%a2)+

	addq.w	#4,%a2
	dbra	%d3,VcoRNFLoop
	rts

/* - - convert shifting right x flipped, y normal - - - - - - - - - - -	*/
VcoRFN:
	lea	ReverseTable(%pc),%a6
VcoRFNLoop:
	move.w	%d4,(%a2)+

	move.b	(%a3),%d5
	or.b	1(%a3),%d5
	move.w	%d5,%d1
	move.b	(%a6,%d1.w),%d1
	add.w	%d1,%d1
	move.w	(%a4,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsr.l	%d2,%d1
	not.l	%d1
	move.l	%d1,(%a2)+

	moveq	#0,%d0
	moveq	#0,%d1
	jsr	(%a0)
	and.b	%d5,%d0
	move.b	(%a6,%d0.w),%d0
	swap	%d0
	jsr	(%a1)
	and.b	%d5,%d0
	move.b	(%a6,%d0.w),%d0
	add.l	%d0,%d0
	move.w	(%a4,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a4,%d0.w),%d1
	lsr.l	%d2,%d1
	move.l	%d1,(%a2)+

	addq.w	#2,%a3
	addq.w	#4,%a2
	dbra	%d3,VcoRFNLoop
	rts

/* - - convert shifting right x flipped, y flipped - - - - - - - - - -	*/
VcoRFF:
	move.w	%d3,%d0
	addq.w	#1,%d0
	add.w	%d0,%d0
	add.w	%d0,%a3
	lea	ReverseTable(%pc),%a6
VcoRFFLoop:
	move.w	%d4,(%a2)+

	move.b	-(%a3),%d5
	or.b	-(%a3),%d5
	move.w	%d5,%d1
	move.b	(%a6,%d1.w),%d1
	add.w	%d1,%d1
	move.w	(%a4,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsr.l	%d2,%d1
	not.l	%d1
	move.l	%d1,(%a2)+

	moveq	#0,%d0
	moveq	#0,%d1
	jsr	(%a0)
	and.b	%d5,%d0
	move.b	(%a6,%d0.w),%d0
	swap	%d0
	jsr	(%a1)
	and.b	%d5,%d0
	move.b	(%a6,%d0.w),%d0
	add.l	%d0,%d0
	move.w	(%a4,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a4,%d0.w),%d1
	lsr.l	%d2,%d1
	move.l	%d1,(%a2)+

	addq.w	#4,%a2
	dbra	%d3,VcoRFFLoop
	rts

/* --------------------------------------------------------------------	*/

	.globl	VideoObLink
VideoObLink:
/*	d0	offset to object in VideoObTiles
 *	d1	(scratch)
 *	d2	(scratch)
 *	d3.w	(counter 16-1 / 8-1)
 *	d4.w	(scratch)
 *	a3	pointer into oam
 *	a6	pointer VideoObFirst
 */
	move.w	%d0,%d4
	add.w	#5*2,%d4
	move.w	VideoObCount@END.w(%a5),%d3
VolOutLoop:
	move.w	(%a6,%d4.w),%d1
	move.w	2(%a6,%d4.w),%d2
	move.w	%d1,(%a6,%d2.w)
	move.w	%d2,6*2(%a6,%d1.w)
	add.w	#7*2,%d4
	dbra	%d3,VolOutLoop

	/* falls through to ObLinkIn */

/* --------------------------------------------------------------------	*/

ObLinkIn:
/*	d0	offset to object in VideoObTiles
 *	d1	(scratch)
 *	d2	(scratch)
 *	d3.w	(counter 16-1 / 8-1)
 *	d4.w	(scratch)
 *	a3	pointer into oam
 *	a6	pointer VideoObFirst
 */
	move.w	%d0,%d4
	move.w	VideoObCount@END.w(%a5),%d3

	moveq	#0,%d1
	move.b	(%a3),%d1
	jeq	OlOff
	move.b	1(%a3),%d2
	jeq	OlOff
	cmp.b	#videoScreenWidth+8,%d2
	jcc	OlOff
	sub.w	#16,%d1
	jcs	OlTopclip
	move.w	#videoScreenHeight-1,%d2
	sub.w	%d1,%d2
	jcs	OlOff
	sub.w	%d3,%d2
	jcc	OlGoon
	add.w	%d2,%d3
	not.w	%d2
	move.w	%d2,-(%sp)
	jbsr	OlGoon(%pc)
	move.w	(%sp)+,%d3

OlOff:
	add.w	#5*2,%d4
	moveq	#-7*2,%d2
OlOLoop:
	move.w	%d2,(%a6,%d4.w)
	move.w	%d4,2(%a6,%d4.w)
	sub.w	%d2,%d4
	dbra	%d3,OlOLoop
	rts

OlTopclip:
	add.w	#5*2,%d4
	add.w	%d1,%d3
	not.w	%d1
	moveq	#-7*2,%d2
OlTLoop:
	move.w	%d2,(%a6,%d4.w)
	move.w	%d4,2(%a6,%d4.w)
	sub.w	%d2,%d4
	dbra	%d1,OlTLoop

	tst.w	%d3
	jmi	OlQuit
	moveq	#0,%d1
	sub.w	#5*2,%d4

/* - - now link lines - - - - - - - - - - - - - - - - - - - - - - - - -	*/
OlGoon:
	swap	%d0

	add.w	%d1,%d1
	add.w	%d1,%d1
	moveq	#0x80-0x100,%d2
	and.b	3(%a3),%d2
	jne	OlLLoop1
	addq.w	#2,%d1			/* offset background/foreground */

OlLLoop1:
	move.w	%d1,%d0
OlLLoop2:
	move.w	(%a6,%d0.w),%d2
	cmp.w	%d2,%d4
	jge	OlLInsert
	move.w	%d2,%d0
	add.w	#5*2,%d0
	jra	OlLLoop2

OlLInsert:
	move.w	%d4,(%a6,%d0.w)
	add.w	#5*2,%d4
	move.w	%d2,(%a6,%d4.w)
	move.w	%d0,2(%a6,%d4.w)
	add.w	#6*2,%d2
	move.w	%d4,(%a6,%d2.w)
	addq.w	#2*2,%d4
	addq.w	#2*2,%d1
	dbra	%d3,OlLLoop1

	swap	%d0
OlQuit:
	rts

/* --------------------------------------------------------------------	*/

	.globl	VideoObTileCheck
VideoObTileCheck:
#define votcRegs %d1-%d3/%d5/%a0-%a4/%a6
/*	d0	(scratch)
 *	d1.w	offset into videoeo ram
 *	d2	(scratch)
 *	d3.w	(scratch)
 *	a2	base address of video ram
 *	a3	(scratch)
 */
	move.l	%d4,-(%sp)
	move.w	%d1,%d2
	swap	%d1

	lsr.w	#4,%d2
	move.w	VideoObTilemask@END.w(%a5),%d1
	move.l	MemoryBaseRamOam@END.w(%a5),%a3
	lea	0xfe00-0x10000(%a3),%a3
	move.w	#40-1,%d3
VotcLoop:
	move.w	(%a3)+,%d4
	jeq	VotcNext
	move.b	(%a3),%d0
	eor.b	%d2,%d0
	and.w	%d1,%d0
	jne	VotcNext

	movem.l	votcRegs,-(%sp)

	move.w	(%a3),%d0
	move.w	%d1,%d2
	ext.w	%d2
	lsl.w	#4,%d2
	not.w	%d2
	swap	%d1
	and.w	#0xfffe,%d1
	and.w	%d1,%d2
	lea	(%a2,%d1.w),%a3
	move.l	VideoObTiles@END.w(%a5),%a2
	moveq	#40-1,%d1
	sub.w	%d3,%d1
	lsl.w	#4+1,%d1
	add.w	%d1,%d2
	move.w	%d2,%d1
	lsl.w	#3,%d2
	sub.w	%d1,%d2
	add.w	%d2,%a2
	moveq	#0x10,%d2
	and.b	%d0,%d2
	jeq	VotcPal0
	move.l	VideoObColor1L@END.w(%a5),%a0
	move.l	VideoObColor1H@END.w(%a5),%a1
	jra	VotcConv
VotcPal0:
	move.l	VideoObColor0L@END.w(%a5),%a0
	move.l	VideoObColor0H@END.w(%a5),%a1
VotcConv:
	moveq	#1-1,%d3
	lea	VideoGrayCopyTable(%pc),%a4
	jbsr	VideoConvertOb(%pc)	/* convert one line */
	movem.l	(%sp)+,votcRegs
VotcNext:
	addq.w	#2,%a3
	dbra	%d3,VotcLoop

	swap	%d1
	move.l	(%sp)+,%d4
	rts

/* --------------------------------------------------------------------	*/

ObLinkConvertAll:
/*	d0-d5/a0-a3/%a6	(scratch, not saved)
 */
	move.l	#((-7*2)<<16)+0x10000-7*2,%d2
	move.l	VideoObFirst@END.w(%a5),%a2
	moveq	#videoScreenHeight*2/4/2-1,%d3
OlcaClearLoop:
	move.l	%d2,(%a2)+
	move.l	%d2,(%a2)+
	move.l	%d2,(%a2)+
	move.l	%d2,(%a2)+
	dbra	%d3,OlcaClearLoop	/* clear line offsets */

	move.l	MemoryBaseRamOam@END.w(%a5),%a3
	lea	0xfe00+40*4-0x10000(%a3),%a3
	move.l	VideoObFirst@END.w(%a5),%a6
	move.w	#videoScreenHeight*2*2+40*16*7*2,%d0
	moveq	#40-1,%d5
OlcaLinkLoop:
	subq.w	#4,%a3
	sub.w	#16*7*2,%d0
	jbsr	ObLinkIn(%pc)
	dbra	%d5,OlcaLinkLoop	/* link in all objects */

	move.l	VideoObTiles@END.w(%a5),%a2
	move.l	%a3,%a6
	move.l	MemoryBaseRamVid@END.w(%a5),%a3
	lea	0x8000-0x10000(%a3),%a3
	move.l	%a4,-(%sp)
	lea	VideoGrayCopyTable(%pc),%a4
	moveq	#40-1,%d3
OlcaConvLoop:
	move.w	(%a6)+,%d4
	move.w	VideoObTilemask@END.w(%a5),%d1
	and.b	(%a6)+,%d1
	move.b	(%a6)+,%d0
	moveq	#0x10,%d2
	and.b	%d0,%d2
	jeq	OlcaPal0
	move.l	VideoObColor1L@END.w(%a5),%a0
	move.l	VideoObColor1H@END.w(%a5),%a1
	jra	OlcaConv
OlcaPal0:
	move.l	VideoObColor0L@END.w(%a5),%a0
	move.l	VideoObColor0H@END.w(%a5),%a1
OlcaConv:
	swap	%d3
	movem.l	%a2/%a3/%a6,-(%sp)
	lsl.w	#3+1,%d1
	add.w	%d1,%a3
	move.w	VideoObCount@END.w(%a5),%d3
	jbsr	VideoConvertOb(%pc)
	movem.l	(%sp)+,%a2/%a3/%a6
	swap	%d3
	lea	16*7*2(%a2),%a2
	dbra	%d3,OlcaConvLoop	/* convert all objects */

	move.l	(%sp)+,%a4
	rts

/* === Functions emulating LCD controller registers ===================	*/

	.globl	VideoIoWrLcdc
VideoIoWrLcdc:
#define viwlRegs %d3/%a0-%a1
	lea	EmulationIo.lcdc@END.w(%a5),%a3
	pea	(%a6)			/* save return address */

	move.b	(%a3),%d1
	eor.b	%d0,%d1
	jeq	ViwlNop			/* no changes? */
	move.b	%d0,(%a3)

	movem.l	viwlRegs,-(%sp)

/* - - handle bit 7 - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	moveq	#7,%d2			/* LCD on: 0/1 = off/on */
	btst	%d2,%d0
	jne	Viwl7On			/* LCD on? */

	btst	%d2,%d1
	jeq	ViwlQuit		/* LCD still off? */

	lea	MemoryRamNone(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr8087)
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr888f)
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr9097)
	move.l	%a3,2+indexToAddress(gbemuJmpRamWrOam)
	lea	MemoryRamMapNone(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr989b)
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr9c9f)

	lea	LineNone(%pc),%a6
	move.l	%a6,VideoLineFunction@END.w(%a5)

	moveq	#0,%d2
	move.b	%d2,VideoIntMaskVsync@END.w(%a5)
	move.b	%d2,VideoIntMaskStat@END.w(%a5)
	jra	ViwlQuit

Viwl7On:
	btst	%d2,%d1
	jeq	Viwl7Quit		/* LCD still on? */

	or.b	#0x01,%d1
	and.b	#0xfb,%d1		/* force change bit 0 on / 2 off */

	btst	#2,%d0
	jeq	Viwl7Small
	move.w	#16-1,VideoObCount@END.w(%a5)
	move.w	#0xfe,VideoObTilemask@END.w(%a5)
	jra	Viwl7Goon
Viwl7Small:
	move.w	#8-1,VideoObCount@END.w(%a5)
	move.w	#0xff,VideoObTilemask@END.w(%a5)

Viwl7Goon:
	movem.l	%d0/%d1/%d4/%d5,-(%sp)
	jbsr	ObLinkConvertAll(%pc)
	movem.l	(%sp)+,%d0/%d1/%d4/%d5

	lea	MemoryRamOam(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRamWrOam)

	move.w	#screenYMax-153,VideoLyPrv@END.w(%a5)
	lea	LineGap(%pc),%a6
	move.l	%a6,2+indexToAddress(gbemuJmpLcdLine)

	move.b	#0x01,VideoIntMaskVsync@END.w(%a5)
	move.b	#0x02,VideoIntMaskStat@END.w(%a5)
Viwl7Quit:

/* - - handle bit 0 - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	moveq	#0,%d2			/* bg./wi. on: 0/1 = off/on */
	btst	%d2,%d0
	jne	Viwl0On			/* background/window on? */

	btst	%d2,%d1
	jeq	Viwl1BgWiOff		/* background/window still off? */

	lea	MemoryRamTileOb(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr8087)
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr888f)
	lea	MemoryRamNone(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr9097)
	lea	MemoryRamMapNone(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr989b)
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr9c9f)

	lea	LineNone(%pc),%a6
	btst	#1,%d0
	jeq	Viwl0ObOff		/* objects off? */
	move.w	#screenYMax,%d2
	sub.w	VideoLyPrv@END.w(%a5),%d2
	add.w	%d2,%d2
	add.w	%d2,%d2
	move.w	%d2,VideoObYOffset@END.w(%a5)
	lea	LineOb(%pc),%a6
Viwl0ObOff:
	move.l	%a6,VideoLineFunction@END.w(%a5)
	jra	Viwl1BgWiOff

Viwl0On:
	btst	%d2,%d1
	jeq	Viwl0Quit		/* background/window still on? */

	or.b	#0x38,%d1		/* force change bits 5..3 */

	move.w	#0x100/4-1,%d3
	move.l	VideoBgUsed@END.w(%a5),%a6
	moveq	#-1,%d2
Viwl0Loop:
	move.l	%d2,(%a6)+
	dbra	%d3,Viwl0Loop		/* no background tiles used */
Viwl0Quit:

/* - - handle bit 5 - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	moveq	#5,%d2			/* window on: 0/1 = off/on */
	btst	%d2,%d0
	jne	Viwl5On			/* window on? */

	btst	%d2,%d1
	jeq	Viwl3WiOff		/* window still off? */

	lea	MemoryRamTileOb(%pc),%a1
	lea	MemoryRamTileBgOb(%pc),%a2
	lea	MemoryRamTileBg(%pc),%a3
	btst	#4,%d0
	jeq	Viwl5Signed		/* tile type signed? */
	move.l	%a2,%a1
	lea	MemoryRamNone(%pc),%a3
Viwl5Signed:
	move.l	%a1,2+indexToAddress(gbemuJmpRamWr8087)
	move.l	%a2,2+indexToAddress(gbemuJmpRamWr888f)
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr9097)

	lea	LineBg(%pc),%a6
	btst	#1,%d0
	jeq	Viwl5ObOff		/* objects off? */
	move.w	#screenYMax,%d2
	sub.w	VideoLyPrv@END.w(%a5),%d2
	add.w	%d2,%d2
	add.w	%d2,%d2
	move.w	%d2,VideoObYOffset@END.w(%a5)
	lea	LineBgOb(%pc),%a6
Viwl5ObOff:
	move.l	%a6,VideoLineFunction@END.w(%a5)

	lea	MemoryRamMapBg(%pc),%a2
	lea	MemoryRamMapNone(%pc),%a6
	btst	#3,%d0
	jeq	Viwl5Low		/* background at lower address? */
	exg	%a2,%a6
Viwl5Low:
	move.l	%a2,2+indexToAddress(gbemuJmpRamWr989b)
	move.l	%a6,2+indexToAddress(gbemuJmpRamWr9c9f)
	jra	Viwl3WiOff

Viwl5On:
	btst	%d2,%d1
	jeq	Viwl5Quit		/* window still on? */

	or.b	#0x40,%d1		/* force change bit 6 */

	moveq	#0,%d2
	move.b	EmulationIo.wy@END.w(%a5),%d2
	add.w	VideoLyPrv@END.w(%a5),%d2
	sub.w	#screenYMax-1+1,%d2
	jpl	Viwl5WiCounter
	swap	%d2
Viwl5WiCounter:
	move.w	%d2,VideoWyPrv@END.w(%a5)	/* lines without window */

	lea	MemoryRamTileOb(%pc),%a1
	lea	MemoryRamTileBgWiOb(%pc),%a2
	lea	MemoryRamTileBgWi(%pc),%a3
	btst	#4,%d0
	jeq	Viwl5WiSigned		/* tile type signed? */
	move.l	%a2,%a1
	lea	MemoryRamNone(%pc),%a3
Viwl5WiSigned:
	move.l	%a1,2+indexToAddress(gbemuJmpRamWr8087)
	move.l	%a2,2+indexToAddress(gbemuJmpRamWr888f)
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr9097)

	lea	LineBgCh(%pc),%a6
	btst	#1,%d0
	jeq	Viwl5WiObOff		/* objects off? */
	move.w	#screenYMax,%d2
	sub.w	VideoLyPrv@END.w(%a5),%d2
	add.w	%d2,%d2
	add.w	%d2,%d2
	move.w	%d2,VideoObYOffset@END.w(%a5)
	lea	LineBgChOb(%pc),%a6
Viwl5WiObOff:
	move.l	%a6,VideoLineFunction@END.w(%a5)

	move.w	#0x100/4-1,%d3
	move.l	VideoWiUsed@END.w(%a5),%a6
	moveq	#-1,%d2
Viwl5Loop:
	move.l	%d2,(%a6)+
	dbra	%d3,Viwl5Loop		/* no window tiles used */
Viwl5Quit:

/* - - handle bit 6 - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	moveq	#6,%d2			/* wi. base: 0/1 = 0x9800/0x9c00 */
	btst	%d2,%d1
	jeq	Viwl6Quit		/* window base not changed? */

	move.l	MemoryBaseRamVid@END.w(%a5),%a3
	move.l	VideoMapIndices@END.w(%a5),%a0

	btst	%d2,%d0
	jeq	Viwl6Low		/* at lower address? */

	move.w	#videoMapWrap*videoMapWrap,%d2

	btst	#3,%d0
	jeq	Viwl6WiHBgL		/* background at lower address? */

	lea	MemoryRamMapNone(%pc),%a2
	lea	MemoryRamMapBgWi(%pc),%a6
	jra	Viwl6Goon
Viwl6WiHBgL:
	lea	MemoryRamMapBg(%pc),%a2
	lea	MemoryRamMapWi(%pc),%a6
	jra	Viwl6Goon

Viwl6Low:
	moveq	#0,%d2

	btst	#3,%d0
	jeq	Viwl6WiLBgL		/* background at lower address? */

	lea	MemoryRamMapWi(%pc),%a2
	lea	MemoryRamMapBg(%pc),%a6
	jra	Viwl6Goon
Viwl6WiLBgL:
	lea	MemoryRamMapBgWi(%pc),%a2
	lea	MemoryRamMapNone(%pc),%a6

Viwl6Goon:
	move.l	%a2,2+indexToAddress(gbemuJmpRamWr989b)
	move.l	%a6,2+indexToAddress(gbemuJmpRamWr9c9f)

	lea	0x9800-0x10000(%a3),%a3
	add.w	%d2,%a3

	add.w	%d2,%d2
	add.w	%d2,%a0
	move.l	%a0,VideoWiMapStart@END.w(%a5)

	move.w	%d0,-(%sp)
	move.w	%d1,-(%sp)

	moveq	#0,%d0
	moveq	#0x7f,%d1
	move.w	VideoWiWxPixel@END.w(%a5),%d2
	move.w	#videoScreenHeight/8*videoMapWrap-1,%d3
	move.l	VideoWiUsed@END.w(%a5),%a6
Viwl6Loop:
	move.b	(%a3)+,%d0
	cmp.b	(%a6,%d0.w),%d2
	jeq	Viwl6Next
	move.b	%d1,(%a6,%d0.w)
Viwl6Next:
	dbra	%d3,Viwl6Loop		/* which tiles need conversion */

	move.w	(%sp)+,%d1
	move.w	(%sp)+,%d0
Viwl6Quit:

/* - - handle bit 3, two entries: window on or off - - - - - - - - - -	*/
Viwl3WiOn:
	moveq	#3,%d2			/* bg. base: 0/1 = 0x9800/0x9c00 */
	btst	%d2,%d1
	jeq	Viwl3Quit		/* background base not changed? */

	move.l	MemoryBaseRamVid@END.w(%a5),%a3
	move.l	VideoMapIndices@END.w(%a5),%a0

	btst	%d2,%d0
	jeq	Viwl3WiLow		/* at lower address? */

	move.w	#videoMapWrap*videoMapWrap,%d2

	btst	#6,%d0
	jeq	Viwl3WiLBgH		/* window at lower address? */

	lea	MemoryRamMapNone(%pc),%a2
	lea	MemoryRamMapBgWi(%pc),%a6
	jra	Viwl3Scan
Viwl3WiLBgH:
	lea	MemoryRamMapWi(%pc),%a2
	lea	MemoryRamMapBg(%pc),%a6
	jra	Viwl3Scan

Viwl3WiLow:
	moveq	#0,%d2

	btst	#6,%d0
	jeq	Viwl3WiLBgL		/* window at lower address? */

	lea	MemoryRamMapBg(%pc),%a2
	lea	MemoryRamMapWi(%pc),%a6
	jra	Viwl3Scan
Viwl3WiLBgL:
	lea	MemoryRamMapBgWi(%pc),%a2
	lea	MemoryRamMapNone(%pc),%a6
	jra	Viwl3Scan

Viwl3WiOff:
	moveq	#3,%d2			/* bg. base: 0/1 = 0x9800/0x9c00 */
	btst	%d2,%d1
	jeq	Viwl3Quit		/* background base not changed? */

	move.l	MemoryBaseRamVid@END.w(%a5),%a3
	move.l	VideoMapIndices@END.w(%a5),%a0

	btst	%d2,%d0
	jeq	Viwl3BgLow		/* at lower address? */

	move.w	#videoMapWrap*videoMapWrap,%d2

	lea	MemoryRamMapNone(%pc),%a2
	lea	MemoryRamMapBg(%pc),%a6
	jra	Viwl3Scan
Viwl3BgLow:
	moveq	#0,%d2

	lea	MemoryRamMapBg(%pc),%a2
	lea	MemoryRamMapNone(%pc),%a6

Viwl3Scan:
	move.l	%a2,2+indexToAddress(gbemuJmpRamWr989b)
	move.l	%a6,2+indexToAddress(gbemuJmpRamWr9c9f)

	lea	0x9800-0x10000(%a3),%a3
	add.w	%d2,%a3

	add.w	%d2,%d2
	add.w	%d2,%a0
	move.l	%a0,VideoBgMapStart@END.w(%a5)

	moveq	#screenYMax-0x100,%d2
	sub.w	VideoLyPrv@END.w(%a5),%d2
	add.b	EmulationIo.scy@END.w(%a5),%d2
	add.w	%d2,%d2
	add.w	%d2,%d2
	move.b	%d2,-(%sp)
	and.w	#(videoMapWrap-1)*videoMapWrap,%d2
	add.w	VideoBgScxTile@END.w(%a5),%d2
	add.w	%d2,%d2
	move.w	%d2,VideoBgMapOffset@END.w(%a5)
	move.w	(%sp)+,%d2
	and.w	#7*0x100*4,%d2
	move.l	VideoBgTile@END.w(%a5),%a2
	add.w	%d2,%a2
	move.l	%a2,VideoBgTilePtr@END.w(%a5)	/* correct curr. pointer */

	move.w	%d0,-(%sp)
	move.w	%d1,-(%sp)

	moveq	#0,%d0
	moveq	#0x7f,%d1
	move.w	VideoBgScxPixel@END.w(%a5),%d2
	move.w	#videoMapWrap*videoMapWrap-1,%d3
	move.l	VideoBgUsed@END.w(%a5),%a6
Viwl3Loop:
	move.b	(%a3)+,%d0
	cmp.b	(%a6,%d0.w),%d2
	jeq	Viwl3Next
	move.b	%d1,(%a6,%d0.w)
Viwl3Next:
	dbra	%d3,Viwl3Loop		/* which tiles need conversion */

	move.w	(%sp)+,%d1
	move.w	(%sp)+,%d0
Viwl3Quit:

/* - - handle bit 4 - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	moveq	#4,%d2			/* tile: 0/1 = signed/unsigned */
	btst	%d2,%d1
	jeq	Viwl4Quit		/* tile type not changed? */

	btst	#5,%d0
	jne	Viwl4WiOn		/* window on? */

	lea	MemoryRamTileOb(%pc),%a1
	lea	MemoryRamTileBgOb(%pc),%a2
	lea	MemoryRamTileBg(%pc),%a3
	jra	Viwl4Goon

Viwl4WiOn:
	lea	MemoryRamTileOb(%pc),%a1
	lea	MemoryRamTileBgWiOb(%pc),%a2
	lea	MemoryRamTileBgWi(%pc),%a3

	or.b	#0x40,%d1		/* window tiles need conversion */

	moveq	#0x7f,%d2
	moveq	#0x80-1,%d3
	move.l	VideoWiUsed@END.w(%a5),%a6
Viwl4WiLoop:
	tst.b	(%a6)+
	jmi	Viwl4WiNext
	move.b	%d2,-1(%a6)
Viwl4WiNext:
	dbra	%d3,Viwl4WiLoop		/* which tiles need conversion */

Viwl4Goon:
	move.l	MemoryBaseRamVid@END.w(%a5),%a6
	lea	0x8000-0x10000(%a6),%a6
	move.l	%a6,VideoBgWiTileHigh@END.w(%a5)

	lea	0x1000(%a6),%a6
	btst	#4,%d0
	jeq	Viwl4Signed		/* tile type signed? */
	move.l	%a2,%a1
	lea	MemoryRamNone(%pc),%a3
	lea	-0x1000(%a6),%a6
Viwl4Signed:
	move.l	%a1,2+indexToAddress(gbemuJmpRamWr8087)
	move.l	%a2,2+indexToAddress(gbemuJmpRamWr888f)
	move.l	%a3,2+indexToAddress(gbemuJmpRamWr9097)
	move.l	%a6,VideoBgWiTileLow@END.w(%a5)

	moveq	#0x7f,%d2
	moveq	#0x80-1,%d3
	move.l	VideoBgUsed@END.w(%a5),%a6
Viwl4BgLoop:
	tst.b	(%a6)+
	jmi	Viwl4BgNext
	move.b	%d2,-1(%a6)
Viwl4BgNext:
	dbra	%d3,Viwl4BgLoop		/* which tiles need conversion */

	or.b	#0x08,%d1		/* bg. tiles need conversion */
Viwl4Quit:

/* - - convert background tiles if necessary - - - - - - - - - - - - -	*/
	moveq	#3,%d2			/* bg. tiles need conversion */
	btst	%d2,%d1
	jeq	ViwlBgQuit		/* no conversions? */

	move.l	VideoBgWiColorL@END.w(%a5),%a0
	move.l	VideoBgWiColorH@END.w(%a5),%a1
	move.w	VideoBgScxPixel@END.w(%a5),%d2
	move.l	VideoBgUsed@END.w(%a5),%a6
	move.l	VideoBgTile@END.w(%a5),%a2
	jbsr	ConvertBgWiAllMarked(%pc)
ViwlBgQuit:

/* - - convert window tiles if necessary - - - - - - - - - - - - - - -	*/
	moveq	#6,%d2			/* window tiles need conversion */
	btst	%d2,%d1
	jeq	ViwlWiQuit		/* no conversions? */

	move.l	VideoBgWiColorL@END.w(%a5),%a0
	move.l	VideoBgWiColorH@END.w(%a5),%a1
	move.w	VideoWiWxPixel@END.w(%a5),%d2
	move.l	VideoWiUsed@END.w(%a5),%a6
	move.l	VideoWiTile@END.w(%a5),%a2
	jbsr	ConvertBgWiAllMarked(%pc)
ViwlWiQuit:

/* - - handle bit 1, two entries: background/window on or off - - - - -	*/
Viwl1BgWiOn:
	moveq	#1,%d2			/* objects on: 0/1 = off/on */
	btst	%d2,%d1
	jeq	Viwl1Quit		/* state not changed? */

	btst	#5,%d0
	jeq	Viwl1WiOff		/* window off? */
	lea	LineBgCh(%pc),%a2
	lea	LineBgChOb(%pc),%a3
	jra	Viwl1Goon
Viwl1WiOff:
	lea	LineBg(%pc),%a2
	lea	LineBgOb(%pc),%a3
	jra	Viwl1Goon

Viwl1BgWiOff:
	moveq	#1,%d2			/* objects on: 0/1 = off/on */
	btst	%d2,%d1
	jeq	Viwl1Quit		/* state not changed? */

	lea	LineNone(%pc),%a2
	lea	LineOb(%pc),%a3
Viwl1Goon:
	btst	#1,%d0
	jeq	Viwl1ObOff		/* objects off? */
	move.w	#screenYMax,%d2
	sub.w	VideoLyPrv@END.w(%a5),%d2
	add.w	%d2,%d2
	add.w	%d2,%d2
	move.w	%d2,VideoObYOffset@END.w(%a5)
	move.l	%a3,%a2
Viwl1ObOff:
	move.l	%a2,VideoLineFunction@END.w(%a5)
Viwl1Quit:

/* - - handle bit 2 - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	moveq	#2,%d2			/* object height: 0/1 = 8/16 */
	btst	%d2,%d1
	jeq	Viwl2Quit		/* height not changed? */

	btst	%d2,%d0
	jeq	Viwl2Small
	move.w	#16-1,VideoObCount@END.w(%a5)
	move.w	#0xfe,VideoObTilemask@END.w(%a5)
	jra	Viwl2Goon
Viwl2Small:
	move.w	#8-1,VideoObCount@END.w(%a5)
	move.w	#0xff,VideoObTilemask@END.w(%a5)

Viwl2Goon:
	movem.l	%d0/%d1/%d4/%d5,-(%sp)
	jbsr	ObLinkConvertAll(%pc)
	movem.l	(%sp)+,%d0/%d1/%d4/%d5
Viwl2Quit:

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
ViwlQuit:
	movem.l	(%sp)+,viwlRegs

	move.l	VideoLineFunction@END.w(%a5),%a6
	move.w	VideoLyPrv@END.w(%a5),%d2
	jmi	ViwlNop			/* not in visible part? */
	move.l	%a6,2+indexToAddress(gbemuJmpLcdLine)

ViwlNop:
	rts

/* --------------------------------------------------------------------	*/

	.globl	VideoIoRdStat
VideoIoRdStat:
/* NOTE:
 * These were correct timings, but don't work really and slow down the
 * emulation:
 *	cmp.w	#(opsPerLine*(108-19-41))/108,%d6
 *	cmp.w	#(opsPerLine*(108-19))/108,%d6
 */
	lea	EmulationIo.stat@END.w(%a5),%a3
	and.b	#0xf8,(%a3)

	moveq	#screenYMax-1-0x100,%d0
	sub.w	VideoLyPrv@END.w(%a5),%d0
	cmp.b	#screenYMax+1,%d0
	jcs	VirsVisible
	or.b	#0x01,(%a3)
	jra	VirsLyc

VirsVisible:
	cmp.w	#opsPerLine-10,%d6
	jcs	VirsLyc
	or.b	#0x02,(%a3)
	cmp.w	#opsPerLine-5,%d6
	jcc	VirsLyc
	or.b	#0x01,(%a3)

VirsLyc:
	cmp.b	EmulationIo.lyc@END.w(%a5),%d0
	jne	VirsQuit
	or.b	#0x04,(%a3)

VirsQuit:
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	VideoIoWrStat
VideoIoWrStat:
	move.b	%d0,EmulationIo.stat@END.w(%a5)	/* store value */

	move.b	#0x00,VideoIntMaskSV@END.w(%a5)
	btst	#4,%d0
	jeq	ViwsVsyncOk
	move.b	#0x02,VideoIntMaskSV@END.w(%a5)
ViwsVsyncOk:				/* set up for "vsync" */

	move.b	#0x00,VideoIntMaskSH@END.w(%a5)
	moveq	#0x28,%d1
	and.b	%d0,%d1
	jeq	ViwsHsyncOk
	move.b	#0x02,VideoIntMaskSH@END.w(%a5)
ViwsHsyncOk:				/* set up for "hsync" */

	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	VideoIoWrScy
VideoIoWrScy:
	move.b	%d0,EmulationIo.scy@END.w(%a5)	/* store value */

	sub.w	VideoLyPrv@END.w(%a5),%d0
	add.b	#screenYMax,%d0
	add.w	%d0,%d0
	add.w	%d0,%d0
	move.b	%d0,-(%sp)
	and.w	#(videoMapWrap-1)*videoMapWrap,%d0
	add.w	VideoBgScxTile@END.w(%a5),%d0
	add.w	%d0,%d0
	move.w	%d0,VideoBgMapOffset@END.w(%a5)
	move.w	(%sp)+,%d0
	and.w	#7*0x100*4,%d0
	move.l	VideoBgTile@END.w(%a5),%a3
	add.w	%d0,%a3
	move.l	%a3,VideoBgTilePtr@END.w(%a5)	/* correct curr. pointer */
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	VideoIoWrScx
VideoIoWrScx:
	move.l	%d3,-(%sp)

	move.b	%d0,EmulationIo.scx@END.w(%a5)	/* store value */

	move.w	VideoBgScxPixel@END.w(%a5),%d1
	moveq	#7,%d2
	and.b	%d0,%d2
	add.w	%d2,%d2
	move.w	%d2,VideoBgScxPixel@END.w(%a5)
	lsr.b	#3,%d0
	ext.w	%d0
	move.w	%d0,%d3
	sub.w	VideoBgScxTile@END.w(%a5),%d3
	move.w	%d0,VideoBgScxTile@END.w(%a5)
	add.w	%d3,%d3
	add.w	%d3,VideoBgMapOffset@END.w(%a5)	/* update variables */

	moveq	#videoScreenWidth/8-1,%d3
	sub.w	#videoMapWrap-videoScreenWidth/8,%d0
	jmi	ViwsxCounterBg
	sub.w	%d0,%d3
	exg	%d0,%d3
ViwsxCounterBg:
	move.w	%d0,VideoBgCount1@END.w(%a5)
	move.w	%d3,VideoBgCount2@END.w(%a5)	/* set up counters (bg.) */

	move.w	%d0,VideoBgWiCount1@END.w(%a5)
	move.w	VideoBgWiCount2@END.w(%a5),%d0
	jmi	ViwsxCounterExit
	move.w	VideoBgWiCount3@END.w(%a5),%d0
	jmi	ViwsxCounterBgWi
	sub.w	%d0,%d3
	jpl	ViwsxCounterBgWi
	move.w	%d3,VideoBgWiCount1@END.w(%a5)
	moveq	#videoScreenWidth/8-1,%d3
	sub.w	%d0,%d3
ViwsxCounterBgWi:
	move.w	%d3,VideoBgWiCount2@END.w(%a5)	/* set up cnts (bg./wi.) */
ViwsxCounterExit:

	moveq	#0x81-0x100,%d0
	move.b	EmulationIo.lcdc@END.w(%a5),%d3
	and.b	%d0,%d3
	cmp.b	%d0,%d3
	jne	ViwswxQuit		/* background off? */

	move.l	VideoBgUsed@END.w(%a5),%a3
	move.l	VideoBgTile@END.w(%a5),%a2

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
ViwswxShift:
	sub.w	%d2,%d1
	jeq	ViwswxQuit		/* no new pixel shift? */
	jcc	ViwswxRight		/* shift right? */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	neg.w	%d1
	move.w	#0x100-1,%d3
ViwswxLLoop:
	move.b	(%a3)+,%d0
	jmi	ViwswxLNext
	cmp.b	%d0,%d2
	jne	ViwswxLCorr		/* correction necessary? */
ViwswxLNext:
	addq.w	#4,%a2
	dbra	%d3,ViwswxLLoop
	jra	ViwswxQuit
ViwswxLCorr:
	move.b	%d2,-1(%a3)

	swap	%d3
	move.w	#8-1,%d3
ViwswxLIloop:
	move.l	(%a2),%d0
	lsl.l	%d1,%d0
	move.l	%d0,(%a2)
	move.l	8*0x100*4(%a2),%d0
	lsl.l	%d1,%d0
	move.l	%d0,8*0x100*4(%a2)

	lea	0x100*4(%a2),%a2

	dbra	%d3,ViwswxLIloop
	swap	%d3
	lea	4-8*0x100*4(%a2),%a2

	dbra	%d3,ViwswxLLoop
	jra	ViwswxQuit

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
ViwswxRight:
	move.w	#0x100-1,%d3
ViwswxRLoop:
	move.b	(%a3)+,%d0
	jmi	ViwswxRNext
	cmp.b	%d0,%d2
	jne	ViwswxRCorr		/* correction necessary? */
ViwswxRNext:
	addq.w	#4,%a2
	dbra	%d3,ViwswxRLoop
	jra	ViwswxQuit
ViwswxRCorr:
	move.b	%d2,-1(%a3)

	swap	%d3
	move.w	#8-1,%d3
ViwswxRIloop:
	move.l	(%a2),%d0
	lsr.l	%d1,%d0
	move.l	%d0,(%a2)
	move.l	8*0x100*4(%a2),%d0
	lsr.l	%d1,%d0
	move.l	%d0,8*0x100*4(%a2)

	lea	0x100*4(%a2),%a2

	dbra	%d3,ViwswxRIloop
	swap	%d3
	lea	4-8*0x100*4(%a2),%a2

	dbra	%d3,ViwswxRLoop

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
ViwswxQuit:
	move.l	(%sp)+,%d3
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	VideoIoWrLyc
VideoIoWrLyc:
	move.b	%d0,EmulationIo.lyc@END.w(%a5)	/* store value */

	move.b	%d0,VideoIntLycReload@END.w(%a5)
	moveq	#screenYMax-0x100,%d1
	sub.w	VideoLyPrv@END.w(%a5),%d1
	sub.b	%d1,%d0
	move.b	%d0,VideoIntLycCount@END.w(%a5)	/* set up counter */
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	VideoIoWrDma
VideoIoWrDma:
#define viwdRegs %d3/%a0-%a1
	movem.l	viwdRegs,-(%sp)

	move.l	MemoryBaseRamOam@END.w(%a5),%a1
	lea	0xfe00+40*4-0x10000(%a1),%a1
	lea	40*4(%a3),%a0
	moveq	#40*4-4-0x100,%d3
ViwdLoop:
	move.l	-(%a1),%d2
	move.l	-(%a0),%d0
	move.l	%d0,(%a1)
	move.b	%d3,%d1
	move.l	%a1,%a3
	jbsr	MemoryRamOamEntry(%pc)
	subq.b	#4,%d3
	jcc	ViwdLoop

	movem.l	(%sp)+,viwdRegs
	rts

/* --------------------------------------------------------------------	*/

	.globl	VideoIoWrBgp
VideoIoWrBgp:
#define viwbRegs %d3/%a0-%a1
	lea	EmulationIo.bgp@END.w(%a5),%a3
	pea	(%a6)			/* save return address */

	cmp.b	(%a3),%d0
	jeq	ViwbQuit		/* no changes? */
	move.b	%d0,(%a3)

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	moveq	#0,%d1
	moveq	#0,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2			/* calculate color indices */

	lea	ColorTab(%pc),%a3
	add.w	%d1,%d1
	add.w	(%a3,%d1.w),%d1
	lea	(%a3,%d1.w),%a2
	move.l	%a2,VideoBgWiColorH@END.w(%a5)
	add.w	%d2,%d2
	add.w	(%a3,%d2.w),%d2
	add.w	%d2,%a3
	move.l	%a3,VideoBgWiColorL@END.w(%a5)	/* set up color func's */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	moveq	#0x81-0x100,%d0
	move.b	EmulationIo.lcdc@END.w(%a5),%d1
	and.b	%d0,%d1
	cmp.b	%d0,%d1
	jne	ViwbQuit		/* background/window off? */

	movem.l	viwbRegs,-(%sp)

	move.l	%a3,%a0
	move.l	%a2,%a1

	move.l	VideoBgUsed@END.w(%a5),%a6
	move.l	VideoBgTile@END.w(%a5),%a2
	move.w	VideoBgScxPixel@END.w(%a5),%d2
	jbsr	ConvertBgWiAllUsed(%pc)

	btst	#5,EmulationIo.lcdc@END.w(%a5)
	jeq	ViwbExit		/* window off? */

	move.l	VideoWiUsed@END.w(%a5),%a6
	move.l	VideoWiTile@END.w(%a5),%a2
	move.w	VideoWiWxPixel@END.w(%a5),%d2
	jbsr	ConvertBgWiAllUsed(%pc)

ViwbExit:
	movem.l	(%sp)+,viwbRegs

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
ViwbQuit:
	rts

/* --------------------------------------------------------------------	*/

	.globl	VideoIoWrObp0
VideoIoWrObp0:
#define viwoRegs %d3-%d5/%a0-%a1/%a4
	lea	EmulationIo.obp0@END.w(%a5),%a3
	pea	(%a6)			/* save return address */

	cmp.b	(%a3),%d0
	jeq	Viwo0Quit		/* no changes? */
	move.b	%d0,(%a3)

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	moveq	#0,%d1
	moveq	#0,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2			/* calculate color indices */

	lea	ColorTab(%pc),%a3
	add.w	%d1,%d1
	add.w	(%a3,%d1.w),%d1
	lea	(%a3,%d1.w),%a2
	move.l	%a2,VideoObColor0H@END.w(%a5)
	add.w	%d2,%d2
	add.w	(%a3,%d2.w),%d2
	add.w	%d2,%a3
	move.l	%a3,VideoObColor0L@END.w(%a5)	/* set up color func's */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	btst	#7,EmulationIo.lcdc@END.w(%a5)
	jeq	Viwo0Quit		/* LCD off? */

	movem.l	viwoRegs,-(%sp)

	move.l	%a3,%a0
	move.l	%a2,%a1
	move.l	VideoObTiles@END.w(%a5),%a2
	move.l	MemoryBaseRamVid@END.w(%a5),%a3
	lea	0x8000-0x10000(%a3),%a3
	move.l	MemoryBaseRamOam@END.w(%a5),%a6
	lea	0xfe00-0x10000(%a6),%a6
	lea	VideoGrayCopyTable(%pc),%a4

	moveq	#40-1,%d3
Viwo0Loop:
	move.w	(%a6)+,%d4
	move.w	VideoObTilemask@END.w(%a5),%d1
	and.b	(%a6)+,%d1
	move.b	(%a6)+,%d0
	moveq	#0x10,%d2
	and.b	%d0,%d2
	jne	Viwo0Next		/* not this palette? */

	swap	%d3
	movem.l	%a2/%a3/%a6,-(%sp)
	lsl.w	#3+1,%d1
	add.w	%d1,%a3
	move.w	VideoObCount@END.w(%a5),%d3
	jbsr	VideoConvertOb(%pc)
	movem.l	(%sp)+,%a2/%a3/%a6
	swap	%d3
Viwo0Next:
	lea	16*7*2(%a2),%a2
	dbra	%d3,Viwo0Loop		/* convert object with new pal. */

Viwo0Exit:
	movem.l	(%sp)+,viwoRegs

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Viwo0Quit:
	rts

/* --------------------------------------------------------------------	*/

	.globl	VideoIoWrObp1
VideoIoWrObp1:
	lea	EmulationIo.obp1@END.w(%a5),%a3
	pea	(%a6)			/* save return address */

	cmp.b	(%a3),%d0
	jeq	Viwo1Quit		/* no changes? */
	move.b	%d0,(%a3)

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	moveq	#0,%d1
	moveq	#0,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2
	add.b	%d0,%d0
	addx.b	%d1,%d1
	add.b	%d0,%d0
	addx.b	%d2,%d2			/* calculate color indices */

	lea	ColorTab(%pc),%a3
	add.w	%d1,%d1
	add.w	(%a3,%d1.w),%d1
	lea	(%a3,%d1.w),%a2
	move.l	%a2,VideoObColor1H@END.w(%a5)
	add.w	%d2,%d2
	add.w	(%a3,%d2.w),%d2
	add.w	%d2,%a3
	move.l	%a3,VideoObColor1L@END.w(%a5)	/* set up color func's */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	btst	#7,EmulationIo.lcdc@END.w(%a5)
	jeq	Viwo1Quit		/* LCD off? */

	movem.l	viwoRegs,-(%sp)

	move.l	%a3,%a0
	move.l	%a2,%a1
	move.l	VideoObTiles@END.w(%a5),%a2
	move.l	MemoryBaseRamVid@END.w(%a5),%a3
	lea	0x8000-0x10000(%a3),%a3
	move.l	MemoryBaseRamOam@END.w(%a5),%a6
	lea	0xfe00-0x10000(%a6),%a6
	lea	VideoGrayCopyTable(%pc),%a4

	moveq	#40-1,%d3
Viwo1Loop:
	move.w	(%a6)+,%d4
	move.w	VideoObTilemask@END.w(%a5),%d1
	and.b	(%a6)+,%d1
	move.b	(%a6)+,%d0
	moveq	#0x10,%d2
	and.b	%d0,%d2
	jeq	Viwo1Next		/* not this palette? */

	swap	%d3
	movem.l	%a2/%a3/%a6,-(%sp)
	lsl.w	#3+1,%d1
	add.w	%d1,%a3
	move.w	VideoObCount@END.w(%a5),%d3
	jbsr	VideoConvertOb(%pc)
	movem.l	(%sp)+,%a2/%a3/%a6
	swap	%d3
Viwo1Next:
	lea	16*7*2(%a2),%a2
	dbra	%d3,Viwo1Loop		/* convert object with new pal. */

Viwo1Exit:
	movem.l	(%sp)+,viwoRegs

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
Viwo1Quit:
	rts

/* --------------------------------------------------------------------	*/

	.globl	VideoIoWrWy
VideoIoWrWy:
	move.b	%d0,EmulationIo.wy@END.w(%a5)	/* store value */

	moveq	#0xa1-0x100,%d1
	move.b	EmulationIo.lcdc@END.w(%a5),%d2
	and.b	%d1,%d2
	cmp.b	%d1,%d2
	jne	ViwwyQuit		/* window off? */

	move.w	VideoLyPrv@END.w(%a5),%d2
	jmi	ViwwyQuit		/* not in visible part? */

	lea	LineBgCh(%pc),%a3
	btst	#1,EmulationIo.lcdc@END.w(%a5)
	jeq	ViwwyObOff		/* objects off? */
	lea	LineBgChOb(%pc),%a3
ViwwyObOff:
	move.l	%a3,2+indexToAddress(gbemuJmpLcdLine)

	and.w	#0xff,%d0
	add.w	%d2,%d0
	sub.w	#screenYMax-1+1,%d0
	move.w	%d0,VideoWyPrv@END.w(%a5)
	jmi	ViwwyQuit		/* window shown yet? */

	move.w	#0,VideoWiMapOffset@END.w(%a5)
	move.l	VideoWiTile@END.w(%a5),%a3
	move.l	%a3,VideoWiTilePtr@END.w(%a5)	/* correct curr. pointer */

ViwwyQuit:
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	VideoIoWrWx
VideoIoWrWx:
	move.l	%d3,-(%sp)

	move.b	%d0,EmulationIo.wx@END.w(%a5)	/* store value */

	move.w	VideoWiWxPixel@END.w(%a5),%d1
	move.b	%d0,%d2
	not.b	%d2
	and.w	#7,%d2
	add.w	%d2,%d2
	move.w	%d2,VideoWiWxPixel@END.w(%a5)
	moveq	#-1,%d3
	lsl.w	%d2,%d3
	move.w	%d3,VideoWiWxMask@END.w(%a5)	/* update variables */

	moveq	#videoScreenWidth/8-1,%d3
	subq.b	#8,%d0
	jcs	ViwwxFull
	move.w	VideoBgCount2@END.w(%a5),%d2
	cmp.b	#videoScreenWidth-1,%d0
	jcc	ViwwxEmpty
	lsr.b	#3,%d0
	ext.w	%d0
	sub.w	%d0,%d3
	move.w	VideoBgCount1@END.w(%a5),%d0
	sub.w	%d3,%d2
	jpl	ViwwxCounters
	moveq	#-1,%d0
	moveq	#videoScreenWidth/8-1,%d2
	sub.w	%d3,%d2
	jra	ViwwxCounters
ViwwxFull:
	moveq	#-1,%d2
	jra	ViwwxCounters
ViwwxEmpty:
	move.w	VideoBgCount1@END.w(%a5),%d0
	moveq	#-1,%d3
ViwwxCounters:
	move.w	%d0,VideoBgWiCount1@END.w(%a5)
	move.w	%d2,VideoBgWiCount2@END.w(%a5)
	move.w	%d3,VideoBgWiCount3@END.w(%a5)	/* set up counters */

	moveq	#0xa1-0x100,%d0
	move.b	EmulationIo.lcdc@END.w(%a5),%d3
	and.b	%d0,%d3
	cmp.b	%d0,%d3
	jne	ViwswxQuit		/* window off? */

	move.w	VideoWiWxPixel@END.w(%a5),%d2
	move.l	VideoWiUsed@END.w(%a5),%a3
	move.l	VideoWiTile@END.w(%a5),%a2
	jra	ViwswxShift		/* check for tile shifting */

/* === The end ========================================================	*/
