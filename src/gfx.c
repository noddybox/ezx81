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
static const char ident[]="$Id$";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "gfx.h"
#include "exit.h"
#include "config.h"

#include "font.h"

static const char ident_h[]=EZX81_GFX_H;
static const char ident_fh[]=EZX81_FONT_H;


/* ---------------------------------------- MACROS
*/
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define LOCK	do							\
		{							\
		    if (SDL_MUSTLOCK(surface))				\
		    	if (SDL_LockSurface(surface)<0)			\
			    Exit("Failed to lock surface: %s\n",	\
			    		SDL_GetError());		\
		} while(0)

#define UNLOCK	do							\
		{							\
		    if (SDL_MUSTLOCK(surface))				\
		    	SDL_UnlockSurface(surface);			\
		} while(0)


/* ---------------------------------------- STATICS
*/
static SDL_Surface	*surface;
static Uint32		ticks;
static Uint32		frame;
static int		scale;

static void		(*putpixel)(int x, int y, Uint32 col);


/* ---------------------------------------- PRIVATE FUNCTIONS
*/

/* Taken from SDL documentation
*/
static void normal_putpixel(int x, int y, Uint32 pixel)
{
    int bpp;
    Uint8 *p;

    bpp=surface->format->BytesPerPixel;
    p=(Uint8 *)surface->pixels+y*surface->pitch+x*bpp;

    switch(bpp)
    {
	case 1:
	    *p=pixel;
	    break;

	case 2:
	    *(Uint16 *)p=pixel;
	    break;

	case 3:
	    if(SDL_BYTEORDER==SDL_BIG_ENDIAN)
	    {
		p[0]=(pixel>>16)&0xff;
		p[1]=(pixel>>8)&0xff;
		p[2]=pixel&0xff;
	    } else
	    {
		p[0]=pixel&0xff;
		p[1]=(pixel>>8)&0xff;
		p[2]=(pixel>>16)&0xff;
	    }
	    break;

	case 4:
	    *(Uint32 *)p=pixel;
	    break;
    }
}


static void scale_putpixel(int x, int y, Uint32 pixel)
{
    int sx,sy;

    x*=scale;
    y*=scale;

    for(sx=0;sx<scale;sx++)
	for(sy=0;sy<scale;sy++)
	    normal_putpixel(x+sx,y+sy,pixel);
}


static void DoHLine(int x1, int x2, int y, Uint32 col)
{
    for(;x1<=x2;x1++)
    	putpixel(x1,y,col);
}


static void DoVLine(int x, int y1, int y2, Uint32 col)
{
    for(;y1<=y2;y1++)
    	putpixel(x,y1,col);
}


/* ---------------------------------------- EXPORTED INTERFACES
*/
void GFXInit(void)
{
    if (IConfig(CONF_FULLSCREEN))
	scale=1;
    else
	scale=IConfig(CONF_SCALE);

    if (scale<0)
    	scale=1;

    if (scale>1)
    	putpixel=scale_putpixel;
    else
    	putpixel=normal_putpixel;

    frame=1000/IConfig(CONF_FRAMES_PER_SEC);

    if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO))
	Exit("Failed to init SDL: %s\n",SDL_GetError());

    if (!(surface=SDL_SetVideoMode(GFX_WIDTH*scale,
				   GFX_HEIGHT*scale,
				   0,
				   IConfig(CONF_FULLSCREEN) ?
				   		SDL_FULLSCREEN : 0)))
    {
	Exit("Failed to open video: %s\n",SDL_GetError());
    }

    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_SetCaption("eZX81","eZX81");

    atexit(SDL_Quit);
}


SDL_Surface *GFXGetSurface(void)
{
    return surface;
}


Uint32 GFXRGB(Uint8 r, Uint8 g, Uint8 b)
{
    return SDL_MapRGB(surface->format,r,g,b);
}


void GFXClear(Uint32 col)
{
    SDL_FillRect(surface,NULL,col);
}


