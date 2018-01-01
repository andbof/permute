#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "permute.h"

#define NUM_TESTS 11

struct test {
	char *in[4];
	unsigned int level;
	char *out;
};

static struct test tests[] = {
	/* Test with normal data */
	{
		.in = {"hello", NULL},
		.level = 1,
		.out = "hello\n",
	},
	{
		.in = {"hello", NULL},
		.level = 2,
		.out = "hello\nhellohello\n",
	},
	{
		.in = {"hello", "world", NULL},
		.level = 1,
		.out = "hello\nworld\n",
	},
	{
		.in = {"hello", "world", NULL},
		.level = 2,
		.out = "hello\nhellohello\nhelloworld\nworld\nworldhello\nworldworld\n",
	},
	{
		.in = {"hello", "world", NULL},
		.level = 3,
		.out =
			"hello\nhellohello\nhellohellohello\nhellohelloworld\n"
			"helloworld\nhelloworldhello\nhelloworldworld\n"
			"world\nworldhello\nworldhellohello\nworldhelloworld\n"
			"worldworld\nworldworldhello\nworldworldworld\n"
	},
	{
		.in = {"hello", "world", "foo", NULL},
		.level = 1,
		.out = "hello\nworld\nfoo\n",
	},
	{
		.in = {"hello", "world", "foo", NULL},
		.level = 2,
		.out =
			"hello\nhellohello\nhelloworld\nhellofoo\nworld\nworldhello\n"
			"worldworld\nworldfoo\nfoo\nfoohello\nfooworld\nfoofoo\n"
	},
	{
		.in = {"hello", "world", "foo", NULL},
		.level = 3,
		.out =
			"hello\nhellohello\nhellohellohello\nhellohelloworld\n"
			"hellohellofoo\n"
			"helloworld\nhelloworldhello\nhelloworldworld\nhelloworldfoo\n"
			"hellofoo\nhellofoohello\nhellofooworld\nhellofoofoo\n"
			"world\nworldhello\nworldhellohello\nworldhelloworld\n"
			"worldhellofoo\n"
			"worldworld\nworldworldhello\nworldworldworld\nworldworldfoo\n"
			"worldfoo\nworldfoohello\nworldfooworld\nworldfoofoo\n"
			"foo\nfoohello\nfoohellohello\nfoohelloworld\n"
			"foohellofoo\n"
			"fooworld\nfooworldhello\nfooworldworld\nfooworldfoo\n"
			"foofoo\nfoofoohello\nfoofooworld\nfoofoofoo\n"
	},
	/* Test invalid inputs to ensure we don't crash */
	{
		.in = {NULL},
		.level = 0,
		.out = "",
	},
	{
		.in = {NULL},
		.level = 1,
		.out = "",
	},
	{
		.in = {"hello", NULL},
		.level = 0,
		.out = "",
	},
	/* Only add new tests above this line */
	{ {NULL}, 0, NULL }
};

static void assert_equals(int x, int y)
{
	if (x == y)
		return;

	printf("Assertion failed: %d != %d\n", x, y);
	exit(1);
}

static void assert_strequals(const char *x, const char *y)
{
	if (!strcmp(x, y))
		return;
	
	printf("Assertion failed: \"%s\" != \"%s\"\n", x, y);
	exit(1);
}

const char* permute_in_child(unsigned int level)
{
	/*
	 * The idea is to call permute() from a child process, where we've
	 * reset the standard stdout file descriptor to a pipe we've opened
	 * in the parent. This allows us to double-check the result later.
	 */

	/*
	 * This buffer needs to be big enough to catch the output from the
	 * largest set of permutations tested. We'll bail out if we try to
	 * write past the end of it.
	 */
	static char child_buf[1024];

	int fds[2];
	if (pipe(fds) < 0) {
		perror("pipe(): ");
		exit(1);
	}

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork(): ");
		exit(1);
	}
	if (!pid) {
		/* In child */
		close(fds[0]);
		if (dup2(fds[1], STDOUT_FILENO) < 0) {
			perror("dup2(): ");
			exit(1);
		}
		permute(level);
		permute_free();
		close(fds[1]);
		exit(0);
	}

	/* In parent */
	close(fds[1]);
	ssize_t r;
	size_t idx = 0;

	memset(child_buf, 0, sizeof(child_buf));
	do {
		r = read(fds[0], child_buf + idx, sizeof(child_buf) - idx);
		if (r > 0)
			idx += r;
		if (idx >= sizeof(child_buf)) {
			fprintf(stderr, "internal error: child_buf too small!\n");
			exit(1);
		}
	} while (r > 0);
	if (r < 0) {
		perror("read(): ");
		exit(1);
	}
	close(fds[0]);

	int wstatus;
	if (waitpid(pid, &wstatus, 0) < 0) {
		perror("waitpid(): ");
		exit(1);
	}
	if (!WIFEXITED(wstatus)) {
		fprintf(stderr, "permuting child did not exit cleanly\n");
		if (WIFSIGNALED(wstatus)) {
			fprintf(stderr, "it was interrupted by signal %d\n",
					WTERMSIG(wstatus));
		}
		exit(1);
	}
	if (WEXITSTATUS(wstatus)) {
		fprintf(stderr, "permuting child terminated with exit status %d\n",
				WEXITSTATUS(wstatus));
		exit(1);
	}

	permute_free();
	return child_buf;
}

int count_lines(const char *buf)
{
	int cnt = 0;

	for (const char *c = buf; *c; c++) {
		if (*c == '\n')
			cnt++;
	}

	return cnt;
}

int main(int argc, char **argv)
{
	unsigned int num_tests = 0;
	const char *out;

	for (struct test *test = tests; test->out; test++) {
		permute_init();
		size_t words = 0;
		while (test->in[words]) {
			if (permute_add(test->in[words])) {
				fprintf(stderr, "permute_add() failed\n");
				exit(1);
			}
			words++;
		}

		out = permute_in_child(test->level);
		assert_strequals(out, test->out);

#if 0
		/* Sanity check the number of permutations that should be in the output */
		int lines = count_lines(out);
		int perms = 0;
		for (unsigned int i = test->level; i; i--)
			perms += i * i;
		assert_equals(lines, perms);
#endif

		permute_free();
		num_tests++;
	}

	assert_equals(num_tests, NUM_TESTS);
}
