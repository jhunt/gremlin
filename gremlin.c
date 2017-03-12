#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <dlfcn.h>

static int DEBUG = 0;
static const char *GREMLINS = NULL;
static const char *GREMLIN;

static int
ok()
{
	if (!GREMLINS) return 1;
	if (DEBUG) fprintf(stderr, "gremlin: next gremlin is '%c'; ", *GREMLIN);
	int rc = *GREMLIN != '!';
	switch (*++GREMLIN) {
		case '\0': GREMLIN--;          break;
		case '^':  GREMLIN = GREMLINS; break;
	}
	if (DEBUG) {
		if (rc) fprintf(stderr, "allowing next *alloc() call through...\n");
		else    fprintf(stderr, "wreaking havoc on the next *alloc() call.  tehe!\n");
	}
	return rc;
}

static void* (*real_malloc)(size_t) = NULL;
static void* (*real_calloc)(size_t, size_t) = NULL;
static void* (*real_realloc)(void*, size_t) = NULL;

static void
setup()
{
	const char *env;

	real_malloc = dlsym(RTLD_NEXT, "malloc");
	if (!real_malloc) {
		fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
		abort();
	}

	real_calloc = dlsym(RTLD_NEXT, "calloc");
	if (!real_calloc) {
		fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
		abort();
	}

	real_realloc = dlsym(RTLD_NEXT, "realloc");
	if (!real_realloc) {
		fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
		abort();
	}

	env = getenv("GREMLIN_DEBUG");
	if (env && *env) DEBUG = 1;
	if (DEBUG) fprintf(stderr, "gremlin: debugging enabled via GREMLIN_DEBUG=%s\n", env);

	GREMLINS = getenv("GREMLINS");
	if (!GREMLINS || !*GREMLINS) GREMLINS = "!";
	GREMLIN = GREMLINS;
	if (DEBUG) fprintf(stderr, "gremlin: GREMLINS set to [%s]\n", GREMLINS);
	if (DEBUG) fprintf(stderr, "gremlin: loaded malloc() at %p\n", real_malloc);
	if (DEBUG) fprintf(stderr, "gremlin: loaded calloc() at %p\n", real_calloc);
	if (DEBUG) fprintf(stderr, "gremlin: loaded realloc() at %p\n", real_realloc);
	if (DEBUG) fprintf(stderr, "gremlin: setup complete\n");
}

void *
calloc(size_t count, size_t size)
{
	if (!real_calloc) setup();
	if (ok()) return real_calloc(count, size);
	errno = ENOMEM;
	if (DEBUG) fprintf(stderr, "gremlin: in calloc(count=%li, size=%li), forcing a NULL return\n", count, size);
	return NULL;
}

void *
malloc(size_t size)
{
	if (!real_malloc) setup();
	if (ok()) return real_malloc(size);
	errno = ENOMEM;
	if (DEBUG) fprintf(stderr, "gremlin: in malloc(size=%li), forcing a NULL return\n", size);
	return NULL;
}

void *
realloc(void *ptr, size_t size)
{
	if (!real_realloc) setup();
	if (ok()) return real_realloc(ptr, size);
	errno = ENOMEM;
	if (DEBUG) fprintf(stderr, "gremlin: in realloc(p=%p, size=%li), forcing a NULL return\n", ptr, size);
	return NULL;
}
