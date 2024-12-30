/*
 * main.c
 *
 *  Created on: 6 Dec 2024
 *      Author: oliver.kasten@drollik.net
 *
 *  BIG endian:
 *  uint32 x = 0x0A0B0C0D
 *      higher memory
 *           ----->
 *  +----+----+----+----+
 *  |0x0A|0x0B|0x0C|0x0D| BIG endian
 *  +----+----+----+----+
 *  +----+----+----+----+
 *  |0x0D|0x0C|0x0B|0x0A| BIG endian
 *  +----+----+----+----+
 *
 *
 *  16-bit address bus
 *  16-bit instructions
 *    - 1st byte: opcode
 *    - 2nd byte: destination register
 *    - 3rd+4th byte: source value/ addr/ register
 *    - no operations directly in memory
 *  BIG-endian
 */


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "mcu/endianness.h"
#include "mcu/mem.h" // memory
#include "mcu/mcu.h" // mcu
#include "unittests/unittests.h" // mcu



int main(void) {
	//==========================================
	printf("Host is %s endian.\n", HOST_IS_BE?"BIG":"LITTLE");
	setbuf(stdout, NULL);

	run_all_unittests(); // run all unit tests

	return 0;

	mcu_t mcu; // MCU
	mcu_reset( &mcu );
	// mcu_state_dump( &mcu );

	mem_t mem; // memory
	mem_reset( &mem );

	// *** inline op-codes ***
//	size_t offset = CODE;
//
//	ADD_INST2((mem.addr+offset), 0x01, 0x03, 0xCAFE); // LD R3, 0xCAFE
//	offset+=4;
//	ADD_INST2((mem.addr+offset), 0x01, 0x02, 0x000A); // LD R2, 0xA
//	offset+=4;
//	ADD_INST2((mem.addr+offset), 0x02, 0x01, 0x0003); // LD R1, R3
//	offset+=4;
//	ADD_INST2((mem.addr+offset), 0xFF, 0x00, 0x0000); // HALT
//	offset+=4;

	// *** execute code ***

	op_state_t ret = OK;
	do {
		mcu_state_dump( &mcu, &mem );
		ret = mcu_exec_op( &mcu, &mem );
	} while(ret == OK);

	printf("Program terminated with code: %d", ret);

	return 0;
}
