/*

    emma - Z80 testbed

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

*/
static const char id[]="$Id$";

#include <stdlib.h>
#include <stdio.h>

#include "z80.h"


/* ---------------------------------------- GLOBALS
*/
static Z80Byte	mem[0x10000];


/* ---------------------------------------- PROTOS
*/
static Z80Byte		ReadMem(Z80 *z80, Z80Word addr);
static void		WriteMem(Z80 *z80, Z80Word addr, Z80Byte val);
static Z80Byte		ReadPort(Z80 *z80, Z80Word addr);
static void		WritePort(Z80 *z80, Z80Word addr, Z80Byte val);
static const char	*Label(Z80 *z80, Z80Word addr);


/* ---------------------------------------- MAIN
*/
int main(int argc, char *argv[])
{
    Z80 *z80;
    FILE *fp;
    Z80Word pc=0;

    z80=Z80Init(WriteMem,ReadMem,WritePort,ReadPort,ReadMem,Label);

    if (!z80)
    {
    	printf("Failed to initialise Z80\n");
	return EXIT_FAILURE;
    }

    if ((fp=fopen("/files/emu/ROM/zx81.rom","rb")))
    {
    	fread(mem,1,0x10000,fp);
	fclose(fp);
    }

    while(1)
    {
	Z80Word opc=pc;

    	printf("%4.4x: ",pc);
    	printf("%s\n",Z80Disassemble(z80,&pc));

	if (pc<opc)
	    break;
    }

    return EXIT_SUCCESS;
}


/* ---------------------------------------- MEMORY
*/
static Z80Byte ReadMem(Z80 *z80, Z80Word addr)
{
    return mem[addr];
}


static void WriteMem(Z80 *z80, Z80Word addr, Z80Byte val)
{
    mem[addr]=val;
}


static Z80Byte ReadPort(Z80 *z80, Z80Word addr)
{
    printf("Read from port 0x%4.4x\n",(int)addr);
    return 0;
}


static void WritePort(Z80 *z80, Z80Word addr, Z80Byte val)
{
    printf("Wrote 0x%2.2x to port 0x%4.4x\n",(int)val,(int)addr);
}


static const char *Label(Z80 *z80, Z80Word addr)
{
    if (addr==0x0a2a)
    	return "CLS";

    return NULL;
}


/* END OF FILE */
