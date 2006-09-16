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
#include "gfx.h"
#include "gui.h"
#include "config.h"
#include "util.h"
#include "exit.h"

static const char ident_h[]=EZX81_ZX81H;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/* ---------------------------------------- STATICS
*/
static const int	ROMLEN=0x2000;
static const int	ROM_SAVE=0x2fc;
static const int	ROM_LOAD=0x347;

/* No of T-states in each 64us HSYNC (hopefully)
*/
static const Z80Val	HSYNC_PERIOD=321;

/* The ZX81 screen
*/
static int		scr_enable=TRUE;

#define	SCR_W		256
#define	SCR_H		192
#define	TXT_W		32
#define	TXT_H		24

#define OFF_X		(GFX_WIDTH-SCR_W)/2
#define OFF_Y		(GFX_HEIGHT-SCR_H)/2

static Z80Byte		mem[0x10000];

static Z80Word		RAMBOT=0;
static Z80Word		RAMTOP=0;
static Z80Word		RAMLEN=0;

/* Counter used when triggering the interrupts for the display
*/
static int		nmigen=FALSE;
static int		hsync=FALSE;
static int		vsync=FALSE;

/* The ULA
*/
static struct
{
    int		x;
    int		y;
    int		c;
    int		release;
} ULA;

/* GFX vars
*/
static Uint32		white;
static Uint32		black;


/* The keyboard
*/
static Z80Byte		matrix[8];

typedef struct
{
    SDLKey	key;
    int		m1,b1,m2,b2;
} MatrixMap;

#define KY1(m,b)		m,1<<b,-1,-1
#define KY2(m1,b1,m2,b2)	m1,1<<b1,m2,1<<b2

static const MatrixMap keymap[]=
{
    {SDLK_1,		KY1(3,0)},
    {SDLK_2,		KY1(3,1)},
    {SDLK_3,		KY1(3,2)},
    {SDLK_4,		KY1(3,3)},
    {SDLK_5,		KY1(3,4)},
    {SDLK_6,		KY1(4,4)},
    {SDLK_7,		KY1(4,3)},
    {SDLK_8,		KY1(4,2)},
    {SDLK_9,		KY1(4,1)},
    {SDLK_0,		KY1(4,0)},

    {SDLK_a,		KY1(1,0)},
    {SDLK_b,		KY1(7,4)},
    {SDLK_c,		KY1(0,3)},
    {SDLK_d,		KY1(1,2)},
    {SDLK_e,		KY1(2,2)},
    {SDLK_f,		KY1(1,3)},
    {SDLK_g,		KY1(1,4)},
    {SDLK_h,		KY1(6,4)},
    {SDLK_i,		KY1(5,2)},
    {SDLK_j,		KY1(6,3)},
    {SDLK_k,		KY1(6,2)},
    {SDLK_l,		KY1(6,1)},
    {SDLK_m,		KY1(7,2)},
    {SDLK_n,		KY1(7,3)},
    {SDLK_o,		KY1(5,1)},
    {SDLK_p,		KY1(5,0)},
    {SDLK_q,		KY1(2,0)},
    {SDLK_r,		KY1(2,3)},
    {SDLK_s,		KY1(1,1)},
    {SDLK_t,		KY1(2,4)},
    {SDLK_u,		KY1(5,3)},
    {SDLK_v,		KY1(0,4)},
    {SDLK_w,		KY1(2,1)},
    {SDLK_x,		KY1(0,2)},
    {SDLK_y,		KY1(5,4)},
    {SDLK_z,		KY1(0,1)},

    {SDLK_RETURN,	KY1(6,0)},
    {SDLK_SPACE,	KY1(7,0)},

    {SDLK_COMMA,	KY1(7,1)},	/* In the right place... */
    {SDLK_PERIOD,	KY1(7,1)},	/* ...or the right key... */

    {SDLK_BACKSPACE,	KY2(0,0,4,0)},
    {SDLK_DELETE,	KY2(0,0,4,0)},
    {SDLK_UP,		KY2(0,0,4,3)},
    {SDLK_DOWN,		KY2(0,0,4,4)},
    {SDLK_LEFT,		KY2(0,0,3,4)},
    {SDLK_RIGHT,	KY2(0,0,4,2)},

    {SDLK_RSHIFT,	KY1(0,0)},
    {SDLK_LSHIFT,	KY1(0,0)},

    {SDLK_UNKNOWN,	0,0,0,0},
};


