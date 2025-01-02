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

/**
 * @brief Adds a node to the beginning of the list 'first' and returns a
 *        pointer to the first list node.
 *
 * Note: The key is copied into memory allocated by this function. It must
 * be freed when the node is deleted.
 *
 * @param first
 * @param key
 * @param value
 * @return
 */
llnode_t* list_add_first(llnode_t *first, const char *key, int32_t value) {
	assert( key != NULL );

	llnode_t *new_node = malloc( sizeof(llnode_t) );
	// CHECK for NULL-pointer
	assert( new_node != NULL );
	new_node->key = strdup( key ); // beware: no error checking here
	new_node->value = value;
	new_node->next = first;

	return new_node;
}

/**
 * @brief Delete the entire list identified by first
 *
 * @param first The lists first node.
 * @return NULL if successful
 */
llnode_t* list_delete_all( llnode_t *first ) {
	llnode_t *prev= NULL;
	llnode_t *cur = NULL;

	for (cur = first, prev = NULL; cur != NULL; prev = cur, cur = cur->next) {
		if( prev != NULL ) {
			free( prev ); // free previous node
			prev = NULL; // just for debugging
		}
		free( cur->key ); // free key of current node
	}
	if( prev != NULL ) {
		free( prev ); // free previous node
		prev = NULL; // just for debugging
	}
	return NULL;
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
