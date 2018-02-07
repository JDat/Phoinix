/************************************************************************
 *  gbemu_xt.inc
 *                      Emulation of the Gameboy CPU
 *                      (Extensions)
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
 *  Revision 1.1.1.1  2001/12/16 13:38:52  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-22  Bodo Wenzel   Some optimization
 *  2001-09-16  Bodo Wenzel   Saving states now possible
 *  2001-07-21  Bodo Wenzel   Changed syntax to CPP + GAS
 *  2001-02-04  Bodo Wenzel   New stack management
 *  2001-01-30  Bodo Wenzel   Support of more cartridges
 *  2000-12-02  Bodo Wenzel   HALT with DI support
 *  2000-11-16  Bodo Wenzel   New screen rendering and
 *                            better joypad emulation
 *  2000-10-22  Bodo Wenzel   Interrupts and timer
 *  2000-09-29  Bodo Wenzel   PalmBoy-Scroller as GB code
 *  2000-09-27  Bodo Wenzel   DAA implemented
 *  2000-09-09  Bodo Wenzel   Now realy full screen emulation
 *  2000-06-06  Bodo Wenzel   New memory management
 *  2000-05-06  Bodo Wenzel   External RAM pageable
 *  2000-04-29  Bodo Wenzel   Creation
 ************************************************************************
 */

.macro	xt_void	num
.macro	xt_\num			/* unused extension */
	.endm
	.endm

/* === Space for a small hidden game ;-) ==============================	*/

/* The space herein is 0x30 bytes each chunk... */

/* this is not yet filled in... and must be made by a tool */

	.irpc	i,89abcdef01234
	.irpc	j,0123456789abcdef
	xt_void	\i\j
	.endr
	.endr

	.irpc	j,012345678
	xt_void	5\j
	.endr

/* === Miscellaneous support ==========================================	*/

.macro	xt_59			/* stop opcode */
	jmp	0.l
	.endm

/* === Interrupt support ==============================================	*/

/* the following routine has to be here because of the used macros */
.macro	xt_5a			/* check for pending interrupts */
	move.b	EmulationIo.if_@END.w(%a5),%d0
	and.b	EmulationIntIm@END.w(%a5),%d0
	jne	goon\@
	rts
goon\@:	moveq	#0x0060,%d1
	moveq	#4,%d2
loop\@:	btst	%d2,%d0
	jne	xt_ci1
	subq.w	#0x0008,%d1
	dbra	%d2,loop\@
	rts
	.endm

.macro	xt_5b			/* call interrupt (part 1) */
xt_ci1:	bclr	%d2,EmulationIo.if_@END.w(%a5)
	addq.w	#4,%sp
	do_push	%a0
	jra	xt_ci2
	.endm

.macro	xt_5c			/* call interrupt (part 2) */
xt_ci2:
	moveq	#0,%d0
	move.b	%d0,EmulationIntIme@END.w(%a5)
	move.b	%d0,EmulationIntIm@END.w(%a5)
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	add.w	%d1,%a0
	next_pc
	.endm

/* === Memory vectors =================================================	*/

.macro	xt_5d			/* RAM enable */
	jmp	0.l
	.endm

.macro	xt_5e			/* ROM bank (low) selection */
	jmp	0.l
	.endm

.macro	xt_5f			/* ROM bank (high) selection */
	jmp	0.l
	.endm

.macro	xt_60			/* RAM bank selection */
	jmp	0.l
	.endm

.macro	xt_61			/* ROM/RAM mode selection */
	jmp	0.l
	.endm

.macro	xt_62			/* write to tile tab 0x8000..0x87ff */
	jmp	0.l
	.endm

.macro	xt_63			/* write to tile tab 0x8800..0x8fff */
	jmp	0.l
	.endm

.macro	xt_64			/* write to tile tab 0x9000..0x97ff */
	jmp	0.l
	.endm

.macro	xt_65			/* write to screen 0x9800..0x9bff */
	jmp	0.l
	.endm

