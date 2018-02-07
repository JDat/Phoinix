/************************************************************************
 *  eggs.h
 *                      Easter eggs
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
 *  $Log: eggs.h,v $
 *  Revision 1.1  2005/02/27 16:38:24  bodowenzel
 *  Added module Eggs for easter eggs
 *
 ************************************************************************
 */

#ifndef EGGS_H
#define EGGS_H

/* === Includes =======================================================	*/

#include "sections.h"

/* === Function prototypes ============================================	*/

void EggsHandleKey(WChar key)
  EGGS_SECTION;

void EggsRemoveHiddenGame(void)
  EGGS_SECTION;

/* === The end ========================================================	*/

#endif
