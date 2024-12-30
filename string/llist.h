/*
 * llist.h
 *
 *  Created on: 29 Dec 2024
 *      Author: olive
 */

#ifndef STRING_LLIST_H_
#define STRING_LLIST_H_

#include <stdint.h>

// definition of list node: it contains a key (string) and a value (int)
typedef struct llnode_s {
	char *key;
	int32_t value;
	struct llnode_s *next;
} llnode_t;

// adds a node to the beginning of the list
llnode_t* list_add_first(llnode_t *first, const char *key, int32_t value);

// returns the value corresponding to the given key
int32_t list_get_key(llnode_t *first, const char *key);

// print entire list
void list_print( llnode_t *first );

// returns number of elements in the list
int list_numelems( llnode_t *first );

#endif /* STRING_LLIST_H_ */
