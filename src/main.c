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

    ConfigRead();

    ZX81Init();

    z80=Z80Init(ZX81WriteMem,
    		ZX81ReadMem,
		ZX81WritePort,
		ZX81ReadPort,
		ZX81ReadForDisassem,
		NULL);

    GFXInit();

    white=GFXRGB(255,255,255);
    grey=GFXRGB(128,128,128);
    black=GFXRGB(0,0,0);

    GFXClear(grey);
    GFXPrint(1,1,black,"Quick SDL check");
    GFXPrint(2,2,white,"Quick SDL check");
    GFXRect(100,100,20,20,white,TRUE);
    GFXRect(99,99,22,22,white,FALSE);

    GFXHLine(0,319,0,white);
    GFXVLine(0,1,199,black);

    {
    SDL_Event e;
    int f;

    for(f=0;f<1000;f++)
    {
    	GFXPlot(rand()%320,rand()%200,white);
	GFXEndFrame(FALSE);
    }

    do
    {
    	SDL_WaitEvent(&e);
    } while(e.type!=SDL_KEYUP);
    }

    SDL_Quit();

    return EXIT_SUCCESS;
}


/* END OF FILE */
