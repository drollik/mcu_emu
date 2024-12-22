/*
 * asm_unittests.c
 *
 *  Created on: 21 Dec 2024
 *      Author: olive
 */

#include <stdio.h>
#include <strings.h>

#include "../asm/asm.h"
#include "../string/strmanip.h"


#include "asm_unittests.h"

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

void run_asm_unittests(void) {

	char delim[]="\n";
	char *next;
	char *lines_save;

	// break lines at the newline (\n) char - modifies the input!
	next = strtok_r(lines, delim, &lines_save);
	while( next != NULL) {

		next = trim_whitespace(remove_comments(next));
		printf("Looking at [%s]:\n", next);

		char* token[3];
		tokenize_line(next, token);
		printf( "  OP: %s\n", token[0] );
		printf( "  OPER1: %s\n", token[1] );
		printf( "  OPER2: %s\n", token[2] );

		// find_op(next);
		next = strtok_r(NULL, delim, &lines_save);
	}
}
