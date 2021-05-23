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
#include "util.h"
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

#define WHITE	GFXRGB(255,255,255)
#define BLACK	GFXRGB(0,0,0)
#define RED	GFXRGB(255,100,100)
#define GREY	GFXRGB(160,160,160)
#define LOGREY	GFXRGB(100,100,100)
#define GREEN	GFXRGB(100,255,100)

/* ---------------------------------------- STATICS
*/


/* ---------------------------------------- PRIVATE FUNCTIONS
*/
static void Trim(char *p,size_t len)
{
    if (strlen(p)>len)
    {
    	p[len]=0;
    	p[len-1]='.';
    	p[len-2]='.';
    }
}


static void Centre(const char *p, int y, Uint32 col)
{
    GFXPrint((GFX_WIDTH-strlen(p)*8)/2,y,col,"%s",p);
}


static void Box(const char *title, int x, int y, int width, int height)
{
    GFXRect(x+1,y+1,
	    width-2,height-2,
	    BLACK,TRUE);

    GFXRect(x+1,y+1,
	    width-2,11,
	    LOGREY,TRUE);

    GFXRect(x,y,
	    width,height,
	    GREY,FALSE);

    Centre(title,y+2,GREEN);
    GFXHLine(x,x+width-1,y+11,GREY);
}


static int DoList(const char *title, int no, char * const list[], int *option)
{
    static const int max=GFX_HEIGHT/8-8;
    SDL_Event *e;
    int top;
    int cur;
    int done;
    int f;

    if (no==0)
    	return -1;

    top=0;
    cur=0;

    done=FALSE;

    while(!done)
    {
	Box(title,7,7,GFX_WIDTH-14,GFX_HEIGHT-14);

	if (option)
	{
	    Centre("Cursors to move, RETURN to accept",
	    		GFX_HEIGHT-44,WHITE);
	    Centre("SPACE toggles, I inverts all",
	    		GFX_HEIGHT-36,WHITE);
	    Centre("S select all, C clear all",
	    		GFX_HEIGHT-28,WHITE);
	}
	else
	    Centre("Cursors and RETURN to select",GFX_HEIGHT-28,WHITE);

	Centre("ESCAPE to cancel",GFX_HEIGHT-20,WHITE);

	for(f=0;f<max;f++)
	{
	    if (f+top<no)
	    {
		Uint32 pen,paper;

		if (f+top==cur)
		{
		    pen=WHITE;
		    paper=RED;
		}
		else
		{
		    pen=GREY;
		    paper=BLACK;
		}

		if (option)
		    GFXPrintPaper(16,20+f*8,pen,paper,"%c %-34.34s",
				  option[f+top] ? FONT_TICK:' ',list[f+top]);
		else
		    GFXPrintPaper(16,20+f*8,pen,paper,"%-36.36s",list[f+top]);
	    }
	}

	GFXEndFrame(FALSE);

	e=GFXWaitKey();

	switch(e->key.keysym.sym)
	{
	    case SDLK_RETURN:
	    	done=TRUE;
		break;

	    case SDLK_SPACE:
	    	if (option)
		    option[cur]=!option[cur];
		break;

	    case SDLK_i:
	    	if (option)
		    for(f=0;f<no;f++)
			option[f]=!option[f];
		break;

	    case SDLK_s:
	    	if (option)
		    for(f=0;f<no;f++)
			option[f]=TRUE;
		break;

	    case SDLK_c:
	    	if (option)
		    for(f=0;f<no;f++)
			option[f]=FALSE;
		break;

	    case SDLK_ESCAPE:
		cur=-1;
	    	done=TRUE;
	    	break;

	    case SDLK_UP:
		if (cur>0)
		{
		    cur--;

		    if (cur<top)
		    	top=cur;
		}
	    	break;

	    case SDLK_DOWN:
		if (cur<no-1)
		{
		    cur++;

		    if (cur>top+max-2)
		    	top=cur-max+2;
		}
		break;

	    default:
		break;
	}
    }

    return cur;
}


/* ---------------------------------------- EXPORTED INTERFACES
*/
int GUIMessage(GUIBoxType type, const char *title, const char *format,...)
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
    int ret;

    ret=FALSE;

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

    width=16;

    line[0]=strtok(buff,"\n");
    Trim(line[0],38);

    if (strlen(line[0])>width)
	width=strlen(line[0]);

    for(f=1;f<no;f++)
    {
    	line[f]=strtok(NULL,"\n");
	Trim(line[f],38);

	if (strlen(line[f])>width)
	    width=strlen(line[f]);
    }

    width=(width*8)+18;
    height=(no+5)*10;

    if (width>(GFX_WIDTH-10))
    	width=GFX_WIDTH-10;

    if (height>(GFX_HEIGHT-10))
    	height=GFX_HEIGHT-10;

    y=(GFX_HEIGHT-height)/2;
    x=(GFX_WIDTH-width)/2;

    Box(title,x,y,width,height);

    for(f=0;f<no;f++)
    	Centre(line[f],y+5+10*(f+1),WHITE);

    if (type==eMessageBox)
	Centre("Press a key",y+height-12,RED);
    else
	Centre("Press Y/N",y+height-12,RED);

    GFXEndFrame(FALSE);

    if (type==eYesNoBox)
    {
	SDL_Event *e;

	while(TRUE)
	{
	    e=GFXWaitKey();

	    if (e->key.keysym.sym==SDLK_y)
	    {
	    	ret=TRUE;
		break;
	    }
	    else if (e->key.keysym.sym==SDLK_n)
	    {
	    	ret=FALSE;
		break;
	    }
	}
    }
    else
	GFXWaitKey();


    free(line);

    return ret;
}


const char *GUIInputString(const char *prompt, const char *orig)
{
    static const int y_pos=GFX_HEIGHT-8;
    static char buff[41];
    size_t len;
    int done=FALSE;
    unsigned char c;
    SDL_Event *e;

    buff[0]=0;
    strncat(buff,orig,40);
    len=strlen(buff);

    while(!done)
    {
    	GFXRect(0,y_pos,GFX_WIDTH,8,BLACK,TRUE);
	GFXPrint(0,y_pos,WHITE,"%s %s%c",prompt,buff,FONT_CURSOR);
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
		c=(unsigned char)e->key.keysym.sym;

	    	if (len<40 && isprint(c))
		{
		    buff[len++]=c;
		    buff[len]=0;
		}
		break;
	}
    }

    return buff;
}


int GUIListSelect(const char *title, int no, char * const list[])
{
    return DoList(title,no,list,NULL);
}


int GUIListOption(const char *title, int no, char * const list[], int option[])
{
    int *o;
    int sel;
    int f;

    if (no==0)
    	return FALSE;

    o=Malloc(no * sizeof *o);

    for(f=0;f<no;f++)
    	o[f]=option[f];

    sel=DoList(title,no,list,o);

    if (sel!=-1)
    	for(f=0;f<no;f++)
	    option[f]=o[f];

    free(o);

    return sel!=-1;
}


/* END OF FILE */
