/************************************************************************
 *  emulation_s.S
 *                      Main emulation routines (assembler part)
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
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:39:09  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-16  Bodo Wenzel     Saving states now possible
 *  2001-07-25  Bodo Wenzel     Got from runmode.asm, changed syntax to
 *                              CPP + GAS
 *  2001-07-23  Bodo Wenzel     v.4.0, Many sources converted to C,
 *                              compiler/assembler now GCC
 *  2001-07-01  Bodo Wenzel     v.3.3b, Bugfix corrected ;-)
 *  2001-06-23  Bodo Wenzel     v.3.3, Bug in MBC1 code corrected
 *  2001-02-22  Bodo Wenzel     v.3.2 (v.3.14159), Hsync interrupts and
 *                              support of more cartridges
 *  2000-11-18  Bodo Wenzel     v.3.1, Support of color devices
 *  2000-11-03  Bodo Wenzel     v.3.0a, No more "Visor crash"
 *  2000-10-16  Bodo Wenzel     v.3.0, Interrupts and timer
 *  2000-09-30  Bodo Wenzel     v.2.1, Scroller in GB code
 *  2000-09-11  Bodo Wenzel     v.2.0, Nearly full screen emulation
 *  2000-05-07  Bodo Wenzel     v.1.0, First working version
 *  2000-05-06  Bodo Wenzel     v.0.9, First working version with built-in
 *                              games
 *  2000-05-01  Bodo Wenzel     v.0.4, Quick'n'dirty screen conversion,
 *                              splitting in modules, and usage of more
 *                              registers
 *  2000-04-25  Bodo Wenzel     v.0.3, Errors fixed with Palm IIIC, V
 *  2000-04-19  Bodo Wenzel     v.0.2, Creation of user interface
 *  2000-04-15  Bodo Wenzel     v.0.1, Creation of user interface without
 *                              gray image
 *  2000-03-28  Bodo Wenzel     v.0.0, Creation of user interface without
 *                              emulator mock-up and gray image
 ************************************************************************
 */

/* === Includes =======================================================	*/

#include "emulation.h"
#include "video.h"

/* === Global variables ===============================================	*/

	.bss

	.globl	EmulationCpuState
EmulationCpuState:
/* NOTE: this has to match the struct in emulation.h! */
	.globl	EmulationCpuState.pcBase
EmulationCpuState.pcBase:	.space	4	/* base address of PC */
	.globl	EmulationCpuState.pcOffset
EmulationCpuState.pcOffset:	.space	2	/* PC in GB format */
	.globl	EmulationCpuState.spBase
EmulationCpuState.spBase:	.space	4	/* base address of SP */
	.globl	EmulationCpuState.spOffset
EmulationCpuState.spOffset:	.space	2	/* SP in GB format */
	.globl	EmulationCpuState.a
EmulationCpuState.a:		.space	1	/* A */
	.globl	EmulationCpuState.f
EmulationCpuState.f:		.space	1	/* F */
	.globl	EmulationCpuState.b
EmulationCpuState.b:		.space	1	/* B */
	.globl	EmulationCpuState.c
EmulationCpuState.c:		.space	1	/* C */
	.globl	EmulationCpuState.d
EmulationCpuState.d:		.space	1	/* D */
	.globl	EmulationCpuState.e
EmulationCpuState.e:		.space	1	/* E */
	.globl	EmulationCpuState.h
EmulationCpuState.h:		.space	1	/* H */
	.globl	EmulationCpuState.l
EmulationCpuState.l:		.space	1	/* L */

	.globl	EmulationIo
EmulationIo:
/* NOTE: this has to match the struct in emulation.h! */
	.globl	EmulationIo.p1
EmulationIo.p1:			.space	1	/* joypad, used bit 1/0 */
	.globl	EmulationIo.sb
EmulationIo.sb:			.space	1	/* serial transfer data */
	.globl	EmulationIo.sc
EmulationIo.sc:			.space	1	/* serial transfer ctrl */
				.space	1
