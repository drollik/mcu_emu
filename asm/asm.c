/*
 * asm.c
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>

#include "asm.h"
#include "labels.h"
#include "../mcu/mem.h" // HI_BYTE, LO_BYTE

#include "../string/llist.h"
#include "../string/strmanip.h"


// this table determines how assembly language is assembled into machine code
op_t ops[] = {
		// DATA MOVEMENT
		{ 0x01, 2, "LD", REG, VAL, b1, b23 },  // LD Rx, value
		{ 0x02, 2, "LD", REG, REG, b1, b_3 },  // LD Rx, Ry
		{ 0x03, 2, "LD", REG, ADS, b1, b23 },  // LD Rx, (addr)
		{ 0x04, 2, "LD", REG, ADR, b1, b_3 },  // LD Rx, (Ry)
		// store
		{ 0x10, 2, "ST", ADS, REG, b23, b1},   // ST (addr), Ry -STORED BACKWARDS
		{ 0x11, 2, "ST", ADR, REG, b_3, b1 },  // ST (Rx), Ry -STORED BACKWARDS
		// DATA OPS
		// and, sub
		{ 0x20, 2, "ADD", REG, VAL, b1, b23 }, // ADD Rx, value
		{ 0x21, 2, "ADD", REG, REG, b1, b_3 }, // ADD Rx, Ry
		{ 0x22, 2, "SUB", REG, VAL, b1, b23 }, // SUB Rx, value
		{ 0x23, 2, "SUB", REG, REG, b1, b_3 }, // SUB Rx, Ry
		// not
		{ 0x24, 1, "NOT", REG, NON, b1, xx },  // NOT Rx
		// and, cmp
		{ 0x26, 2, "AND", REG, VAL, b1, b23 }, // ADD Rx, value
		{ 0x27, 2, "AND", REG, REG, b1, b_3 }, // ADD Rx, Ry
		{ 0x28, 2, "CMP", REG, VAL, b1, b23 }, // ADD Rx, value
		{ 0x29, 2, "CMP", REG, REG, b1, b_3 }, // ADD Rx, Ry
		// CONTROL OPS
		{ 0x50, 1, "BRA", ADB, NON, b23, xx }, // BRA $addr
		{ 0x51, 1, "BRN", ADB, NON, b23, xx }, // BRN $addr
		{ 0x52, 1, "BRZ", ADB, NON, b23, xx }, // BRZ $addr
		{ 0x53, 1, "BRP", ADB, NON, b23, xx }, // BRP $addr

		// MISC
		{ 0x00, 0, "NOP", NON, NON, xx, xx },  // NOP
		{ 0xFF, 0, "HALT", NON, NON, xx, xx }, // HALT
};

static char * const oper_type_names[] ={
	"ERROR", // error
	"NON", // no operand
	"REG", // register: R1 ... R3
	"VAL", // immediate value: 0xABCD, 1234, -1234, const
	"ADS", // storage address: (0x1000), (label)
	"ADR", // address in register: (R1) ... (R3)
	"ADB" // address for branch instruction $0x1000, $label
};

// returns the name of an oper_t as const string.
const char * get_oper_name( oper_t op ) {
	if( op < OPER_START || OPER_END < op ) return "unknown operand type";
	return oper_type_names[op];
}

// CHAR_IN('3', '0', '3')
#define CHAR_IN(ch,srt,end) ((srt)<=(ch) && (ch) <=(end))

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
	if( str == end || errno != 0) return false; // couldn't parse
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
	if( str[0] != '$' ) return false; // must starts with '('
    char* end = NULL;
	str++;

	// check if str is either a known label or a number
	int32_t label_addr = label_get_address( str );
	if( label_addr >= 0 ) {
		*addr = label_addr; // found label
		return true;
	}

	const long i = strtol(str, &end, 0); // str to long, base 0 = auto-detect
	if( str == end || errno != 0) return false; // decode error or overflow
	if( i < 0 || 0xFFFF < i) return false; // out of range for 16 bit
	if( *end != '\0') return false; // must end after address
	if( addr ) *addr = (uint16_t)i;
	return true;
}


oper_t get_operand( char *str, void *data ) {
	if( str == NULL || *str == '\0') return NON;
	else if( get_register( str, data ) ) return REG;// R1 ... R3
	else if( get_immediate_value( str, data ) ) return VAL; // 0xABCD, -1234, XXX label?
	else if( get_storage_address( str, data ) ) return ADS; // (0x1000), (label)
	else if( get_register_address( str, data ) ) return ADR; // (R1) ... (R3)
	else if( get_immediate_address( str, data ) ) return ADB; //  $0x1000, $label
	else return OPER_ERROR;
}

/**
 * @brief Matches the given tokens to the instructions in the instruction list
 * 	      (ops) and returns the index (0+) if a match is found,
 * 	      otherwise -1 (not found).
 *
 * @param token the tokens to match the instructions against.
 * @return The index of the matching instruction in the instruction list ops
 * 	       or -1 if not found.
 */

