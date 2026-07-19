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

    ----------------------------------------------------------------

    Provides the emulation for the ZX81

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "zx81.h"
#include "gfx.h"
#include "gui.h"
#include "config.h"
#include "util.h"
#include "exit.h"


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

#define ED_SAVE		0xf0
#define ED_LOAD		0xf1
#define ED_WAITKEY	0xf2
#define ED_ENDWAITKEY	0xf3
#define ED_PAUSE	0xf4

#define SLOW_TSTATES	16000
#define FAST_TSTATES	(64000*10)

#define E_LINE		16404
#define LASTK1		16421
#define LASTK2		16422
#define MARGIN		16424
#define FRAMES		16436
#define CDFLAG		16443
#define DFILE           0x400c

static Z80Val		FRAME_TSTATES = FAST_TSTATES;

/* The ZX81 screen and emulation
*/
static int		scr_enable=TRUE;

#define	SCR_W		256
#define	SCR_H		192
#define	TXT_W		32
#define	TXT_H		24

#define OFF_X		(GFX_WIDTH-SCR_W)/2
#define OFF_Y		(GFX_HEIGHT-SCR_H)/2

static int		waitkey=FALSE;
static int		started=FALSE;

static int		last_I;

static int              fast_mode=FALSE;

/* Memory
*/
static Z80Byte		mem[0x10000];

static Z80Word		RAMBOT=0;
static Z80Word		RAMTOP=0;
static Z80Word		RAMLEN=0;

/* GFX vars
*/
static Uint32		white;
static Uint32		black;


/* The keyboard
*/
static Z80Byte		matrix[8];

static unsigned		prev_lk1;
static unsigned		prev_lk2;

