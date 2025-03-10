/*
 * asm_unittests.c
 *
 *  Created on: 21 Dec 2024
 *      Author: olive
 */

#include <stdio.h>
#include <strings.h>

#include "asm_unittests.h"
#include "../asm/asm.h"
#include "../asm/labels.h"
#include "../mcu/mem.h"
#include "../uCUnit/uCUnit-v1.0.h"
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

	ok = get_immediate_address("$0", &addr );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( addr == 0 );

	ok = get_immediate_address("$0x1000", &addr );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( addr == 0x1000 );

	ok = get_immediate_address("$0xFFFF", &addr );
	UCUNIT_CheckIsTrue( ok ); // true
	UCUNIT_CheckIsTrue( addr == 0xFFFF );

	ok = get_immediate_address("$0x1FFFF", &addr ); // to big
	UCUNIT_CheckIsFalse( ok ); // true

	ok = get_immediate_address("$-0x0001", &addr ); // to small
	UCUNIT_CheckIsFalse( ok ); // true

	// XXX tests missing?

	UCUNIT_TestcaseEnd();
}


void _test_get_operand() {
	UCUNIT_TestcaseBegin("asm.get_operand()");

	oper_t opertype = NON;
	uint32_t data = 0;

	opertype = get_operand( "R1", &data );
	UCUNIT_CheckIsTrue( opertype == REG ); // true
	UCUNIT_CheckIsTrue( (uint8_t)data == 1 );

	opertype = get_operand( "0x1000", &data );
	UCUNIT_CheckIsTrue( opertype == VAL ); // true
	UCUNIT_CheckIsTrue( (int16_t)data == 0x1000 );

	opertype = get_operand( "0xF", &data );
	UCUNIT_CheckIsTrue( opertype == VAL ); // true
	UCUNIT_CheckIsTrue( (int16_t)data == 0xF );

	opertype = get_operand( "-0xF", &data );
	UCUNIT_CheckIsTrue( opertype == VAL ); // true
	UCUNIT_CheckIsTrue( (int16_t)data == -0xF);

	opertype = get_operand( "-0x8000", &data );
	UCUNIT_CheckIsTrue( opertype == VAL ); // true
	UCUNIT_CheckIsTrue( (int16_t)data == -0x8000 );

	opertype = get_operand( "0x7FFF", &data );
	UCUNIT_CheckIsTrue( opertype == VAL ); // true
	UCUNIT_CheckIsTrue( (int16_t)data == 0x7FFF );

	opertype = get_operand( "-0x8001", &data ); // too small
	UCUNIT_CheckIsTrue( opertype == OPER_ERROR ); // true

	opertype = get_operand( "0x8000", &data ); // too big
	UCUNIT_CheckIsTrue( opertype == OPER_ERROR ); // true

	UCUNIT_TestcaseEnd();
}


void _test_labels() {
	UCUNIT_TestcaseBegin("asm.label_*");

//	char *label_extract( char *s, int32_t addr );
//	void label_print();
//	int label_numelems();
//	int32_t label_get_address( char *label );

	char line[64] = {'\0'};
	char *next = NULL;

	// initially empty
	UCUNIT_CheckIsTrue( label_numelems() == 0 );

	strcpy( line, "one_thousand:x" );
	next = label_extract( line, 0x1000 );
	UCUNIT_CheckIsEqual( 'x', *next );
	UCUNIT_CheckIsEqual( 1, label_numelems() );
	UCUNIT_CheckIsEqual( 0x1000, label_get_address("one_thousand") );

	strcpy( line, "two_thousand : LD R1, R2" );
	next = label_extract( line, 0x2000 );
	UCUNIT_CheckIsEqual( 'L', *next );
	UCUNIT_CheckIsEqual( 2, label_numelems() );
	UCUNIT_CheckIsEqual( 0x2000, label_get_address("two_thousand") );
	UCUNIT_CheckIsTrue( strcmp( next, "LD R1, R2" ) == 0 );

	strcpy( line, "four: thousand:" ); // 2 labels in one line
	next = label_extract( line, 0x4 );
	UCUNIT_CheckIsEqual( 't', *next ); // "thousand"
	UCUNIT_CheckIsEqual( 3, label_numelems() );
	UCUNIT_CheckIsEqual( 0x4, label_get_address("four") );

	next = label_extract( next, 0x1000 );
	UCUNIT_CheckIsEqual( '\0', *next ); // end of line
	UCUNIT_CheckIsEqual( 4, label_numelems() );
	UCUNIT_CheckIsEqual( 0x1000, label_get_address("thousand") );

	label_delete_all();
	UCUNIT_CheckIsEqual( 0, label_numelems() );

	// ERRORs
	strcpy( line, "three thousand:" );
	next = label_extract( line, 0x3000 );
	UCUNIT_CheckIsEqual( NULL, next );
	UCUNIT_CheckIsEqual( 0, label_numelems() ); // unchanged

	label_delete_all();
	UCUNIT_CheckIsEqual( 0, label_numelems() );

	UCUNIT_TestcaseEnd();
}

