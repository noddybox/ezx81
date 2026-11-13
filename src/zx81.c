/*

    ezx81 - ZX81 emulator

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
#include "joystick.h"
#include "bitmaps.h"
#include "util.h"
#include "exit.h"
#include "file.h"


/* ---------------------------------------- STATICS
*/
#define ROMLEN		0x2000
#define ROM_SAVE	0x2fc
#define ROM_LOAD	0x347

#define ED_SAVE		0xf0
#define ED_LOAD		0xf1
#define ED_WAITKEY	0xf2
#define ED_ENDWAITKEY	0xf3
#define ED_PAUSE	0xf4

#define HSYNC_TSTATES	207
#define FAST_TSTATES	(64000*10)

#define E_LINE		16404
#define LASTK1		16421
#define LASTK2		16422
#define MARGIN		16424
#define FRAMES		16436
#define CDFLAG		16443
#define DFILE           0x400c

#define YESNO(x)	((x) ? 'Y':'N')

/* The ZX81 screen and emulation
*/
static int		scr_enable=TRUE;

#define	SCR_W		256
#define	SCR_H		192
#define	TXT_W		32
#define	TXT_H		24
#define PAL_SCANLINE	310
#define NTSC_SCANLINE	262

#define OFF_X		(GFX_WIDTH-SCR_W)/2
#define OFF_Y		(GFX_HEIGHT-SCR_H)/2

static int		waitkey=FALSE;
static int		started=FALSE;

static int		last_I;

static int              fast_mode=FALSE;

static int		nmi_generator = FALSE;

static struct
{
    int		x;
    int		y;
    int		row;
    int		used;
    int		in_vsync;
} video = {0};

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


/* Joysticks
*/
typedef struct
{
    Z80Byte	row;
    Z80Byte	bit;
} Joykey;

static Joykey		joystick[JOY_NumButtons] =
{
    {4, 1 << 2},		/* JOY_Right */
    {3, 1 << 4},		/* JOY_Left */
    {4, 1 << 3},		/* JOY_Up */
    {4, 1 << 4},		/* JOY_Down */
    {4, 1 << 0},		/* JOY_Button_X */
    {4, 1 << 0},		/* JOY_Button_Y */
    {4, 1 << 0},		/* JOY_Button_A */
    {4, 1 << 0},		/* JOY_Button_B */
    {4, 1 << 0},		/* JOY_Right_Shoulder */
    {4, 1 << 0},		/* JOY_Left_Shoulder */
};

static int		defining_joykeys = FALSE;
static char 		joykey_file[MAX_PATH_BUFF];
static const char	*joyfile_ext = "joy81";
static const char	*joyfile_magic = "!JOY81";
static JoystickButton	last_joystick;


/* The keyboard
*/
static Z80Byte		matrix[8];

static unsigned		prev_lk1;
static unsigned		prev_lk2;

typedef struct
{
    SDL_Keycode	key;
    const char	*display;
    int		m1,b1,m2,b2;
} MatrixMap;

#define KY1(m,b)		m,1<<b,-1,-1
#define KY2(m1,b1,m2,b2)	m1,1<<b1,m2,1<<b2

