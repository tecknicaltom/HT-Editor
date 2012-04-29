/* 
 *	HT Editor
 *	x86dis.h
 *
 *	Copyright (C) 1999-2002 Stefan Weyergraf
 *	Copyright (C) 2005-2007 Sebastian Biallas (sb@biallas.net)
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

#ifndef __X86DIS_H__
#define __X86DIS_H__

#include "asm.h"
#include "x86opc.h"

#define X86DIS_OPCODE_CLASS_STD		0		/* no prefix */
#define X86DIS_OPCODE_CLASS_EXT		1		/* 0F */
#define X86DIS_OPCODE_CLASS_EXT_66	2		/* 66 0F */
#define X86DIS_OPCODE_CLASS_EXT_F2	3		/* F2 0F */
#define X86DIS_OPCODE_CLASS_EXT_F3	4		/* F3 0F */
#define X86DIS_OPCODE_CLASS_EXTEXT	5		/* 0F 0F */

/* x86-specific styles */
#define X86DIS_STYLE_EXPLICIT_MEMSIZE	0x00000001	/* IF SET: mov word ptr [0000], ax 	ELSE: mov [0000], ax */
#define X86DIS_STYLE_OPTIMIZE_ADDR	0x00000002	/* IF SET: mov [eax*3], ax 		ELSE: mov [eax+eax*2+00000000], ax */

struct x86dis_vex {
	uint8 mmmm;
	uint8 vvvv;
	uint8 l;
	uint8 w;
	uint8 pp;
};

struct x86dis_insn {
	bool invalid;
	sint8 opsizeprefix;
	sint8 lockprefix;
	sint8 repprefix;
	sint8 segprefix;
	uint8 rexprefix;
	x86dis_vex vexprefix;
	int size;
	int opcode;
	int opcodeclass;
	X86OpSize eopsize;
	X86AddrSize eaddrsize;
	bool ambiguous;
	const char *name;
	x86_insn_op op[5];
};

/*
 *	CLASS x86dis
 */

class x86dis: public Disassembler {
public:
	X86OpSize opsize;
	X86AddrSize addrsize;

	x86opc_insn (*x86_insns)[256];

protected:
	x86dis_insn insn;
	char insnstr[256];
	byte *codep, *ocodep;
	CPU_ADDR addr;
	byte c;
	int modrm;
	int sib;
	int drex;
	int maxlen;
	int special_imm;
	uint32 disp;
	bool have_disp;
	bool fixdisp;

	/* new */
	virtual		void	checkInfo(x86opc_insn *xinsn);
			void	decode_insn(x86opc_insn *insn);
			void	decode_vex_insn(x86opc_vex_insn *xinsn);
	virtual		void	decode_modrm(x86_insn_op *op, char size, bool allow_reg, bool allow_mem, bool mmx, bool xmm, bool ymm);
			void	decode_op(x86_insn_op *op, x86opc_insn_op *xop);
			void	decode_sib(x86_insn_op *op, int mod);
			int	esizeop(uint c);
			int	esizeop_ex(uint c);
			byte	getbyte();
			uint16	getword();
			uint32	getdword();
			uint64	getqword();
			int	getmodrm();
			int	getsib();
			int	getdrex();
			uint32	getdisp();
			int	getspecialimm();
			void	invalidate();
			bool	isfloat(char c);
			bool	isaddr(char c);
	virtual		void	prefixes();
			void	str_format(char **str, const char **format, char *p, char *n, char *op[3], int oplen[3], char stopchar, int print);
	virtual		void	str_op(char *opstr, int *opstrlen, x86dis_insn *insn, x86_insn_op *op, bool explicit_params);
			uint	mkmod(uint modrm);
			uint	mkreg(uint modrm);
			uint	mkindex(uint modrm);
			uint	mkrm(uint modrm);
	virtual		uint64	getoffset();
	virtual		void	filloffset(CPU_ADDR &addr, uint64 offset);
public:
				x86dis(X86OpSize opsize, X86AddrSize addrsize);
				x86dis(BuildCtorArg&a): Disassembler(a) {};

	/* overwritten */
	virtual	dis_insn *	decode(byte *code, int maxlen, CPU_ADDR addr);
	virtual	dis_insn *	duplicateInsn(dis_insn *disasm_insn);
	virtual	void		getOpcodeMetrics(int &min_length, int &max_length, int &min_look_ahead, int &avg_look_ahead, int &addr_align);
	virtual	const char *	getName();
	virtual	byte		getSize(dis_insn *disasm_insn);
	virtual	void		load(ObjectStream &f);
	virtual ObjectID	getObjectID() const;
	virtual const char *	str(dis_insn *disasm_insn, int options);
	virtual const char *	strf(dis_insn *disasm_insn, int options, const char *format);
	virtual void		store(ObjectStream &f) const;
	virtual bool		validInsn(dis_insn *disasm_insn);
};

class x86_64dis: public x86dis {
	static x86opc_insn (*x86_64_insns)[256];
public:	
				x86_64dis();
				x86_64dis(BuildCtorArg&a): x86dis(a) {};
	virtual	void		checkInfo(x86opc_insn *xinsn);
	virtual	void		decode_modrm(x86_insn_op *op, char size, bool allow_reg, bool allow_mem, bool mmx, bool xmm, bool ymm);
	virtual	void		prefixes();
	virtual	uint64		getoffset();
	virtual	void		filloffset(CPU_ADDR &addr, uint64 offset);
		void		load(ObjectStream &f);
	virtual ObjectID	getObjectID() const;
	
			void	prepInsns();
};

class x86dis_vxd: public x86dis {
protected:
	virtual void str_op(char *opstr, int *opstrlen, x86dis_insn *insn, x86_insn_op *op, bool explicit_params);
public:
				x86dis_vxd(BuildCtorArg&a): x86dis(a) {};
				x86dis_vxd(X86OpSize opsize, X86AddrSize addrsize);

	virtual dis_insn *	decode(byte *code, int maxlen, CPU_ADDR addr);
	virtual ObjectID	getObjectID() const;
};

#endif /* __X86DIS_H__ */
