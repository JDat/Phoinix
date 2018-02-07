/************************************************************************
 *  gbemu.S
 *                      Emulation of the Gameboy CPU
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
 *  Revision 1.1.1.1  2001/12/16 13:38:24  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-22  Bodo Wenzel   Some optimization
 *  2001-09-17  Bodo Wenzel   Saving states now possible
 *  2001-07-21  Bodo Wenzel   Changed syntax to CPP + GAS
 *  2001-02-05  Bodo Wenzel   New stack management
 *  2001-01-28  Bodo Wenzel   Correction at debug modes
 *  2000-10-03  Bodo Wenzel   Interrupts
 *  2000-09-27  Bodo Wenzel   DAA implemented
 *  2000-09-09  Bodo Wenzel   Some improvements
 *  2000-06-06  Bodo Wenzel   New memory management
 *  2000-05-04  Bodo Wenzel   Squeezing code snippets
 *  2000-04-28  Bodo Wenzel   Several emulations in one executable
 *  2000-02-29  Bodo Wenzel   Creation
 ************************************************************************
 */

/* ======================================================================
 * The registers of cpus are mapped that way:
 *	Motorola	Gameboy
 *	d0		(scratch) data byte
 *	d1		(scratch) lower address byte
 *	d2		(scratch) higher address byte
 *	d3		h(23..16) / l(7..0)
 *	d4		d(23..16) / e(7..0)
 *	d5		b(23..16) / c(7..0)
 *	d6		flags(31..16) / opcode timer (15..0)
 *	d7		a(7..0)
 *	a0		PC as 68000 address
 *	a1		SP as 68000 address
 *	a2		(scratch) base pointer for memory access
 *	a3		(scratch) pointer for memory access
 *	a4		(pointer to label "Gbemu")
 *	a5		(pointer to globals)
 *	a6		(scratch) temporary return address
 *	a7		(SP for 68000)
 * ======================================================================
 */
	
/* === Section definition =============================================	*/
	
	.section	emulator,"x"

/* === Includes =======================================================	*/

#include "gbemu.h"
#include "emulation.h"
#include "video.h"

/* === Constants ======================================================	*/

offsetOp	=	0x00			/* the code snippets are */
offsetCb	=	offsetOp+gbemuOpSize	/* interleaved this way */
offsetRd	=	offsetCb+gbemuCbSize	/* on pages of 256 bytes */
offsetWr	=	offsetRd+gbemuRdSize
offsetIn	=	offsetWr+gbemuWrSize
offsetOut	=	offsetIn+gbemuInSize
offsetXt	=	offsetOut+gbemuOutSize

dontOptHi	=	0x5500		/* otherwise the offsets will be */
dontOptLo	=	0x55		/* optimized to non-functioning */

m68kFlagZ	=	2+16		/* masks for Motorola status */
m68kFlagC	=	4+16
m68kMaskZ	=	1<<(m68kFlagZ-16)
m68kMaskC	=	1<<(m68kFlagC-16)

gbFlagZ		=	-0x80		/* masks for GB status */
gbFlagC		=	0x10

/* === Macros for less typing =========================================	*/

/* --- Administration -------------------------------------------------	*/

.macro	return	ret			/* return to caller with code */
	moveq	#\ret,%d0
	rts
	.endm

.macro	next_pc				/* execute next instruction */
	dbra	%d6,goon\@
	jsr	indexToAddress(gbemuJmpLcdLine)
goon\@:	move.b	(%a0)+,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetOp+dontOptHi(%a4)
	.endm

/* --- Jumps and calls ------------------------------------------------	*/

.macro	do_jp				/* get jump address */
	move.b	(%a0)+,%d1
	mem_rd	(%a0)+
	move.l	%a2,EmulationCpuState.pcBase@END.w(%a5)
	.endm

.macro	do_push	addr			/* push address */
	sp_push
	move.l	\addr,%d0
	sub.l	EmulationCpuState.pcBase@END.w(%a5),%d0
	move.w	%d0,-(%sp)
	move.b	(%sp)+,-(%a1)
	move.b	%d0,-(%a1)
	.endm

.macro	do_call				/* push address and get call addr */
	do_push	%a3
	do_jp
	.endm

.macro	do_ret				/* get return address from stack */
	move.b	(%a1)+,%d1
	mem_rd	(%a1)+
	move.l	%a2,EmulationCpuState.pcBase@END.w(%a5)
	move.l	%a3,%a0
	sp_pop
	.endm

/* --- Memory and input/output ----------------------------------------	*/

.macro	mem_rd	hi			/* read mem (input, hi=0xff) */
	lea	ret\@-Gbemu(%a4),%a6
	move.b	\hi,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetRd+dontOptHi(%a4)
ret\@:
	.endm

.macro	mem_wr	hi			/* write mem (output, hi=0xff) */
	lea	ret\@-Gbemu(%a4),%a6
	move.b	\hi,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetWr+dontOptHi(%a4)
ret\@:
	.endm

.macro	io_rd	lo			/* read input */
	lea	ret\@-Gbemu(%a4),%a6
	move.b	\lo,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetIn+dontOptHi(%a4)
ret\@:
	.endm

.macro	io_wr	lo			/* write output */
	lea	ret\@-Gbemu(%a4),%a6
	move.b	\lo,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetOut+dontOptHi(%a4)
ret\@:
	.endm

.macro	sp_push				/* check and correct pushing SP */
	subq.b	#2,EmulationCpuState.spOffset+1@END.w(%a5)
	jcc	goon\@
	jsr	indexToAddress(gbemuCodeSpPushCorrect)
	/* Here could be a check for writing into ROM... */
goon\@:
	.endm

.macro	sp_pop				/* check and correct popping SP */
	addq.b	#2,EmulationCpuState.spOffset+1@END.w(%a5)
	jcc	goon\@
	jsr	indexToAddress(gbemuCodeSpPopCorrect)
goon\@:
	.endm

/* === Macros for opcodes =============================================	*/

#include "gbemu_op.inc"

/* === Macros for opcodes after prefix byte 0xcb ======================	*/

#include "gbemu_cb.inc"

/* === Macros for reading and writing memory ==========================	*/

#include "gbemu_rw.inc"

/* === Macros for input and output ====================================	*/

#include "gbemu_io.inc"

/* === Macros for extensions ==========================================	*/

#include "gbemu_xt.inc"

/* === And now produce the code! ======================================	*/

.macro	combo	num
	.org	((0x\num ^ 0x80) << 8) + offsetOp
	op_\num
	.org	((0x\num ^ 0x80) << 8) + offsetCb
	cb_\num
	.org	((0x\num ^ 0x80) << 8) + offsetRd
	rd_\num
	.org	((0x\num ^ 0x80) << 8) + offsetWr
	wr_\num
	.org	((0x\num ^ 0x80) << 8) + offsetIn
	in_\num
	.org	((0x\num ^ 0x80) << 8) + offsetOut
	out_\num
	.org	((0x\num ^ 0x80) << 8) + offsetXt
	xt_\num
	.endm

/* --------------------------------------------------------------------	*/

	.org	0x0000			/* first codes 0x80 to 0xff */

	.irpc	i,89abcdef
	.irpc	j,0123456789abcdef
	combo	\i\j
	.endr
	.endr

	.org	0x8000			/* now codes 0x00 to 0x7f */

Gbemu:					/* "The Middle of the Code" */
					/* that way we can reach all 64K */

	.irpc	i,01234567
	.irpc	j,0123456789abcdef
	combo	\i\j
	.endr
	.endr

/* === The end ========================================================	*/
