/************************************************************************
 *  gbemu_rw.inc
 *                      Emulation of the Gameboy CPU
 *                      (Reading and writing memory)
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
 *  Revision 1.1.1.1  2001/12/16 13:38:35  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-22  Bodo Wenzel   Some optimization
 *  2001-07-23  Bodo Wenzel   Changed syntax to CPP + GAS
 *  2001-01-31  Bodo Wenzel   Support of more cartridges
 *  2000-10-02  Bodo Wenzel   Interrupts
 *  2000-08-05  Bodo Wenzel   Some improvements
 *  2000-06-06  Bodo Wenzel   New memory management
 *  2000-05-06  Bodo Wenzel   External RAM pageable
 *  2000-04-24  Bodo Wenzel   Creation
 ************************************************************************
 */

/* ======================================================================
 *	All memory accesses are generated with:
 *	d0.b	byte to be written
 *	d1.b	lower address byte (mem_rd: must be preserved)
 *	-	higher address byte (is implicit)
 *	d2.b	(mem_rd: must be preserved)
 *	a2	68000 base memory address (used after mem_rd)
 *	a3	68000 memory address (used after mem_rd,
 *		  also as "mem_lea")
 *	a6	68000 return address
 * ======================================================================
 */

/* === Macros for generating macros... ================================	*/

.macro	rd_003f	num		/* ROM bank 0 */
.macro	rd_\num
	move.l	MemoryBaseRom0@END.w(%a5),%a2
	move.b	%d1,rd\@+3-Gbemu(%a4)
rd\@:	lea	dontOptLo+(0x\num << 8)(%a2),%a3
	jmp	(%a6)
	.endm
	.endm

.macro	rd_407f	num		/* ROM bank 1 */
.macro	rd_\num
	move.l	MemoryBaseRom1@END.w(%a5),%a2
	move.b	%d1,rd\@+3-Gbemu(%a4)
rd\@:	lea	dontOptLo+(0x\num << 8)(%a2),%a3
	jmp	(%a6)
	.endm
	.endm

.macro	wr_001f	num		/* RAM bank enable (MCB1, MCB2) */
.macro	wr_\num
	jmp	indexToAddress(gbemuJmpRamEnable)
	.endm
	.endm

.macro	wr_202f	num		/* ROM bank select */
.macro	wr_\num
	jmp	indexToAddress(gbemuJmpRomSelectL)
	.endm
	.endm

.macro	wr_303f	num		/* ROM bank select */
.macro	wr_\num
	jmp	indexToAddress(gbemuJmpRomSelectH)
	.endm
	.endm

.macro	wr_405f	num		/* RAM bank select */
.macro	wr_\num
	jmp	indexToAddress(gbemuJmpRamSelect)
	.endm
	.endm

.macro	wr_607f	num		/* ROM/RAM mode select (MCB1) */
.macro	wr_\num
	jmp	indexToAddress(gbemuJmpModeSelect)
	.endm
	.endm

/* --------------------------------------------------------------------	*/

.macro	rd_809f	num		/* RAM video read */
.macro	rd_\num
	move.l	MemoryBaseRamVid@END.w(%a5),%a2
	move.b	%d1,rd\@+3-Gbemu(%a4)
rd\@:	lea	dontOptLo+(0x\num << 8)-0x10000(%a2),%a3
	jmp	(%a6)
	.endm
	.endm

.macro	wr_8087	num		/* RAM tile tab 0x8000..0x87ff write */
.macro	wr_\num
	move.l	MemoryBaseRamVid@END.w(%a5),%a2
	and.w	#0xff,%d1
	or.w	#0x\num << 8,%d1
	jmp	indexToAddress(gbemuJmpRamWr8087)
	.endm
	.endm

.macro	wr_888f	num		/* RAM tile tab 0x8800..0x8fff write */
.macro	wr_\num
	move.l	MemoryBaseRamVid@END.w(%a5),%a2
	and.w	#0xff,%d1
	or.w	#0x\num << 8,%d1
	jmp	indexToAddress(gbemuJmpRamWr888f)
	.endm
	.endm

.macro	wr_9097	num		/* RAM tile tab 0x9000..0x97ff write */
.macro	wr_\num
	move.l	MemoryBaseRamVid@END.w(%a5),%a2
	and.w	#0xff,%d1
	or.w	#0x\num << 8,%d1
	jmp	indexToAddress(gbemuJmpRamWr9097)
	.endm
	.endm

.macro	wr_989b	num		/* RAM screen 0x9800..0x9bff write */
.macro	wr_\num
	move.l	MemoryBaseRamVid@END.w(%a5),%a2
	and.w	#0xff,%d1
	or.w	#0x\num << 8,%d1
	jmp	indexToAddress(gbemuJmpRamWr989b)
	.endm
	.endm

.macro	wr_9c9f	num		/* RAM screen 0x9c00..0x9fff write */
.macro	wr_\num
	move.l	MemoryBaseRamVid@END.w(%a5),%a2
	and.w	#0xff,%d1
	or.w	#0x\num << 8,%d1
	jmp	indexToAddress(gbemuJmpRamWr9c9f)
	.endm
	.endm

/* --------------------------------------------------------------------	*/

.macro	rd_a0bf	num		/* RAM external read */
.macro	rd_\num
	and.w	#0xff,%d1
	or.w	#0x\num << 8,%d1
	jmp	indexToAddress(gbemuJmpRamRdExt)
	.endm
	.endm

