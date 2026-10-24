/*

    espec - Sinclair Spectrum emulator

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

    Convert an SDL keysym into a char

*/
#include <ctype.h>
#include "symtochar.h"

/* ---------------------------------------- GLOBALS
*/
static struct
{
    SDL_Keycode sym;
    char        c;
} map[] =
{
    {SDLK_1,		'1'},
    {SDLK_2,		'2'},
    {SDLK_3,		'3'},
    {SDLK_4,		'4'},
    {SDLK_5,		'5'},
    {SDLK_6,		'6'},
    {SDLK_7,		'7'},
    {SDLK_8,		'8'},
    {SDLK_9,		'9'},
    {SDLK_0,		'0'},

    {SDLK_a,		'a'},
    {SDLK_b,		'b'},
    {SDLK_c,		'c'},
    {SDLK_d,		'd'},
    {SDLK_e,		'e'},
    {SDLK_f,		'f'},
    {SDLK_g,		'g'},
    {SDLK_h,		'h'},
    {SDLK_i,		'i'},
    {SDLK_j,		'j'},
    {SDLK_k,		'k'},
    {SDLK_l,		'l'},
    {SDLK_m,		'm'},
    {SDLK_n,		'n'},
    {SDLK_o,		'o'},
    {SDLK_p,		'p'},
    {SDLK_q,		'q'},
    {SDLK_r,		'r'},
    {SDLK_s,		's'},
    {SDLK_t,		't'},
    {SDLK_u,		'u'},
    {SDLK_v,		'v'},
    {SDLK_w,		'w'},
    {SDLK_x,		'x'},
    {SDLK_y,		'y'},
    {SDLK_z,		'z'},

    {SDLK_SPACE,	' '},

    {SDLK_EXCLAIM,	'!'},
    {SDLK_QUOTEDBL,	'"'},
    {SDLK_HASH,		'#'},
    {SDLK_PERCENT,	'%'},
    {SDLK_DOLLAR,	'$'},
    {SDLK_AMPERSAND,	'&'},
    {SDLK_QUOTE,	'\''},
    {SDLK_LEFTPAREN,	'('},
    {SDLK_RIGHTPAREN,	')'},
    {SDLK_ASTERISK,	'*'},
    {SDLK_PLUS,		'+'},
    {SDLK_COMMA,	','},
    {SDLK_MINUS,	'-'},
    {SDLK_PERIOD,	'.'},
    {SDLK_SLASH,	'/'},
    {SDLK_COLON,	':'},
    {SDLK_SEMICOLON,	';'},
    {SDLK_LESS,		'<'},
    {SDLK_EQUALS,	'='},
    {SDLK_GREATER,	'>'},
    {SDLK_QUESTION,	'?'},
    {SDLK_AT,		'@'},
    {SDLK_LEFTBRACKET,	'['},
    {SDLK_BACKSLASH,	'\\'},
    {SDLK_RIGHTBRACKET,	']'},
    {SDLK_CARET,	'^'},
    {SDLK_UNDERSCORE,	'_'},
    {SDLK_BACKQUOTE,	'`'},

    {SDLK_UNKNOWN,	0}
};

/* ---------------------------------------- INTERFACES
*/

char SYMToChar(SDL_Keycode keysym)
{
    int f;

    for(f = 0; map[f].sym != SDLK_UNKNOWN; f++)
    {
        if (map[f].sym == keysym)
        {
            return map[f].c;
        }
    }

    return 0;
}
