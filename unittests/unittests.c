/*
 * unittests.c
 *
 *  Created on: 21 Dec 2024
 *      Author: olive
 */

#include "unittests.h"
#include "strmanip_unittests.h"
#include "mcu_unittests.h"
#include "asm_unittests.h"

#include "../uCUnit/uCUnit-v1.0.h"

void run_all_unittests(void) {

    UCUNIT_Init(); // initialize uCUnit test framework
	UCUNIT_ResetTracepointCoverage(); // unused

	// run_mcu_unittests();

	// *** assembler ***
	// run_strmanip_unittest(); // used for parsing in assembler
	run_asm_unittests(); // XXX TODO: convert from output to actual UT

	UCUNIT_Shutdown();
}
