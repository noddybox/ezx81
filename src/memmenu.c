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

    Provides the memory menu

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "memmenu.h"
#include "zx81.h"
#include "gfx.h"
#include "gui.h"
#include "expr.h"
#include "util.h"

#include <SDL.h>

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#define WHITE		GFXRGB(255,255,255)
#define BLACK		GFXRGB(0,0,0)
#define RED		GFXRGB(255,100,100)
#define GREEN		GFXRGB(100,255,100)
#define BLUE		GFXRGB(100,100,255)

#define TRACE		"trace"
#define TRACEMEM_WIN	6

#define HI(w)		(((w)&0xff00)>>8)
#define LO(w)		((w)&0xff)



/* ---------------------------------------- TYPES
*/
typedef struct
{
    int		no;
    int		*active;
    char	**expr;
} Breakpoint;


typedef struct
{
    Z80Word	addr;
    Z80Word	val;
} MemTrace;


typedef struct
{
    Z80State	s;
    Z80Val	c;
    MemTrace	hl[TRACEMEM_WIN];
    MemTrace	de[TRACEMEM_WIN];
    MemTrace	sp[TRACEMEM_WIN];
} Trace;


/* ---------------------------------------- STATIC DATA
*/
static FILE		*trace=NULL;
static Breakpoint	bpoint={0,NULL,NULL};
static const char	*raised=NULL;
static int		lodged=FALSE;


/* ---------------------------------------- PROTOS
*/
static int		Instruction(Z80 *z80, Z80Val data);
static int		Address(Z80 *z80, const char *p, Z80Word *addr);
static int		Expand(void *client, const char *p, long *res);


/* ---------------------------------------- PRIVATE FUNCTIONS
*/
static int StrEq(const char *a, const char *b)
{
    while(*a && *b && tolower(*a)==tolower(*b))
    {
        a++;
        b++;
    }

    if (*a || *b)
        return FALSE;
    else
        return TRUE;
}


static int Address(Z80 *z80, const char *p, Z80Word *addr)
{
    long e=0;

    if (ExprEval(p,&e,Expand,z80))
    {
    	*addr=e;
	return TRUE;
    }
    else
    {
    	return FALSE;
    }
}

static int Expand(void *client, const char *p, long *res)
{
    Z80State state;
    Z80 *cpu;
    int ok=TRUE;

    cpu=client;

    Z80GetState(cpu, &state);

    if (StrEq(p,"AF"))
    	*res=state.AF;
    else if (StrEq(p,"BC"))
    	*res=state.BC;
    else if (StrEq(p,"DE"))
    	*res=state.DE;
    else if (StrEq(p,"HL"))
    	*res=state.HL;
    else if (StrEq(p,"IX"))
    	*res=state.IX;
    else if (StrEq(p,"IY"))
    	*res=state.IY;
    else if (StrEq(p,"SP"))
    	*res=state.SP;
    else if (StrEq(p,"PC"))
    	*res=state.PC;
    else if (StrEq(p,"A"))
    	*res=HI(state.AF);
    else if (StrEq(p,"F"))
    	*res=LO(state.AF);
    else if (StrEq(p,"B"))
    	*res=HI(state.BC);
    else if (StrEq(p,"C"))
    	*res=LO(state.BC);
    else if (StrEq(p,"D"))
    	*res=HI(state.DE);
    else if (StrEq(p,"E"))
    	*res=LO(state.DE);
    else if (StrEq(p,"H"))
    	*res=HI(state.HL);
    else if (StrEq(p,"L"))
    	*res=LO(state.HL);
    else if (StrEq(p,"AF_"))
    	*res=state.AF_;
    else if (StrEq(p,"BC_"))
    	*res=state.BC_;
    else if (StrEq(p,"DE_"))
    	*res=state.DE_;
    else if (StrEq(p,"HL_"))
    	*res=state.HL_;
    else if (StrEq(p,"A_"))
    	*res=HI(state.AF_);
    else if (StrEq(p,"F_"))
    	*res=LO(state.AF_);
    else if (StrEq(p,"B_"))
    	*res=HI(state.BC_);
    else if (StrEq(p,"C_"))
    	*res=LO(state.BC_);
    else if (StrEq(p,"D_"))
    	*res=HI(state.DE_);
    else if (StrEq(p,"E_"))
    	*res=LO(state.DE_);
    else if (StrEq(p,"H_"))
    	*res=HI(state.HL_);
    else if (StrEq(p,"L_"))
	*res=LO(state.HL_);
    else if (StrEq(p,"IM"))
	*res=state.IM;
    else if (StrEq(p,"R"))
	*res=state.R;
    else if (StrEq(p,"IFF1"))
	*res=state.IFF1;
    else if (StrEq(p,"IFF2"))
	*res=state.IFF2;
    else if (p[0]=='@')
    {
	Z80Word n;

	if (Address(client,p+1,&n))
	{
	    *res=ZX81ReadForDisassem(client,n);
	}
	else
	{
	    ok=FALSE;
	}
    }

    return ok;
}


