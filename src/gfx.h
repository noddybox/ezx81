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

    Wrapper to SDL

*/

#ifndef EZX81_GFX_H
#define EZX81_GFX_H "$Id$"

#include "SDL.h"


/* ---------------------------------------- CONSTANTS
*/
#define FONT_UP_ARROW		'\001'
#define FONT_DOWN_ARROW		'\002'
#define FONT_LEFT_ARROW		'\003'
#define FONT_RIGHT_ARROW	'\004'
#define FONT_TICK		'\005'
#define FONT_CROSS		'\006'
#define FONT_CURSOR		'\007'
#define FONT_COPYRIGHT		'\010'

/* The size of the display
*/
#define GFX_WIDTH		320
#define GFX_HEIGHT		200


/* ---------------------------------------- INTERFACES
*/

/* Initialise
*/
void		GFXInit(void);


/* Get the SDL_Surface for the screen
*/
SDL_Surface	*GFXGetSurface(void);


/* Get the pixel value for a colour
*/
Uint32		GFXRGB(Uint8 r, Uint8 g, Uint8 b);


/* Clear the screen
*/
void		GFXClear(Uint32 col);


/* Indicate a frame is starting
*/
void		GFXStartFrame(void);


/* Indicate a frame is done and refreshes the screen
   If delay is TRUE, sleeps to implement the frames_per_sec config.
   It's safe to call this multiple times without a matching GFXStartFrame().
*/
void		GFXEndFrame(int delay);


/* Set key repeat
*/
void		GFXKeyRepeat(int repeat);


/* Suck out keyboard events.  Returns NULL if no more keyboard events
*/
SDL_Event	*GFXGetKey(void);


/* Wait for a keypress.  Note that key up events are returned.
*/
SDL_Event	*GFXWaitKey(void);


/*
    Note that no bound checking (except for GFXPrint()) is done - it is the
    callers responsibility to plot onscreen.
*/

/* Plot a point
*/
void		GFXPlot(int x, int y, Uint32 col);


/* Draw a rectangle
*/
void		GFXRect(int x, int y, int w, int h, Uint32 col, int solid);


/* Draw an horizontal line
*/
void		GFXHLine(int x1, int x2, int y, Uint32 col);


/* Draw a vertical line
*/
void		GFXVLine(int x, int y1, int y2, Uint32 col);


/* Draws text using an internal 8x8 monspaced font.  The only supported
   characters are 32-126 and the special FONT_xxx characters defined
   above.  The routine assumes the environment operates with ASCII.

   The co-ord given is the top left of the first character, and characters
   are drawn transparently.

   Causes undefined behaviour if the generated string from the printf
   format is longer than 256 characters.
*/
void		GFXPrint(int x, int y, Uint32 col, const char *format, ...);


/* As above, but draws text using the supplied background colour, rather
   than transparently.
*/
void		GFXPrintPaper(int x, int y, Uint32 col, Uint32 paper,
			      const char *format, ...);


#endif


/* END OF FILE */
