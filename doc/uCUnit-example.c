/**
 * @file    sw10.c
 * @brief   Framework to test YOUR implementation of a stack ...
 */

#include "stack.h"
#include "uCUnit-v1.0.h"

static void CheckInitDeinit(void) {
	/* performs Init() and DeInit() tests */
	UCUNIT_TestcaseBegin("Stack: Init() and DeInit() Tests");
	/*--------------------------------------------------------*/
	/* Normal Init() and DeInit() check */
	UCUNIT_CheckIsEqual(Init(64), STACK_NO_ERROR); /* allocation of this size shall work */
	UCUNIT_CheckIsEqual(DeInit(), STACK_NO_ERROR); /* must work fine */

	/* double DeInit() check */
	UCUNIT_CheckIsEqual(Init(100), STACK_NO_ERROR); /* allocation of this size shall work */
	UCUNIT_CheckIsEqual(DeInit(), STACK_NO_ERROR); /* must work fine */
	UCUNIT_CheckIsEqual(DeInit(), STACK_NO_MEMORY); /* deallocation twice must be an error! */

	/* double Init check */
	UCUNIT_CheckIsEqual(Init(50), STACK_NO_ERROR); /* allocation of this size shall work */
	UCUNIT_CheckIsEqual(Init(10), STACK_REALLOC_ERROR); /* reallocating memory not allowed */
	UCUNIT_CheckIsEqual(DeInit(), STACK_NO_ERROR); /* must work fine */

	/* huge memory allocation check */
	UCUNIT_CheckIsEqual(Init(1000000), STACK_NO_MEMORY); /* far too much memory! */
	UCUNIT_CheckIsEqual(DeInit(), STACK_NO_MEMORY); /* deallocation twice must be an error! */
	/*--------------------------------------------------------*/
	UCUNIT_TestcaseEnd();
}

static void CheckBasics(void) {
	const int stack_size = 16;
	int i;
	StackUnit_t value;

	/* performs basic push/pop/peek tests */
	UCUNIT_TestcaseBegin("Stack: Basic Tests");
	/*--------------------------------------------------------*/
	UCUNIT_CheckIsEqual(Init(stack_size), STACK_NO_ERROR); /* Init Stack */

	/* push and peek test */
	for(i=0;i<stack_size;i++) {
		UCUNIT_CheckIsEqual(Push(i), STACK_NO_ERROR);
		UCUNIT_CheckIsEqual(Peek(&value), STACK_NO_ERROR);
		UCUNIT_CheckIsEqual(value, i);
	}

	/* pop and peek test */
	for(i=stack_size-1;i>=0;i--) {
		UCUNIT_CheckIsEqual(Peek(&value), STACK_NO_ERROR);
		UCUNIT_CheckIsEqual(value, i);
		UCUNIT_CheckIsEqual(Pop(&value), STACK_NO_ERROR);
		UCUNIT_CheckIsEqual(value, i);
	}

	UCUNIT_CheckIsEqual(DeInit(), STACK_NO_ERROR); /* DeInit Stack */
	/*--------------------------------------------------------*/
	UCUNIT_TestcaseEnd();
}

static void CheckUnderOverflow(void) {
	const int stack_size = 10;
	int i;
	StackUnit_t value;

	/* performs basic push/pop/peek tests */
	UCUNIT_TestcaseBegin("Stack: Underflow and Overflow Tests");
	/*--------------------------------------------------------*/
	UCUNIT_CheckIsEqual(Init(stack_size), STACK_NO_ERROR); /* Init Stack */

	/* no element on stack, must give underflow */
	UCUNIT_CheckIsEqual(Peek(&value), STACK_UNDERFLOW_ERROR);
	UCUNIT_CheckIsEqual(Pop(&value), STACK_UNDERFLOW_ERROR);

	/* push until stack is full */
	for(i=0;i<stack_size;i++) {
		UCUNIT_CheckIsEqual(Push(i), STACK_NO_ERROR);
	}
	/* must give overflow */
	UCUNIT_CheckIsEqual(Push(i), STACK_OVERFLOW_ERROR);

	/* pop the values from the stack */
	for(i=stack_size-1;i>=0;i--) {
		UCUNIT_CheckIsEqual(Pop(&value), STACK_NO_ERROR);
		UCUNIT_CheckIsEqual(value, i);
	}
	/* stack is empty, pop() must give underflow */
	UCUNIT_CheckIsEqual(Pop(&value), STACK_UNDERFLOW_ERROR);

	UCUNIT_CheckIsEqual(DeInit(), STACK_NO_ERROR); /* DeInit Stack */
	/*--------------------------------------------------------*/
	UCUNIT_TestcaseEnd();
}

static void CheckAdvanced(void) {
	const int stack_size = 300;
	int i;
	StackUnit_t value;

	/* performs basic push/pop/peek tests */
	UCUNIT_TestcaseBegin("Stack: Advanced :-)");
	/*--------------------------------------------------------*/
	UCUNIT_CheckIsEqual(Init(0), STACK_NO_ERROR); /* hmm, what about a stack with size zero? malloc(0) might return NULL or a pointer */
	UCUNIT_CheckIsEqual(Push(i), STACK_OVERFLOW_ERROR); /* does it properly handle this? */
	UCUNIT_CheckIsEqual(DeInit(), STACK_NO_ERROR); /* DeInit Stack */

	UCUNIT_CheckIsEqual(Init(stack_size), STACK_NO_ERROR); /* Init Stack */

	/* possible problem: what is the type of the stack index? how does it handle a larger stack? */
	/* push until stack is full */
	for(i=0;i<stack_size;i++) {
		UCUNIT_CheckIsEqual(Push(i), STACK_NO_ERROR);
		UCUNIT_CheckIsEqual(Peek(&value), STACK_NO_ERROR);
		UCUNIT_CheckIsEqual(value, i);
	}
	/* pop the values from the stack */
	for(i=stack_size-1;i>=0;i--) {
		UCUNIT_CheckIsEqual(Pop(&value), STACK_NO_ERROR);
		UCUNIT_CheckIsEqual(value, i);
	}

	UCUNIT_CheckIsEqual(DeInit(), STACK_NO_ERROR); /* DeInit Stack */
	/*--------------------------------------------------------*/
	UCUNIT_TestcaseEnd();
}

int main(void) {
	UCUNIT_Init(); /* initialize test framework */

	/* Call test cases */
	CheckInitDeinit();
	CheckBasics();
	CheckUnderOverflow();
	CheckAdvanced();

	UCUNIT_WriteSummary();
	UCUNIT_Shutdown();
	return 0;
}