EmulationIo.div:		.space	1	/* divider, not used */
EmulationIo.tima:		.space	1	/* timer cnt, not used */
	.globl	EmulationIo.tma
EmulationIo.tma:		.space	1	/* timer modulo */
	.globl	EmulationIo.tac
EmulationIo.tac:		.space	1	/* timer control */
				.space	0x07
	.globl	EmulationIo.if_
EmulationIo.if_:		.space	1	/* interrupt flags */
EmulationIo.ch1_ent:		.space	1	/* sound, not emulated */
EmulationIo.ch1_wave:		.space	1	/* sound, not emulated */
EmulationIo.ch1_env:		.space	1	/* sound, not emulated */
EmulationIo.ch1_freq_lo:	.space	1	/* sound, not emulated */
EmulationIo.ch1_freq_hi_kick:	.space	1	/* sound, not emulated */
				.space	1
EmulationIo.ch2_wave:		.space	1	/* sound, not emulated */
EmulationIo.ch2_env:		.space	1	/* sound, not emulated */
EmulationIo.ch2_freq_lo:	.space	1	/* sound, not emulated */
EmulationIo.ch2_freq_hi_kick:	.space	1	/* sound, not emulated */
EmulationIo.ch3_onoff:		.space	1	/* sound, not emulated */
EmulationIo.ch2_length:		.space	1	/* sound, not emulated */
EmulationIo.ch3_volume:		.space	1	/* sound, not emulated */
EmulationIo.ch3_freq_lo:	.space	1	/* sound, not emulated */
EmulationIo.ch3_freq_hi_kick:	.space	1	/* sound, not emulated */
				.space	1
EmulationIo.ch4_length:		.space	1	/* sound, not emulated */
EmulationIo.ch4_env:		.space	1	/* sound, not emulated */
EmulationIo.ch4_poly:		.space	1	/* sound, not emulated */
EmulationIo.ch4_kick:		.space	1	/* sound, not emulated */
EmulationIo.snd_vin:		.space	1	/* sound, not emulated */
EmulationIo.snd_stereo:		.space	1	/* sound, not emulated */
EmulationIo.snd_stat:		.space	1	/* sound, not emulated */
				.space	0x09
EmulationIo.wave:		.space	0x10	/* sound, not emulated */
	.globl	EmulationIo.lcdc
EmulationIo.lcdc:		.space	1	/* LCD control */
	.globl	EmulationIo.stat
EmulationIo.stat:		.space	1	/* LCD status */
	.globl	EmulationIo.scy
EmulationIo.scy:		.space	1	/* LCD scroll y */
	.globl	EmulationIo.scx
EmulationIo.scx:		.space	1	/* LCD scroll x */
	.globl	EmulationIo.ly
EmulationIo.ly:			.space	1	/* LCD y coordinate */
	.globl	EmulationIo.lyc
EmulationIo.lyc:		.space	1	/* LCD y compare */
	.globl	EmulationIo.dma
EmulationIo.dma:		.space	1	/* LCD DMA transfer */
	.globl	EmulationIo.bgp
EmulationIo.bgp:		.space	1	/* LCD bg. palette */
	.globl	EmulationIo.obp0
EmulationIo.obp0:		.space	1	/* LCD object palette 0 */
	.globl	EmulationIo.obp1
EmulationIo.obp1:		.space	1	/* LCD object palette 1 */
	.globl	EmulationIo.wy
EmulationIo.wy:			.space	1	/* LCD window y */
	.globl	EmulationIo.wx
EmulationIo.wx:			.space	1	/* LCD window x */
				.space	0x34
EmulationIo.ram:		.space	0x7f	/* CPU RAM */
	.globl	EmulationIo.ie
EmulationIo.ie:			.space	1	/* interrupt enable */

	.text

/* === Function for starting the emulation ============================	*/

	.globl	EmulationStartKernel
