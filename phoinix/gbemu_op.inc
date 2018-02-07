/************************************************************************
 *  gbemu_op.inc
 *                      Emulation of the Gameboy CPU
 *                      ("Normal" opcodes)
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
 *  $Log: gbemu_op.inc,v $
 *  Revision 1.8  2007/07/01 10:12:51  bodowenzel
 *  Cleanup
 *
 *  Revision 1.7  2007/01/03 10:24:34  bodowenzel
 *  Correct flags for rotates and sums of SP and immediate
 *
 *  Revision 1.6  2006/10/23 15:39:08  bodowenzel
 *  Opcode pointer with illegal STOP corrected
 *
 *  Revision 1.5  2005/05/03 08:39:11  bodowenzel
 *  Moved coldReset, ime, im and error into EmulationCpuState
 *
 *  Revision 1.4  2005/01/30 19:34:07  bodowenzel
 *  Added labels for all combo parts
 *
 *  Revision 1.3  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:39:50  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-22  Bodo Wenzel   Some optimization
 *  2001-09-17  Bodo Wenzel   Saving states now possible
 *  2001-07-21  Bodo Wenzel   Changed syntax to CPP + GAS
 *  2001-02-05  Bodo Wenzel   New stack management
 *  2001-02-01  Bodo Wenzel   Correction at debug modes
 *  2000-12-02  Bodo Wenzel   HALT with DI support
 *  2000-10-03  Bodo Wenzel   Interrupts
 *  2000-09-27  Bodo Wenzel   DAA implemented
 *  2000-08-23  Bodo Wenzel   Some improvements
 *  2000-06-18  Bodo Wenzel   New memory management
 *  2000-02-23  Bodo Wenzel   Creation
 ************************************************************************
 */

.macro	op_00			/* nop */
op_00:
	next_pc
	.endm

.macro	op_01			/* ld bc,## */
op_01:
	move.b	(%a0)+,%d0
	move.b	(%a0)+,%d5
	swap	%d5
	move.b	%d0,%d5
	next_pc
	.endm

.macro	op_02			/* ld (bc),a */
op_02:
	move.b	%d7,%d0
	move.b	%d5,%d1
	move.l	%d5,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_03			/* inc bc */
op_03:
	ext.w	%d5
	addq.l	#1,%d5
	next_pc
	.endm

.macro	op_04			/* inc b */
op_04:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	swap	%d5
	addq.b	#1,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	swap	%d5
	next_pc
	.endm

.macro	op_05			/* dec b */
op_05:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	swap	%d5
	subq.b	#1,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	swap	%d5
	next_pc
	.endm

.macro	op_06			/* ld b,# */
op_06:
	swap	%d5
	move.b	(%a0)+,%d5
	swap	%d5
	next_pc
	.endm

.macro	op_07			/* rlca */
op_07:
	swap	%d6
	move.b	%d7,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d7
	move	%sr,%d6
	and.b	#~m68kMaskZ,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_08			/* ld (##),sp */
op_08:
	move.b	(%a0)+,%d1
	move.b	(%a0)+,%d2
	jpl	quit\@
	mem_rd	%d2		/* used as "get mem addr" */
	move.b	EmulationCpuState.spOffset+1@END.w(%a5),(%a3)+
	move.b	EmulationCpuState.spOffset+0@END.w(%a5),(%a3)
quit\@:	next_pc
	.endm

.macro	op_09			/* add hl,bc */
op_09:
	moveq	#m68kFlagC,%d0
	bset	%d0,%d6
	add.b	%d5,%d3
	swap	%d3
	swap	%d5
	addx.b	%d5,%d3
	jcs	c\@
	bchg	%d0,%d6
c\@:	swap	%d3
	swap	%d5
	next_pc
	.endm

.macro	op_0a			/* ld a,(bc) */
op_0a:
	move.b	%d5,%d1
	move.l	%d5,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_0b			/* dec bc */
op_0b:
	ext.w	%d5
	subq.l	#1,%d5
	next_pc
	.endm

.macro	op_0c			/* inc c */
op_0c:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	addq.b	#1,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_0d			/* dec c */
op_0d:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	subq.b	#1,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_0e			/* ld c,# */
op_0e:
	move.b	(%a0)+,%d5
	next_pc
	.endm

.macro	op_0f			/* rrca */
op_0f:
	swap	%d6
	move.b	%d7,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d7
	move	%sr,%d6
	and.b	#~m68kMaskZ,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_10			/* stop */
op_10:
	move.b	(%a0)+,%d0
	jeq	goon\@
	subq.w	#1,%a0
	return	gbemuRetOpcodeStop
goon\@:	jsr	indexToAddress(gbemuJmpOpcodeStop)
	next_pc
	.endm

.macro	op_11			/* ld de,## */
op_11:
	move.b	(%a0)+,%d0
	move.b	(%a0)+,%d4
	swap	%d4
	move.b	%d0,%d4
	next_pc
	.endm

.macro	op_12			/* ld (de),a */
op_12:
	move.b	%d7,%d0
	move.b	%d4,%d1
	move.l	%d4,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_13			/* inc de */
op_13:
	ext.w	%d4
	addq.l	#1,%d4
	next_pc
	.endm

.macro	op_14			/* inc d */
op_14:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	swap	%d4
	addq.b	#1,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	swap	%d4
	next_pc
	.endm

