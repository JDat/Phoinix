/************************************************************************
 *  gbemu_cb.inc
 *                      Emulation of the Gameboy CPU
 *                      (Opcodes after prefix byte 0xcb)
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
 *  Revision 1.1.1.1  2001/12/16 13:38:47  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-07-21  Bodo Wenzel   Changed syntax to CPP + GAS
 *  2000-09-29  Bodo Wenzel   Some improvements
 *  2000-02-23  Bodo Wenzel   Creation
 ************************************************************************
 */

.macro	cb_00			/* rlc b */
	swap	%d6
	swap	%d5
	move.b	%d5,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_01			/* rlc c */
	swap	%d6
	move.b	%d5,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_02			/* rlc d */
	swap	%d6
	swap	%d4
	move.b	%d4,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_03			/* rlc e */
	swap	%d6
	move.b	%d4,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_04			/* rlc h */
	swap	%d6
	swap	%d3
	move.b	%d3,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_05			/* rlc l */
	swap	%d6
	move.b	%d3,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_06			/* rlc (hl) */
	swap	%d6
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	roxl.b	#1,%d0
	move.b	(%a3),%d0
	rol.b	#1,%d0
	move	%sr,%d6
	mem_wr	%d2
	swap	%d6
	next_pc
	.endm

.macro	cb_07			/* rlc a */
	swap	%d6
	move.b	%d7,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_08			/* rrc b */
	swap	%d6
	swap	%d5
	move.b	%d5,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_09			/* rrc c */
	swap	%d6
	move.b	%d5,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_0a			/* rrc d */
	swap	%d6
	swap	%d4
	move.b	%d4,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_0b			/* rrc e */
	swap	%d6
	move.b	%d4,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_0c			/* rrc h */
	swap	%d6
	swap	%d3
	move.b	%d3,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_0d			/* rrc l */
	swap	%d6
	move.b	%d3,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_0e			/* rrc (hl) */
	swap	%d6
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	roxr.b	#1,%d0
	move.b	(%a3),%d0
	ror.b	#1,%d0
	move	%sr,%d6
	mem_wr	%d2
	swap	%d6
	next_pc
	.endm

.macro	cb_0f			/* rrc a */
	swap	%d6
	move.b	%d7,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_10			/* rl b */
	swap	%d6
	swap	%d5
	move	%d6,%ccr
	roxl.b	#1,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_11			/* rl c */
	swap	%d6
	move	%d6,%ccr
	roxl.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_12			/* rl d */
	swap	%d6
	swap	%d4
	move	%d6,%ccr
	roxl.b	#1,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_13			/* rl e */
	swap	%d6
	move	%d6,%ccr
	roxl.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_14			/* rl h */
	swap	%d6
	swap	%d3
	move	%d6,%ccr
	roxl.b	#1,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_15			/* rl l */
	swap	%d6
	move	%d6,%ccr
	roxl.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_16			/* rl (hl) */
	swap	%d6
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	move	%d6,%ccr
	roxl.b	#1,%d0
	move	%sr,%d6
	mem_wr	%d2
	swap	%d6
	next_pc
	.endm

.macro	cb_17			/* rl a */
	swap	%d6
	move	%d6,%ccr
	roxl.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_18			/* rr b */
	swap	%d6
	swap	%d5
	move	%d6,%ccr
	roxr.b	#1,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_19			/* rr c */
	swap	%d6
	move	%d6,%ccr
	roxr.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_1a			/* rr d */
	swap	%d6
	swap	%d4
	move	%d6,%ccr
	roxr.b	#1,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_1b			/* rr e */
	swap	%d6
	move	%d6,%ccr
	roxr.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_1c			/* rr h */
	swap	%d6
	swap	%d3
	move	%d6,%ccr
	roxr.b	#1,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_1d			/* rr l */
	swap	%d6
	move	%d6,%ccr
	roxr.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_1e			/* rr (hl) */
	swap	%d6
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	move	%d6,%ccr
	roxr.b	#1,%d0
	move	%sr,%d6
	mem_wr	%d2
	swap	%d6
	next_pc
	.endm

