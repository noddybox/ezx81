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

    Basic GUI routines

*/
static const char ident[]="$Id$";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "gui.h"
#include "gfx.h"
#include "exit.h"

static const char ident_h[]=EZX81_GUI_H;


/* ---------------------------------------- MACROS
*/
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Must match gfx.c
*/
#define SCR_W	320
#define SCR_H	200

/* ---------------------------------------- STATICS
*/


/* ---------------------------------------- PRIVATE FUNCTIONS
*/
static void *Malloc(size_t size)
{
    void *p=malloc(size);

    if (!p)
    	Exit("malloc failed for %lu bytes\n",(unsigned long)size);

    return p;
}


static void Centre(const char *p, int y, int r, int g, int b)
{
    GFXPrint((SCR_W-strlen(p)*8)/2,y,GFXRGB(r,g,b),"%s",p);
}


/* ---------------------------------------- EXPORTED INTERFACES
*/
void GUIMessage(const char *title, const char *format,...)
{
    char buff[1025];
    va_list va;
    char *p;
    int no;
    int f;
    char **line;
    int width;
    int height;
    int x,y;

    va_start(va,format);
    vsprintf(buff,format,va);
    va_end(va);

    p=buff;
    no=1;

    while(*p)
    {
    	if (*p=='\n')
	    no++;

	p++;
    }

    line=Malloc(sizeof *line * no);

    line[0]=strtok(buff,"\n");
    width=strlen(line[0]);

    for(f=1;f<no;f++)
    {
    	line[f]=strtok(NULL,"\n");

	if (strlen(line[f])>width)
	    width=strlen(line[f]);
    }

    width=(width*8)+16;
    height=(no+3)*10;

    if (width>(SCR_W-10))
    	width=SCR_W-10;

    if (height>(SCR_H-10))
    	height=SCR_H-10;

    y=(SCR_H-height)/2;
    x=(SCR_W-width)/2;

    GFXRect(x-1,y-1,
	    width+2,height+2,
	    GFXRGB(255,255,255),FALSE);

    GFXRect(x,y,
	    width,height,
	    GFXRGB(0,0,0),TRUE);

    Centre(title,y+2,255,255,255);
    GFXHLine(x+2,x+width-4,y+10,GFXRGB(255,255,255));

    for(f=0;f<no;f++)
    	Centre(line[f],y+5+10*(f+1),200,200,200);

    Centre("Press a key",y+height-10,255,0,0);

    GFXEndFrame(FALSE);

    GFXWaitKey();

    free(line);
}


const char *GUIInputString(const char *prompt, const char *orig)
{
    static const int y_pos=SCR_H-8;
    static char buff[41];
    size_t len;
    int done=FALSE;
    SDL_Event *e;

    buff[0]=0;
    strncat(buff,orig,40);
    len=strlen(buff);

    while(!done)
    {
    	GFXRect(0,y_pos,SCR_W,8,GFXRGB(0,0,0),TRUE);
	GFXPrint(0,y_pos,GFXRGB(255,255,255),"%s %s%c",prompt,buff,FONT_CURSOR);
	GFXEndFrame(FALSE);

	e=GFXWaitKey();

	switch(e->key.keysym.sym)
	{
	    case SDLK_RETURN:
	    case SDLK_KP_ENTER:
	    	done=TRUE;
		break;

	    case SDLK_ESCAPE:
		buff[0]=0;
		len=0;
	    	break;

	    case SDLK_BACKSPACE:
	    case SDLK_DELETE:
	    	if (len)
		    buff[--len]=0;

		break;

	    default:
	    	if (len<40 && isprint(e->key.keysym.sym))
		{
		    buff[len++]=(char)e->key.keysym.sym;
		    buff[len]=0;
		}
		break;
	}
    }

    return buff;
}


/* END OF FILE */
