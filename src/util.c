/*

    espec - Sinclair Spectrum emulator

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

    Usual library wrappers and utils

*/
static const char ident[]="$Id$";

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "util.h"
#include "exit.h"

static const char ident_h[]=ESPEC_UTIL_H;


/* ---------------------------------------- MACROS
*/
#define DIRSEP	'/'

/* ---------------------------------------- INTERFACES
*/
void *Malloc(size_t size)
{
    void *new=malloc(size);

    if (!new)
    	Exit("malloc failed for %lu bytes\n",(unsigned long)size);

    return new;
}


void *Realloc(void *p, size_t size)
{
    void *new=realloc(p,size);

    if (!new)
    	Exit("realloc failed for %lu bytes\n",(unsigned long)size);

    return new;
}


char *StrCopy(const char *source)
{
    return strcpy(Malloc(strlen(source)+1),source);
}


const char *Basename(const char *path)
{
    const char *p=path+strlen(path);

    while(p>path && *p!=DIRSEP)
    	p--;

    if (p>path)
    	p++;

    return p;
}


const char *Dirname(const char *path)
{
    static char dir[FILENAME_MAX];
    char *p;

    strcpy(dir,path);

    p=dir+strlen(dir);

    while(p>dir && *p!=DIRSEP)
    	p--;

    if (p>path)
    {
    	p++;
	*p=0;
    }
    else
    {
    	strcpy(dir,".");
    }

    return dir;
}


void Debug(const char *format, ...)
{
    static FILE *fp=NULL;
    va_list ap;

    if (!fp)
    {
    	fp=fopen("debug.txt","w");
	setbuf(fp,NULL);
    }

    if (!fp)
    	return;

    va_start(ap,format);
    vfprintf(fp,format,ap);
    va_end(ap);
}

/* END OF FILE */