typedef struct
{
    SDL_Keycode	key;
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
#define PEEKW(addr)		(mem[addr] | (Z80Word)mem[addr+1]<<8)

#define POKEW(addr,val)         do					\
                                {					\
                                    Z80Word wa=addr;			\
                                    Z80Word wv=val;			\
                                    mem[wa]=wv;				\
                                    mem[wa+1]=wv>>8;			\
                                } while(0)

static void InstallRomPatch(Z80Word address, const Z80Byte patch[])
{
    for(int f = 0; patch[f] != 0xff; f++)
    {
    	mem[address + f] = patch[f];
    }
}

static void RomPatch(void)
{
    static const Z80Byte save[]=
    {
    	0xed, ED_SAVE,		/* ED_SAVE illegal op	*/
	0xc3, 0x07, 0x02,	/* JP $0207		*/
	0xff			/* End of patch		*/
    };

    static const Z80Byte load[]=
    {
    	0xed, ED_LOAD,		/* ED_LOAD illegal op	*/
	0xc3, 0x07, 0x02,	/* JP $0207		*/
	0xff			/* End of patch		*/
    };

    static const Z80Byte fast_hack[]=
    {
	0xed, ED_WAITKEY,	/* (START KEY WAIT)	*/
	0xcb,0x46,		/* L: bit 0,(hl)	*/
	0x28,0xfc,		/* jr z,L		*/
	0xed, ED_ENDWAITKEY,	/* (END KEY WAIT)	*/
    	0x00,			/* nop			*/
	0xff			/* End of patch		*/
    };

    static const Z80Byte kbd_hack[]=
    {
	0x2a,0x25,0x40,		/* ld hl,(LASTK)	*/
	0xc9,			/* ret			*/
	0xff			/* End of patch		*/
    };

    static const Z80Byte pause_hack[]=
    {
	0xed, ED_PAUSE,		/* (PAUSE)		*/
    	0x00,			/* nop			*/
	0xff			/* End of patch		*/
    };

    InstallRomPatch(ROM_SAVE, save);
    InstallRomPatch(ROM_LOAD, load);
    InstallRomPatch(0x4ca, fast_hack);
    InstallRomPatch(0x2bb, kbd_hack);
    InstallRomPatch(0xf3a, pause_hack);

    mem[0x21c]=0x00;
    mem[0x21d]=0x00;

    /* Remove HALTs as we don't do interrupts
    */
    mem[0x0079]=0;
    mem[0x02ec]=0;
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


static void LoadTape(Z80 *cpu)
{
    Z80State state;
    const char *p;
    char path[FILENAME_MAX];
    FILE *fp;
    Z80Word addr;
    int c;

    Z80GetState(cpu, &state);

    p=ConvertFilename(state.DE);

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


static void SaveTape(Z80 *cpu)
{
    Z80State state;
    const char *p;
    char path[FILENAME_MAX];
    FILE *fp;
    Z80Word start;
    Z80Word end;

    Z80GetState(cpu, &state);

    p=ConvertFilename(state.DE);

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


static Z80Word FindHiresDFILE(void)
{
    /* Somewhat based on the code from xz81, an X-based ZX81 emulator,
       (C) 1994 Ian Collier.  Search the ZX81's RAM until we find what looks
       like a hi-res display file.

       Bizarrely the original code used 'f' for a loop counter too...  Another
       poor soul forever damaged by the ZX81's keyword entry system...
    */
    int f;

    for(f=0x8000-(33*192); f>0x4000 ; f--)
    {
	int v;

	v = mem[f+32];

	if (v&0x40)
	{
	    int ok = TRUE;
	    int n;

	    for(n=0;n<192 && ok;n++)
	    {
	    	if (mem[f+33*n]&0x40)
		{
		    ok = FALSE;
		}

		if (mem[f+32+33*n] != v)
		{
		    ok = FALSE;
		}
	    }

	    if (ok)
	    {
	    	return f;
	    }
	}
    }

    /* All else fails, put the hires dfile at 0x4000 -- at least it should be
       obvious that the hires won't work for whatever is being run.
    */
    return 0x4000;
}


/* Perform ZX81 housekeeping functions like updating FRAMES and updating LASTK
*/
static void ZX81HouseKeeping(Z80 *z80)
{
    unsigned row;
    unsigned lastk1;
    unsigned lastk2;

    /* British ZX81
    */
    mem[MARGIN] = 55;

    /* Update FRAMES
    */
    if (FRAME_TSTATES == SLOW_TSTATES)
    {
    	Z80Word frame = PEEKW(FRAMES) & 0x7fff;

	if (frame)
	{
	    frame--;
	}

	POKEW(FRAMES,frame|0x8000);
    }

    if (!started)
    {
    	prev_lk1 = 0;
    	prev_lk2 = 0;
	return;
    }

    /* Update LASTK
    */
    lastk1 = 0;
    lastk2 = 0;

    for(row = 0; row < 8; row++)
    {
    	unsigned b;

	b = (~matrix[row]&0x1f)<<1;

	if (row == 0)
	{
	    unsigned shift;

	    shift=b&2;
	    b&=~2;
	    b|=(shift>>1);
	}

	if (b)
	{
	    if (b>1)
	    {
		lastk1|=(1<<row);
	    }

	    lastk2|=b;
	}
    }

    if (lastk1 && (lastk1 != prev_lk1 || lastk2 != prev_lk2))
    {
    	mem[CDFLAG]|=1;
    }
    else
    {
    	mem[CDFLAG]&=~1;
    }

    mem[LASTK1] = lastk1^0xff;
    mem[LASTK2] = lastk2^0xff;

    prev_lk1 = lastk1;
    prev_lk2 = lastk2;
}


static void DrawScreenText(Z80State *state)
{
    int x,y;
    int table;
    Z80Byte *scr;

    GFXStartFrame();
    GFXClear(white);

    table = state->I << 8;
    scr = mem + PEEKW(DFILE);

    y = 0;

    while(y < TXT_H)
    {
	scr++;
	x = 0;

	/* 118 is the opcode for HALT
	*/
	while(*scr != 118 && x < TXT_W)
	{
	    int row;
	    int c = *scr++;

	    for(row = 0; row < 8; row++)
	    {
		int v = mem[table + (c & 0x3f) * 8 + row];
		int b;

		if (c & 0x80)
		{
		    v ^= 0xff;
		}

		for(b = 0; b < 8; b++)
		{
		    GFXPlot(OFF_X + x * 8 + b, OFF_Y + y * 8 + row,
		    		(v & 0x80) ? black : white);
		    v = v << 1;
		}
	    }

	    x++;
	}

	y++;
    }

    GFXEndFrame(TRUE);
}


static void DrawScreenHires(Z80State *state, Z80Word hires_dfile)
{
    int x,y;
    int table;
    Z80Byte *scr;

    GFXStartFrame();
    GFXClear(white);

    table = state->I << 8;
    scr = mem + hires_dfile;

    for(y = 0; y < SCR_H; y++)
    {
	for(x = 0; y < SCR_W; x++)
	{
	    int c = *scr++;
	    int v = mem[table + (c & 0x3f) * 8];
	    int b;

	    if (c & 0x80)
	    {
	    	v ^= 0xff;
	    }

	    for(b = 0; b < 8; b++)
	    {
	    	GFXPlot(OFF_X + x, OFF_Y + y, (v & 0x80) ? black : white);
		v = v << 1;
	    }
	}
    }

    GFXEndFrame(TRUE);
}


static void DrawSnow(void)
{
    int x,y;

    GFXStartFrame();

    for(y = 0; y < GFX_HEIGHT; y++)
    {
	for(x = 0; x < GFX_WIDTH; x++)
	{
	    GFXPlot(x, y, rand() & 1 ? black : white);
	}
    }

    GFXEndFrame(TRUE);
}


static int CheckTimers(Z80 *z80, Z80Val val)
{
    Z80Val current_frame_tstates = FRAME_TSTATES;

    if (val >= FRAME_TSTATES)
    {
	Z80State state;
	Z80Word hires_dfile = 0;

	Z80GetState(z80, &state);

    	/* Check for hi-res modes
	*/
	if (state.I && state.I != last_I)
	{
	    last_I = state.I;

	    if (state.I != 0x1e)
	    {
		hires_dfile = FindHiresDFILE();
	    }
	}

	if (started && ((mem[CDFLAG] & 0x80) || waitkey || hires_dfile))
	{
	    fast_mode = FALSE;
	    FRAME_TSTATES=SLOW_TSTATES;

	    if (hires_dfile)
	    {
		DrawScreenHires(&state, hires_dfile);
	    }
	    else
	    {
		DrawScreenText(&state);
	    }
	}
	else
	{
            fast_mode = TRUE;
	    FRAME_TSTATES=FAST_TSTATES;
	    DrawSnow();
	}

	/* Update FRAMES (if in SLOW) and scan the keyboard.  This only happens
	   once we've got to a decent point in the boot cycle (detected with
	   a valid stack pointer).
	*/
	if (state.SP < 0x8000)
	{
	    ZX81HouseKeeping(z80);
	}

	Z80ResetCycles(z80, val - current_frame_tstates);
    }

    return TRUE;
}


static int EDCallback(Z80 *z80, Z80Val data)
{
    Z80State state;
    Z80Word pause;

    Z80GetState(z80, &state);

    switch((Z80Byte)data)
    {
    	case ED_SAVE:
	    SaveTape(z80);
	    break;

    	case ED_LOAD:
	    LoadTape(z80);
	    break;

	case ED_WAITKEY:
	    waitkey = TRUE;
	    started = TRUE;
	    break;

	case ED_ENDWAITKEY:
	    waitkey = FALSE;
	    break;

	case ED_PAUSE:
	    waitkey = TRUE;

	    pause = state.BC;

	    while(--pause && !(mem[CDFLAG] & 1))
	    {
		SDL_Event *e;

		while((e = GFXGetKey()))
		{
		    ZX81KeyEvent(e);
		}

		CheckTimers(z80, FRAME_TSTATES);
	    }

	    waitkey = FALSE;
	    break;

	default:
	    break;
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

    /* Mirror the ROM
    */
    memcpy(mem+ROMLEN,mem,ROMLEN);

    RAMBOT=0x4000;

    /* Memory size (1 or 16K)
    */
    if (IConfig(CONF_MEMSIZE)==16)
	RAMLEN=0x4000;
    else
	RAMLEN=0x400;

    RAMTOP=RAMBOT+RAMLEN;

    for(f=RAMBOT;f<=RAMTOP;f++)
    	mem[f]=0;

    for(f=0;f<8;f++)
    	matrix[f]=0x1f;

    white=GFXRGB(230,230,230);
    black=GFXRGB(0,0,0);
}


void ZX81KeyEvent(SDL_Event *e)
{
    const MatrixMap *m;

    if (e->key.repeat)
    {
        return;
    }

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

    /* Debug("IN  %4.4x\n",port); */

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
	    break;

	default:
	    break;
    }

    return b;
}


void ZX81WritePort(Z80 *z80, Z80Word port, Z80Byte val)
{
    /* Debug("OUT %4.4x,%2.2X\n",port,val); */
}


Z80Byte ZX81ReadForDisassem(Z80 *z80, Z80Word addr)
{
    return mem[addr&0x7fff];
}


const char *ZX81Info(Z80 *z80)
{
    static char buff[80];

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

    for(f=0;f<8;f++)
    	matrix[f]=0;
}


/* END OF FILE */
