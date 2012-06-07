/*
 *	HT Editor
 *	mipsdis.cc
 *
 *	Copyright (C) 2008 Sebastian Biallas (sb@biallas.net)
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

#include "endianess.h"
#include "mipsdis.h"
#include "mipsopc.h"
#include "snprintf.h"
#include "tools.h"

MIPSDisassembler::MIPSDisassembler()
{
}

dis_insn *MIPSDisassembler::decode(byte *code, int maxlen, CPU_ADDR addr)
{
	const struct mips_opcode *opcode;
	const struct mips_opcode *opcode_end;
	uint32 op;

	if (maxlen < 4) {
		insn.valid = false;
		insn.size = maxlen;
		return &insn;
	}

	insn.size = 4;
	insn.data = createHostInt(code, 4, little_endian);

	/*
	if ((insn.data & 0xfe0c) == 0x940c || (insn.data & 0xfc0f) == 0x9000) {
		insn.size = 4;
		if (maxlen < 4) {
			insn.valid = false;
			insn.size = maxlen;
			return &insn;
		}
		insn.data |= createHostInt(code+2, 2, little_endian) << 16;
	} else {
		insn.size = 2;
	}
	*/
	
	op = insn.data;

	opcode_end = mips_opcodes + mips_num_opcodes;
	
	for (opcode = mips_opcodes; opcode < opcode_end; opcode++) {
		const byte *opindex;
		const struct mips_operand *operand;
		bool invalid;

		if ((insn.data & opcode->mask) != opcode->opcode) {
			continue;
		}

		/* Make two passes over the operands.  First see if any of them
		   have extraction functions, and, if they do, make sure the
		   instruction is valid.  */
		invalid = false;
		for (opindex = opcode->operands; *opindex != 0; opindex++) {
			operand = mips_operands + *opindex;
			if (operand->extract) (*operand->extract)(insn.data, &invalid);
		}
		if (invalid) continue;

		/* The instruction is valid.  */
		insn.name = opcode->name;

		/* Now extract and print the operands.  */
		int opidx = 0;
		for (opindex = opcode->operands; *opindex != 0; opindex++) {
			sint32 value;

			operand = mips_operands + *opindex;

			/* Operands that are marked FAKE are simply ignored.  We
			   already made sure that the extract function considered
			   the instruction to be valid.  */
			if ((operand->flags & MIPS_OPERAND_FAKE) != 0) continue;

			insn.op[opidx].op = operand;
			insn.op[opidx].flags = operand->flags;
			/* Extract the value from the instruction.  */
			if (operand->extract) {
				value = (*operand->extract)(insn.data, NULL);
			} else {
				value = (insn.data >> operand->shift) & ((1 << operand->bits) - 1);
				value <<= operand->scale;
				value += operand->add;
				if ((operand->flags & MIPS_OPERAND_SIGNED) != 0 && (value & (1 << (operand->bits + operand->scale - 1))) != 0) {
					value |= -1 << (operand->bits + operand->scale);
				}
			}

			if (operand->flags & (MIPS_OPERAND_GPR | MIPS_OPERAND_GPR_2)) {
				insn.op[opidx++].reg = value;
			} else if (operand->flags & (MIPS_OPERAND_IMM | MIPS_OPERAND_ABS)) {
				insn.op[opidx++].imm = value;
			} else if (operand->flags & MIPS_OPERAND_REL) {
				insn.op[opidx++].imm = addr.addr32.offset + value + 4;
			} else if (operand->flags & MIPS_OPERAND_Yq) {
				insn.op[opidx++].mem.disp = value;
			} else if (operand->flags & MIPS_OPERAND_Zq) {
				insn.op[opidx++].mem.disp = value;
			}

		}
		insn.ops = opidx;

		/* We have found and printed an instruction; return.  */
		insn.valid = true;
		return &insn;
	}

	insn.valid = false;
	return &insn;
}

dis_insn *MIPSDisassembler::duplicateInsn(dis_insn *disasm_insn)
{
	mipsdis_insn *insn = ht_malloc(sizeof (mipsdis_insn));
	*insn = *(mipsdis_insn *)disasm_insn;
	return insn;
}

void MIPSDisassembler::getOpcodeMetrics(int &min_length, int &max_length, int &min_look_ahead, int &avg_look_ahead, int &addr_align)
{
	min_length = 4;
	max_length = 4;
	min_look_ahead = 4;
	avg_look_ahead = 4;
	addr_align = 4;
}

byte MIPSDisassembler::getSize(dis_insn *disasm_insn)
{
	return ((mipsdis_insn*)disasm_insn)->size;
}

const char *MIPSDisassembler::getName()
{
	return "MIPS/Disassembler";
}

const char *MIPSDisassembler::str(dis_insn *disasm_insn, int style)
{
	return strf(disasm_insn, style, "");
}