.macro	op_15			/* dec d */
op_15:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	swap	%d4
	subq.b	#1,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	swap	%d4
	next_pc
	.endm

.macro	op_16			/* ld d,# */
op_16:
	swap	%d4
	move.b	(%a0)+,%d4
	swap	%d4
	next_pc
	.endm

.macro	op_17			/* rla */
op_17:
	swap	%d6
	move	%d6,%ccr
	roxl.b	#1,%d7
	move	%sr,%d6
	and.b	#~m68kMaskZ,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_18			/* jr # */
op_18:
	move.b	(%a0)+,%d0
	ext.w	%d0
	add.w	%d0,%a0
	next_pc
	.endm

.macro	op_19			/* add hl,de */
op_19:
	moveq	#m68kFlagC,%d0
	bset	%d0,%d6
	add.b	%d4,%d3
	swap	%d3
	swap	%d4
	addx.b	%d4,%d3
	jcs	c\@
	bchg	%d0,%d6
c\@:	swap	%d3
	swap	%d4
	next_pc
	.endm

.macro	op_1a			/* ld a,(de) */
op_1a:
	move.b	%d4,%d1
	move.l	%d4,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_1b			/* dec de */
op_1b:
	ext.w	%d4
	subq.l	#1,%d4
	next_pc
	.endm

.macro	op_1c			/* inc e */
op_1c:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	addq.b	#1,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_1d			/* dec e */
op_1d:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	subq.b	#1,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_1e			/* ld e,# */
op_1e:
	move.b	(%a0)+,%d4
	next_pc
	.endm

.macro	op_1f			/* rra */
op_1f:
	swap	%d6
	move	%d6,%ccr
	roxr.b	#1,%d7
	move	%sr,%d6
	and.b	#~m68kMaskZ,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_20			/* jr nz,# */
op_20:
	move.b	(%a0)+,%d1
	btst	#m68kFlagZ,%d6
	jne	quit\@
	ext.w	%d1
	add.w	%d1,%a0
quit\@:	next_pc
	.endm

.macro	op_21			/* ld hl,## */
op_21:
	move.b	(%a0)+,%d0
	move.b	(%a0)+,%d3
	swap	%d3
	move.b	%d0,%d3
	next_pc
	.endm

.macro	op_22			/* ldi (hl),a */
op_22:
	move.b	%d7,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	ext.w	%d3
	addq.l	#1,%d3
	mem_wr	%d2
	next_pc
	.endm

.macro	op_23			/* inc hl */
op_23:
	ext.w	%d3
	addq.l	#1,%d3
	next_pc
	.endm

.macro	op_24			/* inc h */
op_24:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	swap	%d3
	addq.b	#1,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	swap	%d3
	next_pc
	.endm

.macro	op_25			/* dec h */
op_25:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	swap	%d3
	subq.b	#1,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	swap	%d3
	next_pc
	.endm

.macro	op_26			/* ld h,# */
op_26:
	swap	%d3
	move.b	(%a0)+,%d3
	swap	%d3
	next_pc
	.endm

.macro	op_27			/* daa */
op_27:
	move.b	%d7,%d0
	move.b	op27a-Gbemu(%a4),%d7
	btst	#m68kFlagC,%d6
	jeq	nc\@
	sub.b	%d7,%d0
	jcc	s\@
a\@:	swap	%d6
	move	#m68kMaskZ,%ccr
	abcd	%d0,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
nc\@:	sub.b	%d7,%d0
	jcc	a\@
s\@:	swap	%d6
	neg.b	%d0
	move	#m68kMaskZ,%ccr
	sbcd	%d0,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
op27a:	.byte	0x00
	.endm

.macro	op_28			/* jr z,# */
op_28:
	move.b	(%a0)+,%d1
	btst	#m68kFlagZ,%d6
	jeq	quit\@
	ext.w	%d1
	add.w	%d1,%a0
quit\@:	next_pc
	.endm

.macro	op_29			/* add hl,hl */
op_29:
	moveq	#m68kFlagC,%d0
	bset	%d0,%d6
	add.b	%d3,%d3
	swap	%d3
	addx.b	%d3,%d3
	jcs	c\@
	bchg	%d0,%d6
c\@:	swap	%d3
	next_pc
	.endm

.macro	op_2a			/* ldi a,(hl) */
op_2a:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	ext.w	%d3
	addq.l	#1,%d3
	mem_rd	%d2
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_2b			/* dec hl */
op_2b:
	ext.w	%d3
	subq.l	#1,%d3
	next_pc
	.endm

.macro	op_2c			/* inc l */
op_2c:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	addq.b	#1,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_2d			/* dec l */
op_2d:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	subq.b	#1,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_2e			/* ld l,# */
op_2e:
	move.b	(%a0)+,%d3
	next_pc
	.endm

.macro	op_2f			/* cpl */
op_2f:
	not.b	%d7
	next_pc
	.endm

.macro	op_30			/* jr nc,# */
op_30:
	move.b	(%a0)+,%d1
	btst	#m68kFlagC,%d6
	jne	quit\@
	ext.w	%d1
	add.w	%d1,%a0
quit\@:	next_pc
	.endm

