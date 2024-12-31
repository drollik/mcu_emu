/*
 * asm.h
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */

#ifndef ASM_H_
#define ASM_H_

#include <stdint.h>
#include <stdbool.h>
// ASSEMBLER


typedef enum oper_e {

	ERROR  = 0, // error
	OPER_START = ERROR, // only for iterating through operand types
	NON, // no operand
	REG, // register: R1 ... R3
	VAL, // immediate value: 0xABCD, 1234, -1234, const
	ADS, // storage address: (0x1000), (label)
	ADR, // address in register: (R1) ... (R3)
	ADB, // immediate address for branch instruction $0x1000, $label
	OPER_END // only for iterating through operand types
} oper_t;

typedef struct op_s {
	uint8_t opcode;
	uint8_t operand_count;
	char* mnemonic;
	oper_t oper1;
	oper_t oper2;
} op_t;

// parse a buffer with assembly language; write the opcode to out
// returns the number of bytes written to out
size_t parse_all( char *amsbuf, uint8_t *out );

// splits a given string s in 3 tokens according to one of the following patterns:
// OP				(1 token)
// OP OPER1 		(2 tokens)
// OP OPER1, OPER2	(3 tokens)
// the string s is modified in the process!
// the function returns the number of tokens parsed. the tokens are returnd in the array token,
// which must be provided by the caller.
int tokenize_line( char *s, char *token[3] );


// extracts a label from a given line and stores the corresponding addr with it.
// returns a pointer to the character after the ':', e.g.:
// given "  loop:NOP" the function will store the label and return a pointer to 'N'.

char *label_extract( char *s, int32_t addr );

void label_print();

int label_numelems();

// XXX: TBD
void find_op( char *s );

bool get_register( char*str, int *reg );
bool get_immediate_value( char *str, int16_t *val );
bool get_storage_address( char *str, uint16_t *addr );
bool get_register_address( char *str, int *reg ); // address in register: (R1) ... (R3)
bool get_immediate_address( char *str, uint16_t *addr ); // $0x0000, $0xFFF, $256

oper_t get_operand_type( char *str );
int find_instruction( char *token[3] );

#endif /* ASM_H_ */
