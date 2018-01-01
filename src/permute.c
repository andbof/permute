#include <config.h>
#include <alloca.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

struct entry {
	const char *word;
	size_t len;
	STAILQ_ENTRY(entry) list;
};

static STAILQ_HEAD(wordshead, entry) words;
static ssize_t max_word_len = -1;
static char *out;

void permute_free(void)
{
	struct entry *e1, *e2;

	e1 = STAILQ_FIRST(&words);
	while (e1) {
		e2 = STAILQ_NEXT(e1, list);
		free((void*)e1->word);
		free(e1);
		e1 = e2;
	}
	STAILQ_INIT(&words);

	max_word_len = -1;
}

void permute_init(void)
{
	STAILQ_INIT(&words);
	max_word_len = 0;
}

int permute_add(const char *s)
{
	size_t l;
	struct entry *e = malloc(sizeof(*e));
	if (!e)
		return 1;

	e->word = strdup(s);
	if (!e->word) {
		free(e);
		return 1;
	}

	l = strlen(s);
	e->len = l;
	if (l > max_word_len)
		max_word_len = l;

	STAILQ_INSERT_TAIL(&words, e, list);

	return 0;
}

static void _permute(char *out, char *buf,
		unsigned int depth)
{
	struct entry *e;

	STAILQ_FOREACH(e, &words, list) {
		/*
		 * We know there is sufficient space in buf because
		 * the size is set in permute() and depends on the lengths
		 * of all strings in words.
		 */
		strcpy(buf, e->word);
		puts(out);
		if (depth > 1)
			_permute(out, buf + e->len, depth - 1);
	}
}

void permute(unsigned int depth)
{
	/* Make sure the caller has called permute_init() properly */
	if (max_word_len < 0)
		abort();

	/* +1 for trailing NULL */
	out = alloca((max_word_len * depth) + 1);

	if (depth < 1)
		return;

	return _permute(out, out, depth);
}
