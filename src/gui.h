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

#ifndef EZX81_GUI_H
#define EZX81_GUI_H "$Id$"

#include "SDL.h"


/* ---------------------------------------- INTERFACES
*/

/* Message types
*/
typedef enum {eMessageBox, eYesNoBox} GUIBoxType;

/* Display a simple message box.  A message of longer than 1024 bytes causes
   undefined behaviour.  Newlines cause a line break.  If a line is over 38
   characters then it will be truncated.

   If type is eYesNoBox then TRUE/FALSE is returned depending on whether
   yes/no was selected.
*/
int		GUIMessage(GUIBoxType type,
			   const char *title,
			   const char *format,...);


/* Enter a string, max 40 characters
*/
const char	*GUIInputString(const char *prompt, const char *orig);


/* Selects an entry from a list.  Returns the index selected, or
   -1 if cancelled.
*/
int		GUIListSelect(const char *title, int no, char * const list[]);


/* Allows options to be toggled in a list.  Returns FALSE for cancelled (in
   which case option will be as it was.  TRUE if accepted, and option will be
   updated.
*/
int		GUIListOption(const char *title,
			      int no, char * const list[], int option[]);


#endif


/* END OF FILE */