int find_instruction( char *token[3] ) {
	// PRINT_INSTR(token, "\n");
	int i, num_ops = sizeof(ops)/sizeof(ops[0]);

	// for every instruction
	for(i = 0; i<num_ops; i++ ) {
		// printf( "0x%3x: %s ", ops[i].opcode, ops[i].mnemonic );
		// printf( "%s, %s -- ", get_oper_name(ops[i].oper1), get_oper_name(ops[i].oper2) );

		if( strcmp(token[0], ops[i].mnemonic) == 0) { // mnemonic match
			// printf("might match - ");
			if( get_operand(token[1], NULL) == ops[i].oper1 ) {
				// printf("oper1 does match - "); // oper1 match
				if( get_operand(token[2], NULL) == ops[i].oper2 ) {
					// printf("and oper2 does match!\n"); // oper2 match
					break;
				} else {
					// printf("but oper2 does NOT match\n");
					continue;
				}
			} else {
				// printf("oper1 does NOT match\n");
				continue;
			}
		} else {// NO mnemonic match
			// printf("\n");
		}
	} // end for every instruction

	if( i >= num_ops) { // instruction not found
		return -1;
	} else
		return i;
}

/**
 * @brief Assembles one already tokenized instruction and writes it to the
 * output buffer.
 *
 * Returns the number of bytes written if successful (always 4, since all
 * instructions currently use 4 bytes in machine code), otherwise 0 if an
 * error occurred.
 *
 * @param token Array of 3 tokens (instruction, operand 1 and 2)
 * @param out The output buffer to which the generated machine code is written.
 * @return The number of bytes written to out (4, or 0 on error).
 */
int assemble_instruction( char *token[3], uint8_t *out ) {

	int inst_no = find_instruction(token);
	if( inst_no < 0 ) {
		printf("Parse ERROR: instruction not found or operand mismatch: \'");
		PRINT_INSTR(token, "\'\n");
		return -1;
	}

	// opcode always goes first
	*(out+0) = ops[inst_no].opcode;
	*(out+1) = 0;
	*(out+2) = 0;
	*(out+3) = 0;

	// XXX replace spaghetti-code with something more elegant, please :)
	uint32_t data = 0;
	/*oper_t oper1 =*/ get_operand( token[1], &data );

	switch( ops[inst_no].tok1 ) {
	case xx: // ignore
		break;
	case b1:
		*(out+1) = (uint8_t)data;
		break;
	case b_3:
		*(out+2) = 0;
		*(out+3) = (uint8_t)data;
		break;
	case b23:
		*(out+2) = HI_BYTE(data);
		*(out+3) = LO_BYTE(data);
		break;
	default:
		printf("Internal ERROR in assembler at line %d in %s\n", __LINE__, __FILE__ );
		return -666; // internal error
	}

	/*oper_t opert2 =*/ get_operand( token[2], &data );
	switch( ops[inst_no].tok2 ) {
	case xx: // ignore
		break;
	case b1:
		*(out+1) = (uint8_t)data;
		break;
	case b_3:
		*(out+2) = 0;
		*(out+3) = (uint8_t)data;
		break;
	case b23:
		*(out+2) = HI_BYTE(data);
		*(out+3) = LO_BYTE(data);
		break;
	default:
		printf("Internal ERROR in assembler at line %d in %s\n", __LINE__, __FILE__ );
		return -666; // internal error
	}

	return 4;
}