.macro	xt_66			/* write to screen 0x9c00..0x9fff */
	jmp	0.l
	.endm

.macro	xt_67			/* read from external RAM 0xa000..0xbfff */
	jmp	0.l
	.long	0
	.endm

.macro	xt_68			/* write to external RAM 0xa000..0xbfff */
	jmp	0.l
	.long	0
	.endm

.macro	xt_69			/* write to OAM 0xfe00..0xfeff */
	jmp	0.l
	.endm

/* === LCD vectors ====================================================	*/

.macro	xt_6a			/* support for LCD line */
	jmp	0.l
	.endm

.macro	xt_6b			/* write to DMA */
	jmp	0.l
	.endm

/* === Memory support =================================================	*/

/* the following routine has to be here because it is used in macros */
.macro	xt_6c			/* SP push correction */
	movem.l	%d0-%d2/%a2-%a3,-(%sp)
	move.b	EmulationCpuState.spOffset+1@END.w(%a5),%d1
	subq.b	#1,EmulationCpuState.spOffset+0@END.w(%a5)
	mem_rd	EmulationCpuState.spOffset+0@END.w(%a5)
	move.l	%a2,EmulationCpuState.spBase@END.w(%a5)
	lea	2(%a3),%a1
	movem.l	(%sp)+,%d0-%d2/%a2-%a3
	rts
	.endm

/* the following routine has to be here because it is used in macros */
.macro	xt_6d			/* SP pop correction */
	movem.l	%d0-%d2/%a2-%a3,-(%sp)
	move.b	EmulationCpuState.spOffset+1@END.w(%a5),%d1
	addq.b	#1,EmulationCpuState.spOffset+0@END.w(%a5)
	mem_rd	EmulationCpuState.spOffset+0@END.w(%a5)
	move.l	%a2,EmulationCpuState.spBase@END.w(%a5)
	move.l	%a3,%a1
	movem.l	(%sp)+,%d0-%d2/%a2-%a3
	rts
	.endm

/* the following routine has to be here because it is used in macros */
.macro	xt_6e			/* SP ROM bank correction */
	move.w	EmulationCpuState.spOffset@END.w(%a5),%d1
	jmi	goon\@
	cmp.w	#0x4000,%d1
	jcs	goon\@
	move.l	MemoryBaseRom1@END.w(%a5),%a1
	move.l	%a1,EmulationCpuState.spBase@END.w(%a5)
	add.w	%d1,%a1
goon\@:	jmp	(%a6)
	.endm

/* the following routine has to be here because it is used in macros */
.macro	xt_6f			/* SP RAM bank correction */
	move.w	EmulationCpuState.spOffset@END.w(%a5),%d1
	cmp.w	#0xc000,%d1
	jcc	goon\@
	cmp.w	#0xa000,%d1
	jcs	goon\@
	move.l	MemoryBaseRamExt@END.w(%a5),%a1
	move.l	%a1,EmulationCpuState.spBase@END.w(%a5)
	add.w	%d1,%a1
goon\@:	jmp	(%a6)
	.endm

/* === GB code for startup ========================================================	*/

.macro	xt_70	/* check for restart, set all I/O registers (part 1) */
				/*	reset:		;at 0x0000	*/
	.byte	0xa7		/*		and	a		*/
	.byte	0xca,0x00,0x0c	/*		jp	z,continue	*/
				/*					*/
	.byte	0x3e,0xff	/*		ld	a,0xff		*/
	.byte	0xe0,0x00	/*		ldh	(P1),a		*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x05	/*		ldh	(TIMA),a	*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x06	/*		ldh	(TMA),a		*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x07	/*		ldh	(TAC),a		*/
	.byte	0x3e,0x80	/*		ld	a,0x80		*/
	.byte	0xe0,0x10	/*		ldh	(NR10),a	*/
	.byte	0x3e,0xbf	/*		ld	a,0xbf		*/
	.byte	0xe0,0x11	/*		ldh	(NR11),a	*/
	.byte	0x3e,0xf3	/*		ld	a,0xf3		*/
	.byte	0xe0,0x12	/*		ldh	(NR12),a	*/
	.byte	0x3e,0xbf	/*		ld	a,0xbf		*/
	.byte	0xe0,0x14	/*		ldh	(NR14),a	*/
	.byte	0x3e,0x3f	/*		ld	a,0x3f		*/
	.byte	0xe0,0x16	/*		ldh	(NR21),a	*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x17	/*		ldh	(NR22),a	*/
				/*					*/
	.byte	0xc3,0x00,0x01	/*		jp	reset1		*/
	.endm

