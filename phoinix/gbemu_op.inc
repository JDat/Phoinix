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
	next_pc
	.endm

.macro	op_01			/* ld bc,## */
	move.b	(%a0)+,%d0
	move.b	(%a0)+,%d5
	swap	%d5
	move.b	%d0,%d5
	next_pc
	.endm

.macro	op_02			/* ld (bc),a */
	move.b	%d7,%d0
	move.b	%d5,%d1
	move.l	%d5,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_03			/* inc bc */
	ext.w	%d5
	addq.l	#1,%d5
	next_pc
	.endm

.macro	op_04			/* inc b */
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
	swap	%d5
	move.b	(%a0)+,%d5
	swap	%d5
	next_pc
	.endm

.macro	op_07			/* rlca */
	moveq	#m68kFlagC,%d0
	bset	%d0,%d6
	rol.b	#1,%d7
	jcs	c\@
	bchg	%d0,%d6
c\@:	next_pc
	.endm

.macro	op_08			/* ld (##),sp */
	move.b	(%a0)+,%d1
	move.b	(%a0)+,%d2
	jpl	quit\@
	mem_rd	%d2		/* used as "get mem addr" */
	move.b	EmulationCpuState.spOffset+1@END.w(%a5),(%a3)+
	move.b	EmulationCpuState.spOffset+0@END.w(%a5),(%a3)
quit\@:	next_pc
	.endm

.macro	op_09			/* add hl,bc */
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
	move.b	%d5,%d1
	move.l	%d5,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_0b			/* dec bc */
	ext.w	%d5
	subq.l	#1,%d5
	next_pc
	.endm

.macro	op_0c			/* inc c */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	addq.b	#1,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_0d			/* dec c */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	subq.b	#1,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_0e			/* ld c,# */
	move.b	(%a0)+,%d5
	next_pc
	.endm

.macro	op_0f			/* rrca */
	moveq	#m68kFlagC,%d0
	bset	%d0,%d6
	ror.b	#1,%d7
	jcs	c\@
	bchg	%d0,%d6
c\@:	next_pc
	.endm

.macro	op_10			/* stop */
	move.b	(%a0)+,%d0
	jeq	goon\@
	return	gbemuRetOpcodeStop
goon\@:
	jsr	indexToAddress(gbemuJmpOpcodeStop)
	next_pc
	.endm

.macro	op_11			/* ld de,## */
	move.b	(%a0)+,%d0
	move.b	(%a0)+,%d4
	swap	%d4
	move.b	%d0,%d4
	next_pc
	.endm

.macro	op_12			/* ld (de),a */
	move.b	%d7,%d0
	move.b	%d4,%d1
	move.l	%d4,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_13			/* inc de */
	ext.w	%d4
	addq.l	#1,%d4
	next_pc
	.endm

.macro	op_14			/* inc d */
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
	swap	%d4
	move.b	(%a0)+,%d4
	swap	%d4
	next_pc
	.endm

.macro	op_17			/* rla */
	swap	%d6
	move	%d6,%ccr
	swap	%d6
	moveq	#m68kFlagC,%d0
	bset	%d0,%d6
	roxl.b	#1,%d7
	jcs	c\@
	bchg	%d0,%d6
c\@:	next_pc
	.endm

.macro	op_18			/* jr # */
	move.b	(%a0)+,%d0
	ext.w	%d0
	add.w	%d0,%a0
	next_pc
	.endm

.macro	op_19			/* add hl,de */
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
	move.b	%d4,%d1
	move.l	%d4,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_1b			/* dec de */
	ext.w	%d4
	subq.l	#1,%d4
	next_pc
	.endm

.macro	op_1c			/* inc e */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	addq.b	#1,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_1d			/* dec e */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	subq.b	#1,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_1e			/* ld e,# */
	move.b	(%a0)+,%d4
	next_pc
	.endm

.macro	op_1f			/* rra */
	swap	%d6
	move	%d6,%ccr
	swap	%d6
	moveq	#m68kFlagC,%d0
	bset	%d0,%d6
	roxr.b	#1,%d7
	jcs	c\@
	bchg	%d0,%d6
c\@:	next_pc
	.endm