static int BreaksActive(void)
{
    int f;
    int ret=FALSE;

    for(f=0;f<bpoint.no;f++)
    	ret|=bpoint.active[f];

    return ret;
}


static void SetCallback(Z80 *z80)
{
    if ((trace || BreaksActive()) && !lodged)
    {
	Z80LodgeCallback(z80,eZ80_Instruction,Instruction);
	lodged=TRUE;
    }
}


static void ClearCallback(Z80 *z80)
{
    if (!trace && !BreaksActive() && lodged)
    {
	Z80RemoveCallback(z80,eZ80_Instruction,Instruction);
	lodged=FALSE;
    }
}


static void Centre(const char *p, int y, Uint32 col)
{
    GFXPrint((GFX_WIDTH-strlen(p)*8)/2,y,col,"%s",p);
}


static void CentrePaper(const char *p, int y, Uint32 pen, Uint32 paper)
{
    GFXPrintPaper((GFX_WIDTH-strlen(p)*8)/2,y,pen,paper,"%s",p);
}


static void DisplayMenu(void)
{
    static const char *menu[]=
    {
    	"1   - Disassemble/Hex dump  ",
    	"2   - Disassemble to file   ",
	"3   - Start/Stop trace log  ",
	"4   - Playback trace log    ",
	"5   - Add a new breakpoint  ",
	"6   - Set active breakpoints",
	"7   - Remove a breakpoint   ",
	"8   - Clear all breakpoints ",
	"9   - Monitor               ",
	"R   - Reset                 ",
	"S   - Show current CPU state",
	"X   - Exit (without confirm)",
    	"ESC - Return                ",
	NULL
    };

    int f;

    GFXClear(BLACK);

    Centre("MEMORY MENU",0,WHITE);
    Centre("Select an option",10,RED);

    f=0;

    while(menu[f])
    {
    	Centre(menu[f],25+f*10,WHITE);
	f++;
    }
}


static const char *FlagString(Z80Byte flag)
{
    static char s[]="76543210";
    static char c[]="SZ5H3PNC";
    int f;

    for(f=0;f<8;f++)
        if (flag&(1<<(7-f)))
            s[f]=c[f];
        else
            s[f]='-';

    return s;
}


int DisplayZ80State(Z80State s, Z80Val cycle, int y, Uint32 col)
{
    GFXPrintPaper(0,y,col,BLACK,
		  "PC=%4.4x  A=%2.2x     F=%s",
		  s.PC,s.AF>>8,FlagString(s.AF&0xff));
    y+=8;
    GFXPrintPaper(0,y,col,BLACK,
    		  "BC=%4.4x  DE=%4.4x  HL=%4.4x",
		  s.BC,s.DE,s.HL);
    y+=8;
    GFXPrintPaper(0,y,col,BLACK,
    		  "IX=%4.4x  IY=%4.4x  SP=%4.4x",
		  s.IX,s.IY,s.SP);
    y+=8;
    GFXPrintPaper(0,y,col,BLACK,
    		  "AF'=%4.4x BC'=%4.4x DE'=%4.4x HL'=%4.4x",
		  s.AF_,s.BC_,s.DE_,s.HL_);
    y+=8;
    GFXPrintPaper(0,y,col,BLACK,
    		  "I=%2.2x     IM=%2.2x    R=%2.2x",
		  s.I,s.IM,s.R);
    y+=8;
    GFXPrintPaper(0,y,col,BLACK,
    		  "IFF1=%2.2x  IFF2=%2.2x  CY=%8.8lx",
		  s.IFF1,s.IFF2,cycle);

    return y+8;
}