.macro	xt_71	/* set all I/O registers (part 2) */
				/*	reset1:				*/
	.byte	0x3e,0xbf	/*		ld	a,0xbf		*/
	.byte	0xe0,0x19	/*		ldh	(NR24),a	*/
	.byte	0x3e,0x7f	/*		ld	a,0x7f		*/
	.byte	0xe0,0x1a	/*		ldh	(NR30),a	*/
	.byte	0x3e,0xff	/*		ld	a,0xff		*/
	.byte	0xe0,0x1b	/*		ldh	(NR31),a	*/
	.byte	0x3e,0x9f	/*		ld	a,0x9f		*/
	.byte	0xe0,0x1c	/*		ldh	(NR32),a	*/
	.byte	0x3e,0xbf	/*		ld	a,0xbf		*/
	.byte	0xe0,0x1e	/*		ldh	(NR33),a	*/
	.byte	0x3e,0xff	/*		ld	a,0xff		*/
	.byte	0xe0,0x20	/*		ldh	(NR41),a	*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x21	/*		ldh	(NR42),a	*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x22	/*		ldh	(NR43),a	*/
	.byte	0x3e,0xbf	/*		ld	a,0xbf		*/
	.byte	0xe0,0x23	/*		ldh	(NR44),a	*/
	.byte	0x3e,0x77	/*		ld	a,0x77		*/
	.byte	0xe0,0x24	/*		ldh	(NR50),a	*/
	.byte	0x3e,0xf3	/*		ld	a,0xf3		*/
	.byte	0xe0,0x25	/*		ldh	(NR51),a	*/
				/*					*/
	.byte	0xc3,0x00,0x02	/*		jp	reset2		*/
	.endm

.macro	xt_72	/* set all I/O registers (part 3) */
				/*	reset2:				*/
	.byte	0x3e,0xf1	/*		ld	a,0xf1		*/
	.byte	0xe0,0x26	/*		ldh	(NR52),a	*/
				/*					*/
	.byte	0xaf		/*		xor	a		*/
	.byte	0xe0,0x40	/*		ldh	(LCDC),a	*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x41	/*		ldh	(STAT),a	*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x42	/*		ldh	(SCY),a		*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x43	/*		ldh	(SCX),a		*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x45	/*		ldh	(LYC),a		*/
	.byte	0x3e,0xfc	/*		ld	a,0xfc		*/
	.byte	0xe0,0x47	/*		ldh	(BGP),a		*/
	.byte	0x3e,0xff	/*		ld	a,0xff		*/
	.byte	0xe0,0x48	/*		ldh	(OBP0),a	*/
	.byte	0x3e,0xff	/*		ld	a,0xff		*/
	.byte	0xe0,0x49	/*		ldh	(OBP1),a	*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x4a	/*		ldh	(WY),a		*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0x4b	/*		ldh	(WX),a		*/
				/*					*/
	.byte	0xc3,0x00,0x03	/*		jp	reset3		*/
	.endm

