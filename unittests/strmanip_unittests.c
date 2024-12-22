/*
 * strmanip_unittest.c
 *
 *  Created on: 21 Dec 2024
 *      Author: oliver
 */


#include <stdio.h>
#include <stdbool.h>
#include <strings.h>

#include "../string/strmanip.h"
#include "../asm/asm.h"
#include "../uCUnit/uCUnit-v1.0.h"

#include "strmanip_unittests.h"

static void _test_starts_with() {
	// bool starts_with( const char* str, const char *pre );
	UCUNIT_TestcaseBegin("start_with()");

	// expected, actual
	UCUNIT_CheckIsTrue( starts_with("ABCD","AB") ); // true
	UCUNIT_CheckIsTrue( starts_with("AB","AB") ); // true
	UCUNIT_CheckIsTrue( starts_with("AB","")); // true
	UCUNIT_CheckIsTrue( starts_with("","") ); // true

	UCUNIT_CheckIsFalse( starts_with("A","AB") ); // false
	UCUNIT_CheckIsFalse( starts_with("","AB") ); // false

	UCUNIT_CheckIsFalse( starts_with( NULL,"AB") ); // false
	UCUNIT_CheckIsFalse( starts_with( "ABC",NULL) ); // false
	UCUNIT_CheckIsFalse( starts_with( NULL,NULL) ); // false


//	UCUNIT_CheckIsNull(NULL);
//	UCUNIT_CheckIsNotNull(&errno);

	UCUNIT_TestcaseEnd();
}

static void _test_skip_whitespace() {
	// char *skip_whitespace(const char* s);
	UCUNIT_TestcaseBegin("skip_whitespace()");

	char *s = " \t\v\fA "; // space, tab, vertical tab, form feed
	UCUNIT_CheckIsEqual( s+4, skip_whitespace(s) );
	s = " A ";
	UCUNIT_CheckIsEqual( s+1, skip_whitespace(s) );
	s= "\tA ";
	UCUNIT_CheckIsEqual( s+1, skip_whitespace(s) );
	s="A ";
	UCUNIT_CheckIsEqual( s, skip_whitespace(s) );
	s=" ";
	UCUNIT_CheckIsEqual( s+1, skip_whitespace(s) );
	s="";
	UCUNIT_CheckIsEqual( s, skip_whitespace(s) );

	UCUNIT_CheckIsNull( skip_whitespace(NULL) );

	UCUNIT_TestcaseEnd();
}

static void _test_skip_char_and_whitespace() {
	// char *skip_char_and_whitespace(const char *s, char ch);
	UCUNIT_TestcaseBegin("skip_char_and_whitespace()");

	char *s = " A ";
	UCUNIT_CheckIsNull( skip_char_and_whitespace(s, ',') ); // no ',' => NULL
	s="";
	UCUNIT_CheckIsNull( skip_char_and_whitespace(s, ',') ); // no ',' => NULL
	s="XXX";
	UCUNIT_CheckIsNull( skip_char_and_whitespace(s, ' ') ); // ch is whitespace => NULL

	s = " , A ";
	UCUNIT_CheckIsEqual( s+3, skip_char_and_whitespace(s, ',') );
	s= ", A ";
	UCUNIT_CheckIsEqual( s+2, skip_char_and_whitespace(s, ',') );
	s=",A ";
	UCUNIT_CheckIsEqual( s+1, skip_char_and_whitespace(s, ',') );
	s=",";
	UCUNIT_CheckIsEqual( s+1, skip_char_and_whitespace(s, ',') );

	UCUNIT_TestcaseEnd();
}


static void _test_trim_whitespace() {
	// char *trim_whitespace( char *s );

	UCUNIT_TestcaseBegin("trim_whitespace()");

	char s1[] = " A ";
	char *p = trim_whitespace(s1);
	UCUNIT_CheckIsEqual( 'A', p[0] );
	UCUNIT_CheckIsEqual( '\0', p[1] );

	char s2[] = " A";
	p = trim_whitespace(s2);
	UCUNIT_CheckIsEqual( 'A', p[0] );
	UCUNIT_CheckIsEqual( '\0', p[1] );

	char s3[] = "  ";
	p = trim_whitespace(s3);
	UCUNIT_CheckIsEqual( '\0', p[0] );

	UCUNIT_CheckIsNull( trim_whitespace(NULL) );

	UCUNIT_TestcaseEnd();
}

static void _test_subst_char() {
	// char *subst_char(char *s, char u, char x);

	UCUNIT_TestcaseBegin("subst_char()");

	char s1[] = " A ";
	char *p = subst_char(s1, 'A', 'B'); // replace 1st A with B
	UCUNIT_CheckIsEqual( s1, p ); // point to same string
	UCUNIT_CheckIsEqual( ' ', p[0] );
	UCUNIT_CheckIsEqual( 'B', p[1] ); // A has been replaced
	UCUNIT_CheckIsEqual( ' ', p[2] );
	UCUNIT_CheckIsEqual( '\0', p[3] );

	char s2[] = " AA ";
	p = subst_char(s2, 'A','B'); // X not fount => no substitution
	UCUNIT_CheckIsEqual( s2, p ); // point to same string
	UCUNIT_CheckIsEqual( ' ', p[0] );
	UCUNIT_CheckIsEqual( 'B', p[1] ); // 1st A has been replaced
	UCUNIT_CheckIsEqual( 'A', p[2] ); // 2nd A has not been replaced
	UCUNIT_CheckIsEqual( ' ', p[3] );
	UCUNIT_CheckIsEqual( '\0', p[4] );

	char s3[] = " A ";
	p = subst_char(s3, 'X','B'); // X not found => no substitution
	UCUNIT_CheckIsEqual( s3, p ); // point to same string
	UCUNIT_CheckIsEqual( ' ', p[0] );
	UCUNIT_CheckIsEqual( 'A', p[1] ); // A has not been replaced
	UCUNIT_CheckIsEqual( ' ', p[2] );
	UCUNIT_CheckIsEqual( '\0', p[3] );

	UCUNIT_CheckIsNull( subst_char(NULL, 'X','Y') );

	UCUNIT_TestcaseEnd();
}

void run_strmanip_unittest(void) {
	UCUNIT_Init(); // initialize uCUnit test framework

	// all testcases here
	_test_starts_with();
	_test_skip_whitespace();
	_test_skip_char_and_whitespace();
	_test_trim_whitespace();
	_test_subst_char();

	UCUNIT_WriteSummary(); // uCUnit test summary
	UCUNIT_Shutdown();


}