void GFXStartFrame(void)
{
    ticks=SDL_GetTicks();
}


void GFXEndFrame(int delay)
{
    SDL_UpdateRect(surface,0,0,0,0);

    if (delay)
    {
	Uint32 diff;

	diff=SDL_GetTicks()-ticks;

	if (diff<frame)
	    SDL_Delay(frame-diff);
    }
}


void GFXKeyRepeat(int repeat)
{
    if (repeat)
    	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
			    SDL_DEFAULT_REPEAT_INTERVAL);
    else
    	SDL_EnableKeyRepeat(0,0);
}


SDL_Event *GFXGetKey(void)
{
    static SDL_Event e;

    while(SDL_PollEvent(&e))
    {
    	if (e.type==SDL_KEYUP || e.type==SDL_KEYDOWN)
	    return &e;
    }

    return NULL;
}


SDL_Event *GFXWaitKey(void)
{
    static SDL_Event e;

    do
    {
    	SDL_WaitEvent(&e);
    } while (e.type!=SDL_KEYDOWN);

    return &e;
}


void GFXLock(void)
{
    LOCK;
}


void GFXUnlock(void)
{
    UNLOCK;
}


void GFXPlot(int x, int y, Uint32 col)
{
    LOCK;

    putpixel(x,y,col);

    UNLOCK;
}


void GFXFastPlot(int x, int y, Uint32 col)
{
    putpixel(x,y,col);
}


void GFXRect(int x, int y, int w, int h, Uint32 col, int solid)
{
    LOCK;

    if (solid)
    {
	SDL_Rect r;

	r.x=x*scale;
	r.y=y*scale;
	r.w=w*scale;
	r.h=h*scale;

	SDL_FillRect(surface,&r,col);
    }
    else
    {
	DoHLine(x,x+w-1,y,col);
	DoHLine(x,x+w-1,y+h-1,col);
	DoVLine(x,y,y+h-1,col);
	DoVLine(x+w-1,y,y+h-1,col);
    }

    UNLOCK;
}


void GFXHLine(int x1, int x2, int y, Uint32 col)
{
    LOCK;
    DoHLine(x1,x2,y,col);
    UNLOCK;
}


void GFXVLine(int x, int y1, int y2, Uint32 col)
{
    LOCK;
    DoVLine(x,y1,y2,col);
    UNLOCK;
}


void GFXPrint(int x, int y, Uint32 col, const char *format, ...)
{
    char buff[257];
    va_list va;
    char *p;
    int sx,sy;

    va_start(va,format);
    vsprintf(buff,format,va);
    va_end(va);

    p=buff;

    LOCK;

    while(*p)
    {
	for(sy=0;sy<8;sy++)
	{
	    if (y+sy<GFX_HEIGHT && x<GFX_WIDTH)
	    {
		for(sx=0;sx<8;sx++)
		{
		    if (font[(int)*p][sx+sy*8] && x+sx<GFX_WIDTH)
			putpixel(x+sx,y+sy,col);
		}
	    }
	}

	p++;
	x+=8;
    }

    UNLOCK;
}


void GFXPrintPaper(int x, int y, Uint32 col, Uint32 paper,
		   const char *format, ...)
{
    char buff[257];
    va_list va;
    char *p;
    int sx,sy;

    va_start(va,format);
    vsprintf(buff,format,va);
    va_end(va);

    p=buff;

    LOCK;

    while(*p)
    {
	for(sy=0;sy<8;sy++)
	{
	    if (y+sy<GFX_HEIGHT && x<GFX_WIDTH)
	    {
		for(sx=0;sx<8;sx++)
		{
		    if (font[(int)*p][sx+sy*8] && x+sx<GFX_WIDTH)
			putpixel(x+sx,y+sy,col);
		    else
			putpixel(x+sx,y+sy,paper);
		}
	    }
	}

	p++;
	x+=8;
    }

    UNLOCK;
}


/* END OF FILE */
