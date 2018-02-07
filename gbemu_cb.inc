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
 *  $Log: gbemu_cb.inc,v $
 *  Revision 1.6  2007/07/01 10:12:51  bodowenzel
 *  Cleanup
 *
 *  Revision 1.5  2007/01/03 10:24:44  bodowenzel
 *  Optimized swap flag evaluation
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
cb_00:
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
cb_01:
	swap	%d6
	move.b	%d5,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_02			/* rlc d */
cb_02:
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
cb_03:
	swap	%d6
	move.b	%d4,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_04			/* rlc h */
cb_04:
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
cb_05:
	swap	%d6
	move.b	%d3,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_06			/* rlc (hl) */
cb_06:
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
cb_07:
	swap	%d6
	move.b	%d7,%d0
	roxl.b	#1,%d0
	rol.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_08			/* rrc b */
cb_08:
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
cb_09:
	swap	%d6
	move.b	%d5,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_0a			/* rrc d */
cb_0a:
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
cb_0b:
	swap	%d6
	move.b	%d4,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_0c			/* rrc h */
cb_0c:
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
cb_0d:
	swap	%d6
	move.b	%d3,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_0e			/* rrc (hl) */
cb_0e:
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
cb_0f:
	swap	%d6
	move.b	%d7,%d0
	roxr.b	#1,%d0
	ror.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_10			/* rl b */
cb_10:
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
cb_11:
	swap	%d6
	move	%d6,%ccr
	roxl.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_12			/* rl d */
cb_12:
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
cb_13:
	swap	%d6
	move	%d6,%ccr
	roxl.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_14			/* rl h */
cb_14:
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
cb_15:
	swap	%d6
	move	%d6,%ccr
	roxl.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_16			/* rl (hl) */
cb_16:
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
cb_17:
	swap	%d6
	move	%d6,%ccr
	roxl.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_18			/* rr b */
cb_18:
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
cb_19:
	swap	%d6
	move	%d6,%ccr
	roxr.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_1a			/* rr d */
cb_1a:
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
cb_1b:
	swap	%d6
	move	%d6,%ccr
	roxr.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_1c			/* rr h */
cb_1c:
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
cb_1d:
	swap	%d6
	move	%d6,%ccr
	roxr.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_1e			/* rr (hl) */
cb_1e:
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
cb_1f:
	swap	%d6
	move	%d6,%ccr
	roxr.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_20			/* sla b */
cb_20:
	swap	%d6
	swap	%d5
	asl.b	#1,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_21			/* sla c */
cb_21:
	swap	%d6
	asl.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_22			/* sla d */
cb_22:
	swap	%d6
	swap	%d4
	asl.b	#1,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_23			/* sla e */
cb_23:
	swap	%d6
	asl.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_24			/* sla h */
cb_24:
	swap	%d6
	swap	%d3
	asl.b	#1,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_25			/* sla l */
cb_25:
	swap	%d6
	asl.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_26			/* sla (hl) */
cb_26:
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
cb_27:
	swap	%d6
	asl.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_28			/* sra b */
cb_28:
	swap	%d6
	swap	%d5
	asr.b	#1,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_29			/* sra c */
cb_29:
	swap	%d6
	asr.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_2a			/* sra d */
cb_2a:
	swap	%d6
	swap	%d4
	asr.b	#1,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_2b			/* sra e */
cb_2b:
	swap	%d6
	asr.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_2c			/* sra h */
cb_2c:
	swap	%d6
	swap	%d3
	asr.b	#1,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_2d			/* sra l */
cb_2d:
	swap	%d6
	asr.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_2e			/* sra (hl) */
cb_2e:
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
cb_2f:
	swap	%d6
	asr.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_30			/* swap b */
cb_30:
	swap	%d6
	swap	%d5
	rol.b	#4,%d5
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_31			/* swap c */
cb_31:
	swap	%d6
	rol.b	#4,%d5
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_32			/* swap d */
cb_32:
	swap	%d6
	swap	%d4
	rol.b	#4,%d4
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_33			/* swap e */
cb_33:
	swap	%d6
	rol.b	#4,%d4
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_34			/* swap h */
cb_34:
	swap	%d6
	swap	%d3
	rol.b	#4,%d3
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_35			/* swap l */
cb_35:
	swap	%d6
	rol.b	#4,%d3
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_36			/* swap (hl) */
cb_36:
	swap	%d6
	move.b	%d3,%d1
	move.l	%d3,%d2
	swap	%d2
	mem_rd	%d2
	move.b	(%a3),%d0
	rol.b	#4,%d0
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	mem_wr	%d2
	swap	%d6
	next_pc
	.endm

