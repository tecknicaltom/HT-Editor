/*
 *	HT Editor
 *	mipsopc.cc
 *
 *	Copyright (C) 2003 Sebastian Biallas (sb@biallas.net)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __MIPS_OPC_H__
#define __MIPS_OPC_H__

#include "io/types.h"

struct mips_opcode
{
	/* The opcode name.  */
	const char *name;

	uint32 mask;

	uint32 opcode;

	/* An array of operand codes.  Each code is an index into the
	   operand table.  They appear in the order which the operands must
	   appear in assembly code, and are terminated by a zero.  */
	byte operands[8];
};

/* The table itself is sorted by major opcode number, and is otherwise
   in the order in which the disassembler should consider
   instructions.  */
extern const struct mips_opcode mips_opcodes[];
extern const int mips_num_opcodes;

/* Values defined for the flags field of a struct powerpc_opcode.  */


/* A macro to extract the major opcode from an instruction.  */
//#define PPC_OP(i) (((i) >> 26) & 0x3f)

/* The operands table is an array of struct powerpc_operand.  */

struct mips_operand
{
	/* The number of bits in the operand.  */
	byte bits;

	/* How far the operand is left shifted in the instruction.  */
	byte shift;
	
	byte add;
	
	byte scale;

	/* Extraction function.  This is used by the disassembler.  To
	   extract this operand type from an instruction, check this field.

	If it is NULL, compute
	    op = ((i) >> o->shift) & ((1 << o->bits) - 1);
	 if ((o->flags & PPC_OPERAND_SIGNED) != 0
		&& (op & (1 << (o->bits - 1))) != 0)
	   op -= 1 << o->bits;
	(i is the instruction, o is a pointer to this structure, and op
	is the result; this assumes twos complement arithmetic).

	If this field is not NULL, then simply call it with the
	instruction value.  It will return the value of the operand.  If
	the INVALID argument is not NULL, *INVALID will be set to
	non-zero if this operand type can not actually be extracted from
	this operand (i.e., the instruction does not match).  If the
	operand is valid, *INVALID will not be changed.  */

	uint32 (*extract)(uint32 instruction, bool *invalid);

	/* One bit syntax flags.  */
	uint32 flags;
};

/* Elements in the table are retrieved by indexing with values from
   the operands field of the powerpc_opcodes table.  */

extern const struct mips_operand mips_operands[];

extern const char *mips_reg_names[];

#define MIPS_OPERAND_X   1
#define MIPS_OPERAND_XP  2
#define MIPS_OPERAND_XM  3
#define MIPS_OPERAND_Y   4
#define MIPS_OPERAND_YP  5
#define MIPS_OPERAND_YM  6
#define MIPS_OPERAND_Yq  7
#define MIPS_OPERAND_Z   8
#define MIPS_OPERAND_ZP  9
#define MIPS_OPERAND_ZM  10
#define MIPS_OPERAND_Zq  11
#define MIPS_OPERAND_XYZ_MASK 0xf
#define MIPS_OPERAND_GPR   16
#define MIPS_OPERAND_GPR_2 32
#define MIPS_OPERAND_IMM   64
#define MIPS_OPERAND_FAKE   128
#define MIPS_OPERAND_SIGNED 256
#define MIPS_OPERAND_ABS   512
#define MIPS_OPERAND_REL   1024

/* The next operand should be wrapped in parentheses rather than
   separated from this one by a comma.  This is used for the load and
   store instructions which want their operands to look like
	  reg,displacement(reg)
   */
#define MIPS_OPERAND_PARENS (010)
#endif