.macro	op_31			/* ld sp,## */
op_31:
	move.b	(%a0)+,%d1
	move.b	%d1,EmulationCpuState.spOffset+1@END.w(%a5)
	move.b	(%a0)+,%d2
	move.b	%d2,EmulationCpuState.spOffset+0@END.w(%a5)
	mem_rd	%d2
	move.l	%a2,EmulationCpuState.spBase@END.w(%a5)
	move.l	%a3,%a1
	next_pc
	.endm

.macro	op_32			/* ldd (hl),a */
op_32:
	move.b	%d7,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	ext.w	%d3
	subq.l	#1,%d3
	mem_wr	%d2
	next_pc
	.endm

.macro	op_33			/* inc sp */
op_33:
	addq.w	#1,%a1
	addq.b	#1,EmulationCpuState.spOffset+1@END.w(%a5)
	jcc	goon\@
	move.b	EmulationCpuState.spOffset+1@END.w(%a5),%d1
	addq.b	#1,EmulationCpuState.spOffset+0@END.w(%a5)
	mem_rd	EmulationCpuState.spOffset+0@END.w(%a5)
	move.l	%a2,EmulationCpuState.spBase@END.w(%a5)
	move.l	%a3,%a1
goon\@:	next_pc
	.endm

.macro	op_34			/* inc (hl) */
op_34:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	bset	#m68kFlagZ,%d6
	addq.b	#1,%d0
	jeq	z\@
	bchg	#m68kFlagZ,%d6
z\@:	mem_wr	%d2
	next_pc
	.endm

.macro	op_35			/* dec (hl) */
op_35:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	bset	#m68kFlagZ,%d6
	subq.b	#1,%d0
	jeq	z\@
	bchg	#m68kFlagZ,%d6
z\@:	mem_wr	%d2
	next_pc
	.endm

.macro	op_36			/* ld (hl),# */
op_36:
	move.b	(%a0)+,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_37			/* scf */
op_37:
	bset	#m68kFlagC,%d6
	next_pc
	.endm

.macro	op_38			/* jr c,# */
op_38:
	move.b	(%a0)+,%d1
	btst	#m68kFlagC,%d6
	jeq	quit\@
	ext.w	%d1
	add.w	%d1,%a0
quit\@:	next_pc
	.endm

.macro	op_39			/* add hl,sp */
op_39:
	moveq	#m68kFlagC,%d0
	bset	%d0,%d6
	add.b	EmulationCpuState.spOffset+1@END.w(%a5),%d3
	swap	%d3
	move.b	EmulationCpuState.spOffset+0@END.w(%a5),%d1
	addx.b	%d1,%d3
	jcs	c\@
	bchg	%d0,%d6
c\@:	swap	%d3
	next_pc
	.endm

.macro	op_3a			/* ldd a,(hl) */
op_3a:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	ext.w	%d3
	subq.l	#1,%d3
	mem_rd	%d2
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_3b			/* dec sp */
op_3b:
	subq.w	#1,%a1
	subq.b	#1,EmulationCpuState.spOffset+1@END.w(%a5)
	jcc	goon\@
	move.b	EmulationCpuState.spOffset+1@END.w(%a5),%d1
	subq.b	#1,EmulationCpuState.spOffset+0@END.w(%a5)
	mem_rd	EmulationCpuState.spOffset+0@END.w(%a5)
	move.l	%a2,EmulationCpuState.spBase@END.w(%a5)
	move.l	%a3,%a1
goon\@:	next_pc
	.endm

.macro	op_3c			/* inc a */
op_3c:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	move.b	%d7,op27a-Gbemu(%a4)
	addq.b	#1,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_3d			/* dec a */
op_3d:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	move.b	%d7,op27a-Gbemu(%a4)
	subq.b	#1,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_3e			/* ld a,# */
op_3e:
	move.b	(%a0)+,%d7
	next_pc
	.endm

.macro	op_3f			/* ccf */
op_3f:
	bchg	#m68kFlagC,%d6
	next_pc
	.endm

.macro	op_40			/* ld b,b */
op_40:
	next_pc
	.endm

.macro	op_41			/* ld b,c */
op_41:
	move.b	%d5,%d0
	swap	%d5
	move.b	%d0,%d5
	next_pc
	.endm

.macro	op_42			/* ld b,d */
op_42:
	move.l	%d4,%d0
	move.b	%d5,%d0
	move.l	%d0,%d5
	next_pc
	.endm

.macro	op_43			/* ld b,e */
op_43:
	swap	%d5
	move.b	%d4,%d5
	swap	%d5
	next_pc
	.endm

.macro	op_44			/* ld b,h */
op_44:
	move.l	%d3,%d0
	move.b	%d5,%d0
	move.l	%d0,%d5
	next_pc
	.endm

.macro	op_45			/* ld b,l */
op_45:
	swap	%d5
	move.b	%d3,%d5
	swap	%d5
	next_pc
	.endm

.macro	op_46			/* ld b,(hl) */
op_46:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	swap	%d5
	move.b	(%a3),%d5
	swap	%d5
	next_pc
	.endm

.macro	op_47			/* ld b,a */
op_47:
	swap	%d5
	move.b	%d7,%d5
	swap	%d5
	next_pc
	.endm

.macro	op_48			/* ld c,b */
op_48:
	move.l	%d5,%d0
	swap	%d0
	move.b	%d0,%d5
	next_pc
	.endm

.macro	op_49			/* ld c,c */
op_49:
	next_pc
	.endm

