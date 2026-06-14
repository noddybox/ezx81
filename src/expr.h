/*
    expr - Simple, expression evaluator

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

    This is a simple expression evaluator, supporting user supplied variables.

    It works on longs, but a floating point version would be easy enough to
    generate from this.

    NB: THIS CODE IS NOT THREAD SAFE
*/

#ifndef EXPR_H
#define EXPR_H

/* ---------------------------------------- INTERFACES
*/

/* Returns the result of expr and stores the answer in result.
   Returns FALSE on error.

   If expand_var is not NULL then unrecognised strings of characters
   (basically anything that gives strtol() problems) are passed to that
   to expand the value.  The return of this routine should be TRUE if the
   value was expanded into value.  The client_data pointer is passed to
   expand_var().
*/
int		ExprEval(const char *expr, long *result,
			 int (*expand_var)(void *client_data,
			 		   const char *p,
					   long *value),
			 void *client_data);


/* Gets a readable reason for an error from ExprEval() or ExprParse.
*/
const char	*ExprError(void);


#endif

/* END OF FILE */