.macro	op_20			/* jr nz,# */
	move.b	(%a0)+,%d1
	btst	#m68kFlagZ,%d6
	jne	quit\@
	ext.w	%d1
	add.w	%d1,%a0
quit\@:	next_pc
	.endm

.macro	op_21			/* ld hl,## */
	move.b	(%a0)+,%d0
	move.b	(%a0)+,%d3
	swap	%d3
	move.b	%d0,%d3
	next_pc
	.endm

.macro	op_22			/* ldi (hl),a */
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
	ext.w	%d3
	addq.l	#1,%d3
	next_pc
	.endm

.macro	op_24			/* inc h */
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
	swap	%d3
	move.b	(%a0)+,%d3
	swap	%d3
	next_pc
	.endm

.macro	op_27			/* daa */
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
	move.b	(%a0)+,%d1
	btst	#m68kFlagZ,%d6
	jeq	quit\@
	ext.w	%d1
	add.w	%d1,%a0
quit\@:	next_pc
	.endm

.macro	op_29			/* add hl,hl */
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
	ext.w	%d3
	subq.l	#1,%d3
	next_pc
	.endm

.macro	op_2c			/* inc l */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	addq.b	#1,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_2d			/* dec l */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	subq.b	#1,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_2e			/* ld l,# */
	move.b	(%a0)+,%d3
	next_pc
	.endm

.macro	op_2f			/* cpl */
	not.b	%d7
	next_pc
	.endm

.macro	op_30			/* jr nc,# */
	move.b	(%a0)+,%d1
	btst	#m68kFlagC,%d6
	jne	quit\@
	ext.w	%d1
	add.w	%d1,%a0
quit\@:	next_pc
	.endm

.macro	op_31			/* ld sp,## */
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
	move.b	(%a0)+,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_37			/* scf */
	bset	#m68kFlagC,%d6
	next_pc
	.endm

.macro	op_38			/* jr c,# */
	move.b	(%a0)+,%d1
	btst	#m68kFlagC,%d6
	jeq	quit\@
	ext.w	%d1
	add.w	%d1,%a0
quit\@:	next_pc
	.endm

.macro	op_39			/* add hl,sp */
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
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	move.b	%d7,op27a-Gbemu(%a4)
	addq.b	#1,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_3d			/* dec a */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	move.b	%d7,op27a-Gbemu(%a4)
	subq.b	#1,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	op_3e			/* ld a,# */
	move.b	(%a0)+,%d7
	next_pc
	.endm

.macro	op_3f			/* ccf */
	bchg	#m68kFlagC,%d6
	next_pc
	.endm

.macro	op_40			/* ld b,b */
	next_pc
	.endm

.macro	op_41			/* ld b,c */
	move.b	%d5,%d0
	swap	%d5
	move.b	%d0,%d5
	next_pc
	.endm

.macro	op_42			/* ld b,d */
	move.l	%d4,%d0
	move.b	%d5,%d0
	move.l	%d0,%d5
	next_pc
	.endm

.macro	op_43			/* ld b,e */
	swap	%d5
	move.b	%d4,%d5
	swap	%d5
	next_pc
	.endm

.macro	op_44			/* ld b,h */
	move.l	%d3,%d0
	move.b	%d5,%d0
	move.l	%d0,%d5
	next_pc
	.endm

.macro	op_45			/* ld b,l */
	swap	%d5
	move.b	%d3,%d5
	swap	%d5
	next_pc
	.endm

.macro	op_46			/* ld b,(hl) */
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
	swap	%d5
	move.b	%d7,%d5
	swap	%d5
	next_pc
	.endm

.macro	op_48			/* ld c,b */
	move.l	%d5,%d0
	swap	%d0
	move.b	%d0,%d5
	next_pc
	.endm

.macro	op_49			/* ld c,c */
	next_pc
	.endm

.macro	op_4a			/* ld c,d */
	swap	%d4
	move.b	%d4,%d5
	swap	%d4
	next_pc
	.endm

.macro	op_4b			/* ld c,e */
	move.b	%d4,%d5
	next_pc
	.endm

.macro	op_4c			/* ld c,h */
	swap	%d3
	move.b	%d3,%d5
	swap	%d3
	next_pc
	.endm

.macro	op_4d			/* ld c,l */
	move.b	%d3,%d5
	next_pc
	.endm

