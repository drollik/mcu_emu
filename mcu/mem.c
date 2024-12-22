/*
 * mem.c
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */
#include "mem.h"

void mem_set_addr( mem_t* mem, uint16_t addr, int16_t value ) {
	mem->addr[addr+0] = HI_BYTE(value); // we are BE!
	mem->addr[addr+1] = LO_BYTE(value);
}

// returns value at memory addresses addr+0 and addr+1 as int16_t
int16_t mem_get_word(mem_t* mem, uint16_t addr ) {
	return WORD(mem->addr[addr+0], mem->addr[addr+1]);
}

// returns value at memory address as int8_t
int8_t mem_get_byte(mem_t* mem, uint16_t addr ) {
	return mem->addr[addr+0];
}


void mem_reset( mem_t* mem ) {
	// TEXT section
	for(int i=0; i<=CODE; i++)
		mem->addr[i] = 0xFF; // HALT
	// HEAP/ DATA
	for(int i=CODE; i<=MEM_END; i++)
		mem->addr[i] = 0x00; // HALT
}



