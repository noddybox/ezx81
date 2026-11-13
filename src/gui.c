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

    Basic GUI routines

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <SDL.h>

#include "gui.h"
#include "gfx.h"
#include "util.h"
#include "exit.h"


/* ---------------------------------------- MACROS
*/
#define WHITE	GFXRGB(255,255,255)
#define BLACK	GFXRGB(0,0,0)
#define RED	GFXRGB(255,100,100)
#define GREY	GFXRGB(160,160,160)
#define LOGREY	GFXRGB(100,100,100)
#define GREEN	GFXRGB(100,255,100)

#define MAX_MATCH 20

#define LIST_CANCEL -1
#define LIST_NEW -2


/* ---------------------------------------- TYPES
*/
typedef struct
{
    char	name[MAX_PATH_BUFF+1];
    unsigned 	size;
    int		is_dir;
} FileEnt;

typedef struct toast_struct
{
    time_t		display_till;
    char		*message;
    struct toast_struct	*next;
    struct toast_struct	*prev;
} Toast;


/* ---------------------------------------- STATICS
*/

static Toast	*toast = NULL;


/* ---------------------------------------- PRIVATE FUNCTIONS
*/
static void DrawToast(const char *message, int y)
{
    int w = strlen(message) * 8 + 4;

    GFXRect(GFX_WIDTH - w, y, w, 10, BLACK, TRUE);
    GFXRect(GFX_WIDTH - w, y, w, 10, GREEN, FALSE);
    GFXPrint(GFX_WIDTH - w + 2, y, WHITE, "%s", message);
}

static void HandleToast(void)
{
    Toast *t = toast;
    int y = GFX_HEIGHT - 10;
    time_t now = time(NULL);

    while(t)
    {
    	if (t->display_till < now)
	{
	    Toast *n = t->next;

	    if (t->prev)
	    {
		t->prev->next = t->next;
	    }
	    
	    if (t->next)
	    {
	    	t->next->prev = t->prev;
	    }

	    if (toast == t)
	    {
	    	toast = t->next;
	    }

	    free(t->message);
	    free(t);

	    t = n;
	}
	else
	{
	    DrawToast(t->message, y);
	    y -= 12;
	    t = t->next;
	}
    }
}


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


