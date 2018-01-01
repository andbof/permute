#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "str_to_uint.h"

#define NUM_TESTS 17

struct test {
	char *in;
	int r;
	int out;
};

static char str_int_max[20];
static char str_int_min[20];
static char str_uint_max[20];
static char str_uint_max_1[40];
static char str_longlong_max[40];
static char str_longlong_min[40];

static const struct test tests[] = {
	/* These tests should be successful */
	{
		.in = "1",
		.r = 0,
		.out = 1,
	},
	{
		.in = "16",
		.r = 0,
		.out = 16,
	},
	{
		.in = "0",
		.r = 0,
		.out = 0,
	},
	{
		.in = str_int_max,
		.r = 0,
		.out = INT_MAX,
	},
	{
		.in = str_uint_max,
		.r = 0,
		.out = UINT_MAX,
	},
	/* These tests should fail */
	{
		.in = "",
		.r = 1,
	},
	{
		.in = "a",
		.r = 1,
	},
	{
		.in = "0a",
		.r = 1,
	},
	{
		.in = "a0",
		.r = 1,
	},
	{
		.in = "0 a",
		.r = 1,
	},
	{
		.in = "a 0",
		.r = 1,
	},
	{
		.in = "-1",
		.r = 1,
	},
	{
		.in = str_int_min,
		.r = 1,
	},
	{
		.in = str_uint_max_1,
		.r = 1,
	},
	{
		.in = str_longlong_max,
		.r = 1,
	},
	{
		.in = str_longlong_min,
		.r = 1,
	},
	/* Only add new tests above this line */
	{ NULL, 0 }
};

static void assert_equals(int x, int y)
{
	if (x == y)
		return;

	printf("Assertion failed: %d != %d\n", x, y);
	exit(1);
}

static void init(void)
{
	snprintf(str_int_max, sizeof(str_int_max), "%d", INT_MAX);
	snprintf(str_int_min, sizeof(str_int_min), "%d", INT_MIN);
	snprintf(str_uint_max, sizeof(str_uint_max), "%u", UINT_MAX);
	snprintf(str_uint_max_1, sizeof(str_uint_max_1), "%llu", (unsigned long long)UINT_MAX + 1);
	snprintf(str_longlong_max, sizeof(str_longlong_max), "%lld", LLONG_MAX);
	snprintf(str_longlong_min, sizeof(str_longlong_min), "%lld", LLONG_MIN);
}

int main(int argc, char **argv)
{
	unsigned int u;
	int r;
	unsigned int num_tests = 0;

	init();

	for (const struct test *test = tests; test->in; test++) {
		printf("Testing '%s'\n", test->in);
		r = str_to_uint(test->in, &u);
		assert_equals(r, test->r);
		if (!r) {
			/* If the conversion fails, u will be uninitialized */
			assert_equals(u, test->out);
		}
		num_tests++;
	}

	printf("Testing NULL == 0\n");
	r = str_to_uint(NULL, &u);
	assert_equals(r, 1);
	num_tests++;

	assert_equals(num_tests, NUM_TESTS);
}