void DisplayState(Z80 *z80)
{
    Z80State state;
    int y;

    Z80GetState(z80, &state);
    y=DisplayZ80State(state,Z80Cycles(z80),GFX_HEIGHT/2,RED);
    GFXPrintPaper(0,y,GREEN,BLACK,"%s",ZX81Info(z80));
}

static int EnterAddress(const char *prompt, Z80 *z80, Z80Word *w)
{
    const char *p;
    long l;

    p=GUIInputString(prompt ? prompt : "Address?","");

    if (*p)
    {
	if (ExprEval(p,&l,Expand,z80))
	{
	    *w=(Z80Word)l;
	    return TRUE;
	}
	else
	{
	    GUIMessage(eMessageBox,"ERROR","%s",ExprError());
	}
    }

    return FALSE;
}


static void EnterLong(const char *prompt, long *l)
{
    const char *p;

    p=GUIInputString(prompt ? prompt : "Value?","");

    if (*p)
    	*l=strtol(p,NULL,0);
}


static void DoDisassem(Z80 *z80)
{
    static int hexmode=FALSE;
    Z80Word pc=Z80GetPC(z80);
    int quit=FALSE;

    while(!quit)
    {
	SDL_Event *e;
	Z80Word curr;
	Z80Word next;
	int f;

	GFXClear(BLACK);

    	Centre("DISASSEMBLY",0,WHITE);
    	Centre("Press F1 for help",9,RED);

	curr=pc;

	for(f=0;f<GFX_HEIGHT/8-8;f++)
	{
	    char s[80];
	    char *p;
	    int y;

	    y=24+f*8;

	    GFXPrint(0,y,GREEN,"%4.4x",curr);

	    if (hexmode)
	    {
		int n;
		char asc[9];

	    	for(n=0;n<8;n++)
		{
		    Z80Byte b;

		    b=ZX81ReadForDisassem(z80,curr);

		    GFXPrint(40+n*24,y,WHITE,"%2.2x",(int)b);

		    curr++;

		    if (isprint(b))
		    	asc[n]=b;
		    else
		    	asc[n]='.';
		}

		asc[8]=0;

		GFXPrint(40+8*24,y,RED,"%s",asc);
	    }
	    else
	    {
		strcpy(s,Z80Disassemble(z80,&curr));

		p=strtok(s,";");
		GFXPrint(40,y,WHITE,"%s",p);
	    }

	    if (f==0)
		next=curr;
	}

	GFXEndFrame(FALSE);

	e=GFXWaitKey();

	switch(e->key.keysym.sym)
	{
	    case SDLK_F1:
		GUIMessage
		    (eMessageBox, "DISASSEMBLY HELP","%s",
		       "ESC         - Exit              \n"
		       "H           - Disassembly/hex   \n"
		       "Enter       - Enter address     \n"
		       "Up          - Prev byte         \n"
		       "Down        - Next op           \n"
		       "Page Up     - Back 1 page       \n"
		       "Page Down   - Forward 1 page    \n"
		       "Left        - Forward 1024 bytes\n"
		       "Right       - Back 1024 bytes   ");
		break;

	    case SDLK_ESCAPE:
	    	quit=TRUE;
		break;

	    case SDLK_h:
		hexmode=!hexmode;
	    	break;

	    case SDLK_RETURN:
	    case SDLK_KP_ENTER:
		EnterAddress(NULL,z80,&pc);
	    	break;

	    case SDLK_UP:
		if (hexmode)
		    pc-=8;
		else
		    pc--;
	    	break;

	    case SDLK_DOWN:
		pc=next;
	    	break;

	    case SDLK_PAGEUP:
		if (hexmode)
		    pc-=21*8;
		else
		    pc-=21;
		break;

	    case SDLK_PAGEDOWN:
	    	pc=curr;
		break;

	    case SDLK_LEFT:
	    	pc-=1024;
		break;

	    case SDLK_RIGHT:
	    	pc+=1024;
		break;

	    default:
	    	break;
	}
    }
}