.macro	op_4e			/* ld c,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d5
	next_pc
	.endm

.macro	op_4f			/* ld c,a */
	move.b	%d7,%d5
	next_pc
	.endm

.macro	op_50			/* ld d,b */
	move.l	%d5,%d0
	move.b	%d4,%d0
	move.l	%d0,%d4
	next_pc
	.endm

.macro	op_51			/* ld d,c */
	swap	%d4
	move.b	%d5,%d4
	swap	%d4
	next_pc
	.endm

.macro	op_52			/* ld d,d */
	next_pc
	.endm

.macro	op_53			/* ld d,e */
	move.b	%d4,%d0
	swap	%d4
	move.b	%d0,%d4
	next_pc
	.endm

.macro	op_54			/* ld d,h */
	move.l	%d3,%d0
	move.b	%d4,%d0
	move.l	%d0,%d4
	next_pc
	.endm

.macro	op_55			/* ld d,l */
	swap	%d4
	move.b	%d3,%d4
	swap	%d4
	next_pc
	.endm

.macro	op_56			/* ld d,(hl) */
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
	swap	%d4
	move.b	%d7,%d4
	swap	%d4
	next_pc
	.endm

.macro	op_58			/* ld e,b */
	swap	%d5
	move.b	%d5,%d4
	swap	%d5
	next_pc
	.endm

.macro	op_59			/* ld e,c */
	move.b	%d5,%d4
	next_pc
	.endm

.macro	op_5a			/* ld e,d */
	move.l	%d4,%d0
	swap	%d0
	move.b	%d0,%d4
	next_pc
	.endm

.macro	op_5b			/* ld e,e */
	next_pc
	.endm

.macro	op_5c			/* ld e,h */
	swap	%d3
	move.b	%d3,%d4
	swap	%d3
	next_pc
	.endm

.macro	op_5d			/* ld e,l */
	move.b	%d3,%d4
	next_pc
	.endm

.macro	op_5e			/* ld e,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d4
	next_pc
	.endm

.macro	op_5f			/* ld e,a */
	move.b	%d7,%d4
	next_pc
	.endm

.macro	op_60			/* ld h,b */
	move.l	%d5,%d0
	move.b	%d3,%d0
	move.l	%d0,%d3
	next_pc
	.endm

.macro	op_61			/* ld h,c */
	swap	%d3
	move.b	%d5,%d3
	swap	%d3
	next_pc
	.endm

.macro	op_62			/* ld h,d */
	move.l	%d4,%d0
	move.b	%d3,%d0
	move.l	%d0,%d3
	next_pc
	.endm

.macro	op_63			/* ld h,e */
	swap	%d3
	move.b	%d4,%d3
	swap	%d3
	next_pc
	.endm

.macro	op_64			/* ld h,h */
	next_pc
	.endm

.macro	op_65			/* ld h,l */
	move.b	%d3,%d0
	swap	%d3
	move.b	%d0,%d3
	next_pc
	.endm

.macro	op_66			/* ld h,(hl) */
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
	swap	%d3
	move.b	%d7,%d3
	swap	%d3
	next_pc
	.endm

.macro	op_68			/* ld l,b */
	swap	%d5
	move.b	%d5,%d3
	swap	%d5
	next_pc
	.endm

.macro	op_69			/* ld l,c */
	move.b	%d5,%d3
	next_pc
	.endm

.macro	op_6a			/* ld l,d */
	swap	%d4
	move.b	%d4,%d3
	swap	%d4
	next_pc
	.endm

.macro	op_6b			/* ld l,e */
	move.b	%d4,%d3
	next_pc
	.endm

.macro	op_6c			/* ld l,h */
	move.l	%d3,%d0
	swap	%d0
	move.b	%d0,%d3
	next_pc
	.endm

.macro	op_6d			/* ld l,l */
	next_pc
	.endm

.macro	op_6e			/* ld l,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d3
	next_pc
	.endm

.macro	op_6f			/* ld l,a */
	move.b	%d7,%d3
	next_pc
	.endm

