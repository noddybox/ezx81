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

    Provides the emulation for the ZX81

*/
static const char ident[]="$Id$";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "zx81.h"
#include "config.h"
#include "exit.h"

static const char ident_h[]=EZX81ZX81H;


/* ---------------------------------------- STATICS
*/
static const int	ROMLEN=0x2000;
static const int	ROM_SAVE=0x2fc;
static const int	ROM_LOAD=0x347;

/* These assume a 320x200 screen
*/
static const int	OFF_X=(320-256)/2;
static const int	OFF_Y=(200-192)/2;

static Z80Byte		mem[0x10000];

static Z80Word		RAMBOT=0;
static Z80Word		RAMTOP=0;
static Z80Word		RAMLEN=0;


/* ---------------------------------------- PRIVATE FUNCTIONS
*/
void ULA_Video_Shifter(Z80 *z80, Z80Byte val)
{
}


/* ---------------------------------------- EXPORTED INTERFACES
*/
void ZX81Init(void)
{
    FILE *fp;
    Z80Word f;

    if (!(fp=fopen(SConfig(CONF_ROMFILE),"rb")))
    	Exit("Failed to open ZX81 ROM - %s\n",SConfig(CONF_ROMFILE));

    if (fread(mem,1,ROMLEN,fp)!=ROMLEN)
    {
    	fclose(fp);
	Exit("ROM file must be %d bytes long\n",ROMLEN);
    }

    /* Mirror the ROM
    */
    memcpy(mem+ROMLEN,mem,ROMLEN);

    RAMBOT=0x4000;

    /* Memory size (1 or 16K)
    */
    if (IConfig(CONF_MEMSIZE)==16)
	RAMLEN=0x2000;
    else
	RAMLEN=0x400;

    RAMTOP=RAMBOT+RAMLEN;

    for(f=RAMBOT;f<=RAMTOP;f++)
    	mem[f]=0;
}


Z80Byte ZX81ReadMem(Z80 *z80, Z80Word addr)
{
    /* Memory reads above 32K invoke the ULA
    */
    if (addr>0x7fff)
    {
	Z80Byte b;

        /* B6 of R is tied to the IRQ line (only when HSYNC active?)
        if ((HSYNC)&&(!(z80->R&0x40)))
            z80->IRQ=TRUE;
        */

        b=mem[addr&0x7fff];

        /* If bit 6 of the opcode is set the opcode is sent as is to the
           Z80.  If it's not, the byte is interretted by the ULA.
        */
        if (b&0x40)
	{
            ULA_Video_Shifter(z80,0);
	}
        else
	{
            ULA_Video_Shifter(z80,b);
            b=0;
	}

	return b;
    }
    else
        return mem[addr&0x7fff];

}


void ZX81WriteMem(Z80 *z80, Z80Word addr, Z80Byte val)
{
    if (addr>=RAMBOT && addr<=RAMTOP)
	mem[addr&0x7fff]=val;
}


Z80Byte ZX81ReadPort(Z80 *z80, Z80Word port)
{
    return 0;
}


void ZX81WritePort(Z80 *z80, Z80Word port, Z80Byte val)
{
}


Z80Byte ZX81ReadForDisassem(Z80 *z80, Z80Word addr)
{
    return mem[addr&0x7fff];
}


/* END OF FILE */
