/*
 * asm.c
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */

#include "asm.h"

op_t ops[] = {
		// data-movement instructions
		// load
		{0x00, "LD R%d, %0xh" },
		{0x01, "LD R%d, R%d"},
		{0x02, "LD R%d, (%0xh)"},
		{0x03, "LD R%d, (R%d)" },
		// store
		{0x02, "LD R%d, (%0xh)"},
		{0x03, "LD R%d, (R%d)" },
};

