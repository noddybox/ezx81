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

    Config file

*/
static const char ident[]="$Id$";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "exit.h"
#include "config.h"

static const char ident_h[]=EZX81_CONFIG_H;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/* ---------------------------------------- CONFIG
*/
static char	rompath[FILENAME_MAX]="./zx81.rom";
static char	tapedir[FILENAME_MAX]=".";
static int	fullscreen=FALSE;
static int	memsize=16;
static int	frames=50;
static int	scale=1;

static const struct
{
    const char	*name;
    void	*var;
    int		is_int;
} config[]= {
		{"rompath",	rompath,	FALSE},
		{"tapedir",	tapedir,	FALSE},
		{"fullscreen",	&fullscreen,	TRUE},
		{"memsize",	&memsize,	TRUE},
		{"frames",	&frames,	TRUE},
		{"scale",	&scale,		TRUE},
		{NULL,		NULL,		FALSE}
	    };


/* ---------------------------------------- PRIVATE FUNCTIONS
*/
static void Parse(FILE *fp)
{
    char buff[1024];

    while(fgets(buff,sizeof buff,fp))
    {
    	size_t l;

	l=strlen(buff);

	if (buff[l-1]=='\n')
	    buff[--l]=0;

	if (l>0 && buff[0]!='#')
	{
	    char *t1=NULL;
	    char *t2=NULL;

	    t1=strtok(buff,"\t");
	    t2=strtok(NULL,"\t");

	    if (t2)
	    {
	    	int f;

		for(f=0;config[f].name;f++)
		{
		    if (strcmp(config[f].name,t1)==0)
		    {
			if (config[f].is_int)
			{
			    int *i;

			    i=config[f].var;
			    *i=atoi(t2);
			}
			else
			{
			    char *p;

			    p=config[f].var;
			    strcpy(p,t2);
			}
		    }
		}
	    }
	    else
	    {
	    	fprintf(stderr,"Ignored bad config: %s %s\n",t1,t2 ? t2:"");
	    }
	}
    }
}


/* ---------------------------------------- EXPORTED INTERFACES
*/
void ConfigRead(void)
{
    FILE *fp;
    char path[FILENAME_MAX]={0};

    if (getenv("HOME"))
    	strcpy(path,getenv("HOME"));

    strcat(path,"/.ezx81");

    if ((fp=fopen(path,"r")))
    {
    	Parse(fp);
	fclose(fp);
    }
}


int IConfig(IConfigVar v)
{
    static const int *vars[]=
    		{
		    &fullscreen,
		    &memsize,
		    &frames,
		    &scale
		};

    return *vars[v];
}


const char *SConfig(SConfigVar v)
{
    static const char *vars[]=
    		{
		    rompath,
		    tapedir
		};

    return vars[v];
}


/* END OF FILE */