const char *MIPSDisassembler::strf(dis_insn *disasm_insn, int style, const char *format)
{
	if (style & DIS_STYLE_HIGHLIGHT) enable_highlighting();
	
	const char *cs_default = get_cs(e_cs_default);
	const char *cs_number = get_cs(e_cs_number);
	const char *cs_symbol = get_cs(e_cs_symbol);

	mipsdis_insn *mips_insn = (mipsdis_insn *) disasm_insn;
	if (!mips_insn->valid) {
		switch (mips_insn->size) {
		case 1:
			strcpy(insnstr, "db            ?");
			break;
		case 2:
			sprintf(insnstr, "dw           %s0x%04x", cs_number, mips_insn->data);
			break;
		case 3:
			strcpy(insnstr, "db            ? * 3");
			break;
		case 4:
			sprintf(insnstr, "dd           %s0x%08x", cs_number, mips_insn->data);
			break;
		default: { /* braces for empty assert */
			strcpy(insnstr, "?");
//			assert(0);
		}
		}
	} else {
		char *is = insnstr+sprintf(insnstr, "%-13s", mips_insn->name);

		bool need_comma = false;
		bool need_paren = false;
		for (int opidx = 0; opidx < mips_insn->ops; opidx++) {
			int flags = mips_insn->op[opidx].flags;
			if (need_comma) {
				is += sprintf(is, "%s, ", cs_symbol);
//				need_comma = false;
			} else {
				need_comma = true;
			}
			if (flags & MIPS_OPERAND_GPR) {
				is += sprintf(is, "%s%s", cs_default, mips_reg_names[mips_insn->op[opidx].reg]);
			} else if (flags & MIPS_OPERAND_GPR_2) {
				is += sprintf(is, "%s$f%d", cs_default, mips_insn->op[opidx].reg);
				/*
			} else if (flags & MIPS_OPERAND_GPR_2) {
				is += sprintf(is, "%sr%d%s:%sr%d", cs_default, mips_insn->op[opidx].reg, cs_symbol, cs_default, mips_insn->op[opidx].reg+1);
			} else if (flags & MIPS_OPERAND_XYZ_MASK) {
				int xyz = (flags & MIPS_OPERAND_XYZ_MASK) - 1;
				const char *r[] = {"X", "X+", "-X", "Y", "Y+", "-Y", "Y%s%c%s%d", "Z", "Z+", "-Z", "Z%s%c%s%d"};
				char buf[100];
				ht_snprintf(buf, sizeof buf, "%s%s", cs_default, r[xyz]);
				unsigned q = mips_insn->op[opidx].mem.disp;
				char c;
				if (q < 0) {
					q = -q;
					c = '-';
				} else {
					c = '+';
				}
				is += sprintf(is, buf, cs_symbol, c, cs_number, q);
				*/
			} else if (flags & (MIPS_OPERAND_ABS | MIPS_OPERAND_REL)) {
				CPU_ADDR caddr;
				caddr.addr32.offset = uint32(mips_insn->op[opidx].imm);
				int slen;
				char *s = (addr_sym_func) ? addr_sym_func(caddr, &slen, addr_sym_func_context) : 0;
				if (s) {
					is += sprintf(is, "%s", cs_default);
					memcpy(is, s, slen);
					is[slen] = 0;
					is += slen;
				} else {
					is += ht_snprintf(is, 100, "%s0x%x", cs_number, mips_insn->op[opidx].imm);
				}
			} else if ((flags & MIPS_OPERAND_IMM) != 0) {
				is += sprintf(is, "%s%d", cs_number, (int)mips_insn->op[opidx].imm);
			}
#if 0
			} else if (flags & (PPC_OPERAND_RELATIVE | )) {
				CPU_ADDR caddr;
				if (mode == PPC_MODE_32) {
					caddr.addr32.offset = (uint32)ppc_insn->op[opidx].mem.disp;
				} else {
					caddr.flat64.addr = ppc_insn->op[opidx].mem.disp;
				}
				int slen;
				char *s = (addr_sym_func) ? addr_sym_func(caddr, &slen, addr_sym_func_context) : 0;
				if (s) {
					is += sprintf(is, "%s", cs_default);
					memcpy(is, s, slen);
					is[slen] = 0;
					is += slen;
				} else {
					is += ht_snprintf(is, 100, "%s0x%qx", cs_number, ppc_insn->op[opidx].rel.mem);
				}
			} else if ((flags & PPC_OPERAND_ABSOLUTE) != 0) {
				is += ht_snprintf(is, 100, "%s0x%qx", cs_number, ppc_insn->op[opidx].abs.mem);
			} else if ((flags & PPC_OPERAND_CR) == 0 || (dialect & PPC_OPCODE_PPC) == 0) {
				is += ht_snprintf(is, 100, "%s%qd", cs_number, ppc_insn->op[opidx].imm);
			} else if (ppc_insn->op[opidx].op->bits == 3) {
				is += sprintf(is, "%scr%d", cs_default, ppc_insn->op[opidx].creg);
			} else {
				static const char *cbnames[4] = { "lt", "gt", "eq", "so" };
				int cr;
				int cc;
				cr = ppc_insn->op[opidx].creg >> 2;
				if (cr != 0) is += sprintf(is, "%s4%s*%scr%d", cs_number, cs_symbol, cs_default, cr);
				cc = ppc_insn->op[opidx].creg & 3;
				if (cc != 0) {
					if (cr != 0) is += sprintf(is, "%s+", cs_symbol);
					is += sprintf(is, "%s%s", cs_default, cbnames[cc]);
				}
			}
#endif
			if (need_paren) {
				is += sprintf(is, "%s)", cs_symbol);
				need_paren = false;
			}

			if ((flags & MIPS_OPERAND_PARENS) == 0) {
				need_comma = true;
			} else {
				is += sprintf(is, "%s(", cs_symbol);
				need_paren = true;
				need_comma = false;
			}
		}
	}
	disable_highlighting();
	return insnstr;     
}

ObjectID MIPSDisassembler::getObjectID() const
{
	return ATOM_DISASM_MIPS;
}

bool MIPSDisassembler::validInsn(dis_insn *disasm_insn)
{
	return ((mipsdis_insn*)disasm_insn)->valid;
}
