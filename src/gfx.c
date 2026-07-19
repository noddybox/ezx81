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

    Wrapper to SDL

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "gfx.h"
#include "exit.h"
#include "config.h"
#include "util.h"

#include "font.h"


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
static SDL_Window       *window;
static SDL_Renderer	*renderer;
static SDL_Texture      *texture;
static Uint32           *pixels;
static Uint32		ticks;
static Uint32		frame;
static int		scale;

static void		(*putpixel)(int x, int y, Uint32 col);

#define NO_BMPIX	10

static struct
{
    Uint32      col;
    int         r,g,b;
} bmpix[NO_BMPIX]=
{
    {0,     0x00,0x00,0x00},        /* BLACK */
    {0,     0x00,0x00,0xff},        /* BLUE */
    {0,     0xff,0x00,0x00},        /* RED */
    {0,     0xff,0x00,0xff},        /* MAGENTA */
    {0,     0x00,0xff,0x00},        /* GREEN */
    {0,     0x00,0xff,0xff},        /* CYAN */
    {0,     0xff,0xff,0x00},        /* YELLOW */
    {0,     0xff,0xff,0xff},        /* WHITE */
    {0,     0x60,0x60,0x60},        /* GREY */
};



/* ---------------------------------------- PRIVATE FUNCTIONS
*/

static void normal_putpixel(int x, int y, Uint32 pixel)
{
    pixels[x + y * GFX_WIDTH] = pixel;
}


static void scale_putpixel(int x, int y, Uint32 pixel)
{
    int sx,sy;

    x*=scale;
    y*=scale;

    for(sx=0;sx<scale;sx++)
	for(sy=0;sy<scale;sy++)
            pixels[x+sx + (y+sy) * GFX_WIDTH * scale] = pixel;
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


static void BMPlot(int bx, int by, int *x, int *y, int w, int h, int col)
{
    if (*y<h)
    {
	putpixel(bx+*x,by+*y,bmpix[col].col);

	(*x)++;

	if (*x==w)
	{
	    *x=0;
	    (*y)++;
	}
    }
}


/* ---------------------------------------- EXPORTED INTERFACES
*/
void GFXInit(void)
{
    int f;

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

    if (!(window=SDL_CreateWindow("eSPEC",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  GFX_WIDTH*scale,
				  GFX_HEIGHT*scale,
				  IConfig(CONF_FULLSCREEN) ?
				   		SDL_WINDOW_FULLSCREEN : 0)))
    {
	Exit("Failed to open window: %s\n",SDL_GetError());
    }

    if (!(renderer = SDL_CreateRenderer(window, -1, 0)))
    {
	Exit("Failed to create renderer: %s\n",SDL_GetError());
    }

    if (!(texture = SDL_CreateTexture(renderer,
                                      SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      GFX_WIDTH*scale,
                                      GFX_HEIGHT*scale)))
    {
	Exit("Failed to create texture: %s\n",SDL_GetError());
    }

    pixels = Malloc(sizeof(Uint32) * GFX_WIDTH*scale * GFX_HEIGHT*scale);

    SDL_ShowCursor(SDL_DISABLE);

    for(f=0;f<NO_BMPIX;f++)
    	bmpix[f].col=GFXRGB(bmpix[f].r,bmpix[f].g,bmpix[f].b);

    atexit(SDL_Quit);
}


Uint32 GFXRGB(Uint8 r, Uint8 g, Uint8 b)
{
    return 0xff << 24 | (Uint32)r << 16 | (Uint32)g << 8 | b; 
}


void GFXClear(Uint32 col)
{
    int f;

    for(f = 0; f < GFX_WIDTH * scale * GFX_HEIGHT * scale; f++)
    {
        pixels[f] = col;
    }
}


void GFXStartFrame(void)
{
    ticks=SDL_GetTicks();
}


void GFXEndFrame(int delay)
{
    SDL_UpdateTexture(texture, NULL,
                      pixels, GFX_WIDTH * scale * sizeof(Uint32));

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    if (delay)
    {
	Uint32 diff;

	diff=SDL_GetTicks()-ticks;

	if (diff<frame)
	    SDL_Delay(frame-diff);
    }
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


void GFXPlot(int x, int y, Uint32 col)
{
    putpixel(x,y,col);
}


void GFXRect(int x, int y, int w, int h, Uint32 col, int solid)
{
    if (solid)
    {
        while(h--)
        {
            DoHLine(x,x+w-1,y,col);
            y++;
        }
    }
    else
    {
	DoHLine(x,x+w-1,y,col);
	DoHLine(x,x+w-1,y+h-1,col);
	DoVLine(x,y,y+h-1,col);
	DoVLine(x+w-1,y,y+h-1,col);
    }
}


void GFXHLine(int x1, int x2, int y, Uint32 col)
{
    DoHLine(x1,x2,y,col);
}


void GFXVLine(int x, int y1, int y2, Uint32 col)
{
    DoVLine(x,y1,y2,col);
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
}


void GFXBitmap(int x, int y, int w, int h, const unsigned char *data)
{
    int pix;
    int px,py;

    pix=0;
    px=0;
    py=0;

    while(TRUE)
    {
    	int i;

	i=*data++;

	if (i<0x80)
	{
	    pix=i;

	    BMPlot(x,y,&px,&py,w,h,pix);

	    if (py>=h)
	    	break;
	}
	else
	{
	    int f;

	    for (f=0;f<i-0x80;f++)
		BMPlot(x,y,&px,&py,w,h,pix);

	    if (py>=h)
	    	break;
	}
    }
}


/* END OF FILE */
