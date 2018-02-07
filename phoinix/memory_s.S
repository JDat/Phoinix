/************************************************************************
 *  memory_s.S
 *                      Memory emulation routines (assembler part)
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 ************************************************************************
 *  History:
 *
 *  $Log: memory_s.S,v $
 *  Revision 1.4  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.3  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:21  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-16  Bodo Wenzel   Saving states now possible
 *  2001-07-25  Bodo Wenzel   Changed syntax to CPP + GAS
 *  2001-02-24  Bodo Wenzel   Support of more cartridges
 *  2000-11-15  Bodo Wenzel   Support of color devices
 *  2000-10-19  Bodo Wenzel   Interrupts and timer
 *  2000-09-17  Bodo Wenzel   Now nearly full screen emulation
 *  2000-09-10  Bodo Wenzel   Got from runmode.asm
 ************************************************************************
 */

/* === Includes =======================================================	*/

#include "memory.h"

#include "video.h"
#include "gbemu.h"

/* === Placing the code ===============================================	*/

	.section	emulator,"x"

/* === Functions for memory bank control ==============================	*/

	.globl	MemoryMbcDummy
MemoryMbcDummy:				/* Dummy ROM write */
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc1RomSelect
MemoryMbc1RomSelect:			/* 4/32 mode */
	and.w	#0x1f,%d0		/* 32 banks (512 KB) */
	add.w	%d0,%d0
	add.w	%d0,%d0
	move.w	%d0,MemoryMbc1RomLsb@END.w(%a5)
	lea	MemoryRomPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRom1@END.w(%a5)
	jmp	indexToAddress(gbemuCodeSpRomCorrect)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc1RomSelectL
MemoryMbc1RomSelectL:			/* 16/8 mode */
	and.w	#0x1f,%d0		/* 128 banks (2 MB) */
	add.w	%d0,%d0
	add.w	%d0,%d0
	move.w	%d0,MemoryMbc1RomLsb@END.w(%a5)
	move.w	MemoryMbc1RomMsb@END.w(%a5),%d1
	lsl.w	#5,%d1
	add.w	%d1,%d0
	lea	MemoryRomPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRom1@END.w(%a5)
	jmp	indexToAddress(gbemuCodeSpRomCorrect)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc1RomSelectH
MemoryMbc1RomSelectH:			/* 16/8 mode */
	and.w	#0x03,%d0		/* 128 banks (2 MB) */
	add.w	%d0,%d0
	add.w	%d0,%d0
	move.w	%d0,MemoryMbc1RomMsb@END.w(%a5)
	lsl.w	#5,%d0
	add.w	MemoryMbc1RomLsb@END.w(%a5),%d0
	lea	MemoryRomPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRom1@END.w(%a5)
	jmp	indexToAddress(gbemuCodeSpRomCorrect)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc1RamSelect
MemoryMbc1RamSelect:			/* 4/32 mode */
	and.w	#0x03,%d0		/* 4 banks (32 KB) */
	add.w	%d0,%d0
	add.w	%d0,%d0
	move.w	%d0,MemoryMbc1RomMsb@END.w(%a5)
	lea	MemoryRamPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRamExt@END.w(%a5)
	jmp	indexToAddress(gbemuCodeSpRamCorrect)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc1ModeSelect
MemoryMbc1ModeSelect:
	move.b	%d0,MemoryMbc1Mode@END.w(%a5)
	btst	#0,%d0
	jeq	MM1MS168

	move.w	MemoryMbc1RomLsb@END.w(%a5),%d0	/* 32 banks (512 KB) */
	lea	MemoryRomPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRom1@END.w(%a5)

	move.w	MemoryMbc1RomMsb@END.w(%a5),%d0	/* 4 banks (32 KB) */
	lea	MemoryRamPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRamExt@END.w(%a5)

	lea	MemoryMbc1RomSelect(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRomSelectL)
	move.l	%a3,2+indexToAddress(gbemuJmpRomSelectH)
	lea	MemoryMbc1RamSelect(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRamSelect)
	jmp	indexToAddress(gbemuCodeSpRomCorrect)