.macro	xt_73	/* set all I/O registers (part 4), scroller (part 1) */
				/*	reset3:				*/
	.byte	0x3e,0x91	/*		ld	a,0x91		*/
	.byte	0xe0,0x40	/*		ldh	(LCDC),a	*/
	.byte	0x3e,0x00	/*		ld	a,0x00		*/
	.byte	0xe0,0xff	/*		ldh	(IE),a		*/
				/*					*/
	.byte	0x21,0x00,0x98	/*		ld	hl,0x9800	*/
				/*	resetloop1:			*/
	.byte	0x7e		/*		ld	a,(hl)		*/
	.byte	0x2f		/*		cpl			*/
	.byte	0x22		/*		ld	(hli),a		*/
	.byte	0x7c		/*		ld	a,h		*/
	.byte	0xfe,0xa0	/*		cp	0xa0		*/
	.byte	0x20,0xf8	/*		jr	nz,resetloop1	*/
				/*					*/
	.byte	0x31,0xfe,0xff	/*		ld	sp,0xfffe	*/
				/*					*/
	.byte	0x3e,0x50	/*		ld	a,144/2+8	*/
	.byte	0xa3		/*		and	e		*/
	.byte	0x3c		/*		inc	a		*/
	.byte	0xe0,0x42	/*		ld	(SCY),a		*/
				/*					*/
	.byte	0x21,0x00,0x81	/*		ld	hl,0x8100	*/
	.byte	0x11,0x00,0x07	/*		ld	de,tiletab+0x000*/
	.byte	0xcd,0x00,0x06	/*		call	copydehl	*/
	.byte	0x11,0x00,0x08	/*		ld	de,tiletab+0x100*/
	.byte	0xcd,0x00,0x06	/*		call	copydehl	*/
				/*					*/
	.byte	0xc3,0x00,0x04	/*		jp	reset4		*/
	.endm

.macro	xt_74	/* scroller (part 2) */
				/*	reset4:				*/
	.byte	0x11,0x00,0x09	/*		ld	de,tiletab+0x200*/
	.byte	0xcd,0x00,0x06	/*		call	copydehl	*/
	.byte	0x11,0x00,0x0a	/*		ld	de,tiletab+0x300*/
	.byte	0xcd,0x00,0x06	/*		call	copydehl	*/
	.byte	0x11,0x00,0x0b	/*		ld	de,tiletab+0x400*/
	.byte	0xcd,0x00,0x06	/*		call	copydehl	*/
				/*					*/
	.byte	0x3e,0x10	/*		ld	a,0x10		*/
	.byte	0x21,0x04,0x99	/*		ld	hl,0x9800+8*32+10*/
	.byte	0x06,0x0c	/*		ld	b,12		*/
				/*	resetloop2:			*/
	.byte	0x22		/*		ld	(hli),a		*/
	.byte	0x3c		/*		inc	a		*/
	.byte	0x05		/*		dec	b		*/
	.byte	0x20,0xfb	/*		jr	nz,resetloop2	*/
	.byte	0x21,0x24,0x99	/*		ld	hl,0x9800+9*32+10*/
	.byte	0x06,0x0c	/*		ld	b,12		*/
				/*	resetloop3:			*/
	.byte	0x22		/*		ld	(hli),a		*/
	.byte	0x3c		/*		inc	a		*/
	.byte	0x05		/*		dec	b		*/
	.byte	0x20,0xfb	/*		jr	nz,resetloop3	*/
	.byte	0x21,0x10,0x99	/*		ld	hl,0x9800+8*32+22*/
	.byte	0x77		/*		ld	(hl),a		*/
				/*					*/
	.byte	0xc3,0x00,0x05	/*		jp	reset5		*/
	.endm