.macro	cb_37			/* swap a */
cb_37:
	swap	%d6
	rol.b	#4,%d7
	move	%sr,%d6
	and.b	#~m68kMaskC,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_38			/* srl b */
cb_38:
	swap	%d6
	swap	%d5
	lsr.b	#1,%d5
	move	%sr,%d6
	swap	%d5
	swap	%d6
	next_pc
	.endm

.macro	cb_39			/* srl c */
cb_39:
	swap	%d6
	lsr.b	#1,%d5
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_3a			/* srl d */
cb_3a:
	swap	%d6
	swap	%d4
	lsr.b	#1,%d4
	move	%sr,%d6
	swap	%d4
	swap	%d6
	next_pc
	.endm

.macro	cb_3b			/* srl e */
cb_3b:
	swap	%d6
	lsr.b	#1,%d4
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_3c			/* srl h */
cb_3c:
	swap	%d6
	swap	%d3
	lsr.b	#1,%d3
	move	%sr,%d6
	swap	%d3
	swap	%d6
	next_pc
	.endm

.macro	cb_3d			/* srl l */
cb_3d:
	swap	%d6
	lsr.b	#1,%d3
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_3e			/* srl (hl) */
cb_3e:
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
cb_3f:
	swap	%d6
	lsr.b	#1,%d7
	move	%sr,%d6
	swap	%d6
	next_pc
	.endm

.macro	cb_40			/* bit 0,b */
cb_40:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#16,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_41			/* bit 0,c */
cb_41:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#0,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_42			/* bit 0,d */
cb_42:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#16,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_43			/* bit 0,e */
cb_43:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#0,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_44			/* bit 0,h */
cb_44:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#16,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_45			/* bit 0,l */
cb_45:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#0,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_46			/* bit 0,(hl) */
cb_46:
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
cb_47:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#0,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_48			/* bit 1,b */
cb_48:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#17,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_49			/* bit 1,c */
cb_49:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#1,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4a			/* bit 1,d */
cb_4a:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#17,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4b			/* bit 1,e */
cb_4b:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#1,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4c			/* bit 1,h */
cb_4c:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#17,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4d			/* bit 1,l */
cb_4d:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#1,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_4e			/* bit 1,(hl) */
cb_4e:
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
cb_4f:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#1,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_50			/* bit 2,b */
cb_50:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#18,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_51			/* bit 2,c */
cb_51:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#2,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_52			/* bit 2,d */
cb_52:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#18,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_53			/* bit 2,e */
cb_53:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#2,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_54			/* bit 2,h */
cb_54:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#18,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_55			/* bit 2,l */
cb_55:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#2,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_56			/* bit 2,(hl) */
cb_56:
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
cb_57:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#2,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_58			/* bit 1,b */
cb_58:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#19,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_59			/* bit 3,c */
cb_59:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#3,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5a			/* bit 3,d */
cb_5a:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#19,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5b			/* bit 3,e */
cb_5b:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#3,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5c			/* bit 3,h */
cb_5c:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#19,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5d			/* bit 3,l */
cb_5d:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#3,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_5e			/* bit 3,(hl) */
cb_5e:
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
cb_5f:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#3,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_60			/* bit 4,b */
cb_60:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#20,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_61			/* bit 4,c */
cb_61:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#4,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_62			/* bit 4,d */
cb_62:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#20,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_63			/* bit 4,e */
cb_63:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#4,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_64			/* bit 4,h */
cb_64:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#20,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_65			/* bit 4,l */
cb_65:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#4,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_66			/* bit 4,(hl) */
cb_66:
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
cb_67:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#4,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_68			/* bit 5,b */
cb_68:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#21,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_69			/* bit 5,c */
cb_69:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#5,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6a			/* bit 5,d */
cb_6a:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#21,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6b			/* bit 5,e */
cb_6b:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#5,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6c			/* bit 5,h */
cb_6c:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#21,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6d			/* bit 5,l */
cb_6d:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#5,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_6e			/* bit 5,(hl) */
cb_6e:
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
cb_6f:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#5,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_70			/* bit 6,b */
cb_70:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#22,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_71			/* bit 6,c */
cb_71:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#6,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_72			/* bit 6,d */
cb_72:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#22,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_73			/* bit 6,e */
cb_73:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#6,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_74			/* bit 6,h */
cb_74:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#22,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_75			/* bit 6,l */
cb_75:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#6,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_76			/* bit 6,(hl) */
cb_76:
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
cb_77:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#6,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_78			/* bit 7,b */
cb_78:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#23,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_79			/* bit 7,c */
cb_79:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#7,%d5
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7a			/* bit 7,d */
cb_7a:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#23,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7b			/* bit 7,e */
cb_7b:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#7,%d4
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7c			/* bit 7,h */
cb_7c:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#23,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7d			/* bit 7,l */
cb_7d:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#7,%d3
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_7e			/* bit 7,(hl) */
cb_7e:
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
cb_7f:
	moveq	#m68kFlagZ,%d0
	bset	%d0,%d6
	btst	#7,%d7
	jeq	z\@
	bchg	%d0,%d6