/* ---------------------------------------- PRIVATE FUNCTIONS
*/
static void RomPatch(void)
{
    static const Z80Byte save[]=
    {
    	0xed, 0xf0,		/* ED F0 illegal op	*/
	0xc3, 0x08, 0x02,	/* JP $0207		*/
	0xff			/* End of patch		*/
    };

    static const Z80Byte load[]=
    {
    	0xed, 0xf1,		/* ED F0 illegal op	*/
	0xc3, 0x08, 0x02,	/* JP $0207		*/
	0xff			/* End of patch		*/
    };

    int f;

    for(f=0;save[f]!=0xff;f++)
    	mem[ROM_SAVE+f]=save[f];

    for(f=0;load[f]!=0xff;f++)
    	mem[ROM_LOAD+f]=load[f];
}


static char ToASCII(Z80Byte b)
{
    if (b==0)			/* SPACE */
    	return ' ';
    
    if (b==22)			/* Dash (-) */
    	return '-';

    if (b>=28 && b<=37)		/* 0-9 */
    	return '0'+b-28;

    if (b>=38 && b<=63)		/* A-Z */
    	return 'a'+b-38;

    return 0;
}


static const char *ConvertFilename(Z80Word addr)
{
    static char buff[FILENAME_MAX];
    char *p;

    p=buff;
    *p=0;

    if (addr>0x8000)
    	return buff;

    do
    {
    	char c=ToASCII(mem[addr]&0x7f);

	if (c)
	    *p++=c;

    } while(mem[addr++]<0x80);

    *p=0;

    return buff;
}


static void LoadTape(Z80State *state)
{
    const char *p=ConvertFilename(state->DE);
    char path[FILENAME_MAX];
    FILE *fp;
    Z80Word addr;
    int c;

    if (strlen(p)==0)
    {
    	GUIMessage(eMessageBox,"ERROR","Can't load empty filename");
	return;
    }

    strcpy(path,SConfig(CONF_TAPEDIR));
    strcat(path,"/");
    strcat(path,p);
    strcat(path,".p");

    if (!(fp=fopen(path,"rb")))
    {
    	GUIMessage(eMessageBox,"ERROR","Can't load file:\n%s",path);
	return;
    }

    addr=0x4009;

    while((c=getc(fp))!=EOF)
    {
	if (addr>=0x4000)
	    mem[addr]=(Z80Byte)c;

	addr++;
    }

    fclose(fp);
}


static void SaveTape(Z80State *state)
{
    const char *p=ConvertFilename(state->DE);
    char path[FILENAME_MAX];
    FILE *fp;
    Z80Word start;
    Z80Word end;

    if (strlen(p)==0)
    {
    	GUIMessage(eMessageBox,"ERROR","Can't save empty filename");
	return;
    }

    strcpy(path,SConfig(CONF_TAPEDIR));
    strcat(path,"/");
    strcat(path,p);
    strcat(path,".p");

    if (!(fp=fopen(path,"wb")))
    {
    	GUIMessage(eMessageBox,"ERROR","Can't write file:\n%s",path);
	return;
    }

    start=0x4009;
    end=(Z80Word)mem[0x4014]|(Z80Word)mem[0x4015]<<8;

    while(start<=end)
    	putc(mem[start++],fp);

    fclose(fp);
}


