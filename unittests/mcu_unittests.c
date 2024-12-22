/*
 * unittests.c
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */
#include "mcu_unittests.h"

#include <stdio.h>
#include <assert.h>

#include "../mcu/mcu.h"
#include "../mcu/mem.h"

void run_mcu_unittests(void) {
	_test_LD_Rx_value();
	_test_LD_Rx_Ry();

	printf("DONE!\n");
}


void _test_LD_Rx_value() {

	mcu_t mcu; // new MCU
	mcu_reset( &mcu );

	mem_t mem; // memory
	mem_reset( &mem );

	size_t offset = CODE;
	op_state_t ret = OK;

	// R3 = 0xCAFE (negative value)
	ADD_INST2((mem.addr+offset), 0x01, 0x03, 0xCAFE); // LD R3, 0xCAFE
	ret = mcu_exec_op( &mcu, &mem );
	assert( ret == OK );
	assert( mcu.regs[3] == ASI16(0xCAFE));
	assert( mcu.Flags.N == 1);
	assert( mcu.Flags.Z == 0);
	assert( mcu.Flags.P == 0);
	offset+=4;

	// R3 = 0x7FFF (highest positive value)
	ADD_INST2((mem.addr+offset), 0x01, 0x03, 0x7FFF); // LD R3, 0x7FFF
	ret = mcu_exec_op( &mcu, &mem );
	assert( ret == OK );
	assert( mcu.regs[3] == ASI16(0x7FFF) );
	assert( mcu.Flags.N == 0);
	assert( mcu.Flags.Z == 0);
	assert( mcu.Flags.P == 1);
	offset+=4;

	// R3 = 0x0000 (zero)
	ADD_INST2((mem.addr+offset), 0x01, 0x03, 0x0000); // LD R3, 0x0000
	ret = mcu_exec_op( &mcu, &mem );
	assert( ret == OK );
	assert( mcu.regs[3] == ASI16(0x000) );
	assert( mcu.Flags.N == 0);
	assert( mcu.Flags.Z == 1);
	assert( mcu.Flags.P == 0);
	offset+=4;
}

void _test_LD_Rx_Ry() {

	mcu_t mcu; // new MCU
	mcu_reset( &mcu );

	mem_t mem; // memory
	mem_reset( &mem );

	size_t offset = CODE;
	op_state_t ret = OK;

	// R2 = (R3 = 0xCAFE) (negative value)
	ADD_INST2((mem.addr+offset), 0x01, 0x03, 0xCAFE); // LD R3, 0xCAFE
	offset+=4;
	ret = mcu_exec_op( &mcu, &mem );
	assert( mcu.regs[3] == ASI16(0xCAFE));

	ADD_INST2((mem.addr+offset), 0x02, 0x02, 0x03); // LD R2, R3
	offset+=4;
	ret = mcu_exec_op( &mcu, &mem );
	assert( ret == OK );
	assert( mcu.regs[2] == ASI16(0xCAFE));
	assert( mcu.Flags.N == 1);
	assert( mcu.Flags.Z == 0);
	assert( mcu.Flags.P == 0);
}

void _test_LD_Rx_addr() { //LD Rx, (addr)
}

//LD Rx, (Ry)
//ST Rx, (addr)
//ST Rx, (Ry)
//
//Data ops, process information
//ADD Rx, value
//ADD Rx, Ry
//SUB Rx, value
//SUB Rx, Ry
//NOT Rx, value
//???
//AND Rx, value
//AND Rx, Ry
//CMP Rx, value
//CMP Rx, Ry
//
//Control ops,  change the sequence of instructions to be executed
//BRA $addr
//BRN $addr
//BRZ $addr
//BRP $addr
//
//Misc ops
//NOP
//HALT
