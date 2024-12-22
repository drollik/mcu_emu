/*
 * mcu.h
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */

#ifndef MCU_H_
#define MCU_H_

/* 16-bit instructions
 *   - 1st byte: opcode
 *   - 2nd byte: destination register
 *   - 3rd+4th byte: source value/ address/ register
 *   - no operations directly in memory
 *
 *   - general format: OP DEST SRC
 */

#include <stdint.h>
#include "mem.h"

// MCU type
typedef struct mcu_s {

	// Registers
	uint16_t PC; // program counter, starts at 0x0000 upwards
	uint16_t SP; // stack pointer, starts at 0xFFFF downwards

	int16_t regs[4];

	// Flags
	struct {
		uint8_t N : 1; // negative (aka sign)
		uint8_t Z : 1; // zero
		uint8_t P : 1; // positive (= !N)
	} Flags;

	/*
	 The three single-bit registers are called N, Z, and P, corresponding to their
	 meaning: negative, zero, and positive. Each time a GPR is written by an operate
	 or a load instruction, the N, Z, and P one-bit registers are individually set to 0
	 or 1, corresponding to whether the result written to the GPR is negative, zero,
	 or positive. That is, if the result is negative, the N register is set, and Z and P
	 are cleared. If the result is zero, Z is set and N and P are cleared. If the result is
	 positive, P is set and N and Z are cleared.
	 */
} mcu_t;

typedef enum op_state_e {
	OK=0, HALT=0xFF, ERROR=-1
} op_state_t;

void mcu_flags_set( mcu_t* mcu, int16_t value);

void mcu_register_set(mcu_t* mcu, uint8_t dest_r, int16_t value);

int16_t mcu_register_get(mcu_t* mcu, uint8_t dest_r );

// execute the operation at the PC.
op_state_t mcu_exec_op( mcu_t* mcu, mem_t* mem );

// print out registers, flags, and current instruction
void mcu_state_dump( mcu_t* mcu, mem_t* mem  );

// reset the mcu
void mcu_reset( mcu_t* mcu );


#endif /* MCU_H_ */
