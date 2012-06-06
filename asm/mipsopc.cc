/*
 *	HT Editor
 *	mipsopc.cc
 *
 *	Copyright (C) 1999-2003 Sebastian Biallas (sb@biallas.net)
 *	Copyright 1994 Free Software Foundation, Inc.
 *	Written by Ian Lance Taylor, Cygnus Support
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

#include <cstdio>
#include "mipsopc.h"

const char *mips_reg_names[] = {
/*     0 */ "zero",
/*     1 */ "$at",
/*  2- 3 */ "$v0", "$v1",
/*  4- 7 */ "$a0", "$a1", "$a2", "$a3",
/*  8-15 */ "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
/* 16-23 */ "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
/* 24-25 */ "$t8", "$t9",
/* 26-27 */ "$k0", "$k1",
/*    28 */ "$gp",
/*    29 */ "$sp",
/*    30 */ "$fp",
/*    31 */ "$ra",
};

#undef UNUSED

const struct mips_operand mips_operands[] =
{
  /* The zero index is used to indicate the end of the list of
     operands.  */
     
//     bit, shr, add, scale, extract
     
#define UNUSED 0
  { 0, 0, 0, 0 },

#define Rs UNUSED + 1
  { 5, 21, 0, 0, 0, MIPS_OPERAND_GPR },

#define Rt Rs + 1
  { 5, 16, 0, 0, 0, MIPS_OPERAND_GPR },

#define Rd Rt + 1
  { 5, 11, 0, 0, 0, MIPS_OPERAND_GPR },

#define Sa Rd + 1
  { 5, 6, 0, 0, 0, MIPS_OPERAND_GPR },

#define Imm Sa + 1
  { 16, 0, 0, 0, 0, MIPS_OPERAND_IMM },

#define Label Imm + 1
  { 16, 0, 0, 0, 0, MIPS_OPERAND_REL },

#define ImmOffs Label + 1
  { 16, 0, 0, 0, 0, MIPS_OPERAND_IMM | MIPS_OPERAND_PARENS },

#define Base ImmOffs + 1
  { 5, 21, 0, 0, 0, MIPS_OPERAND_GPR },
  /*
#define Rd UNUSED + 1
  { 5, 4, 0, 0, 0, AVR_OPERAND_GPR },
  
#define Rd16 Rd + 1
  { 4, 4, 16, 0, 0, AVR_OPERAND_GPR },

#define Rd23 Rd16 + 1
  { 3, 4, 16, 0, 0, AVR_OPERAND_GPR },

#define RdW Rd23 + 1
  { 4, 4, 0, 1, 0, AVR_OPERAND_GPR_2 },

#define RdW24 RdW + 1
  { 2, 4, 24, 1, 0, AVR_OPERAND_GPR_2 },

#define Rr RdW24 + 1
  { 5, 0, 0, 0, extract_rr, AVR_OPERAND_GPR },
  
#define Rr16 Rr + 1
  { 4, 0, 16, 0, 0, AVR_OPERAND_GPR },

#define Rr23 Rr16 + 1
  { 3, 0, 16, 0, 0, AVR_OPERAND_GPR },

#define RrW Rr23 + 1
  { 4, 0, 0, 1, 0, AVR_OPERAND_GPR_2 },

#define Rrd RrW + 1
  { 5, 0, 0, 0, extract_rrd, AVR_OPERAND_FAKE },
  
#define K6 Rrd + 1
  { 6, 0, 0, 0, extract_k6, AVR_OPERAND_IMM },

#define K8 K6 + 1
  { 8, 0, 0, 0, extract_k8, AVR_OPERAND_IMM },

#define k16 K8 + 1
  { 16, 0, 0, 0, extract_k16, AVR_OPERAND_IMM },

#define b3 k16 + 1
  { 3, 0, 0, 0, 0, AVR_OPERAND_IMM },

#define A5 b3 + 1
  { 5, 3, 0, 0, 0, AVR_OPERAND_IMM },

#define A6 A5 + 1
  { 6, 0, 0, 0, extract_a6, AVR_OPERAND_IMM },

#define Rel7 A6 + 1
  { 7, 3, 0, 1, 0, AVR_OPERAND_REL | AVR_OPERAND_SIGNED },
  
#define Rel12 Rel7 + 1
  { 12, 0, 0, 1, 0, AVR_OPERAND_REL | AVR_OPERAND_SIGNED },

#define Rel22 Rel12 + 1
  { 22, 0, 0, 0, extract_rel22, AVR_OPERAND_ABS },
  
#define X Rel22 + 1
  { 0, 0, 0, 0, 0, AVR_OPERAND_X },
#define XP X + 1
  { 0, 0, 0, 0, 0, AVR_OPERAND_XP },
#define XM XP + 1
  { 0, 0, 0, 0, 0, AVR_OPERAND_XM },
  
#define Y XM + 1
  { 0, 0, 0, 0, 0, AVR_OPERAND_Y },
#define YP Y + 1
  { 0, 0, 0, 0, 0, AVR_OPERAND_YP },
#define YM YP + 1
  { 0, 0, 0, 0, 0, AVR_OPERAND_YM },
#define Yq YM + 1
  { 6, 0, 0, 0, extract_q, AVR_OPERAND_Yq },
  
#define Z Yq + 1
  { 0, 0, 0, 0, 0, AVR_OPERAND_Z },
#define ZP Z + 1
  { 0, 0, 0, 0, 0, AVR_OPERAND_ZP },
#define ZM ZP + 1
  { 0, 0, 0, 0, 0, AVR_OPERAND_ZM },
#define Zq ZM + 1
  { 6, 0, 0, 0, extract_q, AVR_OPERAND_Zq },
*/  
};

