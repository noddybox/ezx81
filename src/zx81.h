/*

    ezx81 - X11 ZX81 emulator

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

    Provides the emulation for the ZX81

*/

#ifndef EZX81ZX81H
#define EZX81ZX81H "$Id$"

#include "z80.h"


/* Initialise the ZX81
*/
void	ZX81Init(void);

/* Interfaces for the Z80
*/
Z80Byte	ZX81ReadMem(Z80 *z80, Z80Word addr);
void	ZX81WriteMem(Z80 *z80, Z80Word addr, Z80Byte val);
Z80Byte	ZX81ReadPort(Z80 *z80, Z80Word port);
void	ZX81WritePort(Z80 *z80, Z80Word port, Z80Byte val);
Z80Byte	ZX81ReadForDisassem(Z80 *z80, Z80Word addr);


#endif


/* END OF FILE */