.macro	op_70			/* ld (hl),b */
	move.l	%d5,%d0
	swap	%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_71			/* ld (hl),c */
	move.b	%d5,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_72			/* ld (hl),d */
	move.l	%d4,%d0
	swap	%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_73			/* ld (hl),e */
	move.b	%d4,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_74			/* ld (hl),h */
	move.l	%d3,%d0
	swap	%d0
	move.b	%d3,%d1
	move.b	%d0,%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_75			/* ld (hl),l */
	move.b	%d3,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_76			/* halt, no GB quirk emulation */
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
	move.b	%d7,%d0
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_wr	%d2
	next_pc
	.endm

.macro	op_78			/* ld a,b */
	swap	%d5
	move.b	%d5,%d7
	swap	%d5
	next_pc
	.endm

.macro	op_79			/* ld a,c */
	move.b	%d5,%d7
	next_pc
	.endm

.macro	op_7a			/* ld a,d */
	swap	%d4
	move.b	%d4,%d7
	swap	%d4
	next_pc
	.endm

.macro	op_7b			/* ld a,e */
	move.b	%d4,%d7
	next_pc
	.endm

.macro	op_7c			/* ld a,h */
	swap	%d3
	move.b	%d3,%d7
	swap	%d3
	next_pc
	.endm

.macro	op_7d			/* ld a,l */
	move.b	%d3,%d7
	next_pc
	.endm

.macro	op_7e			/* ld a,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_7f			/* ld a,a */
	next_pc
	.endm

.macro	op_80			/* add a,b */
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
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d5,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_82			/* add a,d */
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
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d4,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_84			/* add a,h */
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
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d3,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_86			/* add a,(hl) */
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
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	%d7,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_88			/* adc a,b */
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
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d5,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_92			/* sub d */
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
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d4,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_94			/* sub h */
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
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d3,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_96			/* sub (hl) */
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
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	%d7,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_98			/* sbc a,b */
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
	swap	%d6
	and.b	%d5,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_a2			/* and d */
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
	swap	%d6
	and.b	%d4,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_a4			/* and h */
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
	swap	%d6
	and.b	%d3,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_a6			/* and (hl) */
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
	swap	%d6
	and.b	%d7,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_a8			/* xor b */
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
	swap	%d6
	eor.b	%d5,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_aa			/* xor d */
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
	swap	%d6
	eor.b	%d4,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_ac			/* xor h */
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
	swap	%d6
	eor.b	%d3,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_ae			/* xor (hl) */
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
	swap	%d6
	eor.b	%d7,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b0			/* or b */
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
	swap	%d6
	or.b	%d5,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b2			/* or d */
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
	swap	%d6
	or.b	%d4,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b4			/* or h */
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
	swap	%d6
	or.b	%d3,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b6			/* or (hl) */
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
	swap	%d6
	or.b	%d7,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_b8			/* cp b */
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
	move.b	%d7,%d0
	swap	%d6
	sub.b	%d5,%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_ba			/* cp d */
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
	move.b	%d7,%d0
	swap	%d6
	sub.b	%d4,%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_bc			/* cp h */
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
	move.b	%d7,%d0
	swap	%d6
	sub.b	%d3,%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_be			/* cp (hl) */
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
	move.b	%d7,%d0
	swap	%d6
	sub.b	%d0,%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_c0			/* ret nz */
	btst	#m68kFlagZ,%d6
	jne	quit\@
	do_ret
quit\@:	next_pc
	.endm

.macro	op_c1			/* pop bc */
	move.b	(%a1)+,%d0
	move.b	(%a1)+,%d5
	swap	%d5
	move.b	%d0,%d5
	sp_pop
	next_pc
	.endm

.macro	op_c2			/* jp nz,## */
	btst	#m68kFlagZ,%d6
	jne	quit\@
	do_jp
	move.l	%a3,%a0
	next_pc
quit\@:	addq.w	#2,%a0
	next_pc
	.endm

.macro	op_c3			/* jp ## */
	do_jp
	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_c4			/* call nz,## */
	lea	2(%a0),%a3
	btst	#m68kFlagZ,%d6
	jne	quit\@
	do_call
quit\@:	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_c5			/* push bc */
	sp_push
	move.l	%d5,%d0
	swap	%d0
	move.b	%d0,-(%a1)
	move.b	%d5,-(%a1)
	next_pc
	.endm