MM1MS168:
	move.w	MemoryMbc1RomMsb@END.w(%a5),%d0
	lsl.w	#5,%d0			/* 128 banks (2 MB) */
	add.w	MemoryMbc1RomLsb@END.w(%a5),%d0
	lea	MemoryRomPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRom1@END.w(%a5)

	move.l	MemoryRamPages@END.w(%a5),MemoryBaseRamExt@END.w(%a5)

	lea	MemoryMbc1RomSelectL(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRomSelectL)
	move.l	%a3,2+indexToAddress(gbemuJmpRomSelectH)
	lea	MemoryMbc1RomSelectH(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRamSelect)
	jmp	indexToAddress(gbemuCodeSpRomCorrect)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc2RomSelect
MemoryMbc2RomSelect:
	and.w	#0x0f,%d0		/* 16 banks (256 KB) */
	add.w	%d0,%d0
	add.w	%d0,%d0
	lea	MemoryRomPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRom1@END.w(%a5)
	jmp	indexToAddress(gbemuCodeSpRomCorrect)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc3RomSelect
MemoryMbc3RomSelect:
	and.w	#0x7f,%d0		/* 128 banks (2 MB) */
	add.w	%d0,%d0
	add.w	%d0,%d0
	lea	MemoryRomPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRom1@END.w(%a5)
	jmp	indexToAddress(gbemuCodeSpRomCorrect)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc3RamSelect
MemoryMbc3RamSelect:
	move.b	%d0,MemoryMbc3Mode@END.w(%a5)
	btst	#3,%d0
	jne	MM3RtcSelect

	and.w	#0x03,%d0		/* 4 banks (32 KB) */
	add.w	%d0,%d0
	add.w	%d0,%d0
	lea	MemoryRamPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRamExt@END.w(%a5)
	move.l	6+indexToAddress(gbemuJmpRamRdExt),2+indexToAddress(gbemuJmpRamRdExt)
	move.l	6+indexToAddress(gbemuJmpRamWrExt),2+indexToAddress(gbemuJmpRamWrExt)
	jmp	indexToAddress(gbemuCodeSpRamCorrect)

MM3RtcSelect:
	lea	MemoryMbc3RtcLatched@END.w(%a5),%a3
	and.w	#0x07,%d0
	add.w	%d0,%a3
	move.l	%a3,MemoryMbc3RtcPtr@END.w(%a5)
	lea	MemoryMbc3RtcRd(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRamRdExt)
	lea	MemoryMbc3RtcWr(%pc),%a3
	move.l	%a3,2+indexToAddress(gbemuJmpRamWrExt)
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc3RtcRd
MemoryMbc3RtcRd:
	move.l	MemoryMbc3RtcPtr@END.w(%a5),%a3
	move.l	%a3,%a2
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc3RtcWr
MemoryMbc3RtcWr:
	move.l	MemoryMbc3RtcPtr@END.w(%a5),%a3
	lea	MemoryMbc3RtcLatched@END.w(%a5),%a2
	sub.l	%a2,%a3
	lea	MemoryMbc3RtcCurrent@END.w(%a5),%a2
	move.b	%d0,(%a2,%a3.w)
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc3RtcLatch
MemoryMbc3RtcLatch:
	btst	#0,%d0
	jeq	MM3RLQuit

	lea	MemoryMbc3RtcCurrent@END.w(%a5),%a2
	lea	MemoryMbc3RtcLatched@END.w(%a5),%a3
	move.b	(%a2)+,(%a3)+
	move.b	(%a2)+,(%a3)+
	move.b	(%a2)+,(%a3)+
	move.b	(%a2)+,(%a3)+
	move.b	(%a2)+,(%a3)+