static void DoDisassemFile(Z80 *z80)
{
    static char fname[FILENAME_MAX]="";
    FILE *fp;
    Z80Word start;
    Z80Word len;
    Z80Word prev;
    const char *p;

    EnterAddress("Disassemble from?",z80,&start);
    EnterAddress("For how many bytes?",z80,&len);

    p=GUIInputString("To file?",fname);

    if (!strlen(p))
    	return;

    strcpy(fname,p);

    if (!(fp=fopen(fname,"w")))
    {
    	GUIMessage(eMessageBox,"ERROR","Couldn't create file:\n%s",fname);
	return;
    }

    prev=len;

    while(len<=prev)
    {
    	Z80Word orig=start;

	fprintf(fp,"%4.4x: %s\n",orig,Z80Disassemble(z80,&start));
	prev=len;
	len-=(start-orig);
    }

    fclose(fp);
}


static void GetMemTrace(Z80 *z80, MemTrace *t, Z80Word from, int count)
{
    int f;

    f=0;

    while(f<count)
    {
	Z80Word w;

	w=(Z80Word)ZX81ReadForDisassem(z80,from)|
	  (Z80Word)ZX81ReadForDisassem(z80,from+1)<<8;

	t[f].addr=from;
	t[f].val=w;

    	from+=2;
	f++;
    }
}


static int Instruction(Z80 *z80, Z80Val data)
{
    int f;

    if (trace)
    {
	Trace t;

	Z80GetState(z80, &t.s);
	t.c=Z80Cycles(z80);

	GetMemTrace(z80,t.hl,t.s.HL-(TRACEMEM_WIN/2+1),TRACEMEM_WIN);
	GetMemTrace(z80,t.de,t.s.DE-(TRACEMEM_WIN/2+1),TRACEMEM_WIN);
	GetMemTrace(z80,t.sp,t.s.SP-(TRACEMEM_WIN/2+1),TRACEMEM_WIN);

	fwrite(&t,sizeof t,1,trace);
    }

    for(f=0;f<bpoint.no;f++)
	if (bpoint.active[f])
	{
	    long l;

	    if (strstr(bpoint.expr[f], "=="))
	    {
		if (ExprEval(bpoint.expr[f],&l,Expand,z80))
		{
		    if (l)
			raised=bpoint.expr[f];
		}
	    }
	    else
	    {
		if (ExprEval(bpoint.expr[f],&l,Expand,z80))
		{
		    if (l == z80->PC)
			raised=bpoint.expr[f];
		}
	    }
	}

    return TRUE;
}


static void EnableTrace(Z80 *z80)
{
    if (!trace)
    {
    	trace=fopen(TRACE,"wb");

	if (trace)
	{
	    GUIMessage(eMessageBox,"NOTICE","Created trace log");
	    SetCallback(z80);
	}
	else
	{
	    GUIMessage(eMessageBox,"ERROR","Failed to create trace log");
	}
    }
}


static void DisableTrace(Z80 *z80)
{
    if (trace)
    {
	GUIMessage(eMessageBox,"NOTICE","Closing current trace log");
    	fclose(trace);
	trace=NULL;
	ClearCallback(z80);
    }
}


static void DisplayTraceMem(int x, int y, const char *name,
			    MemTrace *m, Z80Word expect)
{
    int f;

    GFXPrint(x,y,GREEN,"%s",name);

    for(f=0;f<TRACEMEM_WIN;f++)
    {
	Uint32 paper;

	if (m[f].addr==expect)
	    paper=RED;
	else
	    paper=BLACK;

    	GFXPrintPaper(x,y+f*8+12,GREEN,paper,"%4.4x",m[f].addr);
    	GFXPrintPaper(x+32,y+f*8+12,WHITE,paper," %4.4x",m[f].val);
    }
}

