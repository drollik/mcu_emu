/*
 * asm.h
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */

#ifndef ASM_H_
#define ASM_H_

#include <stdint.h>
// ASSEMBLER


typedef enum oper_e {
	NON = 0, // no operand
	REG, // register: R1 ... R3
	VAL, // immediate value: 0xABCD, 1234, -1234, const
	ADI, // address: (0x1000), (label)
	ADR, // address in register: (R1) ... (R3)
	ADB // address for branch instruction $0x1000, $label
} oper_t;

typedef struct op_s {
	uint8_t opcode;
	uint8_t operand_count;
	char* mnemonic;
	oper_t oper1;
	oper_t oper2;
} op_t;


// splits a given string s in 3 tokens according to one of the following patterns:
// OP				(1 token)
// OP OPER1 		(2 tokens)
// OP OPER1, OPER2	(3 tokens)
// the string s is modified in the process!
// the function returns the number of tokens parsed. the tokens are returnd in the array token,
// which must be provided by the caller.
int tokenize_line( char *s, char *token[3] );

// XXX: TBD
void find_op( char *s );

#endif /* ASM_H_ */