z\@:	next_pc
	.endm

.macro	cb_80			/* res 0,b */
cb_80:
	moveq	#16,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_81			/* res 0,c */
cb_81:
	and.b	#~0x01,%d5
	next_pc
	.endm

.macro	cb_82			/* res 0,d */
cb_82:
	moveq	#16,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_83			/* res 0,e */
cb_83:
	and.b	#~0x01,%d4
	next_pc
	.endm

.macro	cb_84			/* res 0,h */
cb_84:
	moveq	#16,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_85			/* res 0,l */
cb_85:
	and.b	#~0x01,%d3
	next_pc
	.endm

.macro	cb_86			/* res 0,(hl) */
cb_86:
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
cb_87:
	and.b	#~0x01,%d7
	next_pc
	.endm

.macro	cb_88			/* res 1,b */
cb_88:
	moveq	#17,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_89			/* res 1,c */
cb_89:
	and.b	#~0x02,%d5
	next_pc
	.endm

.macro	cb_8a			/* res 1,d */
cb_8a:
	moveq	#17,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_8b			/* res 1,e */
cb_8b:
	and.b	#~0x02,%d4
	next_pc
	.endm

.macro	cb_8c			/* res 1,h */
cb_8c:
	moveq	#17,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_8d			/* res 1,l */
cb_8d:
	and.b	#~0x02,%d3
	next_pc
	.endm

.macro	cb_8e			/* res 1,(hl) */
cb_8e:
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
cb_8f:
	and.b	#~0x02,%d7
	next_pc
	.endm

.macro	cb_90			/* res 2,b */
cb_90:
	moveq	#18,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_91			/* res 2,c */
cb_91:
	and.b	#~0x04,%d5
	next_pc
	.endm

.macro	cb_92			/* res 2,d */
cb_92:
	moveq	#18,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_93			/* res 2,e */
cb_93:
	and.b	#~0x04,%d4
	next_pc
	.endm

.macro	cb_94			/* res 2,h */
cb_94:
	moveq	#18,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_95			/* res 2,l */
cb_95:
	and.b	#~0x04,%d3
	next_pc
	.endm

.macro	cb_96			/* res 2,(hl) */
cb_96:
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
cb_97:
	and.b	#~0x04,%d7
	next_pc
	.endm

.macro	cb_98			/* res 3,b */
cb_98:
	moveq	#19,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_99			/* res 3,c */
cb_99:
	and.b	#~0x08,%d5
	next_pc
	.endm

.macro	cb_9a			/* res 3,d */
cb_9a:
	moveq	#19,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_9b			/* res 3,e */
cb_9b:
	and.b	#~0x08,%d4
	next_pc
	.endm

.macro	cb_9c			/* res 3,h */
cb_9c:
	moveq	#19,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_9d			/* res 3,l */
cb_9d:
	and.b	#~0x08,%d3
	next_pc
	.endm

.macro	cb_9e			/* res 3,(hl) */
cb_9e:
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
cb_9f:
	and.b	#~0x08,%d7
	next_pc
	.endm

.macro	cb_a0			/* res 4,b */
cb_a0:
	moveq	#20,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_a1			/* res 4,c */
cb_a1:
	and.b	#~0x10,%d5
	next_pc
	.endm

.macro	cb_a2			/* res 4,d */
cb_a2:
	moveq	#20,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_a3			/* res 4,e */
cb_a3:
	and.b	#~0x10,%d4
	next_pc
	.endm

.macro	cb_a4			/* res 4,h */
cb_a4:
	moveq	#20,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_a5			/* res 4,l */
cb_a5:
	and.b	#~0x10,%d3
	next_pc
	.endm

.macro	cb_a6			/* res 4,(hl) */
cb_a6:
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
cb_a7:
	and.b	#~0x10,%d7
	next_pc
	.endm

