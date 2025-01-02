/*
 * labels.c
 *
 *  Created on: 2 Jan 2025
 *      Author: oliver
 */

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <ctype.h> // isspace
#include <strings.h>

#include "../string/llist.h"
#include "../string/strmanip.h"
#include "labels.h"


static llnode_t *labels = NULL; // list of labels with their corresponding addresses

/**
 * @brief Extracts a label from an input line (if there is one) and stores it
 *        together with its address in llist.
 *
 * Note:
 * - All leading and trailing whitespace must have been removed of
 * the input line before invoking.
 * - label identifier follow C rules (1st char is alpha or '_', then alphnum)
 *
 *
 * @param line	A line of assembly language without leading and trailing whitespace.
 * @param addr	The address of next instruction, that is, the address the label refers to.
 * @return The remainder of the input line after all whitespace has been
 * 		   removed or NULL on error
 */

char* label_extract(char *line, int32_t addr) {
	if( line == NULL || *line == '\0') return line;
	assert( !isspace(*line) ); // expects no whitespace at *line

	char *next = NULL, *save_ptr = NULL;
	const char *colon = ":";
	next = strtok_r( line, colon, &save_ptr );

	if( save_ptr == NULL ) {
		// there was NO colon in line
		// => the line is completely parsed
		// => there is no label
		return line; // return unchanged
	}
	// next contains the label
	next = trim_whitespace( next );
	// check if identifier is valid
	if( !label_is_valid_identifier( next ) ) {
		return NULL; // identifier not valid
	}

	// printf( "label is: \"%s\"\n", next );
	labels = list_add_first( labels, next, addr );

	// return the string from after the ':' after having removed all whitespace
	return skip_whitespace(save_ptr);
}

//char* label_extract2(char *line, int32_t addr) {
//	if(*line == '\0') return line; // return if end of line
//	assert( !isspace(*line) ); // expects no whitespace at *line
//
//	// inspect a line if it contains a ':' => has a label
//	char *colon_pos = strchr( line, ':' );
//	if (colon_pos == NULL) {
//		// printf( "  contains no label!\n" );
//		return line; // no label, return unchanged
//	}
//	*colon_pos = '\0'; // replace colon with end of string
//
//	// remove whitespace between label and former ':'
//	line = trim_whitespace( line );
//	// check if identifier is valid
//	if( !label_is_valid_identifier( line ) ) {
//		return NULL; // identifier not valid
//	}
//
//	printf( "label is: \"%s\"\n", line );
//	labels = list_add_first( labels, line, addr );
//
//	// return the string from after the ':' after having removed all whitespace
//	return skip_whitespace(colon_pos + 1);
//}
/**
 * @brief Returns the address corresponding to the given label or -1 if the
 *        label is not defined.
 *
 * @param label to get the address of.
 * @return address of the label or -1 if the label is not defined.
 */
int32_t label_get_address( const char *label ) {
	return list_get_key( labels, label );
}

// delete all stored labels
void label_delete_all(void) {
	labels = list_delete_all( labels );
}

/**
 * @brief Prints out all labels found in the assembly so far.
 */
void label_print(void) {
	printf("Labels:\n");
	list_print( labels );
}

/**
 * @brief Returns the number of labels found in the assembly so far.
 * @return The the number of labels found in the assembly so far.
 */
int label_numelems(void) {
	return list_numelems( labels );
}


// Function to check if a string is a valid C identifier
bool label_is_valid_identifier( const char *str ) {
    // Empty string is not valid
    if(str == NULL || str[0] == '\0' ) {
        return false;
    }

    // Check if the first character is valid
    if( !isalpha( str[0] ) && str[0] != '_' ) {
        return false;
    }

    // Check remaining characters
    for( const char *ptr = str + 1; *ptr != '\0'; ++ptr) {
        if( !isalnum( *ptr ) && *ptr != '_' ) {
            return false;
        }
    }
    return true;
}
