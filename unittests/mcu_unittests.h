/*
 * unittests.h
 *
 *  Created on: 13 Dec 2024
 *      Author: olive
 */

#ifndef MCU_UNITTESTS_H_
#define MCU_UNITTESTS_H_

void run_mcu_unittests(void);

// UNIT TESTS, one per instruction
void _test_LD_Rx_value();
void _test_LD_Rx_Ry();
void _test_LD_Rx_addr();

#endif /* MCU_UNITTESTS_H_ */