static int EDCallback(Z80 *z80, Z80Val data)
{
    Z80State state;

    Z80GetState(z80,&state);

    switch((Z80Byte)data)
    {
    	case 0xf0:
	    SaveTape(&state);
	    break;

    	case 0xf1:
	    LoadTape(&state);
	    break;

	default:
	    break;
    }

    return TRUE;
}


static void ULA_Video_Shifter(Z80 *z80, Z80Byte val)
{
    Z80State state;
    Z80Word base;
    int x,y;
    int inv;
    int b;

    if (!scr_enable)
    	return;

    Z80GetState(z80,&state);

    /* Extra check due to out dodgy ULA emulation
    */
    if (ULA.y>=0 && ULA.y<SCR_H)
    {
	Uint32 fg,bg;

    	/* Position on screen corresponding to ULA
	*/
	x=OFF_X+ULA.x*8;
	y=OFF_Y+ULA.y;

	/* Get ULA invert state and clear to ULA 6-but code
	*/
	inv=val&0x80;
	val&=0x3f;

	base=((Z80Word)state.I<<8)|(val<<3)|ULA.c;

	if (inv)
	{
	    fg=white;
	    bg=black;
	}
	else
	{
	    fg=black;
	    bg=white;
	}

	for(b=0;b<8;b++)
	{
	    if (mem[base]&(1<<(7-b)))
	    	GFXPlot(x+b,y,fg);
	    else
	    	GFXPlot(x+b,y,bg);
	}
    }

    ULA.x=(ULA.x+1)&0x1f;
}


static int CheckTimers(Z80 *z80, Z80Val val)
{
    if (val>HSYNC_PERIOD)
    {
	Z80ResetCycles(z80,val-HSYNC_PERIOD);

	if (nmigen && hsync)
	{
	    Z80NMI(z80);
	    Debug("NMIGEN\n");
	}
	else if (hsync)
	{
	    Debug("HSYNC\n");
	    Z80Interrupt(z80,0xff);
	    if (ULA.release)
	    {
	    	/* ULA.release=FALSE; */
		ULA.c=(ULA.c+1)&7;
		ULA.y++;
		ULA.x=0;
	    }
	}
    }

    return TRUE;
}


static int CheckHalt(Z80 *z80, Z80Val val)
{
    if (val)
    {
	if (!nmigen && !hsync)
	    GUIMessage(eMessageBox,"POSSIBLE BUG",
		       "Executed HALT while NMI generator\n"
		       "and HSYNC generator off");
    }

    return TRUE;
}


/* ---------------------------------------- EXPORTED INTERFACES
*/
void ZX81Init(Z80 *z80)
{
    FILE *fp;
    Z80Word f;

    if (!(fp=fopen(SConfig(CONF_ROMFILE),"rb")))
    {
	GUIMessage(eMessageBox,
		   "ERROR","Failed to open ZX81 ROM\n%s",SConfig(CONF_ROMFILE));
	Exit("");
    }

    if (fread(mem,1,ROMLEN,fp)!=ROMLEN)
    {
    	fclose(fp);
	GUIMessage(eMessageBox,
		   "ERROR","ROM file must be %d bytes long\n",ROMLEN);
	Exit("");
    }

    /* Patch the ROM
    */
    RomPatch();
    Z80LodgeCallback(z80,eZ80_EDHook,EDCallback);
    Z80LodgeCallback(z80,eZ80_Instruction,CheckTimers);
    Z80LodgeCallback(z80,eZ80_Halt,CheckHalt);

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

    for(f=0;f<8;f++)
    	matrix[f]=0x1f;

    white=GFXRGB(230,230,230);
    black=GFXRGB(0,0,0);

    nmigen=FALSE;
    hsync=FALSE;

    GFXStartFrame();
}


void ZX81KeyEvent(SDL_Event *e)
{
    const MatrixMap *m;

    m=keymap;

    while(m->key!=SDLK_UNKNOWN)
    {
    	if (e->key.keysym.sym==m->key)
	{
	    if (e->key.state==SDL_PRESSED)
	    {
	    	matrix[m->m1]&=~m->b1;

		if (m->m2!=-1)
		    matrix[m->m2]&=~m->b2;
	    }
	    else
	    {
	    	matrix[m->m1]|=m->b1;

		if (m->m2!=-1)
		    matrix[m->m2]|=m->b2;
	    }
	}

	m++;
    }
}


