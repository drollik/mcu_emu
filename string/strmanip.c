/*
 * strmanip.c
 *
 *  Created on: 21 Dec 2024
 *      Author: olive
 */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "strmanip.h"

// returns true, if string str starts with the string pre, otherwise false.
bool starts_with( const char* str, const char *pre ) {
	if( str == NULL || pre == NULL) return false;
	return strncmp(pre, str, strlen(pre)) == 0;
}
// returns a pointer to the first non-whitespace character in s
// (effectively skipping the whitespace in front of the string s)
// does not modify the string passed in s.
char *skip_whitespace(const char* s) {
	if(s == NULL) return NULL;
	while( isspace(*s) ) s++;
	return (char*)s;
}

// returns a pointer to the first non-whitespace character in s after character ch
// or NULL if ch is not found within whitespace before the next non-whitespace
// (effectively skipping any whitespace and exactly one occurrence of ch).
// also returns NULL is ch is whitespace
// does not modify the string passed in s.
char *skip_char_and_whitespace(const char *s, char ch) {
	if( s == NULL || isspace(ch) ) return NULL;
	s = skip_whitespace(s); // skip any leading whitespace
	if( *s != ch ) return NULL; // ch is not the first char surrounded by whitespace
	return skip_whitespace(++s); // skip trailing whitespace
}

// returns a pointer to the string in s that has been trimmed of leading
// and trailing whitespace characters.
// modifies the string s by replacing the the first whitespace in the
// end of the string by '\0'
char *trim_whitespace( char *s ) {
	if( s == NULL ) return NULL;
	s = skip_whitespace(s); // skip leading whitespace characters in s
	char *p = s;
	while( *p != '\0' && !isspace(*p) ) p++; // skip any non-whitespace characters
	*p='\0'; // end string
	return s;
}

// iterates through the string, replacing the first u by x.
// modifies the input string!
// returns s, that is, the unmodified start of the input string
char *subst_char(char *s, char u, char x) {
	if( s == NULL) return NULL;
	char *p = s;
	while( *p != '\0' ) {
		if (*p == u ) {
			*p = x; // replace u by x
			break;
		}
		p++;
	}
	return s;
}

// iterates through the string, replacing the first ';' with '\0'
// modifies the input string!
// returns s, that is the unmodified start of the input string
char *remove_comments(char *s) {
	return subst_char(s, ';', '\0');
}
