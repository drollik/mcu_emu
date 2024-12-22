/*
 * endianness.h
 *
 *  Created on: 7 Dec 2024
 *      Author: olive
 */

#ifndef ENDIANNESS_H_
#define ENDIANNESS_H_

#include <stdint.h>

typedef union
{
  uint16_t u16;
  uint8_t  u8 [2];
} be_t;

// setting only the 'little' part of u16 to 1.
// if it shows up first (at .u8[0]), its little endian
// if it shows up last (at .u8[1]), its big endian
#define HOST_IS_BE ((be_t){.u16=0x0001}.u8[0] == 0)

#endif /* ENDIANNESS_H_ */
