/*
 * strnamip.h
 *
 *  Created on: 21 Dec 2024
 *      Author: olive
 */

#ifndef STRING_STRMANIP_H_
#define STRING_STRMANIP_H_

#include <stdbool.h>


// returns true, if string str starts with the string pre, otherwise false.
bool starts_with( const char* str, const char *pre );

// returns a pointer to the first non-whitespace character in s
// (effectively skipping the whitespace in front of the string s)
// does not modify the string passed in s.
char *skip_whitespace(const char* s);

// returns a pointer to the first non-whitespace character in s after character ch
// or NULL if ch is not found within whitespace before the next non-whitespace
// (effectively skipping any whitespace and exactly one occurrence of ch).
// does not modify the string passed in s.
char *skip_char_and_whitespace(const char *s, char ch);

// returns a pointer to the string in s that has been trimmed of leading
// and trailing whitespace characters.
// modifies the string s by replacing the the first whitespace in the
// end of the string by '\0'
char *trim_whitespace( char *s );

// iterates through the string, replacing the first u by x.
// modifies the input string!
// returns s, that is, the unmodified start of the input string
char *subst_char(char *s, char u, char x);

// XXX make general or move elsewhere
// replaces the first occurrence of ';' with '\0'
char *remove_comments(char *s);


#endif /* STRING_STRMANIP_H_ */
