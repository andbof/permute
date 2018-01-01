#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include "str_to_uint.h"

int str_to_uint(const char *s, unsigned int *out)
{
	long l;
	char *end;

	if (!s)
		return 1;

	errno = 0;
	l = strtol(s, &end, 10);
	if (errno)
		return 1;
	if (end == s)
		return 1;
	if (*end)
		return 1;

	if (l < 0 || l > UINT_MAX)
		return 1;

	*out = l;
	return 0;
}