void _test_tokenize_line() {
	// testing int tokenize_line( char *line, char *token[3] );

	UCUNIT_TestcaseBegin("asm.tokenize_line()");
	char *tokens[3] = {NULL};
	char line[64] = {'\0'};
	int n = 0;

	strcpy( line, " NOP " );
	n = tokenize_line( line, tokens );
	UCUNIT_CheckIsTrue( n = 1 );
	UCUNIT_CheckIsTrue( strcmp( tokens[0], "NOP" ) == 0 );
	UCUNIT_CheckIsNull( tokens[1] );
	UCUNIT_CheckIsNull( tokens[2] );

	strcpy( line, " NOT R1 " );
	n = tokenize_line( line, tokens );
	UCUNIT_CheckIsTrue( n == 2 );
	UCUNIT_CheckIsTrue( strcmp( tokens[0], "NOT" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[1], "R1" ) == 0 );
	UCUNIT_CheckIsNull( tokens[2] );

	strcpy( line, " LD R1,R2 " );
	n = tokenize_line( line, tokens );
	UCUNIT_CheckIsTrue( n == 3 );
	UCUNIT_CheckIsTrue( strcmp( tokens[0], "LD" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[1], "R1" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[2], "R2" ) == 0 );

	strcpy( line, " LD R1, R2 " );
	n = tokenize_line( line, tokens );
	UCUNIT_CheckIsTrue( n == 3 );
	UCUNIT_CheckIsTrue( strcmp( tokens[0], "LD" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[1], "R1" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[2], "R2" ) == 0 );

	strcpy( line, " LD R1 , R2 " );
	n = tokenize_line( line, tokens );
	UCUNIT_CheckIsTrue( n == 3 );
	UCUNIT_CheckIsTrue( strcmp( tokens[0], "LD" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[1], "R1" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[2], "R2" ) == 0 );

	strcpy( line, " LD R1 ,R2 " );
	n = tokenize_line( line, tokens );
	UCUNIT_CheckIsTrue( n == 3 );
	UCUNIT_CheckIsTrue( strcmp( tokens[0], "LD" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[1], "R1" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[2], "R2" ) == 0 );

	// LOGIC ERRORS
	strcpy( line, " LDR1, R2 " ); // space missing btw mnemonic & operand
	n = tokenize_line( line, tokens );
	UCUNIT_CheckIsTrue( n == 2 );
	UCUNIT_CheckIsTrue( strcmp( tokens[0], "LDR1," ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[1], "R2" ) == 0 );
	UCUNIT_CheckIsNull( tokens[2] );

	strcpy( line, "LD, R1, R2" );
	n = tokenize_line( line, tokens ); // excess comma after mnemonic
	UCUNIT_CheckIsTrue( n == 3 );
	UCUNIT_CheckIsTrue( strcmp( tokens[0], "LD," ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[1], "R1" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[2], "R2" ) == 0 );

	strcpy( line, " LD R1 R2 " ); // comma missing after R1
	n = tokenize_line( line, tokens );
	UCUNIT_CheckIsTrue( n == 2 );
	UCUNIT_CheckIsTrue( strcmp( tokens[0], "LD" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[1], "R1 R2" ) == 0 );  // "R1 R2" => token[1]
	UCUNIT_CheckIsNull( tokens[2] );

	strcpy( line, "LD, R1, R2 XYZ" ); // excess comma after mnemonic plus
	n = tokenize_line( line, tokens ); // excess characters after operand 2
	UCUNIT_CheckIsTrue( n == 3 );
	UCUNIT_CheckIsTrue( strcmp( tokens[0], "LD," ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[1], "R1" ) == 0 );
	UCUNIT_CheckIsTrue( strcmp( tokens[2], "R2 XYZ" ) == 0 );

	// ERRORS with respect to expected sequence
	strcpy( line, " LD R1, " );
	n = tokenize_line( line, tokens ); // 2nd operand missing after comma
	UCUNIT_CheckIsTrue( n == -1 );
	UCUNIT_CheckIsNull( tokens[1] );
	UCUNIT_CheckIsNull( tokens[1] );
	UCUNIT_CheckIsNull( tokens[2] );

	strcpy( line, " LD R1, R2," );
	n = tokenize_line( line, tokens ); // excess comma
	UCUNIT_CheckIsTrue( n == -1 );
	UCUNIT_CheckIsNull( tokens[1] );
	UCUNIT_CheckIsNull( tokens[1] );
	UCUNIT_CheckIsNull( tokens[2] );


	UCUNIT_TestcaseEnd();
}