static const MatrixMap keymap[]=
{
    {SDLK_1,		"1",		KY1(3,0)},
    {SDLK_2,		"2",		KY1(3,1)},
    {SDLK_3,		"3",		KY1(3,2)},
    {SDLK_4,		"4",		KY1(3,3)},
    {SDLK_5,		"5",		KY1(3,4)},
    {SDLK_6,		"6",		KY1(4,4)},
    {SDLK_7,		"7",		KY1(4,3)},
    {SDLK_8,		"8",		KY1(4,2)},
    {SDLK_9,		"9",		KY1(4,1)},
    {SDLK_0,		"0",		KY1(4,0)},

    {SDLK_a,		"A",		KY1(1,0)},
    {SDLK_b,		"B",		KY1(7,4)},
    {SDLK_c,		"C",		KY1(0,3)},
    {SDLK_d,		"D",		KY1(1,2)},
    {SDLK_e,		"E",		KY1(2,2)},
    {SDLK_f,		"F",		KY1(1,3)},
    {SDLK_g,		"G",		KY1(1,4)},
    {SDLK_h,		"H",		KY1(6,4)},
    {SDLK_i,		"I",		KY1(5,2)},
    {SDLK_j,		"J",		KY1(6,3)},
    {SDLK_k,		"K",		KY1(6,2)},
    {SDLK_l,		"L",		KY1(6,1)},
    {SDLK_m,		"M",		KY1(7,2)},
    {SDLK_n,		"N",		KY1(7,3)},
    {SDLK_o,		"O",		KY1(5,1)},
    {SDLK_p,		"P",		KY1(5,0)},
    {SDLK_q,		"Q",		KY1(2,0)},
    {SDLK_r,		"R",		KY1(2,3)},
    {SDLK_s,		"S",		KY1(1,1)},
    {SDLK_t,		"T",		KY1(2,4)},
    {SDLK_u,		"U",		KY1(5,3)},
    {SDLK_v,		"V",		KY1(0,4)},
    {SDLK_w,		"W",		KY1(2,1)},
    {SDLK_x,		"X",		KY1(0,2)},
    {SDLK_y,		"Y",		KY1(5,4)},
    {SDLK_z,		"Z",		KY1(0,1)},

    {SDLK_RETURN,	"Return",	KY1(6,0)},
    {SDLK_SPACE,	"Space",	KY1(7,0)},

    {SDLK_COMMA,	".",		KY1(7,1)},	/* Correct place...   */
    {SDLK_PERIOD,	".",		KY1(7,1)},	/* ...or correct key. */

    {SDLK_BACKSPACE,	NULL,		KY2(0,0,4,0)},
    {SDLK_DELETE,	NULL,		KY2(0,0,4,0)},
    {SDLK_UP,		NULL,		KY2(0,0,4,3)},
    {SDLK_DOWN,		NULL,		KY2(0,0,4,4)},
    {SDLK_LEFT, 	NULL,		KY2(0,0,3,4)},
    {SDLK_RIGHT, 	NULL,		KY2(0,0,4,2)},

    {SDLK_RSHIFT,	"Shift",	KY1(0,0)},
    {SDLK_LSHIFT,	"Shift",	KY1(0,0)},

    {SDLK_UNKNOWN,	NULL,		0,0,0,0},
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

    InstallRomPatch(ROM_SAVE, save);
    InstallRomPatch(ROM_LOAD, load);
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


static void ClearKeys(void)
{
    int f;

    for(f = 0; f < 8; f++)
    {
        matrix[f] = 0xff;
    }
}


static const char *ConvertFilename(Z80Word addr)
{
    static char buff[MAX_PATH_BUFF];
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


static const char *JoystickMapping(JoystickButton button)
{
    const Joykey *joy = joystick + button;

    int f;

    for(f = 0; keymap[f].key != SDLK_UNKNOWN; f++)
    {
	if (keymap[f].m1 == joy->row && keymap[f].b1 == joy->bit &&
	    keymap[f].display)
	{
	    return keymap[f].display;
	}
    }

    return "Unknown!";
}

static void SaveJoystick(const char *path)
{
    void *fp;
    int f;

    fp = FILEOpen(path, TRUE, joyfile_magic);

    if (!fp)
    {
    	return;
    }

    FILEWriteNumber(fp, JOY_NumButtons);

    for(f = 0; f < JOY_NumButtons; f++)
    {
	FILEWriteNumber(fp, joystick[f].row);
	FILEWriteNumber(fp, joystick[f].bit);
    }

    FILEClose(fp);
}


static void LoadJoystick(const char *path)
{
    void *fp;
    long no;
    int f;

    fp = FILEOpen(path, FALSE, joyfile_magic);

    if (!fp)
    {
    	return;
    }

    no = FILEReadNumber(fp);

    if (no > JOY_NumButtons)
    {
    	GUIMessage(eMessageBox, "WARNING", "More buttons defined in file\n"
						"than expected");

	no = JOY_NumButtons;
    }

    for(f = 0; f < no; f++)
    {
	joystick[f].row = FILEReadNumber(fp);
	joystick[f].bit = FILEReadNumber(fp);
    }

    FILEClose(fp);
}


static void LoadTape(Z80 *cpu)
{
    Z80State state;
    const char *p;
    char path[MAX_PATH_BUFF];
    FILE *fp;
    Z80Word addr;
    int c;
    const char *joyfile;

    Z80GetState(cpu, &state);

    p=ConvertFilename(state.DE);

    if (strlen(p)==0)
    {
	static char selected_file[MAX_PATH_BUFF];

	/* Clear the keys as otherwise the ENTER up is consumed
	   by the GUI and the ZX81 thinks the ENTER key is being
	   held down.
	*/
	ClearKeys();

	if (GUIFileSelect("SNAPSHOT FILE TO LOAD",TRUE,
			  selected_file[0] ? Dirname(selected_file) :
					     SConfig(CONF_TAPEDIR),
			  selected_file, "P"))
	{
	    strcpy(path, selected_file);
	}
	else
	{
	    return;
	}
    }
    else
    {
	strcpy(path,SConfig(CONF_TAPEDIR));
	strcat(path,"/");
	strcat(path,p);
	strcat(path,".p");
    }

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

    joyfile = ReplaceExtension(path, joyfile_ext);

    if (FileExists(joyfile))
    {
    	LoadJoystick(joyfile);
    }
}


static void SaveTape(Z80 *cpu)
{
    Z80State state;
    const char *p;
    char path[MAX_PATH_BUFF];
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


static void DrawSnow(void)
{
    int y;
    int x;

    for(y = 0; y < GFX_HEIGHT; y++)
    {
	for(x = 0; x < GFX_WIDTH; x++)
	{
	    GFXPlot(x, y, rand() & 1 ? black : white);
	}
    }
}


static void VideoStartVsync(Z80 *z80)
{
    Debug("In START VSYNC\n");
    video.in_vsync = TRUE;
}


static void VideoEndVsync(Z80 *z80)
{
    Debug("In END VSYNC\n");

    GFXEndFrame(TRUE);
    GFXStartFrame();
    GFXClear(white);

    video.x = 0;
    video.y = 0;
    video.row = 0;
    video.used = FALSE;
    video.in_vsync = FALSE;
}


static void VideoFalseVsync(Z80 *z80)
{
    Debug("In FALSE VSYNC\n");

    DrawSnow();
    GFXEndFrame(TRUE);
    GFXStartFrame();
    GFXClear(white);

    video.x = 0;
    video.y = 0;
    video.row = 0;
    video.used = FALSE;
    video.in_vsync = FALSE;
}


static void VideoHsync(Z80 *z80)
{
    if (nmi_generator)
    {
	Debug("Triggering NMI\n");
    	Z80NMI(z80);
    }

    if (video.row >= mem[MARGIN])
    {
	video.x = 0;
	video.y++;
    }

    video.row++;

    if (fast_mode && video.row == PAL_SCANLINE)
    {
	VideoFalseVsync(z80);
    }

    Debug("In HSYNC (y = %d)\n", video.y);
}


static void VideoShifter(Z80 *z80, Z80Byte chr)
{
    Z80State state;
    int table;
    int char_row;
    int b;
    int v;

    Z80GetState(z80, &state);

    table = state.I << 8;
    char_row = (video.row - mem[MARGIN]) & 7;

    v = mem[table + (chr & 0x3f) * 8 + char_row];

    if (chr & 0x80)
    {
	v ^= 0xff;
    }

    for(b = 0; b < 8; b++)
    {
	GFXPlot(OFF_X + video.x++,
		OFF_Y + video.y,
		(v & 0x80) ? black : white);

	v = v << 1;
    }

    video.used = TRUE;
}


static int CheckTimers(Z80 *z80, Z80Val val)
{
    Z80State state;

    Z80GetState(z80, &state);

    if (state.R & 0x20)
    {
    	Z80Interrupt(z80, 0);
    }

    /* If we're in VSYNC keep resetting the cycle count until vsync ends
    */
    if (video.in_vsync)
    {
	Z80ResetCycles(z80, 0);
    }
    else
    {
	if (val >= HSYNC_TSTATES)
	{
	    VideoHsync(z80);

	    if (started && (mem[CDFLAG] & 0x80))
	    {
		fast_mode = FALSE;
	    }
	    else
	    {
		fast_mode = TRUE;
	    }

	    Z80ResetCycles(z80, val - HSYNC_TSTATES);
	}
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

		CheckTimers(z80, 0);
	    }

	    waitkey = FALSE;
	    break;

	default:
	    break;
    }

    return TRUE;
}


static const char *LastJoystick(void)
{
    static const char *name[] =
    {
    	"Right",
    	"Left",
    	"Up",
    	"Down",
    	"X",
    	"Y",
    	"A",
    	"B",
    	"Right Shoulder",
    	"Left Shoulder",
    };

    return name[last_joystick];
}


static void HandleJoystick(JoystickButton button, int pressed)
{
    if (defining_joykeys)
    {
    	if (pressed)
	{
	    last_joystick = button;
	}
    }
    else
    {
	Joykey *joy = joystick + button;

	if (pressed)
	{
	    matrix[joy->row] &= ~joy->bit;
	}
	else
	{
	    matrix[joy->row] |= joy->bit;
	}
    }
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

    /* Mirror the ROM if no RAM at 0x2000
    */
    if (IConfig(CONF_RAM_AT_2000))
    {
    	RAMLEN = 0x2000;
	RAMBOT = 0x2000;
    }
    else
    {
	memcpy(mem+ROMLEN,mem,ROMLEN);
	RAMLEN = 0;
	RAMBOT = 0x4000;
    }

    /* Memory size (1 or 16K)
    */
    if (IConfig(CONF_MEMSIZE)==16)
	RAMLEN += 0x4000;
    else
	RAMLEN += 0x400;

    RAMTOP=RAMBOT+RAMLEN;

    for(f=RAMBOT;f<=RAMTOP;f++)
    	mem[f]=0;

    for(f=0;f<8;f++)
    	matrix[f]=0xff;

    white=GFXRGB(230,230,230);
    black=GFXRGB(0,0,0);

    if (IConfig(CONF_JOYSTICK))
    {
    	JOYRegister(HandleJoystick);
    }
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
    if (addr > 0x7fff)
    {
    	Z80Byte b = mem[addr & 0x7fff];

	VideoShifter(z80, b & 0x20 ? 0 : b);

	if (b & 0x20)
	{
	    return b;
	}
	else
	{
	    return 0;
	}
    }
    else
    {
	return mem[addr];
    }
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

    started = TRUE;

    if (port & 0x01)
    {
	/* Start vsync
	*/
	if (!video.in_vsync)
	{
	    VideoStartVsync(z80);
	}

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
    }

    return b;
}


void ZX81WritePort(Z80 *z80, Z80Word port, Z80Byte val)
{
    if ((port & 0x0003) == 0x0001)
    {
    	nmi_generator = FALSE;
    	VideoEndVsync(z80);
    }
    else if ((port & 0x0001) == 0x0000)
    {
    	nmi_generator = TRUE;
    	VideoEndVsync(z80);
    }
    else if ((port & 0x00ff) == 0x00ff)
    {
    	VideoEndVsync(z80);
    }

    Debug("NMI generator %c, IN_VSYNC %c\n", YESNO(nmi_generator),
					     YESNO(video.in_vsync));
}


Z80Byte ZX81ReadForDisassem(Z80 *z80, Z80Word addr)
{
    return mem[addr&0x7fff];
}


const Z80Label *ZX81GetLabels(void)
{
    static const Z80Label label[]=
    	{
	    {ROM_SAVE,	"SAVE"},
	    {ROM_LOAD,	"LOAD"},
	    {0x0918,	"PRINT_CHAR"},
	    {0x0a2a,	"CLS"},
	    {0x02bb,	"DISPLAY_5"},
	    {0x0340,	"SLOW"},
	    {0x0420,	"FAST"},
	    {0x0043,	"WAIT_INT"},
	    {16384,	"ERR_NR"},
	    {16385,	"FLAGS"},
	    {16386,	"ERR_SP"},
	    {16388,	"RAMTOP"},
	    {16390,	"MODE"},
	    {16391,	"PPC"},
	    {16393,	"VERSN"},
	    {16394,	"E_PPC"},
	    {16396,	"D_FILE"},
	    {16398,	"DF_CC"},
	    {16400,	"VARS"},
	    {16402,	"DEST"},
	    {16404,	"E_LINE"},
	    {16406,	"CH_ADD"},
	    {16408,	"X_PTR"},
	    {16410,	"STKBOT"},
	    {16412,	"STKEND"},
	    {16414,	"BERG"},
	    {16415,	"MEM"},
	    {16418,	"DF_SZ"},
	    {16419,	"S_TOP"},
	    {16421,	"LAST_K"},
	    {16424,	"MARGIN"},
	    {16425,	"NXTLIN"},
	    {16427,	"OLDPPC"},
	    {16429,	"FLAGX"},
	    {16430,	"STRLEN"},
	    {16432,	"T_ADDR"},
	    {16434,	"SEED"},
	    {16436,	"FRAMES"},
	    {16438,	"COORDS"},
	    {16440,	"PR_CC"},
	    {16441,	"S_POSN"},
	    {16443,	"CDFLAG"},
	    {16444,	"PRBUFF"},
	    {16477,	"MEMBOT"},
	    {0,		NULL}
	};

    return label;
}


const char *ZX81Info(Z80 *z80)
{
    static char buff[80];

    snprintf(buff, sizeof buff, "NMI=%c BEAM=%d,%d",
    			YESNO(nmi_generator), video.x, video.y);

    return buff;
}


void ZX81EnableScreen(int enable)
{
    scr_enable=enable;
}


void ZX81Reset(Z80 *z80)
{
    int f;

    scr_enable = TRUE;

    for(f = 0; f < 8; f++)
    {
    	matrix[f] = 0xff;
    }
}


void ZX81SetupJoystick(void)
{
    Uint32 white;
    Uint32 black;
    Uint32 red;
    Uint32 green;
    int f;

    if (!JOYConnected())
    {
	GUIMessage(eMessageBox, "ERROR","No joystick connected");
	return;
    }

    white = GFXRGB(255, 255, 255);
    black = GFXRGB(0, 0, 0);
    red = GFXRGB(255, 0, 0);
    green = GFXRGB(0, 255, 0);

    defining_joykeys = TRUE;

    GFXClear(black);

    GFXRect(0, 0, GFX_WIDTH, GFX_HEIGHT, red, TRUE);
    GFXRect(8, 8, GFX_WIDTH - 16, GFX_HEIGHT - 16, black, TRUE);

    GFXPrintCentre(10, green, black, "%s", "JOYSTICK MAPPING");
    GFXPrintCentre(26, white, black, "%s",
    				"Press joystick direction or button");
    GFXPrintCentre(34, white, black, "%s", "then key to map it onto.");
    GFXPrintCentre(42, white, black, "%s", "Press ESCAPE to finish.");

    GFXEndFrame(FALSE);

    while(TRUE)
    {
    	SDL_Event *e;
	JoystickButton button;
	const MatrixMap *map;

	last_joystick = JOY_NumButtons;

	GFXPrintPaper(10, GFX_HEIGHT - 20, green, black,
				"%s", "PRESS JOYSTICK");
	GFXEndFrame(FALSE);

	do
	{
	    e = GFXGetKey();

	    if (e && e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)
	    {
		defining_joykeys = FALSE;
		return;
	    }
	} while(last_joystick == JOY_NumButtons);

	GFXPrintPaper(10, 60, white, black,
				"Joystick: %-20.20s", LastJoystick());
	GFXPrintPaper(10, GFX_HEIGHT - 20, green, black,
						"%s", "PRESS KEY     ");
	GFXEndFrame(FALSE);

	button = last_joystick;

	do
	{
	    map = NULL;
	    e = GFXGetKey();

	    if (e && e->type == SDL_KEYDOWN)
	    {
		if (e->key.keysym.sym == SDLK_ESCAPE)
		{
		    defining_joykeys = FALSE;
		    return;
		}

		for(f = 0; keymap[f].key != SDLK_UNKNOWN && !map; f++)
		{
		    if (keymap[f].key == e->key.keysym.sym && keymap[f].display)
		    {
			map = keymap + f;
		    }
		}
	    }
	} while(!map);

	GFXPrintPaper(10, 70, white, black, "Key: %-20.20s", map->display);
	GFXEndFrame(FALSE);

	joystick[button].row = map->m1;
	joystick[button].bit = map->b1;
    }
}


void ZX81SaveJoystick(void)
{
    if (GUIFileSelect("JOYSTICK FILE TO SAVE", FALSE,
		      joykey_file[0] ? Dirname(joykey_file) :
					 SConfig(CONF_TAPEDIR),
		      joykey_file, joyfile_ext))
    {
	SaveJoystick(joykey_file);
    }
}


void ZX81LoadJoystick(void)
{
    if (GUIFileSelect("JOYSTICK FILE TO LOAD", TRUE,
		      joykey_file[0] ? Dirname(joykey_file) :
					 SConfig(CONF_TAPEDIR),
		      joykey_file, joyfile_ext))
    {
	LoadJoystick(joykey_file);
    }
}


void ZX81DisplayJoytick(void)
{
    char msg[1024] = {0};
    JoystickButton button;

    button = JOY_FIRST_BUTTON;

    while (button <= JOY_LAST_BUTTON)
    {
    	char s[80];

	snprintf(s, sizeof s, "%-15s: %-15s", JOYButtonName(button),
					      JoystickMapping(button));

	strcat(msg, s);

	if (++button != JOY_NumButtons)
	{
	    strcat(msg, "\n");
	}
    }

    GUIMessage(eMessageBox, "JOYSTICK MAPPINGS", "%s", msg);
}
