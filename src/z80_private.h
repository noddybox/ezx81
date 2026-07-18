/*

    z80 - Z80 emulation

    Copyright (C) 2006  Ian Cowburn (ianc@noddybox.demon.co.uk)

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

    Private macros for Z80

*/

#ifndef Z80_PRIVATE_H
#define Z80_PRIVATE_H

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#define MAX_PER_CALLBACK	10


/* ---------------------------------------- TYPES
*/

typedef signed short	sword;

typedef union
{
    Z80Word		w;
    Z80Byte		b[2];
} Z80Reg;

struct Z80
{
    Z80Val		cycle;

    Z80Word		PC;

    Z80Reg		AF;
    Z80Reg		BC;
    Z80Reg		DE;
    Z80Reg		HL;

    Z80Word		AF_;
    Z80Word		BC_;
    Z80Word		DE_;
    Z80Word		HL_;

    Z80Reg		IX;
    Z80Reg		IY;

    Z80Word		SP;

    Z80Byte		IFF1;
    Z80Byte		IFF2;
    Z80Byte		IM;
    Z80Byte		I;
    Z80Byte		R;
    int			halt;

    Z80Byte		shift;

    int			raise;
    Z80Byte		devbyte;
    int			nmi;

    Z80ReadMemory	disread;

    Z80ReadMemory	mread;
    Z80WriteMemory	mwrite;

    Z80ReadPort		pread;
    Z80WritePort	pwrite;

    Z80Callback		callback[eZ80_NO_CALLBACK][MAX_PER_CALLBACK];

    int			last_cb;

    Z80Val		timer[3];

    const Z80Label	*labels;
};


/* ---------------------------------------- MACROS
*/

/* NOTE: A lot of these macros assume you have a variable called 'cpu'
         which is a pointer to Z80
*/


/* Invoke a callback class
*/
#define CALLBACK(r,d)	do					\
			{					\
			int f;					\
								\
			for(f=0;f<MAX_PER_CALLBACK;f++)		\
			    if (cpu->callback[r][f])		\
				cpu->callback[r][f](cpu,d);	\
			} while(0)

/* Flag register
*/
#define C_Z80			0x01
#define N_Z80			0x02
#define P_Z80			0x04
#define V_Z80			P_Z80
#define H_Z80			0x10
#define Z_Z80			0x40
#define S_Z80			0x80

#define B3_Z80			0x08
#define B5_Z80			0x20


#define SET(v,b)		(v)|=b
#define CLR(v,b)		(v)&=~(b)

#define SETFLAG(f)		SET(cpu->AF.b[LO],f)
#define CLRFLAG(f)		CLR(cpu->AF.b[LO],f)

#define PEEK(addr)		(cpu->mread(cpu,addr))
#define PEEKW(addr)		FPEEKW(cpu,addr)

#define POKE(addr,val)		cpu->mwrite(cpu,addr,val)
#define POKEW(addr,val)		FPOKEW(cpu,addr,val)

#define FETCH_BYTE		(cpu->mread(cpu,cpu->PC++))
#define FETCH_WORD		(cpu->PC+=2,FPEEKW(cpu,cpu->PC-2))

#define IS_C			(cpu->AF.b[LO]&C_Z80)
#define IS_N			(cpu->AF.b[LO]&N_Z80)
#define IS_P			(cpu->AF.b[LO]&P_Z80)
#define IS_H			(cpu->AF.b[LO]&H_Z80)
#define IS_Z			(cpu->AF.b[LO]&Z_Z80)
#define IS_S			(cpu->AF.b[LO]&S_Z80)

#define CARRY			IS_C

#define IS_IX_IY		(cpu->shift==0xdd || cpu->shift==0xfd)
#define OFFSET(off)		off=(IS_IX_IY ? (Z80Relative)FETCH_BYTE:0)

#define TSTATE(n)		do					\
				{					\
				    cpu->cycle+=n;			\
				    cpu->timer[eZ80_Timer_1]+=n;	\
				    cpu->timer[eZ80_Timer_2]+=n;	\
				    cpu->timer[eZ80_Timer_3]+=n;	\
				} while(0)

#define ADD_R(v)		cpu->R=((cpu->R&0x80)|((cpu->R+(v))&0x7f))
#define INC_R			ADD_R(1)

#define PUSH(REG)		do					\
				{					\
				    Z80Word pushv=REG;			\
				    cpu->SP-=2;				\
				    cpu->mwrite(cpu,cpu->SP,pushv);	\
				    cpu->mwrite(cpu,cpu->SP+1,pushv>>8);\
				} while(0)

#define POP(REG)		do					\
				{					\
				    REG=PEEK(cpu->SP) |			\
				    	(Z80Word)PEEK(cpu->SP+1)<<8;	\
				    cpu->SP+=2;				\
				} while(0)

#define SETHIDDEN(res)		cpu->AF.b[LO]=(cpu->AF.b[LO]&~(B3_Z80|B5_Z80))|\
					((res)&(B3_Z80|B5_Z80))

#define CALL			do				\
				{				\
				    PUSH(cpu->PC+2);		\
				    cpu->PC=PEEKW(cpu->PC);	\
				} while(0)

#define NOCALL			cpu->PC+=2
#define JP			cpu->PC=PEEKW(cpu->PC)
#define NOJP			cpu->PC+=2
#define JR			cpu->PC+=(Z80Relative)PEEK(cpu->PC)+1
#define NOJR			cpu->PC++

#define OUT(P,V)		do				\
				{				\
				    if (cpu->pwrite)		\
				    	cpu->pwrite(cpu,P,V);	\
				} while(0)

#define IN(P)			(cpu->pread?cpu->pread(cpu,P):0)



/* ---------------------------------------- GLOBAL GENERAL OPCODES/ROUTINES
*/
void Z80_Decode(Z80 *cpu, Z80Byte opcode);
void Z80_InitialiseInternals(void);
Z80Word FPEEKW(Z80 *cpu, Z80Word address);
void FPOKEW(Z80 *cpu, Z80Word address, Z80Word val);


/* ---------------------------------------- DISASSEMBLY
*/
#ifdef ENABLE_DISASSEM
typedef void		(*DIS_OP_CALLBACK)(Z80 *z80, Z80Byte op, Z80Word *pc);

extern DIS_OP_CALLBACK	dis_CB_opcode[];
extern DIS_OP_CALLBACK	dis_DD_opcode[];
extern DIS_OP_CALLBACK	dis_DD_CB_opcode[];
extern DIS_OP_CALLBACK	dis_ED_opcode[];
extern DIS_OP_CALLBACK	dis_FD_opcode[];
extern DIS_OP_CALLBACK	dis_FD_CB_opcode[];
extern DIS_OP_CALLBACK	dis_opcode_z80[];

const char	*Z80_Dis_Printf(const char *format, ...);

Z80Byte		Z80_Dis_FetchByte(Z80 *cpu, Z80Word *pc);
Z80Word		Z80_Dis_FetchWord(Z80 *cpu, Z80Word *pc);

void		Z80_Dis_Set(const char *op, const char *arg);
const char	*Z80_Dis_GetOp(void);
const char	*Z80_Dis_GetArg(void);
#endif	/* ENABLE_DISASSEM */

#endif	/* Z80_PRIVATE_H */

/* END OF FILE */