.macro	op_c6			/* add a,# */
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	add.b	(%a0)+,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_c7			/* rst 00 */
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	next_pc
	.endm

.macro	op_c8			/* ret z */
	btst	#m68kFlagZ,%d6
	jeq	quit\@
	do_ret
quit\@:	next_pc
	.endm

.macro	op_c9			/* ret */
	do_ret
	next_pc
	.endm

.macro	op_ca			/* jp z,## */
	btst	#m68kFlagZ,%d6
	jeq	quit\@
	do_jp
	move.l	%a3,%a0
	next_pc
quit\@:	addq.w	#2,%a0
	next_pc
	.endm

.macro	op_cb			/* prefix byte 0xcb */
	move.b	(%a0)+,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetCb+dontOptHi(%a4)
	.endm

.macro	op_cc			/* call z,## */
	lea	2(%a0),%a3
	btst	#m68kFlagZ,%d6
	jeq	quit\@
	do_call
quit\@:	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_cd			/* call ## */
	lea	2(%a0),%a3
	do_call
	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_ce			/* adc a,# */
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
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x08(%a0),%a0
	next_pc
	.endm

.macro	op_d0			/* ret nc */
	btst	#m68kFlagC,%d6
	jne	quit\@
	do_ret
quit\@:	next_pc
	.endm

.macro	op_d1			/* pop de */
	move.b	(%a1)+,%d0
	move.b	(%a1)+,%d4
	swap	%d4
	move.b	%d0,%d4
	sp_pop
	next_pc
	.endm

.macro	op_d2			/* jp nc,## */
	btst	#m68kFlagC,%d6
	jne	quit\@
	do_jp
	move.l	%a3,%a0
	next_pc
quit\@:	addq.w	#2,%a0
	next_pc
	.endm

.macro	op_d3			/* undefined */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_d4			/* call nc,## */
	lea	2(%a0),%a3
	btst	#m68kFlagC,%d6
	jne	quit\@
	do_call
quit\@:	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_d5			/* push de */
	sp_push
	move.l	%d4,%d0
	swap	%d0
	move.b	%d0,-(%a1)
	move.b	%d4,-(%a1)
	next_pc
	.endm

.macro	op_d6			/* sub # */
	move.b	%d7,op27a-Gbemu(%a4)
	swap	%d6
	sub.b	(%a0)+,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_d7			/* rst 10 */
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x10(%a0),%a0
	next_pc
	.endm

.macro	op_d8			/* ret c */
	btst	#m68kFlagC,%d6
	jeq	quit\@
	do_ret
quit\@:	next_pc
	.endm

.macro	op_d9			/* reti */
	moveq	#-1,%d0
	move.b	%d0,EmulationIntIme@END.w(%a5)
	and.b	EmulationIo.ie@END.w(%a5),%d0
	move.b	%d0,EmulationIntIm@END.w(%a5)
	do_ret
	next_pc
	.endm

.macro	op_da			/* jp c,## */
	btst	#m68kFlagC,%d6
	jeq	quit\@
	do_jp
	move.l	%a3,%a0
	next_pc
quit\@:	addq.w	#2,%a0
	next_pc
	.endm

.macro	op_db			/* undefined, used as breakpoint */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_dc			/* call c,## */
	lea	2(%a0),%a3
	btst	#m68kFlagC,%d6
	jeq	quit\@
	do_call
quit\@:	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_dd			/* undefined */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_de			/* sbc a,# */
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
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x18(%a0),%a0
	next_pc
	.endm

.macro	op_e0			/* ld (ff00+#),a */
	move.b	%d7,%d0
	io_wr	(%a0)+
	next_pc
	.endm

.macro	op_e1			/* pop hl */
	move.b	(%a1)+,%d0
	move.b	(%a1)+,%d3
	swap	%d3
	move.b	%d0,%d3
	sp_pop
	next_pc
	.endm

.macro	op_e2			/* ld (ff00+c),a */
	move.b	%d7,%d0
	io_wr	%d5
	next_pc
	.endm

.macro	op_e3			/* undefined */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_e4			/* undefined */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_e5			/* push hl */
	sp_push
	move.l	%d3,%d0
	swap	%d0
	move.b	%d0,-(%a1)
	move.b	%d3,-(%a1)
	next_pc
	.endm