.macro	cb_1f			/* rr a */
	swap	%d6
	move	%d6,%ccr
	roxr.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_20			/* sla b */
	swap	%d6
	swap	%d5
	asl.b	#1,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_21			/* sla c */
	swap	%d6
	asl.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_22			/* sla d */
	swap	%d6
	swap	%d4
	asl.b	#1,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_23			/* sla e */
	swap	%d6
	asl.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_24			/* sla h */
	swap	%d6
	swap	%d3
	asl.b	#1,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_25			/* sla l */
	swap	%d6
	asl.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_26			/* sla (hl) */
	swap	%d6
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	asl.b	#1,%d0
	move	%sr,%d6
	mem_wr	%d2
	swap	%d6
	next_pc
	.endm

.macro	cb_27			/* sla a */
	swap	%d6
	asl.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_28			/* sra b */
	swap	%d6
	swap	%d5
	asr.b	#1,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_29			/* sra c */
	swap	%d6
	asr.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_2a			/* sra d */
	swap	%d6
	swap	%d4
	asr.b	#1,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_2b			/* sra e */
	swap	%d6
	asr.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_2c			/* sra h */
	swap	%d6
	swap	%d3
	asr.b	#1,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_2d			/* sra l */
	swap	%d6
	asr.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_2e			/* sra (hl) */
	swap	%d6
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	asr.b	#1,%d0
	move	%sr,%d6
	mem_wr	%d2
	swap	%d6
	next_pc
	.endm

.macro	cb_2f			/* sra a */
	swap	%d6
	asr.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_30			/* swap b */
	swap	%d6
	swap	%d5
	move	#~m68kMaskC,%ccr
	rol.b	#4,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_31			/* swap c */
	swap	%d6
	move	#~m68kMaskC,%ccr
	rol.b	#4,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_32			/* swap d */
	swap	%d6
	swap	%d4
	move	#~m68kMaskC,%ccr
	rol.b	#4,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_33			/* swap e */
	swap	%d6
	move	#~m68kMaskC,%ccr
	rol.b	#4,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_34			/* swap h */
	swap	%d6
	swap	%d3
	move	#~m68kMaskC,%ccr
	rol.b	#4,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_35			/* swap l */
	swap	%d6
	move	#~m68kMaskC,%ccr
	rol.b	#4,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_36			/* swap (hl) */
	swap	%d6
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	move	#~m68kMaskC,%ccr
	rol.b	#4,%d0
	move	%sr,%d6
	mem_wr	%d2
	swap	%d6
	next_pc
	.endm

.macro	cb_37			/* swap a */
	swap	%d6
	move	#~m68kMaskC,%ccr
	rol.b	#4,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_38			/* srl b */
	swap	%d6
	swap	%d5
	lsr.b	#1,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_39			/* srl c */
	swap	%d6
	lsr.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_3a			/* srl d */
	swap	%d6
	swap	%d4
	lsr.b	#1,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_3b			/* srl e */
	swap	%d6
	lsr.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_3c			/* srl h */
	swap	%d6
	swap	%d3
	lsr.b	#1,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_3d			/* srl l */
	swap	%d6
	lsr.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_3e			/* srl (hl) */
	swap	%d6
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	lsr.b	#1,%d0
	move	%sr,%d6
	mem_wr	%d2
	swap	%d6
	next_pc
	.endm

