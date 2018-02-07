/************************************************************************
 *  sections.S
 *                      Code sections
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
 *  $Log: sections.S,v $
 *  Revision 1.1  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 ************************************************************************
 */

/* === Global variables ===============================================	*/

	.globl		__text__
	.lcomm		__text__,4
	.globl		__text__kernel
	.lcomm		__text__kernel,4
	.globl		__text__emulator
	.lcomm		__text__emulator,4

/* === Support for relocating =========================================	*/

	.text
	.globl		_GccRelocateData
_GccRelocateData:
	bra.w		_GccLoadCodeAndRelocateData

	.section	ehook
	.long		_GccReleaseCode

/* === The end ========================================================	*/
