/************************************************************************
 *  gbemu_io.inc
 *                      Emulation of the Gameboy CPU
 *                      (Input and output)
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
 *  Revision 1.1.1.1  2001/12/16 13:39:54  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-22  Bodo Wenzel   Some optimization
 *  2001-09-16  Bodo Wenzel   Saving states now possible
 *  2001-07-23  Bodo Wenzel   Changed syntax to CPP + GAS
 *  2001-01-28  Bodo Wenzel   Dummy SC emulation
 *  2000-11-16  Bodo Wenzel   Better joypad emulation
 *  2000-10-15  Bodo Wenzel   Interrupts and timer
 *  2000-09-29  Bodo Wenzel   Some improvements in source text
 *  2000-09-09  Bodo Wenzel   Now realy full screen emulation
 *  2000-06-18  Bodo Wenzel   Creation
 ************************************************************************
 */

/* ======================================================================
 * All i/o accesses are generated with:
 *	d0.b	byte to be written
 *	d1.b	(io_rd: must be preserved)
 *	d2.b	(io_rd: must be preserved)
 *	a2	68000 base memory address (used after io_rd)
 *	a2	68000 base memory address
 *	a3	68000 memory address (used after io_rd,
 *		  also as "io_lea")
 *	a6	68000 return address
 * ======================================================================
 */

/* === Macros for generating macros... ================================	*/

.macro	in_std	num		/* standard read */
.macro	in_\num
	lea	EmulationIo+0x0100@END.w(%a5),%a2
	lea	0x\num-0x0100(%a2),%a3
	jmp	(%a6)
	.endm
	.endm

.macro	out_std	num		/* standard write */
.macro	out_\num
	move.b	%d0,EmulationIo+0x\num@END.w(%a5)
	jmp	(%a6)		/* no special treatment */
	.endm
	.endm

.macro	out_nul	num
.macro	out_\num		/* write ignored */
	jmp	(%a6)
	.endm
	.endm

.macro	in_jmp	num
.macro	in_\num
	jmp	0.l		/* jump vector to external function */
	.endm
	.endm

.macro	out_jmp	num
.macro	out_\num
	jmp	0.l		/* jump vector to external function */
	.endm
	.endm

/* === And now the actual macros... ===================================	*/

.macro	in_00			/* P1 joypad */
	move.l	EmulationJoypadPtr@END.w(%a5),%a3
	jmp	(%a6)
	.endm

.macro	out_00			/* P1 joypad */
	lsr.b	#4,%d0
	and.w	#3,%d0
	lea	EmulationJoypad@END.w(%a5),%a3
	add.w	%d0,%a3
	move.l	%a3,EmulationJoypadPtr@END.w(%a5)
	jmp	(%a6)
	.endm

	in_std	01		/* SB serial transfer data */

.macro	out_01			/* SB serial transfer data */
	jmp	(%a6)
io02m:	bclr	#7,%d0
	bset	#3,EmulationIo.if_@END.w(%a5)
	bra	io02q
	.endm

	in_std	02		/* SC serial transfer control */

.macro	out_02			/* SC serial transfer control */
	btst	#0,%d0
	bne	io02m
io02q:	move.b	%d0,EmulationIo+0x02@END.w(%a5)
	jmp	(%a6)
	.endm

	in_std	03		/* 0x03 (not existing) */
	out_nul	03

.macro	in_04			/* DIV divider */
	lea	VideoTimerDiv@END.w(%a5),%a3
	jmp	(%a6)
	.endm

.macro	out_04			/* DIV divider */
	move.w	#0,VideoTimerDiv@END.w(%a5)
	jmp	(%a6)
	.endm

.macro	in_05			/* TIMA timer counter */
	lea	VideoTimerTima@END.w(%a5),%a3
	jmp	(%a6)
	.endm