.macro	xt_75	/* scroller (part 3), set registers */
				/*	reset5:				*/
	.byte	0x21,0x42,0xff	/*		ld	hl,SCY		*/
				/*	resetwait1:			*/
	.byte	0xf0,0x44	/*		ldh	a,(LY)		*/
	.byte	0xfe,0x90	/*		cp	144		*/
	.byte	0x28,0xfa	/*		jr	z,resetwait1	*/
				/*	resetwait2:			*/
	.byte	0xf0,0x44	/*		ldh	a,(LY)		*/
	.byte	0xfe,0x90	/*		cp	144		*/
	.byte	0x20,0xfa	/*		jr	nz,resetwait2	*/
	.byte	0x35		/*		dec	(hl)		*/
	.byte	0x20,0xf1	/*		jr	nz,resetwait1	*/
				/*					*/
	.byte	0x01,0x13,0x00	/*		ld	bc,0x0013	*/
	.byte	0x11,0xd8,0x00	/*		ld	de,0x00d8	*/
	.byte	0x21,0x4d,0x01	/*		ld	hl,0x014d	*/
	.byte	0x3e,0x03	/*		ld	a,0x03		*/
	.byte	0xcb,0x3f	/*		srl	a	;af=0x01b0*/
				/*					*/
	.byte	0xdb		/*		(illegal for stop)	*/
	.endm

.macro	xt_76	/* copy subroutine for tile table */
				/*	copydehl:	;at 0x0600	*/
	.byte	0x06,0x28	/*		ld	b,40		*/
				/*	copydehlloop:			*/
	.byte	0x1a		/*		ld	a,(de)		*/
	.byte	0x13		/*		inc	de		*/
	.byte	0x22		/*		ldi	(hl),a		*/
	.byte	0x22		/*		ldi	(hl),a		*/
	.byte	0x05		/*		dec	b		*/
	.byte	0x20,0xf9	/*		jr	nz,copydehlloop	*/
				/*					*/
	.byte	0xc9		/*		ret			*/
	.endm

.macro	xt_77
				/*	tiletab:			*/
	.byte	0x1f,0x1f,0x18,0x18,0x30,0x30,0x3f,0x3f
	.byte	0xc0,0xe0,0x70,0x30,0x30,0x70,0xe0,0x80
	.byte	0x60,0x60,0x60,0x60,0xc0,0xc0,0xc0,0xff
	.byte	0x60,0x60,0x60,0x60,0xc1,0xc1,0xc3,0xc3
	.byte	0x0e,0x3f,0x71,0xc0,0x80,0x80,0x00,0x00
	.endm

.macro	xt_78
	.byte	0x00,0x80,0xc0,0xc0,0x60,0x60,0x60,0x60
	.byte	0x60,0x60,0x60,0x60,0xc1,0xc1,0xc1,0xc1
	.byte	0xc0,0xc0,0xe0,0xe0,0xb0,0xb0,0x98,0x98
	.byte	0x60,0x60,0x60,0x60,0xc1,0xc1,0xc1,0xc1
	.byte	0xc3,0xc1,0xc1,0xc0,0x80,0x80,0x80,0x80
	.endm

.macro	xt_79
	.byte	0x80,0x80,0xc0,0xc1,0xe3,0x67,0x7e,0x3c
	.byte	0x38,0x70,0xe1,0xc1,0x81,0x01,0x00,0x00
	.byte	0x60,0x60,0x60,0x60,0xc0,0xc0,0xc0,0xc0
	.byte	0x01,0x01,0x01,0x01,0x03,0x03,0x03,0x03
	.byte	0xff,0x81,0x81,0x81,0x03,0x03,0x03,0x03
	.endm

.macro	xt_7a
	.byte	0x83,0x83,0x83,0x83,0x01,0x01,0x00,0x00
	.byte	0x00,0x00,0x00,0x00,0x81,0xc7,0xfe,0x38
	.byte	0x61,0x61,0xc1,0xc1,0x83,0x03,0x03,0x03
	.byte	0x83,0x83,0x83,0x83,0x06,0x06,0x06,0x06
	.byte	0x0d,0x0d,0x07,0x07,0x03,0x03,0x03,0x03
	.endm

.macro	xt_7b
	.byte	0x83,0x83,0x83,0x83,0x06,0x06,0x06,0x06
	.byte	0x00,0x00,0x00,0x01,0x03,0x07,0x0e,0x1c
	.byte	0x38,0x78,0xfc,0xcc,0x8e,0x06,0x07,0x03
	.byte	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80
	.byte	0x7c,0x82,0x7d,0x11,0x39,0x11,0x82,0x7c
	.endm