MM3RLQuit:
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc3RtcWork
MemoryMbc3RtcWork:
	lea	MemoryMbc3RtcCurrent@END.w(%a5),%a2

	btst	#6,4(%a2)
	jne	MM3RWQuit		/* rtc stopped? */

	subq.b	#1,MemoryMbc3RtcDivider@END.w(%a5)
	jcc	MM3RWQuit
	move.b	#videoFramesPerSec-1,MemoryMbc3RtcDivider@END.w(%a5)
	addq.b	#1,(%a2)
	cmp.b	#60,(%a2)
	jne	MM3RWQuit
	clr.b	(%a2)+
	addq.b	#1,(%a2)
	cmp.b	#60,(%a2)
	jne	MM3RWQuit
	clr.b	(%a2)+
	addq.b	#1,(%a2)
	cmp.b	#24,(%a2)
	jne	MM3RWQuit
	clr.b	(%a2)+
	addq.b	#1,(%a2)+
	jcc	MM3RWQuit
	bchg	#0,(%a2)
	jeq	MM3RWQuit
	bset	#7,(%a2)		/* count values */
MM3RWQuit:
	rts

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc5RomSelect
MemoryMbc5RomSelect:
	and.w	#0xff,%d0		/* only 256 banks (4 MB) */
	add.w	%d0,%d0
	add.w	%d0,%d0
	lea	MemoryRomPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRom1@END.w(%a5)
	jmp	indexToAddress(gbemuCodeSpRomCorrect)

/* --------------------------------------------------------------------	*/

	.globl	MemoryMbc5RamSelect
MemoryMbc5RamSelect:
	and.w	#0x03,%d0		/* only 4 banks (32 KB) */
	add.w	%d0,%d0
	add.w	%d0,%d0
	lea	MemoryRamPages@END.w(%a5),%a3
	move.l	(%a3,%d0.w),MemoryBaseRamExt@END.w(%a5)
	jmp	indexToAddress(gbemuCodeSpRamCorrect)

/* === Functions accessing external memory ============================	*/

	.globl	MemoryRamRdExtNone
MemoryRamRdExtNone:
	move.l	EmulationCpuState.pcBase@END.w(%a5),%a2
	lea	-1(%a0),%a3
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamWrExtNone
MemoryRamWrExtNone:
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamRdExtMbc2
MemoryRamRdExtMbc2:
	move.l	MemoryBaseRamExt@END.w(%a5),%a2
	and.w	#0xa1ff,%d1
	lea	(%a2,%d1.w),%a3
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamWrExtMbc2
MemoryRamWrExtMbc2:
	move.l	MemoryBaseRamExt@END.w(%a5),%a2
	and.w	#0xa1ff,%d1
	and.b	#0x0f,%d0
	move.b	%d0,(%a2,%d1.w)
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamRdExt2K
MemoryRamRdExt2K:
	move.l	MemoryBaseRamExt@END.w(%a5),%a2
	and.w	#0xa7ff,%d1
	lea	(%a2,%d1.w),%a3
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamWrExt2K
MemoryRamWrExt2K:
	move.l	MemoryBaseRamExt@END.w(%a5),%a2
	and.w	#0xa7ff,%d1
	move.b	%d0,(%a2,%d1.w)
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamRdExt8K
MemoryRamRdExt8K:
	move.l	MemoryBaseRamExt@END.w(%a5),%a2
	lea	(%a2,%d1.w),%a3
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamWrExt8K
MemoryRamWrExt8K:
	move.l	MemoryBaseRamExt@END.w(%a5),%a2
	move.b	%d0,(%a2,%d1.w)
	jmp	(%a6)

/* === Functions accessing internal memory ============================	*/

	.globl	MemoryRamNone
MemoryRamNone:
	move.b	%d0,(%a2,%d1.w)
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamTileBg
MemoryRamTileBg:
	cmp.b	(%a2,%d1.w),%d0
	jeq	MrtbQuit		/* no change? */

	move.w	%d3,-(%sp)

	move.b	%d0,(%a2,%d1.w)		/* store pattern in tile table */