.macro	op_e6			/* and # */
	swap	%d6
	and.b	(%a0)+,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_e7			/* rst 20 */
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x20(%a0),%a0
	next_pc
	.endm

.macro	op_e8			/* add sp,# */
	move.b	(%a0)+,%d0
	ext.w	%d0
	jpl	up\@
	add.w	%d0,%a1
	add.b	%d0,EmulationCpuState.spOffset+1@END.w(%a5)
	jcs	goon\@
	subq.b	#1,EmulationCpuState.spOffset+0@END.w(%a5)
	jra	corr\@
up\@:	add.w	%d0,%a1
	add.b	%d0,EmulationCpuState.spOffset+1@END.w(%a5)
	jcc	goon\@
	addq.b	#1,EmulationCpuState.spOffset+0@END.w(%a5)
corr\@:	move.b	EmulationCpuState.spOffset+1@END.w(%a5),%d1
	mem_rd	EmulationCpuState.spOffset+0@END.w(%a5)
	move.l	%a2,EmulationCpuState.spBase@END.w(%a5)
	move.l	%a3,%a1
goon\@:	next_pc
	.endm

.macro	op_e9			/* jp hl */
	move.l	%d3,%d2
	swap	%d2
	move.b	%d3,%d1
	mem_rd	%d2
	move.l	%a2,EmulationCpuState.pcBase@END.w(%a5)
	move.l	%a3,%a0
	next_pc
	.endm

.macro	op_ea			/* ld (##),a */
	move.b	%d7,%d0
	move.b	(%a0)+,%d1
	mem_wr	(%a0)+
	next_pc
	.endm

.macro	op_eb			/* undefined */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_ec			/* undefined */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_ed			/* undefined */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_ee			/* xor # */
	move.b	(%a0)+,%d0
	swap	%d6
	eor.b	%d0,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_ef			/* rst 28 */
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x28(%a0),%a0
	next_pc
	.endm

.macro	op_f0			/* ld a,(ff00+#) */
	io_rd	(%a0)+
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_f1			/* pop af */
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
	io_rd	%d5
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_f3			/* di */
	moveq	#0,%d0
	move.b	%d0,EmulationIntIme@END.w(%a5)
	move.b	%d0,EmulationIntIm@END.w(%a5)
	next_pc
	.endm

.macro	op_f4			/* undefined */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_f5			/* push af */
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
	swap	%d6
	or.b	(%a0)+,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_f7			/* rst 30 */
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x30(%a0),%a0
	next_pc
	.endm

.macro	op_f8			/* ld hl,sp+# */
	move.b	(%a0)+,%d0
	ext.w	%d0
	add.w	EmulationCpuState.spOffset@END.w(%a5),%d0
	move.w	%d0,-(%sp)
	move.b	(%sp)+,%d3
	swap	%d3
	move.b	%d0,%d3
	next_pc
	.endm

.macro	op_f9			/* ld sp,hl */
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
	move.b	(%a0)+,%d1
	mem_rd	(%a0)+
	move.b	(%a3),%d7
	next_pc
	.endm

.macro	op_fb			/* ei */
	dbra	%d6,goon\@
	jsr	indexToAddress(gbemuJmpLcdLine)
goon\@:	moveq	#-1,%d0
	move.b	%d0,EmulationIntIme@END.w(%a5)
	and.b	EmulationIo.ie@END.w(%a5),%d0
	move.b	%d0,EmulationIntIm@END.w(%a5)
	move.b	(%a0)+,jump\@+2-Gbemu(%a4)
jump\@:	jmp	offsetOp+dontOptHi(%a4)
	.endm

.macro	op_fc			/* undefined */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_fd			/* undefined */
	return	gbemuRetOpcodeUnknown
	.endm

.macro	op_fe			/* cp # */
	move.b	%d7,%d0
	swap	%d6
	sub.b	(%a0)+,%d0
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	op_ff			/* rst 38 */
	do_push	%a0
	move.l	MemoryBaseRom0@END.w(%a5),%a0
	move.l	%a0,EmulationCpuState.pcBase@END.w(%a5)
	lea	0x38(%a0),%a0
	next_pc
	.endm

/* === The end ========================================================	*/
