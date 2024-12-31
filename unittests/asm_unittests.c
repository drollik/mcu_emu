/*
 * asm_unittests.c
 *
 *  Created on: 21 Dec 2024
 *      Author: olive
 */

#include <stdio.h>
#include <strings.h>

#include "../asm/asm.h"
#include "../uCUnit/uCUnit-v1.0.h"

#include "asm_unittests.h"

#include "../string/strmanip.h"


void _test_parse_operands() {
	UCUNIT_TestcaseBegin("asm.parse_operands()");

	bool ok = 666;
	//	bool get_register( char*str, int *reg );
	// operand type REG
	int reg;
	ok = get_register("R0", &reg);
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( reg == 0 );

	ok = get_register("R1", &reg);
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( reg == 1 );

	ok = get_register("R2", &reg);
	UCUNIT_CheckIsTrue( ok );// true
	UCUNIT_CheckIsTrue( reg == 2 );

	ok = get_register("R3", &reg);
	UCUNIT_CheckIsTrue( ok );// true
	UCUNIT_CheckIsTrue( reg == 3 );

	ok = get_register("R4", &reg); // no such register
	UCUNIT_CheckIsFalse( ok ); // false

	ok = get_register("R41", &reg); // has extra space
	UCUNIT_CheckIsFalse( ok ); // false

	ok = get_register("R4 ", &reg); // has extra space
	UCUNIT_CheckIsFalse( ok ); // false

	//	bool get_immediate_value( char *str, int16_t *val );
	// operand type: VAL
	int16_t val = 0;
	ok = get_immediate_value("0", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == 0 );

	ok = get_immediate_value("1", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == 1 );

	ok = get_immediate_value("255", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == 255 );

	ok = get_immediate_value("1000", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == 1000 );

	ok = get_immediate_value("32767", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == 32767 );

	ok = get_immediate_value("-1", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == -1 );

	ok = get_immediate_value("-255", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == -255 );

	ok = get_immediate_value("-1000", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == -1000 );

	ok = get_immediate_value("-32768", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == -32768 );

	ok = get_immediate_value("0xF", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == 15 );

	ok = get_immediate_value("-0xF", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == -15 );

	ok = get_immediate_value("-0x8000", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == -0x8000 );

	ok = get_immediate_value("0x7FFF", &val );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( val == 0x7FFF );

	ok = get_immediate_value("-0x8001", &val ); // too small
	UCUNIT_CheckIsFalse( ok ); // false

	ok = get_immediate_value("0x8000", &val ); // too big
	UCUNIT_CheckIsFalse( ok ); // false

	//	bool get_storage_address( char *str, uint16_t *addr );
	// operand type: ADS
	uint16_t addr = 0;

	ok = get_storage_address("(00000)", &addr ); // (0x0000), (0xFFF), (256)
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( addr == 0 );

	ok = get_storage_address("(256)", &addr ); // (0x0000), (0xFFF), (256)
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( addr == 256 );

	ok = get_storage_address("(0x0000)", &addr ); // (0x0000), (0xFFF), (256)
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( addr == 0 );

	ok = get_storage_address("(0x1000)", &addr ); // (0x0000), (0xFFF), (256)
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( addr == 0x1000 );

	ok = get_storage_address("(0xFFFF)", &addr ); // (0x0000), (0xFFF), (256)
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( addr == 0xFFFF );

	// bool get_register_address( char *str, int *reg ) { // address in register: (R1) ... (R3)
	// operand type: ADR

	ok = get_register_address("(R0)", &reg ); // (R0) ... (R3)
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( reg == 0 );

	ok = get_register_address("(R3)", &reg ); // (R0) ... (R3)
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( reg == 3 );

	// bool get_immediate_address( char *str, uint16_t *addr ) { // $0x0000, $0xFFF, $256
	// operand type:ADB

	//	oper_t get_operand_type( char *str );

	UCUNIT_TestcaseEnd();
}


