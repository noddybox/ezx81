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

    Provides a common error exit point

*/
static const char ident[]="$Id$";

#include <stdlib.h>
#include <stdarg.h>
#include "exit.h"

#include <SDL.h>

static const char ident_h[]=EZX81_EXIT_H;


/* ---------------------------------------- EXPORTED INTERFACES
*/
void Exit(const char *format,...)
{
    va_list va;

    if (SDL_WasInit(SDL_INIT_EVERYTHING))
    {
    	SDL_Quit();
    }

    va_start(va,format);
    vfprintf(stderr,format,va);
    va_end(va);

    exit(EXIT_FAILURE);
}


/* END OF FILE */