MrtbStart:
	and.w	#0xfffe,%d1
	lea	(%a2,%d1.w),%a3
	ror.w	#4,%d1
	move.w	%d1,%d0
	and.w	#0xff,%d1
	move.w	%d1,%d3
	ror.w	#3,%d0
	and.w	#7*0x100*4,%d0
	add.w	%d1,%d1
	add.w	%d1,%d1
	add.w	%d1,%d0			/* prepare some values */

MrtbCheck:
	move.w	VideoBgScxPixel@END.w(%a5),%d2
	move.l	VideoBgUsed@END.w(%a5),%a2
	cmp.b	(%a2,%d3.w),%d2
	jne	MrtbExit		/* tile not used? */

	move.l	VideoBgTile@END.w(%a5),%a2
	add.w	%d0,%a2

	move.l	%a0,-(%sp)
	moveq	#0,%d0
	moveq	#0,%d1
	move.l	VideoBgWiColorL@END.w(%a5),%a0
	jsr	(%a0)
	swap	%d0
	move.l	VideoBgWiColorH@END.w(%a5),%a0
	jsr	(%a0)
	add.l	%d0,%d0
	lea	VideoGrayCopyTable(%pc),%a0
	move.w	(%a0,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a0,%d0.w),%d1
	lsl.l	%d2,%d1
	move.l	%d1,(%a2)
	moveq	#0,%d1
	move.b	(%a3)+,%d1
	or.b	(%a3),%d1
	add.w	%d1,%d1
	move.w	(%a0,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsl.l	%d2,%d1
	move.l	%d1,8*0x100*4(%a2)
	move.l	(%sp)+,%a0		/* convert one line */
MrtbExit:
	move.w	(%sp)+,%d3
MrtbQuit:
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamTileBgWi
MemoryRamTileBgWi:
	cmp.b	(%a2,%d1.w),%d0
	jeq	MrtbQuit		/* no change? */

	move.w	%d3,-(%sp)

	move.b	%d0,(%a2,%d1.w)		/* store pattern in tile table */
MrtbwStart:
	and.w	#0xfffe,%d1
	lea	(%a2,%d1.w),%a3
	ror.w	#4,%d1
	move.w	%d1,%d0
	and.w	#0xff,%d1
	move.w	%d1,%d3
	ror.w	#3,%d0
	and.w	#7*0x100*4,%d0
	add.w	%d1,%d1
	add.w	%d1,%d1
	add.w	%d1,%d0			/* prepare some values */

	move.w	VideoWiWxPixel@END.w(%a5),%d2
	move.l	VideoWiUsed@END.w(%a5),%a2
	cmp.b	(%a2,%d3.w),%d2
	jne	MrtbwQuit		/* tile not used in window? */

	move.l	VideoWiTile@END.w(%a5),%a2
	add.w	%d0,%a2

	move.l	%a0,-(%sp)
	move.w	%d0,-(%sp)
	moveq	#0,%d0
	moveq	#0,%d1
	move.l	VideoBgWiColorL@END.w(%a5),%a0
	jsr	(%a0)
	swap	%d0
	move.l	VideoBgWiColorH@END.w(%a5),%a0
	jsr	(%a0)
	add.l	%d0,%d0
	lea	VideoGrayCopyTable(%pc),%a0
	move.w	(%a0,%d0.w),%d1
	add.w	%d1,%d1
	swap	%d0
	or.w	(%a0,%d0.w),%d1
	lsl.l	%d2,%d1
	move.l	%d1,(%a2)
	moveq	#0,%d1
	move.b	(%a3),%d1
	or.b	1(%a3),%d1
	add.w	%d1,%d1
	move.w	(%a0,%d1.w),%d1
	move.w	%d1,%d0
	add.w	%d0,%d0
	or.w	%d0,%d1
	lsl.l	%d2,%d1
	move.l	%d1,8*0x100*4(%a2)
	move.w	(%sp)+,%d0
	move.l	(%sp)+,%a0		/* convert one line (wi) */
MrtbwQuit:
	jra	MrtbCheck		/* now check for background */

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamTileOb
MemoryRamTileOb:
	cmp.b	(%a2,%d1.w),%d0
	jeq	MrtoQuit		/* no change? */

	move.b	%d0,(%a2,%d1.w)		/* store pattern in tile table */

	move.w	%d3,-(%sp)
	jbsr	VideoObTileCheck(%pc)
	move.w	(%sp)+,%d3
MrtoQuit:
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamTileBgOb
MemoryRamTileBgOb:
	cmp.b	(%a2,%d1.w),%d0
	jeq	MrtoQuit		/* no change? */

	move.b	%d0,(%a2,%d1.w)		/* store pattern in tile table */

	move.w	%d3,-(%sp)
	jbsr	VideoObTileCheck(%pc)
	jra	MrtbStart

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamTileBgWiOb
MemoryRamTileBgWiOb:
	cmp.b	(%a2,%d1.w),%d0
	jeq	MrtoQuit		/* no change? */

	move.b	%d0,(%a2,%d1.w)		/* store pattern in tile table */

	move.w	%d3,-(%sp)
	jbsr	VideoObTileCheck(%pc)
	jra	MrtbwStart

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamMapNone
MemoryRamMapNone:
	cmp.b	(%a2,%d1.w),%d0
	jeq	MrmnQuit		/* no change? */

	move.b	%d0,(%a2,%d1.w)		/* store tile number in map */

	add.w	%d1,%d1
	and.w	#2*videoMapWrap*videoMapWrap*2-1,%d1
	move.l	VideoMapIndices@END.w(%a5),%a2
	moveq	#0,%d2
	move.b	%d0,%d2
	add.w	%d2,%d2
	add.w	%d2,%d2
	move.w	%d2,(%a2,%d1.w)		/* store precalculated index */
MrmnQuit:
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamMapBg
MemoryRamMapBg:
	cmp.b	(%a2,%d1.w),%d0
	jeq	MrmbQuit		/* no change? */

	move.b	%d0,(%a2,%d1.w)		/* store tile number in map */

	add.w	%d1,%d1
	and.w	#2*videoMapWrap*videoMapWrap*2-1,%d1
	move.l	VideoMapIndices@END.w(%a5),%a2
	moveq	#0,%d2
	move.b	%d0,%d2
	move.w	%d2,%d0
	add.w	%d2,%d2
	add.w	%d2,%d2
	move.w	%d2,(%a2,%d1.w)		/* store precalculated index */

	move.w	%d2,%d1
	move.w	VideoBgScxPixel@END.w(%a5),%d2
	move.l	VideoBgUsed@END.w(%a5),%a2
	cmp.b	(%a2,%d0.w),%d2
	jeq	MrmbQuit		/* already converted? */
	move.b	%d2,(%a2,%d0.w)

	move.l	VideoBgWiTileLow@END.w(%a5),%a3
	tst.b	%d0
	jpl	MrmbTileOK
	move.l	VideoBgWiTileHigh@END.w(%a5),%a3
MrmbTileOK:
	move.l	VideoBgTile@END.w(%a5),%a2
	add.w	%d1,%a2
	add.w	%d1,%d1
	add.w	%d1,%d1
	add.w	%d1,%a3
	movem.l	%d3/%a0-%a1/%a4,-(%sp)
	move.l	VideoBgWiColorL@END.w(%a5),%a0
	move.l	VideoBgWiColorH@END.w(%a5),%a1
	lea	VideoGrayCopyTable(%pc),%a4
	jbsr	VideoConvertBgWi(%pc)
	movem.l	(%sp)+,%d3/%a0-%a1/%a4

MrmbQuit:
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamMapWi
MemoryRamMapWi:
	cmp.b	(%a2,%d1.w),%d0
	jeq	MrmwQuit		/* no change? */

	move.b	%d0,(%a2,%d1.w)		/* store tile number in map */

	add.w	%d1,%d1
	and.w	#2*videoMapWrap*videoMapWrap*2-1,%d1
	move.l	VideoMapIndices@END.w(%a5),%a2
	moveq	#0,%d2
	move.b	%d0,%d2
	move.w	%d2,%d0
	add.w	%d2,%d2
	add.w	%d2,%d2
	move.w	%d2,(%a2,%d1.w)		/* store precalculated index */

	move.w	%d2,%d1
	move.w	VideoWiWxPixel@END.w(%a5),%d2
	move.l	VideoWiUsed@END.w(%a5),%a2
	cmp.b	(%a2,%d0.w),%d2
	jeq	MrmwQuit		/* already converted? */
	move.b	%d2,(%a2,%d0.w)

	move.l	VideoBgWiTileLow@END.w(%a5),%a3
	tst.b	%d0
	jpl	MrmwTileOK
	move.l	VideoBgWiTileHigh@END.w(%a5),%a3
MrmwTileOK:
	move.l	VideoWiTile@END.w(%a5),%a2
	add.w	%d1,%a2
	add.w	%d1,%d1
	add.w	%d1,%d1
	add.w	%d1,%a3
	movem.l	%d3/%a0-%a1/%a4,-(%sp)
	move.l	VideoBgWiColorL@END.w(%a5),%a0
	move.l	VideoBgWiColorH@END.w(%a5),%a1
	lea	VideoGrayCopyTable(%pc),%a4
	jbsr	VideoConvertBgWi(%pc)
	movem.l	(%sp)+,%d3/%a0-%a1/%a4

MrmwQuit:
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamMapBgWi
MemoryRamMapBgWi:
	cmp.b	(%a2,%d1.w),%d0
	jeq	MrmbwWiQuit		/* no change? */

	move.l	VideoBgWiTileLow@END.w(%a5),%a3
	move.b	%d0,(%a2,%d1.w)		/* store tile number in map */
	jpl	MrmbwTileOK
	move.l	VideoBgWiTileHigh@END.w(%a5),%a3
MrmbwTileOK:
	add.w	%d1,%d1
	and.w	#2*videoMapWrap*videoMapWrap*2-1,%d1
	move.l	VideoMapIndices@END.w(%a5),%a2
	moveq	#0,%d2
	move.b	%d0,%d2
	move.w	%d2,%d0
	add.w	%d2,%d2
	add.w	%d2,%d2
	move.w	%d2,(%a2,%d1.w)		/* store precalculated index */

	move.w	%d0,-(%sp)
	move.w	%d2,%d1
	add.w	%d2,%d2
	add.w	%d2,%d2
	add.w	%d2,%a3			/* prepare some values */

	move.w	VideoBgScxPixel@END.w(%a5),%d2
	move.l	VideoBgUsed@END.w(%a5),%a2
	cmp.b	(%a2,%d0.w),%d2
	jeq	MrmbwBgQuit		/* already converted? */
	move.b	%d2,(%a2,%d0.w)

	move.l	VideoBgTile@END.w(%a5),%a2
	add.w	%d1,%a2
	movem.l	%d3/%a0-%a1/%a4,-(%sp)
	move.l	VideoBgWiColorL@END.w(%a5),%a0
	move.l	VideoBgWiColorH@END.w(%a5),%a1
	lea	VideoGrayCopyTable(%pc),%a4
	jbsr	VideoConvertBgWi(%pc)	/* convert for background */
	lea	-8*2(%a3),%a3
	movem.l	(%sp)+,%d3/%a0-%a1/%a4
MrmbwBgQuit:
	move.w	(%sp)+,%d0

	move.w	VideoWiWxPixel@END.w(%a5),%d2
	move.l	VideoWiUsed@END.w(%a5),%a2
	cmp.b	(%a2,%d0.w),%d2
	jeq	MrmbwWiQuit		/* already converted? */
	move.b	%d2,(%a2,%d0.w)

	add.w	%d0,%d0
	add.w	%d0,%d0
	move.l	VideoWiTile@END.w(%a5),%a2
	add.w	%d0,%a2
	movem.l	%d3/%a0-%a1/%a4,-(%sp)
	move.l	VideoBgWiColorL@END.w(%a5),%a0
	move.l	VideoBgWiColorH@END.w(%a5),%a1
	lea	VideoGrayCopyTable(%pc),%a4
	jbsr	VideoConvertBgWi(%pc)	/* convert for window */
	movem.l	(%sp)+,%d3/%a0-%a1/%a4
MrmbwWiQuit:
	jmp	(%a6)

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamOam
MemoryRamOam:
	pea	(%a6)			/* save return address */

	moveq	#0xfc-0x100,%d2
	and.w	%d1,%d2
	lea	(%a2,%d2.w),%a3
	move.l	(%a3),%d2
	move.b	%d0,(%a2,%d1.w)		/* store byte in memory */
	move.l	(%a3),%d0

	cmp.b	#40<<2,%d1
	jcc	MroeQuit		/* object # too big? */

/* fall through to MemoryRamOamEntry */

/* --------------------------------------------------------------------	*/

	.globl	MemoryRamOamEntry
MemoryRamOamEntry:
#define mroeRegs %d3-%d5/%a0/%a1/%a4
/*	d0.l	new value (already stored)
 *	d1.b	offset into oam
 *	d2.l	old value
 *	a2	(scratch)
 *	a3	pointer into oam
 *	a6	(scratch)
 */
	eor.l	%d0,%d2
	jeq	MroeQuit		/* no change? */

	movem.l	mroeRegs,-(%sp)

/* - - check changes in y or x - - - - - - - - - - - - - - - - - - - -	*/
	move.l	#0xffff7f,%d5
	and.l	%d2,%d5			/* save changes in x/tile/flags */

	move.l	VideoObFirst@END.w(%a5),%a6
	move.w	#0x00fc,%d0
	and.w	%d1,%d0
	lsl.w	#4-1,%d0
	move.w	%d0,%d1
	lsl.w	#3,%d0
	sub.w	%d1,%d0
	add.w	#videoScreenHeight*2*2,%d0
	lea	(%a6,%d0.w),%a2		/* prepare some values */

	eor.w	%d5,%d2
	jne	MroeXYLink
	swap	%d2
	tst.w	%d2
	jeq	MroeXYQuit		/* no change? */
MroeXYLink:
	jbsr	VideoObLink(%pc)	/* re-link object */
MroeXYQuit:

/* - - check changes in x, tile #, or flags - - - - - - - - - - - - - -	*/
	tst.l	%d5
	jeq	MroeXTFQuit		/* no change? */

	move.w	(%a3)+,%d4
	move.w	VideoObTilemask@END.w(%a5),%d3
	and.b	(%a3)+,%d3
	move.b	(%a3)+,%d0
	lsl.w	#3+1,%d3
	move.l	MemoryBaseRamVid@END.w(%a5),%a3
	lea	0x8000-0x10000(%a3),%a3
	add.w	%d3,%a3
	move.w	VideoObCount@END.w(%a5),%d3
	moveq	#0x10,%d2
	and.b	%d0,%d2
	jne	MroeXTFObp1
	move.l	VideoObColor0L@END.w(%a5),%a0
	move.l	VideoObColor0H@END.w(%a5),%a1
	jra	MroeXTFConv
MroeXTFObp1:
	move.l	VideoObColor1L@END.w(%a5),%a0
	move.l	VideoObColor1H@END.w(%a5),%a1
MroeXTFConv:
	lea	VideoGrayCopyTable(%pc),%a4
	jbsr	VideoConvertOb(%pc)
MroeXTFQuit:

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	*/
	movem.l	(%sp)+,mroeRegs
MroeQuit:
	rts

/* === The end ========================================================	*/