static int DoList(const char *title, int no, const char * const list[],
		  int *option, char *input, int display_only, int inital_select)
{
    static const int max=GFX_HEIGHT/8-8;
    SDL_Event *e;
    int top;
    int cur;
    int done;
    int f;
    char match[MAX_MATCH + 1] = {0};
    size_t input_len = 0;
    int is_text;
    int text_input;

    if (no==0)
    	return LIST_CANCEL;

    text_input = GFXSetTextInput(TRUE);

    if (input)
    {
    	*input = 0;
    }

    cur = (inital_select >=0 && inital_select < no) ? inital_select : 0;

    top = 0;

    if (cur > top + max - 2)
    {
	top = cur - max + 2;
    }

    done=FALSE;

    while(!done)
    {
	Box(title,7,7,GFX_WIDTH-14,GFX_HEIGHT-14);

	if (!display_only)
	{
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
	    {
		if (input)
		{
		    Centre("Enter new name and press RETURN",
		    					GFX_HEIGHT-36,WHITE);
		    Centre("Or select with cursors",GFX_HEIGHT-28,WHITE);
		}
		else
		{
		    Centre("Cursors and RETURN to select",GFX_HEIGHT-28,WHITE);
		}
	    }
	}

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

	if (input)
	{
	    static const int y_pos = GFX_HEIGHT - 44;

	    GFXRect(8,y_pos,GFX_WIDTH-16,8,BLACK,TRUE);
	    GFXPrint(8,y_pos,WHITE,"File: %s%c",input,FONT_CURSOR);
	}

	GFXEndFrame(FALSE);

	e=GFXWaitKey(&is_text);

	if (!is_text)
	{
	    switch(e->key.keysym.sym)
	    {
		case SDLK_RETURN:
		    if (!display_only)
		    {
			done=TRUE;
		    }
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
		    else
		    {
		    	cur = no - 1;

			if (cur>top+max-2)
			    top=cur-max+2;
		    }
		    break;

		case SDLK_DOWN:
		    if (cur<no-1)
		    {
			cur++;

			if (cur>top+max-2)
			    top=cur-max+2;
		    }
		    else
		    {
		    	cur = 0;
			top = 0;
		    }
		    break;

		case SDLK_PAGEUP:
		    if (cur>0)
		    {
			cur-=(max-1);

			if (cur<0)
			    cur=0;

			if (cur<top)
			    top=cur;
		    }
		    break;

		case SDLK_PAGEDOWN:
		    if (cur<no-1)
		    {
			cur+=(max-1);

			if (cur>=no)
			    cur=no-1;

			if (cur>top+max-2)
			    top=cur-max+2;
		    }
		    break;

		case SDLK_BACKSPACE:
		case SDLK_DELETE:
		    if (input)
		    {
			if (input_len)
			{
			    input[--input_len] = 0;
			}
		    }
		    else
		    {
			match[0] = 0;
		    }
		    break;

		default:
		    break;
	    }
	}
	else
	{
	    char c = GFX_TEXT_ASCII(e);

	    if (option)
	    {
		switch(toupper((unsigned char)c))
		{
		    case ' ':
			option[cur]=!option[cur];
			break;

		    case 'I':
			for(f=0;f<no;f++)
			    option[f]=!option[f];
			break;

		    case 'S':
			for(f=0;f<no;f++)
			    option[f]=TRUE;
			break;

		    case 'C':
			for(f=0;f<no;f++)
			    option[f]=FALSE;
			break;

		    default:
			break;
		}
	    }
	    else
	    {
		if (input)
		{
		    if (input_len < 25)
		    {
			input[input_len++] = c;
			input[input_len] = 0;
		    }
		}
		else
		{
		    size_t l = strlen(match);

		    if (c && l < MAX_MATCH)
		    {
			int found = FALSE;

			match[l++] = c;
			match[l] = 0;

			for(f = 0; f < no && !found; f++)
			{
			    if (StartsWith(list[f], match, l))
			    {
				found = TRUE;

				cur = f;
				
				if (cur < top)
				{
				    top = cur;
				}

				if (cur > top + max - 2)
				{
				    top = cur - max + 2;
				}
			    }
			}
		    }
		}
	    }
	}
    }

    GFXSetTextInput(text_input);

    if (input && input_len)
    {
    	return LIST_NEW;
    }
    else
    {
	return cur;
    }
}


static int CmpFile(const void *va,const void *vb)
{
    const FileEnt *a=(FileEnt *)va;
    const FileEnt *b=(FileEnt *)vb;

    if ((a->is_dir)&&(!b->is_dir))
	return -1;

    if ((!a->is_dir)&&(b->is_dir))
	return 1;

    return strcmp(a->name,b->name);
}


/* ---------------------------------------- EXPORTED INTERFACES
*/
void GUIInit(void)
{
    GFXRegisterRedrawCallback(HandleToast);
}


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
	    int is_text;

	    e=GFXWaitKey(&is_text);

	    if (!is_text)
	    {
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
    }
    else
	GFXWaitKey(NULL);

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
    int state;

    buff[0]=0;
    strncat(buff,orig,40);
    len=strlen(buff);

    state = GFXSetTextInput(TRUE);

    while(!done)
    {
	int is_text;

    	GFXRect(0,y_pos,GFX_WIDTH,8,BLACK,TRUE);
	GFXPrint(0,y_pos,WHITE,"%s %s%c",prompt,buff,FONT_CURSOR);
	GFXEndFrame(FALSE);

	e=GFXWaitKey(&is_text);

	if (!is_text)
	{
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
		    break;
	    }
	}
	else
	{
	    c = GFX_TEXT_ASCII(e);

	    if (len<40 && isprint(c))
	    {
		buff[len++]=c;
		buff[len]=0;
	    }
	}
    }

    GFXSetTextInput(state);

    return buff;
}


int GUIListSelect(const char *title, int no, const char * const list[])
{
    return DoList(title, no,list, NULL, NULL, FALSE, -1);
}


int GUIMenu(const char *title, int no, int selected, const char * const list[])
{
    return DoList(title, no, list, NULL, NULL, FALSE, selected);
}