.macro	cb_3f			/* srl a */
	swap	%d6
	lsr.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_40			/* bit 0,b */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#16,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_41			/* bit 0,c */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#0,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_42			/* bit 0,d */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#16,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_43			/* bit 0,e */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#0,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_44			/* bit 0,h */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#16,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_45			/* bit 0,l */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#0,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_46			/* bit 0,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#0,(%a3)
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_47			/* bit 0,a */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#0,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_48			/* bit 1,b */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#17,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_49			/* bit 1,c */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#1,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4a			/* bit 1,d */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#17,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4b			/* bit 1,e */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#1,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4c			/* bit 1,h */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#17,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4d			/* bit 1,l */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#1,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4e			/* bit 1,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#1,(%a3)
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4f			/* bit 1,a */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#1,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_50			/* bit 2,b */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#18,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_51			/* bit 2,c */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#2,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_52			/* bit 2,d */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#18,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_53			/* bit 2,e */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#2,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_54			/* bit 2,h */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#18,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_55			/* bit 2,l */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#2,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_56			/* bit 2,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#2,(%a3)
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_57			/* bit 2,a */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#2,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_58			/* bit 1,b */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#19,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_59			/* bit 3,c */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#3,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5a			/* bit 3,d */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#19,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5b			/* bit 3,e */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#3,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5c			/* bit 3,h */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#19,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5d			/* bit 3,l */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#3,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5e			/* bit 3,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#3,(%a3)
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5f			/* bit 3,a */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#3,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_60			/* bit 4,b */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#20,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_61			/* bit 4,c */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#4,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_62			/* bit 4,d */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#20,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_63			/* bit 4,e */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#4,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_64			/* bit 4,h */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#20,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_65			/* bit 4,l */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#4,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_66			/* bit 4,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#4,(%a3)
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_67			/* bit 4,a */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#4,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_68			/* bit 5,b */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#21,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_69			/* bit 5,c */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#5,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6a			/* bit 5,d */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#21,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6b			/* bit 5,e */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#5,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6c			/* bit 5,h */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#21,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6d			/* bit 5,l */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#5,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6e			/* bit 5,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#5,(%a3)
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6f			/* bit 5,a */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#5,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_70			/* bit 6,b */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#22,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_71			/* bit 6,c */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#6,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_72			/* bit 6,d */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#22,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_73			/* bit 6,e */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#6,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_74			/* bit 6,h */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#22,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_75			/* bit 6,l */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#6,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_76			/* bit 6,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#6,(%a3)
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_77			/* bit 6,a */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#6,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_78			/* bit 7,b */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#23,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_79			/* bit 7,c */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#7,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7a			/* bit 7,d */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#23,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7b			/* bit 7,e */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#7,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7c			/* bit 7,h */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#23,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7d			/* bit 7,l */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#7,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7e			/* bit 7,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#7,(%a3)
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7f			/* bit 7,a */
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#7,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_80			/* res 0,b */
	moveq	#16,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_81			/* res 0,c */
	and.b	#~0x01,%d5
	next_pc
	.endm

.macro	cb_82			/* res 0,d */
	moveq	#16,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_83			/* res 0,e */
	and.b	#~0x01,%d4
	next_pc
	.endm

.macro	cb_84			/* res 0,h */
	moveq	#16,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_85			/* res 0,l */
	and.b	#~0x01,%d3
	next_pc
	.endm

.macro	cb_86			/* res 0,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#~0x01,%d0
	and.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_87			/* res 0,a */
	and.b	#~0x01,%d7
	next_pc
	.endm

.macro	cb_88			/* res 1,b */
	moveq	#17,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_89			/* res 1,c */
	and.b	#~0x02,%d5
	next_pc
	.endm

.macro	cb_8a			/* res 1,d */
	moveq	#17,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_8b			/* res 1,e */
	and.b	#~0x02,%d4
	next_pc
	.endm

.macro	cb_8c			/* res 1,h */
	moveq	#17,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_8d			/* res 1,l */
	and.b	#~0x02,%d3
	next_pc
	.endm

.macro	cb_8e			/* res 1,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#~0x02,%d0
	and.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_8f			/* res 1,a */
	and.b	#~0x02,%d7
	next_pc
	.endm

.macro	cb_90			/* res 2,b */
	moveq	#18,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_91			/* res 2,c */
	and.b	#~0x04,%d5
	next_pc
	.endm

.macro	cb_92			/* res 2,d */
	moveq	#18,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_93			/* res 2,e */
	and.b	#~0x04,%d4
	next_pc
	.endm

.macro	cb_94			/* res 2,h */
	moveq	#18,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_95			/* res 2,l */
	and.b	#~0x04,%d3
	next_pc
	.endm

.macro	cb_96			/* res 2,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#~0x04,%d0
	and.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_97			/* res 2,a */
	and.b	#~0x04,%d7
	next_pc
	.endm

.macro	cb_98			/* res 3,b */
	moveq	#19,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_99			/* res 3,c */
	and.b	#~0x08,%d5
	next_pc
	.endm

.macro	cb_9a			/* res 3,d */
	moveq	#19,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_9b			/* res 3,e */
	and.b	#~0x08,%d4
	next_pc
	.endm

.macro	cb_9c			/* res 3,h */
	moveq	#19,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_9d			/* res 3,l */
	and.b	#~0x08,%d3
	next_pc
	.endm

