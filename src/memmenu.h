/*

    ezx81 - ZX81 emulator

    Copyright (C) 2003  Ian Cowburn (ianc@noddybox.demon.co.uk)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    -------------------------------------------------------------------------

    Provides a menu driven interface for analysing memory

*/

#ifndef EZX81_MEMMENU_H
#define EZX81_MEMMENU_H

#include "z80.h"


/* Memory menu.  Returns TRUE if exit (from program) selected.
*/
int		MemoryMenu(Z80 *z80);


/* Display the state of the SPEC at the bottom of the screen
*/
void		DisplayState(Z80 *z80);


/* Non-NULL (the breakpoint hit) if a breakpoint has been hit
*/
const char	*Break(void);


#endif


/* END OF FILE */