.macro	op_4a			/* ld c,d */
op_4a:
	swap	%d4
	move.b	%d4,%d5
	swap	%d4
	next_pc
	.endm

.macro	op_4b			/* ld c,e */
op_4b:
	move.b	%d4,%d5
	next_pc
	.endm

.macro	op_4c			/* ld c,h */
op_4c:
	swap	%d3
	move.b	%d3,%d5
	swap	%d3
	next_pc
	.endm

.macro	op_4d			/* ld c,l */
op_4d:
	move.b	%d3,%d5
	next_pc
	.endm

.macro	op_4e			/* ld c,(hl) */
op_4e:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d5
	next_pc
	.endm

.macro	op_4f			/* ld c,a */
op_4f:
	move.b	%d7,%d5
	next_pc
	.endm

.macro	op_50			/* ld d,b */
op_50:
	move.l	%d5,%d0
	move.b	%d4,%d0
	move.l	%d0,%d4
	next_pc
	.endm

.macro	op_51			/* ld d,c */
op_51:
	swap	%d4
	move.b	%d5,%d4
	swap	%d4
	next_pc
	.endm

.macro	op_52			/* ld d,d */
op_52:
	next_pc
	.endm

.macro	op_53			/* ld d,e */
op_53:
	move.b	%d4,%d0
	swap	%d4
	move.b	%d0,%d4
	next_pc
	.endm

.macro	op_54			/* ld d,h */
op_54:
	move.l	%d3,%d0
	move.b	%d4,%d0
	move.l	%d0,%d4
	next_pc
	.endm

.macro	op_55			/* ld d,l */
op_55:
	swap	%d4
	move.b	%d3,%d4
	swap	%d4
	next_pc
	.endm

.macro	op_56			/* ld d,(hl) */
op_56:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	swap	%d4
	move.b	(%a3),%d4
	swap	%d4
	next_pc
	.endm

.macro	op_57			/* ld d,a */
op_57:
	swap	%d4
	move.b	%d7,%d4
	swap	%d4
	next_pc
	.endm

.macro	op_58			/* ld e,b */
op_58:
	swap	%d5
	move.b	%d5,%d4
	swap	%d5
	next_pc
	.endm

.macro	op_59			/* ld e,c */
op_59:
	move.b	%d5,%d4
	next_pc
	.endm

.macro	op_5a			/* ld e,d */
op_5a:
	move.l	%d4,%d0
	swap	%d0
	move.b	%d0,%d4
	next_pc
	.endm

.macro	op_5b			/* ld e,e */
op_5b:
	next_pc
	.endm

.macro	op_5c			/* ld e,h */
op_5c:
	swap	%d3
	move.b	%d3,%d4
	swap	%d3
	next_pc
	.endm

.macro	op_5d			/* ld e,l */
op_5d:
	move.b	%d3,%d4
	next_pc
	.endm

.macro	op_5e			/* ld e,(hl) */
op_5e:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d4
	next_pc
	.endm

.macro	op_5f			/* ld e,a */
op_5f:
	move.b	%d7,%d4
	next_pc
	.endm

.macro	op_60			/* ld h,b */
op_60:
	move.l	%d5,%d0
	move.b	%d3,%d0
	move.l	%d0,%d3
	next_pc
	.endm

.macro	op_61			/* ld h,c */
op_61:
	swap	%d3
	move.b	%d5,%d3
	swap	%d3
	next_pc
	.endm

.macro	op_62			/* ld h,d */
op_62:
	move.l	%d4,%d0
	move.b	%d3,%d0
	move.l	%d0,%d3
	next_pc
	.endm

.macro	op_63			/* ld h,e */
op_63:
	swap	%d3
	move.b	%d4,%d3
	swap	%d3
	next_pc
	.endm

.macro	op_64			/* ld h,h */
op_64:
	next_pc
	.endm

.macro	op_65			/* ld h,l */
op_65:
	move.b	%d3,%d0
	swap	%d3
	move.b	%d0,%d3
	next_pc
	.endm

.macro	op_66			/* ld h,(hl) */
op_66:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	swap	%d3
	move.b	(%a3),%d3
	swap	%d3
	next_pc
	.endm

.macro	op_67			/* ld h,a */
op_67:
	swap	%d3
	move.b	%d7,%d3
	swap	%d3
	next_pc
	.endm

.macro	op_68			/* ld l,b */
op_68:
	swap	%d5
	move.b	%d5,%d3
	swap	%d5
	next_pc
	.endm

.macro	op_69			/* ld l,c */
op_69:
	move.b	%d5,%d3
	next_pc
	.endm

.macro	op_6a			/* ld l,d */
op_6a:
	swap	%d4
	move.b	%d4,%d3
	swap	%d4
	next_pc
	.endm

.macro	op_6b			/* ld l,e */
op_6b:
	move.b	%d4,%d3
	next_pc
	.endm

.macro	op_6c			/* ld l,h */
op_6c:
	move.l	%d3,%d0
	swap	%d0
	move.b	%d0,%d3
	next_pc
	.endm

.macro	op_6d			/* ld l,l */
op_6d:
	next_pc
	.endm

.macro	op_6e			/* ld l,(hl) */
op_6e:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d3
	next_pc
	.endm

.macro	op_6f			/* ld l,a */
op_6f:
	move.b	%d7,%d3
	next_pc
	.endm

