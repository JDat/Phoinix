/************************************************************************
 *  manager.h
 *                      Game manager
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
 *  Revision 1.3  2002/05/10 15:17:39  bodowenzel
 *  Till's VFS contribution
 *
 *  Revision 1.2  2001/12/30 18:47:11  bodowenzel
 *  CVS keyword Log was faulty
 *
 *  Revision 1.1.1.1  2001/12/16 13:39:04  bodowenzel
 *  Import
 *
 ************************************************************************
 */

#ifndef MANAGER_H
#define MANAGER_H

/* === Constants ===============================================	*/

#define gameDbType 'ROMU'

/* === Type definitions ===============================================	*/

/* === Global and static variables ====================================	*/

typedef struct {
  UInt16 cardNo, volIdx;
  Char   name[dmDBNameLength];
} ManagerDbListEntryType;

/* === Function prototypes ============================================	*/

void ManagerExit(void);
Boolean ManagerFormHandleEvent(EventType *evtP);
void ManagerListChangeLocation(UInt16 loc, UInt16 card);
Boolean ManagerListAdd(DmOpenRef db, Char *name, UInt16 card, UInt16 vol);

/* === The end ========================================================	*/

#endif
