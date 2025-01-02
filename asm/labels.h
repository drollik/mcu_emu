/*
 * labels.h
 *
 *  Created on: 2 Jan 2025
 *      Author: olive
 */

#ifndef ASM_LABELS_H_
#define ASM_LABELS_H_

#include <stdbool.h>
#include <stdint.h>

// extracts a label from a given line and stores the corresponding addr with it.
// returns a pointer to the character after the ':', e.g.:
// given "  loop:NOP" the function will store the label and return a pointer to 'N'.
char *label_extract( char *s, int32_t addr );

void label_print(void);

int label_numelems(void);

int32_t label_get_address( const char *label );

// delete all stored labels
void label_delete_all(void);

bool label_is_valid_identifier( const char *str );

#endif /* ASM_LABELS_H_ */