//// assembles a string buffer with assembly language and writes the machine code to out.
//// returns the number of bytes written to out.
//size_t assemble_buffer( char *amsbuf, uint8_t *out ) {
//	size_t count = 0;
//	char delim[] = "\n";
//	char *next;
//	char *lines_save;
//
//	// break lines at the newline (\n) char - modifies the input!
//	next = strtok_r( amsbuf, delim, &lines_save );
//	while (next != NULL) {
//
//		printf( "Looking at   [%s]:\n", next );
//		next = remove_comments( next );
//		printf( "wo comments: [%s]:\n", next );
//		next = trim_whitespace( next );
//		printf( "trimmed:     [%s]:\n", next );
//
//		next = label_extract( next, count ); // any labels?
//		// next = skip_whitespace( next ); // label_extrac() already skips
//
//		char *token[3];
//		tokenize_line( next, token );
//		// printf( "  OP: %s\n", token[0] );
//		// printf( "  OPER1: %s\n", token[1] );
//		// printf( "  OPER2: %s\n", token[2] );
//
//		count += 4;
//		next = strtok_r( NULL, delim, &lines_save );
//	}
//	return count;
//}


/**
 * @brief Assembles a string buffer with ONE LINE of input and writes the
 * machine code to the output buffer out.
 *
 * Supposed to handle everything supported thrown at it (comments, labels, ...).
 *
 * @param line		one line of assembly code
 * @param out		the output buffer to write the assembled machine code to
 * @param mcode_cnt	the current machine-code position in the buffer
 * @returns 		If successful, the number of bytes written to out (0 or 4),
 * 					otherwise an negative number on error.
 */
int assemble_line( char *line, uint8_t *out, size_t mcode_cnt ) {
	char *next = line;

	next = remove_comments( next );
	next = trim_whitespace( next );
	next = label_extract( next, mcode_cnt );
	if( next == NULL ) {
		printf( "ERROR parsing label \"%s\"\n", line );
		return -4; // invalid syntax for label
	}
	if( *next == '\0' ) return 0; // empty input or label only

	char *token[3] = { NULL, NULL, NULL };

	// tokenize the line according to the syntax
	// only checks for syntax errors, not semantic
	int num = tokenize_line( next, token );
	if( num < 0 ) {
		printf( "ERROR tokenizing line \"%s\"\n", line );
		return num;
	}

	return assemble_instruction( token, out + mcode_cnt );
}

#define MACHINE_CODE_SIZE 1024 // XXX move to filehandling.c

// assemble the assembly language file (source_file) and produce the binary
// machine code file (output_file)
void assemble_file( const char *source_file, const char *output_file ) {
    FILE *src = fopen(source_file, "r");
    FILE *out = fopen(output_file, "wb");

    if ( !src || !out ) {
        perror("File error");
        exit(1);
    }

    char line[256];
    int line_no = 0;
    uint8_t mcode_buf[MACHINE_CODE_SIZE] = {0}; // machine code buffer
    size_t mcode_cnt = 0; // machine code counter (like PC but while producing)

    while( fgets(line, sizeof(line), src) ) { // read line by line
    	line_no++;
    	printf( "%3d: %s", line_no, line ); // output to terminal
    	int instr_size = assemble_line( line, mcode_buf, mcode_cnt ); // assemble to buffer

    	if(instr_size < 0) { // parse error
    		printf( "%s: ", source_file );
    		printf( "%d: error while parsing input file.\n", line_no );
            exit(1);
    	}

    	mcode_cnt += instr_size;
    	if( mcode_cnt >= MACHINE_CODE_SIZE) {
    		// XXX perform proper file io: write partial files, then continue
    		printf( "Internal ERROR: " );
    		printf( "generated machine code exceeds buffer size of %d.", MACHINE_CODE_SIZE );
            exit(1);
    	} // end if
    } // end while

    // buffer must contain multiples of 4 bytes if we haven't fubar
    assert( mcode_cnt % 4 == 0 );

    // print machine code buffer
    printf( "+-------+-----+-----+----------+\n");
    printf( "|line no| inst| op 1| op 2     |\n");
    printf( "+-------+-----+-----+----------+\n");
    for(int i=0; i<mcode_cnt; i+=4) {
    	printf( " 0x%04X:  ", i ); // address
    	printf( "0x%02X, 0x%02X, ", mcode_buf[i+0], mcode_buf[i+1] );
    	printf( "0x%02X 0x%02X.\n", mcode_buf[i+2], mcode_buf[i+3] );
    }

    // fwrite( machine_code, 4, code_pos, out );

    fclose(src);
    fclose(out);
}