int GUIListOption(const char *title, int no, 
		  const char * const list[], int option[])
{
    int *o;
    int sel;
    int f;

    if (no==0)
    	return FALSE;

    o=Malloc(no * sizeof *o);

    for(f=0;f<no;f++)
    	o[f]=option[f];

    sel=DoList(title, no, list, o, NULL, FALSE, -1);

    if (sel!=-1)
    	for(f=0;f<no;f++)
	    option[f]=o[f];

    free(o);

    return sel!=-1;
}


void GUIDisplayList(const char *title, int no, const char * const list[])
{
    DoList(title, no, list, NULL, NULL, TRUE, -1);
}


int GUIFileSelect(const char *prompt, int load,
		  const char *start_dir, char path[], const char *extension)
{
    int done=FALSE;
    int ret=FALSE;
    char olddir[MAX_PATH_BUFF+1];
    char pwd[MAX_PATH_BUFF+1];
    char input[MAX_PATH_BUFF + 1];
    int state;

    getcwd(olddir,MAX_PATH_BUFF);

    if (chdir(start_dir) == -1)
    {
    	chdir(Dirname(start_dir));
    }

    state = GFXSetTextInput(TRUE);

    while(!done)
    {
	int f;
	int sel;
	DIR *d;
	struct dirent *de;
	struct stat sbuf;
    	FileEnt *fe=NULL;
	char **list;
	int no=0;

    	getcwd(pwd,MAX_PATH_BUFF);

	d=opendir(".");

	while(readdir(d))
	    no++;

	rewinddir(d);

	fe=Malloc(sizeof *fe * no);
	no=1;

	strcpy(fe[0].name,"..");
	fe[0].is_dir=TRUE;

	while((de=readdir(d)))
	{
	    if (stat(de->d_name,&sbuf)!=-1)
	    {
	    	if (S_ISDIR(sbuf.st_mode) && de->d_name[0]!='.')
		{
		    strcpy(fe[no].name,de->d_name);
		    fe[no].is_dir=TRUE;
		    no++;
		}
		else if (S_ISREG(sbuf.st_mode) && de->d_name[0]!='.' &&
			   CheckExtension(de->d_name, extension))
		{
		    strcpy(fe[no].name,de->d_name);
		    fe[no].is_dir=FALSE;
		    fe[no].size=(unsigned)sbuf.st_size;
		    no++;
		}
	    }
	}

	closedir(d);

	qsort(fe,no,sizeof *fe,CmpFile);

	list=Malloc(sizeof *list * no);

	for(f=0;f<no;f++)
	{
	    list[f]=Malloc(80);

	    if (fe[f].is_dir)
		sprintf(list[f],"%-20.20s %9s",fe[f].name,"<DIR>");
	    else
		sprintf(list[f],"%-20.20s %9u",fe[f].name,fe[f].size);
	}

	sel=DoList(pwd, no, GUI_LIST_CAST(list), NULL,
		   load ? NULL:input, FALSE, -1);

	if (sel == -1)
	{
	    ret=FALSE;
	    done=TRUE;
	}
	else
	{
	    if (sel == LIST_NEW)
	    {
		strcpy(path,pwd);
		
		if (path[strlen(path)]!='/')
		    strcat(path,"/");

		strcat(path,input);

		if (!CheckExtension(path, extension))
		{
		    strcat(path, ".");
		    strcat(path, extension);
		}

		done=TRUE;
		ret=TRUE;
	    }
	    else
	    {
		if (fe[sel].is_dir)
		{
		    chdir(fe[sel].name);
		}
		else
		{
		    strcpy(path,pwd);
		    
		    if (path[strlen(path)]!='/')
			strcat(path,"/");

		    strcat(path,fe[sel].name);

		    done=TRUE;
		    ret=TRUE;
		}
	    }
	}

	for(f=0;f<no;f++)
	{
	    free(list[f]);
	}

	free(list);
	free(de);
    }

    GFXSetTextInput(state);

    return ret;
}


void GUIToast(const char *message)
{
    Toast *new = Malloc(sizeof *new);

    new->prev = NULL;
    new->next = toast;

    if (new->next)
    {
	new->next->prev = new;
    }

    new->display_till = time(NULL) + 4;
    new->message = StrCopy(message);
    toast = new;

    /* Force a redraw so that the toast appears, even during a keypress wait
    */
    GFXEndFrame(FALSE);
}