.macro	op_70			/* ld (hl),b */
op_70:
	move.l	%d5,%d0
	swap	%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_71			/* ld (hl),c */
op_71:
	move.b	%d5,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_72			/* ld (hl),d */
op_72:
	move.l	%d4,%d0
	swap	%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_73			/* ld (hl),e */
op_73:
	move.b	%d4,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_74			/* ld (hl),h */
op_74:
	move.l	%d3,%d0
	swap	%d0
	move.b	%d3,%d1
	move.b	%d0,%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_75			/* ld (hl),l */
op_75:
	move.b	%d3,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_76			/* halt, no GB quirk emulation */
op_76:
	move.b	EmulationIo.if_@END.w(%a5),%d0
	not.b	%d0
	move.b	%d0,op76if-Gbemu(%a4)
loop\@:	jsr	indexToAddress(gbemuJmpLcdLine)
	move.b	EmulationIo.if_@END.w(%a5),%d0
	and.b	op76if-Gbemu(%a4),%d0
	jeq	loop\@
	move.b	(%a0)+,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetOp+dontOptHi(%a4)
op76if:	.byte	0x00
	.endm

.macro	op_77			/* ld (hl),a */
op_77:
	move.b	%d7,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_78			/* ld a,b */
op_78:
	swap	%d5
	move.b	%d5,%d7
	swap	%d5
	next_pc
	.endm

.macro	op_79			/* ld a,c */
op_79:
	move.b	%d5,%d7
	next_pc
	.endm

.macro	op_7a			/* ld a,d */
op_7a:
	swap	%d4
	move.b	%d4,%d7
	swap	%d4
	next_pc
	.endm

.macro	op_7b			/* ld a,e */
op_7b:
	move.b	%d4,%d7
	next_pc
	.endm

.macro	op_7c			/* ld a,h */
op_7c:
	swap	%d3
	move.b	%d3,%d7
	swap	%d3
	next_pc
	.endm

.macro	op_7d			/* ld a,l */
op_7d:
	move.b	%d3,%d7
	next_pc
	.endm

.macro	op_7e			/* ld a,(hl) */
op_7e:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_7f			/* ld a,a */
op_7f:
	next_pc
	.endm

.macro	op_80			/* add a,b */
op_80:
	swap	%d5
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d5,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d5
	next_pc
	.endm

.macro	op_81			/* add a,c */
op_81:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d5,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_82			/* add a,d */
op_82:
	swap	%d4
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d4,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d4
	next_pc
	.endm

.macro	op_83			/* add a,e */
op_83:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d4,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_84			/* add a,h */
op_84:
	swap	%d3
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d3,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d3
	next_pc
	.endm

.macro	op_85			/* add a,l */
op_85:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d3,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_86			/* add a,(hl) */
op_86:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	(%a3),%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_87			/* add a,a */
op_87:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d7,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_88			/* adc a,b */
op_88:
	swap	%d5
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	addx.b	%d5,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d5
	next_pc
	.endm

.macro	op_89			/* adc a,c */
op_89:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	addx.b	%d5,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_8a			/* adc a,d */
op_8a:
	swap	%d4
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	addx.b	%d4,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d4
	next_pc
	.endm

.macro	op_8b			/* adc a,e */
op_8b:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	addx.b	%d4,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_8c			/* adc a,h */
op_8c:
	swap	%d3
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	addx.b	%d3,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d3
	next_pc
	.endm

.macro	op_8d			/* adc a,l */
op_8d:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	addx.b	%d3,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_8e			/* adc a,(hl) */
op_8e:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	%d7,op27a-Gbemu(%a4)
	move.b	(%a3),%d0
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	addx.b	%d0,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_8f			/* adc a,a */
op_8f:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	addx.b	%d7,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_90			/* sub b */
op_90:
	swap	%d5
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d5,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d5
	next_pc
	.endm

.macro	op_91			/* sub c */
op_91:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d5,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_92			/* sub d */
op_92:
	swap	%d4
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d4,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d4
	next_pc
	.endm

.macro	op_93			/* sub e */
op_93:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d4,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_94			/* sub h */
op_94:
	swap	%d3
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d3,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d3
	next_pc
	.endm

.macro	op_95			/* sub l */
op_95:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d3,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_96			/* sub (hl) */
op_96:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	(%a3),%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_97			/* sub a */
op_97:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d7,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_98			/* sbc a,b */
op_98:
	swap	%d5
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	subx.b	%d5,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d5
	next_pc
	.endm

.macro	op_99			/* sbc a,c */
op_99:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	subx.b	%d5,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_9a			/* sbc a,d */
op_9a:
	swap	%d4
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	subx.b	%d4,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d4
	next_pc
	.endm

.macro	op_9b			/* sbc a,e */
op_9b:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	subx.b	%d4,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_9c			/* sbc a,h */
op_9c:
	swap	%d3
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	subx.b	%d3,%d7
	move	%sr,%d6
	swap	%d6
	swap	%d3
	next_pc
	.endm

.macro	op_9d			/* sbc a,l */
op_9d:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	subx.b	%d3,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_9e			/* sbc a,(hl) */
op_9e:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	%d7,op27a-Gbemu(%a4)
	move.b	(%a3),%d0
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	subx.b	%d0,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_9f			/* sbc a,a */
op_9f:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	subx.b	%d7,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_a0			/* and b */
op_a0:
	swap	%d5
	swap	%d6
	and.b	%d5,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	swap	%d5
	next_pc
	.endm