/**
 * @brief Tokenizes an input line of assembly code in the form
 * "Mnemonic Operand1, Operand2". Does not handle labels!
 *
 * Takes a line of assembly code and cuts it into its pieces: one instruction
 *  (token 1) and 0...2 operands (tokens 2 and 3).
 *
 * The parameter token (array of 3 char pointers) is used as output parameter.
 * All 3 string pointers are initially set to NULL. After the function call,
 * the pointers in token point to the respective parts of the input line.
 *
 * Examples:
 * NOP 		 => token 0: "NOP", 1: NULL, 2: NULL
 * NOT R2	 => token 0: "NOT", 1: "R2", 2: NULL
 * LD R3, 10 => token 0: "LD",  1: "R3", 2: "10"
 *
 * @param line The input line of assembly code to tokenize.
 * @param token Pointer to an array of 3 string pointers. After the function
 *        return, they point to the tokens or NULL.
 * @return The number of tokens identified (0..3) or -1 if an error occurred.
 */
int tokenize_line( char *line, char *token[3] ) {

	token[0] = token[1] = token[2] = NULL;

	// remove comments, then trim input line
	// XXX: already done in assemble_line()?!
	line = trim_whitespace( remove_comments( line ) );
	if (line == NULL || *line == '\0')
		return 0; // nothing here!

	const char whitespace[] = " \t\v\f"; // whitespace (space, tab, vertical tab, form feed)
	const char comma[] = ","; // comma only
	char *save_ptr = NULL;

	// MNEMONIC: must end in whitespace or '\0'
	token[0] = strtok_r( line, whitespace, &save_ptr ); // (OP)  (_OPER1_comma_OPER2_)
	if (token[0] == NULL) { // no more tokens
		return -1; // INTERNAL ERROR: empty line despite check above?!
	}

	if( save_ptr == NULL ) {
		// there was NO whitespace in line
		// => the line is completely parsed
		// => there only was an instruction
		return 1;
	}

	// OPERAND 1: splitting "OPER1, OPER2" at comma
	line = strtok_r( NULL, comma, &save_ptr );
	if( line == NULL ) {
		// this means: there was no leftover from 1st call
		// this is the case when the line ends after the comma
		// => only instruction found
		// should have been caught at "if (save_ptr == NULL)"
		assert( false );
		return 1;
	}
	token[1] = line = trim_whitespace( line ); // OPER1

	if( save_ptr == NULL ) {
		// there was NO comma in line
		// => the line is completely parsed
		// => there was one mnemonic and one operand
		return 2;
	}

	// OPERAND 2: part after comma
	line = strtok_r( NULL, comma, &save_ptr );
	if (line == NULL) {
		// this means: no more tokens
		// => there was a comma after the 1st call
		// => but after that comma, there is nothing
		// => this is an error, because we expect oper 2 after the comma
		token[0] = token[1] = token[2] = NULL;
		return -1;
	}
	token[2] = line = trim_whitespace( line ); // OPER2

	if( save_ptr == NULL ) {
		// there was no 2ND comma in line (good!)
		// => the line is completely parsed
		// => there was one mnemonic and two operands
		return 3;
	} else {
		// the line war NOT completely parsed
		// i.e., there is more stuff after a second comma
		printf( "ERROR: input line contains an excess comma before: \"%s\"\n", save_ptr );
		token[0] = token[1] = token[2] = NULL;
	}
	return -1;
}



