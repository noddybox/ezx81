/*

    z80 - Z80 Emulator

    Copyright (C) 2006  Ian Cowburn <ianc@noddybox.co.uk>

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

    $Id$

    Z80

*/
#include <stdlib.h>
#include <string.h>

#include "z80.h"
#include "z80_private.h"

/* ---------------------------------------- PRIVATE FUNCTIONS
*/
static void InitTables()
{
    static int init=FALSE;

    if (init)
    	return;

    init=TRUE;

    Z80_InitialiseInternals();
}

static int Z80_CheckInterrupt(Z80 *cpu)
{
    Z80Word vector;
    int raise = cpu->raise;

    /* Check interrupts
    */
    if (cpu->raise)
    {
    	if (cpu->nmi)
	{
	    if (cpu->halt)
	    {
		cpu->halt=FALSE;
		CALLBACK(eZ80_Halt,0);
		cpu->PC++;
	    }

	    TSTATE(2);
	    cpu->IFF1=0;
	    cpu->nmi=FALSE;
	    PUSH(cpu->PC);
	    cpu->PC=0x66;
	}
    	else if (cpu->IFF1)
	{
	    if (cpu->halt)
	    {
		cpu->halt=FALSE;
		CALLBACK(eZ80_Halt,0);
		cpu->PC++;
	    }

	    TSTATE(2);

	    switch(cpu->IM)
	    {
		default:
		case 0:
		    INC_R;
		    Z80_Decode(cpu,cpu->devbyte);
		    return TRUE;
		    break;

		case 1:
		    PUSH(cpu->PC);
		    cpu->PC=0x38;
		    break;

		case 2:
		    PUSH(cpu->PC);
                    vector=(Z80Word)cpu->I*256+cpu->devbyte;
                    cpu->PC=PEEKW(vector);
		    break;
	    }
	}

	cpu->raise=FALSE;
    }

    return raise;
}


/* ---------------------------------------- INTERFACES
*/

Z80	*Z80Init(Z80ReadMemory read_memory,
		 Z80WriteMemory write_memory,
		 Z80ReadPort read_port,
		 Z80WritePort write_port,
		 Z80ReadMemory read_disassem)
{
    Z80 *cpu;
    int f;
    int r;

    InitTables();

    if (!read_memory || !write_memory)
    	return NULL;

    cpu=malloc(sizeof *cpu);

    if (cpu)
    {
	cpu->mread=read_memory;
	cpu->mwrite=write_memory;
	cpu->pread=read_port;
	cpu->pwrite=write_port;
	cpu->disread=read_disassem;

	for(f=0;f<eZ80_NO_CALLBACK;f++)
	    for(r=0;r<MAX_PER_CALLBACK;r++)
		cpu->callback[f][r]=NULL;

	cpu->labels = NULL;

	Z80Reset(cpu);
    }

    return cpu;
}


void Z80Reset(Z80 *cpu)
{
    int f;

    cpu->cycle=0;
    cpu->PC=0;

    cpu->timer[eZ80_Timer_1] = 0;
    cpu->timer[eZ80_Timer_2] = 0;
    cpu->timer[eZ80_Timer_3] = 0;

    cpu->AF.w=0xffff;
    cpu->BC.w=0xffff;
    cpu->DE.w=0xffff;
    cpu->HL.w=0xffff;
    cpu->AF_=0xffff;
    cpu->BC_=0xffff;
    cpu->DE_=0xffff;
    cpu->HL_=0xffff;

    cpu->IX.w=0xffff;
    cpu->IY.w=0xffff;

    cpu->SP=0xffff;
    cpu->IFF1=0;
    cpu->IFF2=0;
    cpu->IM=0;
    cpu->I=0;
    cpu->R=0;
    cpu->halt=0;

    cpu->raise=FALSE;
    cpu->nmi=FALSE;
}


void Z80SetPC(Z80 *cpu,Z80Word PC)
{
    cpu->PC=PC;
}


Z80Word Z80GetPC(Z80 *cpu)
{
    return cpu->PC;
}


void Z80ResetCycles(Z80 *cpu, Z80Val cycles)
{
    cpu->cycle=cycles;
}


int Z80LodgeCallback(Z80 *cpu, Z80CallbackReason reason, Z80Callback callback)
{
    int f;

    for(f=0;f<MAX_PER_CALLBACK;f++)
    {
    	if (!cpu->callback[reason][f])
	{
	    cpu->callback[reason][f]=callback;
	    return TRUE;
	}
    }

    return FALSE;
}