#define OPCODE(o)      (o<<26)
#define FUNCTION(f)    (f)
#define OPCODE_EXT(o)  (o<<16)

#define OP_MASK          0xfc000000
#define OP_FUNCTION_MASK 0x0000003f
#define OP_FORMAT_MASK   0x02e00000
#define OP_EXT_MASK      0x001f0000
#define OP_INSTR_MASK    0xffffffff
#define OP_RT_MASK       0x001f0000

#define OP_R_MASK OP_MASK|OP_FUNCTION_MASK

const struct mips_opcode mips_opcodes[] = {
{ "nop",    OP_INSTR_MASK, 0, {0} },

// move pseudoinstruction
{ "move",   OP_R_MASK|OP_RT_MASK, OPCODE(0)|FUNCTION(0x21), {Rd, Rs} },

{ "add",    OP_R_MASK, OPCODE(0)|FUNCTION(0x20), {Rd, Rs, Rt} },
{ "addu",   OP_R_MASK, OPCODE(0)|FUNCTION(0x21), {Rd, Rs, Rt} },
{ "and",    OP_R_MASK, OPCODE(0)|FUNCTION(0x24), {Rd, Rs, Rt} },
{ "break",  OP_R_MASK, OPCODE(0)|FUNCTION(0x0d), {0} },
{ "div",    OP_R_MASK, OPCODE(0)|FUNCTION(0x1a), {Rs, Rt} },
{ "divu",   OP_R_MASK, OPCODE(0)|FUNCTION(0x1b), {Rs, Rt} },
{ "jalr",   OP_R_MASK, OPCODE(0)|FUNCTION(0x09), {Rd, Rs} },
{ "jr",     OP_R_MASK, OPCODE(0)|FUNCTION(0x08), {Rs} },
{ "mfhi",   OP_R_MASK, OPCODE(0)|FUNCTION(0x10), {Rd} },
{ "mflo",   OP_R_MASK, OPCODE(0)|FUNCTION(0x12), {Rd} },
{ "mthi",   OP_R_MASK, OPCODE(0)|FUNCTION(0x11), {Rs} },
{ "mtlo",   OP_R_MASK, OPCODE(0)|FUNCTION(0x13), {Rs} },
{ "mult",   OP_R_MASK, OPCODE(0)|FUNCTION(0x18), {Rs, Rt} },
{ "multu",  OP_R_MASK, OPCODE(0)|FUNCTION(0x19), {Rs, Rt} },
{ "nor",    OP_R_MASK, OPCODE(0)|FUNCTION(0x27), {Rd, Rs, Rt} },
{ "or",     OP_R_MASK, OPCODE(0)|FUNCTION(0x25), {Rd, Rs, Rt} },
{ "sll",    OP_R_MASK, OPCODE(0)|FUNCTION(0x00), {Rd, Rt, Sa} },
{ "sllv",   OP_R_MASK, OPCODE(0)|FUNCTION(0x04), {Rd, Rt, Rs} },
{ "slt",    OP_R_MASK, OPCODE(0)|FUNCTION(0x2a), {Rd, Rs, Rt} },
{ "sltu",   OP_R_MASK, OPCODE(0)|FUNCTION(0x2b), {Rd, Rs, Rt} },
{ "sra",    OP_R_MASK, OPCODE(0)|FUNCTION(0x03), {Rd, Rt, Sa} },
{ "srav",   OP_R_MASK, OPCODE(0)|FUNCTION(0x07), {Rd, Rt, Rs} },
{ "srl",    OP_R_MASK, OPCODE(0)|FUNCTION(0x02), {Rd, Rt, Sa} },
{ "srlv",   OP_R_MASK, OPCODE(0)|FUNCTION(0x06), {Rd, Rt, Rs} },
{ "sub",    OP_R_MASK, OPCODE(0)|FUNCTION(0x22), {Rd, Rs, Rt} },
{ "subu",   OP_R_MASK, OPCODE(0)|FUNCTION(0x23), {Rd, Rs, Rt} },
{ "syscall",OP_R_MASK, OPCODE(0)|FUNCTION(0x0c), {0} },
{ "xor",    OP_R_MASK, OPCODE(0)|FUNCTION(0x26), {Rd, Rs, Rt} },

{ "addi",   OP_MASK,   OPCODE(0x08), {Rt, Rs, Imm} },
{ "addiu",  OP_MASK,   OPCODE(0x09), {Rt, Rs, Imm} },
{ "andi",   OP_MASK,   OPCODE(0x0c), {Rt, Rs, Imm} },
{ "beq",    OP_MASK,   OPCODE(0x04), {Rs, Rt, Label} },
{ "bgez",   OP_MASK|OP_EXT_MASK, OPCODE(0x01)|OPCODE_EXT(0x01), {Rs, Label} },
{ "bgtz",   OP_MASK|OP_EXT_MASK, OPCODE(0x07)|OPCODE_EXT(0x00), {Rs, Label} },
{ "blez",   OP_MASK|OP_EXT_MASK, OPCODE(0x06)|OPCODE_EXT(0x00), {Rs, Label} },
{ "bltz",   OP_MASK|OP_EXT_MASK, OPCODE(0x01)|OPCODE_EXT(0x00), {Rs, Label} },
{ "bne",    OP_MASK,             OPCODE(0x05),                  {Rs, Rt, Label} },
{ "lb",     OP_MASK,             OPCODE(0x20),                  {Rt, ImmOffs, Base} },
{ "lbu",    OP_MASK,             OPCODE(0x24),                  {Rt, ImmOffs, Base} },
{ "lh",     OP_MASK,             OPCODE(0x21),                  {Rt, ImmOffs, Base} },
{ "lhu",    OP_MASK,             OPCODE(0x25),                  {Rt, ImmOffs, Base} },
{ "lui",    OP_MASK,             OPCODE(0x0f),                  {Rt, Imm} },
{ "lw",     OP_MASK,             OPCODE(0x23),                  {Rt, ImmOffs, Base} },
{ "lwc1",   OP_MASK,             OPCODE(0x31),                  {Rt, ImmOffs, Base} },
{ "ori",    OP_MASK,             OPCODE(0x0d),                  {Rt, Rs, Imm} },
{ "sb",     OP_MASK,             OPCODE(0x28),                  {Rt, ImmOffs, Base} },
{ "slti",   OP_MASK,             OPCODE(0x0a),                  {Rt, Rs, Imm} },
{ "sltiu",  OP_MASK,             OPCODE(0x0b),                  {Rt, Rs, Imm} },
{ "sh",     OP_MASK,             OPCODE(0x29),                  {Rt, ImmOffs, Base} },
{ "sw",     OP_MASK,             OPCODE(0x2b),                  {Rt, ImmOffs, Base} },
{ "swc1",   OP_MASK,             OPCODE(0x39),                  {Rt, ImmOffs, Base} },
{ "xori",   OP_MASK,             OPCODE(0x0e),                  {Rt, Rs, Imm} },

	/*
{ "nop",    OP_MASK, 0x0000, {0} },

{ "muls",   OP_3MASK, 0x0200, {Rd16, Rr16} },
{ "mulsu",  OP_4MASK, 0x0300, {Rd23, Rr23} },

{ "movw",   OP_3MASK, 0x0100, {RdW, RrW} },

{ "cpi",    OP_1MASK, 0x3000, {Rd16, K8} },
{ "sbci",   OP_1MASK, 0x4000, {Rd16, K8} },
{ "subi",   OP_1MASK, 0x5000, {Rd16, K8} },
{ "ori",    OP_1MASK, 0x6000, {Rd16, K8} },
{ "andi",   OP_1MASK, 0x7000, {Rd16, K8} },
{ "ldi",    OP_1MASK, 0xe000, {Rd16, K8} },

{ "rjmp",   OP_1MASK, 0xc000, {Rel12} },
{ "rcall",  OP_1MASK, 0xd000, {Rel12} },

{ "clr",    OP_2MASK, 0x2400, {Rd, Rrd} },
{ "lsl",    OP_2MASK, 0x0c00, {Rd, Rrd} },
{ "rol",    OP_2MASK, 0x1c00, {Rd, Rrd} },
{ "tst",    OP_2MASK, 0x2000, {Rd, Rrd} },

{ "adc",    OP_2MASK, 0x1c00, {Rd, Rr} },
{ "add",    OP_2MASK, 0x0c00, {Rd, Rr} },
{ "and",    OP_2MASK, 0x2000, {Rd, Rr} },
{ "cp",     OP_2MASK, 0x1400, {Rd, Rr} },
{ "cpc",    OP_2MASK, 0x0400, {Rd, Rr} },
{ "cpse",   OP_2MASK, 0x1000, {Rd, Rr} },
{ "eor",    OP_2MASK, 0x2400, {Rd, Rr} },
{ "mov",    OP_2MASK, 0x2c00, {Rd, Rr} },
{ "mul",    OP_2MASK, 0x9c00, {Rd, Rr} },
{ "or",     OP_2MASK, 0x2800, {Rd, Rr} },
{ "sbc",    OP_2MASK, 0x0800, {Rd, Rr} },
{ "sub",    OP_2MASK, 0x1800, {Rd, Rr} },

{ "break",  OP_MASK, 0x9594, {0} },
{ "clc",    OP_MASK, 0x9488, {0} },
{ "clh",    OP_MASK, 0x94d8, {0} },
{ "cli",    OP_MASK, 0x94f8, {0} },
{ "cln",    OP_MASK, 0x94a8, {0} },
{ "cls",    OP_MASK, 0x94c8, {0} },
{ "clt",    OP_MASK, 0x94e8, {0} },
{ "clv",    OP_MASK, 0x94b8, {0} },
{ "clz",    OP_MASK, 0x9498, {0} },
{ "eicall", OP_MASK, 0x9519, {0} },
{ "eijmp",  OP_MASK, 0x9419, {0} },
{ "icall",  OP_MASK, 0x9509, {0} },
{ "ijmp",   OP_MASK, 0x9409, {0} },
{ "ret",    OP_MASK, 0x9508, {0} },
{ "reti",   OP_MASK, 0x9518, {0} },
{ "sec",    OP_MASK, 0x9408, {0} },
{ "seh",    OP_MASK, 0x9458, {0} },
{ "sei",    OP_MASK, 0x9478, {0} },
{ "sen",    OP_MASK, 0x9428, {0} },
{ "ses",    OP_MASK, 0x9448, {0} },
{ "set",    OP_MASK, 0x9468, {0} },
{ "sev",    OP_MASK, 0x9438, {0} },
{ "sez",    OP_MASK, 0x9418, {0} },
{ "sleep",  OP_MASK, 0x9588, {0} },
{ "wdr",    OP_MASK, 0x95a8, {0} },

{ "spm",    OP_MASK, 0x95e8, {ZP} },

{ "elpm",   OP_MASK,  0x95d8, {0} },
{ "elpm",   OP_6MASK, 0x9006, {Rd, Z} },
{ "elpm",   OP_6MASK, 0x9007, {Rd, ZP} },
{ "lpm",    OP_MASK,  0x95c8, {0} },
{ "lpm",    OP_6MASK, 0x9004, {Rd, Z} },
{ "lpm",    OP_6MASK, 0x9005, {Rd, ZP} },
{ "ld",     OP_6MASK, 0x900c, {Rd, X} },
{ "ld",     OP_6MASK, 0x900d, {Rd, XP} },
{ "ld",     OP_6MASK, 0x900e, {Rd, XM} },
{ "ld",     OP_6MASK, 0x8008, {Rd, Y} },
{ "ldd",    OP_9MASK, 0x8008, {Rd, Yq} },
{ "ld",     OP_6MASK, 0x9009, {Rd, YP} },
{ "ld",     OP_6MASK, 0x900a, {Rd, YM} },
{ "ld",     OP_6MASK, 0x8000, {Rd, Z} },
{ "ldd",    OP_9MASK, 0x8000, {Rd, Zq} },
{ "ld",     OP_6MASK, 0x9001, {Rd, ZP} },
{ "ld",     OP_6MASK, 0x9002, {Rd, ZM} },

{ "st",     OP_6MASK, 0x920c, {X, Rd} },
{ "st",     OP_6MASK, 0x920d, {XP, Rd} },
{ "st",     OP_6MASK, 0x920e, {XM, Rd} },
{ "st",     OP_6MASK, 0x8208, {Y, Rd} },
{ "std",    OP_9MASK, 0x8208, {Yq, Rd} },
{ "st",     OP_6MASK, 0x9209, {YP, Rd} },
{ "st",     OP_6MASK, 0x920a, {YM, Rd} },
{ "st",     OP_6MASK, 0x8200, {Z, Rd} },
{ "std",    OP_9MASK, 0x8200, {Zq, Rd} },
{ "st",     OP_6MASK, 0x9201, {ZP, Rd} },
{ "st",     OP_6MASK, 0x9202, {ZM, Rd} },


{ "adiw",   OP_3MASK, 0x9600, {RdW24, K6} },
{ "sbiw",   OP_3MASK, 0x9700, {RdW24, K6} },

{ "asr",    OP_6MASK, 0x9405, {Rd} },
{ "com",    OP_6MASK, 0x9400, {Rd} },
{ "dec",    OP_6MASK, 0x940a, {Rd} },
{ "inc",    OP_6MASK, 0x9403, {Rd} },
{ "lsr",    OP_6MASK, 0x9406, {Rd} },
{ "neg",    OP_6MASK, 0x9401, {Rd} },
{ "pop",    OP_6MASK, 0x900f, {Rd} },
{ "push",   OP_6MASK, 0x920f, {Rd} },
{ "ror",    OP_6MASK, 0x9407, {Rd} },
{ "swap",   OP_6MASK, 0x9402, {Rd} },

{ "cbi",    OP_3MASK, 0x9800, {A5, b3} },
{ "sbi",    OP_3MASK, 0x9a00, {A5, b3} },
{ "sbic",   OP_3MASK, 0x9900, {A5, b3} },
{ "sbis",   OP_3MASK, 0x9b00, {A5, b3} },


{ "call",   OP_11MASK, 0x940e, {Rel22} },
{ "jmp",    OP_11MASK, 0x940c, {Rel22} },

{ "lds",    OP_6MASK, 0x9000, {Rd, k16} },
{ "sts",    OP_6MASK, 0x9200, {k16, Rd} },

{ "in",     OP_10MASK, 0xb000, {Rd, A6} },
{ "out",    OP_10MASK, 0xb800, {A6, Rd} },

{ "ser",    OP_8MASK, 0xef0f, {Rd16} },

{ "brcc",   OP_7MASK, 0xf400, {Rel7} },
{ "brcs",   OP_7MASK, 0xf000, {Rel7} },
{ "breq",   OP_7MASK, 0xf001, {Rel7} },
{ "brge",   OP_7MASK, 0xf404, {Rel7} },
{ "brhc",   OP_7MASK, 0xf405, {Rel7} },
{ "brhs",   OP_7MASK, 0xf005, {Rel7} },
{ "brid",   OP_7MASK, 0xf407, {Rel7} },
{ "brie",   OP_7MASK, 0xf007, {Rel7} },
{ "brlo",   OP_7MASK, 0xf000, {Rel7} },
{ "brlt",   OP_7MASK, 0xf004, {Rel7} },
{ "brmi",   OP_7MASK, 0xf002, {Rel7} },
{ "brne",   OP_7MASK, 0xf401, {Rel7} },
{ "brpl",   OP_7MASK, 0xf402, {Rel7} },
{ "brsh",   OP_7MASK, 0xf400, {Rel7} },
{ "brtc",   OP_7MASK, 0xf406, {Rel7} },
{ "brts",   OP_7MASK, 0xf006, {Rel7} },
{ "brvc",   OP_7MASK, 0xf403, {Rel7} },
{ "brvs",   OP_7MASK, 0xf003, {Rel7} },

{ "bld",    OP_5MASK, 0xf800, {Rd, b3} },
{ "bst",    OP_5MASK, 0xfa00, {Rd, b3} },
{ "sbrc",   OP_5MASK, 0xfc00, {Rd, b3} },
{ "sbrs",   OP_5MASK, 0xfe00, {Rd, b3} },
*/
};

const int mips_num_opcodes =
  sizeof (mips_opcodes) / sizeof (mips_opcodes[0]);

