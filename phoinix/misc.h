/************************************************************************
 *  misc.h
 *                      Miscellaneous functions
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
 *  Revision 1.4  2002/10/23 16:34:35  bodowenzel
 *  Busy form centralized
 *
 *  Revision 1.3  2002/10/19 08:08:11  bodowenzel
 *  History cleanup, Javadoc-style header
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:38:10  bodowenzel
 *  Import
 *
 ************************************************************************
 */

#ifndef MISC_H
#define MISC_H

/* === Constants ======================================================	*/

#define miscCreator 'Phnx'

/* === Function prototypes ============================================	*/

Err MiscRomVersionCompatible(UInt32 minV, UInt32 maxV,
			     Int16 launchFlags);

Boolean MiscNoGameFormHandleEvent(EventType *evtP);

void MiscShowAbout(void);
void MiscShowBusy(Boolean on);

/* === The end ========================================================	*/

#endif