char lines[] = {
		"   LD   R1, 0x1000  ; bla!! \n"\
		"	LD R2 , R1	   ; copy shit\n"\
		"  	LD R3,(0x1002); more stuff  \n"\
		"\tLD R4, (R3)     ; and so on and so forth\n"\
		"   ST (0x1004), R4  \n"\
		"   ST (R1), R1     \n"\
		"loop:\n" /* label */ \
		"   NOT R1\n"\
		"   NOP    	\n"\
		"	ADD R1, R2\n" \
		"   	\n" \
		"end:  	\n" \
		"	HALT\n"
};

uint8_t mem[256] = {0};

void _test_find_instruction() {

	UCUNIT_TestcaseBegin("asm.find_instruction()");
	char *tokens[3];

	// *** Data Movement ***
	// LD Rx, value
	tokens[0] = "LD"; tokens[1] = "R1", tokens[2] = "0x1000";
	find_instruction(tokens); // opcode 0x01

	// LD Rx, Ry
	tokens[0] = "LD"; tokens[1] = "R1", tokens[2] = "R2";
	find_instruction(tokens); // opcode 0x02

	// LD Rx, (addr)
	tokens[0] = "LD"; tokens[1] = "R1", tokens[2] = "(0x1000)";
	find_instruction(tokens); // opcode 0x03

	// LD Rx, (Ry)
	tokens[0] = "LD"; tokens[1] = "R1", tokens[2] = "(R2)";
	find_instruction(tokens); // opcode 0x04

	// ST (addr), Ry
	tokens[0] = "ST"; tokens[1] = "(0x1000)", tokens[2] = "R1";
	find_instruction(tokens); // opcode 0x10

	// ST (Rx), Ry
	tokens[0] = "ST"; tokens[1] = "(R3)", tokens[2] = "R1";
	find_instruction(tokens); // opcode 0x11

	// *** Data Processing ***
	// XXX TBD: 0x20 - 0x23

	// NOT Rx
	tokens[0] = "NOT"; tokens[1] = "R3", tokens[2] = "";
	find_instruction(tokens); // opcode 0x24

	// XXX TBD: 0x26 - 0x29

	// *** Control Operations ***

	// BRA $1000; BRA label
	tokens[0] = "BRA"; tokens[1] = "$0x1000", tokens[2] = "";
	find_instruction(tokens); // opcode 0x50

	// XXX TBD: 0x51 - 0x53

	// *** Misc Operations ***
	// NOP
	tokens[0] = "NOP"; tokens[1] = "", tokens[2] = "";
	find_instruction(tokens); // opcode 0x00

	// HALT
	tokens[0] = "HALT"; tokens[1] = "", tokens[2] = "";
	find_instruction(tokens); // opcode 0xFF


	UCUNIT_TestcaseEnd();
}

void _test_parse_all() {
	size_t count = 0;
	UCUNIT_TestcaseBegin("asm.parse_all()");
	count = parse_all( lines, mem );
	label_print();

	// expected, actual
	UCUNIT_CheckIsTrue( count > 0 ); // how long is the generated machine code?
	UCUNIT_CheckIsTrue( 2 == label_numelems() ); // 2 labels contained in lines

	UCUNIT_TestcaseEnd();
}


void run_asm_unittests(void) {
	UCUNIT_ResetTracepointCoverage(); // unused
	_test_parse_operands();
	_test_find_instruction();
	// _test_parse_all();
	UCUNIT_WriteSummary(); // uCUnit test summary


//	char delim[]="\n";
//	char *next;
//	char *lines_save;
//
//	// break lines at the newline (\n) char - modifies the input!
//	next = strtok_r(lines, delim, &lines_save);
//	while( next != NULL) {
//		printf("XXX: check if label!!\n");
//
//		printf("Looking at   [%s]:\n", next);
//		next = remove_comments(next);
//		printf("wo comments: [%s]:\n", next);
//		next = trim_whitespace(next);
//		printf("trimmed:     [%s]:\n", next);
//
//		next = extract_label( next );
//		next = trim_whitespace(next);
//
//		char* token[3];
//		tokenize_line(next, token);
//		printf( "  OP: %s\n", token[0] );
//		printf( "  OPER1: %s\n", token[1] );
//		printf( "  OPER2: %s\n", token[2] );
//
//		// find_op(next);
//		next = strtok_r(NULL, delim, &lines_save);
//	}
}