EmulationStartKernel:
#define eskRegs %d3-%d7/%a2-%a4/%a6
	movem.l	eskRegs,-(%sp)

	move.l	EmulationEmulator@END.w(%a5),%a4

	move.l	EmulationCpuState.pcBase@END.w(%a5),%a0
	add.w	EmulationCpuState.pcOffset@END.w(%a5),%a0
	move.l	EmulationCpuState.spBase@END.w(%a5),%a1
	add.w	EmulationCpuState.spOffset@END.w(%a5),%a1
	move.b	EmulationCpuState.a@END.w(%a5),%d7
	move.b	EmulationCpuState.f@END.w(%a5),%d6
	swap	%d6
	move.b	EmulationCpuState.b@END.w(%a5),%d5
	swap	%d5
	move.b	EmulationCpuState.c@END.w(%a5),%d5
	move.b	EmulationCpuState.d@END.w(%a5),%d4
	swap	%d4
	move.b	EmulationCpuState.e@END.w(%a5),%d4
	move.b	EmulationCpuState.h@END.w(%a5),%d3
	swap	%d3
	move.b	EmulationCpuState.l@END.w(%a5),%d3	/* get CPU */

	move.w	VideoCycles@END.w(%a5),%d6
	addq.w	#1,%d6			/* correction of starting opcode */

/* *** HERE IS IT! THE EMULATOR STARTS EXECUTION... *******************	*/
	jsr	(%a4)
/* ********************************************************************	*/

	move.b	%d0,EmulationError@END.w(%a5)

	move.w	%d6,VideoCycles@END.w(%a5)	/* currently unnecessary */

	sub.l	EmulationCpuState.pcBase@END.w(%a5),%a0
	move.w	%a0,EmulationCpuState.pcOffset@END.w(%a5)
	move.b	%d3,EmulationCpuState.l@END.w(%a5)
	swap	%d3
	move.b	%d3,EmulationCpuState.h@END.w(%a5)
	move.b	%d4,EmulationCpuState.e@END.w(%a5)
	swap	%d4
	move.b	%d4,EmulationCpuState.d@END.w(%a5)
	move.b	%d5,EmulationCpuState.c@END.w(%a5)
	swap	%d5
	move.b	%d5,EmulationCpuState.b@END.w(%a5)
	swap	%d6
	move.b	%d6,EmulationCpuState.f@END.w(%a5)
	move.b	%d7,EmulationCpuState.a@END.w(%a5)	/* save CPU */

	movem.l	(%sp)+,eskRegs
	rts

/* === Functions for additional support ===============================	*/

/* --- Emulation of opcode $10 $00 (STOP) -----------------------------	*/

	.globl	EmulationOpcodeStop
EmulationOpcodeStop:
	bsr	VideoClearScreen

	lea	EmulationIntIm@END.w(%a5),%a2
	move.b	(%a2),-(%sp)
	move.b	#0x00,(%a2)
	pea	EosAbort(%pc)		/* set up for waiting */
EosWait:
	jbsr	VideoUserPoll(%pc)
	moveq	#0x0f,%d0
	lea	EmulationJoypad@END.w(%a5),%a2
	and.b	emulationJoypadBoth(%a2),%d0
	cmp.b	#0x0f,%d0
	beq.s	EosWait			/* wait for user */

	addq.w	#4,%sp
	move.b	(%sp)+,EmulationIntIm@END.w(%a5)
	rts				/* back to emulation */

/* If VideoUserPoll tries to return to the emulation caller, it will
 * resume here:
 */
EosAbort:
	move.b	(%sp)+,EmulationIntIm@END.w(%a5)
	subq.w	#2,%a0			/* restore state before STOP */

	addq.w	#4,%sp			/* ignore return to emulation */
	rts				/* return to emulation caller */

/* --- Return after executed breakpoint	-------------------------------	*/

#if 0 /* currently unused */
	.globl	EmulationBreakpointReturn
EmulationBreakpointReturn:
	addq.w	#4,%sp			/* ignore return to emulation */
	moveq	#0,%d0
	rts				/* return to emulation caller */
#endif

/* === The end ========================================================	*/