static void PlaybackTrace(Z80 *z80)
{
    static int showmem=FALSE;
    FILE *fp;
    int quit;
    long prev_pos;
    long pos;
    long max_pos;
    Z80Word pc;
    Trace t;

    fp=fopen(TRACE,"rb");

    if (!fp)
    {
	GUIMessage(eMessageBox,"ERROR","Couldn't open trace file");
	return;
    }

    fseek(fp,0,SEEK_END);

    max_pos=ftell(fp)/sizeof t;

    if (max_pos==0)
    {
	GUIMessage(eMessageBox,"ERROR","Empty trace file");
	fclose(fp);
	return;
    }

    pos=0;
    prev_pos=pos;
    pc=0;
    quit=FALSE;

    while(!quit)
    {
	SDL_Event *e;
	size_t rd;
	int f;

	GFXClear(BLACK);

    	Centre("TRACE PLAYBACK",0,WHITE);
    	Centre("Press F1 for help",9,RED);

	fseek(fp,pos*sizeof t,SEEK_SET);
	fread(&t,sizeof t,1,fp);

	if (showmem)
	{
	    DisplayTraceMem(0,136,"MEM (SP)",t.sp,t.s.SP);
	    DisplayTraceMem(100,136,"MEM (HL)",t.hl,t.s.HL);
	    DisplayTraceMem(200,136,"MEM (DE)",t.de,t.s.DE);
	}
	else
	    DisplayZ80State(t.s,t.c,136,WHITE);

	for(f=0;f<10;f++)
	{
	    char str[80];
	    char *p;
	    int paper;
	    int y;

	    y=24+f*8;

	    if (f==0)
		paper=RED;
	    else
		paper=BLACK;

	    GFXPrintPaper(0,y,GREEN,paper,"%4.4x ",t.s.PC);

	    strcpy(str,Z80Disassemble(z80,&t.s.PC));
	    p=strtok(str,";");
	    GFXPrintPaper(40,y,WHITE,paper,"%s",str);
	}

	GFXPrint(0,112,GREEN,"Current step: %ld",pos+1);
	GFXPrint(0,120,GREEN,"Total steps : %ld",max_pos);

	prev_pos=pos;

	GFXEndFrame(FALSE);

	e=GFXWaitKey();

	switch(e->key.keysym.sym)
	{
	    case SDLK_F1:
		GUIMessage
		    (eMessageBox,"PLAYBACK HELP","%s",
		       "ESC       - Exit                     \n"
		       "Enter     - Step number              \n"
		       "P         - Search for PC            \n"
		       "M         - Toggle CPU state/memory  \n"
		       "Up        - Prev step                \n"
		       "Down      - Next step                \n"
		       "Page Up   - Back 50 steps            \n"
		       "Page Down - Forward 50 steps         \n"
		       "Left      - Back 1000 steps          \n"
		       "Right     - Forward 1000 steps       \n \n"
		       "NOTE: Disassembly is as memory is NOW");
		break;

	    case SDLK_ESCAPE:
	    	quit=TRUE;
		break;

	    case SDLK_RETURN:
	    case SDLK_KP_ENTER:
		EnterLong("Step number?",&pos);
		pos--;
		break;

	    case SDLK_p:
		if (EnterAddress("PC to search for?",z80,&pc))
		{
		    while((rd=fread(&t,sizeof t,1,fp))==1)
		    {
			pos++;
			if (t.s.PC==pc)
			    break;
		    }

		    if (rd!=1)
		    {
		    	GUIMessage(eMessageBox,"NOTICE","Address not found");
			pos=prev_pos;
		    }
		}
	    	break;

	    case SDLK_m:
		showmem=!showmem;
	    	break;

	    case SDLK_UP:
		pos--;
	    	break;

	    case SDLK_DOWN:
		pos++;
	    	break;

	    case SDLK_PAGEUP:
		pos-=50;
		break;

	    case SDLK_PAGEDOWN:
	    	pos+=50;
		break;

	    case SDLK_LEFT:
	    	pos-=1000;
		break;

	    case SDLK_RIGHT:
	    	pos+=1000;
		break;

	    default:
	    	break;
	}

	/* Check position before next loop
	*/
	if (pos<0)
	    pos=0;

	if (pos>=max_pos)
	    pos=max_pos-1;
    }

    fclose(fp);
}


static void DoAddBreakpoint(Z80 *z80)
{
    const char *expr;
    long l;

    expr=GUIInputString("Expression?","");

    if (!*expr)
    	return;

    if (ExprEval(expr,&l,Expand,z80))
    {
	bpoint.no++;
	bpoint.expr=Realloc(bpoint.expr,bpoint.no * sizeof(*bpoint.expr));
	bpoint.active=Realloc(bpoint.active,bpoint.no * sizeof(*bpoint.active));

	bpoint.expr[bpoint.no-1]=StrCopy(expr);
	bpoint.active[bpoint.no-1]=TRUE;

	SetCallback(z80);
    }
    else
    {
	GUIMessage(eMessageBox,"INVALID EXPRESSION","%s",ExprError());
    }
}