.macro	cb_a8			/* res 5,b */
cb_a8:
	moveq	#21,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_a9			/* res 5,c */
cb_a9:
	and.b	#~0x20,%d5
	next_pc
	.endm

.macro	cb_aa			/* res 5,d */
cb_aa:
	moveq	#21,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_ab			/* res 5,e */
cb_ab:
	and.b	#~0x20,%d4
	next_pc
	.endm

.macro	cb_ac			/* res 5,h */
cb_ac:
	moveq	#21,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_ad			/* res 5,l */
cb_ad:
	and.b	#~0x20,%d3
	next_pc
	.endm

.macro	cb_ae			/* res 5,(hl) */
cb_ae:
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
cb_af:
	and.b	#~0x20,%d7
	next_pc
	.endm

.macro	cb_b0			/* res 6,b */
cb_b0:
	moveq	#22,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_b1			/* res 6,c */
cb_b1:
	and.b	#~0x40,%d5
	next_pc
	.endm

.macro	cb_b2			/* res 6,d */
cb_b2:
	moveq	#22,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_b3			/* res 6,e */
cb_b3:
	and.b	#~0x40,%d4
	next_pc
	.endm

.macro	cb_b4			/* res 6,h */
cb_b4:
	moveq	#22,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_b5			/* res 6,l */
cb_b5:
	and.b	#~0x40,%d3
	next_pc
	.endm

.macro	cb_b6			/* res 6,(hl) */
cb_b6:
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
cb_b7:
	and.b	#~0x40,%d7
	next_pc
	.endm

.macro	cb_b8			/* res 7,b */
cb_b8:
	moveq	#23,%d0
	bclr	%d0,%d5
	next_pc
	.endm

.macro	cb_b9			/* res 7,c */
cb_b9:
	and.b	#0xff^0x80,%d5
	next_pc
	.endm

.macro	cb_ba			/* res 7,d */
cb_ba:
	moveq	#23,%d0
	bclr	%d0,%d4
	next_pc
	.endm

.macro	cb_bb			/* res 7,e */
cb_bb:
	and.b	#0xff^0x80,%d4
	next_pc
	.endm

.macro	cb_bc			/* res 7,h */
cb_bc:
	moveq	#23,%d0
	bclr	%d0,%d3
	next_pc
	.endm

.macro	cb_bd			/* res 7,l */
cb_bd:
	and.b	#0xff^0x80,%d3
	next_pc
	.endm

.macro	cb_be			/* res 7,(hl) */
cb_be:
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
cb_bf:
	and.b	#0xff^0x80,%d7
	next_pc
	.endm

.macro	cb_c0			/* set 0,b */
cb_c0:
	moveq	#16,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_c1			/* set 0,c */
cb_c1:
	or.b	#0x01,%d5
	next_pc
	.endm

.macro	cb_c2			/* set 0,d */
cb_c2:
	moveq	#16,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_c3			/* set 0,e */
cb_c3:
	or.b	#0x01,%d4
	next_pc
	.endm

.macro	cb_c4			/* set 0,h */
cb_c4:
	moveq	#16,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_c5			/* set 0,l */
cb_c5:
	or.b	#0x01,%d3
	next_pc
	.endm

.macro	cb_c6			/* set 0,(hl) */
cb_c6:
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
cb_c7:
	or.b	#0x01,%d7
	next_pc
	.endm

.macro	cb_c8			/* set 1,b */
cb_c8:
	moveq	#17,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_c9			/* set 1,c */
cb_c9:
	or.b	#0x02,%d5
	next_pc
	.endm

.macro	cb_ca			/* set 1,d */
cb_ca:
	moveq	#17,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_cb			/* set 1,e */
cb_cb:
	or.b	#0x02,%d4
	next_pc
	.endm

.macro	cb_cc			/* set 1,h */
cb_cc:
	moveq	#17,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_cd			/* set 1,l */
cb_cd:
	or.b	#0x02,%d3
	next_pc
	.endm

.macro	cb_ce			/* set 1,(hl) */
cb_ce:
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
cb_cf:
	or.b	#0x02,%d7
	next_pc
	.endm

.macro	cb_d0			/* set 2,b */
cb_d0:
	moveq	#18,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_d1			/* set 2,c */
cb_d1:
	or.b	#0x04,%d5
	next_pc
	.endm

.macro	cb_d2			/* set 2,d */
cb_d2:
	moveq	#18,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_d3			/* set 2,e */
