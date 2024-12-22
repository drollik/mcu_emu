/*
 * asm.c
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */

#include <strings.h>
#include <stdio.h>

#include "asm.h"
#include "../string/strmanip.h"

op_t ops[] = {
		// DATA MOVEMENT
		{0x01, 2, "LD", REG, VAL}, // LD Rx, value
		{0x02, 2, "LD", REG, REG}, // LD Rx, Ry
		{0x03, 2, "LD", REG, ADI}, // LD Rx, (addr)
		{0x04, 2, "LD", REG, ADR}, // LD Rx, (Ry)
		// store
		{0x10, 2, "ST", ADI, REG}, // ST (addr), Ry
		{0x11, 2, "ST", ADR, REG}, // ST (Rx), Ry
		// DATA OPS
		// ...
		{0x24, 1, "NOT", REG, NON}, // NOT Rx
		// ...
		// CONTROL OPS
		{0x50, 1, "BRA", ADB, NON}, // BRA $addr
		{0x51, 1, "BRN", ADB, NON}, // BRN $addr
		{0x52, 1, "BRZ", ADB, NON}, // BRZ $addr
		{0x53, 1, "BRP", ADB, NON}, // BRP $addr

		// MISC
		{0x00, 0, "NOP", NON, NON}, // NOP
		{0xFF, 0, "HALT", NON, NON}, // HALT
};


int tokenize_line( char *s, char *token[3] ) {
	// OP_________________________[space]_
	// OP_space_OPER1_____________[space]_
	// OP_space_OPER1_comma_OPER2_[space]_
	token[0] = token[1] = token[2] = NULL;

	// remove comments, then trim input line
	s = trim_whitespace( remove_comments(s) );
	if( s == NULL || *s == '\0') return 0; // nothing here!

	const char whitespace[] = " \t\v\f"; // whitespace (space, tab, vertical tab, form feed)
	const char comma[] = ","; // comma only
	char *save_ptr = NULL;

	// finding OP
	token[0] = strtok_r( s, whitespace, &save_ptr ); // (OP)  (_OPER1_comma_OPER2_)
	if( token[0] == NULL ) {
		// printf("INTERNAL ERROR: empty line?!");
		return -1;
	}
	// printf( "- token[0]: [%s]\n", token[0] );

	// splitting OPER1_comma_OPER2_ at comma
	s = strtok_r( NULL, comma, &save_ptr ); // (_OPER1_) , (_OPER2_)
	if( s == NULL ) {
		// printf("No operands found!\n");
		return 1;
	}
	// printf( "- left in s: [%s]\n", s );

	s = trim_whitespace( s ); // OPER1
	token[1] = s;
	// printf( "- token[1]: [%s]\n", token[1] );

	// OPER2_
	s = strtok_r( NULL, comma, &save_ptr );
	if( s == NULL ) {
		// printf("2nd operand == NULL\n");
		return 2;
	}
	// printf( "- left in s: [%s]\n", s );

	s = skip_whitespace( s ); // OPER2
	token[2] = s;
	// printf( "- token[2]: [%s]\n", token[2] );

	return 2;
}

// XXX: to be done
void find_op( char *s ) {
	for(int i = 0; i< sizeof(ops)/sizeof(ops[0]); i++) { // for all ops ...

		// check if mnemonic matches
		// XXX check if there is a whitespace following the mnemonic!
		if( starts_with(s, ops[i].mnemonic)) {
			printf("  Mnemonic of [%s] matches opcode 0x%X\n", s, ops[i].opcode);
		}
	}
} // end find_op()



