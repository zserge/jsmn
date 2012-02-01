#include <stdio.h>

#include "jsmn.c"

static int test_passed = 0;
static int test_failed = 0;

/* Terminate current test with error */
#define fail()	return __LINE__

/* Successfull end of the test case */
#define done() return 0

/* Check single condition */
#define check(cond) do { if (!(cond)) fail(); } while (0)

/* Test runner */
static void test(int (*func)(void), const char *name) {
	int r = func();
	if (r == 0) {
		test_passed++;
	} else {
		test_failed++;
		printf("FAILED: %s (at line %d)\n", name, r);
	}
}

#define TOKEN_EQ(t, tok_start, tok_end, tok_type) \
	((t).start == tok_start \
	 && (t).end == tok_end  \
	 && (t).type == (tok_type))

#define TOKEN_PRINT(t) \
	printf("start: %d, end: %d, type: %d\n", (t).start, (t).end, (t).type)



int test_primitive() {
	int r;
	jsmn_parser p;
	jsmntok_t tokens[10];

	jsmn_init_parser(&p, "{\"a\": 0}", tokens, 10);

	r = jsmn_parse(&p);
	check(r == JSMN_SUCCESS);
	check(TOKEN_EQ(tokens[0], 0, 8, JSMN_OBJECT));
	check(TOKEN_EQ(tokens[1], 2, 3, JSMN_STRING));
	check(TOKEN_EQ(tokens[2], 6, 7, JSMN_PRIMITIVE));

	return 0;
}

int main() {
	test(test_primitive, "test primitive values");
	return 0;
}
