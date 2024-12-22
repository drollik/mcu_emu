/*
 * mcu.c
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */

#include <stdio.h>
#include "mem.h"
#include "mcu.h"

void mcu_flags_set( mcu_t* mcu, int16_t value) {
	mcu->Flags.N = value < 0 ? 1: 0;
	mcu->Flags.Z = value == 0 ? 1: 0;
	mcu->Flags.P = value > 0 ? 1: 0;
}

void mcu_register_set(mcu_t* mcu, uint8_t dest_r, int16_t value) {
	mcu->regs[dest_r] = value;
	mcu_flags_set( mcu, value );
}

int16_t mcu_register_get(mcu_t* mcu, uint8_t dest_r ) {
	return mcu->regs[dest_r];
}

op_state_t mcu_exec_op( mcu_t* mcu, mem_t* mem ) {
	uint8_t opcode = mem->addr[mcu->PC++];
	uint8_t dest_r = mem->addr[mcu->PC++];
	uint8_t oper_h = mem->addr[mcu->PC++];
	uint8_t oper_l = mem->addr[mcu->PC++];
	int16_t value = ((uint16_t)oper_h<<8) | (uint16_t)oper_l; // 16 bit signed operand

	op_state_t ret = OK;

	switch( opcode ) {
	case 0x00: // NOP
		break;
		// MOVEMENT OPS
	case 0x01: // LD Rx, value
		mcu_register_set(mcu, dest_r, value);
		break;
	case 0x02: // LD Rx, Ry
		mcu_register_set(mcu, dest_r, mcu_register_get(mcu, oper_l));
		break;
	case 0x03: // LD Rx, (addr)
		{uint16_t addr = WORD(oper_h, oper_l);
		mcu_register_set(mcu, dest_r, mem_get_word(mem, addr));}
		break;
	case 0x04: // LD Rx, (Ry)
		{uint16_t addr =  mcu_register_get(mcu, oper_l);
		mcu_register_set(mcu, dest_r, mem_get_word(mem, addr));}
		break;
	case 0x10: // ST Rx, (addr)
		{uint16_t addr = WORD(oper_h, oper_l);
		mem_set_addr(mem, addr, dest_r);} // NB: dest_r is source
		break;
	case 0x11: // ST Rx, (Ry)
		{uint16_t addr = mcu_register_get(mcu, oper_l);
		mem_set_addr(mem, addr,  dest_r);} // NB: dest_r is source
		break;
		// ========
		// DATA OPS
		// ========
	case 0x20: // ADD Rx, value
		{int16_t val = mcu_register_get(mcu, dest_r); // Rx
		val = val + WORD(oper_h, oper_l); // Rx + value
		mcu_register_set(mcu, dest_r, val);} // Rx = Rx+value
		break;
	case 0x21: { // ADD Rx, Ry
		int16_t val_x = mcu_register_get(mcu, dest_r); // Rx
		int16_t val_y = mcu_register_get(mcu, oper_l); // Ry
		int16_t val = val_x + val_y; // Rx + Ry
		mcu_register_set(mcu, dest_r, val);} // Rx = Rx+Ry
		break;
	case 0x22: {// SUB Rx, value
		int16_t val = mcu_register_get(mcu, dest_r); // Rx
		val = val - WORD(oper_h, oper_l); // Rx + value
		mcu_register_set(mcu, dest_r, val);} // Rx = Rx+value
		break;
	case 0x23: {// SUB Rx, Ry
		int16_t val_x = mcu_register_get(mcu, dest_r); // Rx
		int16_t val_y = mcu_register_get(mcu, oper_l); // Ry
		int16_t val = val_x - val_y; // Rx + Ry
		mcu_register_set(mcu, dest_r, val);} // Rx = Rx+Ry
		break;
	case 0x24: {// NOT Rx
		int16_t val = !mcu_register_get(mcu, dest_r); // !Rx
		mcu_register_set(mcu, dest_r, val);} // Rx = Rx+Ry
		break;
	case 0x25: //
		printf("OPCODE 0x25 IS INVALID!");
		break;
	case 0x26: { // AND Rx, value
		int16_t val_x = mcu_register_get(mcu, dest_r); // Rx
		int16_t val_y = WORD(oper_h, oper_l); // value
		int16_t val = val_x & val_y; // Rx & Ry
		mcu_register_set(mcu, dest_r, val);} // Rx = Rx & Ry
		break;
	case 0x27: {// AND Rx, Ry
		int16_t val_x = mcu_register_get(mcu, dest_r); // Rx
		int16_t val_y = mcu_register_get(mcu, oper_l); // Ry
		int16_t val = val_x & val_y; // Rx & Ry
		mcu_register_set(mcu, dest_r, val);} // Rx = Rx & Ry
		break;
	case 0x28: { // CMP Rx, value
		int16_t val_x = mcu_register_get(mcu, dest_r); // Rx
		int16_t val_y = WORD(oper_h, oper_l); // value
		int16_t val = val_x - val_y;  // Rx & value
		mcu_flags_set( mcu, val );} // doesn't change any registers, only flags
		break;
	case 0x29: // CMP Rx, Ry
		break;
		// ===========
		// CONTROL OPS
		// ===========
	case 0x50: // BRA $addr - unconditional branch
		mcu->PC = WORD(oper_h, oper_l); // value
		break;
	case 0x51: // BRN $addr
		if(mcu->Flags.N)
			mcu->PC = WORD(oper_h, oper_l); // value
		break;
	case 0x52: // BRZ $addr
		if(mcu->Flags.Z)
			mcu->PC = WORD(oper_h, oper_l); // value
		break;
	case 0x53: // BRP $addr
		if(mcu->Flags.P)
			mcu->PC = WORD(oper_h, oper_l); // value
		break;
		// ===========
		// MISC OPS
		// ===========
	case 0xFF:
		ret = HALT;
		break;
	default:
		printf("ERROR: UNKNOWN OPCODE %hhx!\n", opcode);
		break;
	}
	return ret;
} // end mcu_exec_op()