cb_d3:
	or.b	#0x04,%d4
	next_pc
	.endm

.macro	cb_d4			/* set 2,h */
cb_d4:
	moveq	#18,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_d5			/* set 2,l */
cb_d5:
	or.b	#0x04,%d3
	next_pc
	.endm

.macro	cb_d6			/* set 2,(hl) */
cb_d6:
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
cb_d7:
	or.b	#0x04,%d7
	next_pc
	.endm

.macro	cb_d8			/* set 3,b */
cb_d8:
	moveq	#19,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_d9			/* set 3,c */
cb_d9:
	or.b	#0x08,%d5
	next_pc
	.endm

.macro	cb_da			/* set 3,d */
cb_da:
	moveq	#19,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_db			/* set 3,e */
cb_db:
	or.b	#0x08,%d4
	next_pc
	.endm

.macro	cb_dc			/* set 3,h */
cb_dc:
	moveq	#19,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_dd			/* set 3,l */
cb_dd:
	or.b	#0x08,%d3
	next_pc
	.endm

.macro	cb_de			/* set 3,(hl) */
cb_de:
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
cb_df:
	or.b	#0x08,%d7
	next_pc
	.endm

.macro	cb_e0			/* set 4,b */
cb_e0:
	moveq	#20,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_e1			/* set 4,c */
cb_e1:
	or.b	#0x10,%d5
	next_pc
	.endm

.macro	cb_e2			/* set 4,d */
cb_e2:
	moveq	#20,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_e3			/* set 4,e */
cb_e3:
	or.b	#0x10,%d4
	next_pc
	.endm

.macro	cb_e4			/* set 4,h */
cb_e4:
	moveq	#20,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_e5			/* set 4,l */
cb_e5:
	or.b	#0x10,%d3
	next_pc
	.endm

.macro	cb_e6			/* set 4,(hl) */
cb_e6:
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
cb_e7:
	or.b	#0x10,%d7
	next_pc
	.endm

.macro	cb_e8			/* set 5,b */
cb_e8:
	moveq	#21,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_e9			/* set 5,c */
cb_e9:
	or.b	#0x20,%d5
	next_pc
	.endm

.macro	cb_ea			/* set 5,d */
cb_ea:
	moveq	#21,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_eb			/* set 5,e */
cb_eb:
	or.b	#0x20,%d4
	next_pc
	.endm

.macro	cb_ec			/* set 5,h */
cb_ec:
	moveq	#21,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_ed			/* set 5,l */
cb_ed:
	or.b	#0x20,%d3
	next_pc
	.endm

.macro	cb_ee			/* set 5,(hl) */
cb_ee:
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
cb_ef:
	or.b	#0x20,%d7
	next_pc
	.endm

.macro	cb_f0			/* set 6,b */
cb_f0:
	moveq	#22,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_f1			/* set 6,c */
cb_f1:
	or.b	#0x40,%d5
	next_pc
	.endm

.macro	cb_f2			/* set 6,d */
cb_f2:
	moveq	#22,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_f3			/* set 6,e */
cb_f3:
	or.b	#0x40,%d4
	next_pc
	.endm

.macro	cb_f4			/* set 6,h */
cb_f4:
	moveq	#22,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_f5			/* set 6,l */
cb_f5:
	or.b	#0x40,%d3
	next_pc
	.endm

.macro	cb_f6			/* set 6,(hl) */
cb_f6:
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
cb_f7:
	or.b	#0x40,%d7
	next_pc
	.endm

.macro	cb_f8			/* set 7,b */
cb_f8:
	moveq	#23,%d0
	bset	%d0,%d5
	next_pc
	.endm

.macro	cb_f9			/* set 7,c */
cb_f9:
	or.b	#0x80,%d5
	next_pc
	.endm

.macro	cb_fa			/* set 7,d */
cb_fa:
	moveq	#23,%d0
	bset	%d0,%d4
	next_pc
	.endm

.macro	cb_fb			/* set 7,e */
cb_fb:
	or.b	#0x80,%d4
	next_pc
	.endm

.macro	cb_fc			/* set 7,h */
cb_fc:
	moveq	#23,%d0
	bset	%d0,%d3
	next_pc
	.endm

.macro	cb_fd			/* set 7,l */
cb_fd:
	or.b	#0x80,%d3
	next_pc
	.endm

.macro	cb_fe			/* set 7,(hl) */
cb_fe:
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
cb_ff:
	or.b	#0x80,%d7
	next_pc
	.endm

/* === The end ========================================================	*/