.macro	cb_9e			/* res 3,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#~0x08,%d0
	and.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_9f			/* res 3,a */
	and.b	#~0x08,%d7
	next_pc
	.endm

.macro	cb_a0			/* res 4,b */
	moveq	#20,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_a1			/* res 4,c */
	and.b	#~0x10,%d5
	next_pc
	.endm

.macro	cb_a2			/* res 4,d */
	moveq	#20,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_a3			/* res 4,e */
	and.b	#~0x10,%d4
	next_pc
	.endm

.macro	cb_a4			/* res 4,h */
	moveq	#20,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_a5			/* res 4,l */
	and.b	#~0x10,%d3
	next_pc
	.endm

.macro	cb_a6			/* res 4,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#~0x10,%d0
	and.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_a7			/* res 4,a */
	and.b	#~0x10,%d7
	next_pc
	.endm

.macro	cb_a8			/* res 5,b */
	moveq	#21,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_a9			/* res 5,c */
	and.b	#~0x20,%d5
	next_pc
	.endm

.macro	cb_aa			/* res 5,d */
	moveq	#21,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_ab			/* res 5,e */
	and.b	#~0x20,%d4
	next_pc
	.endm

.macro	cb_ac			/* res 5,h */
	moveq	#21,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_ad			/* res 5,l */
	and.b	#~0x20,%d3
	next_pc
	.endm

.macro	cb_ae			/* res 5,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#~0x20,%d0
	and.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_af			/* res 5,a */
	and.b	#~0x20,%d7
	next_pc
	.endm

.macro	cb_b0			/* res 6,b */
	moveq	#22,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_b1			/* res 6,c */
	and.b	#~0x40,%d5
	next_pc
	.endm

.macro	cb_b2			/* res 6,d */
	moveq	#22,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_b3			/* res 6,e */
	and.b	#~0x40,%d4
	next_pc
	.endm

.macro	cb_b4			/* res 6,h */
	moveq	#22,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_b5			/* res 6,l */
	and.b	#~0x40,%d3
	next_pc
	.endm

.macro	cb_b6			/* res 6,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#~0x40,%d0
	and.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_b7			/* res 6,a */
	and.b	#~0x40,%d7
	next_pc
	.endm

.macro	cb_b8			/* res 7,b */
	moveq	#23,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_b9			/* res 7,c */
	and.b	#0xff^0x80,%d5
	next_pc
	.endm

.macro	cb_ba			/* res 7,d */
	moveq	#23,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_bb			/* res 7,e */
	and.b	#0xff^0x80,%d4
	next_pc
	.endm

.macro	cb_bc			/* res 7,h */
	moveq	#23,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_bd			/* res 7,l */
	and.b	#0xff^0x80,%d3
	next_pc
	.endm

.macro	cb_be			/* res 7,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#0xff^0x80,%d0
	and.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_bf			/* res 7,a */
	and.b	#0xff^0x80,%d7
	next_pc
	.endm

.macro	cb_c0			/* set 0,b */
	moveq	#16,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_c1			/* set 0,c */
	or.b	#0x01,%d5
	next_pc
	.endm

.macro	cb_c2			/* set 0,d */
	moveq	#16,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_c3			/* set 0,e */
	or.b	#0x01,%d4
	next_pc
	.endm

.macro	cb_c4			/* set 0,h */
	moveq	#16,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_c5			/* set 0,l */
	or.b	#0x01,%d3
	next_pc
	.endm

.macro	cb_c6			/* set 0,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#0x01,%d0
	or.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_c7			/* set 0,a */
	or.b	#0x01,%d7
	next_pc
	.endm

.macro	cb_c8			/* set 1,b */
	moveq	#17,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_c9			/* set 1,c */
	or.b	#0x02,%d5
	next_pc
	.endm

.macro	cb_ca			/* set 1,d */
	moveq	#17,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_cb			/* set 1,e */
	or.b	#0x02,%d4
	next_pc
	.endm

.macro	cb_cc			/* set 1,h */
	moveq	#17,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_cd			/* set 1,l */
	or.b	#0x02,%d3
	next_pc
	.endm

.macro	cb_ce			/* set 1,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#0x02,%d0
	or.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_cf			/* set 1,a */
	or.b	#0x02,%d7
	next_pc
	.endm