.macro	wr_a0bf	num		/* RAM external write */
.macro	wr_\num
	and.w	#0xff,%d1
	or.w	#0x\num << 8,%d1
	jmp	indexToAddress(gbemuJmpRamWrExt)
	.endm
	.endm

.macro	rd_c0df	num		/* RAM internal read */
.macro	rd_\num
	move.l	MemoryBaseRamInt@END.w(%a5),%a2
	move.b	%d1,rd\@+3-Gbemu(%a4)
rd\@:	lea	dontOptLo+(0x\num << 8)-0x10000(%a2),%a3
	jmp	(%a6)
	.endm
	.endm

.macro	wr_c0df	num		/* RAM internal write */
.macro	wr_\num
	move.l	MemoryBaseRamInt@END.w(%a5),%a2
	move.b	%d1,wr\@+3-Gbemu(%a4)
wr\@:	move.b	%d0,dontOptLo+(0x\num << 8)-0x10000(%a2)
	jmp	(%a6)
	.endm
	.endm

.macro	rd_e0fd	num		/* RAM internal read (mirror) */
.macro	rd_\num
	move.l	MemoryBaseRamInt@END.w(%a5),%a2
	move.b	%d1,rd\@+3-Gbemu(%a4)
rd\@:	lea	dontOptLo+((0x\num-0x20) << 8)-0x10000(%a2),%a3
	jmp	(%a6)
	.endm
	.endm

.macro	wr_e0fd	num		/* RAM internal write (mirror) */
.macro	wr_\num
	move.l	MemoryBaseRamInt@END.w(%a5),%a2
	move.b	%d1,wr\@+3-Gbemu(%a4)
wr\@:	move.b	%d0,dontOptLo+((0x\num-0x20) << 8)-0x10000(%a2)
	jmp	(%a6)
	.endm
	.endm

/* === And now the actual macros... ===================================	*/

	.irpc	i,01
	.irpc	j,0123456789abcdef
	rd_003f	\i\j		/* 1st half of ROM bank 0 */
	wr_001f	\i\j		/* RAM bank enable */
	.endr
	.endr

	.irpc	j,0123456789abcdef
	rd_003f	2\j		/* 3rd quarter of ROM bank 0 */
	wr_202f	2\j		/* ROM bank select */
	.endr

	.irpc	j,0123456789abcdef
	rd_003f	3\j		/* 4th quarter of ROM bank 0 */
	wr_303f	3\j		/* ROM bank select */
	.endr

	.irpc	i,45
	.irpc	j,0123456789abcdef
	rd_407f	\i\j		/* 1st half of ROM bank 1 */
	wr_405f	\i\j		/* RAM bank select */
	.endr
	.endr

	.irpc	i,67
	.irpc	j,0123456789abcdef
	rd_407f	\i\j		/* 2nd half of ROM bank 1 */
	wr_607f	\i\j		/* ROM/RAM mode select */
	.endr
	.endr

	.irpc	j,01234567
	rd_809f	8\j		/* RAM tile tab 0x8000..0x87ff */
	wr_8087	8\j
	.endr

	.irpc	j,89abcdef
	rd_809f	8\j		/* RAM tile tab 0x8800..0x8fff */
	wr_888f	8\j
	.endr

	.irpc	j,01234567
	rd_809f	9\j		/* RAM tile tab 0x9000..0x97ff */
	wr_9097	9\j
	.endr

	.irpc	j,89ab
	rd_809f	9\j		/* RAM screen 0x9800..0x9bff */
	wr_989b	9\j
	.endr

	.irpc	j,cdef
	rd_809f	9\j		/* RAM screen 0x9c00..0x9fff */
	wr_9c9f	9\j
	.endr

	.irpc	i,ab
	.irpc	j,0123456789abcdef
	rd_a0bf	\i\j		/* RAM external */
	wr_a0bf	\i\j
	.endr
	.endr

	.irpc	i,cd
	.irpc	j,0123456789abcdef
	rd_c0df	\i\j		/* RAM internal */
	wr_c0df	\i\j
	.endr
	.endr

	.irpc	j,0123456789abcdef
	rd_e0fd	e\j		/* RAM internal (mirror) */
	wr_e0fd	e\j
	.endr

	.irpc	j,0123456789abcd
	rd_e0fd	f\j		/* RAM internal (mirror) */
	wr_e0fd	f\j
	.endr

.macro	rd_fe			/* RAM OAM read */
	move.l	MemoryBaseRamOam@END.w(%a5),%a2
	move.b	%d1,rd\@+3-Gbemu(%a4)
rd\@:	lea	dontOptLo+(0xfe << 8)-0x10000(%a2),%a3
	jmp	(%a6)
	.endm

.macro	wr_fe			/* RAM OAM write */
	move.l	MemoryBaseRamOam@END.w(%a5),%a2
	and.w	#0xff,%d1
	or.w	#0xfe << 8,%d1
	jmp	indexToAddress(gbemuJmpRamWrOam)
	.endm

.macro	rd_ff			/* input */
	move.b	%d1,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetIn+dontOptHi(%a4)
	.endm

.macro	wr_ff			/* output */
	move.b	%d1,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetOut+dontOptHi(%a4)
	.endm

/* === The end ========================================================	*/
