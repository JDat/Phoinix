/************************************************************************
 *  Phoinix.ld
 *                      Linker script
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
 *  $Log: Phoinix.ld,v $
 *  Revision 1.4  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 *  Revision 1.3  2002/10/19 08:08:10  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:30  bodowenzel
 *  Import
 *
 ************************************************************************
 */

/* Only additional code resources need to be defined. */

MEMORY
{
  kernelres   : ORIGIN = 0x0, LENGTH = 0xFFD0
  emulatorres : ORIGIN = 0x0, LENGTH = 0x8000
}

SECTIONS
{
  /* Section entries for .text, .data, and .bss necessary to force
   * correct section order in linker output with m68k-palmos-ld in
   * prc-tools-2.2 and later.
   */
  .text    : { *(.text)    } > coderes
  .data    : { *(.data)    } > datares
  .bss     : { *(.bss)     } > datares
  kernel   : { *(kernel)   } > kernelres
  emulator : { *(emulator) } > emulatorres
}

/* === The end ========================================================	*/