void _test_assemble_instruction() {
	UCUNIT_TestcaseBegin("asm.assemble_instruction()");

	char *tokens[3];
	uint8_t instr[4] = {0}; // the machine code for 1 instruction

	// *** Data Movement ***
	// LD Rx, value
	tokens[0] = "LD"; tokens[1] = "R3", tokens[2] = "0x1000"; // opcode 0x01
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x01, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 3, *(instr+1) ); // register
	UCUNIT_CheckIsEqual( HI_BYTE(0x1000), *(instr+2) ); // register
	UCUNIT_CheckIsEqual( LO_BYTE(0x1000), *(instr+3) ); // register

	// LD Rx, value
	tokens[0] = "LD"; tokens[1] = "R2", tokens[2] = "0x0BAD"; // opcode 0x01
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x01, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 2, *(instr+1) ); // register
	UCUNIT_CheckIsEqual( HI_BYTE(0x0BAD), *(instr+2) ); // register
	UCUNIT_CheckIsEqual( LO_BYTE(0x0BAD), *(instr+3) ); // register

	// LD Rx, value
	tokens[0] = "LD"; tokens[1] = "R2", tokens[2] = "-0x8000"; // opcode 0x01
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x01, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 2, *(instr+1) ); // register
	UCUNIT_CheckIsEqual( HI_BYTE(-0x8000), *(instr+2) ); // register
	UCUNIT_CheckIsEqual( LO_BYTE(-0x8000), *(instr+3) ); // register

	// LD Rx, value
	tokens[0] = "LD"; tokens[1] = "R2", tokens[2] = "0x7FFF"; // opcode 0x01
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x01, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 2, *(instr+1) ); // register
	UCUNIT_CheckIsEqual( HI_BYTE(0x7FFF), *(instr+2) ); // register
	UCUNIT_CheckIsEqual( LO_BYTE(0x7FFF), *(instr+3) ); // register

	// LD Rx, Ry
	tokens[0] = "LD"; tokens[1] = "R1", tokens[2] = "R2"; // opcode 0x02
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x02, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 1, *(instr+1) ); // register R1
	UCUNIT_CheckIsEqual( 0, *(instr+2) ); // x
	UCUNIT_CheckIsEqual( 2, *(instr+3) ); // register R2

	// LD Rx, (addr)
	tokens[0] = "LD"; tokens[1] = "R3", tokens[2] = "(0x1000)"; // opcode 0x3
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x03, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 3, *(instr+1) ); // register R3
	UCUNIT_CheckIsEqual( HI_BYTE(0x1000), *(instr+2) ); // 0x1000
	UCUNIT_CheckIsEqual( LO_BYTE(0x1000), *(instr+3) ); // 0x1000

	// LD Rx, (Ry)
	tokens[0] = "LD"; tokens[1] = "R2", tokens[2] = "(R3)"; // opcode 0x4
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x04, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 2, *(instr+1) ); // register R2
	UCUNIT_CheckIsEqual( 0, *(instr+2) ); // x
	UCUNIT_CheckIsEqual( 3, *(instr+3) ); // register R3

	// ST (addr), Ry - STORED BACKWARDS
	tokens[0] = "ST"; tokens[1] = "(0x1000)", tokens[2] = "R3"; // opcode 0x10
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x10, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 3, *(instr+1) ); // register R3
	UCUNIT_CheckIsEqual( HI_BYTE(0x1000), *(instr+2) ); // 0x1000
	UCUNIT_CheckIsEqual( LO_BYTE(0x1000), *(instr+3) ); // 0x1000

	// ST (Rx), Ry - STORED BACKWARDS
	tokens[0] = "ST"; tokens[1] = "(R3)", tokens[2] = "R1"; // opcode 0x11
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x11, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 1, *(instr+1) ); // register R1
	UCUNIT_CheckIsEqual( 0, *(instr+2) ); // x
	UCUNIT_CheckIsEqual( 3, *(instr+3) ); // register R3

	// *** Data Processing ***
	// XXX TBD: 0x20 - 0x23
	// ADD Rx, value
	// ADD Rx, Ry
	// SUB Rx, value
	// SUB Rx, Ry

	// NOT Rx
	tokens[0] = "NOT"; tokens[1] = "R3", tokens[2] = ""; // opcode 0x24
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x24, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 3, *(instr+1) ); // register R3
	UCUNIT_CheckIsEqual( 0, *(instr+2) ); // x
	UCUNIT_CheckIsEqual( 0, *(instr+3) ); // x

	// XXX TBD: 0x26 - 0x29
	// AND Rx, value
	// AND Rx, Ry
	// CMP Rx, value
	// CMP Rx, Ry

	// *** Control Operations ***

	// BRA $1000; BRA label
	tokens[0] = "BRA"; tokens[1] = "$0x1000", tokens[2] = ""; // opcode 0x50
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x50, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 0, *(instr+1) ); // x
	UCUNIT_CheckIsEqual( HI_BYTE(0x1000), *(instr+2) ); // 0x1000
	UCUNIT_CheckIsEqual( LO_BYTE(0x1000), *(instr+3) ); // 0x1000

	// XXX TBD: 0x51 - 0x53
	// BRN $addr
	// BRZ $addr
	// BRP $addr

	// *** Misc Operations ***
	// NOP
	tokens[0] = "NOP"; tokens[1] = "", tokens[2] = ""; // opcode 0x00
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0x00, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 0, *(instr+1) ); // x
	UCUNIT_CheckIsEqual( 0, *(instr+2) ); // x
	UCUNIT_CheckIsEqual( 0, *(instr+3) ); // x

	// HALT
	tokens[0] = "HALT"; tokens[1] = "", tokens[2] = ""; // opcode 0xFF
	assemble_instruction( tokens, instr );
	UCUNIT_CheckIsEqual( 0xFF, *(instr+0) ); // opcode
	UCUNIT_CheckIsEqual( 0, *(instr+1) ); // x
	UCUNIT_CheckIsEqual( 0, *(instr+2) ); // x
	UCUNIT_CheckIsEqual( 0, *(instr+3) ); // x

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
	int instr_no = 0;
	extern op_t ops[];

	// *** Data Movement ***
	// LD Rx, value
	tokens[0] = "LD"; tokens[1] = "R1", tokens[2] = "0x1000"; // opcode 0x01
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no >= 0 ); // valid instruction number
	UCUNIT_CheckIsTrue( 0x01 == ops[instr_no].opcode );

	// LD Rx, value
	tokens[0] = "LD"; tokens[1] = "R1", tokens[2] = "0xCAFE"; // opcode 0x01
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no < 0 ); // error: number too big

	// LD Rx, Ry
	tokens[0] = "LD"; tokens[1] = "R1", tokens[2] = "R2"; // opcode 0x02
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no >= 0 ); // valid instruction number
	UCUNIT_CheckIsTrue( 0x02 == ops[instr_no].opcode );

	// LD Rx, (addr)
	tokens[0] = "LD"; tokens[1] = "R1", tokens[2] = "(0x1000)"; // opcode 0x03
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no >= 0 ); // valid instruction number
	UCUNIT_CheckIsTrue( 0x03 == ops[instr_no].opcode );

	// LD Rx, (Ry)
	tokens[0] = "LD"; tokens[1] = "R1", tokens[2] = "(R2)"; // opcode 0x04
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no >= 0 ); // valid instruction number
	UCUNIT_CheckIsTrue( 0x04 == ops[instr_no].opcode );

	// ST (addr), Ry
	tokens[0] = "ST"; tokens[1] = "(0x1000)", tokens[2] = "R1"; // opcode 0x10
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no >= 0 ); // valid instruction number
	UCUNIT_CheckIsTrue( 0x10 == ops[instr_no].opcode );

	// ST (Rx), Ry
	tokens[0] = "ST"; tokens[1] = "(R3)", tokens[2] = "R1"; // opcode 0x11
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no >= 0 ); // valid instruction number
	UCUNIT_CheckIsTrue( 0x11 == ops[instr_no].opcode );


	// *** Data Processing ***
	// XXX TBD: 0x20 - 0x23

	// NOT Rx
	tokens[0] = "NOT"; tokens[1] = "R3", tokens[2] = ""; // opcode 0x24
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no >= 0 ); // valid instruction number
	UCUNIT_CheckIsTrue( 0x24 == ops[instr_no].opcode );

	// XXX TBD: 0x26 - 0x29

	// *** Control Operations ***

	// BRA $1000; BRA label
	tokens[0] = "BRA"; tokens[1] = "$0x1000", tokens[2] = ""; // opcode 0x50
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no >= 0 ); // valid instruction number
	UCUNIT_CheckIsTrue( 0x50 == ops[instr_no].opcode );

	// XXX TBD: 0x51 - 0x53

	// *** Misc Operations ***
	// NOP
	tokens[0] = "NOP"; tokens[1] = "", tokens[2] = ""; // opcode 0x00
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no >= 0 ); // valid instruction number
	UCUNIT_CheckIsTrue( 0x00 == ops[instr_no].opcode );

	// HALT
	tokens[0] = "HALT"; tokens[1] = "", tokens[2] = ""; // opcode 0xFF
	instr_no = find_instruction( tokens );
	UCUNIT_CheckIsTrue( instr_no >= 0 ); // valid instruction number
	UCUNIT_CheckIsTrue( 0xFF == ops[instr_no].opcode );

	UCUNIT_TestcaseEnd();
}

//void _test_assemble_buffer() {
//	size_t count = 0;
//	UCUNIT_TestcaseBegin("asm.assemble_buffer()");
//	count = assemble_buffer( lines, mem );
//	label_print();
//
//	// expected, actual
//	UCUNIT_CheckIsTrue( count > 0 ); // how long is the generated machine code?
//	UCUNIT_CheckIsTrue( 2 == label_numelems() ); // 2 labels contained in lines
//
//	UCUNIT_TestcaseEnd();
//}


void run_asm_unittests(void) {
	UCUNIT_ResetTracepointCoverage(); // unused
	_test_parse_operands();
	_test_get_operand();
	_test_labels();
	_test_tokenize_line();
	_test_find_instruction();
	_test_assemble_instruction();
	// _test_assemble_buffer;
	UCUNIT_WriteSummary(); // uCUnit test summary

}