Z80Byte ZX81ReadMem(Z80 *z80, Z80Word addr)
{
    /* Memory reads above 32K invoke the ULA
    */
    if (addr>0x7fff)
    {
	Z80Byte b;

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
    addr=addr&0x7fff;

    if (addr>=RAMBOT && addr<=RAMTOP)
	mem[addr]=val;
}


Z80Byte ZX81ReadPort(Z80 *z80, Z80Word port)
{
    Z80Byte b=0;

    Debug("IN  %4.4x\n",port);

    switch(port&0xff)
    {
    	case 0xfe:	/* ULA */
	    /* Key matrix
	    */
	    switch(port&0xff00)
	    {
	    	case 0xfe00:
		    b=matrix[0];
		    break;
	    	case 0xfd00:
		    b=matrix[1];
		    break;
	    	case 0xfb00:
		    b=matrix[2];
		    break;
	    	case 0xf700:
		    b=matrix[3];
		    break;
	    	case 0xef00:
		    b=matrix[4];
		    break;
	    	case 0xdf00:
		    b=matrix[5];
		    break;
	    	case 0xbf00:
		    b=matrix[6];
		    break;
	    	case 0x7f00:
		    b=matrix[7];
		    break;
	    }

	    /* Turn on VSYNC if not on
	    */
	    if (!vsync)
	    {
		Debug("VSYNC\n");

		GFXEndFrame(TRUE);
		GFXClear(white);

		ULA.x=0;
		ULA.y=-1;
		ULA.c=7;
		ULA.release=FALSE;

		GFXStartFrame();
		vsync=TRUE;
	    }
	    else
	    {
	    	/* Reset and hold ULA counter
		*/
		ULA.c=0;
		ULA.release=FALSE;
	    }

	    /* If NMI on, turn off the HSYNC generator
	    */
	    if (!nmigen)
	    {
		Debug("HSYNC OFF\n");
	    	hsync=FALSE;
	    }
	    break;

	default:
	    break;
    }

    return b;
}


void ZX81WritePort(Z80 *z80, Z80Word port, Z80Byte val)
{
    Debug("OUT %4.4x,%2.2X\n",port,val);

    /* Any port write releases the ULA line counter
    */
    ULA.release=TRUE;

    switch(port&0xff)
    {
    	case 0xfd:	/* NMI generator OFF */
	    Debug("NMIGEN OFF\n");
	    nmigen=FALSE;
	    break;

	case 0xfe:	/* NMI generator ON */
	    Debug("NMIGEN ON/VSYNC OFF\n");
	    nmigen=TRUE;
	    vsync=FALSE;
	    break;

	case 0xff:	/* HSYNC generator ON */
	    Debug("HSYNC ON\n");
	    hsync=TRUE;
	    Z80ResetCycles(z80,0);
	    break;
    }
}


Z80Byte ZX81ReadForDisassem(Z80 *z80, Z80Word addr)
{
    return mem[addr&0x7fff];
}


const char *ZX81Info(Z80 *z80)
{
    static char buff[80];

    sprintf(buff,"NMI: %s  HS: %s  ULA: (%d,%d,%d,%d)",
		    nmigen ? "ON":"OFF",
		    hsync ? "ON":"OFF",
		    ULA.x,ULA.y,ULA.c,ULA.release);

    return buff;
}


void ZX81EnableScreen(int enable)
{
    scr_enable=enable;
}


void ZX81Reset(Z80 *z80)
{
    int f;

    scr_enable=TRUE;
    nmigen=FALSE;
    hsync=FALSE;

    for(f=0;f<8;f++)
    	matrix[f]=0;

    GFXStartFrame();
}


/* END OF FILE */
