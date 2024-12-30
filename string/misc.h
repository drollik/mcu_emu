/*
 * misc.h
 *
 *  Created on: 29 Dec 2024
 *      Author: olive
 */

#ifndef STRING_MISC_H_
#define STRING_MISC_H_


// MAX and MIN, see https://stackoverflow.com/a/58532788
#define max(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

#endif /* STRING_MISC_H_ */