.macro	out_05			/* TIMA timer counter */
	lea	VideoTimerTima@END.w(%a5),%a3
	move.w	#0,(%a3)
	move.b	%d0,(%a3)
	jmp	(%a6)
	.endm

	in_std	06		/* TMA timer modulo */
	out_std	06		/* TMA timer modulo */

	in_std	07		/* TAC timer control */
	out_jmp	07		/* TAC timer control */

	.irpc	i,89abcde
	in_std	0\i		/* 0x08..0x0e (not existing) */
	out_nul	0\i
	.endr

	in_std	0f		/* IF interrupt flags */
	out_std	0f		/* IF interrupt flags */

	in_std	10		/* CH1_ENT (sound) */
	out_std	10		/* CH1_ENT (sound) */

	in_std	11		/* CH1_WAVE (sound) */
	out_std	11		/* CH1_WAVE (sound) */

	in_std	12		/* CH1_ENV (sound) */
	out_std	12		/* CH1_ENV (sound) */

	in_std	13		/* 0x13 (not existing) */
	out_nul	13		/* CH1_FREQ_LO (sound) */

	in_std	14		/* CH1_FREQ_HI_KICK (sound) */
	out_std	14		/* CH1_FREQ_HI_KICK (sound) */

	in_std	15		/* 0x15 (not existing) */
	out_nul	15

	in_std	16		/* CH2_WAVE (sound) */
	out_std	16		/* CH2_WAVE (sound) */

	in_std	17		/* CH2_ENV (sound) */
	out_std	17		/* CH2_ENV (sound) */

	in_std	18		/* 0x18 (not existing) */
	out_nul	18		/* CH2_FREQ_LO (sound) */

	in_std	19		/* CH2_FREQ_HI_KICK (sound) */
	out_std	19		/* CH2_FREQ_HI_KICK (sound) */

	in_std	1a		/* CH3_ONOFF (sound) */
	out_std	1a		/* CH3_ONOFF (sound) */

	in_std	1b		/* CH3_LENGTH (sound) */
	out_std	1b		/* CH3_LENGTH (sound) */

	in_std	1c		/* CH3_VOLUME (sound) */
	out_std	1c		/* CH3_VOLUME (sound) */

	in_std	1d		/* 0x1d (not existing) */
	out_nul	1d		/* CH3_FREQ_LO (sound) */

	in_std	1e		/* CH3_FREQ_HI_KICK (sound) */
	out_std	1e		/* CH3_FREQ_HI_KICK (sound) */

	in_std	1f		/* 0x1f (not existing) */
	out_nul	1f

	in_std	20		/* CH4_LENGTH (sound) */
	out_std	20		/* CH4_LENGTH (sound) */

	in_std	21		/* CH4_ENV (sound) */
	out_std	21		/* CH4_ENV (sound) */

	in_std	22		/* CH4_POLY (sound) */
	out_std	22		/* CH4_POLY (sound) */

	in_std	23		/* CH4_KICK (sound) */
	out_std	23		/* CH4_KICK (sound) */

	in_std	24		/* SND_VIN (sound) */
	out_std	24		/* SND_VIN (sound) */

	in_std	25		/* SND_STEREO (sound) */
	out_std	25		/* SND_STEREO (sound) */

	in_std	26		/* SND_STAT (sound) */
	out_std	26		/* SND_STAT (sound) */

	.irpc	i,789abcdef
	in_std	2\i		/* 0x27..0x2f (not existing) */
	out_nul	2\i
	.endr

	.irpc	i,0123456789abcdef
	in_std	3\i		/* RAM wave pattern */
	out_std	3\i
	.endr

	in_std	40		/* LCDC LCD control */
	out_jmp	40		/* LCDC LCD control */

	in_jmp	41		/* STAT LCD status */
	out_jmp	41		/* STAT LCD status */

	in_std	42		/* SCY scroll Y */
	out_jmp	42		/* SCY scroll Y */

	in_std	43		/* SCX scroll X */
	out_jmp	43		/* SCX scroll X */

.macro	in_44			/* LY LCD Y coordinate */
	moveq	#143-1-0x100,%d0
	sub.w	VideoLyPrv@END.w(%a5),%d0
	lea	EmulationIo.ly@END.w(%a5),%a3
	move.b	%d0,(%a3)
	jmp	(%a6)
	.endm

.macro	out_44			/* LY LCD Y coordinate */
	/* it's not clear if it can be reset by writing... */
	jmp	(%a6)
	.endm

	in_std	45		/* LYC LCD Y compare */
	out_jmp	45		/* LYC LCD Y compare */

	in_std	46

.macro	out_46			/* DMA transfer and start address */
	pea	(%a6)
	clr.b	%d1
	move.l	2+indexToAddress(gbemuJmpIoWrDma),%a6
	move.b	%d0,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetRd+dontOptHi(%a4)
	.endm

	in_std	47		/* BGP background palette */
	out_jmp	47		/* BGP background palette */

	in_std	48		/* OBP0 object palette 0 */
	out_jmp	48		/* OBP0 object palette 0 */

	in_std	49		/* OBP1 object palette 1 */
	out_jmp	49		/* OBP1 object palette 1 */

	in_std	4a		/* WY window Y */
	out_jmp	4a		/* WY window Y */

	in_std	4b		/* WX window X */
	out_jmp	4b		/* WX window X */

	.irpc	i,cdef
	in_std	4\i		/* 0x4c..0x4f (not existing) */
	out_nul	4\i
	.endr

	.irpc	i,567
	.irpc	j,0123456789abcdef
	in_std	\i\j		/* 0x50..0x7f (not existing) */
	out_nul	\i\j
	.endr
	.endr

	.irpc	i,89abcde
	.irpc	j,0123456789abcdef
	in_std	\i\j		/* RAM in CPU */
	out_std	\i\j
	.endr
	.endr

	.irpc	i,0123456789abcde
	in_std	f\i		/* RAM in CPU */
	out_std	f\i
	.endr

	in_std	ff		/* IE interrupt enable */

.macro	out_ff			/* IE interrupt enable */
	move.b	%d0,EmulationIo.ie@END.w(%a5)
	and.b	EmulationIntIme@END.w(%a5),%d0
	move.b	%d0,EmulationIntIm@END.w(%a5)
	pea	(%a6)
	jmp	indexToAddress(gbemuCodeCheckInt)
	.endm

/* === The end ========================================================	*/