.macro	cb_d0			/* set 2,b */
	moveq	#18,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_d1			/* set 2,c */
	or.b	#0x04,%d5
	next_pc
	.endm

.macro	cb_d2			/* set 2,d */
	moveq	#18,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_d3			/* set 2,e */
	or.b	#0x04,%d4
	next_pc
	.endm

.macro	cb_d4			/* set 2,h */
	moveq	#18,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_d5			/* set 2,l */
	or.b	#0x04,%d3
	next_pc
	.endm

.macro	cb_d6			/* set 2,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#0x04,%d0
	or.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_d7			/* set 2,a */
	or.b	#0x04,%d7
	next_pc
	.endm

.macro	cb_d8			/* set 3,b */
	moveq	#19,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_d9			/* set 3,c */
	or.b	#0x08,%d5
	next_pc
	.endm

.macro	cb_da			/* set 3,d */
	moveq	#19,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_db			/* set 3,e */
	or.b	#0x08,%d4
	next_pc
	.endm

.macro	cb_dc			/* set 3,h */
	moveq	#19,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_dd			/* set 3,l */
	or.b	#0x08,%d3
	next_pc
	.endm

.macro	cb_de			/* set 3,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#0x08,%d0
	or.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_df			/* set 3,a */
	or.b	#0x08,%d7
	next_pc
	.endm

.macro	cb_e0			/* set 4,b */
	moveq	#20,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_e1			/* set 4,c */
	or.b	#0x10,%d5
	next_pc
	.endm

.macro	cb_e2			/* set 4,d */
	moveq	#20,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_e3			/* set 4,e */
	or.b	#0x10,%d4
	next_pc
	.endm

.macro	cb_e4			/* set 4,h */
	moveq	#20,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_e5			/* set 4,l */
	or.b	#0x10,%d3
	next_pc
	.endm

.macro	cb_e6			/* set 4,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#0x10,%d0
	or.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_e7			/* set 4,a */
	or.b	#0x10,%d7
	next_pc
	.endm

.macro	cb_e8			/* set 5,b */
	moveq	#21,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_e9			/* set 5,c */
	or.b	#0x20,%d5
	next_pc
	.endm

.macro	cb_ea			/* set 5,d */
	moveq	#21,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_eb			/* set 5,e */
	or.b	#0x20,%d4
	next_pc
	.endm

.macro	cb_ec			/* set 5,h */
	moveq	#21,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_ed			/* set 5,l */
	or.b	#0x20,%d3
	next_pc
	.endm

.macro	cb_ee			/* set 5,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#0x20,%d0
	or.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_ef			/* set 5,a */
	or.b	#0x20,%d7
	next_pc
	.endm

.macro	cb_f0			/* set 6,b */
	moveq	#22,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_f1			/* set 6,c */
	or.b	#0x40,%d5
	next_pc
	.endm

.macro	cb_f2			/* set 6,d */
	moveq	#22,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_f3			/* set 6,e */
	or.b	#0x40,%d4
	next_pc
	.endm

.macro	cb_f4			/* set 6,h */
	moveq	#22,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_f5			/* set 6,l */
	or.b	#0x40,%d3
	next_pc
	.endm

.macro	cb_f6			/* set 6,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#0x40,%d0
	or.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_f7			/* set 6,a */
	or.b	#0x40,%d7
	next_pc
	.endm

.macro	cb_f8			/* set 7,b */
	moveq	#23,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_f9			/* set 7,c */
	or.b	#0x80,%d5
	next_pc
	.endm

.macro	cb_fa			/* set 7,d */
	moveq	#23,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_fb			/* set 7,e */
	or.b	#0x80,%d4
	next_pc
	.endm

.macro	cb_fc			/* set 7,h */
	moveq	#23,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_fd			/* set 7,l */
	or.b	#0x80,%d3
	next_pc
	.endm

.macro	cb_fe			/* set 7,(hl) */
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	moveq	#-0x80,%d0
	or.b	(%a3),%d0
	mem_wr	%d2
	next_pc
	.endm

.macro	cb_ff			/* set 7,a */
	or.b	#0x80,%d7
	next_pc
	.endm

/* === The end ========================================================	*/
