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

//	UCUNIT_Init(); // initialize uCUnit test framework

	run_strmanip_unittest();
	// run_asm_unittests();
	// run_mcu_unittests();

//	UCUNIT_WriteSummary(); // uCUnit test summary
//	UCUNIT_Shutdown();
}
