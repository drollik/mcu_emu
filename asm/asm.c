/*
 * asm.c
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */

#include <stdint.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h> // isspace

#include "asm.h"

#include "../string/llist.h"
#include "../string/strmanip.h"

op_t ops[] = {
// DATA MOVEMENT
		{ 0x01, 2, "LD", REG, VAL }, // LD Rx, value
		{ 0x02, 2, "LD", REG, REG }, // LD Rx, Ry
		{ 0x03, 2, "LD", REG, ADS }, // LD Rx, (addr)
		{ 0x04, 2, "LD", REG, ADR }, // LD Rx, (Ry)
		// store
		{ 0x10, 2, "ST", ADS, REG }, // ST (addr), Ry
		{ 0x11, 2, "ST", ADR, REG }, // ST (Rx), Ry
		// DATA OPS
		// ...
		{ 0x24, 1, "NOT", REG, NON }, // NOT Rx
		// ...
		// CONTROL OPS
		{ 0x50, 1, "BRA", ADB, NON }, // BRA $addr
		{ 0x51, 1, "BRN", ADB, NON }, // BRN $addr
		{ 0x52, 1, "BRZ", ADB, NON }, // BRZ $addr
		{ 0x53, 1, "BRP", ADB, NON }, // BRP $addr

		// MISC
		{ 0x00, 0, "NOP", NON, NON }, // NOP
		{ 0xFF, 0, "HALT", NON, NON }, // HALT
		};

static char * const oper_type_names[] ={
	"ERROR", // error
	"NON", // no operand
	"REG", // register: R1 ... R3
	"VAL", // immediate value: 0xABCD, 1234, -1234, const
	"ADI", // address: (0x1000), (label)
	"ADR", // address in register: (R1) ... (R3)
	"ADB" // address for branch instruction $0x1000, $label
};

const char * get_oper_name( oper_t op ) {
	if( op < OPER_START || OPER_END < op ) return "unknown operand type";
	return oper_type_names[op];
}

// CHAR_IN('3', '0', '3')
#define CHAR_IN(ch,srt,end) ((srt)<=(ch) && (ch) <=(end))

//bool is_register(char *str) {
//	if(str[0] != 'R' && str[0] != 'r') return false;
//	return CHAR_IN(str[1], '0', '3') && str[2] == '\0';
//}
//
//int get_register2( char *str ) {
//	if( is_register(str) ) return -1;
//	else return '0' - str[1];
//}

bool get_register( char*str, int *reg ) { // R0, R1, R2, R3
	if(str[0] != 'R' && str[0] != 'r') return false; // must starts with R
	if( !CHAR_IN(str[1], '0', '3') ) return false; // not a number 0..3
	if(	str[2] != '\0' ) return false; // has leftover characters
	if( reg ) *reg = str[1] -'0';
	return true;
}

bool get_immediate_value( char *str, int16_t *val ) { // 0xFFF, 32, -1, -17
    char* end = NULL;
	const long i = strtol(str, &end, 0); // str to long, base 0 = auto-detect
	if( str == end || errno != 0) return false;
	if( i < -0x8000 || 0x7FFF < i) return false; // out of range
	if( val ) *val = (int16_t)i;
	return true;
}

bool get_storage_address( char *str, uint16_t *addr ) { // (0x0000), (0xFFF), (256)
	// XXX needs to be able to use lables!!!
	if(str[0] != '(' ) return false; // must starts with '('
    char* end = NULL;
	str++;
	const long i = strtol(str, &end, 0); // str to long, base 0 = auto-detect
	if( str == end || errno != 0) return false;
	if( i < 0 || 0xFFFF < i) return false; // out of range
	if( *end != ')') return false; // must end with ')'
	if( addr ) *addr = (uint16_t)i;
	return true;
}

