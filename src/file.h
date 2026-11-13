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
#ifndef EZX81_FILE_H
#define EZX81_FILE_H

/* Opens a file, displaying an error message via the GUI routines if it fails.
   Set for_write to TRUE to open a file for writing.
   If magic is not NULL then the start of the file is checked for that string
   if it is opened for reading.  If it is opened for writing that string is
   written.
*/
void		*FILEOpen(const char *path, int for_write, const char *magic);


/* Close a file
*/
void		FILEClose(void *file);


/* Write a string to the file.
*/
void		FILEWriteString(void *file, const char *string);


/* Write a long to the file.
*/
void		FILEWriteNumber(void *file, long number);


/* Read a string from the file.  The return is allocated and must be freed
   with free().
*/
char		*FILEReadString(void *file);


/* Read a long from the file.
*/
long		FILEReadNumber(void *file);


#endif
