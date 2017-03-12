grem·lin /ˈɡremlən/
===================

> **noun** informal
>
> an imaginary mischievous sprite regarded as responsible for an
> unexplained problem or fault, especially a mechanical or
> electronic one.  _a gremlin in my computer omitted a line_

**gremlin** is a small bit of C code that compiles into a shared library that you can use to inject some mischief into your test suites.

It operates on a single environment variable, `$GREMLINS`, that specifies a timeline of sorts for how to muck with the memory allocation subsystem.

To see what **gremlin** is capable of, let's start out with a small test program that reverses the characters in the arguments it is passed:

```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char **ll;
	int i, j, l, n;

	n = argc - 1;
	ll = calloc(n, sizeof(char *));
	for (i = 0; i < n; i++) {
		l = strlen(argv[i+1]);

		ll[i] = malloc(l+1);
		ll[i][l] = '\0';
		for (j = 0; j < l; j++) {
			ll[i][j] = argv[i+1][l-j-1];
		}
	}

	for (i = 0; i < n; i++) {
		printf("[%d] '%s'\n", i, ll[i]);
		free(ll[i]);
	}
	free(ll);
	return 0;
}
```

Compile it if you want (you can also snag a copy from the repo, at
example.c).  Let's run it just to verify that it works:

```
→  ./example test radar racecar "a really long string for testing"
[0] 'tset'
[1] 'radar'
[2] 'racecar'
[3] 'gnitset rof gnirts gnol yllaer a'
```

Neat.  But if you want to know what happens if that first
`calloc()` fails (hint: it ain't pretty), you're going to need
**gremlin**.

```
→  LD_PRELOAD=$PWD/gremlin.so ./example test radar failure
Segmentation fault
```

(Note: this only works on Linux.  For very segfault-y values of
"works")

Out of the box, with no configuration, **gremlin** just causes any
of the `malloc` / `calloc` / `realloc` family of standard library
memory allocators to always fail.  When you want to start seeing
patterns of failure, you can set the `$GREMLINS` environment
variable to a string that represents the _timeline_ of failure.

For example:

```
# fail the first *alloc
GREMLINS='!.' LD_PRELOAD=$PWD/gremlin.so ./test-prog

# fail every subsequent *alloc, after the first
GREMLINS='.!' LD_PRELOAD=$PWD/gremlin.so ./test-prog

# fail every other *alloc
GREMLINS='.!^' LD_PRELOAD=$PWD/gremlin.so ./test-prog
```

The _language_ of `$GREMLINS` is quite simple.  Starting at the
beginning of the string, every character represents what to do
with the next \*alloc call.  `.` allows it to succeed; `!` causes
it to fail with `ENOMEM`.

If a `^` (carat) is found at the end of the `$GREMLINS` string,
the language _loops_, and the next \*alloc call gets handled based
on the first character again (with subsequent calls following the
original timeline).

Otherwise, if we just "fall off" the end of the string, the last
state is repeated _ad infinitum_.

If you don't specify a value for `$GREMLINS`, it defaults to
`"!"`, which of course means "fail now, fail forever after."

That's it.  That's all there is to it.


Use In Production
-----------------

Why would you _ever_ want to do that?  Don't.  Just stop.


Use In Testing
--------------

**gremlin** can be leveraged for testing to ensure that an
application handles failures in the memory allocation subsystem
gracefully.  Supposed, for a minute, that _gracefully_ means
freeing memory and exiting normally, with a non-zero exit code.
We can test this by starting with `$GREMLINS` set to `"!"` to
fail all \*alloc calls, and run the program to ensure that it
exits non-zero, normally (i.e. without segfaulting).  Next, make
the first \*alloc succeed, so that we can move past it, by
prepending a `.` to `$GREMLINS` (making it `".!"`) and run it
again.  Repeat.  Like, a lot of times.

At some point (assuming the program itself halts on every run),
you will have enough successes before the first failure that the
application should terminate normally, with an exit code of zero.

If you perform this iterative test under the purview of a code
coverage tool like `llvm-cov` or `gcov`, you can get a real feel
for what code paths may hide sneaky _only-when-malloc-fails_ bugs.


Building and Installation
-------------------------

`make` should do a pretty good job of building all the things.
You get to decide where you want to install `gremlins.so`, but I
would highly recommend keeping it somewhere safe, hidden, and away
from all sources of water.


Contributing
------------

This code is licensed MIT. Enjoy.

If you find a bug, please raise a [Github Issue][1] first,
before submitting a PR.

Happy Hacking!



[1]: https://github.com/jhunt/gremlin
