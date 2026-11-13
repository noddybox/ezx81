/*

    ezx81 - ZX81 emulator

    Copyright (C) 2026  Ian Cowburn (deathstation9000@gmail.com)

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

    File routines to read strings and numbers.

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "file.h"
#include "gui.h"
#include "util.h"

#define EZX81_FILE_H

/* ---------------------------------------- EXPORTED FUNCTIONS
*/
void *FILEOpen(const char *path, int for_write, const char *magic)
{
    FILE *fp;

    fp = fopen(path, for_write ? "w" : "r");

    if (!fp)
    {
    	GUIMessage(eMessageBox, "ERROR", "Unable to open\n%s", path);
	return NULL;
    }

    if (magic)
    {
    	if (for_write)
	{
	    FILEWriteString(fp, magic);
	}
	else
	{
	    char *p;
	    int ok;

	    p = FILEReadString(fp);

	    ok = (strcmp(p, magic) == 0);

	    free(p);

	    if (!ok)
	    {
		GUIMessage(eMessageBox, "ERROR", "File of wrong type", path);
		return NULL;
	    	
	    }
	}
    }

    return fp;
}


void FILEClose(void *file)
{
    fclose(file);
}


void FILEWriteString(void *file, const char *string)
{
    size_t len = strlen(string);

    FILEWriteNumber(file, (long)len);
    fwrite(string, 1, len, file);
}


void FILEWriteNumber(void *file, long number)
{
    fprintf(file, "%ld\n", number);
}


char *FILEReadString(void *file)
{
    size_t len = FILEReadNumber(file);
    char *p = Malloc(len + 1);

    fread(p, 1, len, file);
    p[len] = 0;

    return p;
}


long FILEReadNumber(void *file)
{
    char buff[80];

    fgets(buff, sizeof buff, file);

    return atol(buff);
}