// print out registers, flags, and current instruction
void mcu_state_dump( mcu_t* mcu, mem_t* mem  ) {
	printf("------------------------------------------------\n");
	printf("PC: 0x%04X  SP: 0x%04X\n", mcu->PC, mcu->SP);
	// registers
	printf("R0: 0x%04hX/ %6d       ", mcu->regs[0], mcu->regs[0]);
	printf("R1: 0x%04hX/ %6d\n", mcu->regs[1], mcu->regs[1]);
	printf("R2: 0x%04hX/ %6d       ", mcu->regs[2], mcu->regs[2]);
	printf("R3: 0x%04hX/ %6d\n", mcu->regs[3], mcu->regs[3]);
	printf("Flags:        ");
	printf("N: %d        ", mcu->Flags.N);
	printf("Z: %d         ", mcu->Flags.Z);
	printf("P: %d\n", mcu->Flags.P);
	// next instruction at PC in HEX
	printf("next OP: 0x%02hhX, ", mem->addr[mcu->PC+0]);
	uint8_t* ptr = (uint8_t*)mem->addr+mcu->PC;
	printf("0x%02hhX, ", *(ptr+1)); // register Rx
	printf("0x%02hhX, ", *(ptr+2)); // OPERAND hi
	printf("0x%02hhX\n", *(ptr+3)); // OPERAND lo
	// XXX disassembly
}

// reset the mcu
void mcu_reset( mcu_t* mcu ) {
	mcu->PC = CODE; // program counter starts at (0x0000 < 0xA000)
	mcu->SP = STACK; // stack hangs from the ceiling (0xFFFF)
	mcu->regs[0] = 0; // R0
	mcu->regs[1] = 0; // R1
	mcu->regs[2] = 0; // R2
	mcu->regs[3] = 0; // R3
	mcu->Flags.N = mcu->Flags.Z = mcu->Flags.P = 0x0; // reset flags
}