.macro	op_a1			/* and c */
op_a1:
	swap	%d6
	and.b	%d5,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_a2			/* and d */
op_a2:
	swap	%d4
	swap	%d6
	and.b	%d4,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	swap	%d4
	next_pc
	.endm

.macro	op_a3			/* and e */
op_a3:
	swap	%d6
	and.b	%d4,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_a4			/* and h */
op_a4:
	swap	%d3
	swap	%d6
	and.b	%d3,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	swap	%d3
	next_pc
	.endm

.macro	op_a5			/* and l */
op_a5:
	swap	%d6
	and.b	%d3,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_a6			/* and (hl) */
op_a6:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	swap	%d6
	and.b	(%a3),%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_a7			/* and a */
op_a7:
	swap	%d6
	and.b	%d7,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_a8			/* xor b */
op_a8:
	swap	%d5
	swap	%d6
	eor.b	%d5,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	swap	%d5
	next_pc
	.endm

.macro	op_a9			/* xor c */
op_a9:
	swap	%d6
	eor.b	%d5,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_aa			/* xor d */
op_aa:
	swap	%d4
	swap	%d6
	eor.b	%d4,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	swap	%d4
	next_pc
	.endm

.macro	op_ab			/* xor e */
op_ab:
	swap	%d6
	eor.b	%d4,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_ac			/* xor h */
op_ac:
	swap	%d3
	swap	%d6
	eor.b	%d3,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	swap	%d3
	next_pc
	.endm

.macro	op_ad			/* xor l */
op_ad:
	swap	%d6
	eor.b	%d3,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_ae			/* xor (hl) */
op_ae:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	swap	%d6
	eor.b	%d0,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_af			/* xor a */
op_af:
	swap	%d6
	eor.b	%d7,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b0			/* or b */
op_b0:
	swap	%d5
	swap	%d6
	or.b	%d5,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	swap	%d5
	next_pc
	.endm

.macro	op_b1			/* or c */
op_b1:
	swap	%d6
	or.b	%d5,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b2			/* or d */
op_b2:
	swap	%d4
	swap	%d6
	or.b	%d4,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	swap	%d4
	next_pc
	.endm

.macro	op_b3			/* or e */
op_b3:
	swap	%d6
	or.b	%d4,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b4			/* or h */
op_b4:
	swap	%d3
	swap	%d6
	or.b	%d3,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	swap	%d3
	next_pc
	.endm

.macro	op_b5			/* or l */
op_b5:
	swap	%d6
	or.b	%d3,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b6			/* or (hl) */
op_b6:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	swap	%d6
	or.b	(%a3),%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b7			/* or a */
op_b7:
	swap	%d6
	or.b	%d7,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b8			/* cp b */
op_b8:
	move.b	%d7,%d0
	swap	%d5
	swap	%d6
	sub.b	%d5,%d0
	move	%sr,%d6
	swap	%d6
	swap	%d5
	next_pc
	.endm

.macro	op_b9			/* cp c */
op_b9:
	move.b	%d7,%d0
	swap	%d6
	sub.b	%d5,%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_ba			/* cp d */
op_ba:
	move.b	%d7,%d0
	swap	%d4
	swap	%d6
	sub.b	%d4,%d0
	move	%sr,%d6
	swap	%d6
	swap	%d4
	next_pc
	.endm

.macro	op_bb			/* cp e */
op_bb:
	move.b	%d7,%d0
	swap	%d6
	sub.b	%d4,%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_bc			/* cp h */
op_bc:
	move.b	%d7,%d0
	swap	%d3
	swap	%d6
	sub.b	%d3,%d0
	move	%sr,%d6
	swap	%d6
	swap	%d3
	next_pc
	.endm

.macro	op_bd			/* cp l */
op_bd:
	move.b	%d7,%d0
	swap	%d6
	sub.b	%d3,%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_be			/* cp (hl) */
op_be:
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	%d7,%d0
	swap	%d6
	sub.b	(%a3),%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_bf			/* cp a */
op_bf:
	move.b	%d7,%d0
	swap	%d6
	sub.b	%d0,%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_c0			/* ret nz */
op_c0:
	btst	#m68kFlagZ,%d6
	jne	quit\@
	do_ret
quit\@:	next_pc
	.endm

.macro	op_c1			/* pop bc */
op_c1:
	move.b	(%a1)+,%d0
	move.b	(%a1)+,%d5
	swap	%d5
	move.b	%d0,%d5
	sp_pop
	next_pc
	.endm

.macro	op_c2			/* jp nz,## */
op_c2:
	btst	#m68kFlagZ,%d6
	jne	quit\@
	do_jp
	move.l	%a3,%a0
	next_pc
quit\@:	addq.w	#2,%a0
	next_pc
	.endm

.macro	op_c3			/* jp ## */
op_c3:
	do_jp
	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_c4			/* call nz,## */
op_c4:
	lea	2(%a0),%a3
	btst	#m68kFlagZ,%d6
	jne	quit\@
	do_call
quit\@:	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_c5			/* push bc */
op_c5:
	sp_push
	move.l	%d5,%d0
	swap	%d0
	move.b	%d0,-(%a1)
	move.b	%d5,-(%a1)
	next_pc
	.endm

