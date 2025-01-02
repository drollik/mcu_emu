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

	OPER_ERROR  = 0, // error
	OPER_START = OPER_ERROR, // only for iterating through operand types
	NON, // no operand
	REG, // register: R1 ... R3
	VAL, // immediate value: 0xABCD, 1234, -1234, const
	ADS, // storage address: (0x1000), (label)
	ADR, // address in register: (R1) ... (R3)
	ADB, // immediate address for branch instruction $0x1000, $label
	OPER_END // only for iterating through operand types
} oper_t;

// where the operands (tokens) are placed in memory, that is,
// in which byte of the 4 byte instruction
typedef enum tok2mem_e {
	xx = 0, // this token is not put in the machine instruction
	b1 = 1,  // this token is put in byte 1 of the the machine instruction
	b_3 = 2,  // this token is put in byte 3 of the the machine instruction, byte 2 is x
	b23 = 3,  // this token is put in byte 2+3 of the the machine instruction
} tok2mem_t;

typedef struct op_s {
	uint8_t opcode; // opcode
	uint8_t operand_count; // XXX # of operands - currently ignored - DELETE?
	char* mnemonic; // the assembly language mnemonic
	oper_t oper1;   // type of operand 1
	oper_t oper2;   // type of operand 2
	tok2mem_t tok1; // where operarand 1 is put in machine code
	tok2mem_t tok2; // where operarand 2 is put in machine code
} op_t;

// parse a buffer with assembly language; write the opcode to out
// returns the number of bytes written to out
size_t assemble_buffer( char *amsbuf, uint8_t *out );

// splits a given assembly language line in 3 tokens.
// the string s is modified in the process!
// the function returns the number of tokens parsed. the tokens are returned
// in the array token, (of 3 char pointers) must be provided by the caller
// by reference.
//
// tokens are split according to one of the following patterns:
// OP				(1 token - no operands)
// OP OPER1 		(2 tokens - 1 operand)
// OP OPER1, OPER2	(3 tokens - 2 operands)
int tokenize_line( char *s, char *token[3] );


// extracts a label from a given line and stores the corresponding addr with it.
// returns a pointer to the character after the ':', e.g.:
// given "  loop:NOP" the function will store the label and return a pointer to 'N'.

char *label_extract( char *s, int32_t addr );

void label_print();

int label_numelems();

bool get_register( char*str, int *reg );
bool get_immediate_value( char *str, int16_t *val );
bool get_storage_address( char *str, uint16_t *addr );
bool get_register_address( char *str, int *reg ); // address in register: (R1) ... (R3)
bool get_immediate_address( char *str, uint16_t *addr ); // $0x0000, $0xFFF, $256

oper_t get_operand( char *str, void *data );

// print an instruction from 3 tokens passed as an array of 3: char *token[3]
#define PRINT_INSTR(token,end) \
	printf( "%s %s, %s%s\n", (token)[0], (token)[1], (token)[2], (end) )

int find_instruction( char *token[3] );
size_t assemble_instruction(char *token[3], uint8_t *out);

// bla bla
void assemble_file( const char *source_file, const char *output_file );

#endif /* ASM_H_ */
