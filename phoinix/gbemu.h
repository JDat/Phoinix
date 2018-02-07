/************************************************************************
 *  gbemu.h
 *                      Emulation of the Gameboy CPU
 *
 ************************************************************************
 *
 * Phoinix,
 * Nintendo Gameboy(TM) emulator for the Palm OS(R) Computing Platform
 *
 * (c)2000-2006 Bodo Wenzel
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
 *  $Log: gbemu.h,v $
 *  Revision 1.5  2006/10/23 15:38:53  bodowenzel
 *  Support for Rana
 *
 *  Revision 1.4  2005/01/30 19:33:15  bodowenzel
 *  Added index for last unused extension
 *
 *  Revision 1.3  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:14  bodowenzel
 *  Import
 *
 ************************************************************************
 *  Pre-CVS History (developed as PalmBoy):
 *
 *  2001-09-22  Bodo Wenzel   Some optimization
 *  2001-09-16  Bodo Wenzel   Saving states now possible
 *  2001-07-23  Bodo Wenzel   Changed syntax to CPP + GAS
 *  2001-02-05  Bodo Wenzel   New stack management
 *  2001-01-31  Bodo Wenzel   Support of more cartridges
 *  2000-11-16  Bodo Wenzel   New screen rendering and
 *                            better joypad emulation
 *  2000-10-22  Bodo Wenzel   Interrupts and timer
 *  2000-09-30  Bodo Wenzel   PalmBoy scroller as GB code
 *  2000-09-09  Bodo Wenzel   Now nearly full screen emulation
 *  2000-05-06  Bodo Wenzel   External RAM pageable
 *  2000-05-04  Bodo Wenzel   Changes to meet Pila requirements
 *  2000-04-28  Bodo Wenzel   Creation as "constants.h"
 ************************************************************************
 */

#ifndef GBEMU_H
#define GBEMU_H

/* === Constants ======================================================	*/

#define gbemuRetUserStop      (1)
#define gbemuRetOpcodeStop    (2)
#define gbemuRetOpcodeUnknown (3)

/* These sizes have to be found by experiment, sorry */
#define gbemuOpSize   (0x50)
#define gbemuCbSize   (0x3c)
#define gbemuRdSize   (0x0e)
#define gbemuWrSize   (0x10)
#define gbemuInSize   (0x0e)
#define gbemuOutSize  (0x12)
#define gbemuXtOffset (gbemuOpSize + gbemuCbSize + gbemuRdSize + \
                       gbemuWrSize + gbemuInSize + gbemuOutSize)
#define gbemuXtSize   (0x100 - gbemuXtOffset)
#if gbemuXtOffset > 0xd0
#error "Sizes of combo elements too big, limited by PalmOS!"
#endif

/* === Type definitions ===============================================	*/

#ifdef __GNUC__ /* only if C preprocessing */

typedef struct {
  UInt8 opCode[gbemuOpSize];            /* see files "gbemu*.*" */

  UInt8 cbCode[gbemuCbSize];

  UInt8 rdCode[gbemuRdSize];

  UInt8 wrCode[gbemuWrSize];

  union {
    struct {
      UInt16 jmpOpcode;                 /* opcode for jump */
      void   *address;                  /* address to jump at */
    }     vector;
    UInt8 code[gbemuInSize];
  }     in;

  union {
    struct {
      UInt16 jmpOpcode;                 /* opcode for jump */
      void   *address;                  /* address to jump at */
    }     vector;
    UInt8 code[gbemuOutSize];
  }     out;

  union {
    struct {
      UInt16 jmpOpcode;                 /* opcode for jump */
      void   *address;                  /* address to jump at */
      void   *buffer;                   /* buffer for address (mbc3) */
    }     vector;
    UInt8 code[gbemuXtSize];
  }     xt;
} GbemuComboType;

#endif

/* === Indices of extensions ==========================================	*/

#define gbemuLastUnused        (0x57)

#define gbemuJmpOpcodeStop     (0x58)

#define gbemuReturn            (0x59)

#define gbemuCodeCheckInt      (0x5a) /* .. (0x5c) */

#define gbemuJmpRamEnable      (0x5d)
#define gbemuJmpRomSelectL     (0x5e)
#define gbemuJmpRomSelectH     (0x5f)
#define gbemuJmpRamSelect      (0x60)
#define gbemuJmpModeSelect     (0x61)
#define gbemuJmpRamWr8087      (0x62)
#define gbemuJmpRamWr888f      (0x63)
#define gbemuJmpRamWr9097      (0x64)
#define gbemuJmpRamWr989b      (0x65)
#define gbemuJmpRamWr9c9f      (0x66)
#define gbemuJmpRamRdExt       (0x67)
#define gbemuJmpRamWrExt       (0x68)
#define gbemuJmpRamWrOam       (0x69)

#define gbemuJmpLcdLine        (0x6a)
#define gbemuJmpIoWrDma        (0x6b)

#define gbemuCodeSpPushCorrect (0x6c)
#define gbemuCodeSpPopCorrect  (0x6d)
#define gbemuCodeSpRomCorrect  (0x6e)
#define gbemuCodeSpRamCorrect  (0x6f)

#define gbemuGbCodeReset       (0x70) /* .. (0x7d) */
#define gbemuGbCodeRestart     (0x7e)

/* === Macros =========================================================	*/

/* transform combo index to offset used by assembler statements */
#define indexToAddress(index) 0x0100*(index)+gbemuXtOffset(%a4)

/* === The end ========================================================	*/

#endif