.macro	op_c6			/* add a,# */
op_c6:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	(%a0)+,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_c7			/* rst 00 */
op_c7:
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	next_pc
	.endm

.macro	op_c8			/* ret z */
op_c8:
	btst	#m68kFlagZ,%d6
	jeq	quit\@
	do_ret
quit\@:	next_pc
	.endm

.macro	op_c9			/* ret */
op_c9:
	do_ret
	next_pc
	.endm

.macro	op_ca			/* jp z,## */
op_ca:
	btst	#m68kFlagZ,%d6
	jeq	quit\@
	do_jp
	move.l	%a3,%a0
	next_pc
quit\@:	addq.w	#2,%a0
	next_pc
	.endm

.macro	op_cb			/* prefix byte 0xcb */
op_cb:
	move.b	(%a0)+,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetCb+dontOptHi(%a4)
	.endm

.macro	op_cc			/* call z,## */
op_cc:
	lea	2(%a0),%a3
	btst	#m68kFlagZ,%d6
	jeq	quit\@
	do_call
quit\@:	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_cd			/* call ## */
op_cd:
	lea	2(%a0),%a3
	do_call
	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_ce			/* adc a,# */
op_ce:
	move.b	%d7,op27a-Gbemu(%a4)
	move.b	(%a0)+,%d0
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	addx.b	%d0,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_cf			/* rst 08 */
op_cf:
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x08(%a0),%a0
	next_pc
	.endm

.macro	op_d0			/* ret nc */
op_d0:
	btst	#m68kFlagC,%d6
	jne	quit\@
	do_ret
quit\@:	next_pc
	.endm

.macro	op_d1			/* pop de */
op_d1:
	move.b	(%a1)+,%d0
	move.b	(%a1)+,%d4
	swap	%d4
	move.b	%d0,%d4
	sp_pop
	next_pc
	.endm

.macro	op_d2			/* jp nc,## */
op_d2:
	btst	#m68kFlagC,%d6
	jne	quit\@
	do_jp
	move.l	%a3,%a0
	next_pc
quit\@:	addq.w	#2,%a0
	next_pc
	.endm

.macro	op_d3			/* undefined */
op_d3:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_d4			/* call nc,## */
op_d4:
	lea	2(%a0),%a3
	btst	#m68kFlagC,%d6
	jne	quit\@
	do_call
quit\@:	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_d5			/* push de */
op_d5:
	sp_push
	move.l	%d4,%d0
	swap	%d0
	move.b	%d0,-(%a1)
	move.b	%d4,-(%a1)
	next_pc
	.endm

.macro	op_d6			/* sub # */
op_d6:
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	(%a0)+,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_d7			/* rst 10 */
op_d7:
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x10(%a0),%a0
	next_pc
	.endm

.macro	op_d8			/* ret c */
op_d8:
	btst	#m68kFlagC,%d6
	jeq	quit\@
	do_ret
quit\@:	next_pc
	.endm

.macro	op_d9			/* reti */
op_d9:
	moveq	#-1,%d0
	move.b	%d0,EmulationCpuState.ime@END.w(%a5)
	and.b	EmulationIo.ie@END.w(%a5),%d0
	move.b	%d0,EmulationCpuState.im@END.w(%a5)
	do_ret
	next_pc
	.endm

.macro	op_da			/* jp c,## */
op_da:
	btst	#m68kFlagC,%d6
	jeq	quit\@
	do_jp
	move.l	%a3,%a0
	next_pc
quit\@:	addq.w	#2,%a0
	next_pc
	.endm

.macro	op_db			/* undefined, used as breakpoint */
op_db:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_dc			/* call c,## */
op_dc:
	lea	2(%a0),%a3
	btst	#m68kFlagC,%d6
	jeq	quit\@
	do_call
quit\@:	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_dd			/* undefined */
op_dd:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_de			/* sbc a,# */
op_de:
	move.b	%d7,op27a-Gbemu(%a4)
	move.b	(%a0)+,%d0
	swap	%d6
	move	%d6,%ccr
	cmp.b	%d7,%d7
	subx.b	%d0,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_df			/* rst 18 */
op_df:
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x18(%a0),%a0
	next_pc
	.endm

.macro	op_e0			/* ld (ff00+#),a */
op_e0:
	move.b	%d7,%d0
	io_wr	(%a0)+
	next_pc
	.endm

.macro	op_e1			/* pop hl */
op_e1:
	move.b	(%a1)+,%d0
	move.b	(%a1)+,%d3
	swap	%d3
	move.b	%d0,%d3
	sp_pop
	next_pc
	.endm

.macro	op_e2			/* ld (ff00+c),a */
op_e2:
	move.b	%d7,%d0
	io_wr	%d5
	next_pc
	.endm

.macro	op_e3			/* undefined */
op_e3:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_e4			/* undefined */
op_e4:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_e5			/* push hl */
op_e5:
	sp_push
	move.l	%d3,%d0
	swap	%d0
	move.b	%d0,-(%a1)
	move.b	%d3,-(%a1)
	next_pc
	.endm

.macro	op_e6			/* and # */
op_e6:
	swap	%d6
	and.b	(%a0)+,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_e7			/* rst 20 */
op_e7:
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x20(%a0),%a0
	next_pc
	.endm

