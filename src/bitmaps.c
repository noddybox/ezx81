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

    Bitmap storage

*/
#include "bitmaps.h"
#include "kbbmp.h"
#include "exit.h"

static GFX_Bitmap bitmaps[BMP_NumBitmaps];

void BMPInit(void)
{
    if (GFX_Bitmap_Decode(keyboard_bitmap,
    			  KEYBOARD_BITMAP_LEN,
			  &bitmaps[BMP_Keyboard]) != eGFX_Ok)
    {
	Exit("error: failed to decoded keyboard bitmap\n");
    }
}

const GFX_Bitmap *BMPGet(BitmapSelector bmp)
{
    return bitmaps + bmp;
}