bool get_register_address( char *str, int *reg ) { // address in register: (R1) ... (R3)
	if(str[0] != '(' ) return false; // must starts with '('
	if(str[1] != 'R' && str[1] != 'r') return false; // must start with R
	if( !CHAR_IN(str[2], '0', '3') ) return false; // not a number 0..3
	if(	str[3] != ')' ) return false; // must end with ')'
	if(	str[4] != '\0' ) return false; // has leftover characters
	if( reg ) *reg = str[2] -'0';
	return true;
}

// immediate address for branching BRA $addr
bool get_immediate_address( char *str, uint16_t *addr ) { // $0x0000, $0xFFF, $256
	// XXX needs to be able to use lables!!!
	if(str[0] != '$' ) return false; // must starts with '('
    char* end = NULL;
	str++;
	const long i = strtol(str, &end, 0); // str to long, base 0 = auto-detect
	if( str == end || errno != 0) return false; // decode error or overflow
	if( i < 0 || 0xFFFF < i) return false; // out of range for 16 bit
	if( *end != '\0') return false; // must end after address
	if( addr ) *addr = (uint16_t)i;
	return true;
}


oper_t get_operand_type( char *str ) {
	if( str == NULL || *str == '\0') return NON;
	else if( get_register( str, NULL ) ) return REG;
	else if( get_immediate_value( str, NULL ) ) return VAL;
	else if( get_storage_address( str, NULL ) ) return ADS;
	else if( get_register_address( str, NULL ) ) return ADR;
	else if( get_immediate_address( str, NULL ) ) return ADR;
	else return -666;
}

//typedef enum oper_e {
//	ERROR = 0,
//	NON, // no operand
//	REG, // register: R1 ... R3
//	VAL, // immediate value: 0xABCD, 1234, -1234, const
//	ADI, // storage address: (0x1000), (label)
//	ADR, // address in register: (R1) ... (R3)
//	ADB // address for branch instruction $0x1000, $label
//} oper_t;


// static uint16_t current_address; // current memory address of the assembled code
static llnode_t *labels = NULL; // list of labels with their corresponding addresses

// finds the instruction in the instruction list (ops) and returns its index
// or a negative value if the instruction could not be found.
int find_instruction( char *token[3] ) {
	printf( "%s %s, %s\n", token[0], token[1], token[2] );
	int i, num_ops = sizeof(ops)/sizeof(ops[0]);

	// for every instruction
	for(i = 0; i<num_ops; i++ ) {
		printf( "0x%3x: %s ", ops[i].opcode, ops[i].mnemonic );
		printf( "%s, %s -- ", get_oper_name(ops[i].oper1), get_oper_name(ops[i].oper2) );

		if( strcmp(token[0], ops[i].mnemonic) == 0) { // mnemonic match
			printf("might match - ");
			if( get_operand_type(token[1]) == ops[i].oper1 ) {
				printf("oper1 does match - "); // oper1 match
				if( get_operand_type(token[2]) == ops[i].oper2 ) {
					printf("and oper2 does match!\n"); // oper2 match
					break;
				} else {
					printf("but oper2 does NOT match\n");
					continue;
				}
			} else {
				printf("oper1 does NOT match\n");
				continue;
			}
		} else // NO mnemonic match
			printf("\n");
	} // end for every instruction

	if( i >= num_ops) // instruction not found
		return -666;
	else
		return i;
}

int assemble_line_(char *token[3], uint8_t *out) {

	int inst_no = find_instruction(token);

	// opcode always goes first
	*(out+0) = ops[inst_no].opcode;

	// in most ops, token[1] goes 2nd, and token[2] goes 3rd
	// in one op, token[1] goes 3rd and token[2] goes 2nd: "ST (addr), Ry"
	// in some ops without a token[2], token[1] goes 3rd: "BRA $0x1000"

	// in other words:
	// an address (ADS, ADV) or value (VAL), always goes 3rd in machine code
	// (regardless if it's in token[1] or token[2]).

	// registers (REG, ADR) in token[1] go 2nd
	// UNLESS there is an address, then they go 3rd only in "ST (addr), Ry"/ op 0x10

	// registers (REG, ADR) in token[2] go 3nd





	return 666;
}


