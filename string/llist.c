/*
 * llist.c
 *
 *  Created on: 29 Dec 2024
 *      Author: olive
 */
#include "../string/llist.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "../string/misc.h"

// adds a node to the beginning of the list; returns pointer to the first node
// of the list. the key is allocated and must be freed when the node is deleted.
llnode_t* list_add_first(llnode_t *first, const char *key, int32_t value) {
	assert( key != NULL );

	llnode_t *new_node = malloc( sizeof(llnode_t) );
	// CHECK for NULL-pointer
	assert( new_node != NULL );
	new_node->key = strdup( key );
	new_node->value = value;
	new_node->next = first;

	return new_node;
}

// returns the value corresponding to the given key. returns -1 if the key
// could not be found.
int32_t list_get_key(llnode_t *first, const char *key) {
	// llnode_t *cur;
	for (llnode_t *cur = first; cur != NULL; cur = cur->next) {
		if (strcmp( cur->key, key ) == 0)
			return cur->value; // found! return pointer to found element
	}
	return -1; // not found
}

// prints the list
void list_print( llnode_t *first ) {
	if( first == NULL) {
		printf("* empty *\n");
		return;
	}
	for (llnode_t *cur = first; cur != NULL; cur = cur->next) {
		printf("  %s:\t%d\n", cur->key, cur->value);
	}
}

int list_numelems( llnode_t *first ) {
	int count = 0;
	for (llnode_t *cur = first; cur != NULL; cur = cur->next) { count++; }
	return count;
}
