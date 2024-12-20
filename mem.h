/*
 * mem.h
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */

#ifndef MEM_H_
#define MEM_H_

#include "stdint.h"

// ======
// MACROS
// ======

// 16bit address bus=> 2^16 bytes = 65’536 bytes
#define MEM_END 0xFFFF // last address

#define STACK 0xFFFF //dangles 'downwards'
#define HEAP  0x1000
#define CODE  0x0000


// returns the high-byte of n
#define HI_BYTE(n) (((n) >>  8) & 0xFFu)

// returns the low-byte of n
#define LO_BYTE(n) ( (n) & 0xFFu )

// returns a word (uint16_t) from high- and low-bytes
#define WORD(hi,lo) (((hi)<<8) | (lo))

// returns the value of 'x' as uint16_t,
// e.g., 0xCAFE is -13570 (int16_t), not 51966(int32_t)
#define ASI16(x) ((int)((int16_t)(x)))

// ================
// TYPE DEFINITIONS
// ================
typedef struct mem_s {
	int8_t addr[MEM_END]; // 65635 bytes of memory (text&data&stack)
} mem_t;

// adds an instruction (op, reg, data) to memory at address addr.
// does not increase PC/ addr/ ...
#define ADD_INST2(addr, op, reg, data) \
	*(addr+0) = op; \
	*(addr+1) = reg; \
	*(addr+2) = HI_BYTE(data); \
	*(addr+3) = LO_BYTE(data)

// FUNCTIONS

// sets the value at memory address 'addr' to value
void mem_set_addr( mem_t* mem, uint16_t addr, int16_t value );

// returns value at memory addresses addr+0 and addr+1 as int16_t
int16_t mem_get_word(mem_t* mem, uint16_t addr );

// returns value at memory address 'addr' as int8_t
int8_t mem_get_byte(mem_t* mem, uint16_t addr );

// clear the entire memory to 0 (text, data, stack)
void mem_reset( mem_t* mem );


#endif /* MEM_H_ */