static void DoActiveBreakpoint(Z80 *z80)
{
    if (bpoint.no==0)
    {
	GUIMessage(eMessageBox,"NOTICE","No breakpoints to set");
	return;
    }

    GFXClear(BLACK);

    GUIListOption("SELECT ACTIVE BREAKPOINTS",
		  bpoint.no,bpoint.expr,bpoint.active);
}


static void DoRemoveBreakpoint(Z80 *z80)
{
    int sel;

    if (bpoint.no==0)
    {
	GUIMessage(eMessageBox,"NOTICE","No breakpoints to delete");
	return;
    }

    GFXClear(BLACK);

    sel=GUIListSelect("BREAKPOINT TO DELETE",bpoint.no,bpoint.expr);

    while (sel!=-1)
    {
	int f;

	free(bpoint.expr[sel]);

	for(f=sel;f<bpoint.no-1;f++)
	{
	    bpoint.expr[f]=bpoint.expr[f+1];
	    bpoint.active[f]=bpoint.active[f+1];
	}

	bpoint.no--;

	if (bpoint.no==0)
	{
	    free(bpoint.expr);
	    bpoint.expr=NULL;
	    bpoint.active=NULL;
	}
	else
	{
	    bpoint.expr=Realloc(bpoint.expr,
				bpoint.no * sizeof(*bpoint.expr));
	    bpoint.active=Realloc(bpoint.active,
				  bpoint.no * sizeof(*bpoint.active));
	}

	ClearCallback(z80);

	sel=GUIListSelect("BREAKPOINT TO DELETE",bpoint.no,bpoint.expr);
    }
}


static void DoClearBreakpoint(Z80 *z80)
{
    if (GUIMessage(eYesNoBox,"BREAKPOINTS","Clear all breakpoints"))
    {
	int f;

	for(f=0;f<bpoint.no;f++)
	    free(bpoint.expr[f]);

	free(bpoint.expr);
	bpoint.expr=NULL;
	bpoint.no=0;

	ClearCallback(z80);
    }
}


