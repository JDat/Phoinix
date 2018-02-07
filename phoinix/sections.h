/************************************************************************
 *  sections.h
 *                      Code sections
 *
 ************************************************************************
 *
 * Phoinix,
 * Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
 *
 * (c)2000-2005 Bodo Wenzel
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
 *  $Log: sections.h,v $
 *  Revision 1.2  2005/02/27 16:38:24  bodowenzel
 *  Added module Eggs for easter eggs
 *
 *  Revision 1.1  2004/12/28 13:56:35  bodowenzel
 *  Split up all C-Code to multi-segmented
 *
 ************************************************************************
 */

#ifndef SECTIONS_H
#define SECTIONS_H

/* === Attributes placing code in sections ============================	*/

/* no attributes, the standard section code0001 */
#define EGGS_SECTION
#define MANAGER_SECTION
#define MISC_SECTION
#define PHOINIX_SECTION
#define PREFS_SECTION
#define RAM_SECTION
#define STATES_SECTION
#define VFS_SECTION

/* emulator kernel */
#define GBEMU_SECTION __attribute((section ("kernel")))

/* emulator user interface and support */
#define EMULATION_SECTION __attribute((section ("emulator")))
#define MEMORY_SECTION    __attribute((section ("emulator")))
#define VIDEO_SECTION     __attribute((section ("emulator")))

/* === Enumerating code sections ======================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

enum {
  codeIdJumpTable = 0, /* PalmOS standard section */
  codeIdStandard,      /* PalmOS standard section */
  codeIdKernel,        /* Emulator kernel */
  codeIdEmulator,      /* Emulator user interface and support */
};

#endif

/* === The end ========================================================	*/

#endif
