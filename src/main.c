#include <config.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "permute.h"
#include "str_to_uint.h"

#define DEFAULT_PERMUTES 1

static unsigned int depth;

__attribute__((noreturn))
static void usage_and_exit(const char *name)
{
	fprintf(stderr,
			"%s %s\n"
			"Copyright (c) 2018 Andreas Bofjäll, see LICENSE for licensing details\n"
			"\n"
			"Usage:   %s DEPTH [FILE]\n"
			"Purpose: Permute input lines\n"
			"\n"
			"Arguments:\n"
			"         DEPTH      Permutation depth\n"
			"         [FILE]     Permute data in file. If file is omitted, permute\n"
			"                    standard input. Note stdin must be closed before\n"
			"                    %s outputs anything.\n"
			"\n"
			"Switches:\n"
			"         -h         This help text\n"
			"         -V         Display version information and exit\n",
			PACKAGE_NAME, PACKAGE_VERSION, name, name);
	exit(1);
}

static void parse_cmdline(int argc, char **argv, FILE **f)
{
	int opt;

	while ((opt = getopt(argc, argv, "hV")) != -1) {
		switch (opt) {
		case 'h':
			usage_and_exit(argv[0]);
		case 'V':
			printf("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
			exit(0);
		default:
			usage_and_exit(argv[0]);
		}
	}

	if (argc - optind < 1) {
		fprintf(stderr, "Depth argument required\n");
		usage_and_exit(argv[0]);
	} else if (argc - optind > 2) {
		fprintf(stderr, "Superfluous arguments: ");
		for (int i = argc - optind; i < argc; i++)
			fprintf(stderr, "%s ", argv[i]);
		fprintf(stderr, "\n");
		usage_and_exit(argv[0]);
	}

	if (str_to_uint(argv[optind], &depth)) {
		fprintf(stderr, "Invalid depth: %s\n", argv[optind]);
		usage_and_exit(argv[0]);
	}
	optind++;

	if (argc - optind > 0) {
		const char *name = argv[optind];
		FILE *_f = fopen(name, "r");
		if (!_f) {
			fprintf(stderr, "Could not open %s: %s\n",
					name, strerror(errno));
			exit(1);
		}
		*f = _f;
		optind++;
	} else {
		*f = stdin;
	}

}

#define DEFAULT_BUF_LEN 1024
int main(int argc, char **argv)
{
	FILE *f;
	ssize_t r;
	size_t buf_len = DEFAULT_BUF_LEN;
	char *buf = malloc(DEFAULT_BUF_LEN);
	if (!buf) {
		fprintf(stderr, "Not enough memory\n");
		return 1;
	}

	parse_cmdline(argc, argv, &f);

	permute_init();

	do {
		r = getline(&buf, &buf_len, f);
		if (r < 1)
			continue;

		if (buf[r - 1] != '\n') {
			/*
			 * This could happen if the file did not properly
			 * terminate the last line with a line feed. We
			 * ignore such lines.
			 */
			continue;
		}

		buf[r - 1] = '\0';
		if (permute_add(buf)) {
			fprintf(stderr, "Not enough memory\n");
			free(buf);
			fclose(f);
			return 1;
		}
	} while (r > 0);

	permute(depth);

	permute_free();
	free(buf);
	fclose(f);
	return 0;
}