static void DoMonitor(Z80 *z80)
{
    static int showmem=FALSE;
    int quit=FALSE;
    int running=FALSE;
    int step=FALSE;

    ZX81EnableScreen(FALSE);

    while(!quit)
    {
	Z80State state;
	Z80Word pc;
	MemTrace mt[TRACEMEM_WIN];
	const char *brkpoint;
	SDL_Event *e;
	int f;

	step=FALSE;

	GFXClear(BLACK);

    	CentrePaper("MONITOR",0,WHITE,BLACK);
    	CentrePaper("Press F1 for help",9,RED,BLACK);

	Z80GetState(z80, &state);

	if (showmem)
	{
	    GetMemTrace(z80,mt,state.SP-(TRACEMEM_WIN/2+1),TRACEMEM_WIN);
	    DisplayTraceMem(0,136,"MEM (SP)",mt,state.SP);
	    GetMemTrace(z80,mt,state.HL-(TRACEMEM_WIN/2+1),TRACEMEM_WIN);
	    DisplayTraceMem(100,136,"MEM (HL)",mt,state.HL);
	    GetMemTrace(z80,mt,state.DE-(TRACEMEM_WIN/2+1),TRACEMEM_WIN);
	    DisplayTraceMem(200,136,"MEM (DE)",mt,state.DE);
	}
	else
	{
	    Z80State state;
	    int y;

	    Z80GetState(z80, &state);

	    y=DisplayZ80State(state,Z80Cycles(z80),136,WHITE);
	    GFXPrint(0,y,GREEN,"%s",ZX81Info(z80));
	}

	pc=Z80GetPC(z80);

	for(f=0;f<10;f++)
	{
	    char str[80];
	    char *p;
	    int paper;
	    int y;

	    y=24+f*8;

	    if (f==0)
		paper=RED;
	    else
		paper=BLACK;

	    GFXPrintPaper(0,y,GREEN,paper,"%4.4x ",pc);

	    strcpy(str,Z80Disassemble(z80,&pc));
	    p=strtok(str,";");
	    GFXPrintPaper(40,y,WHITE,paper,"%s",str);
	}

	GFXEndFrame(FALSE);

	if (running)
	{
	    e=GFXGetKey();

	    if (e && e->key.state!=SDL_PRESSED)
	    	e=NULL;
	}
	else
	    e=GFXWaitKey();

	if (e)
	{
	    switch(e->key.keysym.sym)
	    {
		case SDLK_F1:
		    GUIMessage
			(eMessageBox,"PLAYBACK HELP","%s",
			   "ESC   - Exit                     \n"
			   "ENTER - Single step processor    \n"
			   "R     - Run till break or stop   \n"
			   "SPACE - Stop running             \n"
			   "M     - Toggle CPU state/memory  \n"
			   "5     - Add a new breakpoint     \n"
			   "6     - Set active breakpoints   \n"
			   "7     - Remove a breakpoint      \n"
			   "8     - Clear all breakpoints    ");
		    break;

		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		    step=TRUE;
		    break;

		case SDLK_ESCAPE:
		    quit=TRUE;
		    break;

		case SDLK_r:
		    running=TRUE;
		    break;

		case SDLK_SPACE:
		    running=FALSE;
		    break;

		case SDLK_m:
		    showmem=!showmem;
		    break;

		case SDLK_5:
		    DoAddBreakpoint(z80);
		    break;

		case SDLK_6:
		    DoActiveBreakpoint(z80);
		    break;

		case SDLK_7:
		    DoRemoveBreakpoint(z80);
		    break;

		case SDLK_8:
		    DoClearBreakpoint(z80);
		    break;

		default:
		    break;
	    }
	}

	if (running || step)
	    Z80SingleStep(z80);

	if (running && (brkpoint=Break()))
	{
	    GUIMessage(eMessageBox,"BREAKPOINT","%s",brkpoint);
	    running=FALSE;
	}
    }

    ZX81EnableScreen(TRUE);
}


/* ---------------------------------------- EXPORTED INTERFACES
*/
int MemoryMenu(Z80 *z80)
{
    Z80State state;
    SDL_Event *e;
    int done=FALSE;
    int quit=FALSE;
    int y;

    while(!done)
    {
	DisplayMenu();
	GFXEndFrame(FALSE);

	e=GFXWaitKey();

	switch(e->key.keysym.sym)
	{
	    case SDLK_1:
	    	DoDisassem(z80);
		break;

	    case SDLK_2:
	    	DoDisassemFile(z80);
	    	break;

	    case SDLK_3:
		if (!trace)
		    EnableTrace(z80);
		else
		    DisableTrace(z80);
	    	break;

	    case SDLK_4:
		DisableTrace(z80);
	    	PlaybackTrace(z80);
		break;

	    case SDLK_5:
		DoAddBreakpoint(z80);
	    	break;

	    case SDLK_6:
		DoActiveBreakpoint(z80);
	    	break;

	    case SDLK_7:
		DoRemoveBreakpoint(z80);
	    	break;

	    case SDLK_8:
		DoClearBreakpoint(z80);
	    	break;

	    case SDLK_9:
		DoMonitor(z80);
	    	break;

	    case SDLK_r:
	    	if (GUIMessage(eYesNoBox,"RESET","Sure?"))
		{
		    Z80Reset(z80);
		    ZX81Reset(z80);
		}
		break;

	    case SDLK_x:
	    	done=TRUE;
	    	quit=TRUE;
		break;

	    case SDLK_s:
		Z80GetState(z80, &state);
		GFXClear(BLACK);
		Centre("CURRENT STATE",0,WHITE);
		Centre("Press a key",9,RED);
		y=DisplayZ80State(state,Z80Cycles(z80),20,WHITE);
		GFXPrint(0,y,GREEN,"%s",ZX81Info(z80));
		GFXEndFrame(FALSE);
		GFXWaitKey();
		break;

	    case SDLK_ESCAPE:
	    	done=TRUE;
		break;

	    default:
	    	break;
	}
    }

    GFXClear(BLACK);
    return quit;
}


const char *Break(void)
{
    const char *ret;

    ret=raised;
    raised=NULL;

    return ret;
}


/* END OF FILE */