void Z80RemoveCallback(Z80 *cpu, Z80CallbackReason reason, Z80Callback callback)
{
    int f;

    for(f=0;f<MAX_PER_CALLBACK;f++)
    	if (cpu->callback[reason][f]==callback)
	    cpu->callback[reason][f]=NULL;
}


void Z80Interrupt(Z80 *cpu, Z80Byte devbyte)
{
    cpu->raise=TRUE;
    cpu->devbyte=devbyte;
    cpu->nmi=FALSE;
}


void Z80NMI(Z80 *cpu)
{
    cpu->raise=TRUE;
    cpu->nmi=TRUE;
}


int Z80SingleStep(Z80 *cpu)
{
    Z80Byte opcode;

    cpu->last_cb=TRUE;
    cpu->shift=0;

    if (!Z80_CheckInterrupt(cpu))
    {
	INC_R;

	opcode=FETCH_BYTE;

	Z80_Decode(cpu,opcode);
    }

    CALLBACK(eZ80_Instruction,cpu->cycle);

    return cpu->last_cb;
}


void Z80Exec(Z80 *cpu)
{
    while (Z80SingleStep(cpu));
}


void Z80GetState(Z80 *cpu, Z80State *state)
{
    state->cycle=	cpu->cycle;

    state->AF	=	cpu->AF.w;
    state->BC	=	cpu->BC.w;
    state->DE	=	cpu->DE.w;
    state->HL	=	cpu->HL.w;

    state->AF_	=	cpu->AF_;
    state->BC_	=	cpu->BC_;
    state->DE_	=	cpu->DE_;
    state->HL_	=	cpu->HL_;

    state->IX	=	cpu->IX.w;
    state->IY	=	cpu->IY.w;

    state->SP	=	cpu->SP;
    state->PC	=	cpu->PC;

    state->IFF1	=	cpu->IFF1;
    state->IFF2	=	cpu->IFF2;
    state->IM	=	cpu->IM;
    state->I	=	cpu->I;
    state->R	=	cpu->R;
}


void Z80SetState(Z80 *cpu, const Z80State *state)
{
    cpu->cycle	=	state->cycle;

    cpu->AF.w	=	state->AF;
    cpu->BC.w	=	state->BC;
    cpu->DE.w	=	state->DE;
    cpu->HL.w	=	state->HL;

    cpu->AF_	=	state->AF_;
    cpu->BC_	=	state->BC_;
    cpu->DE_	=	state->DE_;
    cpu->HL_	=	state->HL_;

    cpu->IX.w	=	state->IX;
    cpu->IY.w	=	state->IY;

    cpu->SP	=	state->SP;
    cpu->PC	=	state->PC;

    cpu->IFF1	=	state->IFF1;
    cpu->IFF2	=	state->IFF2;
    cpu->IM	=	state->IM;
    cpu->I	=	state->I;
    cpu->R	=	state->R;
}


Z80Val Z80Cycles(Z80 *cpu)
{
    return cpu->cycle;
}


void Z80SetLabels(Z80 *cpu, const Z80Label labels[])
{
    cpu->labels=labels;
}


const char *Z80Disassemble(Z80 *cpu, Z80Word *pc)
{
#ifdef ENABLE_DISASSEM
    static char s[80];
    Z80Word opc,npc;
    Z80Byte op;
    int f;

    opc=*pc;
    op=Z80_Dis_FetchByte(cpu,pc);
    dis_opcode_z80[op](cpu,op,pc);
    npc=*pc;

    strcpy(s,Z80_Dis_Printf("%-5s",Z80_Dis_GetOp()));
    strcat(s,Z80_Dis_Printf("%-40s ;",Z80_Dis_GetArg()));

    for(f=0;f<5 && opc!=npc;f++)
	strcat(s,Z80_Dis_Printf(" %.2x",(int)cpu->disread(cpu,opc++)));

    if (opc!=npc)
	for(f=1;f<3;f++)
	    s[strlen(s)-f]='.';

    return s;
#else
    (*pc)+=4;
    return "NO DISASSEMBLER";
#endif
}


Z80Val Z80GetTimer(Z80 *cpu, Z80Timer timer)
{
    return cpu->timer[timer];
}


void Z80SetTimer(Z80 *cpu, Z80Timer timer, Z80Val value)
{
    cpu->timer[timer] = value;
}


/* END OF FILE */