.macro	xt_7c	/* set relevant I/O registers (part 1) */
				/*	continue:	;at 0c00	*/
	.byte	0x78		/*		ld	a,b		*/
	.byte	0xe0,0x00	/*		ldh	(P1),a		*/
	.byte	0xf0,0x07	/*		ldh	a,(TAC)		*/
	.byte	0xe0,0x07	/*		ldh	(TAC),a		*/
	.byte	0xaf		/*		xor	a		*/
	.byte	0xe0,0x40	/*		ldh	(LCDC),a	*/
	.byte	0x79		/*		ld	a,c		*/
	.byte	0xe0,0x41	/*		ldh	(STAT),a	*/
	.byte	0xf0,0x42	/*		ldh	a,(SCY)		*/
	.byte	0xe0,0x42	/*		ldh	(SCY),a		*/
	.byte	0xf0,0x43	/*		ldh	a,(SCX)		*/
	.byte	0xe0,0x43	/*		ldh	(SCX),a		*/
	.byte	0xf0,0x45	/*		ldh	a,(LYC)		*/
	.byte	0xe0,0x45	/*		ldh	(LYC),a		*/
	.byte	0xf0,0x47	/*		ldh	a,(BGP)		*/
	.byte	0x2f		/*		cpl			*/
	.byte	0xe0,0x47	/*		ldh	(BGP),a		*/
	.byte	0xf0,0x48	/*		ldh	a,(OBP0)	*/
	.byte	0x2f		/*		cpl			*/
	.byte	0xe0,0x48	/*		ldh	(OBP0),a	*/
	.byte	0xf0,0x49	/*		ldh	a,(OBP1)	*/
	.byte	0x2f		/*		cpl			*/
	.byte	0xe0,0x49	/*		ldh	(OBP1),a	*/
	.byte	0xf0,0x4a	/*		ldh	a,(WY)		*/
	.byte	0xe0,0x4a	/*		ldh	(WY),a		*/
				/*					*/
	.byte	0xc3,0x00,0x0d	/*		jp	continue2	*/
	.endm

.macro	xt_7d	/* set relevant I/O registers (part 2), set up tile map */
				/*	continue2:			*/
	.byte	0xf0,0x4b	/*		ldh	a,(WX)		*/
	.byte	0xe0,0x4b	/*		ldh	(WX),a		*/
	.byte	0x7a		/*		ld	a,d		*/
	.byte	0xe0,0x40	/*		ldh	(LCDC),a	*/
	.byte	0xf0,0xff	/*		ldh	a,(IE)		*/
	.byte	0xe0,0xff	/*		ldh	(IE),a		*/
				/*					*/
	.byte	0x21,0x00,0x98	/*		ld	hl,0x9800	*/
				/*	continueloop:			*/
	.byte	0x7e		/*		ld	a,(hl)		*/
	.byte	0x2f		/*		cpl			*/
	.byte	0x22		/*		ld	(hli),a		*/
	.byte	0x7c		/*		ld	a,h		*/
	.byte	0xfe,0xa0	/*		cp	0xa0		*/
	.byte	0x20,0xf8	/*		jr	nz,continueloop	*/
				/*					*/
	.byte	0xdb		/*		(illegal for stop)	*/
	.endm

/* === GB code (start with PC and SP set up) ==========================	*/

.macro	xt_7e	/* restart gameboy */
				/*	restart:			*/
	.byte	0x31,0xfe,0xff	/*		ld	sp,xxxx		*/
	.byte	0xc3,0x00,0x01	/*		jp	yyyy		*/
	.endm

/* === Last chunk has to be empty! ====================================	*/

.macro	xt_7f
	/* This macro will use the last bytes of the resource. But as
	 * resources have to be a bit smaller than 0x10000 bytes, it must
	 * be left empty!
         */
	.endm

/* === The end ========================================================	*/
