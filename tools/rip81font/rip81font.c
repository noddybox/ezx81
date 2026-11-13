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

    Utility to rip ZX81 font into FontChar definitions

*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

static void OutputChar(FILE *fp, unsigned char *def, unsigned code, int invert)
{
    fprintf(fp, "    {\t\t\t/* %2.2x */\n", code);

    for(int f = 0 ; f < 8; f++)
    {
	unsigned char val = *def++;

	if (invert)
	{
	    val = ~val;
	}

	fprintf(fp, "\t");

    	for(int b = 7; b > -1 ; b--)
	{
	    if (val & (1 << b))
	    {
	    	fprintf(fp, "1,");
	    }
	    else
	    {
	    	fprintf(fp, "0,");
	    }
	}

	fprintf(fp, "\n");
    }

    fprintf(fp, "    },\n");
}

int main(int argc, char *argv[])
{
    unsigned char rom[0x2000] = {0};
    unsigned char undefined[8] =
    {
	0x00,
	0x18,
	0x24,
	0x42,
	0x42,
	0x24,
	0x18,
	0x00,
    };

    if (argc != 2)
    {
    	fprintf(stderr, "usage: rip81font rom_file\n");
	return EXIT_FAILURE;
    }

    FILE *fp = fopen(argv[1], "rb");

    if (!fp)
    {
    	perror(argv[1]);
	return EXIT_FAILURE;
    }

    fread(rom, 1, sizeof rom, fp);

    fclose(fp);

    unsigned char *font = rom + 0x1e00;
    FILE *out = fopen("output.c", "w");

    if (!out)
    {
    	perror(argv[1]);
	return EXIT_FAILURE;
    }

    fprintf(out, "const FontChar zx81font[256] =\n{\n");

    /* Character 0-63 are the normal, un-inverted characters
    */
    for (unsigned f = 0; f < 64; f++)
    {
    	OutputChar(out, font + f * 8, f, 0);
    }

    /* Character 64-127 are the normal, un-inverted characters
    */
    for (unsigned f = 64; f < 128; f++)
    {
    	OutputChar(out, undefined, f, 0);
    }

    /* Character 128-191 are the normal, inverted characters
    */
    for (unsigned f = 128; f < 192; f++)
    {
    	OutputChar(out, font + (f - 128) * 8, f, 1);
    }

    /* Character 192-255 are the normal, un-inverted characters
    */
    for (unsigned f = 192; f < 256; f++)
    {
    	OutputChar(out, undefined, f, 1);
    }

    fprintf(out, "};\n");

    fclose(out);

    return EXIT_SUCCESS;
}