.macro	op_e8			/* add sp,# */
op_e8:
	lea	EmulationCpuState.spOffset@END.w(%a5),%a3
	move.w	(%a3),%d0
	move.b	(%a0)+,%d1
	ext.w	%d1
	add.w	%d1,%a1
	swap	%d6
	add.w	%d0,%d1
	move	%sr,%d6
	and.b	#~m68kMaskZ,%d6
	swap	%d6
	move.w	%d1,(%a3)
	eor.w	%d1,%d0
	and.w	#0xff00,%d0
	jeq	goon\@
	mem_rd	(%a3)
	move.l	%a2,EmulationCpuState.spBase@END.w(%a5)
	move.l	%a3,%a1
goon\@:	next_pc
	.endm

.macro	op_e9			/* jp hl */
op_e9:
	move.l	%d3,%d2
	swap	%d2
	move.b	%d3,%d1
	mem_rd	%d2
	move.l	%a2,EmulationCpuState.pcBase@END.w(%a5)
	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_ea			/* ld (##),a */
op_ea:
	move.b	%d7,%d0
	move.b	(%a0)+,%d1
	mem_wr	(%a0)+
	next_pc
	.endm

.macro	op_eb			/* undefined */
op_eb:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_ec			/* undefined */
op_ec:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_ed			/* undefined */
op_ed:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_ee			/* xor # */
op_ee:
	move.b	(%a0)+,%d0
	swap	%d6
	eor.b	%d0,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_ef			/* rst 28 */
op_ef:
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x28(%a0),%a0
	next_pc
	.endm

.macro	op_f0			/* ld a,(ff00+#) */
op_f0:
	io_rd	(%a0)+
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_f1			/* pop af */
op_f1:
	moveq	#m68kFlagZ,%d1
	moveq	#m68kFlagC,%d2
	bset	%d1,%d6
	bset	%d2,%d6
	move.b	(%a1)+,%d7
	moveq	#gbFlagZ,%d0
	and.b	%d7,%d0
	jne	nz\@
	bchg	%d1,%d6
nz\@:	moveq	#gbFlagC,%d0
	and.b	%d7,%d0
	jne	nc\@
	bchg	%d2,%d6
nc\@:	move.b	(%a1)+,%d7
	sp_pop
	next_pc
	.endm

.macro	op_f2			/* ld a,(ff00+c) */
op_f2:
	io_rd	%d5
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_f3			/* di */
op_f3:
	moveq	#0,%d0
	move.b	%d0,EmulationCpuState.ime@END.w(%a5)
	move.b	%d0,EmulationCpuState.im@END.w(%a5)
	next_pc
	.endm

.macro	op_f4			/* undefined */
op_f4:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_f5			/* push af */
op_f5:
	sp_push
	move.b	%d7,-(%a1)
	clr.b	%d0
	btst	#m68kFlagZ,%d6
	jeq	nz\@
	or	#gbFlagZ,%d0
nz\@:	btst	#m68kFlagC,%d6
	jeq	nc\@
	or	#gbFlagC,%d0
nc\@:	move.b	%d0,-(%a1)
	next_pc
	.endm

.macro	op_f6			/* or # */
op_f6:
	swap	%d6
	or.b	(%a0)+,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_f7			/* rst 30 */
op_f7:
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x30(%a0),%a0
	next_pc
	.endm

.macro	op_f8			/* ld hl,sp+# */
op_f8:
	swap	%d6
	move.b	(%a0)+,%d0
	ext.w	%d0
	add.w	EmulationCpuState.spOffset@END.w(%a5),%d0
	move	%sr,%d6
	and.b	#~m68kMaskZ,%d6
	swap	%d6
	move.w	%d0,-(%sp)
	move.b	(%sp)+,%d3
	swap	%d3
	move.b	%d0,%d3
	next_pc
	.endm

.macro	op_f9			/* ld sp,hl */
op_f9:
	move.b	%d3,%d1
	move.b	%d1,EmulationCpuState.spOffset+1@END.w(%a5)
	move.l	%d3,%d2
	swap	%d2
	move.b	%d2,EmulationCpuState.spOffset+0@END.w(%a5)
	mem_rd	%d2
	move.l	%a2,EmulationCpuState.spBase@END.w(%a5)
	move.l	%a3,%a1
	next_pc
	.endm

.macro	op_fa			/* ld a,(##) */
op_fa:
	move.b	(%a0)+,%d1
	mem_rd	(%a0)+
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_fb			/* ei */
op_fb:
	dbra	%d6,goon\@
	jsr	indexToAddress(gbemuJmpLcdLine)
goon\@:	moveq	#-1,%d0
	move.b	%d0,EmulationCpuState.ime@END.w(%a5)
	and.b	EmulationIo.ie@END.w(%a5),%d0
	move.b	%d0,EmulationCpuState.im@END.w(%a5)
	move.b	(%a0)+,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetOp+dontOptHi(%a4)
	.endm

.macro	op_fc			/* undefined */
op_fc:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_fd			/* undefined */
op_fd:
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_fe			/* cp # */
op_fe:
	move.b	%d7,%d0
	swap	%d6
	sub.b	(%a0)+,%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_ff			/* rst 38 */
op_ff:
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x38(%a0),%a0
	next_pc
	.endm

/* === The end ========================================================	*/
