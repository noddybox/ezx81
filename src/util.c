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

    Usual library wrappers and utils

*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include <unistd.h>

#include "util.h"
#include "exit.h"


/* ---------------------------------------- MACROS
*/
#define DIRSEP	'/'


/* ---------------------------------------- INTERFACES
*/

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
    const char *p = path + strlen(path);

    while(p > path && *p == DIRSEP)
    	p--;

    if (p > path)
    	p++;

    return p;
}


int CheckExtension(const char *path, const char *extension)
{
    const char *p;

    if (!extension)
    {
    	return TRUE;
    }

    p = strrchr(path, '.');

    if (!p)
    {
    	return FALSE;
    }

    return Compare(p + 1, extension);
}


const char *Dirname(const char *path)
{
    static char dir[MAX_PATH_BUFF];
    char *p;

    strcpy(dir, path);

    p = strrchr(dir, DIRSEP);

    if (p)
    {
    	*++p = 0;
    }
    else
    {
    	strcpy(dir,".");
    }

    return dir;
}


const char *ReplaceExtension(const char *path, const char *extension)
{
    static char result[MAX_PATH_BUFF];
    char *p;

    strcpy(result, path);

    p = strrchr(result, '.');

    if (p)
    {
    	*++p = 0;
	strcat(result, extension);
    }

    return result;
}


void Debug(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
}


int Compare(const char *a, const char *b)
{
    while(*a && *b &&
    		tolower((unsigned char)*a) == tolower((unsigned char)*b))
    {
	a++;
	b++;
    }

    return *a ==0 && *b == 0;
}


int StartsWith(const char *a, const char *b, size_t len)
{
    while(len && *a && *b &&
    		tolower((unsigned char)*a) == tolower((unsigned char)*b))
    {
    	len--;
	a++;
	b++;
    }

    return len == 0;
}


int FileExists(const char *path)
{
    return access(path, F_OK) == 0;
}
