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

*/
static const char id[]="$Id$";

#include <stdlib.h>
#include <stdio.h>

#include "SDL.h"

#include "z80.h"
#include "zx81.h"
#include "gfx.h"
#include "gui.h"
#include "memmenu.h"
#include "config.h"
#include "exit.h"


/* ---------------------------------------- MACROS
*/
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/* ---------------------------------------- STATICS
*/
static Uint32	white;
static Uint32	black;
static Uint32	grey;


/* ---------------------------------------- PROTOS
*/


/* ---------------------------------------- MAIN
*/
int main(int argc, char *argv[])
{
    Z80 *z80;
    SDL_Event *e;
    int quit;
    int trace;

    ConfigRead();

    trace=IConfig(CONF_TRACE);

    z80=Z80Init(ZX81WriteMem,
    		ZX81ReadMem,
		ZX81WritePort,
		ZX81ReadPort,
		ZX81ReadForDisassem,
		NULL);

    GFXInit();

    ZX81Init(z80);

    white=GFXRGB(255,255,255);
    grey=GFXRGB(128,128,128);
    black=GFXRGB(0,0,0);

    quit=FALSE;

    while(!quit)
    {
	Z80SingleStep(z80);

	if (trace)
	{
	    DisplayState(z80);
	    GFXEndFrame(FALSE);
	}

	while((e=GFXGetKey()))
	{
	    switch (e->key.keysym.sym)
	    {
	    	case SDLK_ESCAPE:
		    if (e->key.state==SDL_RELEASED)
			quit=TRUE;
		    break;

		case SDLK_F11:
		    if (e->key.state==SDL_RELEASED)
		    	MemoryMenu(z80);
		    break;

		case SDLK_F12:
		    if (e->key.state==SDL_RELEASED)
		    	trace=!trace;
		    break;

		default:
		    ZX81KeyEvent(e);
		    break;
	    }
	}
    }

    SDL_Quit();

    return EXIT_SUCCESS;
}


/* ---------------------------------------- PRIVATE FUNCTIONS
*/


/* END OF FILE */