// parse a buffer with assembly language; write the opcode to out
// returns the number of bytes written to out
size_t parse_all( char *amsbuf, uint8_t *out ) {
	size_t count = 0;
	char delim[] = "\n";
	char *next;
	char *lines_save;

	// break lines at the newline (\n) char - modifies the input!
	next = strtok_r( amsbuf, delim, &lines_save );
	while (next != NULL) {

		printf( "Looking at   [%s]:\n", next );
		next = remove_comments( next );
		printf( "wo comments: [%s]:\n", next );
		next = trim_whitespace( next );
		printf( "trimmed:     [%s]:\n", next );

		next = label_extract( next, count ); // any labels?
		// next = skip_whitespace( next ); // label_extrac() already skips

		char *token[3];
		tokenize_line( next, token );
		printf( "  OP: %s\n", token[0] );
		printf( "  OPER1: %s\n", token[1] );
		printf( "  OPER2: %s\n", token[2] );

		count += 4;
		next = strtok_r( NULL, delim, &lines_save );
	}
	return count;
}

int tokenize_line(char *s, char *token[3]) {
	// OP_________________________[space]_
	// OP_space_OPER1_____________[space]_
	// OP_space_OPER1_comma_OPER2_[space]_
	token[0] = token[1] = token[2] = NULL;

	// remove comments, then trim input line
	s = trim_whitespace( remove_comments( s ) );
	if (s == NULL || *s == '\0')
		return 0; // nothing here!

	const char whitespace[] = " \t\v\f"; // whitespace (space, tab, vertical tab, form feed)
	const char comma[] = ","; // comma only
	char *save_ptr = NULL;

	// finding OP
	token[0] = strtok_r( s, whitespace, &save_ptr ); // (OP)  (_OPER1_comma_OPER2_)
	if (token[0] == NULL) {
		// printf("INTERNAL ERROR: empty line?!");
		return -1;
	}
	// printf( "- token[0]: [%s]\n", token[0] );

	// splitting OPER1_comma_OPER2_ at comma
	s = strtok_r( NULL, comma, &save_ptr ); // (_OPER1_) , (_OPER2_)
	if (s == NULL) {
		// printf("No operands found!\n");
		return 1;
	}
	// printf( "- left in s: [%s]\n", s );

	s = trim_whitespace( s ); // OPER1
	token[1] = s;
	// printf( "- token[1]: [%s]\n", token[1] );

	// OPER2_
	s = strtok_r( NULL, comma, &save_ptr );
	if (s == NULL) {
		// printf("2nd operand == NULL\n");
		return 2;
	}
	// printf( "- left in s: [%s]\n", s );

	s = skip_whitespace( s ); // OPER2
	token[2] = s;
	// printf( "- token[2]: [%s]\n", token[2] );

	return 2;
}

// extracts the label and stores it together with the given address, the label refers to.
//
// note: expects no whitespace before label
char* label_extract(char *line, int32_t addr) {
	if(*line == '\0') return line; // return if end of line
	assert( !isspace(*line) ); // expects no whitespace

	// inspect a line if it contains a ':' => has a label
	char *colon = strchr( line, ':' );
	if (colon == NULL) {
		// printf( "  contains no label!\n" );
		return line;
	}
	*colon = '\0'; // replace colon with end of string
	printf( "label is: %s\n", line );
	labels = list_add_first( labels, line, addr );

	// remove whitespace after the character ':'
	return skip_whitespace(colon + 1);
}

void label_print() {
	printf("Labels:\n");
	list_print( labels );
}

int label_numelems() {
	return list_numelems( labels );
}

// XXX: to be done
void find_op(char *s) {
	for (int i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) { // for all ops ...

		// check if mnemonic matches
		// XXX check if there is a whitespace following the mnemonic!
		if (starts_with( s, ops[i].mnemonic )) {
			printf( "  Mnemonic of [%s] matches opcode 0x%X\n", s,
					ops[i].opcode );
		}
	}
} // end find_op()

